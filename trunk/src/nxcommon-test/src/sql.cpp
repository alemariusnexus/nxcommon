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
#include <nxcommon/sql/sql.h>
#include <istream>
#include <nxcommon/CRC32.h>
#include <nxcommon/util.h>
#include <random>
#include <vector>
#include <initializer_list>

using std::istream;
using std::streamsize;
using std::vector;
using std::initializer_list;




struct TestRecord
{
	UString firstname;
	UString lastname;
	uint32_t age;
	CString imageFname;

	uint32_t imageCRC;
	uint32_t id;
};




void TestSQLResult(SQLPreparedStatement stmt, initializer_list<TestRecord> recs, bool inOrder = false)
{
	size_t numRecs = recs.size();

	bool* confirmed = new bool[numRecs];

	for (size_t i = 0 ; i < numRecs ; i++)
		confirmed[i] = false;

	size_t i = 0;
	while (stmt.nextRecord()) {
		uint32_t id = stmt.getUInt32(0);
		UString firstname = stmt.getString(1);
		UString lastname = stmt.getString(2);
		uint32_t age = stmt.getUInt32(3);
		ByteArray image = stmt.getBLOB(4);

		CRC32 crc;
		crc.append(image.get(), image.length());
		uint32_t cs = crc.getChecksum();

		size_t j = 0;
		for (const TestRecord& record : recs) {
			if (record.id == id) {
				if (inOrder) {
					EXPECT_EQ(i, j);
				}

				EXPECT_FALSE(confirmed[j]);

				EXPECT_EQ(record.firstname, firstname);
				EXPECT_EQ(record.lastname, lastname);
				EXPECT_EQ(record.age, age);
				EXPECT_EQ(record.imageCRC, cs);

				confirmed[j] = true;

				break;
			}

			j++;
		}

		i++;
	}

	for (size_t i = 0 ; i < numRecs ; i++)
		EXPECT_TRUE(confirmed[i]);
}


void TestSQLDatabase(SQLDatabase db)
{
	File sqlDir(File(testRootPath), "sql");

	SQLPreparedStatement stmt = db.createPreparedStatement(u"INSERT INTO person (firstname, lastname, age, image) VALUES (?, ?, ?, ?)");


	TestRecord insRecords[] = {
			{u"Erika",					u"Mustermann",					50,		"erikamustermann.jpg"},
			{u"Ludwig",					u"van Beethoven",				187,	"lvbeethoven.jpg"},
			{u"Carl Gustav",			u"Jung",						139,	"cgjung.jpg"},
			{u"Max",					u"Mustermann",					38,		"maxmustermann.jpg"},
			{u"Otto Eduard Leopold",	u"von Bismarck-Schönhausen",	199,	"ovbismarck.jpg"}
	};

	size_t numInsRecords = sizeof(insRecords) / sizeof(TestRecord);

	for (TestRecord& record : insRecords) {
		stmt.bindString(0, record.firstname);
		stmt.bindString(1, record.lastname);
		stmt.bindUInt32(2, record.age);

		File imageFile(sqlDir, record.imageFname);

		EXPECT_TRUE(imageFile.physicallyExists());

		if (!imageFile.physicallyExists())
			continue;

		CRC32 crc;
		File::filesize imgSize = imageFile.getSize();
		istream* stream = imageFile.openInputStream(istream::binary);

		char* imgData = new char[imgSize];
		size_t imgDataOffs = 0;

		char buf[4096];

		while (!stream->eof()) {
			stream->read(buf, sizeof(buf));
			streamsize sz = stream->gcount();

			crc.append(buf, sz);
			memcpy(imgData+imgDataOffs, buf, sz);
			imgDataOffs += sz;
		}

		delete stream;

		record.imageCRC = crc.getChecksum();

		EXPECT_EQ(imgSize, imgDataOffs);

		ByteArray barr = ByteArray::from(imgData, imgDataOffs);

		stmt.bindBLOB(3, barr);

		stmt.execute();

		EXPECT_EQ(1, stmt.getAffectedRowCount());
	}

	EXPECT_EQ(5, db.getLastInsertID());

	for (size_t i = 0 ; i < numInsRecords ; i++) {
		insRecords[i].id = i+1;
	}

	stmt.prepare(u"SELECT COUNT(*) FROM person");
	stmt.execute();

	EXPECT_TRUE(stmt.nextRecord());
	uint32_t numRecords = stmt.getUInt32(0);

	EXPECT_EQ(numInsRecords, numRecords);

	stmt.prepare(u"SELECT id, firstname, lastname, age, image FROM person WHERE age > ? ORDER BY age ASC");
	stmt.bindUInt32(0, 75);
	stmt.execute();

	TestSQLResult(stmt, {insRecords[2], insRecords[1], insRecords[4]}, true);

	stmt.bindUInt32(0, 150);
	stmt.execute();

	TestSQLResult(stmt, {insRecords[1]});
}



#ifdef NXCOMMON_SQLITE_ENABLED

TEST(SQLTest, TestSQLite)
{
	if (testRootPath.isNull())
		return;

	File testdir(testRootPath);

	if (!testdir.physicallyExists()  ||  !testdir.isDirectory())
		return;

	File sqlDir(testdir, "sql");

	if (!sqlDir.physicallyExists()  ||  !sqlDir.isDirectory())
		return;

	File sqlDbFile(sqlDir, "nxcommon-test.db");

	if (sqlDbFile.physicallyExists())
		sqlDbFile.remove();

	ASSERT_FALSE(sqlDbFile.physicallyExists());

	SQLDatabase db = SQLiteDriver::getInstance()->openDatabase(sqlDbFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

	ASSERT_TRUE(sqlDbFile.physicallyExists());

	db.sendQuery(u"DROP TABLE IF EXISTS person");

	db.sendQuery (
			u"CREATE TABLE person ("
			u"	id INTEGER PRIMARY KEY,"
			u"	firstname VARCHAR(256),"
			u"	lastname VARCHAR(256),"
			u"	age INTEGER,"
			u"  image BLOB"
			u");"
		);

	TestSQLDatabase(db);
}

#endif



#ifdef NXCOMMON_MYSQL_ENABLED

TEST(SQLTest, TestMySQL)
{
	if (mysqlHost.isNull())
		return;
	if (mysqlDb.isNull())
		return;

	SQLDatabase db;
	ASSERT_NO_THROW({
		db = MySQLDriver::getInstance()->openDatabase(mysqlHost, mysqlUser, mysqlPass, mysqlDb, mysqlPort);
	});

	db.sendQuery(u"DROP TABLE IF EXISTS person");

	db.sendQuery (
			u"CREATE TABLE person ("
			u"	id INTEGER AUTO_INCREMENT PRIMARY KEY,"
			u"	firstname VARCHAR(256),"
			u"	lastname VARCHAR(256),"
			u"	age INTEGER,"
			u"  image MEDIUMBLOB"
			u");"
		);

	TestSQLDatabase(db);
}

#endif
