/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    g_object_ref(G_OBJECT(widget)), (GDestroyNotify) g_object_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_tb_customization_dialog (void)
{
  GtkWidget *tb_customization_dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *treeview_scrolled_window;
  GtkWidget *tb_items_treeview;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  tb_customization_dialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (tb_customization_dialog), "Customize toolbar");
  gtk_window_set_type_hint (GTK_WINDOW (tb_customization_dialog), GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_dialog_set_has_separator (GTK_DIALOG (tb_customization_dialog), FALSE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (tb_customization_dialog));
  gtk_widget_show (dialog_vbox1);

  treeview_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (treeview_scrolled_window);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), treeview_scrolled_window, TRUE, TRUE, 0);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (treeview_scrolled_window), GTK_SHADOW_IN);

  tb_items_treeview = gtk_tree_view_new ();
  gtk_widget_show (tb_items_treeview);
  gtk_container_add (GTK_CONTAINER (treeview_scrolled_window), tb_items_treeview);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG (tb_customization_dialog));
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (tb_customization_dialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default(cancelbutton1, TRUE);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (tb_customization_dialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default(okbutton1, TRUE);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (tb_customization_dialog, tb_customization_dialog, "tb_customization_dialog");
  GLADE_HOOKUP_OBJECT_NO_REF (tb_customization_dialog, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, treeview_scrolled_window, "treeview_scrolled_window");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, tb_items_treeview, "tb_items_treeview");
  GLADE_HOOKUP_OBJECT_NO_REF (tb_customization_dialog, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, okbutton1, "okbutton1");

  return tb_customization_dialog;
}

