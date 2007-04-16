/*
 * main.c
 *
 * GTK+ widget to display and handle fonts
 *
 * - Alex Roberts, 2006
 *
 */

/*
 * FontView Test - font viewing widget test app
 * Part of the Fontable Project
 * Copyright (C) 2006 Alex Roberts
 * Copyright (C) 2006 Jon Phillips, <jon@rejon.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 * MA 02111-1307 USA
 * 
 */
 
 
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glade/glade-xml.h>
#include "font-model.h"
#include "font-view.h"

GladeXML *xml;
GtkWidget *font;

enum {
	COLUMN_INT,
	COLUMN_STRING,
	N_COLUMNS
};

void render_size_changed (GtkComboBox *w, gpointer data);

void view_size_changed (GtkWidget *w, gdouble size) {
	GtkWidget *sizew;
	g_print ("signal! FontView changed font size to %.2fpt.\n", size);
	
	sizew = glade_xml_get_widget (xml, "render_size");
	//gtk_spin_button_set_value (GTK_SPIN_BUTTON(sizew), font_view_get_pt_size (FONT_VIEW(w)));
}

void render_text_changed (GtkEntry *w, gpointer data) {
	gchar *text = (gchar *)gtk_entry_get_text (w);
	
	g_message ("text_changed: %s", text);
	
	font_view_set_text (FONT_VIEW(font), text);
}

void render_size_changed (GtkComboBox *w, gpointer data) {
	GtkTreeModel *sizes;
	GtkTreeIter iter;
	gint size;
	
	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (w), &iter)) {
		sizes = gtk_combo_box_get_model (GTK_COMBO_BOX (w));
		gtk_tree_model_get (sizes, &iter, COLUMN_INT, &size, -1);
		font_view_set_pt_size (FONT_VIEW(font), size);
	}
	
}

void print_usage ()
{
    g_print ("\nUsage:\n\tfv <path_to_font>\n\n");
    exit(1);
}

GtkWidget *font_custom_handler (GladeXML *xml, gchar *func, gchar *name, gchar *s1, gchar *s2, gint i1, gint i2, gpointer data) {
	GtkWidget *w = NULL;
	
	g_message ("font custom handler.");
	
	if (g_strcasecmp ("font_view_new_with_model", func) == 0) {
		g_message ("fch: yo! %s", (gchar *)data);
		w = font_view_new_with_model ((gchar *)data);
		return w;
	}
	
	return NULL;
}

GtkListStore *fv_init_sizes (GtkListStore *sizes) {
	GtkTreePath *path;
	GtkTreeIter iter;
	gint i;	
	gint size_array[] = {6, 8, 9, 10, 12, 18, 24, 36, 48, 60, 72, 96, 112, 200};
	
	for (i = 0; i < G_N_ELEMENTS(size_array); i++) {
		gtk_list_store_append (sizes, &iter);
		gtk_list_store_set (sizes, &iter, 
					COLUMN_INT, size_array[i], 
					COLUMN_STRING, g_strdup_printf ("  %dpt", size_array[i]), 
					-1);
	}
	
	return sizes;
}

int main (int argc, char *argv[]) {
	GtkWidget *entry, *size;
	GtkListStore *sizes;
	GtkCellRenderer *renderer = NULL;
	
	gtk_init (&argc, &argv);

	if (!argv[1]) {
		print_usage();
		return 1;
	}	
	
	glade_set_custom_handler (font_custom_handler, argv[1]);
	xml = glade_xml_new ("mainwindow.glade", NULL, NULL);
	if (!xml) {
		xml = glade_xml_new (PACKAGE_DATA_DIR"/mainwindow.glade", NULL, NULL);
	}
	g_return_if_fail (xml);
	
	glade_xml_signal_autoconnect (xml);

	font = glade_xml_get_widget (xml, "font-view");
	g_signal_connect (font, "size-changed", G_CALLBACK(view_size_changed), NULL);
	gtk_widget_show (font);

	entry = glade_xml_get_widget (xml, "render_str");
	gtk_entry_set_text (GTK_ENTRY(entry), font_view_get_text(FONT_VIEW(font)));
	g_signal_connect (entry, "changed", G_CALLBACK(render_text_changed), NULL);
	
	
	/* Set up the size selection combo box */
	sizes = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);
	sizes = fv_init_sizes (sizes);
	
	size = glade_xml_get_widget (xml, "size_combo");
	g_signal_connect (size, "changed", G_CALLBACK(render_size_changed), NULL);

	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (size), renderer, FALSE);
	gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (size), renderer, "text", 1);

	gtk_combo_box_set_model (GTK_COMBO_BOX (size), GTK_TREE_MODEL (sizes));
	gtk_combo_box_set_active (GTK_COMBO_BOX (size), 8);
	
	gtk_main();

	return 0;
}
