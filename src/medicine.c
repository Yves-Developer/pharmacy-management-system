#include <stdio.h>
#include <string.h>

#include "../include/medicine.h"
#include "../include/date_utils.h"
#include "../include/history.h"

static Medicine medicines[MAX_MEDICINES];
static int medicineCount = 0;

// Register a medicine after checking every rule.
int addMedicine(Medicine medicine, char *error, int errorSize)
{
    if (medicineCount >= MAX_MEDICINES)
    {
        snprintf(error, errorSize, "Medicine storage is full.");
        return 0;
    }

    if (medicine.id <= 0)
    {
        snprintf(error, errorSize, "Medicine ID must be positive.");
        return 0;
    }

    if (findMedicineIndex(medicine.id) != -1)
    {
        snprintf(error, errorSize, "This medicine ID already exists.");
        return 0;
    }

    if (medicine.name[0] == '\0')
    {
        snprintf(error, errorSize, "Medicine name cannot be empty.");
        return 0;
    }

    if (medicine.batchNumber[0] == '\0')
    {
        snprintf(error, errorSize, "Batch number cannot be empty.");
        return 0;
    }

    if (medicine.price <= 0)
    {
        snprintf(error, errorSize, "Price must be greater than zero.");
        return 0;
    }

    if (medicine.quantity <= 0)
    {
        snprintf(error, errorSize, "Quantity must be greater than zero.");
        return 0;
    }

    if (!isValidDate(medicine.manufactureDay,
                     medicine.manufactureMonth,
                     medicine.manufactureYear))
    {
        snprintf(error, errorSize, "Manufacture date is invalid.");
        return 0;
    }

    if (!isValidDate(medicine.expiryDay,
                     medicine.expiryMonth,
                     medicine.expiryYear))
    {
        snprintf(error, errorSize, "Expiry date is invalid.");
        return 0;
    }

    if (compareDates(medicine.expiryDay, medicine.expiryMonth,
                     medicine.expiryYear,
                     medicine.manufactureDay, medicine.manufactureMonth,
                     medicine.manufactureYear) <= 0)
    {
        snprintf(error, errorSize,
                 "Expiry date must be after the manufacture date.");
        return 0;
    }

    if (daysFromToday(medicine.expiryDay,
                      medicine.expiryMonth,
                      medicine.expiryYear) <= 0)
    {
        snprintf(error, errorSize, "Expiry date must be later than today.");
        return 0;
    }

    medicines[medicineCount] = medicine;
    medicineCount++;

    char details[DETAILS_LENGTH];
    snprintf(details, sizeof(details),
             "Added %s (%s), qty %d, price %.2f, batch %s",
             medicine.name, medicine.category, medicine.quantity,
             medicine.price, medicine.batchNumber);
    logAction("ADD", medicine.id, details);

    return 1;
}

int getMedicineCount(void)
{
    return medicineCount;
}

int getMedicineAt(int index, Medicine *medicine)
{
    if (index < 0 || index >= medicineCount)
        return 0;

    *medicine = medicines[index];
    return 1;
}

int findMedicineIndex(int id)
{
    for (int i = 0; i < medicineCount; i++)
    {
        if (medicines[i].id == id)
            return i;
    }

    return -1;
}

int getMedicineById(int id, Medicine *medicine)
{
    int index = findMedicineIndex(id);

    if (index == -1)
        return 0;

    *medicine = medicines[index];
    return 1;
}

// ---------------- Status checks ----------------

int daysUntilExpiry(Medicine medicine)
{
    return daysFromToday(medicine.expiryDay,
                         medicine.expiryMonth,
                         medicine.expiryYear);
}

// Expired = the expiry date is before today.
int isMedicineExpired(Medicine medicine)
{
    return daysUntilExpiry(medicine) < 0;
}

int isMedicineExpiringSoon(Medicine medicine)
{
    int days = daysUntilExpiry(medicine);

    return days >= 0 && days <= EXPIRY_WARNING_DAYS;
}

