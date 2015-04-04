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
