#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MAKE_FOLDER(path) _mkdir(path)
#else
#define MAKE_FOLDER(path) mkdir(path, 0755)
#endif

#include "../include/storage.h"
#include "../include/medicine.h"
#include "../include/supplier.h"
#include "../include/sales.h"
#include "../include/history.h"

#define LINE_LENGTH 1024
#define MAX_FIELDS 20

// ---------------- Helpers ----------------

static FILE *openFile(const char *folder, const char *name, const char *mode)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", folder, name);
    return fopen(path, mode);
}

// Write text, replacing '|' and new lines with spaces.
static void writeText(FILE *file, const char *text)
{
    for (int i = 0; text[i] != '\0'; i++)
    {
        char c = text[i];

        if (c == '|' || c == '\n' || c == '\r')
            c = ' ';

        fputc(c, file);
    }
}

// Split a line at each '|' (empty fields are kept). Returns the field count.
static int splitFields(char *line, char *fields[])
{
    int count = 0;

    line[strcspn(line, "\r\n")] = '\0';   // remove the line ending
    fields[count++] = line;

    for (char *p = line; *p != '\0' && count < MAX_FIELDS; p++)
    {
        if (*p == '|')
        {
            *p = '\0';
            fields[count++] = p + 1;
        }
    }

    return count;
}

static void copyField(char *target, int size, const char *field)
{
    snprintf(target, size, "%s", field);
}

// ---------------- Save ----------------

static int saveMedicines(const char *folder)
{
    FILE *file = openFile(folder, "medicines.txt", "w");

    if (file == NULL)
        return 0;

    fprintf(file, "# id|name|category|price|qty|batch|mfgD|mfgM|mfgY|"
                  "expD|expM|expY|prescriptionOnly\n");

    Medicine m;

    for (int i = 0; getMedicineAt(i, &m); i++)
    {
        fprintf(file, "%d|", m.id);
        writeText(file, m.name);
        fprintf(file, "|%s|%.2f|%d|", m.category, m.price, m.quantity);
        writeText(file, m.batchNumber);
        fprintf(file, "|%d|%d|%d|%d|%d|%d|%d\n",
                m.manufactureDay, m.manufactureMonth, m.manufactureYear,
                m.expiryDay, m.expiryMonth, m.expiryYear,
                m.prescriptionOnly);
    }

    fclose(file);
    return 1;
}

static int saveSuppliers(const char *folder)
{
    FILE *file = openFile(folder, "suppliers.txt", "w");

    if (file == NULL)
        return 0;

    fprintf(file, "# id|name|phone|medicineIds\n");

    Supplier s;
    char list[300];

    for (int i = 0; getSupplierAt(i, &s); i++)
    {
        formatMedicineIdList(&s, list, sizeof(list));

        fprintf(file, "%d|", s.id);
        writeText(file, s.name);
        fprintf(file, "|%s|%s\n", s.phone, list);
    }

    fclose(file);
    return 1;
}

static int saveSales(const char *folder)
{
    FILE *file = openFile(folder, "sales.txt", "w");

    if (file == NULL)
        return 0;

    fprintf(file, "# saleId|medId|medName|customer|rxNo|qty|unitPrice|"
                  "subtotal|discount|reason|total|D|M|Y|time|soldBy\n");

    Sale s;

    for (int i = 0; getSaleAt(i, &s); i++)
    {
        fprintf(file, "%d|%d|", s.saleId, s.medicineId);
        writeText(file, s.medicineName);
        fputc('|', file);
        writeText(file, s.customerName);
        fputc('|', file);
        writeText(file, s.prescriptionNumber);
        fprintf(file, "|%d|%.2f|%.2f|%.2f|%s|%.2f|%d|%d|%d|%s|",
                s.quantity, s.unitPrice, s.subtotal,
                s.discountPercent, s.discountReason, s.total,
                s.day, s.month, s.year, s.time);
        writeText(file, s.soldBy);
        fputc('\n', file);
    }

    fclose(file);
    return 1;
}

static int saveHistory(const char *folder)
{
    FILE *file = openFile(folder, "history.txt", "w");

    if (file == NULL)
        return 0;

    fprintf(file, "# D|M|Y|time|user|action|medId|details\n");

    HistoryEntry h;

    for (int i = 0; getHistoryAt(i, &h); i++)
    {
        fprintf(file, "%d|%d|%d|%s|", h.day, h.month, h.year, h.time);
        writeText(file, h.user);
        fprintf(file, "|%s|%d|", h.action, h.medicineId);
        writeText(file, h.details);
        fputc('\n', file);
    }

    fclose(file);
    return 1;
}

