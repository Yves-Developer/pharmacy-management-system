// 2. View Medicines
// Red = low stock, orange = expires within 30 days, grey = expired.

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/medicine.h"

#define ROW_HEIGHT 24
#define COLUMN_COUNT 7

static const char *HEADERS[COLUMN_COUNT] = {
    "ID", "Name", "Category", "Quantity", "Unit Price", "Batch", "Expiry"
};
static const int WIDTHS[COLUMN_COUNT] = {50, 160, 80, 60, 70, 100, 80};

static Vector2 scroll;

void viewMedicinesOpen(void)
{
    scroll = (Vector2){0, 0};
}

// Row colour. Expired is checked first.
static Color rowColor(Medicine m)
{
    if (isMedicineExpired(m))
        return GRAY;

    if (isMedicineExpiringSoon(m))
        return ORANGE;

    if (isMedicineLowStock(m))
        return RED;

    return BLACK;
}

// Draw a cell; text longer than the column is cut.
static void drawCell(const char *text, int x, int y, int width, Color color)
{
    BeginScissorMode(x, y, width - 6, ROW_HEIGHT);
    DrawText(text, x, y + 7, GuiGetStyle(DEFAULT, TEXT_SIZE), color);
    EndScissorMode();
}

static void removeExpired(int id)
{
    char error[200];

    if (removeMedicine(id, error, sizeof(error)))
    {
        saveData();
        showMessage("Removed", TextFormat("Expired medicine %d was removed.", id));
    }
    else
    {
        showMessage("Error", error);
    }
}

void viewMedicinesDraw(void)
{
    float x = AREA_X + 10, y = AREA_Y + 30;

    // Column titles.
    int cx = (int)x + 6;

    for (int c = 0; c < COLUMN_COUNT; c++)
    {
        GuiLabel((Rectangle){cx, y, WIDTHS[c], 20}, HEADERS[c]);
        cx += WIDTHS[c];
    }

    // Rows inside a scroll panel.
    int count = getMedicineCount();
    Rectangle bounds = {x, y + 22, AREA_WIDTH - 20, AREA_HEIGHT - 62};
    Rectangle content = {0, 0, bounds.width - 20, (float)(count * ROW_HEIGHT)};
    Rectangle view = {0};

    GuiScrollPanel(bounds, NULL, content, &scroll, &view);

    // Remove after the loop, not while drawing the rows.
    int removeId = 0;

    for (int i = 0; i < count; i++)
    {
        Medicine m;
        getMedicineAt(i, &m);

        int rowY = (int)(view.y + scroll.y) + i * ROW_HEIGHT;

        // Skip rows that are not fully visible.
        if (rowY < view.y || rowY + ROW_HEIGHT > view.y + view.height)
            continue;

        Color color = rowColor(m);
        char cells[COLUMN_COUNT][NAME_LENGTH];

        snprintf(cells[0], NAME_LENGTH, "%d", m.id);
        snprintf(cells[1], NAME_LENGTH, "%s", m.name);
        snprintf(cells[2], NAME_LENGTH, "%s", m.category);
        snprintf(cells[3], NAME_LENGTH, "%d", m.quantity);
        snprintf(cells[4], NAME_LENGTH, "%.2f", m.price);
        snprintf(cells[5], NAME_LENGTH, "%s", m.batchNumber);
        snprintf(cells[6], NAME_LENGTH, "%02d/%02d/%04d",
                 m.expiryDay, m.expiryMonth, m.expiryYear);

        cx = (int)view.x + 6;

        for (int c = 0; c < COLUMN_COUNT; c++)
        {
            drawCell(cells[c], cx, rowY, WIDTHS[c], color);
            cx += WIDTHS[c];
        }

        // One-click removal for expired medicines.
        if (isMedicineExpired(m))
        {
            if (GuiButton((Rectangle){cx, rowY + 2, 70, ROW_HEIGHT - 4}, "Remove"))
                removeId = m.id;
        }
    }

    if (removeId != 0)
        removeExpired(removeId);
}
