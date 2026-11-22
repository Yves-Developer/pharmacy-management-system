#include <stdio.h>
#include <string.h>

#include "../include/report.h"
#include "../include/medicine.h"
#include "../include/sales.h"
#include "../include/date_utils.h"

// Build the report text. n is the length written so far.
void buildReport(char *buffer, int size)
{
    int n = 0;
    Medicine m;

    int day, month, year;
    char time[9];
    getToday(&day, &month, &year);
    getCurrentTime(time, sizeof(time));

    // ---- Stock totals ----
    int totalQuantity = 0;

    for (int i = 0; getMedicineAt(i, &m); i++)
        totalQuantity += m.quantity;

    double stockValue, expiredLoss;
    calculateStockValue(&stockValue, &expiredLoss);

    // ---- Sales totals ----
    double todayAmount;
    int todayCount;
    getTodaySalesSummary(&todayAmount, &todayCount);

    n += snprintf(buffer + n, size - n,
                  "PHARMACY SUMMARY REPORT\n"
                  "Date: %02d/%02d/%04d %s\n\n",
                  day, month, year, time);

    n += snprintf(buffer + n, size - n,
                  "Total medicines: %d\n"
                  "Total quantity in stock: %d\n"
                  "Stock value: %.2f\n"
                  "Expired stock loss: %.2f\n\n",
                  getMedicineCount(), totalQuantity, stockValue, expiredLoss);

    n += snprintf(buffer + n, size - n,
                  "Total sales amount: %.2f\n"
                  "Number of sales today: %d\n",
                  getTotalSalesAmount(), todayCount);

    int bestId, bestQuantity;
    char bestName[NAME_LENGTH];

    if (getBestSellingMedicine(&bestId, bestName, sizeof(bestName), &bestQuantity))
        n += snprintf(buffer + n, size - n,
                      "Best-selling medicine: %s (%d sold)\n\n",
                      bestName, bestQuantity);
    else
        n += snprintf(buffer + n, size - n, "Best-selling medicine: none\n\n");

    // ---- Medicines below the re-order level ----
    n += snprintf(buffer + n, size - n,
                  "Medicines below re-order level (%d units):\n", LOW_STOCK_LEVEL);

    for (int i = 0; getMedicineAt(i, &m); i++)
    {
        if (isMedicineLowStock(m))
            n += snprintf(buffer + n, size - n, "  %d %s - quantity %d\n",
                          m.id, m.name, m.quantity);
    }

    // ---- Medicines expiring within 30 days ----
    n += snprintf(buffer + n, size - n,
                  "\nMedicines expiring within %d days:\n", EXPIRY_WARNING_DAYS);

    for (int i = 0; getMedicineAt(i, &m); i++)
    {
        if (isMedicineExpiringSoon(m))
            n += snprintf(buffer + n, size - n, "  %d %s - expires %02d/%02d/%04d\n",
                          m.id, m.name, m.expiryDay, m.expiryMonth, m.expiryYear);
    }
}

int exportReport(const char *path, char *error, int errorSize)
{
    static char report[REPORT_SIZE];
    buildReport(report, sizeof(report));

    FILE *file = fopen(path, "w");

    if (file == NULL)
    {
        snprintf(error, errorSize, "Could not create %s.", path);
        return 0;
    }

    fputs(report, file);
    fclose(file);

    return 1;
}
