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

CFLAGS += -std=c99 -fPIC -Wall -Wno-deprecated-declarations
CFLAGS += -O2

GTK2_INCLUDE_DIRS ?= $(shell pkg-config --cflags gtk+-2.0)
GTK2_LIBS ?= $(shell pkg-config --libs gtk+-2.0)

GTK3_INCLUDE_DIRS ?= $(shell pkg-config --cflags gtk+-3.0)
GTK3_LIBS ?= $(shell pkg-config --libs gtk+-3.0)

SOURCES = \
	ddb_customizabletb.c \
	w_toolbar.c \
	toolbar_items.c \
	utils.c \
	interface.c \
	callbacks.c \
	support.c \
	customization_dialog.c \
	icon_selection_dialog.c

OBJS_GTK2 = $(SOURCES:%.c=%_gtk2.o)
OBJS_GTK3 = $(SOURCES:%.c=%_gtk3.o)

DEPS_GTK2 = $(SOURCES:%.c=%_gtk2.d)
DEPS_GTK3 = $(SOURCES:%.c=%_gtk3.d)

TARGET_GTK2 = ddb_customizabletb_gtk2.so
TARGET_GTK3 = ddb_customizabletb_gtk3.so

V ?= 0

ifeq ($(V), 0)
	Q = @
endif

all: $(TARGET_GTK2) $(TARGET_GTK3)

include $(DEPS_GTK2)
include $(DEPS_GTK3)

gtk2: $(TARGET_GTK2)

gtk3: $(TARGET_GTK3)

%_gtk2.o: %.c
	$(info CC $@)
	$(Q)$(CC) -c -o $@ $< $(CFLAGS) $(GTK2_INCLUDE_DIRS)

%_gtk3.o: %.c
	$(info CC $@)
	$(Q)$(CC) -c -o $@ $< $(CFLAGS) $(GTK3_INCLUDE_DIRS)

%_gtk2.d: %.c
	$(Q)set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $(GTK2_INCLUDE_DIRS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1_gtk2.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%_gtk3.d: %.c
	$(Q)set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $(GTK3_INCLUDE_DIRS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1_gtk3.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(TARGET_GTK2): $(OBJS_GTK2)
	$(info LD $@)
	$(Q)$(CC) --shared -o $@ $^ $(LDFLAGS) $(GTK2_LIBS)

$(TARGET_GTK3): $(OBJS_GTK3)
	$(info LD $@)
	$(Q)$(CC) --shared -o $@ $^ $(LDFLAGS) $(GTK3_LIBS)

deploy: $(TARGET_GTK2) $(TARGET_GTK3)
	$(Q)mkdir -p ~/.local/lib/deadbeef
	$(Q)cp $(TARGET_GTK2) ~/.local/lib/deadbeef
	$(Q)cp $(TARGET_GTK3) ~/.local/lib/deadbeef

clean:
	$(Q)rm -f $(TARGET_GTK2) $(TARGET_GTK3) $(OBJS_GTK2) $(OBJS_GTK3) $(DEPS_GTK2) $(DEPS_GTK3)

.PHONY: clean gtk2 gtk3 deploy all

# disable implicit rules
.SUFFIXES:
