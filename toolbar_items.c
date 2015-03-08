#include "toolbar_items.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

void toolbar_items_serialize(GSList *toolbar_items, char *buff, size_t buff_size)
{
    assert(buff_size != 0);

    size_t buff_free_space = buff_size - 1;
    buff[0] = '\0';

    GSList *current_node = toolbar_items;
    while(current_node != NULL)
    {
        ToolbarItem *current_item = (ToolbarItem*)current_node->data;

        char *fmt = NULL;

        if(current_node->next != NULL)
            fmt = "%s|%s,";
        else
            fmt = "%s|%s";

        char *item_str = g_strdup_printf(fmt, current_item->action_name, current_item->icon_name);

        size_t chars_to_add = strlen(item_str);

        strncat(buff, item_str, buff_free_space);

        g_free(item_str);

        if(buff_free_space > chars_to_add)
            buff_free_space -= chars_to_add;
        else
            break;

        current_node = g_slist_next(current_node);
    }
}

GSList* toolbar_items_deserialize(char *layout)
{
    char **elements = g_strsplit(layout, ",", -1);

    GSList *toolbar_items = NULL;
    char **current_element = elements;
    while(*current_element != NULL)
    {
        ToolbarItem *item = malloc(sizeof(ToolbarItem));

        char **parts = g_strsplit(*current_element, "|", 2);

        if(parts[0] == NULL || parts[1] == NULL || item == NULL)
        {
            g_strfreev(parts);
            g_strfreev(elements);
            free_items_list(toolbar_items);
            return NULL;
        }

        item->action_name = g_strdup(parts[0]); // TODO: should we check the return value of g_strdup()?
        item->icon_name = g_strdup(parts[1]);

        toolbar_items = g_slist_append(toolbar_items, item);

        current_element++;
    }

    return toolbar_items;
}

GSList* create_default_toolbar_items()
{
    GSList *toolbar_items = NULL;
    ToolbarItem *item = NULL;

    // TODO: add check for malloc() return value

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("stop");
    item->icon_name = g_strdup("gtk-media-stop");

    toolbar_items = g_slist_append(toolbar_items, item);

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("play");
    item->icon_name = g_strdup("gtk-media-play");

    toolbar_items = g_slist_append(toolbar_items, item);

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("toggle_pause");
    item->icon_name = g_strdup("gtk-media-pause");

    toolbar_items = g_slist_append(toolbar_items, item);

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("prev");
    item->icon_name = g_strdup("gtk-media-previous");

    toolbar_items = g_slist_append(toolbar_items, item);

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("next");
    item->icon_name = g_strdup("gtk-media-next");

    toolbar_items = g_slist_append(toolbar_items, item);

    return toolbar_items;
}

void free_item_data_cb(gpointer data)
{
    ToolbarItem *item = (ToolbarItem*)data;
    assert(item != NULL);

    g_free(item->action_name);
    g_free(item->icon_name);
    g_free(item);
}

void free_items_list(GSList *items_list)
{
    g_slist_free_full(items_list, free_item_data_cb);
}