int saveAllData(const char *folder)
{
    // Create the folder if needed.
    MAKE_FOLDER(folder);

    int ok = 1;

    ok &= saveMedicines(folder);
    ok &= saveSuppliers(folder);
    ok &= saveSales(folder);
    ok &= saveHistory(folder);

    return ok;
}

// ---------------- Load ----------------

static void loadMedicines(const char *folder)
{
    FILE *file = openFile(folder, "medicines.txt", "r");

    if (file == NULL)
        return;   // no file yet: start empty

    char line[LINE_LENGTH];
    char *f[MAX_FIELDS];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '#' || splitFields(line, f) != 13)
            continue;   // comment or damaged line

        Medicine m = {0};

        m.id = atoi(f[0]);
        copyField(m.name, sizeof(m.name), f[1]);
        copyField(m.category, sizeof(m.category), f[2]);
        m.price = (float)atof(f[3]);
        m.quantity = atoi(f[4]);
        copyField(m.batchNumber, sizeof(m.batchNumber), f[5]);
        m.manufactureDay = atoi(f[6]);
        m.manufactureMonth = atoi(f[7]);
        m.manufactureYear = atoi(f[8]);
        m.expiryDay = atoi(f[9]);
        m.expiryMonth = atoi(f[10]);
        m.expiryYear = atoi(f[11]);
        m.prescriptionOnly = atoi(f[12]);

        restoreMedicine(m);
    }

    fclose(file);
}

static void loadSuppliers(const char *folder)
{
    FILE *file = openFile(folder, "suppliers.txt", "r");

    if (file == NULL)
        return;

    char line[LINE_LENGTH];
    char *f[MAX_FIELDS];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '#' || splitFields(line, f) != 4)
            continue;

        Supplier s = {0};

        s.id = atoi(f[0]);
        copyField(s.name, sizeof(s.name), f[1]);
        copyField(s.phone, sizeof(s.phone), f[2]);
        s.medicineCount = parseMedicineIdList(f[3], s.medicineIds,
                                              MAX_SUPPLIED_MEDICINES);

        if (s.medicineCount < 0)
            s.medicineCount = 0;

        restoreSupplier(s);
    }

    fclose(file);
}

static void loadSales(const char *folder)
{
    FILE *file = openFile(folder, "sales.txt", "r");

    if (file == NULL)
        return;

    char line[LINE_LENGTH];
    char *f[MAX_FIELDS];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '#' || splitFields(line, f) != 16)
            continue;

        Sale s = {0};

        s.saleId = atoi(f[0]);
        s.medicineId = atoi(f[1]);
        copyField(s.medicineName, sizeof(s.medicineName), f[2]);
        copyField(s.customerName, sizeof(s.customerName), f[3]);
        copyField(s.prescriptionNumber, sizeof(s.prescriptionNumber), f[4]);
        s.quantity = atoi(f[5]);
        s.unitPrice = (float)atof(f[6]);
        s.subtotal = atof(f[7]);
        s.discountPercent = (float)atof(f[8]);
        copyField(s.discountReason, sizeof(s.discountReason), f[9]);
        s.total = atof(f[10]);
        s.day = atoi(f[11]);
        s.month = atoi(f[12]);
        s.year = atoi(f[13]);
        copyField(s.time, sizeof(s.time), f[14]);
        copyField(s.soldBy, sizeof(s.soldBy), f[15]);

        restoreSale(s);
    }

    fclose(file);
}

static void loadHistory(const char *folder)
{
    FILE *file = openFile(folder, "history.txt", "r");

    if (file == NULL)
        return;

    char line[LINE_LENGTH];
    char *f[MAX_FIELDS];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '#' || splitFields(line, f) != 8)
            continue;

        HistoryEntry h = {0};

        h.day = atoi(f[0]);
        h.month = atoi(f[1]);
        h.year = atoi(f[2]);
        copyField(h.time, sizeof(h.time), f[3]);
        copyField(h.user, sizeof(h.user), f[4]);
        copyField(h.action, sizeof(h.action), f[5]);
        h.medicineId = atoi(f[6]);
        copyField(h.details, sizeof(h.details), f[7]);

        restoreHistoryEntry(h);
    }

    fclose(file);
}

void loadAllData(const char *folder)
{
    clearMedicines();
    clearSuppliers();
    clearSales();
    clearHistory();

    loadMedicines(folder);
    loadSuppliers(folder);
    loadSales(folder);
    loadHistory(folder);
}
