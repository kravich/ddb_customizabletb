#include "w_toolbar.h"

#include <stdlib.h>
#include <memory.h>

extern DB_functions_t *deadbeef;
extern ddb_gtkui_t *gtkui;

void w_toolbar_initmenu(ddb_gtkui_widget_t *w, GtkWidget *menu);

typedef struct
{
    ddb_gtkui_widget_t base;
} w_toolbar_t;

ddb_gtkui_widget_t* w_toolbar_create()
{
    w_toolbar_t *w = malloc(sizeof(w_toolbar_t));
    memset(w, 0, sizeof(w_toolbar_t));

    //w->base.load = w_toolbar_load;
    w->base.initmenu = w_toolbar_initmenu;

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

void on_customize_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    printf("Customize\n");
}

void w_toolbar_initmenu(ddb_gtkui_widget_t *w, GtkWidget *menu)
{
    GtkWidget *customize_menu_item = gtk_menu_item_new_with_mnemonic("Customize");
    gtk_widget_show(customize_menu_item);

    gtk_container_add(GTK_CONTAINER(menu), customize_menu_item);

    g_signal_connect((gpointer)customize_menu_item, "activate", G_CALLBACK(on_customize_activate), w);
}

