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

#include "psqlutils.h"
#include <cstring>
#include "../SQLException.h"
#include "../TimeoutException.h"




SQLException CreatePSQLConnException(PGconn* conn, const char* msg, const char* srcFile, int srcLine)
{
	size_t msgLen = msg ? strlen(msg) : 0;
	const char* psqlErr = PQerrorMessage(conn);
	char* errmsg = new char[msgLen + strlen(psqlErr) + 4];
	if (msg)
		sprintf(errmsg, "%s: %s", msg, psqlErr);
	else
		sprintf(errmsg, "%s", psqlErr);
	SQLException ex(errmsg, srcFile, srcLine);
	delete[] errmsg;
	return ex;
}


SQLException CreatePSQLResultException(PGresult* res, const char* msg, const char* srcFile, int srcLine)
{
	size_t msgLen = msg ? strlen(msg) : 0;
	const char* psqlErr = PQresultErrorMessage(res);
	char* errmsg = new char[msgLen + strlen(psqlErr) + 4];
	if (msg)
		sprintf(errmsg, "%s: %s", msg, psqlErr);
	else
		sprintf(errmsg, "%s", psqlErr);
	SQLException ex(errmsg, srcFile, srcLine);
	delete[] errmsg;
	return ex;
}


void ThrowPSQLConnException(PGconn* conn, const char* msg, const char* srcFile, int srcLine)
{
	throw CreatePSQLConnException(conn, msg, srcFile, srcLine);
}


void ThrowPSQLResultException(PGresult* res, const char* msg, const char* srcFile, int srcLine)
{
	throw CreatePSQLResultException(res, msg, srcFile, srcLine);
}


void FlushPSQL(PGconn* pq, timeval* timeout)
{
	int sock = PQsocket(pq);

	fd_set masterFds;

	FD_ZERO(&masterFds);
	FD_SET(sock, &masterFds);

	int res;

	while ((res = PQflush(pq))  >  0) {
		fd_set fds = masterFds;
		res = select(sock+1, NULL, &fds, NULL, timeout);

		if (res < 0) {
			throw SQLException("Error calling select() in FlushPSQL()", __FILE__, __LINE__);
		} else if (res == 0) {
			throw TimeoutException("Timeout on select() in FlushPSQL()!", __FILE__, __LINE__);
		}
	}

	if (res < 0) {
		ThrowPSQLConnException(pq, "PQflush() returned a value < 0 in FlushPSQL()", __FILE__, __LINE__);
	}
}


PGresult* FetchAsyncPSQLResult(PGconn* pq, timeval* timeout)
{
	int sock = PQsocket(pq);

	fd_set masterFds;

	FD_ZERO(&masterFds);
	FD_SET(sock, &masterFds);

	int res;

	PGresult* lastRes = NULL;
	PGresult* curRes;

	do {
		do {
			fd_set fds = masterFds;
			res = select(sock+1, &fds, NULL, NULL, timeout);

			if (res < 0) {
				throw SQLException("Error calling select() in FetchAsyncPSQLResult()", __FILE__, __LINE__);
			} else if (res == 0) {
				throw TimeoutException("Timeout on select() in FetchAsyncPSQLResult()!", __FILE__, __LINE__);
			}

			if (!PQconsumeInput(pq)) {
				ThrowPSQLConnException(pq, "PQconsumeInput() returned an error in FetchAsyncPSQLResult()", __FILE__, __LINE__);
			}
		} while (PQisBusy(pq));

		do {
			curRes = PQgetResult(pq);

			if (!curRes) {
				// No more results!
			} else {
				if (lastRes) {
					PQclear(lastRes);
				}

				lastRes = curRes;
			}
		} while (curRes  &&  !PQisBusy(pq));
	} while (curRes);

	return lastRes;
}


