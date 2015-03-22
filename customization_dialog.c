#include <gtk/gtk.h>

#include <stdlib.h>
#include <assert.h>

#include "interface.h"
#include "support.h"
#include "toolbar_items.h"

enum
{
    COL_ACTION_TITLE,
    COL_ACTION_NAME,
    COL_ICON_NAME,
    COLS_NUM
};

GtkListStore* create_items_list_store()
{
    GtkListStore *list = gtk_list_store_new(COLS_NUM, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    return list;
}

void fill_items_list(GtkListStore *items_list, GSList *toolbar_items)
{
    GSList *current_node = toolbar_items;

    while(current_node != NULL)
    {
        ToolbarItem *current_item = (ToolbarItem*)(current_node->data);

        GtkTreeIter row_iter;
        gtk_list_store_append(items_list, &row_iter);

        gtk_list_store_set(items_list, &row_iter,
                           COL_ACTION_TITLE, current_item->action_name,
                           COL_ACTION_NAME, current_item->action_name,
                           COL_ICON_NAME, current_item->icon_name,
                           -1);

        current_node = g_slist_next(current_node);
    }
}

void init_items_treeview(GtkTreeView *items_treeview)
{
    GtkTreeViewColumn *action_title_column = gtk_tree_view_column_new();
    GtkCellRenderer *action_title_renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_column_pack_start(action_title_column, action_title_renderer, TRUE);

    gtk_tree_view_column_add_attribute(action_title_column, action_title_renderer, "text", COL_ACTION_TITLE);

    gtk_tree_view_append_column(items_treeview, action_title_column);
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
                           COL_ACTION_NAME, &action_name,
                           COL_ICON_NAME, &icon_name,
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
    GtkTreeView *items_treeview = GTK_TREE_VIEW(user_data);

    GtkTreeModel *model = gtk_tree_view_get_model(items_treeview);
    if(model == NULL)
        return;

    GtkListStore *items_list = GTK_LIST_STORE(model);
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

    g_signal_connect(button_add, "clicked", G_CALLBACK(on_button_add_clicked), items_treeview);
    g_signal_connect(button_remove, "clicked", G_CALLBACK(on_button_remove_clicked), items_treeview);
    g_signal_connect(button_up, "clicked", G_CALLBACK(on_button_up_clicked), items_treeview);
    g_signal_connect(button_down, "clicked", G_CALLBACK(on_button_down_clicked), items_treeview);
}

GSList* run_customization_dialog(GSList *current_toolbar_items)
{
    GtkWidget *d = create_tb_customization_dialog();
    dialog_connect_signals(d);

    GtkWidget *items_treeview = lookup_widget(d, "tb_items_treeview");
    assert(items_treeview != NULL);

    init_items_treeview(GTK_TREE_VIEW(items_treeview));

    GtkListStore *items_list = create_items_list_store();
    fill_items_list(items_list, current_toolbar_items);

    gtk_tree_view_set_model(GTK_TREE_VIEW(items_treeview), GTK_TREE_MODEL(items_list));

    gint dialog_response = gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);

    GSList *new_toolbar_items = NULL;

    if(dialog_response == GTK_RESPONSE_OK)
    {
        new_toolbar_items = extract_items_from_list(items_list);
    }

    g_object_unref(items_list);

    return new_toolbar_items;
}
