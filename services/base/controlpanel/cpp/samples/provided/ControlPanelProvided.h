/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <qcc/platform.h>
#include <stdio.h>
#include <alljoyn/controlpanel/CPSDate.h>
#include <alljoyn/controlpanel/CPSTime.h>

#define AJ_Printf(fmat, ...) \
    do { printf(fmat, ## __VA_ARGS__); } while (0)

#define UNICODE_HI_IN_HEBREW "\xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d\x00"
#define UNICODE_HI_IN_CHINESE "\xe5\x97\xa8"
#define UNICODE_TESTING_CHINESE "\xE6\xB5\x8B\xE8\xAF\x95\xE4\xB8\xAD\xE5\x9B\xBD"
#define UNICODE_MY_LABEL_CONTAINER "\xE6\x88\x91\xE7\x9A\x84\xE6\xA0\x87\xE7\xAD\xBE\xE6\x88\x91\xE7\x9A\x84\xE5\xAE\xB9\xE5\x99\xA8"
#define UNICODE_CURENT_TEMPERATURE "\xE5\xBD\x93\xE5\x89\x8D\xE6\xB8\xA9\xE5\xBA\xA6\xEF\xBC\x9A"
#define UNICODE_OVEN_TEMPERATURE "\xE7\x83\xA4\xE7\xAE\xB1\xE6\xB8\xA9\xE5\xBA\xA6"
#define UNICODE_REGULAR "\xE5\xAE\x9A\xE6\x9C\x9F"
#define UNICODE_HOT "\xE7\x83\xAD"
#define UNICODE_VERY_HOT "\xE9\x9D\x9E\xE5\xB8\xB8\xE7\x83\xAD"
#define UNICODE_DEGREES "\xE5\xBA\xA6"
#define UNICODE_START_OVEN  "\xE5\x90\xAF\xE5\x8A\xA8\xE7\x83\xA4\xE7\xAE\xB1"
#define UNICODE_ARE_YOU_SURE_YOU_WANT_TO_TURN_OFF_THE_LIGHT "\xE4\xBD\xA0\xE7\xA1\xAE\xE5\xAE\x9A\xE4\xBD\xA0\xE8\xA6\x81\xE5\xBC\x80\xE7\x81\xAF"
#define UNICODE_ARE_YOU_SURE "\xE4\xBD\xA0\xE7\xA1\xAE\xE5\xAE\x9A\xE5\x90\x97\xEF\xBC\x9F"
#define UNICODE_NO "\xE4\xB8\x8D"
#define UNICODE_CANCEL "\xE5\x8F\x96\xE6\xB6\x88"
#define UNICODE_TURN_ON_OVEN_LIGHT "\xE6\x89\x93\xE5\xBC\x80\xE7\x83\xA4\xE7\xAE\xB1\xE7\x81\xAF"
#define UNICODE_YES "\xE6\x98\xAF"

bool getboolVar();
void setboolVar(bool value);

double getdoubleVar();
void setdoubleVar(double value);

const char* getStringVar();
void setStringVar(const char* value);

uint16_t getuint16Var();
void setuint16Var(uint16_t value);

int16_t getint16Var();
void setint16Var(int16_t value);

uint32_t getuint32Var();
void setuint32Var(uint32_t value);

int32_t getint32Var();
void setint32Var(int32_t value);

uint64_t getuint64Var();
void setuint64Var(uint64_t value);

int64_t getint64Var();
void setint64Var(int64_t value);

bool getEnabledFunc();
bool getWriteableFunc();

const char* getTestString(uint16_t language);
const char* getUrlString();

const ajn::services::CPSDate& getDateProperty();
void setDateProperty(const ajn::services::CPSDate& datePropertyValue);

const ajn::services::CPSTime& getTimeProperty();
void setTimeProperty(const ajn::services::CPSTime& timePropertyValue);

/**
 * Functions for Oven.xml
 */
void startOven();
void stopOven();
uint16_t getTemperature();
void setTemperature(uint16_t temperature);
uint16_t getProgram();
void setProgram(uint16_t program);
const char* getProgramString(uint16_t language);

