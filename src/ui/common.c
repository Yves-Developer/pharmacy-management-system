#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include "raylib.h"
#include "raygui.h"

#include "../../include/ui.h"
#include "../../include/storage.h"

// ---------------- Text fields ----------------

void textField(const char *label, Rectangle bounds,
               char *text, int size, bool *editMode)
{
    GuiLabel((Rectangle){bounds.x, bounds.y - 20, bounds.width, 20}, label);

    // GuiTextBox returns 1 when typing starts or stops.
    if (GuiTextBox(bounds, text, size, *editMode))
        *editMode = !*editMode;
}

// ---------------- Input parsing ----------------

void trimText(char *text)
{
    int start = 0;

    while (text[start] != '\0' && isspace((unsigned char)text[start]))
        start++;

    memmove(text, text + start, strlen(text + start) + 1);

    int end = (int)strlen(text);

    while (end > 0 && isspace((unsigned char)text[end - 1]))
        end--;

    text[end] = '\0';
}

int parsePositiveInt(const char *text, int *value)
{
    char copy[64];
    snprintf(copy, sizeof(copy), "%s", text);
    trimText(copy);

    char *end;
    errno = 0;
    long number = strtol(copy, &end, 10);

    if (copy[0] == '\0' || *end != '\0' || errno != 0 ||
        number < 1 || number > 1000000000L)
        return 0;

    *value = (int)number;
    return 1;
}

int parsePositiveFloat(const char *text, float *value)
{
    char copy[64];
    snprintf(copy, sizeof(copy), "%s", text);
    trimText(copy);

    char *end;
    errno = 0;
    float number = strtof(copy, &end);

    if (copy[0] == '\0' || *end != '\0' || errno != 0 ||
        !isfinite(number) || number <= 0)
        return 0;

    *value = number;
    return 1;
}

// ---------------- Saving ----------------

const char *dataFolder(void)
{
    static char folder[512] = "";

    // The data folder is next to the program.
    if (folder[0] == '\0')
        snprintf(folder, sizeof(folder), "%sdata", GetApplicationDirectory());

    return folder;
}

void saveData(void)
{
    if (!saveAllData(dataFolder()))
        showMessage("Error", "The data files could not be saved.");
}

// ---------------- Message dialogs ----------------

#define MAX_MESSAGES 5

typedef struct
{
    char title[80];
    char text[600];
    bool isQuestion;
    void (*onYes)(int value);
    int value;
} Message;

static Message messages[MAX_MESSAGES];
static int messageCount = 0;

static void addMessage(const char *title, const char *text, bool isQuestion,
                       void (*onYes)(int), int value)
{
    if (messageCount >= MAX_MESSAGES)
        return;

    Message *m = &messages[messageCount];

    snprintf(m->title, sizeof(m->title), "%s", title);
    snprintf(m->text, sizeof(m->text), "%s", text);
    m->isQuestion = isQuestion;
    m->onYes = onYes;
    m->value = value;

    messageCount++;
}

void showMessage(const char *title, const char *message)
{
    addMessage(title, message, false, NULL, 0);
}

void showConfirm(const char *title, const char *message,
                 void (*onYes)(int value), int value)
{
    addMessage(title, message, true, onYes, value);
}

bool messageOpen(void)
{
    return messageCount > 0;
}

static void closeFirstMessage(void)
{
    for (int i = 1; i < messageCount; i++)
        messages[i - 1] = messages[i];

    messageCount--;
}

void drawMessage(void)
{
    if (messageCount == 0)
        return;

    Message *m = &messages[0];

    // Box size: widest line and number of lines.
    int lines = 1;
    int widest = 0;
    char line[600];
    const char *start = m->text;

    for (const char *p = m->text; ; p++)
    {
        if (*p == '\n' || *p == '\0')
        {
            snprintf(line, sizeof(line), "%.*s", (int)(p - start), start);

            int width = MeasureText(line, GuiGetStyle(DEFAULT, TEXT_SIZE));
            if (width > widest)
                widest = width;

            if (*p == '\0')
                break;

            lines++;
            start = p + 1;
        }
    }

    float width = (float)(widest + 60);
    if (width < 260)
        width = 260;

    float height = (float)(lines * 16 + 90);

    Rectangle bounds = {
        (WINDOW_WIDTH - width) / 2, (WINDOW_HEIGHT - height) / 2,
        width, height
    };

    // Fade the window behind the message box.
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(RAYWHITE, 0.7f));

    int button = -1;

    // Returns 1 when clicked; button = 0 (X), 1 or 2.
    if (GuiMessageBox(bounds, m->title, m->text,
                      m->isQuestion ? "Yes;No" : "OK", &button) == 1)
    {
        // Copy first: the action may add another message.
        bool confirmed = m->isQuestion && button == 1;
        void (*onYes)(int) = m->onYes;
        int value = m->value;

        closeFirstMessage();

        if (confirmed && onYes != NULL)
            onYes(value);
    }
}
