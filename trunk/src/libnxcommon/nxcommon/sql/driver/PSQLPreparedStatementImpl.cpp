/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "PSQLPreparedStatementImpl.h"
#include "psqlutils.h"
#include "../../util.h"
#include "sqlbase.h"
#include <ctime>



uint32_t PSQLPreparedStatementImpl::stmtUid = 0;





PSQLPreparedStatementImpl::PSQLPreparedStatementImpl(PSQLDatabaseImpl* db)
		: db(db), stmtName(makeUniqueStatementName()),
		  inValues(NULL), inValuePtrs(NULL), inValueLens(NULL), inValueFormats(NULL),
		  numInValues(0), sizeInValues(0)
{
	allocInValuesExactly(64);
}


PSQLPreparedStatementImpl::~PSQLPreparedStatementImpl()
{
	delete[] inValues;
	delete[] inValuePtrs;
	delete[] inValueLens;
	delete[] inValueFormats;
}


CString PSQLPreparedStatementImpl::makeUniqueStatementName() const
{
	char* name = new char[64];;
	sprintf(name, "nxc_%lu_%u", GetTickcount(), stmtUid++);
	return CString::from(name);
}


void PSQLPreparedStatementImpl::allocInValuesExactly(size_t size)
{
	if (size <= sizeInValues)
		return;

	InputValue* newInValues = new InputValue[size];
	const char** newInValuePtrs = new const char*[size];
	int* newInValueLens = new int[size];
	int* newInValueFormats = new int[size];

	for (int i = 0 ; i < numInValues ; i++) {
		newInValues[i].barr = inValues[i].barr;
		memcpy(newInValues[i].numBuf, inValues[i].numBuf, sizeof(newInValues[i].numBuf));

		if (inValuePtrs[i] == (const char*) &inValues[i].numBuf) {
			newInValuePtrs[i] = (const char*) &newInValues[i].numBuf;
		} else {
			newInValuePtrs[i] = inValuePtrs[i];
		}

		newInValueLens[i] = inValueLens[i];

		newInValueFormats[i] = inValueFormats[i];
	}

	delete[] inValues;
	delete[] inValuePtrs;
	delete[] inValueLens;
	delete[] inValueFormats;

	inValues = newInValues;
	inValuePtrs = newInValuePtrs;
	inValueLens = newInValueLens;
	inValueFormats = newInValueFormats;

	sizeInValues = size;
}


void PSQLPreparedStatementImpl::allocInValues(size_t size)
{
	if (size <= sizeInValues)
		return;

	allocInValuesExactly(GetNextPowerOfTwo(size));
}


void PSQLPreparedStatementImpl::bindUInt32(size_t index, uint32_t value)
{
	allocInValues(index+1);

	InputValue& val = inValues[index];
	sprintf(val.numBuf, "%u", value);
	inValuePtrs[index] = val.numBuf;
	inValueFormats[index] = 0;
}


void PSQLPreparedStatementImpl::bindInt32(size_t index, int32_t value)
{
	allocInValues(index+1);

	InputValue& val = inValues[index];
	sprintf(val.numBuf, "%d", value);
	inValuePtrs[index] = val.numBuf;
	inValueFormats[index] = 0;
}


void PSQLPreparedStatementImpl::bindUInt64(size_t index, uint64_t value)
{
	allocInValues(index+1);

	InputValue& val = inValues[index];
	sprintf(val.numBuf, "%lu", value);
	inValuePtrs[index] = val.numBuf;
	inValueFormats[index] = 0;
}


void PSQLPreparedStatementImpl::bindInt64(size_t index, int64_t value)
{
	allocInValues(index+1);

	InputValue& val = inValues[index];
	sprintf(val.numBuf, "%ld", value);
	inValuePtrs[index] = val.numBuf;
	inValueFormats[index] = 0;
}


void PSQLPreparedStatementImpl::bindFloat(size_t index, float value)
{
	allocInValues(index+1);

	InputValue& val = inValues[index];
	sprintf(val.numBuf, "%f", value);
	inValuePtrs[index] = val.numBuf;
	inValueFormats[index] = 0;
}


void PSQLPreparedStatementImpl::bindDouble(size_t index, double value)
{
	allocInValues(index+1);

	allocInValues(index+1);

	InputValue& val = inValues[index];
	sprintf(val.numBuf, "%f", value);
	inValuePtrs[index] = val.numBuf;
	inValueFormats[index] = 0;
}


