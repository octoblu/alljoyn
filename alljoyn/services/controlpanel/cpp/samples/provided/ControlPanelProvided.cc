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

#include "ControlPanelProvided.h"
#include <qcc/String.h>

static bool boolVar = false;

static uint16_t uint16Var = 0;
static int16_t int16Var = 0;

static uint32_t uint32Var = 0;
static int32_t int32Var = 0;

static uint64_t uint64Var = 0;
static int64_t int64Var = 0;

static double doubleVar = 0;

static qcc::String StringVar = "Initial String";

static qcc::String sampleString = "This is a test";
static qcc::String sampleUrlString = "www.ControlPanelTest.com";

static ajn::services::CPSDate dateVar(13, 6, 2006);
static ajn::services::CPSTime timeVar(18, 30, 25);

bool getboolVar()
{
    return boolVar;
}

void setboolVar(bool value)
{
    boolVar = value;
}

double getdoubleVar()
{
    return doubleVar;
}

void setdoubleVar(double value)
{
    doubleVar = value;
}

const char* getStringVar()
{
    return StringVar.c_str();
}

void setStringVar(const char* value)
{
    StringVar = value;
}

uint16_t getuint16Var()
{
    return uint16Var;
}

void setuint16Var(uint16_t value)
{
    uint16Var = value;
}

int16_t getint16Var()
{
    return int16Var;
}

void setint16Var(int16_t value)
{
    int16Var = value;
}

uint32_t getuint32Var()
{
    return uint32Var;
}

void setuint32Var(uint32_t value)
{
    uint32Var = value;
}

int32_t getint32Var()
{
    return int32Var;
}

void setint32Var(int32_t value)
{
    int32Var = value;
}

uint64_t getuint64Var()
{
    return uint64Var;
}

void setuint64Var(uint64_t value)
{
    uint64Var = value;
}

int64_t getint64Var()
{
    return int64Var;
}

void setint64Var(int64_t value)
{
    int64Var = value;
}

bool getEnabledFunc()
{
    return true;
}

bool getWriteableFunc()
{
    return true;
}

const char* getTestString(uint16_t language)
{
    return sampleString.c_str();
}

const char* getUrlString()
{
    return sampleUrlString.c_str();
}

const ajn::services::CPSDate& getDateProperty()
{
    return dateVar;
}

void setDateProperty(const ajn::services::CPSDate& datePropertyValue)
{
    dateVar.setDay(datePropertyValue.getDay());
    dateVar.setMonth(datePropertyValue.getMonth());
    dateVar.setYear(datePropertyValue.getYear());
}

const ajn::services::CPSTime& getTimeProperty()
{
    return timeVar;
}

void setTimeProperty(const ajn::services::CPSTime& timePropertyValue)
{
    timeVar.setHour(timePropertyValue.getHour());
    timeVar.setMinute(timePropertyValue.getMinute());
    timeVar.setSecond(timePropertyValue.getSecond());
}

/**
 * Functions for Oven.xml
 */
static uint16_t temperatureVar = 0;
static uint16_t programVar = 0;
static qcc::String programString = "Program: ";

void startOven()
{
    printf("********* Starting the Oven *********\n");
}

void stopOven()
{
    printf("********* Stopping the Oven *********\n");
}

uint16_t getTemperature()
{
    return temperatureVar;
}

void setTemperature(uint16_t temperature)
{
    temperatureVar = temperature;
}

uint16_t getProgram()
{
    return programVar;
}

void setProgram(uint16_t program)
{
    programVar = program;
}

const char* getProgramString(uint16_t language)
{
    return programString.c_str();
}

