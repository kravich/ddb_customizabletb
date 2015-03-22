#include "utils.h"

#include <glib.h>

extern DB_functions_t *deadbeef;

DB_plugin_action_t* find_action(char *action_name)
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

GdkPixbuf* create_pixbuf_from_stock_icon(const char *stock_id)
{
    GtkStockItem stock_item;
    if(!gtk_stock_lookup(stock_id, &stock_item))
        stock_id = "gtk-missing-image";

    GtkWidget *image = gtk_image_new();
    GdkPixbuf *pixbuf = gtk_widget_render_icon(image, stock_id, GTK_ICON_SIZE_BUTTON, NULL);

    g_object_unref(image);

    return pixbuf;
}

