#include "w_toolbar.h"

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

extern DB_functions_t *deadbeef;
extern ddb_gtkui_t *gtkui;

const char* w_toolbar_load(ddb_gtkui_widget_t *w, const char *type, const char *s);
void w_toolbar_save(ddb_gtkui_widget_t *w, char *s, int sz);
void w_toolbar_init(ddb_gtkui_widget_t *w);
void w_toolbar_initmenu(ddb_gtkui_widget_t *w, GtkWidget *menu);
void w_toolbar_destroy(ddb_gtkui_widget_t *w);

GSList* create_default_toolbar_items();

typedef struct
{
    char *action_name;
    char *icon_name;
} ToolbarItem;

typedef struct
{
    ddb_gtkui_widget_t base;
    GSList *items_list;
} w_toolbar_t;

ddb_gtkui_widget_t* w_toolbar_create()
{
    w_toolbar_t *w = malloc(sizeof(w_toolbar_t));
    memset(w, 0, sizeof(w_toolbar_t));

    w->base.load = w_toolbar_load;
    w->base.save = w_toolbar_save;
    w->base.init = w_toolbar_init;
    w->base.initmenu = w_toolbar_initmenu;
    w->base.destroy = w_toolbar_destroy;

    w->base.flags = DDB_GTKUI_WIDGET_FLAG_NON_EXPANDABLE;
    w->base.widget = gtk_hbox_new(FALSE, 0);
    gtk_widget_show((GtkWidget*)(w->base.widget));

    gtkui->w_override_signals(w->base.widget, w);

    w->items_list = create_default_toolbar_items();

    return (ddb_gtkui_widget_t*)w;
}

void free_item_data_cb(gpointer data)
{
    printf("Freeing toolbar item\n");

    ToolbarItem *item = (ToolbarItem*)data;
    assert(item != NULL);

    g_free(item->action_name);
    g_free(item->icon_name);
    g_free(item);
}

void free_items_list(GSList *items_list)
{
    g_slist_free_full(items_list, free_item_data_cb);
}

void w_toolbar_destroy(ddb_gtkui_widget_t *w)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    free_items_list(toolbar->items_list);
}

GSList* create_default_toolbar_items()
{
    GSList *toolbar_items = NULL;
    ToolbarItem *item = NULL;

    // TODO: add check for malloc() return value

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("stop");
    item->icon_name = g_strdup("gtk-media-stop");

    toolbar_items = g_slist_append(toolbar_items, item);

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("play");
    item->icon_name = g_strdup("gtk-media-play");

    toolbar_items = g_slist_append(toolbar_items, item);

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("toggle_pause");
    item->icon_name = g_strdup("gtk-media-pause");

    toolbar_items = g_slist_append(toolbar_items, item);

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("prev");
    item->icon_name = g_strdup("gtk-media-previous");

    toolbar_items = g_slist_append(toolbar_items, item);

    item = malloc(sizeof(ToolbarItem));
    item->action_name = g_strdup("next");
    item->icon_name = g_strdup("gtk-media-next");

    toolbar_items = g_slist_append(toolbar_items, item);

    return toolbar_items;
}

const char* extract_widget_params(const char *s, char *buff, size_t buff_size)
{
    const char *p = s;
    unsigned int n = 0;

    while((*p != '\0') &&
          (*p != '{'))
    {
        if(n < (buff_size - 1))
        {
            buff[n] = *p;
            n++;
        }

        p++;
    }

    buff[n] = '\0';

    if(*p == '{')
        p++;

    return p;
}

void extract_layout_param(const char *params, char *buff, size_t buff_size)
{
    assert(buff_size != 0);

    buff[0] = '\0';

    char key[256] = {0};
    char val[256] = {0};

    // TODO: use more reliable way to parse layout
    int vars_read = sscanf(params, "%255[^=]=\"%255[^=\"]\"", key, val);
    if(vars_read != 2)
    {
        printf("vars read: %d\n", vars_read);
        return;
    }

    g_strstrip(key);
    g_strstrip(val);

    if(strcmp(key, "layout") == 0)
    {
        strncpy(buff, val, buff_size - 1);
    }
}

GSList* toolbar_items_deserialize(char *layout);

const char* w_toolbar_load(ddb_gtkui_widget_t *w, const char *type, const char *s)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    #define PARAMS_SIZE 256
    char params[PARAMS_SIZE] = {0};
    const char *p = extract_widget_params(s, params, PARAMS_SIZE);

    char layout_str[256];
    extract_layout_param(params, layout_str, 256);

    GSList *saved_toolbar_items = toolbar_items_deserialize(layout_str);

    if(saved_toolbar_items != NULL)
    {
        free_items_list(toolbar->items_list);
        toolbar->items_list = saved_toolbar_items;
    }

    return p;
}

