#include <gtk/gtk.h>

#include <stdlib.h>
#include <assert.h>

#include <glib.h>

#include "interface.h"
#include "support.h"
#include "toolbar_items.h"
#include "utils.h"

extern DB_functions_t *deadbeef;

enum
{
    ITEMS_COL_ACTION_TITLE,
    ITEMS_COL_ACTION_NAME,
    ITEMS_COL_ICON_NAME,
    ITEMS_COL_ICON_PIXBUF,
    ITEMS_COLS_NUM
};

enum
{
    ACTIONS_COL_ACTION_TITLE,
    ACTIONS_COL_ACTION_NAME,
    ACTIONS_COLS_NUM
};

enum
{
    CONTEXT_COL_NAME,
    CONTEXT_COL_ID
};

GtkListStore* create_items_list_store(GSList *toolbar_items)
{
    GtkListStore *items_list = gtk_list_store_new(ITEMS_COLS_NUM, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF);

    GSList *current_node = toolbar_items;
    while(current_node != NULL)
    {
        ToolbarItem *current_item = (ToolbarItem*)(current_node->data);

        GdkPixbuf *icon = NULL;
        DB_plugin_action_t *action = find_action(current_item->action_name);

        if(action != NULL)
            icon = create_pixbuf_from_stock_icon(current_item->icon_name);
        else
            icon = create_pixbuf_from_stock_icon("gtk-no");

        GtkTreeIter row_iter;
        gtk_list_store_append(items_list, &row_iter);

        gtk_list_store_set(items_list, &row_iter,
                           ITEMS_COL_ACTION_TITLE, current_item->action_name,
                           ITEMS_COL_ACTION_NAME, current_item->action_name,
                           ITEMS_COL_ICON_NAME, current_item->icon_name,
                           ITEMS_COL_ICON_PIXBUF, icon,
                           -1);

        g_object_unref(icon);

        current_node = g_slist_next(current_node);
    }

    return items_list;
}

gboolean actions_tree_find_group_entry(GtkTreeModel *tree_model, char *required_group_name, GtkTreeIter *iter)
{
    gboolean iter_found = FALSE;

    gboolean res = gtk_tree_model_get_iter_first(tree_model, iter);
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
    GtkTreeStore *actions_tree_store = gtk_tree_store_new(ACTIONS_COLS_NUM, G_TYPE_STRING, G_TYPE_STRING);

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

            if(name_parts[0] == NULL || name_parts[1] == NULL)
            {
                printf("Warning: action %s does not have group specified in name\n", curr_action->title);
                curr_action = curr_action->next;
                g_strfreev(name_parts);
                continue;
            }

            char *group_name = name_parts[0];
            char *action_title = name_parts[1];

            GtkTreeIter group_iter, action_iter;

            gboolean group_entry_exist = actions_tree_find_group_entry(GTK_TREE_MODEL(actions_tree_store),
                                                                       group_name,
                                                                       &group_iter);

            if(!group_entry_exist)
            {
                gtk_tree_store_append(actions_tree_store, &group_iter, NULL);
                gtk_tree_store_set(actions_tree_store, &group_iter,
                                   ACTIONS_COL_ACTION_TITLE, group_name,
                                   ACTIONS_COL_ACTION_NAME, "",
                                   -1);
            }

            gtk_tree_store_append(actions_tree_store, &action_iter, &group_iter);
            gtk_tree_store_set(actions_tree_store, &action_iter,
                               ACTIONS_COL_ACTION_TITLE, action_title,
                               ACTIONS_COL_ACTION_NAME, curr_action->name,
                               -1);

            g_strfreev(name_parts);

            curr_action = curr_action->next;
        }
    }

    return actions_tree_store;
}

