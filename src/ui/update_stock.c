// 3. Update Stock

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/medicine.h"
#include "../../include/date_utils.h"

static char idText[20];
static char quantityText[20];
static char batchText[BATCH_LENGTH];
static char expiryText[20];
static bool edit[4];

void updateStockOpen(void)
{
    idText[0] = quantityText[0] = batchText[0] = expiryText[0] = '\0';
    memset(edit, 0, sizeof(edit));
}

static void doUpdate(void)
{
    int id, quantity, day, month, year;
    char error[200];

    if (!parsePositiveInt(idText, &id))
    {
        showMessage("Error", "Medicine ID must be a positive whole number.");
        return;
    }

    if (!parsePositiveInt(quantityText, &quantity))
    {
        showMessage("Error", "Quantity to add must be a whole number greater than 0.");
        return;
    }

    if (!parseDateText(expiryText, &day, &month, &year))
    {
        showMessage("Error", "Expiry date must be a valid DD/MM/YYYY date.");
        return;
    }

    trimText(batchText);

    if (!updateStock(id, quantity, batchText, day, month, year,
                     error, sizeof(error)))
    {
        showMessage("Error", error);
        return;
    }

    Medicine m;
    getMedicineById(id, &m);

    saveData();
    showMessage("Stock Updated",
                TextFormat("%s now has %d in stock.", m.name, m.quantity));
    updateStockOpen();
}

void updateStockDraw(void)
{
    float x = AREA_X + 20, w = 300, y = AREA_Y + 50, step = 55;

    textField("Medicine ID", (Rectangle){x, y, w, FIELD_HEIGHT},
              idText, sizeof(idText), &edit[0]);
    textField("Quantity to add", (Rectangle){x, y + step, w, FIELD_HEIGHT},
              quantityText, sizeof(quantityText), &edit[1]);
    textField("New batch number", (Rectangle){x, y + 2 * step, w, FIELD_HEIGHT},
              batchText, sizeof(batchText), &edit[2]);
    textField("New expiry date (DD/MM/YYYY)",
              (Rectangle){x, y + 3 * step, w, FIELD_HEIGHT},
              expiryText, sizeof(expiryText), &edit[3]);

    if (GuiButton((Rectangle){x, y + 4 * step, 150, 30}, "Update Stock"))
        doUpdate();
}
