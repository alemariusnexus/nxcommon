#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>



typedef enum State
{
	StateLeadingWhitespace,
	StateOutside,
	StateInsideParentheses,
	StateDone
} State;



typedef struct
{
    char* str;
    size_t capacity;
    size_t length;
} String;


void StringInit(String* str)
{
    str->length = 0;
    str->capacity = 32;
    str->str = (char*) malloc(str->capacity);
    str->str[0] = '\0';
}

void StringFree(String* str)
{
    free(str->str);
    str->str = NULL;
}

void StringReserve(String* str, size_t resCap)
{
    if (str->capacity < resCap) {
        resCap += 2;
        str->capacity = resCap;
        str->str = (char*) realloc(str->str, resCap);
    }
}

void StringAppend(String* str, const char* app, size_t len)
{
    size_t newLen = str->length + len;
    StringReserve(str, newLen+1);
    memcpy(str->str+str->length, app, len);
    str->str[newLen] = '\0';
    str->length = newLen;
}

void StringClear(String* str)
{
    str->str[0] = '\0';
    str->length = 0;
}

void StringCopy(String* to, String* from)
{
    StringFree(to);
    
    to->capacity = from->capacity;
    to->length = from->length;
    to->str = (char*) malloc(from->capacity);
    memcpy(to->str, from->str, from->length+1);
}

int StringFind(String* str, const char* pattern, int offset)
{
    if (offset >= str->length) {
        return -1;
    }
    char* p = strstr(str->str+offset, pattern);
    return p ? (p-str->str) : -1;
}





typedef struct {
    State* stack;
    size_t capacity;
    size_t size;
} StateStack;


void StackInit(StateStack* stack)
{
    stack->capacity = 1;
    stack->stack = (State*) malloc(stack->capacity * sizeof(State));
    stack->size = 0;
}

void StackFree(StateStack* stack)
{
    free(stack->stack);
}

void StackReserve(StateStack* stack, size_t resCap)
{
    if (stack->capacity < resCap) {
        resCap += 2;
        stack->capacity = resCap;
        stack->stack = (State*) realloc(stack->stack, resCap * sizeof(State));
    }
}

void StackPush(StateStack* stack, State state)
{
    StackReserve(stack, stack->size+1);
    stack->stack[stack->size] = state;
    stack->size++;
}

State StackPop(StateStack* stack)
{
    State state = stack->stack[stack->size-1];
    stack->size--;
    return state;
}

State StackTop(StateStack* stack)
{
    return stack->stack[stack->size-1];
}





const char* ParseDeclaration(const char* str, String* name, String* fullDecl)
{
    StateStack stateStack;
    StackInit(&stateStack);
    StackPush(&stateStack, StateLeadingWhitespace);

	String potentialName;
	bool lastWasSpace = false;
    
    StringInit(&potentialName);

    int count = 0;
	State state;
    while ((state = StackTop(&stateStack)) != StateDone) {
		char c = *str;

		if (state == StateLeadingWhitespace) {
			if (isspace(c)) {
				c = '\0';
			} else {
                StackPop(&stateStack);
                StackPush(&stateStack, StateOutside);
			}
		} else if (state == StateOutside) {
			if (c == '(') {
                StringCopy(name, &potentialName);
                StringClear(&potentialName);
				StackPush(&stateStack, StateInsideParentheses);
			} else if (isspace(c)) {
				lastWasSpace = true;
			} else if (strchr(";{", c)) {
				c = '\0';
				StackPop(&stateStack);
				StackPush(&stateStack, StateDone);
			} else {
				if (lastWasSpace) {
                    StringClear(&potentialName);
					lastWasSpace = false;
				}
				StringAppend(&potentialName, &c, 1);
			}
		} else if (state == StateInsideParentheses) {
			if (c == ')') {
                StackPop(&stateStack);
			} else if (c == '(') {
                StackPush(&stateStack, StateInsideParentheses);
			}
		}

		if (c != '\0') {
            StringAppend(fullDecl, &c, 1);
		}

		str++;
        count++;
	}
	
	StackFree(&stateStack);
	
	StringFree(&potentialName);

	return str;
}



