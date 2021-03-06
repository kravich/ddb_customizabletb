/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

/*
 * Public Functions.
 */

/*
 * This function returns a widget in a component created by Glade.
 * Call it with the toplevel widget in the component (i.e. a window/dialog),
 * or alternatively any widget in the component, and the name of the widget
 * you want returned.
 */
GtkWidget*  lookup_widget              (GtkWidget       *widget,
                                        const gchar     *widget_name);


/* Use this function to set the directory containing installed pixmaps. */
void        add_pixmap_directory       (const gchar     *directory);


/*
 * Private Functions.
 */

/* This is used to create the pixmaps used in the interface. */
GtkWidget*  create_pixmap              (GtkWidget       *widget,
                                        const gchar     *filename);

/* This is used to create the pixbufs used in the interface. */
GdkPixbuf*  create_pixbuf              (const gchar     *filename);

/* This is used to set ATK action descriptions. */
void        glade_set_atk_action_description (AtkAction       *action,
                                              const gchar     *action_name,
                                              const gchar     *description);


/* Following functions are added to support some uncommon and relatively new GTK2/GTK3 and GLIB APIs */

#if GTK_CHECK_VERSION(3,0,0)
void gtk_dialog_set_has_separator(GtkDialog *dialog, gboolean setting);
#endif

#if !GTK_CHECK_VERSION(2,18,0)
#define gtk_widget_set_can_focus(widget, canfocus) {if (canfocus) GTK_WIDGET_SET_FLAGS (widget, GTK_CAN_FOCUS); else GTK_WIDGET_UNSET_FLAGS(widget, GTK_CAN_FOCUS);}
#define gtk_widget_set_can_default(widget, candefault) {if (candefault) GTK_WIDGET_SET_FLAGS (widget, GTK_CAN_DEFAULT); else GTK_WIDGET_UNSET_FLAGS(widget, GTK_CAN_DEFAULT);}
#endif

#if !GLIB_CHECK_VERSION(2,28,0)
void g_list_free_full (GList *list, GDestroyNotify free_func);
#endif
