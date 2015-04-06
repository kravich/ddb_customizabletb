#include <gtk/gtk.h>

#include <stdlib.h>
#include <assert.h>

#include <glib.h>

#include "interface.h"
#include "support.h"
#include "toolbar_items.h"
#include "utils.h"
#include "icon_selection_dialog.h"

extern DB_functions_t *deadbeef;

enum
{
    ITEMS_COL_ICON_PIXBUF,
    ITEMS_COL_ACTION_TITLE,
    ITEMS_COL_ACTION_CTX_NAME,
    ITEMS_COL_ACTION_NAME,
    ITEMS_COL_ACTION_CTX_ID,
    ITEMS_COL_ICON_NAME,
    ITEMS_COLS_NUM
};

enum
{
    ACTIONS_COL_TITLE,
    ACTIONS_COL_NAME,
    ACTIONS_COL_CTX,
    ACTIONS_COLS_NUM
};

enum
{
    CONTEXT_COL_NAME,
    CONTEXT_COL_ID
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
                                                  G_TYPE_STRING);

    ToolbarItem *current_item = toolbar_items;
    while(current_item != NULL)
    {
        GdkPixbuf *icon = NULL;

        if(current_item->action != NULL)
            icon = create_pixbuf_from_stock_icon(current_item->icon_name, GTK_ICON_SIZE_BUTTON);
        else
            icon = create_pixbuf_from_stock_icon("gtk-no", GTK_ICON_SIZE_BUTTON);

        GtkTreeIter row_iter;
        gtk_list_store_append(items_list, &row_iter);

        gtk_list_store_set(items_list, &row_iter,
                           ITEMS_COL_ICON_PIXBUF, icon,
                           ITEMS_COL_ACTION_TITLE, current_item->action_name,
                           ITEMS_COL_ACTION_CTX_NAME, get_context_name_by_id(current_item->action_context),
                           ITEMS_COL_ACTION_NAME, current_item->action_name,
                           ITEMS_COL_ACTION_CTX_ID, current_item->action_context,
                           ITEMS_COL_ICON_NAME, current_item->icon_name,
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

            if(name_parts[1] != NULL)   // action title haves group specified in name
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

void init_items_treeview(GtkTreeView *items_treeview)
{
    GtkTreeViewColumn *action_title_column = gtk_tree_view_column_new();
    GtkCellRenderer *icon_renderer = gtk_cell_renderer_pixbuf_new();
    GtkCellRenderer *action_title_renderer = gtk_cell_renderer_text_new();
    GtkCellRenderer *action_ctx_renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_column_pack_start(action_title_column, icon_renderer, FALSE);
    gtk_tree_view_column_pack_start(action_title_column, action_title_renderer, TRUE);
    gtk_tree_view_column_pack_start(action_title_column, action_ctx_renderer, FALSE);

    gtk_tree_view_column_add_attribute(action_title_column, icon_renderer, "pixbuf", ITEMS_COL_ICON_PIXBUF);
    gtk_tree_view_column_add_attribute(action_title_column, action_title_renderer, "text", ITEMS_COL_ACTION_TITLE);
    gtk_tree_view_column_add_attribute(action_title_column, action_ctx_renderer, "text", ITEMS_COL_ACTION_CTX_NAME);

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

        ToolbarItem *item = (ToolbarItem*)malloc(sizeof(ToolbarItem));
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

    GtkWidget *items_treeview = lookup_widget(d, "tb_items_treeview");
    GtkWidget *actions_treeview = lookup_widget(d, "actions_treeview");

    assert(items_treeview != NULL);
    assert(actions_treeview != NULL);

    GtkTreeModel *items_list_store = gtk_tree_view_get_model(GTK_TREE_VIEW(items_treeview));
    GtkTreeModel *actions_tree_store = gtk_tree_view_get_model(GTK_TREE_VIEW(actions_treeview));

    GtkTreeSelection *actions_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(actions_treeview));

    if(gtk_tree_selection_count_selected_rows(actions_selection) != 1)
    {
        printf("No action is selected\n");
        return;
    }

    GtkTreeIter current_action_iter;

    gboolean res = gtk_tree_selection_get_selected(actions_selection, &actions_tree_store, &current_action_iter);
    assert(res == TRUE);

    char *action_name = NULL;
    int action_ctx = 0;
    gtk_tree_model_get(GTK_TREE_MODEL(actions_tree_store), &current_action_iter,
                       ACTIONS_COL_NAME, &action_name,
                       ACTIONS_COL_CTX, &action_ctx,
                       -1);

    if(g_str_equal(action_name, ""))
    {
        printf("Group entry is selected, can't add item\n");
        g_free(action_name);
        return;
    }

    DB_plugin_action_t *action = find_action(action_name);
    if(action == NULL)
    {
        printf("Can't find action for id %s\n", action_name);
        g_free(action_name);
        return;
    }

    GdkPixbuf *new_item_icon = create_pixbuf_from_stock_icon("gtk-missing-image", GTK_ICON_SIZE_BUTTON);

    GtkTreeIter new_item_iter;
    gtk_list_store_append(GTK_LIST_STORE(items_list_store), &new_item_iter);

    gtk_list_store_set(GTK_LIST_STORE(items_list_store), &new_item_iter,
                       ITEMS_COL_ACTION_TITLE, action->title,
                       ITEMS_COL_ACTION_NAME, action_name,
                       ITEMS_COL_ICON_NAME, "gtk-missing-image",
                       ITEMS_COL_ICON_PIXBUF, new_item_icon,
                       ITEMS_COL_ACTION_CTX_NAME, get_context_name_by_id(action_ctx),
                       ITEMS_COL_ACTION_CTX_ID, action_ctx,
                       -1);

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

void on_button_change_icon_clicked(GtkButton *button, gpointer user_data)
{
    GtkTreeView *items_treeview = GTK_TREE_VIEW(user_data);
    GtkTreeModel *items_list_store = gtk_tree_view_get_model(items_treeview);

    GtkTreeSelection *items_selection = gtk_tree_view_get_selection(items_treeview);

    if(gtk_tree_selection_count_selected_rows(items_selection) != 1)
        return;

    GtkTreeIter selected_item_iter;
    gtk_tree_selection_get_selected(items_selection, &items_list_store, &selected_item_iter);

    char *icon_name = NULL;
    gtk_tree_model_get(items_list_store, &selected_item_iter,
                       ITEMS_COL_ICON_NAME, &icon_name,
                       -1);

    char *new_icon_name = run_icon_selection_dialog(icon_name);
    g_free(icon_name);

    if(new_icon_name != NULL)
    {
        GdkPixbuf *new_icon_pixbuf = create_pixbuf_from_stock_icon(new_icon_name, GTK_ICON_SIZE_BUTTON);

        gtk_list_store_set(GTK_LIST_STORE(items_list_store), &selected_item_iter,
                           ITEMS_COL_ICON_PIXBUF, new_icon_pixbuf,
                           ITEMS_COL_ICON_NAME, new_icon_name,
                           -1);

        g_object_unref(new_icon_pixbuf);
        g_free(new_icon_name);
    }
}

void dialog_connect_signals(GtkWidget *dialog)
{
    GtkWidget *button_add = lookup_widget(dialog, "button_add");
    GtkWidget *button_remove = lookup_widget(dialog, "button_remove");
    GtkWidget *button_up = lookup_widget(dialog, "button_up");
    GtkWidget *button_down = lookup_widget(dialog, "button_down");
    GtkWidget *button_change_icon = lookup_widget(dialog, "button_change_icon");
    GtkWidget *items_treeview = lookup_widget(dialog, "tb_items_treeview");

    assert(button_add != NULL);
    assert(button_remove != NULL);
    assert(button_up != NULL);
    assert(button_down != NULL);
    assert(button_change_icon != NULL);
    assert(items_treeview != NULL);

    g_signal_connect(button_add, "clicked", G_CALLBACK(on_button_add_clicked), dialog);
    g_signal_connect(button_remove, "clicked", G_CALLBACK(on_button_remove_clicked), items_treeview);
    g_signal_connect(button_up, "clicked", G_CALLBACK(on_button_up_clicked), items_treeview);
    g_signal_connect(button_down, "clicked", G_CALLBACK(on_button_down_clicked), items_treeview);
    g_signal_connect(button_change_icon, "clicked", G_CALLBACK(on_button_change_icon_clicked), items_treeview);
}

void dialog_init(GtkWidget *dialog, GtkListStore *items_list_store, GtkTreeStore *actions_tree_store)
{
    GtkWidget *items_treeview = lookup_widget(dialog, "tb_items_treeview");
    GtkWidget *actions_treeview = lookup_widget(dialog, "actions_treeview");

    assert(items_treeview != NULL);
    assert(actions_treeview != NULL);

    init_items_treeview(GTK_TREE_VIEW(items_treeview));
    gtk_tree_view_set_model(GTK_TREE_VIEW(items_treeview), GTK_TREE_MODEL(items_list_store));

    init_actions_treeview(GTK_TREE_VIEW(actions_treeview));
    gtk_tree_view_set_model(GTK_TREE_VIEW(actions_treeview), GTK_TREE_MODEL(actions_tree_store));

    dialog_connect_signals(dialog);
}

ToolbarItem* run_customization_dialog(ToolbarItem *current_toolbar_items)
{
    GtkWidget *d = create_tb_customization_dialog();

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
