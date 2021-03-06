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

#include <gtk/gtk.h>

#include <stdlib.h>
#include <assert.h>

#include <glib.h>

#include "interface.h"
#include "support.h"
#include "toolbar_items.h"
#include "utils.h"
#include "icon_selection_dialog.h"
#include "trace.h"

extern DB_functions_t *deadbeef;

enum
{
    ITEMS_COL_ICON_PIXBUF,
    ITEMS_COL_ACTION_TITLE,
    ITEMS_COL_ACTION_CTX_NAME,
    ITEMS_COL_ACTION_NAME,
    ITEMS_COL_ACTION_CTX_ID,
    ITEMS_COL_ICON_NAME,
    ITEMS_COL_WAS_ACTION_FOUND,
    ITEMS_COLS_NUM
};

enum
{
    ACTIONS_COL_TITLE,
    ACTIONS_COL_NAME,
    ACTIONS_COL_CTX,
    ACTIONS_COLS_NUM
};

const char* get_context_name_by_id(int context)
{
    assert(context >= 0 && context < DDB_ACTION_CTX_COUNT);

    char *context_names[] =
    {
        [DDB_ACTION_CTX_MAIN] = "Main context",
        [DDB_ACTION_CTX_NOWPLAYING] = "Nowplaying context",
        [DDB_ACTION_CTX_PLAYLIST] = "Playlist context",
        [DDB_ACTION_CTX_SELECTION] = "Selection context"
    };

    return context_names[context];
}

GtkListStore* create_items_list_store(ToolbarItem *toolbar_items)
{
    GtkListStore *items_list = gtk_list_store_new(ITEMS_COLS_NUM,
                                                  GDK_TYPE_PIXBUF,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_STRING,
                                                  G_TYPE_INT,
                                                  G_TYPE_STRING,
                                                  G_TYPE_BOOLEAN);

    ToolbarItem *current_item = toolbar_items;
    while(current_item != NULL)
    {
        const char *action_title = current_item->action_name;
        gboolean was_action_found = FALSE;

        if(current_item->action != NULL)
        {
            action_title = current_item->action->title;
            was_action_found = TRUE;
        }

        GdkPixbuf *icon = create_pixbuf_by_icon_name(current_item->icon_name, 16);

        GtkTreeIter row_iter;
        gtk_list_store_append(items_list, &row_iter);

        gtk_list_store_set(items_list, &row_iter,
                           ITEMS_COL_ICON_PIXBUF, icon,
                           ITEMS_COL_ACTION_TITLE, action_title,
                           ITEMS_COL_ACTION_CTX_NAME, get_context_name_by_id(current_item->action_context),
                           ITEMS_COL_ACTION_NAME, current_item->action_name,
                           ITEMS_COL_ACTION_CTX_ID, current_item->action_context,
                           ITEMS_COL_ICON_NAME, current_item->icon_name,
                           ITEMS_COL_WAS_ACTION_FOUND, was_action_found,
                           -1);

        g_object_unref(icon);

        current_item = current_item->next;
    }

    return items_list;
}

gboolean actions_tree_find_group_entry(GtkTreeModel *tree_model, GtkTreeIter *ctx_group_iter, char *required_group_name, GtkTreeIter *iter)
{
    gboolean iter_found = FALSE;

    gboolean res = gtk_tree_model_iter_children(tree_model, iter, ctx_group_iter);
    while(res)
    {
        char *group_name = NULL;

        gtk_tree_model_get(tree_model, iter, 0, &group_name, -1);

        if(g_str_equal(group_name, required_group_name))
        {
            g_free(group_name);
            iter_found = TRUE;
            break;
        }

        g_free(group_name);

        res = gtk_tree_model_iter_next(tree_model, iter);
    }

    return iter_found;
}