void PSQLPreparedStatementImpl::bindString(size_t index, const UString& value)
{
	bindStringUTF8(index, value.toUTF8());
}


void PSQLPreparedStatementImpl::bindStringUTF8(size_t index, const ByteArray& value)
{
	allocInValues(index+1);

	InputValue& val = inValues[index];
	val.barr = value;
	inValuePtrs[index] = value.get();
	inValueLens[index] = value.length();
	inValueFormats[index] = 0;
}


void PSQLPreparedStatementImpl::bindBLOB(size_t index, const ByteArray& value)
{
	allocInValues(index+1);

	InputValue& val = inValues[index];
	val.barr = value;
	inValuePtrs[index] = value.get();
	inValueLens[index] = value.length();
	inValueFormats[index] = 1;
}


void PSQLPreparedStatementImpl::bindNull(size_t index)
{
	allocInValues(index+1);

	inValuePtrs[index] = NULL;
	inValueLens[index] = 0;
	inValueFormats[index] = 1;
}


void PSQLPreparedStatementImpl::bindBool(size_t index, bool value)
{
	allocInValues(index+1);

	InputValue& val = inValues[index];
	sprintf(val.numBuf, "%u", value ? 1 : 0);
	inValuePtrs[index] = val.numBuf;
	inValueFormats[index] = 0;
}


void PSQLPreparedStatementImpl::prepare(const UString& query)
{
	prepareUTF8(ConvertPlaceholdersToPSQL(query).toUTF8());
}


void PSQLPreparedStatementImpl::prepareUTF8(const ByteArray& query)
{
	ByteArray editedQuery = ConvertPlaceholdersToPSQLUTF8(query);

	PGconn* pq = db->getPGconn();

	if (!PQsendPrepare(pq, stmtName.get(), editedQuery.get(), 0, NULL)) {
		throw CreatePSQLConnException(pq, "Error preparing query", __FILE__, __LINE__);
	}

	timeval* to = db->getTimevalTimeout();

	FlushPSQL(pq, to);

	PGresult* res = FetchAsyncPSQLResult(pq, to);

	ExecStatusType status = PQresultStatus(res);

	if (status != PGRES_COMMAND_OK  &&  status != PGRES_TUPLES_OK  &&  status != PGRES_EMPTY_QUERY) {
		SQLException ex = CreatePSQLResultException(res, "Error preparing statement", __FILE__, __LINE__);
		PQclear(res);
		throw ex;
	}

	PQclear(res);

	if (!PQsendDescribePrepared(pq, stmtName.get())) {
		throw CreatePSQLConnException(pq, "Error fetching prepared statement description", __FILE__, __LINE__);
	}

	FlushPSQL(pq, to);

	res = FetchAsyncPSQLResult(pq, to);

	numInValues = PQnparams(res);

	PQclear(res);
}


SQLResultImpl* PSQLPreparedStatementImpl::execute()
{
	PGconn* pq = db->getPGconn();

	if (!PQsendQueryPrepared(pq, stmtName.get(), numInValues, inValuePtrs, inValueLens, inValueFormats, 0)) {
		throw CreatePSQLConnException(pq, "Error sending execution request for prepared statement", __FILE__, __LINE__);
	}

	timeval* to = db->getTimevalTimeout();

	FlushPSQL(pq, to);

	PGresult* curRes = FetchAsyncPSQLResult(pq, to);

	int status = PQresultStatus(curRes);
	if (status != PGRES_COMMAND_OK  &&  status != PGRES_TUPLES_OK  &&  status != PGRES_EMPTY_QUERY) {
		SQLException ex = CreatePSQLResultException(curRes, "Error executing prepared statement", __FILE__, __LINE__);
		PQclear(curRes);
		throw ex;
	}

	return new PSQLResultImpl(curRes);
}


void PSQLPreparedStatementImpl::reset()
{
}


void PSQLPreparedStatementImpl::finalize()
{
	char deallocStmt[256];
	sprintf(deallocStmt, "DEALLOCATE \"%s\"", stmtName.get());

	PGresult* res = db->sendQueryRaw(deallocStmt);

	int status = PQresultStatus(res);
	if (status != PGRES_COMMAND_OK) {
		SQLException ex = CreatePSQLResultException(res, "Error finalizing prepared statement (using DEALLOCATE)", __FILE__, __LINE__);
		PQclear(res);
		throw ex;
	}

	PQclear(res);
}