void toolbar_items_serialize(GSList *toolbar_items, char *buff, size_t buff_size)
{
    assert(buff_size != 0);

    size_t buff_free_space = buff_size - 1;
    buff[0] = '\0';

    GSList *current_node = toolbar_items;
    while(current_node != NULL)
    {
        ToolbarItem *current_item = (ToolbarItem*)current_node->data;

        char *fmt = NULL;

        if(current_node->next != NULL)
            fmt = "%s|%s,";
        else
            fmt = "%s|%s";

        char *item_str = g_strdup_printf(fmt, current_item->action_name, current_item->icon_name);

        size_t chars_to_add = strlen(item_str);

        strncat(buff, item_str, buff_free_space);

        g_free(item_str);

        if(buff_free_space > chars_to_add)
            buff_free_space -= chars_to_add;
        else
            break;

        current_node = g_slist_next(current_node);
    }
}

GSList* toolbar_items_deserialize(char *layout)
{
    char **elements = g_strsplit(layout, ",", -1);

    GSList *toolbar_items = NULL;
    char **current_element = elements;
    while(*current_element != NULL)
    {
        ToolbarItem *item = malloc(sizeof(ToolbarItem));

        char **parts = g_strsplit(*current_element, "|", 2);

        if(parts[0] == NULL || parts[1] == NULL || item == NULL)
        {
            g_strfreev(parts);
            g_strfreev(elements);
            free_items_list(toolbar_items);
            return NULL;
        }

        item->action_name = g_strdup(parts[0]); // TODO: should we check the return value of g_strdup()?
        item->icon_name = g_strdup(parts[1]);

        toolbar_items = g_slist_append(toolbar_items, item);

        current_element++;
    }

    return toolbar_items;
}

void w_toolbar_save(ddb_gtkui_widget_t *w, char *s, int sz)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    char serialized_layout[256];
    toolbar_items_serialize(toolbar->items_list, serialized_layout, 256);

    char *layout_param = g_strdup_printf(" layout=\"%s\"", serialized_layout);
    strncat(s, layout_param, sz);
    g_free(layout_param);
}

GtkWidget* create_image_by_name(const char *button_icon_name)
{
    GtkWidget *image = NULL;

    if(button_icon_name != NULL)
    {
        // try to find icon name in stock items
        GtkStockItem stock_item;
        if(gtk_stock_lookup(button_icon_name, &stock_item))
        {
            image = gtk_image_new_from_stock(button_icon_name, GTK_ICON_SIZE_BUTTON);
        }

        // if previous attempt failed, treat icon name
        // as file name and try to load it from pixmap directories
        /*if(image == NULL)
        {
            char *path_to_pixmap_file = find_pixmap_file(button_icon_name);
            if(path_to_pixmap_file != NULL)
            {
                image = gtk_image_new_from_file(path_to_pixmap_file);
                g_free(path_to_pixmap_file);
            }
        }*/

        // There is an another option - treat icon name as absolute path.
        // Should it be allowed?
    }

    // if everything failed - create a 'missing image' icon
    if(image == NULL)
    {
        image = gtk_image_new_from_stock("gtk-missing-image", GTK_ICON_SIZE_BUTTON);
    }

    return image;
}

DB_plugin_action_t* find_action(char *action_name)
{
    DB_plugin_t **plugins = deadbeef->plug_get_list();

    for(int i = 0; plugins[i] != NULL; i++)
    {
        if(plugins[i]->get_actions == NULL)
            continue;

        DB_plugin_action_t *plugin_actions = plugins[i]->get_actions(NULL);

        DB_plugin_action_t *current_action = plugin_actions;
        while(current_action != NULL)
        {
            if(g_str_equal(current_action->name, action_name))
            {
                return current_action;
            }

            current_action = current_action->next;
        }
    }

    return NULL;
}

void toolbar_button_activate_action(GtkButton *button, gpointer user_data)
{
    DB_plugin_action_t *action = (DB_plugin_action_t*)user_data;
    printf("Activate action\n");
}

void toolbar_button_no_action_msg(GtkButton *button, gpointer user_data)
{
    printf("No action\n");
}

void fill_toolbar(w_toolbar_t *toolbar)
{
    GtkBox *hbox = GTK_BOX(toolbar->base.widget);

    GSList *current_node = toolbar->items_list;
    while(current_node != NULL)
    {
        ToolbarItem *current_item = (ToolbarItem*)(current_node->data);

        GtkWidget *button = gtk_button_new();
        gtk_widget_set_can_focus(button, FALSE);
        gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
        gtk_widget_show(button);

        gtk_box_pack_start(hbox, button, FALSE, FALSE, 0);

        DB_plugin_action_t *action = find_action(current_item->action_name);

        GtkWidget *image = NULL;
        if(action != NULL)
        {
            image = create_image_by_name(current_item->icon_name);
            g_signal_connect(button, "clicked", G_CALLBACK(toolbar_button_activate_action), action);
        }
        else
        {
            image = gtk_image_new_from_stock("gtk-no", GTK_ICON_SIZE_BUTTON);
            g_signal_connect(button, "clicked", G_CALLBACK(toolbar_button_no_action_msg), NULL);
        }

        gtk_widget_show(image);
        gtk_container_add(GTK_CONTAINER(button), image);

        current_node = g_slist_next(current_node);
    }
}

void w_toolbar_init(ddb_gtkui_widget_t *w)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    fill_toolbar(toolbar);

    gtk_container_foreach(GTK_CONTAINER(toolbar->base.widget), gtkui->w_override_signals, w);
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
