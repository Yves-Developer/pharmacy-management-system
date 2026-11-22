#include <stdio.h>
#include <string.h>

#include "../include/sales.h"
#include "../include/date_utils.h"

const char *DISCOUNT_REASONS[DISCOUNT_REASON_COUNT] = {
    "Senior citizen",
    "Insurance",
    "Bulk purchase"
};

static Sale sales[MAX_SALES];
static int saleCount = 0;
static int nextSaleId = 1;

int sellMedicine(
    int medicineId,
    int quantity,
    const char *customerName,
    const char *prescriptionNumber,
    Sale *receipt,
    char *error, int errorSize)
{
    Medicine medicine;

    if (saleCount >= MAX_SALES)
    {
        snprintf(error, errorSize, "Sales storage is full.");
        return 0;
    }

    if (!getMedicineById(medicineId, &medicine))
    {
        snprintf(error, errorSize, "Medicine ID %d was not found.", medicineId);
        return 0;
    }

    if (quantity <= 0)
    {
        snprintf(error, errorSize, "Quantity to sell must be greater than zero.");
        return 0;
    }

    if (customerName == NULL || customerName[0] == '\0')
    {
        snprintf(error, errorSize, "Customer name cannot be empty.");
        return 0;
    }

    if (isMedicineExpired(medicine))
    {
        snprintf(error, errorSize, "Expired medicine cannot be sold.");
        return 0;
    }

    if (medicine.prescriptionOnly &&
        (prescriptionNumber == NULL || prescriptionNumber[0] == '\0'))
    {
        snprintf(error, errorSize,
                 "Prescription required for %s.", medicine.name);
        return 0;
    }

    if (medicine.quantity < quantity)
    {
        snprintf(error, errorSize,
                 "Insufficient stock: only %d left.", medicine.quantity);
        return 0;
    }

    // All checks passed: create the sale.
    Sale sale = {0};

    sale.saleId = nextSaleId;
    sale.medicineId = medicine.id;
    snprintf(sale.medicineName, sizeof(sale.medicineName), "%s", medicine.name);
    snprintf(sale.customerName, sizeof(sale.customerName), "%s", customerName);

    if (prescriptionNumber != NULL)
        snprintf(sale.prescriptionNumber, sizeof(sale.prescriptionNumber),
                 "%s", prescriptionNumber);

    sale.quantity = quantity;
    sale.unitPrice = medicine.price;
    sale.subtotal = (double)quantity * medicine.price;
    sale.discountPercent = 0.0f;
    sale.total = sale.subtotal;

    getToday(&sale.day, &sale.month, &sale.year);
    getCurrentTime(sale.time, sizeof(sale.time));
    snprintf(sale.soldBy, sizeof(sale.soldBy), "%s", getCurrentUser());

    deductStock(medicine.id, quantity);

    sales[saleCount] = sale;
    saleCount++;
    nextSaleId++;

    char details[DETAILS_LENGTH];
    snprintf(details, sizeof(details),
             "Sale #%d: %d x %s to %s, total %.2f",
             sale.saleId, quantity, medicine.name, customerName, sale.total);
    logAction("SELL", medicine.id, details);

    *receipt = sale;
    return 1;
}

static int findSaleIndex(int saleId)
{
    for (int i = 0; i < saleCount; i++)
    {
        if (sales[i].saleId == saleId)
            return i;
    }

    return -1;
}

int applyDiscount(
    int saleId,
    float percent,
    const char *reason,
    Sale *updated,
    char *error, int errorSize)
{
    int index = findSaleIndex(saleId);

    if (index == -1)
    {
        snprintf(error, errorSize, "Sale #%d was not found.", saleId);
        return 0;
    }

    if (percent < MIN_DISCOUNT || percent > MAX_DISCOUNT)
    {
        snprintf(error, errorSize,
                 "Discount must be between %.0f%% and %.0f%%.",
                 MIN_DISCOUNT, MAX_DISCOUNT);
        return 0;
    }

    Sale *sale = &sales[index];

    // The discount is always taken from the original amount.
    sale->discountPercent = percent;
    snprintf(sale->discountReason, sizeof(sale->discountReason), "%s", reason);
    sale->total = sale->subtotal * (1.0 - percent / 100.0);

    char details[DETAILS_LENGTH];
    snprintf(details, sizeof(details),
             "Sale #%d: %.1f%% (%s), %.2f -> %.2f",
             saleId, percent, reason, sale->subtotal, sale->total);
    logAction("DISCOUNT", sale->medicineId, details);

    *updated = *sale;
    return 1;
}

int getSaleCount(void)
{
    return saleCount;
}

int getSaleAt(int index, Sale *sale)
{
    if (index < 0 || index >= saleCount)
        return 0;

    *sale = sales[index];
    return 1;
}

double getTotalSalesAmount(void)
{
    double total = 0.0;

    for (int i = 0; i < saleCount; i++)
        total += sales[i].total;

    return total;
}

void getTodaySalesSummary(double *amount, int *count)
{
    int day, month, year;
    getToday(&day, &month, &year);

    *amount = 0.0;
    *count = 0;

    for (int i = 0; i < saleCount; i++)
    {
        if (compareDates(sales[i].day, sales[i].month, sales[i].year,
                         day, month, year) == 0)
        {
            *amount += sales[i].total;
            (*count)++;
        }
    }
}

int getBestSellingMedicine(
    int *medicineId,
    char *name, int nameSize,
    int *quantitySold)
{
    int bestId = 0;
    int bestQuantity = 0;
    const char *bestName = "";

    // Count the total sold for each medicine and keep the highest.
    for (int i = 0; i < saleCount; i++)
    {
        int total = 0;

        for (int j = 0; j < saleCount; j++)
        {
            if (sales[j].medicineId == sales[i].medicineId)
                total += sales[j].quantity;
        }

        if (total > bestQuantity)
        {
            bestQuantity = total;
            bestId = sales[i].medicineId;
            bestName = sales[i].medicineName;
        }
    }

    if (bestQuantity == 0)
        return 0;

    *medicineId = bestId;
    *quantitySold = bestQuantity;
    snprintf(name, nameSize, "%s", bestName);

    return 1;
}

int restoreSale(Sale sale)
{
    if (saleCount >= MAX_SALES)
        return 0;

    sales[saleCount] = sale;
    saleCount++;

    // New sales must not reuse a loaded ID.
    if (sale.saleId >= nextSaleId)
        nextSaleId = sale.saleId + 1;

    return 1;
}

void clearSales(void)
{
    saleCount = 0;
    nextSaleId = 1;
}