void init_items_treeview(GtkTreeView *items_treeview)
{
    GtkTreeViewColumn *action_title_column = gtk_tree_view_column_new();
    GtkCellRenderer *icon_renderer = gtk_cell_renderer_pixbuf_new();
    GtkCellRenderer *action_title_renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_column_pack_start(action_title_column, icon_renderer, FALSE);
    gtk_tree_view_column_pack_start(action_title_column, action_title_renderer, TRUE);

    gtk_tree_view_column_add_attribute(action_title_column, icon_renderer, "pixbuf", ITEMS_COL_ICON_PIXBUF);
    gtk_tree_view_column_add_attribute(action_title_column, action_title_renderer, "text", ITEMS_COL_ACTION_TITLE);

    gtk_tree_view_append_column(items_treeview, action_title_column);
}

void init_actions_treeview(GtkTreeView *actions_tree_view)
{
    GtkTreeViewColumn *action_name_column = gtk_tree_view_column_new();
    GtkCellRenderer *action_name_renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_column_pack_end(action_name_column, action_name_renderer, TRUE);

    gtk_tree_view_column_add_attribute(action_name_column, action_name_renderer, "text", ACTIONS_COL_ACTION_TITLE);

    gtk_tree_view_append_column(actions_tree_view, action_name_column);
}

void init_context_combobox(GtkComboBox *context_combobox)
{
    GtkListStore *context_list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);

    GtkTreeIter iter;

    gtk_list_store_append(context_list_store, &iter);
    gtk_list_store_set(context_list_store, &iter,
                       CONTEXT_COL_NAME, "Main",
                       CONTEXT_COL_ID, DDB_ACTION_CTX_MAIN,
                       -1);

    gtk_list_store_append(context_list_store, &iter);
    gtk_list_store_set(context_list_store, &iter,
                       CONTEXT_COL_NAME, "Selection",
                       CONTEXT_COL_ID, DDB_ACTION_CTX_SELECTION,
                       -1);

    gtk_list_store_append(context_list_store, &iter);
    gtk_list_store_set(context_list_store, &iter,
                       CONTEXT_COL_NAME, "Playlist",
                       CONTEXT_COL_ID, DDB_ACTION_CTX_PLAYLIST,
                       -1);

    gtk_list_store_append(context_list_store, &iter);
    gtk_list_store_set(context_list_store, &iter,
                       CONTEXT_COL_NAME, "Nowplaying",
                       CONTEXT_COL_ID, DDB_ACTION_CTX_NOWPLAYING,
                       -1);

    gtk_combo_box_set_model(context_combobox, GTK_TREE_MODEL(context_list_store));
    g_object_unref(context_list_store);

    gtk_combo_box_set_active(context_combobox, 0);
}

GSList* extract_items_from_list(GtkListStore *items_list)
{
    GtkTreeIter row_iter;
    GSList *new_items = NULL;

    gboolean res = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(items_list), &row_iter);

    while(res == TRUE)
    {
        char *action_name = NULL;
        char *icon_name = NULL;

        gtk_tree_model_get(GTK_TREE_MODEL(items_list), &row_iter,
                           ITEMS_COL_ACTION_NAME, &action_name,
                           ITEMS_COL_ICON_NAME, &icon_name,
                           -1);

        ToolbarItem *item = (ToolbarItem*)malloc(sizeof(ToolbarItem));
        item->action_name = action_name;
        item->icon_name = icon_name;

        new_items = g_slist_append(new_items, item);

        res = gtk_tree_model_iter_next(GTK_TREE_MODEL(items_list), &row_iter);
    }

    return new_items;
}