int isMedicineLowStock(Medicine medicine)
{
    return medicine.quantity < LOW_STOCK_LEVEL;
}

// ---------------- Changes ----------------

int updateStock(
    int id,
    int quantityToAdd,
    const char *newBatch,
    int expiryDay, int expiryMonth, int expiryYear,
    char *error, int errorSize)
{
    int index = findMedicineIndex(id);

    if (index == -1)
    {
        snprintf(error, errorSize, "Medicine ID %d was not found.", id);
        return 0;
    }

    if (quantityToAdd <= 0)
    {
        snprintf(error, errorSize, "Quantity to add must be greater than zero.");
        return 0;
    }

    if (newBatch == NULL || newBatch[0] == '\0')
    {
        snprintf(error, errorSize, "Batch number cannot be empty.");
        return 0;
    }

    if (!isValidDate(expiryDay, expiryMonth, expiryYear))
    {
        snprintf(error, errorSize, "New expiry date is invalid.");
        return 0;
    }

    if (daysFromToday(expiryDay, expiryMonth, expiryYear) < 0)
    {
        snprintf(error, errorSize, "New expiry date cannot be in the past.");
        return 0;
    }

    Medicine *medicine = &medicines[index];

    if (compareDates(expiryDay, expiryMonth, expiryYear,
                     medicine->manufactureDay, medicine->manufactureMonth,
                     medicine->manufactureYear) <= 0)
    {
        snprintf(error, errorSize,
                 "New expiry date must be after the manufacture date.");
        return 0;
    }

    int oldQuantity = medicine->quantity;

    medicine->quantity += quantityToAdd;
    snprintf(medicine->batchNumber, sizeof(medicine->batchNumber),
             "%s", newBatch);
    medicine->expiryDay = expiryDay;
    medicine->expiryMonth = expiryMonth;
    medicine->expiryYear = expiryYear;

    char details[DETAILS_LENGTH];
    snprintf(details, sizeof(details),
             "Stock %d -> %d, batch %s, expiry %02d/%02d/%04d",
             oldQuantity, medicine->quantity, newBatch,
             expiryDay, expiryMonth, expiryYear);
    logAction("UPDATE", id, details);

    return 1;
}

int deductStock(int id, int quantity)
{
    int index = findMedicineIndex(id);

    if (index == -1 || quantity <= 0 ||
        medicines[index].quantity < quantity)
        return 0;

    medicines[index].quantity -= quantity;
    return 1;
}

// Remove the medicine at a position by shifting the rest left.
static void removeAt(int index)
{
    for (int i = index; i < medicineCount - 1; i++)
        medicines[i] = medicines[i + 1];

    medicineCount--;
}

int removeMedicine(int id, char *error, int errorSize)
{
    int index = findMedicineIndex(id);

    if (index == -1)
    {
        snprintf(error, errorSize, "Medicine ID %d was not found.", id);
        return 0;
    }

    char details[DETAILS_LENGTH];
    snprintf(details, sizeof(details), "Removed %s (qty %d)%s",
             medicines[index].name, medicines[index].quantity,
             isMedicineExpired(medicines[index]) ? " - expired" : "");

    removeAt(index);
    logAction("REMOVE", id, details);

    return 1;
}

// ---------------- Values ----------------

void calculateStockValue(double *validValue, double *expiredLoss)
{
    *validValue = 0.0;
    *expiredLoss = 0.0;

    for (int i = 0; i < medicineCount; i++)
    {
        double value = (double)medicines[i].quantity * medicines[i].price;

        if (isMedicineExpired(medicines[i]))
            *expiredLoss += value;
        else
            *validValue += value;
    }
}

// ---------------- Loading ----------------

// Add a medicine loaded from file (no checks, no history).
int restoreMedicine(Medicine medicine)
{
    if (medicineCount >= MAX_MEDICINES ||
        medicine.id <= 0 ||
        findMedicineIndex(medicine.id) != -1)
        return 0;

    medicines[medicineCount] = medicine;
    medicineCount++;

    return 1;
}

void clearMedicines(void)
{
    medicineCount = 0;
}
