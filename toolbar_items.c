#include "toolbar_items.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

void toolbar_items_serialize(ToolbarItem *toolbar_items, char *buff, size_t buff_size)
{
    assert(buff_size != 0);

    size_t buff_free_space = buff_size - 1;
    buff[0] = '\0';

    ToolbarItem *current_item = toolbar_items;
    while(current_item != NULL)
    {
        char *fmt = NULL;

        if(current_item->next != NULL)
            fmt = "%s|%d|%s,";
        else
            fmt = "%s|%d|%s";

        char *item_str = g_strdup_printf(fmt,
                                         current_item->action_name,
                                         current_item->action_context,
                                         current_item->icon_name);

        size_t chars_to_add = strlen(item_str);

        strncat(buff, item_str, buff_free_space);

        g_free(item_str);

        if(buff_free_space > chars_to_add)
            buff_free_space -= chars_to_add;
        else
            break;

        current_item = current_item->next;
    }
}

ToolbarItem* toolbar_items_deserialize(char *layout)
{
    char **elements = g_strsplit(layout, ",", -1);

    ToolbarItem *toolbar_items = NULL;

    char **current_element = elements;
    while(*current_element != NULL)
    {
        ToolbarItem *item = g_malloc(sizeof(ToolbarItem));

        char **parts = g_strsplit(*current_element, "|", 3);

        if(parts[0] == NULL || parts[1] == NULL || parts[2] == NULL || item == NULL)
        {
            g_strfreev(parts);
            g_strfreev(elements);
            free_items_list(toolbar_items);
            return NULL;
        }

        char *action_name = parts[0];
        char* action_context = parts[1];
        char *icon_name = parts[2];

        item->action_name = g_strdup(action_name); // TODO: should we check the return value of g_strdup()?
        item->icon_name = g_strdup(icon_name);
        item->action = find_action(action_name);
        item->action_context = atoi(action_context); // TODO: add check
        item->next = NULL;

        g_strfreev(parts);

        toolbar_items = toolbar_items_append(toolbar_items, item);

        current_element++;
    }

    g_strfreev(elements);

    return toolbar_items;
}

ToolbarItem* create_default_toolbar_items()
{
    ToolbarItem *toolbar_items = NULL;

    struct
    {
        const char *action_name;
        const char *icon_name;
    }
    default_items[] =
    {
        {"stop", "media-playback-stop"},
        {"play", "media-playback-start"},
        {"toggle_pause", "media-playback-pause"},
        {"prev", "media-skip-backward"},
        {"next", "media-skip-forward"}
    };

    int default_items_size = sizeof(default_items) / sizeof(default_items[0]);

    for(int i = 0; i < default_items_size; i++)
    {
        ToolbarItem *item = g_malloc(sizeof(ToolbarItem));
        assert(item != NULL); // TODO: add proper check for this case

        item->action_name = g_strdup(default_items[i].action_name);
        item->icon_name = g_strdup(default_items[i].icon_name);
        item->action = find_action(default_items[i].action_name);
        item->action_context = DDB_ACTION_CTX_MAIN;
        item->next = NULL;

        toolbar_items = toolbar_items_append(toolbar_items, item);
    }

    return toolbar_items;
}

void free_items_list(ToolbarItem *items_list)
{
    ToolbarItem *current_item = items_list;
    while(current_item != NULL)
    {
        ToolbarItem *next_item = current_item->next;

        g_free(current_item->action_name);
        g_free(current_item->icon_name);
        g_free(current_item);

        current_item = next_item;
    }
}

ToolbarItem* toolbar_items_append(ToolbarItem* items, ToolbarItem *new_item)
{
    if(items == NULL)
    {
        return new_item;
    }

    ToolbarItem *last_item = items;
    while(last_item->next != NULL)
        last_item = last_item->next;

    last_item->next = new_item;

    return items;
}
