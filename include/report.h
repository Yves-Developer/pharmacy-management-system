#ifndef REPORT_H
#define REPORT_H

#define REPORT_SIZE 160000

// Build the report text (shown on screen and exported).
void buildReport(char *buffer, int size);

// Build the report and write it to a text file.
int exportReport(const char *path, char *error, int errorSize);

#endif
