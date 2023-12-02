/* 
 * Copyright (C) 2015 Sony Mobile Communications Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names
 *    of its contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <time.h>
#include <sys/time.h>

#define LOG_TAG "TimeKeep"

#include <errno.h>
#include "hilog/log.h"

#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif

#undef LOG_TAG
#undef LOG_DOMAIN
#define LOG_TAG "TimeKeep"
#define LOG_DOMAIN 0xD001C04

#ifndef DISPLAY_LOGD
#define DISPLAY_LOGD(format, ...)                                                                                     \
    do {                                                                                                              \
        HILOG_INFO(LOG_CORE, "[%{public}s:%{public}d] " format "\n", __FUNCTION__, __LINE__, \
            ##__VA_ARGS__);                                                                                           \
    } while (0)
#endif

#define RTC_SYS_FILE "/sys/class/rtc/rtc0/since_epoch"
#define RTC_ATS_FILE "/data/vendor/time/ats_2"

int read_epoch(unsigned long long* epoch) {
	int res = 0;

	int fd = open(RTC_SYS_FILE, O_RDONLY);
	if (fd == -1) {
		res = -errno;
		DISPLAY_LOGD("Failed to open RTC sys path");
        printf("Failed to open RTC sys path\n");
	} else {
		char buffer[16];
		memset(buffer, 0, 16);
		res = read(fd, buffer, 16);
		if (res > 0) {
			char *endp = NULL;
			*epoch = strtoull(buffer, &endp, 10);
			// sysfs read returns newline, ok to end up at '\n'
			if (*endp != '\0' && *endp != '\n') {
				DISPLAY_LOGD("Read from " RTC_SYS_FILE " returned "
				      "invalid string %s (%s)", buffer, endp);
                printf("Read from " RTC_SYS_FILE " returned \n"
				      "invalid string %s (%s)", buffer, endp);
				res = -1;
			}
		}
        printf("read RTC sys buffer=%s \n", buffer);
	}

	return res;
}

/* ats_2 contains the time offset as 8-byte binary representation */
void restore_ats(uint64_t value) {
	FILE *fp = NULL;

	value *= 1000;
	fp = fopen(RTC_ATS_FILE, "wb");

	if (fp != NULL) {
		char str[16];
		sprintf(str,"%ld", value);
		fwrite(str, sizeof(char), 16, fp);
		fclose(fp);
	} else {
		DISPLAY_LOGD("Can't restore " RTC_ATS_FILE);
        printf("Can't restore " RTC_ATS_FILE "\n");
	}
}

uint64_t read_ats() {
	int res = 0;
	uint64_t value = 0;

	int fd = open(RTC_ATS_FILE, O_RDONLY);
	if (fd == -1) {
		res = -errno;
		DISPLAY_LOGD("Failed to open RTC ats path");
        printf("Failed to open RTC ats path\n");
	} else {
		char buffer[16];
		memset(buffer, 0, 16);
		res = read(fd, buffer, 16);
		if (res > 0) {
			char *endp = NULL;
			value = strtoull(buffer, &endp, 10);
			// sysfs read returns newline, ok to end up at '\n'
			if (*endp != '\0' && *endp != '\n') {
				DISPLAY_LOGD("Read from " RTC_SYS_FILE " returned "
				      "invalid string %s (%s)", buffer, endp);
                printf("Read from " RTC_SYS_FILE " returned \n"
				      "invalid string %s (%s)", buffer, endp);
				res = -1;
			}
		}
        printf("read RTC sys buffer=%s \n", buffer);
	}

	return value / 1000;
}

int store_time() {
	//char prop[PROPERTY_VALUE_MAX];
	unsigned long long seconds = 0;
	unsigned long long epoch_since = 0;
	int res = -1;
	struct tm tm;
	time_t t;

	// Get time and adjust for local time
	time(&t);
	localtime_r(&t, &tm);
	seconds = mktime(&tm);

	if (seconds > 0) {
		res = read_epoch(&epoch_since);
		if (res < 0) {
			DISPLAY_LOGD("Failed to read epoch while storing");
            printf("Failed to read epoch while storing \n");
		} else {
			seconds -= epoch_since;
			restore_ats(seconds);
			DISPLAY_LOGD("Time adjustment stored to property");
            printf("Time adjustment stored to property seconds=%lld \n", seconds);
			res = 0;
		}
	}

	return res;
}


int restore_time() {
	struct timeval tv;
	unsigned long long time_adjust = 0;
	unsigned long long epoch_since = 0;
	int res = -1;

	res = read_epoch(&epoch_since);
	if (res < 0) {
		DISPLAY_LOGD("Failed to read from " RTC_SYS_FILE
		      " (%d), bailing out", res);
        printf("Failed to read from " RTC_SYS_FILE
		      " (%d), bailing out \n", res);
	} else {
		// restore_ats(time_adjust);
        time_adjust = read_ats();
		tv.tv_sec = epoch_since + time_adjust;
		tv.tv_usec = 0;
		res = settimeofday(&tv, NULL);
		if (res != 0) {
			DISPLAY_LOGD("Failed to restore time (%d), root?",
			      res);
            printf("Failed to restore time (%d), root? \n",
			      res);
		} else {
			DISPLAY_LOGD("Time restored!");
            printf("Time restored! \n");
		}
	}

	return res;
}

int main(int argc, char* argv[]) {
	int res = -1;
	if (argc != 2) {
		DISPLAY_LOGD("usage: timekeep store|restore");
        printf("usage: timekeep store|restore|readAts \n");
		return res;
	}

	// Keep CAP_SYS_TIME and drop to system user
	if (strcmp(argv[1], "readAts") == 0) {
		res = read_ats();
	}

	if (strcmp(argv[1], "store") == 0) {
		res = store_time();
	}

	if (strcmp(argv[1], "restore") == 0) {
		res = restore_time();
	}

	return res;
}