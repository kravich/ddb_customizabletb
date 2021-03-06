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

#include "utils.h"

#include <glib.h>
#include <assert.h>

#include "trace.h"

extern DB_functions_t *deadbeef;

DB_plugin_action_t* find_action(const char *action_name)
{
    DB_plugin_t **plugins = deadbeef->plug_get_list();

    for(int i = 0; plugins[i] != NULL; i++)
    {
        if(plugins[i]->get_actions == NULL)
            continue;

        DB_plugin_action_t *plugin_actions = plugins[i]->get_actions(NULL);

        DB_plugin_action_t *current_action = plugin_actions;
        while(current_action != NULL)
        {
            if(g_str_equal(current_action->name, action_name))
            {
                return current_action;
            }

            current_action = current_action->next;
        }
    }

    return NULL;
}

GdkPixbuf* create_pixbuf_by_icon_name(const char *icon_name, gint icon_size_px)
{
    GtkIconTheme *default_icon_theme = gtk_icon_theme_get_default();

    GError *error = NULL;
    GdkPixbuf *pixbuf = NULL;

    if(gtk_icon_theme_has_icon(default_icon_theme, icon_name))
    {
        pixbuf = gtk_icon_theme_load_icon(default_icon_theme, icon_name, icon_size_px, GTK_ICON_LOOKUP_FORCE_SIZE, &error);
    }
    else
    {
        trace("Current theme does not have icon %s\n", icon_name);
        pixbuf = gtk_icon_theme_load_icon(default_icon_theme, "image-missing", icon_size_px, GTK_ICON_LOOKUP_FORCE_SIZE, &error); // TODO: is it bulletproof or could also fail?
    }
	
	assert(pixbuf != NULL);

    return pixbuf;
}

GtkWidget* create_image_by_name(const char *button_icon_name, gint icon_size_px)
{
    GdkPixbuf *icon_pixbuf = create_pixbuf_by_icon_name(button_icon_name, icon_size_px);
    if(icon_pixbuf == NULL)
        return NULL;

    GtkWidget *image = gtk_image_new_from_pixbuf(icon_pixbuf);
    g_object_unref(icon_pixbuf);

    return image;
}
