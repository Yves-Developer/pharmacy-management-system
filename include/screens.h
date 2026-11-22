#ifndef SCREENS_H
#define SCREENS_H

// Each screen has Open (clears the fields) and Draw (called every frame).

typedef enum
{
    SCREEN_ADD_MEDICINE,
    SCREEN_VIEW_MEDICINES,
    SCREEN_UPDATE_STOCK,
    SCREEN_SELL,
    SCREEN_CHECK_STOCK,
    SCREEN_DISCOUNT,
    SCREEN_REMOVE_MEDICINE,
    SCREEN_SUPPLIERS,
    SCREEN_HISTORY,
    SCREEN_REPORT,
    SCREEN_COUNT
} ScreenId;

void addMedicineOpen(void);
void addMedicineDraw(void);

void viewMedicinesOpen(void);
void viewMedicinesDraw(void);

void updateStockOpen(void);
void updateStockDraw(void);

void sellOpen(void);
void sellDraw(void);

void checkStockOpen(void);
void checkStockDraw(void);

// "Calculate Stock Value" is a popup, not a screen.
void showStockValue(void);

void discountOpen(void);
void discountDraw(void);

void removeMedicineOpen(void);
void removeMedicineDraw(void);

void suppliersOpen(void);
void suppliersDraw(void);

void historyOpen(void);
void historyDraw(void);

void reportOpen(void);
void reportDraw(void);

#endif
