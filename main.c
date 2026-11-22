// Pharmacy Management System
// Opens the window, asks for the user's name, shows the menu and the
// current screen, and saves the data when the window closes.

#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "raygui.h"

#include "ui.h"
#include "screens.h"
#include "storage.h"
#include "history.h"

typedef struct
{
    const char *title;
    void (*open)(void);
    void (*draw)(void);
} Screen;

static const Screen SCREENS[SCREEN_COUNT] = {
    [SCREEN_ADD_MEDICINE]    = {"Add Medicine",        addMedicineOpen,    addMedicineDraw},
    [SCREEN_VIEW_MEDICINES]  = {"View Medicines",      viewMedicinesOpen,  viewMedicinesDraw},
    [SCREEN_UPDATE_STOCK]    = {"Update Stock",        updateStockOpen,    updateStockDraw},
    [SCREEN_SELL]            = {"Sell Medicine",       sellOpen,           sellDraw},
    [SCREEN_CHECK_STOCK]     = {"Check Stock and Expiry", checkStockOpen,  checkStockDraw},
    [SCREEN_DISCOUNT]        = {"Apply Discount",      discountOpen,       discountDraw},
    [SCREEN_REMOVE_MEDICINE] = {"Remove Medicine",     removeMedicineOpen, removeMedicineDraw},
    [SCREEN_SUPPLIERS]       = {"Manage Suppliers",    suppliersOpen,      suppliersDraw},
    [SCREEN_HISTORY]         = {"Transaction History", historyOpen,        historyDraw},
    [SCREEN_REPORT]          = {"Report",              reportOpen,         reportDraw},
};

static ScreenId currentScreen = SCREEN_VIEW_MEDICINES;

static void openScreen(ScreenId screen)
{
    currentScreen = screen;
    SCREENS[screen].open();
}

// Left column: one button per feature.
static void drawMenu(void)
{
    float y = AREA_Y;

    for (int i = 0; i < SCREEN_COUNT; i++)
    {
        if (GuiButton((Rectangle){MENU_X, y, MENU_WIDTH, 30}, SCREENS[i].title))
            openScreen((ScreenId)i);

        y += 36;

        // Extra button: the stock value is shown in a popup.
        if (i == SCREEN_CHECK_STOCK)
        {
            if (GuiButton((Rectangle){MENU_X, y, MENU_WIDTH, 30},
                          "Calculate Stock Value"))
                showStockValue();

            y += 36;
        }
    }
}

// Asks for the user's name. Returns true once a name was entered.
static bool drawLogin(void)
{
    static char name[USER_LENGTH] = "";
    int button = -1;

    GuiTextInputBox((Rectangle){WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 70, 300, 140},
                    "Login", "Enter your name:", name, sizeof(name),
                    "OK", &button, NULL);

    if (button == 1)
    {
        trimText(name);

        if (name[0] == '\0')
        {
            showMessage("Error", "Please enter your name.");
            return false;
        }

        setCurrentUser(name);
        return true;
    }

    return false;
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Pharmacy Management System");
    SetTargetFPS(60);

    // List text starts on the left.
    GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

    loadAllData(dataFolder());

    bool loggedIn = false;
    openScreen(SCREEN_VIEW_MEDICINES);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // Lock the controls while a message box is open.
        if (messageOpen())
            GuiLock();

        if (!loggedIn)
        {
            loggedIn = drawLogin();
        }
        else
        {
            drawMenu();
            GuiGroupBox((Rectangle){AREA_X, AREA_Y, AREA_WIDTH, AREA_HEIGHT},
                        SCREENS[currentScreen].title);
            SCREENS[currentScreen].draw();
        }

        GuiUnlock();
        drawMessage();

        EndDrawing();
    }

    saveAllData(dataFolder());
    CloseWindow();

    return 0;
}
