// 10. Generate Report (shown in a list, can be exported to a text file)

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/report.h"

#define MAX_LINES 1100

static char report[REPORT_SIZE];
static char *lines[MAX_LINES];
static int lineCount;
static int scrollIndex;
static int active;

void reportOpen(void)
{
    buildReport(report, sizeof(report));

    // Split the report into lines.
    lineCount = 0;
    char *start = report;

    for (char *p = report; *p != '\0' && lineCount < MAX_LINES; p++)
    {
        if (*p == '\n')
        {
            *p = '\0';
            lines[lineCount++] = start;
            start = p + 1;
        }
    }

    if (*start != '\0' && lineCount < MAX_LINES)
        lines[lineCount++] = start;

    scrollIndex = 0;
    active = -1;
}

static void doExport(void)
{
    char path[600];
    char error[700];

    snprintf(path, sizeof(path), "%s/report.txt", dataFolder());

    if (exportReport(path, error, sizeof(error)))
        showMessage("Report Exported", TextFormat("Saved to:\n%s", path));
    else
        showMessage("Error", error);
}

void reportDraw(void)
{
    float x = AREA_X + 20, y = AREA_Y + 30;

    int focus = -1;
    GuiListViewEx((Rectangle){x, y, AREA_WIDTH - 40, AREA_HEIGHT - 90},
                  lines, lineCount, &scrollIndex, &active, &focus);

    if (GuiButton((Rectangle){x, AREA_Y + AREA_HEIGHT - 45, 150, 30},
                  "Export to Text File"))
        doExport();
}