GtkTreeStore* create_actions_tree_store()
{
    GtkTreeStore *actions_tree_store = gtk_tree_store_new(ACTIONS_COLS_NUM, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

    GtkTreeIter main_ctx_group_iter;
    GtkTreeIter selection_ctx_group_iter;
    GtkTreeIter playlist_ctx_group_iter;
    GtkTreeIter nowplaying_ctx_group_iter;

    gtk_tree_store_append(actions_tree_store, &main_ctx_group_iter, NULL);
    gtk_tree_store_append(actions_tree_store, &selection_ctx_group_iter, NULL);
    gtk_tree_store_append(actions_tree_store, &playlist_ctx_group_iter, NULL);
    gtk_tree_store_append(actions_tree_store, &nowplaying_ctx_group_iter, NULL);

    gtk_tree_store_set(actions_tree_store, &main_ctx_group_iter,
                       ACTIONS_COL_TITLE, "Main",
                       ACTIONS_COL_NAME, "",
                       ACTIONS_COL_CTX, 0,
                       -1);

    gtk_tree_store_set(actions_tree_store, &selection_ctx_group_iter,
                       ACTIONS_COL_TITLE, "Selection",
                       ACTIONS_COL_NAME, "",
                       ACTIONS_COL_CTX, 0,
                       -1);

    gtk_tree_store_set(actions_tree_store, &playlist_ctx_group_iter,
                       ACTIONS_COL_TITLE, "Playlist",
                       ACTIONS_COL_NAME, "",
                       ACTIONS_COL_CTX, 0,
                       -1);

    gtk_tree_store_set(actions_tree_store, &nowplaying_ctx_group_iter,
                       ACTIONS_COL_TITLE, "Nowplaying",
                       ACTIONS_COL_NAME, "",
                       ACTIONS_COL_CTX, 0,
                       -1);

    struct
    {
        GtkTreeIter *ctx_group_iter;
        int valid_flags;
    } contexts[] =
    {
        [DDB_ACTION_CTX_MAIN] = { &main_ctx_group_iter, DB_ACTION_COMMON },
        [DDB_ACTION_CTX_SELECTION] = { &selection_ctx_group_iter, (DB_ACTION_SINGLE_TRACK |
                                                                   DB_ACTION_MULTIPLE_TRACKS |
                                                                   DB_ACTION_CAN_MULTIPLE_TRACKS) },
        [DDB_ACTION_CTX_PLAYLIST] = { &playlist_ctx_group_iter, (DB_ACTION_SINGLE_TRACK |
                                                                 DB_ACTION_MULTIPLE_TRACKS |
                                                                 DB_ACTION_CAN_MULTIPLE_TRACKS) },
        [DDB_ACTION_CTX_NOWPLAYING] = { &nowplaying_ctx_group_iter, (DB_ACTION_SINGLE_TRACK |
                                                                     DB_ACTION_MULTIPLE_TRACKS |
                                                                     DB_ACTION_CAN_MULTIPLE_TRACKS) }
    };


    DB_plugin_t **plugins = deadbeef->plug_get_list();

    for(int plug_i = 0; plugins[plug_i] != NULL; plug_i++)
    {
        DB_plugin_t *curr_plugin = plugins[plug_i];

        if(curr_plugin->get_actions == NULL)
            continue;

        DB_plugin_action_t *actions = curr_plugin->get_actions(NULL);
        DB_plugin_action_t *curr_action = actions;
        while(curr_action != NULL)
        {
            char **name_parts = g_strsplit(curr_action->title, "/", 2);

            char *group_name = NULL;
            char *action_title = NULL;

            if(name_parts[1] != NULL)   // action title has group specified in name
            {
                group_name = name_parts[0];
                action_title = name_parts[1];
            }
            else
            {
                action_title = name_parts[0];
            }

            for(int curr_context = DDB_ACTION_CTX_MAIN; curr_context < DDB_ACTION_CTX_COUNT; curr_context++)
            {
                if((curr_action->flags & contexts[curr_context].valid_flags) == 0)
                    continue;

                GtkTreeIter *ctx_group_iter = contexts[curr_context].ctx_group_iter;

                GtkTreeIter action_iter;

                if(group_name != NULL)
                {
                    GtkTreeIter group_iter;

                    gboolean group_exists = actions_tree_find_group_entry(GTK_TREE_MODEL(actions_tree_store),
                                                                          ctx_group_iter,
                                                                          group_name,
                                                                          &group_iter);

                    if(!group_exists)
                    {
                        gtk_tree_store_append(actions_tree_store, &group_iter, ctx_group_iter);
                        gtk_tree_store_set(actions_tree_store, &group_iter,
                                           ACTIONS_COL_TITLE, group_name,
                                           ACTIONS_COL_NAME, "",
                                           ACTIONS_COL_CTX, 0,
                                           -1);
                    }

                    gtk_tree_store_append(actions_tree_store, &action_iter, &group_iter);
                }
                else
                {
                    gtk_tree_store_append(actions_tree_store, &action_iter, ctx_group_iter);
                }

                gtk_tree_store_set(actions_tree_store, &action_iter,
                                   ACTIONS_COL_TITLE, action_title,
                                   ACTIONS_COL_NAME, curr_action->name,
                                   ACTIONS_COL_CTX, curr_context,
                                   -1);
            }

            g_strfreev(name_parts);

            curr_action = curr_action->next;
        }
    }

    return actions_tree_store;
}

void ItemPixbufDataFunc(GtkTreeViewColumn *action_title_column,
                        GtkCellRenderer *item_pixbuf_cell_renderer,
                        GtkTreeModel *tree_model,
                        GtkTreeIter *iter,
                        gpointer data)
{
    GdkPixbuf *item_pixbuf = NULL;
    gboolean was_action_found = FALSE;

    gtk_tree_model_get(tree_model, iter, ITEMS_COL_ICON_PIXBUF,
                                         &item_pixbuf,
                                         ITEMS_COL_WAS_ACTION_FOUND,
                                         &was_action_found,
                                         -1);

    g_object_set(item_pixbuf_cell_renderer, "pixbuf", item_pixbuf, NULL);
    g_object_unref(item_pixbuf);

    if(was_action_found)
        g_object_set(item_pixbuf_cell_renderer, "sensitive", TRUE, NULL);
    else
        g_object_set(item_pixbuf_cell_renderer, "sensitive", FALSE, NULL);
}

void ActionTitleDataFunc(GtkTreeViewColumn *action_title_column,
                         GtkCellRenderer *action_title_cell_renderer,
                         GtkTreeModel *tree_model,
                         GtkTreeIter *iter,
                         gpointer data)
{
    char *item_title = NULL;
    gboolean was_action_found = FALSE;

    gtk_tree_model_get(tree_model, iter, ITEMS_COL_ACTION_TITLE,
                                         &item_title,
                                         ITEMS_COL_WAS_ACTION_FOUND,
                                         &was_action_found,
                                         -1);

    if(was_action_found)
    {
        g_object_set(action_title_cell_renderer, "text", item_title, NULL);
        g_object_set(action_title_cell_renderer, "sensitive", TRUE, NULL);
    }
    else
    {
        char buff[256];
        snprintf(buff, 255, "Unknown action: %s", item_title);

        g_object_set(action_title_cell_renderer, "text", buff, NULL);

        g_object_set(action_title_cell_renderer, "sensitive", FALSE, NULL);
    }

    g_free(item_title);
}

void init_items_treeview(GtkTreeView *items_treeview)
{
    GtkTreeViewColumn *action_title_column = gtk_tree_view_column_new();
    GtkCellRenderer *icon_renderer = gtk_cell_renderer_pixbuf_new();
    GtkCellRenderer *action_title_renderer = gtk_cell_renderer_text_new();
    GtkCellRenderer *action_ctx_renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_column_pack_start(action_title_column, icon_renderer, FALSE);
    gtk_tree_view_column_pack_start(action_title_column, action_title_renderer, TRUE);
    gtk_tree_view_column_pack_start(action_title_column, action_ctx_renderer, FALSE);

    gtk_tree_view_column_add_attribute(action_title_column, action_ctx_renderer, "text", ITEMS_COL_ACTION_CTX_NAME);

    gtk_tree_view_column_set_cell_data_func(action_title_column, icon_renderer, ItemPixbufDataFunc, NULL, NULL);
    gtk_tree_view_column_set_cell_data_func(action_title_column, action_title_renderer, ActionTitleDataFunc, NULL, NULL);

    gtk_tree_view_append_column(items_treeview, action_title_column);
}

void init_actions_treeview(GtkTreeView *actions_tree_view)
{
    GtkTreeViewColumn *action_name_column = gtk_tree_view_column_new();
    GtkCellRenderer *action_name_renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_column_pack_end(action_name_column, action_name_renderer, TRUE);

    gtk_tree_view_column_add_attribute(action_name_column, action_name_renderer, "text", ACTIONS_COL_TITLE);

    gtk_tree_view_append_column(actions_tree_view, action_name_column);
}

ToolbarItem* extract_items_from_list(GtkListStore *items_list)
{
    GtkTreeIter row_iter;
    ToolbarItem *new_items = NULL;

    gboolean res = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(items_list), &row_iter);

    while(res == TRUE)
    {
        char *action_name = NULL;
        char *icon_name = NULL;
        int action_context = 0;

        gtk_tree_model_get(GTK_TREE_MODEL(items_list), &row_iter,
                           ITEMS_COL_ACTION_NAME, &action_name,
                           ITEMS_COL_ICON_NAME, &icon_name,
                           ITEMS_COL_ACTION_CTX_ID, &action_context,
                           -1);

        ToolbarItem *item = (ToolbarItem*)g_malloc(sizeof(ToolbarItem));
        item->action_name = action_name;
        item->icon_name = icon_name;
        item->action = find_action(action_name);
        item->action_context = action_context;
        item->next = NULL;

        new_items = toolbar_items_append(new_items, item);

        res = gtk_tree_model_iter_next(GTK_TREE_MODEL(items_list), &row_iter);
    }

    return new_items;
}