void on_button_add_clicked(GtkButton *button, gpointer user_data)
{
    assert(GTK_IS_DIALOG(user_data));

    GtkWidget *d = GTK_WIDGET(user_data);

    GtkWidget *items_treeview = lookup_widget(d, "tb_items_treeview");
    GtkWidget *actions_treeview = lookup_widget(d, "actions_treeview");
    GtkWidget *context_combobox = lookup_widget(d, "context_combobox");

    assert(items_treeview != NULL);
    assert(actions_treeview != NULL);
    assert(context_combobox != NULL);

    GtkTreeModel *items_list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(items_treeview));
    GtkTreeModel *actions_tree_store = gtk_tree_view_get_model(GTK_TREE_VIEW(actions_treeview));
    GtkTreeModel *context_list_store = gtk_combo_box_get_model(GTK_COMBO_BOX(context_combobox));

    GtkTreeSelection *actions_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(actions_treeview));

    if(gtk_tree_selection_count_selected_rows(actions_selection) != 1)
    {
        printf("No action is selected\n");
        return;
    }

    if(gtk_combo_box_get_active(GTK_COMBO_BOX(context_combobox)) < 0)
    {
        printf("No context option is selected\n");
        return;
    }

    GtkTreeIter current_action_iter;
    GtkTreeIter current_context_iter;

    gboolean res = TRUE;
    res = res && gtk_tree_selection_get_selected(actions_selection, &actions_tree_store, &current_action_iter);
    res = res && gtk_combo_box_get_active_iter(GTK_COMBO_BOX(context_combobox), &current_context_iter);
    assert(res == TRUE);

    char *action_title = NULL;
    char *action_name = NULL;
    gtk_tree_model_get(GTK_TREE_MODEL(actions_tree_store), &current_action_iter,
                       ACTIONS_COL_ACTION_TITLE, &action_title,
                       ACTIONS_COL_ACTION_NAME, &action_name,
                       -1);

    if(g_str_equal(action_name, ""))
    {
        printf("Group entry is selected, can't add item\n");
        g_free(action_title);
        g_free(action_name);
        return;
    }

    GdkPixbuf *new_item_icon = create_pixbuf_from_stock_icon("gtk-missing-image");

    GtkTreeIter new_item_iter;
    gtk_list_store_append(GTK_LIST_STORE(items_list_store), &new_item_iter);

    gtk_list_store_set(GTK_LIST_STORE(items_list_store), &new_item_iter,
                       ITEMS_COL_ACTION_TITLE, action_name,
                       ITEMS_COL_ACTION_NAME, action_name,
                       ITEMS_COL_ICON_NAME, "gtk-missing-image",
                       ITEMS_COL_ICON_PIXBUF, new_item_icon,
                       -1);

    g_free(action_title);
    g_free(action_name);
    g_object_unref(new_item_icon);
}

void on_button_remove_clicked(GtkButton *button, gpointer user_data)
{
    GtkTreeView *items_treeview = GTK_TREE_VIEW(user_data);

    GtkTreeModel *model = gtk_tree_view_get_model(items_treeview);
    if(model == NULL)
        return;

    GtkListStore *items_list = GTK_LIST_STORE(model);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(items_treeview);
    if(gtk_tree_selection_count_selected_rows(selection) != 1)
        return;

    GtkTreeIter selected_iter;
    gtk_tree_selection_get_selected(selection, &model, &selected_iter);

    gtk_list_store_remove(items_list, &selected_iter);
}

void on_button_up_clicked(GtkButton *button, gpointer user_data)
{
    GtkTreeView *items_treeview = GTK_TREE_VIEW(user_data);

    GtkTreeModel *model = gtk_tree_view_get_model(items_treeview);
    if(model == NULL)
        return;

    GtkListStore *items_list = GTK_LIST_STORE(model);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(items_treeview);
    if(gtk_tree_selection_count_selected_rows(selection) != 1)
        return;

    GtkTreeIter selected_iter;
    gtk_tree_selection_get_selected(selection, &model, &selected_iter);

    GtkTreePath *prev_row_path = gtk_tree_model_get_path(model, &selected_iter);
    gtk_tree_path_prev(prev_row_path);

    GtkTreeIter prev_row_iter;
    if(!gtk_tree_model_get_iter(model, &prev_row_iter, prev_row_path))
    {
        gtk_tree_path_free(prev_row_path);
        return;
    }

    gtk_tree_path_free(prev_row_path);

    gtk_list_store_swap(items_list, &selected_iter, &prev_row_iter);
}

