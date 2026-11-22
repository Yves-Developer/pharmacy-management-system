// 5. Check Stock and Expiry
// 6. Calculate Stock Value (popup)

#include <stdio.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/medicine.h"

static char idText[20];
static bool editId;

void checkStockOpen(void)
{
    idText[0] = '\0';
    editId = false;
}

static void doCheck(void)
{
    int id;
    Medicine m;

    if (!parsePositiveInt(idText, &id))
    {
        showMessage("Error", "Medicine ID must be a positive whole number.");
        return;
    }

    if (!getMedicineById(id, &m))
    {
        showMessage("Error", TextFormat("Medicine ID %d was not found.", id));
        return;
    }

    showMessage("Stock and Expiry",
                TextFormat("%s\nStock level: %d\nExpiry date: %02d/%02d/%04d",
                           m.name, m.quantity,
                           m.expiryDay, m.expiryMonth, m.expiryYear));

    // Warnings appear after the first message.
    if (isMedicineLowStock(m))
        showMessage("Low Stock Warning",
                    TextFormat("Only %d left in stock.", m.quantity));

    if (isMedicineExpiringSoon(m))
        showMessage("Expiry Warning",
                    TextFormat("This medicine expires in %d day(s).",
                               daysUntilExpiry(m)));
}

void checkStockDraw(void)
{
    float x = AREA_X + 20, y = AREA_Y + 50;

    textField("Medicine ID", (Rectangle){x, y, 300, FIELD_HEIGHT},
              idText, sizeof(idText), &editId);

    if (GuiButton((Rectangle){x, y + 55, 150, 30}, "Check"))
        doCheck();
}

void showStockValue(void)
{
    double stockValue, expiredLoss;
    calculateStockValue(&stockValue, &expiredLoss);

    showMessage("Stock Value",
                TextFormat("Total stock value (not expired): %.2f\n"
                           "Expired stock value (loss): %.2f",
                           stockValue, expiredLoss));
}