void on_button_add_clicked(GtkButton *button, gpointer user_data)
{
    assert(GTK_IS_DIALOG(user_data));

    GtkWidget *d = GTK_WIDGET(user_data);

    GtkWidget *items_treeview = lookup_widget(d, "items_treeview");
    GtkWidget *actions_treeview = lookup_widget(d, "actions_treeview");

    assert(items_treeview != NULL);
    assert(actions_treeview != NULL);

    GtkTreeSelection *actions_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(actions_treeview));

    if(gtk_tree_selection_count_selected_rows(actions_selection) != 1)
    {
        trace("No action is selected\n");
        return;
    }

    GtkTreeModel *actions_tree_store = NULL;
    GtkTreeIter current_action_iter;

    gboolean res = gtk_tree_selection_get_selected(actions_selection, &actions_tree_store, &current_action_iter);
    assert(res == TRUE);

    char *action_name = NULL;
    int action_ctx = 0;
    gtk_tree_model_get(actions_tree_store, &current_action_iter,
                       ACTIONS_COL_NAME, &action_name,
                       ACTIONS_COL_CTX, &action_ctx,
                       -1);

    if(g_str_equal(action_name, ""))
    {
        trace("Group entry is selected, can't add item\n");
        g_free(action_name);
        return;
    }

    DB_plugin_action_t *action = find_action(action_name);
    if(action == NULL)
    {
        trace("Can't find action for id %s\n", action_name);
        g_free(action_name);
        return;
    }

    GdkPixbuf *new_item_icon = create_pixbuf_by_icon_name("image-missing", 16);

    GtkTreeModel *items_list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(items_treeview));

    GtkTreeIter new_item_iter;
    gtk_list_store_append(GTK_LIST_STORE(items_list_store), &new_item_iter);

    gtk_list_store_set(GTK_LIST_STORE(items_list_store), &new_item_iter,
                       ITEMS_COL_ACTION_TITLE, action->title,
                       ITEMS_COL_ACTION_NAME, action_name,
                       ITEMS_COL_ICON_NAME, "image-missing",
                       ITEMS_COL_ICON_PIXBUF, new_item_icon,
                       ITEMS_COL_ACTION_CTX_NAME, get_context_name_by_id(action_ctx),
                       ITEMS_COL_ACTION_CTX_ID, action_ctx,
                       ITEMS_COL_WAS_ACTION_FOUND, TRUE,
                       -1);

    g_free(action_name);
    g_object_unref(new_item_icon);
}

void on_button_remove_clicked(GtkButton *button, gpointer user_data)
{
    GtkTreeView *items_treeview = GTK_TREE_VIEW(user_data);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(items_treeview);
    if(gtk_tree_selection_count_selected_rows(selection) != 1)
        return;

    GtkTreeModel *items_list = NULL;
    GtkTreeIter selected_iter;

    gtk_tree_selection_get_selected(selection, &items_list, &selected_iter);

    gtk_list_store_remove(GTK_LIST_STORE(items_list), &selected_iter);
}

void on_button_up_clicked(GtkButton *button, gpointer user_data)
{
    GtkTreeView *items_treeview = GTK_TREE_VIEW(user_data);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(items_treeview);
    if(gtk_tree_selection_count_selected_rows(selection) != 1)
        return;

    GtkTreeModel *items_list = NULL;
    GtkTreeIter selected_iter;

    gtk_tree_selection_get_selected(selection, &items_list, &selected_iter);

    GtkTreePath *prev_row_path = gtk_tree_model_get_path(items_list, &selected_iter);
    gtk_tree_path_prev(prev_row_path);

    GtkTreeIter prev_row_iter;
    if(!gtk_tree_model_get_iter(items_list, &prev_row_iter, prev_row_path))
    {
        gtk_tree_path_free(prev_row_path);
        return;
    }

    gtk_tree_path_free(prev_row_path);

    gtk_list_store_swap(GTK_LIST_STORE(items_list), &selected_iter, &prev_row_iter);
}

void on_button_down_clicked(GtkButton *button, gpointer user_data)
{
    GtkTreeView *items_treeview = GTK_TREE_VIEW(user_data);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(items_treeview);
    if(gtk_tree_selection_count_selected_rows(selection) != 1)
        return;

    GtkTreeModel *items_list = NULL;
    GtkTreeIter selected_iter;

    gtk_tree_selection_get_selected(selection, &items_list, &selected_iter);

    GtkTreePath *next_row_path = gtk_tree_model_get_path(items_list, &selected_iter);
    gtk_tree_path_next(next_row_path);

    GtkTreeIter next_row_iter;
    if(!gtk_tree_model_get_iter(items_list, &next_row_iter, next_row_path))
    {
        gtk_tree_path_free(next_row_path);
        return;
    }

    gtk_tree_path_free(next_row_path);

    gtk_list_store_swap(GTK_LIST_STORE(items_list), &selected_iter, &next_row_iter);
}

void on_button_change_icon_clicked(GtkButton *button, gpointer user_data)
{
    assert(GTK_IS_DIALOG(user_data));

    GtkDialog *dialog = GTK_DIALOG(user_data);

    GtkWidget *items_treeview = lookup_widget(GTK_WIDGET(dialog), "items_treeview");
    assert(items_treeview != NULL);

    GtkTreeSelection *items_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(items_treeview));

    if(gtk_tree_selection_count_selected_rows(items_selection) != 1)
        return;

    GtkTreeModel *items_list_store = NULL;
    GtkTreeIter selected_item_iter;

    gtk_tree_selection_get_selected(items_selection, &items_list_store, &selected_item_iter);

    char *icon_name = NULL;
    gtk_tree_model_get(items_list_store, &selected_item_iter,
                       ITEMS_COL_ICON_NAME, &icon_name,
                       -1);

    char *new_icon_name = run_icon_selection_dialog(GTK_WINDOW(dialog), icon_name);
    g_free(icon_name);

    if(new_icon_name != NULL)
    {
        GdkPixbuf *new_icon_pixbuf = create_pixbuf_by_icon_name(new_icon_name, 16);

        gtk_list_store_set(GTK_LIST_STORE(items_list_store), &selected_item_iter,
                           ITEMS_COL_ICON_PIXBUF, new_icon_pixbuf,
                           ITEMS_COL_ICON_NAME, new_icon_name,
                           -1);

        g_object_unref(new_icon_pixbuf);
        g_free(new_icon_name);
    }
}

