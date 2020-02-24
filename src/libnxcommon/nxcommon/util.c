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

#include "util.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#ifdef _POSIX_VERSION
#include <unistd.h>
#include <sys/time.h>
#elif defined(_WIN32)
#include <windows.h>
#elif defined(ESP_PLATFORM)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif



#ifdef ESP_PLATFORM
static uint64_t (*_ESP32GetTickcountMicrosecondsImpl)() = NULL;

void SetESP32GetTickcountMicrosecondsImplementation(uint64_t (*impl)())
{
	_ESP32GetTickcountMicrosecondsImpl = impl;
}
#endif


uint64_t GetTickcount()
{
#ifdef _POSIX_VERSION
	return GetEpochTickcount();
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000 + tv.tv_usec/1000;
#elif defined(_WIN32)
	return GetTickCount();
#elif defined(ESP_PLATFORM)
	if (_ESP32GetTickcountMicrosecondsImpl) {
		return _ESP32GetTickcountMicrosecondsImpl() / 1000;
	} else {
		return xTaskGetTickCount() * portTICK_PERIOD_MS;
	}
#else
#error "GetTickcount() is not implemented for this platform!"
#endif
}


uint64_t GetTickcountMicroseconds()
{
#ifdef _POSIX_VERSION
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000000 + tv.tv_usec;
#elif defined(_WIN32)
	// TODO: Implement this correctly...
	return GetTickcount() * 1000;
#elif defined(ESP_PLATFORM)
	if (_ESP32GetTickcountMicrosecondsImpl) {
		return _ESP32GetTickcountMicrosecondsImpl();
	} else {
		return (xTaskGetTickCount() * portTICK_PERIOD_MS) * 1000;
	}
#else
#error "GetTickcountMicroseconds() is not implemented for this platform!"
#endif
}


#ifdef _POSIX_VERSION

// TODO: Implement this for Windows. It HAS TO BE the milliseconds since the epoch (as defined by POSIX), not
// any other kind of tickcount!

uint64_t GetEpochTickcount()
{
#ifdef _POSIX_VERSION
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000 + tv.tv_usec/1000;
#else
	return GetTickCount();
#endif
}

#endif


float RandomFloat(float min, float max)
{
	return (rand() / (float) RAND_MAX) * (max-min) + min;
}


bool RandomBool()
{
	return rand() <= RAND_MAX/2;
}


bool IsTimeBetween(int8_t timeH, int8_t timeM, int8_t startH, int8_t startM, int8_t endH, int8_t endM)
{
	int16_t timeAbsM = timeH*60 + timeM;
	int16_t startAbsM = startH*60 + startM;
	int16_t endAbsM = endH*60 + endM;

	if (endAbsM >= startAbsM) {
		return timeAbsM >= startAbsM  &&  timeAbsM < endAbsM;
	} else {
		return timeAbsM >= startAbsM  ||  timeAbsM < endAbsM;
	}
}


void SleepMilliseconds(unsigned int time)
{
#ifdef _POSIX_VERSION
	usleep(time * 1000);
#elif defined(_WIN32)
	Sleep(time);
#elif defined(ESP_PLATFORM)
	vTaskDelay(time / portTICK_PERIOD_MS + portTICK_PERIOD_MS);
#else
#error "SleepMilliseconds() is not implemented for this platform!"
#endif
}


