/*
    ddb_customizabletb - Customizable toolbar plugin for DeaDBeeF Music player
    Copyright (C) 2015 Evgeny Kravchenko <cravchik@yandex.ru>

    This file is part of ddb_customizabletb.

    ddb_customizabletb is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ddb_customizabletb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ddb_customizabletb.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>

#define DDB_API_LEVEL 5
#define DDB_WARN_DEPRECATED 1

#include <deadbeef/deadbeef.h>

#if GTK_CHECK_VERSION(3,0,0)
#define TOOLBAR_ICON_SIZE 16
#else
#define TOOLBAR_ICON_SIZE 24
#endif

DB_plugin_action_t* find_action(const char *action_name);

GdkPixbuf* create_pixbuf_by_icon_name(const char *icon_name, gint icon_size_px);

GtkWidget* create_image_by_name(const char *button_icon_name, gint icon_size_px);

#endif