void on_items_selection_changed(GtkTreeSelection *items_selection, gpointer user_data)
{
    GtkWidget *dialog = GTK_WIDGET(user_data);
    GtkWidget *button_up = lookup_widget(dialog, "button_up");
    GtkWidget *button_down = lookup_widget(dialog, "button_down");
    GtkWidget *button_remove = lookup_widget(dialog, "button_remove");
    GtkWidget *button_change_icon = lookup_widget(dialog, "button_change_icon");

    assert(button_up != NULL);
    assert(button_down != NULL);
    assert(button_remove != NULL);
    assert(button_change_icon != NULL);

    if(gtk_tree_selection_count_selected_rows(items_selection) == 1)
    {
        gtk_widget_set_sensitive(button_up, TRUE);
        gtk_widget_set_sensitive(button_down, TRUE);
        gtk_widget_set_sensitive(button_remove, TRUE);
        gtk_widget_set_sensitive(button_change_icon, TRUE);
    }
    else
    {
        gtk_widget_set_sensitive(button_up, FALSE);
        gtk_widget_set_sensitive(button_down, FALSE);
        gtk_widget_set_sensitive(button_remove, FALSE);
        gtk_widget_set_sensitive(button_change_icon, FALSE);
    }
}

void on_actions_selection_changed(GtkTreeSelection *actions_selection, gpointer user_data)
{
    GtkWidget *dialog = GTK_WIDGET(user_data);
    GtkWidget *button_add = lookup_widget(dialog, "button_add");

    assert(button_add != NULL);

    if(gtk_tree_selection_count_selected_rows(actions_selection) != 1)
    {
        gtk_widget_set_sensitive(button_add, FALSE);
        return;
    }

    GtkTreeModel *actions_tree_store = NULL;
    GtkTreeIter iter;

    gtk_tree_selection_get_selected(actions_selection, &actions_tree_store, &iter);

    char *action_name = NULL;

    gtk_tree_model_get(actions_tree_store, &iter, ACTIONS_COL_NAME, &action_name, -1);

    if(g_str_equal(action_name, ""))
        gtk_widget_set_sensitive(button_add, FALSE);
    else
        gtk_widget_set_sensitive(button_add, TRUE);

    g_free(action_name);
}

