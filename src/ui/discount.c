// 7. Apply Discount (5-15 % on a selected sale)

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/sales.h"

static char percentText[20];
static bool editPercent;
static int reason;
static bool reasonOpen;
static int scrollIndex;
static int selected;   // row selected in the list, -1 = none

static char rows[MAX_SALES][200];
static char *rowPointers[MAX_SALES];

void discountOpen(void)
{
    percentText[0] = '\0';
    editPercent = false;
    reason = 0;
    reasonOpen = false;
    scrollIndex = 0;
    selected = -1;
}

static void doDiscount(void)
{
    Sale sale;
    float percent;
    char error[200];

    if (selected < 0 || !getSaleAt(selected, &sale))
    {
        showMessage("Error", "Select a sale from the list first.");
        return;
    }

    if (!parsePositiveFloat(percentText, &percent))
    {
        showMessage("Error", "Discount must be a number between 5 and 15.");
        return;
    }

    // applyDiscount() rejects values outside 5-15.
    if (!applyDiscount(sale.saleId, percent, DISCOUNT_REASONS[reason],
                       &sale, error, sizeof(error)))
    {
        showMessage("Error", error);
        return;
    }

    saveData();
    showMessage("Discount Applied",
                TextFormat("Sale #%d\nOriginal amount: %.2f\n"
                           "Discount: %.1f%% (%s)\nDiscounted amount: %.2f",
                           sale.saleId, sale.subtotal, sale.discountPercent,
                           sale.discountReason, sale.total));
    percentText[0] = '\0';
}

void discountDraw(void)
{
    float x = AREA_X + 20, y = AREA_Y + 30;

    if (reasonOpen)
        GuiLock();

    // List of sales to choose from.
    int count = getSaleCount();

    for (int i = 0; i < count; i++)
    {
        Sale s;
        getSaleAt(i, &s);

        snprintf(rows[i], sizeof(rows[i]),
                 "Sale #%d | %02d/%02d/%04d | %s | %d x %s | Total %.2f",
                 s.saleId, s.day, s.month, s.year, s.customerName,
                 s.quantity, s.medicineName, s.total);
        rowPointers[i] = rows[i];
    }

    GuiLabel((Rectangle){x, y, 300, 20}, "Select a sale:");

    int focus = -1;
    GuiListViewEx((Rectangle){x, y + 20, AREA_WIDTH - 40, 300},
                  rowPointers, count, &scrollIndex, &selected, &focus);

    float formY = y + 360;

    textField("Discount % (5 to 15)", (Rectangle){x, formY, 150, FIELD_HEIGHT},
              percentText, sizeof(percentText), &editPercent);
    GuiLabel((Rectangle){x + 170, formY - 20, 200, 20}, "Reason");

    if (GuiButton((Rectangle){x + 390, formY - 3, 150, 30}, "Apply Discount"))
        doDiscount();

    if (reasonOpen && !messageOpen())
        GuiUnlock();

    // Drawn last so the open list is on top.
    if (GuiDropdownBox((Rectangle){x + 170, formY, 200, FIELD_HEIGHT},
                       "Senior citizen;Insurance;Bulk purchase",
                       &reason, reasonOpen))
        reasonOpen = !reasonOpen;
}
