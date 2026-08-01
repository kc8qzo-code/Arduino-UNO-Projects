#include "date_functions.h"

const char *const DAYS_OF_WEEK[7] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"};

bool parseUTCDate(const String &utcTime, DateTime &dateTime)
{
  unsigned int year;
  unsigned int month;
  unsigned int day;
  unsigned int hour;
  unsigned int minute;
  unsigned int second;

  if (sscanf(utcTime.c_str(),
             "%4u-%2u-%2uT%2u:%2u:%2u",
             &year, &month, &day, &hour, &minute, &second) != 6)
  {
    return false;
  }

  const DateTime parsed(year, month, day, hour, minute, second);
  if (!parsed.isValid())
  {
    return false;
  }

  dateTime = parsed;
  return true;
}

String buildReadableDate(const DateTime &dateTime)
{
  char dateBuffer[11];

  snprintf(
      dateBuffer,
      sizeof(dateBuffer),
      "%02u/%02u/%04u",
      dateTime.month(),
      dateTime.day(),
      dateTime.year());

  return String(dateBuffer);
}

String buildReadableTime(const DateTime &dateTime)
{
  uint8_t hour12 = dateTime.hour() % 12;

  if (hour12 == 0)
  {
    hour12 = 12;
  }

  const char *timeInAmPm =
      dateTime.hour() >= 12 ? "PM" : "AM";

  char timeBuffer[9];

  snprintf(
      timeBuffer,
      sizeof(timeBuffer),
      "%02u:%02u:%02u",
      hour12,
      dateTime.minute(),
      dateTime.second());

  return String(timeBuffer);
}

String buildUTCDate(const DateTime &dateTime)
{
  // The RTC is maintained in UTC and has one-second resolution.
  char utcDate[25];

  snprintf(utcDate, sizeof(utcDate),
           "%04u-%02u-%02uT%02u:%02u:%02u.%03luZ",
           static_cast<unsigned int>(dateTime.year()),
           static_cast<unsigned int>(dateTime.month()),
           static_cast<unsigned int>(dateTime.day()),
           static_cast<unsigned int>(dateTime.hour()),
           static_cast<unsigned int>(dateTime.minute()),
           static_cast<unsigned int>(dateTime.second()),
           0UL);

  return String(utcDate);
}
