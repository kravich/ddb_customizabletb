#include "w_toolbar.h"

#include <stdlib.h>
#include <memory.h>

extern DB_functions_t *deadbeef;
extern ddb_gtkui_t *gtkui;

typedef struct
{
    ddb_gtkui_widget_t base;
} w_toolbar_t;

ddb_gtkui_widget_t* w_toolbar_create()
{
    w_toolbar_t *w = malloc(sizeof(w_toolbar_t));
    memset(w, 0, sizeof(w_toolbar_t));

    w->base.flags = DDB_GTKUI_WIDGET_FLAG_NON_EXPANDABLE;
    w->base.widget = gtk_hbox_new(FALSE, 0);
    gtk_widget_show((GtkWidget*)(w->base.widget));

    GtkWidget *button = gtk_button_new_with_label("Hello, world!");
    gtk_widget_show(button);

    gtk_widget_set_size_request(button, 100, 20);

    gtk_box_pack_start(GTK_BOX(w->base.widget), button, FALSE, FALSE, 0);

    gtkui->w_override_signals(w->base.widget, w);

    return (ddb_gtkui_widget_t*)w;
}
