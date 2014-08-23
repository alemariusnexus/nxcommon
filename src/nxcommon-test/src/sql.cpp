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





void TestSQLResult(SQLResult res, initializer_list<TestRecord> recs, bool inOrder = false)
{
	size_t numRecs = recs.size();

	bool* confirmed = new bool[numRecs];

	for (size_t i = 0 ; i < numRecs ; i++)
		confirmed[i] = false;

	size_t i = 0;
	while (res.nextRecord()) {
		uint32_t id = res.getUInt32(0);
		UString firstname = res.getString(1);
		UString lastname = res.getString(2);
		uint32_t age = res.getUInt32(3);
		ByteArray image = res.getBLOB(4);

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


void TestSQLTypes(SQLDatabase db)
{
	SQLPreparedStatement stmt = db.createPreparedStatement (
			u"INSERT INTO typetest1 (id, strval, intval, floatval, boolval) VALUES (?, ?, ?, ?, ?)");

	stmt.bindUInt32(0, 1);
	stmt.bindString(1, u"Planet Y");
	stmt.bindInt32(2, -1337);
	stmt.bindFloat(3, 3.14159f);
	stmt.bindBool(4, true);
	stmt.execute();

	stmt.bindInt32(0, 2);
	stmt.bindInt64(1, 0xCAFEBADDEADBEEFL); // Int -> String conversion
	stmt.bindString(2, u"30071993"); // String -> Int conversion
	stmt.bindInt32(3, 1235813); // Int -> Float conversion
	stmt.bindUInt32(4, 1); // Int -> Bool conversion
	stmt.execute();


	stmt.prepare(u"SELECT id, strval, intval, floatval, boolval FROM typetest1 ORDER BY id ASC");

	SQLResult res = stmt.execute();


	bool success = false;

	// Record #1

	EXPECT_TRUE(res.nextRecord());

	EXPECT_EQ(1, res.getUInt32(0, &success)); EXPECT_TRUE(success);
	EXPECT_EQ(UString(u"Planet Y"), res.getString(1, &success)); EXPECT_TRUE(success);
	EXPECT_EQ(-1337, res.getInt64(2, &success));  EXPECT_TRUE(success);
	EXPECT_FLOAT_EQ(3.14159f, res.getFloat(3, &success));  EXPECT_TRUE(success);
	EXPECT_TRUE(res.getBool(4, &success));  EXPECT_TRUE(success);

	// Record #2

	EXPECT_TRUE(res.nextRecord());

	EXPECT_EQ(UString(u"2"), res.getString(0, &success)); EXPECT_TRUE(success);
	EXPECT_EQ(0xCAFEBADDEADBEEFL, res.getUInt64(1, &success)); EXPECT_TRUE(success);
	EXPECT_DOUBLE_EQ(30071993.0, res.getDouble(2, &success)); EXPECT_TRUE(success);
	EXPECT_TRUE(res.getBool(4, &success)); EXPECT_TRUE(success);

	CString fstr = res.getStringUTF8(3, &success);
	EXPECT_TRUE(success);
	EXPECT_TRUE(CString(fstr).rtrim('0').rtrim('.') == CString(u8"1235813")) << "SQL getStringUTF8() from float value"
			<< " test failed. Got the following string: " << fstr;
}


void TestSQLDatabase(SQLDatabase db)
{
	File sqlDir(File(testRootPath), "sql");

	TestRecord insRecords[] = {
			{u"Erika",					u"Mustermann",					50,		"erikamustermann.jpg"},
			{u"Ludwig",					u"van Beethoven",				187,	"lvbeethoven.jpg"},
			{u"Carl Gustav",			u"Jung",						139,	"cgjung.jpg"},
			{u"Max",					u"Mustermann",					38,		"maxmustermann.jpg"},
			{u"Otto Eduard Leopold",	u"von Bismarck-Schönhausen",	199,	"ovbismarck.jpg"}
	};

	size_t numInsRecords = sizeof(insRecords) / sizeof(TestRecord);

	SQLPreparedStatement stmt = db.createPreparedStatement(u"INSERT INTO person (firstname, lastname, age, image) VALUES (?, ?, ?, ?)");

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

		SQLResult res = stmt.execute();

		EXPECT_EQ(1, res.getAffectedRowCount());
	}

	if (db.hasCapability(SQL_CAP_LAST_INSERT_ID)) {
		EXPECT_EQ(5, db.getLastInsertID());
	}

	for (size_t i = 0 ; i < numInsRecords ; i++) {
		insRecords[i].id = i+1;
	}

	stmt.prepare(u"SELECT COUNT(*) FROM person");
	SQLResult res = stmt.execute();

	EXPECT_TRUE(res.nextRecord());
	uint32_t numRecords = res.getUInt32(0);

	EXPECT_EQ(numInsRecords, numRecords);

	stmt.prepare(u"SELECT id, firstname, lastname, age, image FROM person WHERE age > ? ORDER BY age ASC");
	stmt.bindUInt32(0, 75);
	res = stmt.execute();

	EXPECT_EQ(5, res.getColumnCount());

	EXPECT_EQ(3, res.getColumnIndex("age"));
	EXPECT_EQ(1, res.getColumnIndex("FirSTnAmE"));
	EXPECT_EQ(-1, res.getColumnIndex("foobar"));

	EXPECT_EQ(CString("image"), res.getColumnName(4));
	EXPECT_EQ(CString("firstname"), res.getColumnName(1));

	TestSQLResult(res, {insRecords[2], insRecords[1], insRecords[4]}, true);

	stmt.bindUInt32(0, 150);
	res = stmt.execute();

	TestSQLResult(res, {insRecords[1]});

	res = db.sendQuery(u"SELECT * FROM person ORDER BY age ASC LIMIT 1");

	EXPECT_NE(-1, res.getColumnIndex("age"));
	EXPECT_NE(-1, res.getColumnIndex("ID"));
	EXPECT_NE(-1, res.getColumnIndex("lasTName"));
	EXPECT_EQ(-1, res.getColumnIndex("foobar"));

	res = db.sendQuery (
			u"INSERT INTO person (firstname, lastname) VALUES ('Adam', 'Ahrendahl'), ('Berta', 'Booglebay');"
			u"SELECT firstname, lastname FROM person ORDER BY firstname ASC, lastname ASC;"
			u"INSERT INTO person (firstname, lastname) VALUES ('Caesar', 'Csihar'), ('Derek', 'Dunckenbury'), ('Emily', 'End');"
		);

	EXPECT_EQ(3, res.getAffectedRowCount());
	EXPECT_FALSE(res.nextRecord());

	TestSQLTypes(db);
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

	EXPECT_TRUE(db.hasCapability(SQL_CAP_LAST_INSERT_ID));

	db.sendQuery (
			u"DROP TABLE IF EXISTS person;"
			u"DROP TABLE IF EXISTS typetest1;"

			u"CREATE TABLE person ("
			u"	id INTEGER PRIMARY KEY,"
			u"	firstname VARCHAR(256),"
			u"	lastname VARCHAR(256),"
			u"	age INTEGER,"
			u"  image BLOB"
			u");"

			u"CREATE TABLE typetest1 ("
			u"	id INTEGER PRIMARY KEY,"
			u"	strval VARCHAR(256),"
			u"	intval BIGINT,"
			u"	floatval DOUBLE,"
			u"	boolval BOOLEAN"
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

	EXPECT_TRUE(db.hasCapability(SQL_CAP_LAST_INSERT_ID));

	//SQLDatabase db = mysqlDb;

	db.sendQuery (
			u"DROP TABLE IF EXISTS person;"
			u"DROP TABLE IF EXISTS typetest1;"

			u"CREATE TABLE person ("
			u"	id INTEGER AUTO_INCREMENT PRIMARY KEY,"
			u"	firstname VARCHAR(256),"
			u"	lastname VARCHAR(256),"
			u"	age INTEGER,"
			u"  image MEDIUMBLOB"
			u");"

			u"CREATE TABLE typetest1 ("
			u"	id INTEGER AUTO_INCREMENT PRIMARY KEY,"
			u"	strval VARCHAR(256),"
			u"	intval BIGINT,"
			u"	floatval FLOAT,"
			u"	boolval BOOLEAN"
			u");"
		);

	TestSQLDatabase(db);
}

#endif



#ifdef NXCOMMON_PSQL_ENABLED

TEST(SQLTest, TestPSQL)
{
	if (psqlHost.isNull())
		return;
	if (psqlDb.isNull())
		return;

	SQLDatabase db;
	ASSERT_NO_THROW({
		db = PSQLDriver::getInstance()->openDatabase(psqlHost, psqlUser, psqlPass, psqlDb, psqlPort);
	});

	db.sendQuery (
			u"DROP TABLE IF EXISTS person;"
			u"DROP TABLE IF EXISTS typetest1;"
			u"DROP TABLE IF EXISTS pqtypetest;"

			u"CREATE TABLE person ("
			u"	id SERIAL PRIMARY KEY,"
			u"	firstname VARCHAR(256),"
			u"	lastname VARCHAR(256),"
			u"	age INTEGER,"
			u"  image BYTEA"
			u");"

			u"CREATE TABLE typetest1 ("
			u"	id SERIAL PRIMARY KEY,"
			u"	strval VARCHAR(256),"
			u"	intval BIGINT,"
			u"	floatval FLOAT8,"
			u"	boolval BOOLEAN"
			u");"

			u"CREATE TABLE pqtypetest ("
			u"	id SERIAL PRIMARY KEY,"
			u"	pointval POINT"
			u");"
		);

	TestSQLDatabase(db);

	db.sendQuery(u"INSERT INTO pqtypetest (id, pointval) VALUES (1, '(1.234, 5.678)');");

	SQLPreparedStatement stmt = db.createPreparedStatement(u"INSERT INTO pqtypetest (id, pointval) VALUES (?, ?)");
	stmt.bindUInt32(0, 2);
	stmt.bindString(1, u"3.14159, 987.6543");
	stmt.execute();

	SQLResult res = db.sendQuery(u"SELECT pointval FROM pqtypetest ORDER BY id ASC;");

	EXPECT_TRUE(res.nextRecord());
	EXPECT_EQ(UString(u"(1.234,5.678)"), res.getString(0));

	EXPECT_TRUE(res.nextRecord());
	EXPECT_EQ(UString(u"(3.14159,987.6543)"), res.getString(0));
}

#endif
