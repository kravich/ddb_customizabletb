#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>

#define DDB_API_LEVEL 5
#define DDB_WARN_DEPRECATED 1

#include <deadbeef/deadbeef.h>

DB_plugin_action_t* find_action(char *action_name);

GdkPixbuf* create_pixbuf_from_stock_icon(const char *stock_id, GtkIconSize icon_size);

GtkWidget* create_image_by_name(const char *button_icon_name);

#endif
