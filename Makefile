#
#    ddb_customizabletb - Customizable toolbar plugin for DeaDBeeF Music player
#    Copyright (C) 2015 Evgeny Kravchenko <cravchik@yandex.ru>
#
#    This file is part of ddb_customizabletb.
#
#    ddb_customizabletb is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    ddb_customizabletb is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with ddb_customizabletb.  If not, see <http://www.gnu.org/licenses/>.
#


CC = gcc

CFLAGS += -std=c99 -fPIC -Wall -Werror -Wfatal-errors
CFLAGS += -Wno-error=deprecated-declarations -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=unused-function
#CFLAGS += -O2
CFLAGS += -g -Wno-deprecated-declarations

GTK2_INCLUDE_DIRS = $(shell pkg-config --cflags gtk+-2.0)
GTK2_LIBS = $(shell pkg-config --libs gtk+-2.0)

GTK3_INCLUDE_DIRS = $(shell pkg-config --cflags gtk+-3.0)
GTK3_LIBS = $(shell pkg-config --libs gtk+-3.0)

LDFLAGS +=


# prepare lists of .c and .h files
INPUT_FILES = \
ddb_customizabletb.c \
w_toolbar.c w_toolbar.h \
toolbar_items.c toolbar_items.h \
utils.c utils.h \
interface.c interface.h \
callbacks.c callbacks.h \
support.c support.h \
customization_dialog.c customization_dialog.h \
icon_selection_dialog.c icon_selection_dialog.h \
trace.h

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
	$(CC) --shared -o $@ $^ $(CFLAGS) $(LDFLAGS) $(GTK2_LIBS)

$(TARGET_GTK3): $(OBJS_GTK3)
	$(CC) --shared -o $@ $^ $(CFLAGS) $(LDFLAGS) $(GTK3_LIBS)

clean:
	rm -f $(TARGET_GTK2) $(TARGET_GTK3) $(OBJS_GTK2) $(OBJS_GTK3)

.PHONY: clean gtk2 gtk3 all

# disable implicit rules
.SUFFIXES:
