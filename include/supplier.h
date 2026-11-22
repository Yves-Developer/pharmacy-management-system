#ifndef SUPPLIER_H
#define SUPPLIER_H

#define MAX_SUPPLIERS 200
#define SUPPLIER_NAME_LENGTH 100
#define PHONE_LENGTH 20
#define MAX_SUPPLIED_MEDICINES 20

typedef struct
{
    int id;
    char name[SUPPLIER_NAME_LENGTH];
    char phone[PHONE_LENGTH];

    int medicineIds[MAX_SUPPLIED_MEDICINES];   // medicines this supplier provides
    int medicineCount;

} Supplier;

// Checks: unique ID > 0, name, phone digits only, at least one medicine.
int addSupplier(Supplier supplier, char *error, int errorSize);

int removeSupplier(int id, char *error, int errorSize);

int getSupplierCount(void);
int getSupplierAt(int index, Supplier *supplier);

// Phone: digits only, optional leading '+'.
int isValidPhone(const char *phone);

// Turn text like "101, 102,105" into a list of IDs.
// Returns the number of IDs, or -1 if the text is invalid.
int parseMedicineIdList(const char *text, int *ids, int maxIds);

// Write the IDs as "101, 102, 105" (for tables and files).
void formatMedicineIdList(const Supplier *supplier, char *buffer, int size);

// Used only by storage.c.
int restoreSupplier(Supplier supplier);
void clearSuppliers(void);

#endif
