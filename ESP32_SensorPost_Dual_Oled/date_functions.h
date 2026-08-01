#ifndef DATE_FUNCTIONS_H
#define DATE_FUNCTIONS_H

#include <Arduino.h>
#include <RTClib.h>

extern const char *const DAYS_OF_WEEK[7];

String buildReadableDate(const DateTime &dateTime);
String buildReadableTime(const DateTime &dateTime);
String buildUTCDate(const DateTime &dateTime);

#endif
