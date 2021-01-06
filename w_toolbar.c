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

#include "w_toolbar.h"

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "toolbar_items.h"
#include "customization_dialog.h"
#include "utils.h"
#include "trace.h"
#include "support.h"
#include "layout_params_parser.h"

#define TMP_BUFF_SIZE 256

#if GTK_CHECK_VERSION(3,0,0)
#define TOOLBAR_ICON_SIZE 16
#else
#define TOOLBAR_ICON_SIZE 22
#endif

typedef struct
{
    const char *layout_str;
    size_t layout_str_len;
} parse_ctx_t;

extern DB_functions_t *g_deadbeef;
extern ddb_gtkui_t *g_gtkui;

typedef struct
{
    ddb_gtkui_widget_t base;
    ToolbarItem *items_list;
} w_toolbar_t;

static void w_toolbar_destroy(ddb_gtkui_widget_t *w)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    free_items_list(toolbar->items_list);
}

static void on_new_param(const char *key, size_t key_len, const char *value, size_t value_len, void *user_data)
{
    parse_ctx_t *ctx = (parse_ctx_t*)user_data;

    if (strequal_len(key, key_len, "layout", strlen("layout")))
    {
        ctx->layout_str = value;
        ctx->layout_str_len = value_len;
    }
}

static const char* w_toolbar_load(ddb_gtkui_widget_t *w, const char *type, const char *s)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    parse_ctx_t parse_ctx = { NULL, 0 };

    const char *p = parse_params(s, on_new_param, &parse_ctx);

    if (parse_ctx.layout_str)
    {
        ToolbarItem *saved_toolbar_items = toolbar_items_deserialize_len(parse_ctx.layout_str, parse_ctx.layout_str_len);

        if (saved_toolbar_items)
        {
            free_items_list(toolbar->items_list);
            toolbar->items_list = saved_toolbar_items;
        }
    }

    return p;
}

static void w_toolbar_save(ddb_gtkui_widget_t *w, char *s, int sz)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    char serialized_layout[TMP_BUFF_SIZE] = {0};
    toolbar_items_serialize(toolbar->items_list, serialized_layout, TMP_BUFF_SIZE);

    char *layout_param = g_strdup_printf(" layout=\"%s\"", serialized_layout);
    strncat(s, layout_param, sz);
    g_free(layout_param);
}

/* This function was copied from Deadbeef sources */
static void activate_action_14(DB_plugin_action_t *action, int cursor)
{
    // Plugin can handle all tracks by itself
    if (action->flags & DB_ACTION_CAN_MULTIPLE_TRACKS)
    {
        action->callback (action, NULL);
        return;
    }

    // For single-track actions just invoke it with first selected track
    if (!(action->flags & DB_ACTION_MULTIPLE_TRACKS))
    {
        if (cursor == -1) {
            cursor = g_deadbeef->pl_get_cursor (PL_MAIN);
        }
        if (cursor == -1)
        {
            return;
        }
        DB_playItem_t *it = g_deadbeef->pl_get_for_idx_and_iter (cursor, PL_MAIN);
        action->callback (action, it);
        g_deadbeef->pl_item_unref (it);
        return;
    }

    //We end up here if plugin won't traverse tracks and we have to do it ourselves
    DB_playItem_t *it = g_deadbeef->pl_get_first (PL_MAIN);
    while (it) {
        if (g_deadbeef->pl_is_selected (it)) {
            action->callback (action, it);
        }
        DB_playItem_t *next = g_deadbeef->pl_get_next (it, PL_MAIN);
        g_deadbeef->pl_item_unref (it);
        it = next;
    }
}

static void toolbar_button_activate_action(GtkButton *button, gpointer user_data)
{
    ToolbarItem *item = (ToolbarItem*)user_data;

    assert(item->action);

    if (item->action->callback)
        activate_action_14(item->action, -1);
    else if (item->action->callback2)
        item->action->callback2(item->action, item->action_context);
}

