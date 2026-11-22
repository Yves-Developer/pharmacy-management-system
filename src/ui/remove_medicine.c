// 8a. Remove Medicine

#include <stdio.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/screens.h"
#include "../../include/medicine.h"

static char idText[20];
static bool editId;

void removeMedicineOpen(void)
{
    idText[0] = '\0';
    editId = false;
}

// Runs when the user clicks "Yes" in the confirmation dialog.
static void confirmedRemove(int id)
{
    char error[200];

    if (!removeMedicine(id, error, sizeof(error)))
    {
        showMessage("Error", error);
        return;
    }

    saveData();
    showMessage("Removed", TextFormat("Medicine %d was removed.", id));
    removeMedicineOpen();
}

static void askRemove(void)
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

    showConfirm("Confirm Deletion",
                TextFormat("Remove %s (ID %d)?", m.name, m.id),
                confirmedRemove, id);
}

void removeMedicineDraw(void)
{
    float x = AREA_X + 20, y = AREA_Y + 50;

    textField("Medicine ID", (Rectangle){x, y, 300, FIELD_HEIGHT},
              idText, sizeof(idText), &editId);

    if (GuiButton((Rectangle){x, y + 55, 150, 30}, "Remove Medicine"))
        askRemove();
}
