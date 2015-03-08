CC = gcc

CFLAGS = -std=c99 -fPIC -Wall -Werror -Wfatal-errors
CFLAGS += -Wno-error=deprecated-declarations -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=unused-function
#CFLAGS += -O2
CFLAGS += -g -Wno-deprecated-declarations

GTK2_INCLUDE_DIRS = $(shell pkg-config --cflags gtk+-2.0)
GTK2_LIBS = $(shell pkg-config --libs gtk+-2.0)

GTK3_INCLUDE_DIRS = $(shell pkg-config --cflags gtk+-3.0)
GTK3_LIBS = $(shell pkg-config --libs gtk+-3.0)

# prepare lists of .c and .h files
INPUT_FILES = ddb_customizabletb.c w_toolbar.c w_toolbar.h toolbar_items.c toolbar_items.h
SOURCES = $(filter %.c, $(INPUT_FILES))
HEADERS = $(filter %.h, $(INPUT_FILES))

#generate lists of objects for different builds
OBJS_GTK2 = $(SOURCES:%.c=%_gtk2.o)
OBJS_GTK3 = $(SOURCES:%.c=%_gtk3.o)

TARGET_GTK2 = ddb_customizabletb_gtk2.so
TARGET_GTK3 = ddb_customizabletb_gtk3.so

all: $(TARGET_GTK2) $(TARGET_GTK3)

gtk2: $(TARGET_GTK2)

gtk3: $(TARGET_GTK3)

# compile objects for gtk2...
%_gtk2.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(GTK2_INCLUDE_DIRS)

# ... and gtk3 independently
%_gtk3.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(GTK3_INCLUDE_DIRS)

$(TARGET_GTK2): $(OBJS_GTK2)
	$(CC) --shared -o $@ $^ $(GTK2_LIBS)

$(TARGET_GTK3): $(OBJS_GTK3)
	$(CC) --shared -o $@ $^ $(GTK3_LIBS)

clean:
	rm -f $(TARGET_GTK2) $(TARGET_GTK3) $(OBJS_GTK2) $(OBJS_GTK3)

.PHONY: clean gtk2 gtk3 all

# disable implicit rules
.SUFFIXES:
