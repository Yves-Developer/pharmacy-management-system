#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/supplier.h"
#include "../include/history.h"

static Supplier suppliers[MAX_SUPPLIERS];
static int supplierCount = 0;

static int findSupplierIndex(int id)
{
    for (int i = 0; i < supplierCount; i++)
    {
        if (suppliers[i].id == id)
            return i;
    }

    return -1;
}

int isValidPhone(const char *phone)
{
    int i = 0;

    if (phone[0] == '+')
        i = 1;

    if (phone[i] == '\0')
        return 0;

    for (; phone[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)phone[i]))
            return 0;
    }

    return 1;
}

int parseMedicineIdList(const char *text, int *ids, int maxIds)
{
    int count = 0;
    const char *p = text;

    while (*p != '\0')
    {
        // Skip spaces and commas between numbers.
        if (*p == ' ' || *p == ',')
        {
            p++;
            continue;
        }

        char *end;
        long value = strtol(p, &end, 10);

        // Not a number, or a non-positive ID.
        if (end == p || value <= 0)
            return -1;

        // Characters glued to the number, e.g. "12a".
        if (*end != '\0' && *end != ',' && *end != ' ')
            return -1;

        if (count >= maxIds)
            return -1;

        ids[count] = (int)value;
        count++;
        p = end;
    }

    return count;
}

void formatMedicineIdList(const Supplier *supplier, char *buffer, int size)
{
    int used = 0;
    buffer[0] = '\0';

    for (int i = 0; i < supplier->medicineCount && used < size; i++)
    {
        used += snprintf(buffer + used, size - used, "%s%d",
                         (i > 0) ? ", " : "", supplier->medicineIds[i]);
    }
}

int addSupplier(Supplier supplier, char *error, int errorSize)
{
    if (supplierCount >= MAX_SUPPLIERS)
    {
        snprintf(error, errorSize, "Supplier storage is full.");
        return 0;
    }

    if (supplier.id <= 0)
    {
        snprintf(error, errorSize, "Supplier ID must be positive.");
        return 0;
    }

    if (findSupplierIndex(supplier.id) != -1)
    {
        snprintf(error, errorSize, "This supplier ID already exists.");
        return 0;
    }

    if (supplier.name[0] == '\0')
    {
        snprintf(error, errorSize, "Supplier name cannot be empty.");
        return 0;
    }

    if (!isValidPhone(supplier.phone))
    {
        snprintf(error, errorSize, "Phone must contain digits only.");
        return 0;
    }

    if (supplier.medicineCount <= 0)
    {
        snprintf(error, errorSize,
                 "Enter at least one medicine ID the supplier provides.");
        return 0;
    }

    suppliers[supplierCount] = supplier;
    supplierCount++;

    char list[150];
    char details[DETAILS_LENGTH];
    formatMedicineIdList(&supplier, list, sizeof(list));
    snprintf(details, sizeof(details),
             "Added supplier #%d %s (%s), supplies: %s",
             supplier.id, supplier.name, supplier.phone, list);
    logAction("SUPPLIER ADD", 0, details);

    return 1;
}

int removeSupplier(int id, char *error, int errorSize)
{
    int index = findSupplierIndex(id);

    if (index == -1)
    {
        snprintf(error, errorSize, "Supplier ID %d was not found.", id);
        return 0;
    }

    char details[DETAILS_LENGTH];
    snprintf(details, sizeof(details), "Removed supplier #%d %s",
             id, suppliers[index].name);

    for (int i = index; i < supplierCount - 1; i++)
        suppliers[i] = suppliers[i + 1];

    supplierCount--;

    logAction("SUPPLIER REMOVE", 0, details);

    return 1;
}

int getSupplierCount(void)
{
    return supplierCount;
}

int getSupplierAt(int index, Supplier *supplier)
{
    if (index < 0 || index >= supplierCount)
        return 0;

    *supplier = suppliers[index];
    return 1;
}

int restoreSupplier(Supplier supplier)
{
    if (supplierCount >= MAX_SUPPLIERS ||
        supplier.id <= 0 ||
        findSupplierIndex(supplier.id) != -1)
        return 0;

    suppliers[supplierCount] = supplier;
    supplierCount++;

    return 1;
}

void clearSuppliers(void)
{
    supplierCount = 0;
}
