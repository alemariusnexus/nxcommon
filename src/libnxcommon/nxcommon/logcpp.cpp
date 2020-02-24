#include "logcpp.h"
#include "stream/IOException.h"



void OpenLogFile(const File& logfile)
{
	FILE* file = fopen(logfile.toString().get(), "a");
	if (!file) {
		throw IOException("Error opening log file", __FILE__, __LINE__);
	}
	OpenLogFile(file);
}
