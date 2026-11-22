// 9. Transaction History (scrollable list with filters)

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/history.h"
#include "../../include/date_utils.h"

static char idFilter[20];
static char dateFilter[20];
static bool edit[2];
static int scrollIndex;
static int active;

// Row texts for GuiListViewEx.
static char rows[MAX_HISTORY][300];
static char *rowPointers[MAX_HISTORY];

void historyOpen(void)
{
    idFilter[0] = dateFilter[0] = '\0';
    memset(edit, 0, sizeof(edit));
    scrollIndex = 0;
    active = -1;
}

void historyDraw(void)
{
    float x = AREA_X + 20, y = AREA_Y + 50;

    textField("Filter by medicine ID", (Rectangle){x, y, 200, FIELD_HEIGHT},
              idFilter, sizeof(idFilter), &edit[0]);
    textField("Filter by date (DD/MM/YYYY)", (Rectangle){x + 220, y, 200, FIELD_HEIGHT},
              dateFilter, sizeof(dateFilter), &edit[1]);

    // Empty filter = show all. Invalid filter = show nothing.
    int medicineId = 0, day = 0, month = 0, year = 0;
    bool validFilter = true;

    if (idFilter[0] != '\0' && !parsePositiveInt(idFilter, &medicineId))
        validFilter = false;

    if (dateFilter[0] != '\0' && !parseDateText(dateFilter, &day, &month, &year))
        validFilter = false;

    // Build the list rows, oldest first.
    int count = 0;
    HistoryEntry h;

    for (int i = 0; validFilter && getHistoryAt(i, &h); i++)
    {
        if (!historyEntryMatches(&h, medicineId, day, month, year))
            continue;

        snprintf(rows[count], sizeof(rows[count]),
                 "%02d/%02d/%04d %s | %s | %s | ID %d | %s",
                 h.day, h.month, h.year, h.time, h.user, h.action,
                 h.medicineId, h.details);
        rowPointers[count] = rows[count];
        count++;
    }

    if (!validFilter)
        GuiLabel((Rectangle){x + 440, y, 250, FIELD_HEIGHT},
                 "Invalid filter");

    int focus = -1;
    GuiListViewEx((Rectangle){x, y + 40, AREA_WIDTH - 40, AREA_HEIGHT - 110},
                  rowPointers, count, &scrollIndex, &active, &focus);
}
