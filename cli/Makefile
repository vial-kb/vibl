CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.c sha256.c
INCLUDE_DIRS=-I .

ifeq ($(OS),Windows_NT)
	SOURCES+=hid-win.c
	LIBS=-lsetupapi -lhid
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		SOURCES+=hid-mac.c
		LIBS=-framework IOKit -framework CoreFoundation
	else
		ifeq ($(UNAME_S),Linux)
			SOURCES+=hid-hidraw.c
			LIBS=`pkg-config libudev --libs` -lrt -lpthread
			INCLUDE_DIRS+=`pkg-config libudev --cflags`
			CFLAGS+=-std=gnu99
		else
			SOURCES+=hid-libusb.c
			LIBS=`pkg-config libusb-1.0 --libs` -lrt -lpthread
			INCLUDE_DIRS+=`pkg-config libusb-1.0 --cflags`
			CFLAGS+=-std=gnu99
		endif
    endif
endif

OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE = vibl-flash

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(EXECUTABLE).exe
