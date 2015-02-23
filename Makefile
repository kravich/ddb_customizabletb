CC=gcc

#CFLAGS = -O2
CFLAGS = -g

GTK2_INCLUDE_DIRS = $(shell pkg-config --cflags gtk+-2.0)
GTK2_LIBS = $(shell pkg-config --libs gtk+-2.0)

GTK3_INCLUDE_DIRS = $(shell pkg-config --cflags gtk+-3.0)
GTK3_LIBS = $(shell pkg-config --libs gtk+-3.0)

TARGET_GTK2 = ddb_customizabletb_gtk2.so
TARGET_GTK3 = ddb_customizabletb_gtk3.so

all: $(TARGET_GTK2) $(TARGET_GTK3)

gtk2: $(TARGET_GTK2)

gtk3: $(TARGET_GTK3)

$(TARGET_GTK2): ddb_customizabletb.c w_toolbar.c
	$(CC) --shared -fPIC -o $@ ddb_customizabletb.c w_toolbar.c $(CFLAGS) $(GTK2_INCLUDE_DIRS) $(GTK2_LIBS)

$(TARGET_GTK3): ddb_customizabletb.c w_toolbar.c
	$(CC) --shared -fPIC -o $@ ddb_customizabletb.c w_toolbar.c $(CFLAGS) $(GTK3_INCLUDE_DIRS) $(GTK3_LIBS)

clean:
	rm $(TARGET_GTK2) $(TARGET_GTK3)

# disable implicit rules
.SUFFIXES:
