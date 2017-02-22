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

#ifndef SQLDATABASEIMPL_H_
#define SQLDATABASEIMPL_H_

#include <nxcommon/config.h>
#include "../../CString.h"
#include "../../ByteArray.h"
#include "sqlbase.h"
#include "SQLPreparedStatementImpl.h"
#include "SQLResultImpl.h"
#include <set>

#ifdef NXCOMMON_UNICODE_ENABLED
#include "../../UString.h"
#endif

using std::set;




class SQLDatabaseImpl
{
public:
	SQLDatabaseImpl();
	virtual ~SQLDatabaseImpl() {}
	virtual SQLPreparedStatementImpl* createPreparedStatement() = 0;
#ifdef NXCOMMON_UNICODE_ENABLED
	virtual SQLResultImpl* sendQuery(const UString& query) = 0;
#endif
	virtual SQLResultImpl* sendQueryUTF8(const CString& query) = 0;
	virtual uint64_t getLastInsertID() const = 0;
#ifdef NXCOMMON_UNICODE_ENABLED
	virtual UString escapeString(const UString& str) const = 0;
#endif
	virtual CString escapeStringUTF8(const CString& str) const = 0;
	virtual void setTimeout(uint64_t timeoutMillis) {}

	virtual uint64_t getCapabilities() const;
	virtual bool hasCapability(uint64_t cap) const;

protected:
	virtual void setCapabilities(uint64_t caps);

private:
	uint64_t caps;
};

#endif /* SQLDATABASEIMPL_H_ */