typedef struct Definition
{
    String def;
    struct Definition* next;
} Definition;


typedef struct FileData
{
	String file;
    Definition* def;
} FileData;


int main(int argc, char** argv)
{
	const char* outFile = argv[1];

    size_t numData = argc-2;
    FileData* data = (FileData*) malloc(numData * sizeof(FileData));

    size_t i;
	for (i = 2 ; i < argc ; i++) {
		FileData* fdata = data + i-2;
        StringInit(&fdata->file);
        StringAppend(&fdata->file, argv[i], strlen(argv[i]));
	}

	for (i = 0 ; i < numData ; i++) {
		FileData* fdata = data + i;
		String inFile = fdata->file;

		FILE* in = fopen(fdata->file.str, "r");
		if (!in) {
			fprintf(stderr, "ERROR: Unable to open input file '%s'!", fdata->file.str);
			return 1;
		}

		unsigned char buffer[2048];
		size_t numRead;
        String inCode;
        StringInit(&inCode);

		while ((numRead = fread(buffer, 1, sizeof(buffer), in)) != 0) {
            StringAppend(&inCode, (const char*) buffer, numRead);
		}

		fclose(in);


		int pos = 0;
        
        Definition* lastDef = NULL;

        while ((pos = StringFind(&inCode, "LUASYS_EXPORT", pos)) >= 0) {
			pos += strlen("LUASYS_EXPORT");
			const char* str = inCode.str + pos;

			String name;
			String fullDecl;
            
            StringInit(&name);
            StringInit(&fullDecl);
            
			pos += ParseDeclaration(str, &name, &fullDecl) - str;
            
            Definition* def = (Definition*) malloc(sizeof(Definition));
            def->next = NULL;
            StringInit(&def->def);
            StringCopy(&def->def, &name);
            
            if (lastDef) {
                lastDef->next = def;
            } else {
                fdata->def = def;
            }
            
            lastDef = def;
            
            StringFree(&name);
            StringFree(&fullDecl);
		}
		
		StringFree(&inCode);
	}



	FILE* out = fopen(outFile, "w");
	if (!out) {
		fprintf(stderr, "ERROR: Unable to open output file '%s'!", outFile);
		return 1;
	}

	//FILE* out = stdout;

	fprintf(out, "// Automatically generated by the nxcommon Lua definition parser\n");
	fprintf(out, "// DO NOT EDIT THIS FILE! CHANGES WILL BE LOST UPON RECOMPILATION!\n");
	fprintf(out, "\n");

    fprintf(out, "#ifdef __cplusplus\n");
    fprintf(out, "#include <cstdint>\n");
	fprintf(out, "#include <cstdlib>\n");
    fprintf(out, "#else\n");
	fprintf(out, "#include <stdint.h>\n");
	fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#endif\n");
	fprintf(out, "\n");

	for (i = 0 ; i < numData ; i++) {
		fprintf(out, "#include \"%s\"\n", data[i].file.str);
	}

	fprintf(out, "\n");
	fprintf(out, "void _LuasysForceKeepSymbolDummy()\n");
	fprintf(out, "{\n");
	fprintf(out, "    int r = rand() | 1;\n");

    size_t j;
	for (j = 0 ; j < numData ; j++) {
		FileData* fdata = data + j;

		fprintf(out, "\n");
		fprintf(out, "    // Exports from '%s'\n", fdata->file.str);

        Definition* def = fdata->def;
        while (def) {
			fprintf(out, "    { intptr_t _p = (intptr_t) ((void*) &%s); if (_p*r < 1) exit(1); }\n",
					def->def.str);
            
            Definition* next = def->next;
            StringFree(&def->def);
            free(def);
            def = next;
		}
	}

	fprintf(out, "}\n");
	fprintf(out, "\n");

	fflush(out);
	fclose(out);
    
    for (i = 0 ; i < numData ; i++) {
        StringFree(&data[i].file);
    }
    
    free(data);

	return 0;
}
