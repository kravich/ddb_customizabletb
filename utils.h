#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>

#define DDB_API_LEVEL 5
#define DDB_WARN_DEPRECATED 1

#include <deadbeef/deadbeef.h>

#if GTK_CHECK_VERSION(3,0,0)
#define TOOLBAR_ICON_SIZE 16
#else
#define TOOLBAR_ICON_SIZE 24
#endif

DB_plugin_action_t* find_action(const char *action_name);

GdkPixbuf* create_pixbuf_by_icon_name(const char *icon_name, gint icon_size_px);

GtkWidget* create_image_by_name(const char *button_icon_name, gint icon_size_px);

#endif
