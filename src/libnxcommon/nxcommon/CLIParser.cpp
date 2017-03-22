/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of nxcommon.

	nxcommon is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	nxcommon is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with nxcommon.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "CLIParser.h"
#include <cstring>
#include <cstdio>
#include "strutil.h"
#include <string>

using std::string;



int CLIParser::addOption(char name, const char* longName, const char* description, bool takesArgument, const char* argName)
{
	Option opt;
	opt.name = name;
	opt.longName = longName;
	opt.description = description;
	opt.takesArgument = takesArgument;
	opt.argName = argName;
	options.push_back(opt);
	return options.size();
}


int CLIParser::parse(int& argc, char**& argv, char*& argument)
{
	if (argc <= 0) {
		return OptionEnd;
	}

	char*& arg = argv[0];
	int len = strlen(arg);

	if (len < 2) {
		// Too few characters for an option.
		argc--;
		argv++;
		argument = arg;
		return 0;
	} else {
		if (arg[0] == '-') {
			if (arg[1] == '-') {
				if (len == 2) {
					// Only '--' is not an option.
					argc--;
					argv++;
					argument = arg;
					return 0;
				} else {
					// Long option

					int numOpt = 1;
					vector<Option>::const_iterator it;
					for (it = options.begin() ; it != options.end() ; it++) {
						const Option& opt = *it;

						if (opt.longName) {
							if (strcmp(arg+2, opt.longName) == 0) {
								if (opt.takesArgument) {
									if (argc == 1) {
										// Error: Argument missing for option
										return OptionMissingArgument;
									} else {
										argument = argv[1];
										argc -= 2;
										argv += 2;
									}
								} else {
									argc--;
									argv++;
									argument = NULL;
								}

								return numOpt;
							}
						}

						numOpt++;
					}

					// Invalid argument
					return OptionInvalid;
				}
			} else {
				int numOpt = 1;
				vector<Option>::const_iterator it;
				for (it = options.begin() ; it != options.end() ; it++) {
					const Option& opt = *it;

					if (opt.name != 0) {
						if (opt.name == arg[1]) {
							if (opt.takesArgument) {
								if (len > 2) {
									// The argument is directly appended
									argument = arg+2;
									argc--;
									argv++;
								} else if (argc == 1) {
									// Error: Argument missing for option
									return OptionMissingArgument;
								} else {
									argument = argv[1];
									argc -= 2;
									argv += 2;
								}
							} else {
								if (len > 2) {
									// A group of short options without arguments. To support them, we remove
									// the first option character in the group and replace it by '-', then we
									// increment arg so that arg starts with just another short option.

									arg[0] = '\0';
									arg[1] = '-';
									arg++;
								} else {
									argc--;
									argv++;
								}

								argument = NULL;
							}

							return numOpt;
						}
					}

					numOpt++;
				}

				// Invalid option
				return OptionInvalid;
			}
		} else {
			argc--;
			argv++;
			argument = arg;
			return 0;
		}
	}
}


CLIParser& CLIParser::printOption(int option, CString& str)
{
	const Option& opt = options[option-1];

	char buf[4096];

	str.append(indentation);

	if (opt.name != 0) {

		sprintf(buf, "-%c", opt.name);
		str.append(buf);

		if (opt.takesArgument) {
			str.append(" ").append(opt.argName);
		}

		if (opt.longName) {
			sprintf(buf, ", --%s", opt.longName);
			str.append(buf);

			if (opt.takesArgument) {
				str.append(" ").append(opt.argName);
			}
		}
	} else {
		sprintf(buf, "--%s", opt.longName);
		str.append(buf);

		if (opt.takesArgument) {
			str.append(" ").append(opt.argName);
		}
	}
	str.append("\n");

	if (opt.description) {
		string description;
		description.reserve(strlen(opt.description));

		const char* srcDescr = opt.description;

		while (*srcDescr != '\0') {
			if (*srcDescr == '%') {
				if (srcDescr[1] != '\0') {
					switch (srcDescr[1]) {
					case 'a':
						description += opt.argName;
						break;
					case 's':
						description += opt.name;
						break;
					case 'l':
						description += opt.longName;
						break;
					case 'n':
						if (opt.name != 0) {
							description += '-';
							description += opt.name;
						} else {
							description += "--";
							description += opt.longName;
						}
						break;
					case '%':
						description += '%';
						break;
					default:
						srcDescr--;
					}
					srcDescr++;
				}
			} else {
				description += *srcDescr;
			}

			srcDescr++;
		}

		char* subIndentation = new char[strlen(indentation)+5];
		strcpy(subIndentation, indentation);
		strcat(subIndentation, "    ");

		char* wrapped = wrapText(description.c_str(), textWrapLength-strlen(subIndentation));
		char* indented = indent(wrapped, subIndentation);
		delete[] wrapped;

		char* tmp = new char[strlen(subIndentation) + strlen(indented) + 4];
		sprintf(tmp, "%s%s\n", subIndentation, indented);
		str.append(CString::from(tmp));

		delete[] indented;

		delete[] subIndentation;
	}

	return *this;
}


CLIParser& CLIParser::printOption(int option)
{
	CString str;

	printOption(option, str);

	printf("%s", str.get());

	return *this;
}


void CLIParser::printOptions(const char* spacing)
{
	for (unsigned int i = 0 ; i < options.size() ; i++) {
		if (i != 0) {
			printf("%s", spacing);
		}

		printOption(i+1);
	}
}


CString CLIParser::splitArgumentString(const char*& src, size_t& srcLen)
{
	CString arg;

	enum {
		OUT,
		OUTESC,
		SQ,
		DQ,
		DQESC,
		START,
		TERM
	};

	int state = START;

	const char* srcEnd = src+srcLen;

	while (src != srcEnd) {
		char c = *src;

		switch (state) {
		case START:
			if (!isspace(c)) {
				state = OUT;

				// Repeat reading this character in OUT state
				src--;
				srcLen++;
			}
			break;

		case OUT:
			if (c == '"') {
				state = DQ;
			} else if (c == '\'') {
				state = SQ;
			} else if (c == '\\') {
				state = OUTESC;
			} else if (isspace(c)) {
				state = TERM;
			} else {
				arg.append(c);
			}
			break;

		case OUTESC:
			arg.append(c);
			state = OUT;
			break;

		case SQ:
			if (c == '\'') {
				state = OUT;
			} else {
				arg.append(c);
			}
			break;

		case DQ:
			if (c == '"') {
				state = OUT;
			} else if (c == '\\') {
				state = DQESC;
			} else {
				arg.append(c);
			}
			break;

		case DQESC:
			arg.append(c);
			state = DQ;
			break;
		}

		if (state == TERM) {
			break;
		}

		src++;
		srcLen--;
	}

	return arg;
}



