#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY 2000
#define USER_LENGTH 50
#define ACTION_LENGTH 30
#define DETAILS_LENGTH 200

// One line in the transaction history.
typedef struct
{
    int day;
    int month;
    int year;
    char time[9];                  // "HH:MM:SS"

    char user[USER_LENGTH];        // who performed the operation
    char action[ACTION_LENGTH];    // ADD, SELL, UPDATE, REMOVE, DISCOUNT, ...
    int medicineId;                // 0 when not about one medicine
    char details[DETAILS_LENGTH];

} HistoryEntry;

// The user's name, saved with every history entry.
void setCurrentUser(const char *name);
const char *getCurrentUser(void);

// Record an operation with today's date and the current time.
void logAction(const char *action, int medicineId, const char *details);

int getHistoryAt(int index, HistoryEntry *entry);

// Filter: medicineId 0 = any ID, day 0 = any date.
int historyEntryMatches(
    const HistoryEntry *entry,
    int medicineId,
    int day, int month, int year);

// Used only when loading from file.
int restoreHistoryEntry(HistoryEntry entry);
void clearHistory(void);

#endif