// Just a few template parameter, nothing serious...
template <typename CharT,
		CharT nlChar, CharT slashChar, CharT astChar, CharT dashChar, CharT squoteChar, CharT dquoteChar,
		CharT qmChar, CharT dollarChar, CharT zeroChar>
CharT* _ConvertPlaceholdersToPSQL(const CharT* sin, size_t len, size_t& outOffs)
{
	// This function uses the property of all the common UTFs that at least the basic ASCII characters are actually
	// encoded as a single code point (1 byte for UTF-8, 2 bytes for UTF-16), and that their code point values and the
	// code point values of all other characters - including multi-byte sequences -  are disjoint. This means we can
	// basically treat UTF strings as one-unit-per-character ASCII strings with possibly bigger code units as long as
	// we only care about ASCII characters in our code.

	// In the worst case, we add an ASCII character after each ASCII character in the input, which might mean
	// twice the size in code units (as ASCII is encoded as a single code unit in all UTFs).
	// TODO: This is not correct for really nasty code, because the positional number might be more than one character
	// if you have many parameters.
	CharT* sout = new CharT[len*2 + 1];

	outOffs = 0;

	// TODO: Maybe support dollar-quoted strings

	bool inString = false;
	bool inQuotedIdentifier = false;
	//bool inDollarString = false;
	bool inSingleLineComment = false;
	unsigned int multiLineCommentDepth = 0;

	unsigned int paramPosCounter = 1;

	UChar c;

	for (size_t i = 0 ; i < len ; i++) {
		c = sin[i];

		if (inSingleLineComment) {
			if (c == nlChar) {
				inSingleLineComment = false;
			}
		} else if (multiLineCommentDepth != 0) {
			if (i+1 < len) {
				if (c == slashChar  &&  sin[i+1] == astChar) {
					multiLineCommentDepth++;
					i++;
					sout[outOffs++] = slashChar;
				} else if (c == astChar  &&  sin[i+1] == slashChar) {
					multiLineCommentDepth--;
					i++;
					sout[outOffs++] = astChar;
				}
			}
		} else if (inQuotedIdentifier) {
			if (c == dquoteChar) {
				inQuotedIdentifier = false;
			}
		} else if (inString) {
			if (c == squoteChar) {
				inString = false;
			}
		} else {
			if (c == squoteChar) {
				inString = true;
			} else if (c == dquoteChar) {
				inQuotedIdentifier = true;
			} else if (c == dashChar  &&  i+1 < len  &&  sin[i+1] == dashChar) {
				i++;
				sout[outOffs++] = dashChar;
				inSingleLineComment = true;
			} else if (c == slashChar  &&  i+1 < len  &&  sin[i+1] == astChar) {
				i++;
				sout[outOffs++] = slashChar;
				multiLineCommentDepth++;
			} else if (c == qmChar) {
				sout[outOffs++] = dollarChar;

				unsigned int val = paramPosCounter;
				while (val != 0) {
					sout[outOffs++] = val % 10 + zeroChar;
					val /= 10;
				}

				paramPosCounter++;
				continue;
			}
		}

		// i might have changed!
		c = sin[i];

		sout[outOffs++] = c;
		//outQuery.append(c);
	}

	sout[outOffs++] = '\0';

	return sout;
}


#ifdef NXCOMMON_UNICODE_ENABLED
UString ConvertPlaceholdersToPSQL(const UString& query)
{
	size_t len;
	UChar* str = _ConvertPlaceholdersToPSQL<UChar, u'\n', u'/', u'*', u'-', u'\'', u'"', u'?', u'$', u'0'> (
			query.get(), query.length(), len);
	return UString::from(str, len);
}
#endif


CString ConvertPlaceholdersToPSQLUTF8(const CString& query)
{
	size_t len;
	char* str = _ConvertPlaceholdersToPSQL<char, '\n', '/', '*', '-', '\'', '"', '?', '$', '0'> (
			query.get(), query.length(), len);
	return CString::from(str, len);
}
