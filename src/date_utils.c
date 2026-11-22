#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../include/date_utils.h"

// Check whether a year is a leap year.
int isLeapYear(int year)
{
    return (year % 400 == 0) ||
           (year % 4 == 0 && year % 100 != 0);
}

// Get the number of days in a month.
int daysInMonth(int month, int year)
{
    int days[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (month < 1 || month > 12)
        return 0;

    if (month == 2 && isLeapYear(year))
        return 29;

    return days[month - 1];
}

// Check whether a date is valid.
int isValidDate(int day, int month, int year)
{
    if (year < 1900 || year > 2200)
        return 0;

    if (month < 1 || month > 12)
        return 0;

    if (day < 1 || day > daysInMonth(month, year))
        return 0;

    return 1;
}

// Compare two dates: year first, then month, then day.
int compareDates(
    int day1, int month1, int year1,
    int day2, int month2, int year2)
{
    if (year1 != year2)
        return (year1 < year2) ? -1 : 1;

    if (month1 != month2)
        return (month1 < month2) ? -1 : 1;

    if (day1 != day2)
        return (day1 < day2) ? -1 : 1;

    return 0;
}

void getToday(int *day, int *month, int *year)
{
    time_t now = time(NULL);
    struct tm *today = localtime(&now);

    *day = today->tm_mday;
    *month = today->tm_mon + 1;
    *year = today->tm_year + 1900;
}

// Date as time_t at midday (midday avoids daylight-saving errors).
static time_t dateToTime(int day, int month, int year)
{
    struct tm date = {0};

    date.tm_mday = day;
    date.tm_mon = month - 1;
    date.tm_year = year - 1900;
    date.tm_hour = 12;
    date.tm_isdst = -1;

    return mktime(&date);
}

int daysFromToday(int day, int month, int year)
{
    int todayDay, todayMonth, todayYear;
    getToday(&todayDay, &todayMonth, &todayYear);

    double seconds = difftime(
        dateToTime(day, month, year),
        dateToTime(todayDay, todayMonth, todayYear));

    // Round to the nearest whole day.
    double days = seconds / (60.0 * 60.0 * 24.0);

    return (int)(days < 0 ? days - 0.5 : days + 0.5);
}

int parseDateText(const char *text, int *day, int *month, int *year)
{
    char extra;

    // %c catches extra text after the year, e.g. "01/02/2027x".
    if (sscanf(text, "%d/%d/%d%c", day, month, year, &extra) != 3)
        return 0;

    return isValidDate(*day, *month, *year);
}

void getCurrentTime(char *buffer, int size)
{
    time_t now = time(NULL);
    struct tm *current = localtime(&now);

    strftime(buffer, size, "%H:%M:%S", current);
}
