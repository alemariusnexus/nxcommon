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

#include "global.h"
#include <nxcommon/CLIParser.h>


int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);

	CLIParser cli;

	int helpOpt = cli.addOption(0, "nxcommon-test-help", "Print help", false);
	int testdirPathOpt = cli.addOption(0, "testdir", "Path to the root test directory 'testdir'", true);

	argv++;
	argc--;

	int opt;
	char* arg;

	while ((opt = cli.parse(argc, argv, arg))  >=  0) {
		if (opt == helpOpt) {
			cli.printOptions();
			exit(0);
		} else if (opt == testdirPathOpt) {
			testRootPath = CString(arg);
		}
	}

	if (testRootPath.isNull()) {
		fprintf(stderr, "WARNING: No test root directory given. Most File tests will be disabled. Consider giving the "
				"path to the test root directory with the --testdir option!\n\n");
	}

	return RUN_ALL_TESTS();
}

