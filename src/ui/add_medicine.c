// 1. Add Medicine

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/medicine.h"
#include "../../include/date_utils.h"

static const char *CATEGORIES[] = {"Tablet", "Syrup", "Injection", "Ointment"};

static char idText[20];
static char nameText[NAME_LENGTH];
static char priceText[30];
static char quantityText[20];
static char batchText[BATCH_LENGTH];
static char manufactureText[20];
static char expiryText[20];
static int category;
static bool categoryOpen;
static bool prescriptionOnly;
static bool edit[7];   // which text box is being typed in

void addMedicineOpen(void)
{
    idText[0] = nameText[0] = priceText[0] = quantityText[0] = '\0';
    batchText[0] = manufactureText[0] = expiryText[0] = '\0';
    category = 0;
    categoryOpen = false;
    prescriptionOnly = false;
    memset(edit, 0, sizeof(edit));
}

static void saveMedicine(void)
{
    Medicine m = {0};
    char error[200];

    // Check the format of each field.
    if (!parsePositiveInt(idText, &m.id))
    {
        showMessage("Error", "Medicine ID must be a positive whole number.");
        return;
    }

    if (!parsePositiveFloat(priceText, &m.price))
    {
        showMessage("Error", "Unit price must be a number greater than 0.");
        return;
    }

    if (!parsePositiveInt(quantityText, &m.quantity))
    {
        showMessage("Error", "Quantity must be a whole number greater than 0.");
        return;
    }

    if (!parseDateText(manufactureText, &m.manufactureDay,
                       &m.manufactureMonth, &m.manufactureYear))
    {
        showMessage("Error", "Manufacture date must be a valid DD/MM/YYYY date.");
        return;
    }

    if (!parseDateText(expiryText, &m.expiryDay,
                       &m.expiryMonth, &m.expiryYear))
    {
        showMessage("Error", "Expiry date must be a valid DD/MM/YYYY date.");
        return;
    }

    trimText(nameText);
    trimText(batchText);
    snprintf(m.name, sizeof(m.name), "%s", nameText);
    snprintf(m.batchNumber, sizeof(m.batchNumber), "%s", batchText);
    snprintf(m.category, sizeof(m.category), "%s", CATEGORIES[category]);
    m.prescriptionOnly = prescriptionOnly ? 1 : 0;

    // addMedicine() checks the rules (unique ID, dates...).
    if (!addMedicine(m, error, sizeof(error)))
    {
        showMessage("Error", error);
        return;
    }

    saveData();
    showMessage("Medicine Saved",
                TextFormat("%s (ID %d) was saved.", m.name, m.id));
    addMedicineOpen();
}

void addMedicineDraw(void)
{
    float x1 = AREA_X + 20, x2 = AREA_X + 360, w = 300;
    float y = AREA_Y + 50, step = 55;

    // Lock the other controls while the list is open.
    if (categoryOpen)
        GuiLock();

    textField("Medicine ID", (Rectangle){x1, y, w, FIELD_HEIGHT},
              idText, sizeof(idText), &edit[0]);
    textField("Name", (Rectangle){x1, y + step, w, FIELD_HEIGHT},
              nameText, sizeof(nameText), &edit[1]);
    GuiLabel((Rectangle){x1, y + 2 * step - 20, w, 20}, "Category");
    textField("Unit price", (Rectangle){x1, y + 3 * step, w, FIELD_HEIGHT},
              priceText, sizeof(priceText), &edit[2]);

    textField("Quantity", (Rectangle){x2, y, w, FIELD_HEIGHT},
              quantityText, sizeof(quantityText), &edit[3]);
    textField("Batch number", (Rectangle){x2, y + step, w, FIELD_HEIGHT},
              batchText, sizeof(batchText), &edit[4]);
    textField("Manufacture date (DD/MM/YYYY)",
              (Rectangle){x2, y + 2 * step, w, FIELD_HEIGHT},
              manufactureText, sizeof(manufactureText), &edit[5]);
    textField("Expiry date (DD/MM/YYYY)",
              (Rectangle){x2, y + 3 * step, w, FIELD_HEIGHT},
              expiryText, sizeof(expiryText), &edit[6]);

    GuiCheckBox((Rectangle){x1, y + 4 * step, 20, 20},
                "Prescription only", &prescriptionOnly);

    if (GuiButton((Rectangle){x1, y + 5 * step, 150, 30}, "Add Medicine"))
        saveMedicine();

    if (categoryOpen && !messageOpen())
        GuiUnlock();

    // Drawn last so the list is on top.
    if (GuiDropdownBox((Rectangle){x1, y + 2 * step, w, FIELD_HEIGHT},
                       "Tablet;Syrup;Injection;Ointment", &category, categoryOpen))
        categoryOpen = !categoryOpen;
}
