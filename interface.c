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
  GtkWidget *dialog_main_vbox;
  GtkWidget *hbox1;
  GtkWidget *frame1;
  GtkWidget *alignment1;
  GtkWidget *vbox1;
  GtkWidget *treeview_scrolled_window;
  GtkWidget *tb_items_treeview;
  GtkWidget *table1;
  GtkWidget *button_remove;
  GtkWidget *button_up;
  GtkWidget *button_down;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *alignment2;
  GtkWidget *vbox2;
  GtkWidget *scrolledwindow1;
  GtkWidget *actions_treeview;
  GtkWidget *vbox3;
  GtkWidget *hbox2;
  GtkWidget *label3;
  GtkWidget *context_combobox;
  GtkWidget *button_add;
  GtkWidget *alignment3;
  GtkWidget *label2;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  tb_customization_dialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (tb_customization_dialog), "Customize toolbar");
  gtk_window_set_default_size (GTK_WINDOW (tb_customization_dialog), 300, 360);
  gtk_window_set_type_hint (GTK_WINDOW (tb_customization_dialog), GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_dialog_set_has_separator (GTK_DIALOG (tb_customization_dialog), FALSE);

  dialog_main_vbox = gtk_dialog_get_content_area (GTK_DIALOG (tb_customization_dialog));
  gtk_widget_show (dialog_main_vbox);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_main_vbox), hbox1, TRUE, TRUE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (hbox1), frame1, FALSE, FALSE, 4);
  gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_IN);

  alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (frame1), alignment1);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 0, 0, 12, 6);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), vbox1);

  treeview_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (treeview_scrolled_window);
  gtk_box_pack_start (GTK_BOX (vbox1), treeview_scrolled_window, TRUE, TRUE, 0);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (treeview_scrolled_window), GTK_SHADOW_IN);

  tb_items_treeview = gtk_tree_view_new ();
  gtk_widget_show (tb_items_treeview);
  gtk_container_add (GTK_CONTAINER (treeview_scrolled_window), tb_items_treeview);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (tb_items_treeview), FALSE);

  table1 = gtk_table_new (2, 2, TRUE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, FALSE, FALSE, 2);

  button_remove = gtk_button_new_from_stock ("gtk-remove");
  gtk_widget_show (button_remove);
  gtk_table_attach (GTK_TABLE (table1), button_remove, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  button_up = gtk_button_new_from_stock ("gtk-go-up");
  gtk_widget_show (button_up);
  gtk_table_attach (GTK_TABLE (table1), button_up, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  button_down = gtk_button_new_from_stock ("gtk-go-down");
  gtk_widget_show (button_down);
  gtk_table_attach (GTK_TABLE (table1), button_down, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label1 = gtk_label_new ("Current toolbar actions");
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (hbox1), frame2, TRUE, TRUE, 4);
  gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_IN);

  alignment2 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (frame2), alignment2);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 0, 0, 12, 6);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), vbox2);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  actions_treeview = gtk_tree_view_new ();
  gtk_widget_show (actions_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), actions_treeview);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (actions_treeview), FALSE);

  vbox3 = gtk_vbox_new (TRUE, 0);
  gtk_widget_show (vbox3);
  gtk_box_pack_start (GTK_BOX (vbox2), vbox3, FALSE, FALSE, 2);

  hbox2 = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox2, TRUE, TRUE, 0);

  label3 = gtk_label_new ("Context:");
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (hbox2), label3, FALSE, FALSE, 0);

  context_combobox = gtk_combo_box_text_new ();
  gtk_widget_show (context_combobox);
  gtk_box_pack_start (GTK_BOX (hbox2), context_combobox, TRUE, TRUE, 0);

  button_add = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (button_add);
  gtk_box_pack_start (GTK_BOX (hbox2), button_add, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment3);
  gtk_box_pack_start (GTK_BOX (vbox3), alignment3, TRUE, TRUE, 0);

  label2 = gtk_label_new ("Available actions");
  gtk_widget_show (label2);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label2);
  gtk_label_set_use_markup (GTK_LABEL (label2), TRUE);

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
  GLADE_HOOKUP_OBJECT_NO_REF (tb_customization_dialog, dialog_main_vbox, "dialog_main_vbox");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, treeview_scrolled_window, "treeview_scrolled_window");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, tb_items_treeview, "tb_items_treeview");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, table1, "table1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, button_remove, "button_remove");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, button_up, "button_up");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, button_down, "button_down");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, label1, "label1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, alignment2, "alignment2");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, actions_treeview, "actions_treeview");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, vbox3, "vbox3");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, label3, "label3");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, context_combobox, "context_combobox");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, button_add, "button_add");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, alignment3, "alignment3");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, label2, "label2");
  GLADE_HOOKUP_OBJECT_NO_REF (tb_customization_dialog, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (tb_customization_dialog, okbutton1, "okbutton1");

  return tb_customization_dialog;
}

