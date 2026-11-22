# Pharmacy Management System

A GUI pharmacy management system written in C with
[raylib](https://www.raylib.com) (window and drawing) and
[raygui](https://github.com/raysan5/raygui) (buttons, text boxes, lists, message boxes).

## Running the ready-made program

| System  | File                       | How to run                                               |
|---------|----------------------------|----------------------------------------------------------|
| Windows | `Pharmacy-Windows.zip`     | Unzip, open the folder, double-click `pharmacy.exe`.     |
| macOS   | `Pharmacy-macOS.zip`       | Unzip, drag `Pharmacy.app` to *Applications*, then right-click it and choose **Open** the first time (the app is not from the App Store). |

No compiler or extra libraries are needed. At start-up, enter your name;
it is recorded with every operation in the transaction history.

Data is saved in a `data` folder next to the program
(on macOS inside `Pharmacy.app/Contents/MacOS/data`). If the folder or the
files do not exist yet, the program starts with empty lists and creates them.

## Project structure

```
main.c                  window, menu buttons, login, main loop
include/                header files (.h)
src/
  medicine.c            medicine list: add, update stock, remove, expiry checks, stock value
  sales.c               selling, receipts, discounts, sales totals, best seller
  supplier.c            supplier list: add, remove
  history.c             transaction history (static list) and filters
  storage.c             save / load all data to text files
  report.c              summary report text and export to .txt
  date_utils.c          date checks and today's date
  ui/                   the GUI (raygui), one file per screen:
    add_medicine.c        1. Add Medicine
    view_medicines.c      2. View Medicines (coloured table)
    update_stock.c        3. Update Stock
    sell.c                4. Sell Medicine (receipt)
    check_stock.c         5. Check Stock and Expiry, 6. Calculate Stock Value
    discount.c            7. Apply Discount
    remove_medicine.c     8. Remove Medicine
    suppliers.c           8. Manage Suppliers
    history.c             9. Transaction History
    report.c              10. Report
    common.c              text box helper, message boxes, number checks, saving
    raygui_impl.c         compiles the raygui library
data/                   data files (medicines, suppliers, sales, history)
packaging/Info.plist    macOS app description
```

## Data files

Plain text, one record per line, fields separated by `|`.
The first line of each file is a comment with the column names.

| File            | Fields |
|-----------------|--------|
| `medicines.txt` | id, name, category, price, quantity, batch, manufacture day/month/year, expiry day/month/year, prescription-only (1/0) |
| `suppliers.txt` | id, name, phone, medicine IDs (comma separated) |
| `sales.txt`     | sale id, medicine id, medicine name, customer, prescription no., quantity, unit price, subtotal, discount %, reason, total, day, month, year, time, user |
| `history.txt`   | day, month, year, time, user, action, medicine id, details |

## Building from source

The program needs a C compiler and raylib 5.0 or newer.
`raygui.h` is already included in `include/`.

### macOS (MacPorts)

```sh
sudo port install raylib pkgconfig
make            # builds ./pharmacy
make run        # builds and starts it
make macapp     # builds dist/Pharmacy-macOS.zip
```

(With Homebrew: `brew install raylib pkg-config`; then the same `make`
commands. `make macapp` expects the library names of raylib 6.2.)

### Linux

```sh
sudo apt install build-essential pkg-config libraylib-dev   # or build raylib from source
make
make run
```

### Windows (MinGW-w64)

1. Install MinGW-w64 (for example with [MSYS2](https://www.msys2.org):
   `pacman -S mingw-w64-x86_64-gcc make zip`).
2. Download `raylib-x.x_win64_mingw-w64.zip` from the
   [raylib releases](https://github.com/raysan5/raylib/releases) and unzip it
   into the project folder as `raylib_win64`.
3. In the project folder run:

```sh
make windows WIN_CC=gcc
```

This creates `build/Pharmacy-Windows/pharmacy.exe` and `dist/Pharmacy-Windows.zip`.
The program is linked statically, so the `.exe` runs on its own.

Without `make`, the same build in one command:

```sh
gcc -std=c99 -O2 -Iinclude -Iraylib_win64/include main.c src/*.c src/ui/*.c -o pharmacy.exe ^
    -Lraylib_win64/lib -lraylib -lopengl32 -lgdi32 -lwinmm -static -mwindows
```

### Source package

`make source` creates `dist/Pharmacy-source.zip` with all `.c`/`.h` files,
the data files, the Makefile and this README.
