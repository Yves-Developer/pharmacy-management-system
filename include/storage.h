#ifndef STORAGE_H
#define STORAGE_H

// Files are plain text, one record per line, fields separated by '|'.
// The first line of each file is a comment (starts with '#').
//
//   medicines.txt  id|name|category|price|qty|batch|mfgD|mfgM|mfgY|expD|expM|expY|rx
//   suppliers.txt  id|name|phone|medicineIds (comma separated)
//   sales.txt      saleId|medId|medName|customer|rxNo|qty|unitPrice|subtotal|
//                  discount%|reason|total|D|M|Y|time|soldBy
//   history.txt    D|M|Y|time|user|action|medId|details

// Load all files. A missing file just means an empty list.
void loadAllData(const char *folder);

// Save every list. Returns 1 if all files were written.
int saveAllData(const char *folder);

#endif