static void toolbar_button_no_action_msg(GtkButton *button, gpointer user_data)
{
    GtkWindow *mainwin = GTK_WINDOW(g_gtkui->get_mainwin());

    GtkWidget *msgbox = gtk_message_dialog_new(mainwin,
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_OK,
                                               "Action assigned to this button was not found");

    gtk_dialog_run(GTK_DIALOG(msgbox));

    gtk_widget_destroy(msgbox);
}

static void fill_toolbar(w_toolbar_t *toolbar)
{
    GtkBox *hbox = GTK_BOX(toolbar->base.widget);

    ToolbarItem *current_item = toolbar->items_list;
    while (current_item)
    {
        GtkWidget *button = gtk_button_new();
        gtk_widget_set_can_focus(button, FALSE);
        gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
        gtk_widget_show(button);

        gtk_box_pack_start(hbox, button, FALSE, FALSE, 0);

        GtkWidget *image = create_image_by_name(current_item->icon_name, TOOLBAR_ICON_SIZE);
        assert(image);

        gtk_widget_show(image);
        gtk_container_add(GTK_CONTAINER(button), image);

        if (current_item->action)
        {
            g_signal_connect(button, "clicked", G_CALLBACK(toolbar_button_activate_action), current_item);
        }
        else
        {
            gtk_widget_set_sensitive(GTK_WIDGET(image), FALSE);
            g_signal_connect(button, "clicked", G_CALLBACK(toolbar_button_no_action_msg), NULL);
        }

        current_item = current_item->next;
    }

    // this is required since deadbeef sets size_request explicitly
    gtk_widget_set_size_request(toolbar->base.widget, -1, -1);
}

static void w_toolbar_init(ddb_gtkui_widget_t *w)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    fill_toolbar(toolbar);

    gtk_container_foreach(GTK_CONTAINER(toolbar->base.widget), g_gtkui->w_override_signals, w);
}

static void empty_hbox(GtkBox *hbox)
{
    GList *list = gtk_container_get_children(GTK_CONTAINER(hbox));

    for (GList *it = list; it; it = g_list_next(it)) {
        gtk_widget_destroy(GTK_WIDGET(it->data));
    }

    g_list_free(list);
}

static void w_toolbar_set_new_items(w_toolbar_t *toolbar, ToolbarItem *new_toolbar_items)
{
    if (new_toolbar_items == NULL)
        return;

    free_items_list(toolbar->items_list);
    toolbar->items_list = new_toolbar_items;

    empty_hbox(GTK_BOX(toolbar->base.widget));
    fill_toolbar(toolbar);
    gtk_container_foreach(GTK_CONTAINER(toolbar->base.widget), g_gtkui->w_override_signals, toolbar);
}

static void on_customize_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)user_data;

    GtkWindow *mainwin = GTK_WINDOW(g_gtkui->get_mainwin());

    ToolbarItem *new_toolbar_items = run_customization_dialog(mainwin, toolbar->items_list);
    if (new_toolbar_items)
        w_toolbar_set_new_items(toolbar, new_toolbar_items);
}

static void w_toolbar_initmenu(ddb_gtkui_widget_t *w, GtkWidget *menu)
{
    GtkWidget *customize_menu_item = gtk_menu_item_new_with_mnemonic("Customize");
    gtk_widget_show(customize_menu_item);

    gtk_container_add(GTK_CONTAINER(menu), customize_menu_item);

    g_signal_connect((gpointer)customize_menu_item, "activate", G_CALLBACK(on_customize_activate), w);
}

ddb_gtkui_widget_t* w_toolbar_create(void)
{
    w_toolbar_t *w = g_malloc0(sizeof(w_toolbar_t));

    w->base.load = w_toolbar_load;
    w->base.save = w_toolbar_save;
    w->base.init = w_toolbar_init;
    w->base.initmenu = w_toolbar_initmenu;
    w->base.destroy = w_toolbar_destroy;

    w->base.flags = DDB_GTKUI_WIDGET_FLAG_NON_EXPANDABLE;
    w->base.widget = gtk_hbox_new(FALSE, 0);
    gtk_widget_show((GtkWidget*)(w->base.widget));

    g_gtkui->w_override_signals(w->base.widget, w);

    w->items_list = create_default_toolbar_items();

    return (ddb_gtkui_widget_t*)w;
}