void on_button_down_clicked(GtkButton *button, gpointer user_data)
{
    GtkTreeView *items_treeview = GTK_TREE_VIEW(user_data);

    GtkTreeModel *model = gtk_tree_view_get_model(items_treeview);
    if(model == NULL)
        return;

    GtkListStore *items_list = GTK_LIST_STORE(model);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(items_treeview);
    if(gtk_tree_selection_count_selected_rows(selection) != 1)
        return;

    GtkTreeIter selected_iter;
    gtk_tree_selection_get_selected(selection, &model, &selected_iter);

    GtkTreePath *next_row_path = gtk_tree_model_get_path(model, &selected_iter);
    gtk_tree_path_next(next_row_path);

    GtkTreeIter next_row_iter;
    if(!gtk_tree_model_get_iter(model, &next_row_iter, next_row_path))
    {
        gtk_tree_path_free(next_row_path);
        return;
    }

    gtk_tree_path_free(next_row_path);

    gtk_list_store_swap(items_list, &selected_iter, &next_row_iter);
}

void dialog_connect_signals(GtkWidget *dialog)
{
    GtkWidget *button_add = lookup_widget(dialog, "button_add");
    GtkWidget *button_remove = lookup_widget(dialog, "button_remove");
    GtkWidget *button_up = lookup_widget(dialog, "button_up");
    GtkWidget *button_down = lookup_widget(dialog, "button_down");
    GtkWidget *items_treeview = lookup_widget(dialog, "tb_items_treeview");

    assert(button_add != NULL);
    assert(button_remove != NULL);
    assert(button_up != NULL);
    assert(button_down != NULL);
    assert(items_treeview != NULL);

    g_signal_connect(button_add, "clicked", G_CALLBACK(on_button_add_clicked), dialog);
    g_signal_connect(button_remove, "clicked", G_CALLBACK(on_button_remove_clicked), items_treeview);
    g_signal_connect(button_up, "clicked", G_CALLBACK(on_button_up_clicked), items_treeview);
    g_signal_connect(button_down, "clicked", G_CALLBACK(on_button_down_clicked), items_treeview);
}

void dialog_init(GtkWidget *dialog, GtkListStore *items_list_store, GtkTreeStore *actions_tree_store)
{
    GtkWidget *items_treeview = lookup_widget(dialog, "tb_items_treeview");
    GtkWidget *actions_treeview = lookup_widget(dialog, "actions_treeview");
    GtkWidget *context_combobox = lookup_widget(dialog, "context_combobox");

    assert(items_treeview != NULL);
    assert(actions_treeview != NULL);
    assert(context_combobox != NULL);

    init_items_treeview(GTK_TREE_VIEW(items_treeview));
    gtk_tree_view_set_model(GTK_TREE_VIEW(items_treeview), GTK_TREE_MODEL(items_list_store));

    init_actions_treeview(GTK_TREE_VIEW(actions_treeview));
    gtk_tree_view_set_model(GTK_TREE_VIEW(actions_treeview), GTK_TREE_MODEL(actions_tree_store));

    init_context_combobox(GTK_COMBO_BOX(context_combobox));

    dialog_connect_signals(dialog);
}

GSList* run_customization_dialog(GSList *current_toolbar_items)
{
    GtkWidget *d = create_tb_customization_dialog();

    GtkListStore *items_list_store = create_items_list_store(current_toolbar_items);
    GtkTreeStore *actions_tree_store = create_actions_tree_store();

    dialog_init(d, items_list_store, actions_tree_store);

    gint dialog_response = gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);

    GSList *new_toolbar_items = NULL;

    if(dialog_response == GTK_RESPONSE_OK)
    {
        new_toolbar_items = extract_items_from_list(items_list_store);
    }

    g_object_unref(actions_tree_store);
    g_object_unref(items_list_store);

    return new_toolbar_items;
}
