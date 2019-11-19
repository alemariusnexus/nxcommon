#ifndef INVALIDFORMATEXCEPTION_H_
#define INVALIDFORMATEXCEPTION_H_

#include "Exception.h"



class InvalidFormatException : public Exception
{
public:
	InvalidFormatException(const CString& message, const CString& srcFile = CString(), int srcLine = -1,
			Exception* nestedException = NULL)
			: Exception(message, srcFile, srcLine, nestedException, "InvalidFormatException") {}
};

#endif /* INVALIDFORMATEXCEPTION_H_ */
