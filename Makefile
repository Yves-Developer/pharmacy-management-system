# Pharmacy Management System - build file
#
#   make           build the program  ./pharmacy
#   make run       build and start it
#   make macapp    make dist/Pharmacy-macOS.zip (Pharmacy.app, runs without a compiler)
#   make windows   make dist/Pharmacy-Windows.zip (pharmacy.exe, needs MinGW + raylib for Windows)
#   make source    make dist/Pharmacy-source.zip (source code, data files, build instructions)
#   make clean     delete build output (keeps the packages in dist/)

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
RAYLIB  = $(shell pkg-config --cflags --libs raylib)

# Data-layer modules (no GUI code).
CORE = src/date_utils.c src/history.c src/medicine.c src/sales.c \
       src/supplier.c src/storage.c src/report.c

# GUI screens and helpers (need raylib).
UI = $(wildcard src/ui/*.c)

HEADERS = $(wildcard include/*.h)

pharmacy: main.c $(CORE) $(UI) $(HEADERS)
	$(CC) $(CFLAGS) -O2 main.c $(CORE) $(UI) -o pharmacy $(RAYLIB) -lm

run: pharmacy
	./pharmacy

# ---------------------------------------------------------------------
# macOS app: the raylib and GLFW libraries are copied into the app and
# the program is told to load them from there instead of /opt/local/lib.
# ---------------------------------------------------------------------
LIBDIR = $(shell pkg-config --variable=libdir raylib)
APP    = build/Pharmacy.app
FW     = @executable_path/../Frameworks

macapp: pharmacy
	rm -rf $(APP)
	mkdir -p $(APP)/Contents/MacOS $(APP)/Contents/Frameworks dist
	cp packaging/Info.plist $(APP)/Contents/
	cp pharmacy $(APP)/Contents/MacOS/
	cp $(LIBDIR)/libraylib.620.dylib $(LIBDIR)/libglfw.3.dylib $(APP)/Contents/Frameworks/
	chmod u+w $(APP)/Contents/Frameworks/*.dylib
	install_name_tool -change $(LIBDIR)/libraylib.620.dylib $(FW)/libraylib.620.dylib $(APP)/Contents/MacOS/pharmacy
	install_name_tool -id $(FW)/libraylib.620.dylib $(APP)/Contents/Frameworks/libraylib.620.dylib
	install_name_tool -change $(LIBDIR)/libglfw.3.dylib $(FW)/libglfw.3.dylib $(APP)/Contents/Frameworks/libraylib.620.dylib
	install_name_tool -id $(FW)/libglfw.3.dylib $(APP)/Contents/Frameworks/libglfw.3.dylib
	codesign --force --sign - $(APP)/Contents/Frameworks/*.dylib
	codesign --force --sign - $(APP)
	rm -f dist/Pharmacy-macOS.zip
	cd build && zip -qry ../dist/Pharmacy-macOS.zip Pharmacy.app

# ---------------------------------------------------------------------
# Windows .exe (static: one file, no DLLs needed).
# RAYLIB_WIN is the unzipped "raylib-x.x_win64_mingw-w64" release folder.
# On Windows itself use WIN_CC=gcc.
# ---------------------------------------------------------------------
WIN_CC     = x86_64-w64-mingw32-gcc
RAYLIB_WIN = raylib_win64

windows: main.c $(CORE) $(UI) $(HEADERS)
	rm -rf build/Pharmacy-Windows
	mkdir -p build/Pharmacy-Windows dist
	$(WIN_CC) $(CFLAGS) -O2 -I$(RAYLIB_WIN)/include main.c $(CORE) $(UI) \
		-o build/Pharmacy-Windows/pharmacy.exe -L$(RAYLIB_WIN)/lib \
		-lraylib -lopengl32 -lgdi32 -lwinmm -static -mwindows
	rm -f dist/Pharmacy-Windows.zip
	cd build && zip -qr ../dist/Pharmacy-Windows.zip Pharmacy-Windows

# ---------------------------------------------------------------------
# Source code package.
# ---------------------------------------------------------------------
source:
	mkdir -p dist
	rm -f dist/Pharmacy-source.zip
	zip -qr dist/Pharmacy-source.zip README.md Makefile main.c src include \
		packaging data -x "*.DS_Store"

clean:
	rm -rf pharmacy build

.PHONY: run macapp windows source clean
