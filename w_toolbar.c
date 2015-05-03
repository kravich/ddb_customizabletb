#include "w_toolbar.h"

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "toolbar_items.h"
#include "customization_dialog.h"
#include "utils.h"

#define TMP_BUFF_SIZE 256

extern DB_functions_t *deadbeef;
extern ddb_gtkui_t *gtkui;

typedef struct
{
    ddb_gtkui_widget_t base;
    ToolbarItem *items_list;
} w_toolbar_t;


void w_toolbar_destroy(ddb_gtkui_widget_t *w)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    free_items_list(toolbar->items_list);
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

    char key[TMP_BUFF_SIZE] = {0};
    char val[TMP_BUFF_SIZE] = {0};

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

const char* w_toolbar_load(ddb_gtkui_widget_t *w, const char *type, const char *s)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    char params[TMP_BUFF_SIZE] = {0};
    const char *p = extract_widget_params(s, params, TMP_BUFF_SIZE);

    char layout_str[TMP_BUFF_SIZE] = {0};
    extract_layout_param(params, layout_str, TMP_BUFF_SIZE);

    ToolbarItem *saved_toolbar_items = toolbar_items_deserialize(layout_str);

    if(saved_toolbar_items != NULL)
    {
        free_items_list(toolbar->items_list);
        toolbar->items_list = saved_toolbar_items;
    }

    return p;
}

void w_toolbar_save(ddb_gtkui_widget_t *w, char *s, int sz)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    char serialized_layout[TMP_BUFF_SIZE] = {0};
    toolbar_items_serialize(toolbar->items_list, serialized_layout, TMP_BUFF_SIZE);

    char *layout_param = g_strdup_printf(" layout=\"%s\"", serialized_layout);
    strncat(s, layout_param, sz);
    g_free(layout_param);
}

void toolbar_button_activate_action(GtkButton *button, gpointer user_data)
{
    ToolbarItem *item = (ToolbarItem*)user_data;

    assert(item->action != NULL);

    printf("Activating \"%s\" in context %d\n", item->action_name, item->action_context);

    if(item->action->callback2)
        item->action->callback2(item->action, item->action_context);
}

void toolbar_button_no_action_msg(GtkButton *button, gpointer user_data)
{
    printf("No action\n");
}

void fill_toolbar(w_toolbar_t *toolbar)
{
    GtkBox *hbox = GTK_BOX(toolbar->base.widget);

    ToolbarItem *current_item = toolbar->items_list;
    while(current_item != NULL)
    {
        GtkWidget *button = gtk_button_new();
        gtk_widget_set_can_focus(button, FALSE);
        gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
        gtk_widget_show(button);

        gtk_box_pack_start(hbox, button, FALSE, FALSE, 0);

        GtkWidget *image = NULL;
        if(current_item->action != NULL)
        {
            image = create_image_by_name(current_item->icon_name, TOOLBAR_ICON_SIZE);
            g_signal_connect(button, "clicked", G_CALLBACK(toolbar_button_activate_action), current_item);
        }
        else
        {
            image = create_image_by_name("process-stop", TOOLBAR_ICON_SIZE);
            g_signal_connect(button, "clicked", G_CALLBACK(toolbar_button_no_action_msg), NULL);
        }

        assert(image != NULL);

        gtk_widget_show(image);
        gtk_container_add(GTK_CONTAINER(button), image);

        current_item = current_item->next;
    }

    // this is required since deadbeef sets size_request explicitly
    gtk_widget_set_size_request(toolbar->base.widget, -1, -1);
}

void w_toolbar_init(ddb_gtkui_widget_t *w)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)w;

    fill_toolbar(toolbar);

    gtk_container_foreach(GTK_CONTAINER(toolbar->base.widget), gtkui->w_override_signals, w);
}

void empty_hbox(GtkBox *hbox)
{
    GList *list = gtk_container_get_children(GTK_CONTAINER(hbox));

    for(GList *it = list; it != NULL; it = g_list_next(it)) {
        gtk_widget_destroy(GTK_WIDGET(it->data));
    }

    g_list_free(list);
}

void w_toolbar_set_new_items(w_toolbar_t *toolbar, ToolbarItem *new_toolbar_items)
{
    printf("Updating toolbar items\n");

    if(new_toolbar_items == NULL)
        return;

    free_items_list(toolbar->items_list);
    toolbar->items_list = new_toolbar_items;

    empty_hbox(GTK_BOX(toolbar->base.widget));
    fill_toolbar(toolbar);
    gtk_container_foreach(GTK_CONTAINER(toolbar->base.widget), gtkui->w_override_signals, toolbar);
}

void on_customize_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    w_toolbar_t *toolbar = (w_toolbar_t*)user_data;

    ToolbarItem *new_toolbar_items = run_customization_dialog(toolbar->items_list);
    if(new_toolbar_items != NULL)
        w_toolbar_set_new_items(toolbar, new_toolbar_items);
}

void w_toolbar_initmenu(ddb_gtkui_widget_t *w, GtkWidget *menu)
{
    GtkWidget *customize_menu_item = gtk_menu_item_new_with_mnemonic("Customize");
    gtk_widget_show(customize_menu_item);

    gtk_container_add(GTK_CONTAINER(menu), customize_menu_item);

    g_signal_connect((gpointer)customize_menu_item, "activate", G_CALLBACK(on_customize_activate), w);
}

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
