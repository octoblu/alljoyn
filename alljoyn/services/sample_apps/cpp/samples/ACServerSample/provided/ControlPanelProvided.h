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

#ifndef CONTROLPANELPROVIDED_H_
#define CONTROLPANELPROVIDED_H_

#include <qcc/platform.h>
#include <stdio.h>
#include <alljoyn/controlpanel/CPSDate.h>
#include <alljoyn/controlpanel/CPSTime.h>

void disableFan();
void enableFan();
void disableTempSelect();
void enableTempSelect();

const char* getNotificationString();
uint16_t isThereANotificationToSend();

uint16_t getCurrentTargetTemp();
void checkTargetTempReached();

void setTemperatureFieldUpdate();
void setStatusFieldUpdate();
void setTempSelectorFieldUpdate();
void setFanSpeedSelectorFieldUpdate();

char const* getCurrentTemperatureString();
void setCurrentTemperatureString(char const* newTemp);
char const* getCurrentHumidityString();
void setCurrentHumidityString(char const* newHumidity);

uint16_t getTargetTemperature();
void setTargetTemperature(uint16_t newTemp);

uint16_t getCurrentMode();
void setCurrentMode(uint16_t newMode);

uint16_t getFanSpeed();
void setFanSpeed(uint16_t newSpeed);

const char* getStatusString();
void setStatusString(const char* newStatusString);

uint8_t checkForUpdatesToSend();

void OnTurnFanOnButton(bool chooseYes);
void OnTurnFanOffButton(bool chooseYes);

bool getOfferToTurnOffTheFan();
void setOfferToTurnOffTheFan(bool turnOffTheFan);

bool getOfferToTurnOnTheFan();
void setOfferToTurnOnTheFan(bool turnOnTheFan);

uint8_t checkForEventsToSend();
void resetEventsToSend();
uint8_t getEventsToSend();

#endif /* CONTROLPANELPROVIDED_H_ */
