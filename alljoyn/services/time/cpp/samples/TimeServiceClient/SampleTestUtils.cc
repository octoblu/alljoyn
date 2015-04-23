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

#include "SampleTestUtils.h"
#include <sstream>
#include <iostream>
#include <ctime>
#include <map>


using namespace ajn;
using namespace services;

//DateTime string representation
void sampleTestUtils::printDateTime(TimeServiceDateTime const& dateTime)
{

    const TimeServiceDate& date = dateTime.getDate();
    const TimeServiceTime& time = dateTime.getTime();

    printf("{DateTime: {Date %d-%d-%d} {Time %d:%d:%d.%d} Offset: '%d'}\n", (int)date.getDay(), (int)date.getMonth(),
           (unsigned int)date.getYear(), (int)time.getHour(), (int)time.getMinute(), time.getSecond(), time.getMillisecond(),
           dateTime.getOffsetMinutes());
}

//Generate DateTime
void sampleTestUtils::dateTimeNow(TimeServiceDateTime* dateTime, uint16_t offsetSeconds)
{

    time_t timeObj;
    struct tm* timeInfo;

    //Current time
    time(&timeObj);
    timeInfo = localtime(&timeObj);

    if (!timeInfo) {

        printf("!!! Failed to get localtime, can't initialize dateTime object !!! \n");
        return;
    }

    if (offsetSeconds > 0) {

        timeInfo->tm_sec += offsetSeconds;
        mktime(timeInfo);
    }

    TimeServiceTime tsTime;
    tsTime.init(timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, 0);

    TimeServiceDate tsDate;
    tsDate.init(timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);

    dateTime->init(tsDate, tsTime, 0);
}

/**
 * Print out the Schedule
 */
void sampleTestUtils::printSchedule(const TimeServiceSchedule& schedule)
{
    const TimeServiceTime& time = schedule.getTime();

    printf("{Schedule: {Time %d:%d:%d.%d} weekdays: '%s'}\n",  (int)time.getHour(), (int)time.getMinute(), time.getSecond(), time.getMillisecond()
           , getWeekdaysString(schedule.getWeekDays()).c_str());
}

/**
 * Creates String representation of the week days
 */
qcc::String sampleTestUtils::getWeekdaysString(const uint8_t weekDays)
{

    const char* days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

    int x;
    qcc::String str;

    for (x = 0; x < 7; x++) {

        if (weekDays & (1 << x)) {
            str = str + days[x] + " ";
        }
    }

    return str;
}

//Converts from comma separated days of week to a weekDays bit map
uint8_t sampleTestUtils::getWeekdaysNum(std::string weekDaysStr)
{

    std::vector<std::string> days;
    weekDaysFromString(weekDaysStr, &days);

    uint8_t bitMap = 0;

    std::map<std::string, uint8_t> bitMask;
    bitMask["sun"] = TimeServiceSchedule::SUNDAY;
    bitMask["mon"] = TimeServiceSchedule::MONDAY;
    bitMask["tue"] = TimeServiceSchedule::TUESDAY;
    bitMask["wen"] = TimeServiceSchedule::WEDNESDAY;
    bitMask["thu"] = TimeServiceSchedule::THURSDAY;
    bitMask["fri"] = TimeServiceSchedule::FRIDAY;
    bitMask["sat"] = TimeServiceSchedule::SATURDAY;

    for (std::vector<std::string>::iterator iter = days.begin(); iter != days.end(); ++iter) {

        std::map<std::string, uint8_t>::iterator bitMaskIter = bitMask.find(*iter);

        if (bitMaskIter == bitMask.end()) {

            printf("Not a day of week: '%s' \n", (*iter).c_str());
            continue;
        }

        bitMap |= bitMaskIter->second;
    }

    return bitMap;
}

//split by "," weekDaysStr
void sampleTestUtils::weekDaysFromString(std::string weekDaysStr, std::vector<std::string>* weekDays)
{

    std::istringstream instr(weekDaysStr);
    std::string token;

    while (std::getline(instr, token, ',')) {

        weekDays->push_back(token);
    }
}

/**
 * Print out the Schedule
 */

void sampleTestUtils::printPeriod(const TimeServicePeriod& interval, const char*meaning)
{
    printf("{%s: {Period: %d:%d:%d.%d}}\n",  meaning, (int)interval.getHour(), (int)interval.getMinute(), interval.getSecond(), interval.getMillisecond());
}

bool sampleTestUtils::periodFromString(const std::string& periodStr, TimeServicePeriod& period)
{

    std::vector<std::string> periodVect;

    std::istringstream instr(periodStr);
    std::string token;

    while (std::getline(instr, token, ':')) {

        periodVect.push_back(token);
    }

    if (periodVect.size() != 3) {
        std::cout << "period " << periodStr << " has an invalid format, format should be hh:mm:ss" << std::endl;
        return false;
    }

    uint32_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond = 0;

    hour   = atoi(periodVect[0].c_str());
    minute = atoi(periodVect[1].c_str());
    second = atoi(periodVect[2].c_str());

    if (period.init(hour, minute, second, millisecond) != ER_OK) {

        return false;
    }

    return true;
}
