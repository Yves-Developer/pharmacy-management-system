#ifndef DATE_UTILS_H
#define DATE_UTILS_H

// Date helpers. A date is three ints: day, month, year.

int isLeapYear(int year);

int daysInMonth(int month, int year);

// Returns 1 if day/month/year is a real calendar date.
int isValidDate(int day, int month, int year);

// Returns -1 if date1 < date2, 0 if equal, 1 if date1 > date2.
int compareDates(
    int day1, int month1, int year1,
    int day2, int month2, int year2);

// Fill day/month/year with today's date.
void getToday(int *day, int *month, int *year);

// Number of days from today until the given date.
// Negative = date is in the past, 0 = today.
int daysFromToday(int day, int month, int year);

// Parse "DD/MM/YYYY". Returns 1 only if the text is a valid date.
int parseDateText(const char *text, int *day, int *month, int *year);

// Write the current time as "HH:MM:SS".
void getCurrentTime(char *buffer, int size);

#endif
