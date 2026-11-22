// 8b. Manage Suppliers (add, show in a list, remove)

#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/supplier.h"

static char idText[20];
static char nameText[SUPPLIER_NAME_LENGTH];
static char phoneText[PHONE_LENGTH];
static char medicinesText[120];
static bool edit[4];
static int scrollIndex;
static int selected;

static char rows[MAX_SUPPLIERS][300];
static char *rowPointers[MAX_SUPPLIERS];

void suppliersOpen(void)
{
    idText[0] = nameText[0] = phoneText[0] = medicinesText[0] = '\0';
    memset(edit, 0, sizeof(edit));
    scrollIndex = 0;
    selected = -1;
}

static void doAdd(void)
{
    Supplier s = {0};
    char error[200];

    if (!parsePositiveInt(idText, &s.id))
    {
        showMessage("Error", "Supplier ID must be a positive whole number.");
        return;
    }

    s.medicineCount = parseMedicineIdList(medicinesText, s.medicineIds,
                                          MAX_SUPPLIED_MEDICINES);

    if (s.medicineCount < 0)
    {
        showMessage("Error", "Medicines must be IDs separated by commas, e.g. 101, 102");
        return;
    }

    trimText(nameText);
    trimText(phoneText);
    snprintf(s.name, sizeof(s.name), "%s", nameText);
    snprintf(s.phone, sizeof(s.phone), "%s", phoneText);

    if (!addSupplier(s, error, sizeof(error)))
    {
        showMessage("Error", error);
        return;
    }

    saveData();
    showMessage("Supplier Added", TextFormat("Supplier %s was added.", s.name));
    suppliersOpen();
}

static void doRemove(void)
{
    Supplier s;
    char error[200];

    if (selected < 0 || !getSupplierAt(selected, &s))
    {
        showMessage("Error", "Select a supplier from the list first.");
        return;
    }

    if (!removeSupplier(s.id, error, sizeof(error)))
    {
        showMessage("Error", error);
        return;
    }

    saveData();
    showMessage("Supplier Removed", TextFormat("Supplier %s was removed.", s.name));
    selected = -1;
}

void suppliersDraw(void)
{
    float x = AREA_X + 20, y = AREA_Y + 50;

    textField("Supplier ID", (Rectangle){x, y, 100, FIELD_HEIGHT},
              idText, sizeof(idText), &edit[0]);
    textField("Name", (Rectangle){x + 110, y, 180, FIELD_HEIGHT},
              nameText, sizeof(nameText), &edit[1]);
    textField("Phone", (Rectangle){x + 300, y, 120, FIELD_HEIGHT},
              phoneText, sizeof(phoneText), &edit[2]);
    textField("Medicine IDs (e.g. 101, 102)", (Rectangle){x + 430, y, 150, FIELD_HEIGHT},
              medicinesText, sizeof(medicinesText), &edit[3]);

    if (GuiButton((Rectangle){x + 590, y - 3, 80, 30}, "Add"))
        doAdd();

    // Supplier list.
    int count = getSupplierCount();

    for (int i = 0; i < count; i++)
    {
        Supplier s;
        char list[200];

        getSupplierAt(i, &s);
        formatMedicineIdList(&s, list, sizeof(list));

        snprintf(rows[i], sizeof(rows[i]), "%d | %s | %s | Medicines: %s",
                 s.id, s.name, s.phone, list);
        rowPointers[i] = rows[i];
    }

    int focus = -1;
    GuiListViewEx((Rectangle){x, y + 45, AREA_WIDTH - 40, AREA_HEIGHT - 150},
                  rowPointers, count, &scrollIndex, &selected, &focus);

    if (GuiButton((Rectangle){x, AREA_Y + AREA_HEIGHT - 45, 150, 30},
                  "Remove Selected"))
        doRemove();
}
