#ifndef MEDICINE_H
#define MEDICINE_H

#define NAME_LENGTH 100
#define CATEGORY_LENGTH 30
#define BATCH_LENGTH 50
#define MAX_MEDICINES 500

#define LOW_STOCK_LEVEL 10       // re-order level
#define EXPIRY_WARNING_DAYS 30   // "expiring soon" window

typedef struct
{
    int id;

    char name[NAME_LENGTH];
    char category[CATEGORY_LENGTH];   // Tablet, Syrup, Injection, Ointment

    float price;
    int quantity;

    char batchNumber[BATCH_LENGTH];

    int manufactureDay;
    int manufactureMonth;
    int manufactureYear;

    int expiryDay;
    int expiryMonth;
    int expiryYear;

    int prescriptionOnly;   // 1 = a prescription number is needed to sell

} Medicine;

// ---- Registration and lookup ----

int addMedicine(Medicine medicine, char *error, int errorSize);

int getMedicineCount(void);

int getMedicineAt(int index, Medicine *medicine);

// Returns the position in the list, or -1 if not found.
int findMedicineIndex(int id);

// Returns 1 and fills *medicine if found.
int getMedicineById(int id, Medicine *medicine);

// ---- Status checks ----

int daysUntilExpiry(Medicine medicine);   // negative = already expired

int isMedicineExpired(Medicine medicine);

int isMedicineExpiringSoon(Medicine medicine);   // 0..30 days left

int isMedicineLowStock(Medicine medicine);       // quantity < 10

// ---- Changes ----

// Add quantity and replace batch number and expiry date.
int updateStock(
    int id,
    int quantityToAdd,
    const char *newBatch,
    int expiryDay, int expiryMonth, int expiryYear,
    char *error, int errorSize);

// Remove quantity after a sale. Used by sales.c.
int deductStock(int id, int quantity);

int removeMedicine(int id, char *error, int errorSize);

// ---- Values ----

// validValue = sum(qty x price) of non-expired medicines.
// expiredLoss = the same sum for expired medicines.
void calculateStockValue(double *validValue, double *expiredLoss);

// ---- Used only by storage.c when loading from file ----

int restoreMedicine(Medicine medicine);
void clearMedicines(void);

#endif
