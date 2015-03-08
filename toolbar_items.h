#ifndef __TOOLBAR_ITEMS_H
#define __TOOLBAR_ITEMS_H

#include <glib.h>

typedef struct
{
    char *action_name;
    char *icon_name;
} ToolbarItem;

void toolbar_items_serialize(GSList *toolbar_items, char *buff, size_t buff_size);
GSList* toolbar_items_deserialize(char *layout);
GSList* create_default_toolbar_items();
void free_items_list(GSList *items_list);

#endif
