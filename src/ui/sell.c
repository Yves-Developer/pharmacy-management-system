// 4. Sell Medicine

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/medicine.h"
#include "../../include/sales.h"

static char idText[20];
static char quantityText[20];
static char customerText[CUSTOMER_LENGTH];
static char prescriptionText[PRESCRIPTION_LENGTH];
static bool edit[4];

void sellOpen(void)
{
    idText[0] = quantityText[0] = customerText[0] = prescriptionText[0] = '\0';
    memset(edit, 0, sizeof(edit));
}

static void doSell(void)
{
    int id, quantity;
    char error[200];
    Sale sale;

    if (!parsePositiveInt(idText, &id))
    {
        showMessage("Error", "Medicine ID must be a positive whole number.");
        return;
    }

    if (!parsePositiveInt(quantityText, &quantity))
    {
        showMessage("Error", "Quantity must be a whole number greater than 0.");
        return;
    }

    trimText(customerText);
    trimText(prescriptionText);

    // sellMedicine() checks expiry, prescription and stock.
    if (!sellMedicine(id, quantity, customerText, prescriptionText,
                      &sale, error, sizeof(error)))
    {
        showMessage("Error", error);
        return;
    }

    saveData();

    // Receipt popup.
    showMessage(TextFormat("Receipt #%d", sale.saleId),
                TextFormat("Date: %02d/%02d/%04d %s\n"
                           "Customer: %s\n"
                           "Medicine: %s\n"
                           "Quantity: %d x %.2f\n"
                           "Total: %.2f",
                           sale.day, sale.month, sale.year, sale.time,
                           sale.customerName, sale.medicineName,
                           sale.quantity, sale.unitPrice, sale.total));
    sellOpen();
}

void sellDraw(void)
{
    float x = AREA_X + 20, w = 300, y = AREA_Y + 50, step = 55;

    textField("Medicine ID", (Rectangle){x, y, w, FIELD_HEIGHT},
              idText, sizeof(idText), &edit[0]);
    textField("Quantity", (Rectangle){x, y + step, w, FIELD_HEIGHT},
              quantityText, sizeof(quantityText), &edit[1]);
    textField("Customer name", (Rectangle){x, y + 2 * step, w, FIELD_HEIGHT},
              customerText, sizeof(customerText), &edit[2]);
    textField("Prescription number (prescription-only medicines)",
              (Rectangle){x, y + 3 * step, w, FIELD_HEIGHT},
              prescriptionText, sizeof(prescriptionText), &edit[3]);

    if (GuiButton((Rectangle){x, y + 4 * step, 150, 30}, "Sell"))
        doSell();
}
