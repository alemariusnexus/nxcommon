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
#include <cstdlib>


int main(int argc, char** argv)
{
	printf("Run with --nxcommon-test-help for help on command line options!\n\n");

	testing::InitGoogleTest(&argc, argv);

	srand(time(NULL));

	CLIParser cli;

	int helpOpt = cli.addOption(0, "nxcommon-test-help", "Print help", false);
	int testdirPathOpt = cli.addOption(0, "testdir", "Path to the root test directory 'testdir'", true);
	int mysqlHostOpt = cli.addOption(0, "mysql-host", "Host of the MySQL server used for the SQL unit tests", true);
	int mysqlUserOpt = cli.addOption(0, "mysql-user", "User name of the MySQL server used for the SQL unit tests", true);
	int mysqlPassOpt = cli.addOption(0, "mysql-pass", "Password for the MySQL server used for the SQL unit tests", true);
	int mysqlDbOpt = cli.addOption(0, "mysql-db", "Database name on the MySQL server used for the SQL unit tests", true);
	int mysqlPortOpt = cli.addOption(0, "mysql-port", "Port of the MySQL server used for the SQL unit tests", true);

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
		} else if (opt == mysqlHostOpt) {
			mysqlHost = CString(arg);
		} else if (opt == mysqlUserOpt) {
			mysqlUser = CString(arg);
		} else if (opt == mysqlPassOpt) {
			mysqlPass = CString(arg);
		} else if (opt == mysqlDbOpt) {
			mysqlDb = CString(arg);
		} else if (opt == mysqlPortOpt) {
			mysqlPort = atoi(arg);
		}
	}

	if (testRootPath.isNull()) {
		fprintf(stderr, "WARNING: No test root directory given. Many tests will be disabled. Consider giving the "
				"path to the test root directory with the --testdir option!\n\n");
	}

	return RUN_ALL_TESTS();
}

