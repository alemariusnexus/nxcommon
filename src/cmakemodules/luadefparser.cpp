#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

using namespace std;



enum State
{
	StateLeadingWhitespace,
	StateOutside,
	StateInsideParentheses,
	StateDone
};


const char* ParseDeclaration(const char* str, string& name, string& fullDecl)
{

	stack<State> stateStack;
	stateStack.push(StateLeadingWhitespace);

	stack<string> potentialNames;
	string potentialName;
	bool lastWasSpace = false;

	State state;
	while ((state = stateStack.top()) != StateDone) {
		char c = *str;

		if (state == StateLeadingWhitespace) {
			if (isspace(c)) {
				c = '\0';
			} else {
				stateStack.pop();
				stateStack.push(StateOutside);
			}
		} else if (state == StateOutside) {
			if (c == '(') {
				potentialNames.push(potentialName);
				potentialName.clear();
				stateStack.push(StateInsideParentheses);
			} else if (isspace(c)) {
				lastWasSpace = true;
			} else if (strchr(";{", c)) {
				c = '\0';
				stateStack.pop();
				stateStack.push(StateDone);
			} else {
				if (lastWasSpace) {
					potentialName.clear();
					lastWasSpace = false;
				}
				potentialName.append(&c, 1);
			}
		} else if (state == StateInsideParentheses) {
			if (c == ')') {
				stateStack.pop();
			} else if (c == '(') {
				stateStack.push(StateInsideParentheses);
			}
		}

		if (c != '\0') {
			fullDecl.append(&c, 1);
		}

		str++;
	}

	name = potentialNames.top();

	return str;
}


struct FileData
{
	string file;
	vector<string> defs;
};


int main(int argc, char** argv)
{
	const char* outFile = argv[1];

	vector<FileData> data;

	for (int i = 2 ; i < argc ; i++) {
		FileData fdata;
		fdata.file = argv[i];
		data.push_back(fdata);
	}

	for (size_t i = 0 ; i < data.size() ; i++) {
		FileData& fdata = data[i];
		string inFile = fdata.file;

		FILE* in = fopen(inFile.c_str(), "r");
		if (!in) {
			fprintf(stderr, "ERROR: Unable to open input file '%s'!", inFile.c_str());
			return 1;
		}

		unsigned char buffer[2048];
		size_t numRead;
		string inCode;

		while ((numRead = fread(buffer, 1, sizeof(buffer), in)) != 0) {
			inCode.append((const char*) buffer, numRead);
		}

		fclose(in);


		string::size_type pos = 0;

		while ((pos = inCode.find("LUASYS_EXPORT", pos)) != string::npos) {
			pos += strlen("LUASYS_EXPORT");
			const char* str = inCode.c_str() + pos;

			string name;
			string fullDecl;
			pos += ParseDeclaration(str, name, fullDecl) - str;

			fdata.defs.push_back(name);
		}
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

	fprintf(out, "#include <cstdint>\n");
	fprintf(out, "#include <cstdlib>\n");
	fprintf(out, "\n");

	for (size_t i = 0 ; i < data.size() ; i++) {
		string inFile = data[i].file;
		fprintf(out, "#include \"%s\"\n", inFile.c_str());
	}

	fprintf(out, "\n");
	fprintf(out, "void _LuasysForceKeepSymbolDummy()\n");
	fprintf(out, "{\n");
	fprintf(out, "    int r = rand() | 1;\n");

	for (size_t j = 0 ; j < data.size() ; j++) {
		FileData& fdata = data[j];

		fprintf(out, "\n");
		fprintf(out, "    // Exports from '%s'\n", fdata.file.c_str());

		for (size_t k = 0 ; k < fdata.defs.size() ; k++) {
			string def = fdata.defs[k];

			fprintf(out, "    { intptr_t _p = (intptr_t) ((void*) &%s); if (_p*r < 1) exit(1); }\n",
					def.c_str());
		}
	}

	fprintf(out, "}\n");
	fprintf(out, "\n");

	fflush(out);
	fclose(out);

	return 0;
}