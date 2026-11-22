#ifndef UI_H
#define UI_H

// Helpers shared by the screens (raygui default controls).

#include <stdbool.h>
#include "raylib.h"

// ---------------- Layout ----------------

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 560

// Menu buttons on the left, the current screen on the right.
#define MENU_X 10
#define MENU_WIDTH 160

#define AREA_X 180
#define AREA_Y 10
#define AREA_WIDTH 710
#define AREA_HEIGHT 540

#define FIELD_HEIGHT 24

// ---------------- Text fields ----------------

// Label + text box. *editMode is true while typing in it.
void textField(const char *label, Rectangle bounds,
               char *text, int size, bool *editMode);

// ---------------- Input parsing ----------------

void trimText(char *text);
int parsePositiveInt(const char *text, int *value);
int parsePositiveFloat(const char *text, float *value);

// ---------------- Saving ----------------

// Folder of the data files (the "data" folder next to the program).
const char *dataFolder(void);

// Save all data after a change. Shows a message if it fails.
void saveData(void);

// ---------------- Message dialogs (GuiMessageBox) ----------------

// Messages are queued and shown one after the other.
void showMessage(const char *title, const char *message);

// Yes/No question. onYes(value) runs only if the user clicks "Yes".
void showConfirm(const char *title, const char *message,
                 void (*onYes)(int value), int value);

bool messageOpen(void);

// Draw the first message in the queue. Call last in the frame.
void drawMessage(void);

#endif
