#include "icon_selection_dialog.h"

#include <gtk/gtk.h>
#include <assert.h>

#include "interface.h"
#include "support.h"
#include "utils.h"

enum
{
    ICONS_COL_PIXBUF,
    ICONS_COL_STOCK_ID,
    ICONS_COLS_NUM
};

GtkTreeModel* create_stock_icons_list()
{
    GtkListStore *list = gtk_list_store_new(ICONS_COLS_NUM, GDK_TYPE_PIXBUF, G_TYPE_STRING);

    GSList *stock_ids_list = gtk_stock_list_ids();
    GSList *curr_node = stock_ids_list;
    while(curr_node != NULL)
    {
        char *curr_stock_id = (char*)(curr_node->data);

        GdkPixbuf *icon = create_pixbuf_from_stock_icon(curr_stock_id, GTK_ICON_SIZE_LARGE_TOOLBAR);
        if(icon != NULL)
        {
            GtkTreeIter iter;
            gtk_list_store_append(list, &iter);
            gtk_list_store_set(list, &iter, ICONS_COL_PIXBUF, icon, ICONS_COL_STOCK_ID, curr_stock_id, -1);

            g_object_unref(icon);
        }
        else
        {
            printf("failed to create stock icon %s, skipping\n", curr_stock_id);
        }

        g_free(curr_stock_id);

        curr_node = g_slist_next(curr_node);
    }

    g_slist_free(stock_ids_list);

    return GTK_TREE_MODEL(list);
}

GtkTreePath* stock_icons_list_find(GtkTreeModel *icons_list, const char *stock_id_to_find)
{
    GtkTreePath *path = NULL;
    GtkTreeIter row_iter;

    gboolean res = gtk_tree_model_get_iter_first(icons_list, &row_iter);
    while(res == TRUE)
    {
        char *curr_stock_id = NULL;

        gtk_tree_model_get(icons_list, &row_iter, ICONS_COL_STOCK_ID, &curr_stock_id, -1);

        if(g_str_equal(curr_stock_id, stock_id_to_find))
        {
            path = gtk_tree_model_get_path(icons_list, &row_iter);
            g_free(curr_stock_id);
            break;
        }

        g_free(curr_stock_id);

        res = gtk_tree_model_iter_next(icons_list, &row_iter);
    }

    return path;
}

void select_icon(GtkIconView *stock_iconview, const char *stock_id)
{
    GtkTreeModel *icons_list = gtk_icon_view_get_model(stock_iconview);
    assert(icons_list != NULL);

    GtkTreePath *path = stock_icons_list_find(icons_list, stock_id);

    if(path != NULL)
    {
        gtk_icon_view_select_path(stock_iconview, path);
        gtk_tree_path_free(path);
    }
}

char* get_selected_icon(GtkIconView *stock_iconview)
{
    GtkTreeModel *icons_list = gtk_icon_view_get_model(stock_iconview);
    assert(icons_list != NULL);

    GList *selected_items = gtk_icon_view_get_selected_items(stock_iconview);

    if(selected_items == NULL)
        return NULL;

    GtkTreeIter curr_iter;

    GtkTreePath *first_selected_path = (GtkTreePath*)(selected_items->data);
    gtk_tree_model_get_iter(icons_list, &curr_iter, first_selected_path);

    g_list_free_full(selected_items, (GDestroyNotify)gtk_tree_path_free);

    char *selected_stock_id = NULL;

    gtk_tree_model_get(icons_list, &curr_iter, ICONS_COL_STOCK_ID, &selected_stock_id, -1);

    return selected_stock_id;
}

char* run_icon_selection_dialog(const char *icon_stock_id)
{
    assert(icon_stock_id != NULL);

    GtkWidget *d = create_icon_selection_dialog();

    GtkWidget *stock_iconview = lookup_widget(d, "stock_iconview");
    assert(stock_iconview != NULL);

    GtkTreeModel *stock_icons_list = create_stock_icons_list();
    gtk_icon_view_set_model(GTK_ICON_VIEW(stock_iconview), stock_icons_list);
    gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(stock_iconview), ICONS_COL_PIXBUF);

    select_icon(GTK_ICON_VIEW(stock_iconview), icon_stock_id);

    char *new_icon_stock_id = NULL;

    gint res = gtk_dialog_run(GTK_DIALOG(d));
    if(res == GTK_RESPONSE_OK)
    {
        new_icon_stock_id = get_selected_icon(GTK_ICON_VIEW(stock_iconview));
    }

    gtk_widget_destroy(d);

    return new_icon_stock_id;
}
