#include <stdio.h>
#include <string.h>

#include "../include/history.h"
#include "../include/date_utils.h"

// Static list of all operations, oldest first.
static HistoryEntry history[MAX_HISTORY];
static int historyCount = 0;

static char currentUser[USER_LENGTH] = "admin";

void setCurrentUser(const char *name)
{
    if (name != NULL && name[0] != '\0')
        snprintf(currentUser, sizeof(currentUser), "%s", name);
}

const char *getCurrentUser(void)
{
    return currentUser;
}

// Add an entry at the end (ignored if the list is full).
static void appendEntry(HistoryEntry entry)
{
    if (historyCount >= MAX_HISTORY)
        return;

    history[historyCount] = entry;
    historyCount++;
}

void logAction(const char *action, int medicineId, const char *details)
{
    HistoryEntry entry = {0};

    getToday(&entry.day, &entry.month, &entry.year);
    getCurrentTime(entry.time, sizeof(entry.time));

    snprintf(entry.user, sizeof(entry.user), "%s", currentUser);
    snprintf(entry.action, sizeof(entry.action), "%s", action);
    snprintf(entry.details, sizeof(entry.details), "%s", details);
    entry.medicineId = medicineId;

    appendEntry(entry);
}

int getHistoryAt(int index, HistoryEntry *entry)
{
    if (index < 0 || index >= historyCount)
        return 0;

    *entry = history[index];
    return 1;
}

int historyEntryMatches(
    const HistoryEntry *entry,
    int medicineId,
    int day, int month, int year)
{
    if (medicineId != 0 && entry->medicineId != medicineId)
        return 0;

    if (day != 0 &&
        compareDates(entry->day, entry->month, entry->year,
                     day, month, year) != 0)
        return 0;

    return 1;
}

int restoreHistoryEntry(HistoryEntry entry)
{
    appendEntry(entry);
    return 1;
}

void clearHistory(void)
{
    historyCount = 0;
}
