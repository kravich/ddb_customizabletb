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

#ifndef __TOOLBAR_ITEMS_H
#define __TOOLBAR_ITEMS_H

#define DDB_API_LEVEL 5
#define DDB_WARN_DEPRECATED 1
#include <deadbeef/deadbeef.h>

#include <glib.h>

typedef struct _ToolbarItem
{
    char *action_name;
    char *icon_name;

    DB_plugin_action_t *action;
    int action_context;

    struct _ToolbarItem *next;
} ToolbarItem;

void toolbar_items_serialize(ToolbarItem *toolbar_items, char *buff, size_t buff_size);
ToolbarItem* toolbar_items_deserialize(char *layout);
ToolbarItem* create_default_toolbar_items();
void free_items_list(ToolbarItem *items_list);

ToolbarItem* toolbar_items_append(ToolbarItem* items, ToolbarItem *new_item);

#endif