void dialog_connect_signals(GtkWidget *dialog)
{
    GtkWidget *button_add = lookup_widget(dialog, "button_add");
    GtkWidget *button_remove = lookup_widget(dialog, "button_remove");
    GtkWidget *button_up = lookup_widget(dialog, "button_up");
    GtkWidget *button_down = lookup_widget(dialog, "button_down");
    GtkWidget *button_change_icon = lookup_widget(dialog, "button_change_icon");
    GtkWidget *items_treeview = lookup_widget(dialog, "items_treeview");
    GtkWidget *actions_treeview = lookup_widget(dialog, "actions_treeview");

    assert(button_add != NULL);
    assert(button_remove != NULL);
    assert(button_up != NULL);
    assert(button_down != NULL);
    assert(button_change_icon != NULL);
    assert(items_treeview != NULL);
    assert(actions_treeview != NULL);

    g_signal_connect(button_add, "clicked", G_CALLBACK(on_button_add_clicked), dialog);
    g_signal_connect(button_remove, "clicked", G_CALLBACK(on_button_remove_clicked), items_treeview);
    g_signal_connect(button_up, "clicked", G_CALLBACK(on_button_up_clicked), items_treeview);
    g_signal_connect(button_down, "clicked", G_CALLBACK(on_button_down_clicked), items_treeview);
    g_signal_connect(button_change_icon, "clicked", G_CALLBACK(on_button_change_icon_clicked), dialog);

    GtkTreeSelection *items_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(items_treeview));
    GtkTreeSelection *actions_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(actions_treeview));

    g_signal_connect(items_selection, "changed", G_CALLBACK(on_items_selection_changed), dialog);
    g_signal_connect(actions_selection, "changed", G_CALLBACK(on_actions_selection_changed), dialog);
}

void update_ok_button_state(GtkTreeModel *items_list, GtkWidget *dialog)
{
    GtkWidget *ok_button = lookup_widget(dialog, "ok_button");
    assert(ok_button != NULL);

    GtkTreeIter iter;
    if(gtk_tree_model_get_iter_first(items_list, &iter) == TRUE)
        gtk_widget_set_sensitive(ok_button, TRUE);
    else
        gtk_widget_set_sensitive(ok_button, FALSE);
}

void on_items_list_row_deleted(GtkTreeModel *items_list, GtkTreePath *path, gpointer user_data)
{
    GtkWidget *dialog = GTK_WIDGET(user_data);
    update_ok_button_state(items_list, dialog);
}

void on_items_list_row_inserted(GtkTreeModel *items_list, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
    GtkWidget *dialog = GTK_WIDGET(user_data);
    update_ok_button_state(items_list, dialog);
}

void dialog_init(GtkWidget *dialog, GtkListStore *items_list_store, GtkTreeStore *actions_tree_store)
{
    GtkWidget *items_treeview = lookup_widget(dialog, "items_treeview");
    GtkWidget *actions_treeview = lookup_widget(dialog, "actions_treeview");

    assert(items_treeview != NULL);
    assert(actions_treeview != NULL);

    init_items_treeview(GTK_TREE_VIEW(items_treeview));
    gtk_tree_view_set_model(GTK_TREE_VIEW(items_treeview), GTK_TREE_MODEL(items_list_store));

    init_actions_treeview(GTK_TREE_VIEW(actions_treeview));
    gtk_tree_view_set_model(GTK_TREE_VIEW(actions_treeview), GTK_TREE_MODEL(actions_tree_store));

    dialog_connect_signals(dialog);

    // connect handler to items list to be able to catch situation when all items
    // are deleted and Ok button should be disabled
    g_signal_connect(items_list_store, "row-deleted", G_CALLBACK(on_items_list_row_deleted), dialog);
    g_signal_connect(items_list_store, "row-inserted", G_CALLBACK(on_items_list_row_inserted), dialog);
}

ToolbarItem* run_customization_dialog(GtkWindow *parent, ToolbarItem *current_toolbar_items)
{
    GtkWidget *d = create_customization_dialog();

    gtk_window_set_transient_for(GTK_WINDOW(d), parent);

    GtkListStore *items_list_store = create_items_list_store(current_toolbar_items);
    GtkTreeStore *actions_tree_store = create_actions_tree_store();

    dialog_init(d, items_list_store, actions_tree_store);

    gint dialog_response = gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);

    ToolbarItem *new_toolbar_items = NULL;

    if(dialog_response == GTK_RESPONSE_OK)
    {
        new_toolbar_items = extract_items_from_list(items_list_store);
    }

    g_object_unref(actions_tree_store);
    g_object_unref(items_list_store);

    return new_toolbar_items;
}
