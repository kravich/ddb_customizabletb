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
    g_object_ref(image);

    GdkPixbuf *pixbuf = gtk_widget_render_icon(image, stock_id, GTK_ICON_SIZE_BUTTON, NULL);

    g_object_unref(image);

    return pixbuf;
}

GtkWidget* create_image_by_name(const char *button_icon_name)
{
    GtkWidget *image = NULL;

    if(button_icon_name != NULL)
    {
        // try to find icon name in stock items
        GtkStockItem stock_item;
        if(gtk_stock_lookup(button_icon_name, &stock_item))
        {
            image = gtk_image_new_from_stock(button_icon_name, GTK_ICON_SIZE_BUTTON);
        }

        // if previous attempt failed, treat icon name
        // as file name and try to load it from pixmap directories
        /*if(image == NULL)
        {
            char *path_to_pixmap_file = find_pixmap_file(button_icon_name);
            if(path_to_pixmap_file != NULL)
            {
                image = gtk_image_new_from_file(path_to_pixmap_file);
                g_free(path_to_pixmap_file);
            }
        }*/

        // There is an another option - treat icon name as absolute path.
        // Should it be allowed?
    }

    // if everything failed - create a 'missing image' icon
    if(image == NULL)
    {
        image = gtk_image_new_from_stock("gtk-missing-image", GTK_ICON_SIZE_BUTTON);
    }

    return image;
}
