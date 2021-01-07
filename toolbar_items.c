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

#include "toolbar_items.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"

ssize_t toolbar_items_serialize(ToolbarItem *toolbar_items, char *buff, size_t buff_size)
{
    assert(buff_size != 0);

    size_t buff_free_space = buff_size - 1;
    ssize_t bytes_written = 0;

    buff[0] = '\0';

    ToolbarItem *current_item = toolbar_items;
    while (current_item)
    {
        char *fmt = NULL;

        if (current_item->next)
            fmt = "%s|%d|%s,";
        else
            fmt = "%s|%d|%s";

        char *item_str = g_strdup_printf(fmt,
                                         current_item->action_name,
                                         current_item->action_context,
                                         current_item->icon_name);

        size_t chars_to_add = strlen(item_str);

        if (buff_free_space >= chars_to_add)
        {
            strncat(buff, item_str, buff_free_space);
            g_free(item_str);

            buff_free_space -= chars_to_add;
            bytes_written += (ssize_t)chars_to_add;
        }
        else
        {
            g_free(item_str);
            return -1;
        }

        current_item = current_item->next;
    }

    return bytes_written + 1;
}

static bool isnamechar(char c)
{
    return !g_ascii_iscntrl(c) && c != '|' && c != ',' && c != ' ';
}

ToolbarItem* toolbar_items_deserialize_len(const char *layout, size_t len)
{
    enum
    {
        STATE_ITEM_SEPARATOR,   // also an initial and a final state
        STATE_ACTION_NAME,
        STATE_ACTION_NAME_SEPARATOR,
        STATE_CONTEXT,
        STATE_CONTEXT_SEPARATOR,
        STATE_ICON_NAME,
        STATE_ERROR
    };

    int state = STATE_ITEM_SEPARATOR;

    const char *action_name = NULL;
    size_t action_name_len = 0;

    const char *context_str = NULL;
    size_t context_str_len = 0;

    const char *icon_name = NULL;
    size_t icon_name_len = 0;

    ToolbarItem *toolbar_items = NULL;

    for (size_t i = 0; i <= len && state != STATE_ERROR; i++)
    {
        char c = layout[i];

        if (i == len)
            c = ','; // synthetic separator to finalize creation of last item

        switch (state)
        {
            case STATE_ITEM_SEPARATOR:
                if (isnamechar(c))
                {
                    action_name = layout + i;
                    action_name_len++;
                    state = STATE_ACTION_NAME;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_ACTION_NAME:
                if (isnamechar(c))
                {
                    action_name_len++;
                }
                else if (c == '|')
                {
                    state = STATE_ACTION_NAME_SEPARATOR;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_ACTION_NAME_SEPARATOR:
                if (g_ascii_isdigit(c))
                {
                    context_str = layout + i;
                    context_str_len++;
                    state = STATE_CONTEXT;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_CONTEXT:
                if (g_ascii_isdigit(c))
                {
                    context_str_len++;
                }
                else if (c == '|')
                {
                    state = STATE_CONTEXT_SEPARATOR;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_CONTEXT_SEPARATOR:
                if (isnamechar(c))
                {
                    icon_name = layout + i;
                    icon_name_len++;
                    state = STATE_ICON_NAME;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_ICON_NAME:
                if (isnamechar(c))
                {
                    icon_name_len++;
                }
                else if (c == ',')
                {
                    int context = strtoi_len(context_str, context_str_len);

                    if (DDB_ACTION_CTX_MAIN <= context && context <= DDB_ACTION_CTX_NOWPLAYING)
                    {
                        assert(action_name);
                        assert(context_str);
                        assert(icon_name);

                        assert(action_name_len);
                        assert(context_str_len);
                        assert(icon_name_len);

                        // produce item object
                        ToolbarItem *item = g_malloc0(sizeof(ToolbarItem));

                        item->action_name = strdup_len(action_name, action_name_len);
                        item->action_context = context;
                        item->icon_name = strdup_len(icon_name, icon_name_len);

                        item->action = find_action(item->action_name);

                        toolbar_items = toolbar_items_append(toolbar_items, item);

                        // reset
                        action_name = NULL;
                        action_name_len = 0;

                        context_str = NULL;
                        context_str_len = 0;

                        icon_name = NULL;
                        icon_name_len = 0;

                        state = STATE_ITEM_SEPARATOR;
                    }
                    else
                    {
                        state = STATE_ERROR;
                    }
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;
        }
    }

    if (state != STATE_ITEM_SEPARATOR) // error state
    {
        free_items_list(toolbar_items);
        toolbar_items = NULL;
    }

    return toolbar_items;
}

ToolbarItem* create_default_toolbar_items(void)
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

    for (int i = 0; i < default_items_size; i++)
    {
        ToolbarItem *item = g_malloc(sizeof(ToolbarItem));

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
    while (current_item)
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
    if (items == NULL)
    {
        return new_item;
    }

    ToolbarItem *last_item = items;
    while (last_item->next)
        last_item = last_item->next;

    last_item->next = new_item;

    return items;
}
