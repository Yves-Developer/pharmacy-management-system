#ifndef SALES_H
#define SALES_H

#include "medicine.h"
#include "history.h"

#define MAX_SALES 2000
#define CUSTOMER_LENGTH 100
#define PRESCRIPTION_LENGTH 50
#define REASON_LENGTH 40

#define MIN_DISCOUNT 5.0f
#define MAX_DISCOUNT 15.0f

#define DISCOUNT_REASON_COUNT 3
extern const char *DISCOUNT_REASONS[DISCOUNT_REASON_COUNT];

typedef struct
{
    int saleId;

    int medicineId;
    char medicineName[NAME_LENGTH];   // kept even if the medicine is removed

    char customerName[CUSTOMER_LENGTH];
    char prescriptionNumber[PRESCRIPTION_LENGTH];   // empty if not needed

    int quantity;
    float unitPrice;
    double subtotal;          // quantity x unit price

    float discountPercent;    // 0 = no discount
    char discountReason[REASON_LENGTH];
    double total;             // subtotal minus discount

    int day;
    int month;
    int year;
    char time[9];

    char soldBy[USER_LENGTH];

} Sale;

// Sell a medicine. On success, *receipt holds the sale.
int sellMedicine(
    int medicineId,
    int quantity,
    const char *customerName,
    const char *prescriptionNumber,
    Sale *receipt,
    char *error, int errorSize);

// Apply a 5-15 % discount. On error the sale is not changed.
int applyDiscount(
    int saleId,
    float percent,
    const char *reason,
    Sale *updated,
    char *error, int errorSize);

int getSaleCount(void);
int getSaleAt(int index, Sale *sale);

// Sum of all sale totals (after discounts).
double getTotalSalesAmount(void);

// Total and number of sales made today.
void getTodaySalesSummary(double *amount, int *count);

// Medicine with the highest quantity sold. Returns 0 if no sales.
int getBestSellingMedicine(
    int *medicineId,
    char *name, int nameSize,
    int *quantitySold);

// Used only by storage.c.
int restoreSale(Sale sale);
void clearSales(void);

#endif
