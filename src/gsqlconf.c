/* 
 * GSQL - database development tool for GNOME
 *
 * Copyright (C) 2006-2008  Taras Halturin  halturin@gmail.com
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

 

#include <libgsql/engines.h>
#include <libgsql/conf.h>
#include <libgsql/common.h>
#include <libgsql/editor.h>
#include <libgsql/workspace.h>
#include <libgsql/plugins.h>
#include <libgsql/type_datetime.h>
#include <libgsql/sqleditor.h>

#include "gsqlconf.h"
#include "gsqlconfcb.h"





static void
gsql_conf_plugins_list_create (gpointer key, 
								gpointer value,
								gpointer user_data);
static void
gsql_conf_engines_conf_widget_create (gpointer key, 
								gpointer value,
								gpointer user_data);

static void
gsql_conf_engines_conf_widget_free (gpointer key, 
								gpointer value,
								gpointer user_data);

void
gsql_conf_dialog()
{
	GSQL_TRACE_FUNC;

	GtkDialog * dialog;
	GtkBuilder* builder;
	const gchar *ui_to_load[] = { "gsql_prefs_dialog",
				      "adjustment1",
				      "adjustment2",
				      "adjustment3",
				      NULL };
	GError *error = NULL;
	gboolean gconf_bool_value = TRUE;
	gint     gconf_int_value;
	gchar    *gconf_gchar_value;
	GtkWidget *conf_notebook;
	GtkWidget *show_navarea_check;
	GtkWidget *show_messarea_check;
	GtkWidget *restore_sizepos_check;
	GtkWidget *widescreen_layout_check;
	GtkWidget *datetime_format_entry;
	GtkWidget *datetime_preview_button;
	GtkWidget *datetime_preview_entry;
	
	GtkWidget *use_system_font_check;
	GtkWidget *font_button;
	GtkWidget *combo_scheme;
	GtkWidget *insert_space_check;
	GtkWidget *tab_width_spin;
	GtkWidget *display_line_num_check;
	GtkWidget *highlight_line_check;
	GtkWidget *enable_text_wrap_check;
	GtkWidget *enable_auto_indent_check;
	GtkWidget *configure_button;
	GtkWidget *plugins_tree_view;
	GtkWidget *fetch_limit_step;
	GtkWidget *fetch_limit_max;
	
	GtkListStore *plugins_list;
	GtkListStore *color_scheme;
	GtkTreeViewColumn *column;
	GtkTreeIter	 *s_iter = NULL;
	GtkCellRenderer *renderer;
	GtkSourceStyleSchemeManager* manager;
	const gchar* const *styles;
	const gchar* const *style;
	
	builder = gtk_builder_new ();

	if (!gtk_builder_add_objects_from_file (builder, GSQL_DIALOGS_UI, (gchar **) ui_to_load, &error))
	{
		g_warning ("Couldn't load ui file: %s", error->message);
		g_error_free (error);
	}

	g_return_if_fail(builder);
	
	dialog = (GtkDialog *) gtk_builder_get_object (builder, "gsql_prefs_dialog");
	
	gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (gsql_window));
	
	conf_notebook = GTK_WIDGET (gtk_builder_get_object (builder, "gsql_conf_notebool"));
	show_navarea_check = GTK_WIDGET (gtk_builder_get_object (builder, "show_navarea_check"));
	show_messarea_check = GTK_WIDGET (gtk_builder_get_object (builder, "show_messarea_check"));
    restore_sizepos_check = GTK_WIDGET (gtk_builder_get_object (builder, "restore_sizepos_check"));
	widescreen_layout_check = GTK_WIDGET (gtk_builder_get_object (builder, "widescreen_layout_check"));
	datetime_format_entry = GTK_WIDGET (gtk_builder_get_object (builder, "datetime_format_entry"));
	datetime_preview_button = GTK_WIDGET (gtk_builder_get_object (builder, "datetime_preview_button"));
	datetime_preview_entry = GTK_WIDGET (gtk_builder_get_object (builder, "datetime_preview_entry"));
		
	font_button = GTK_WIDGET (gtk_builder_get_object (builder, "font_button"));
	use_system_font_check = GTK_WIDGET (gtk_builder_get_object (builder, "use_system_font_check"));
	combo_scheme = GTK_WIDGET (gtk_builder_get_object (builder, "combo_scheme"));
	
	insert_space_check = GTK_WIDGET (gtk_builder_get_object (builder, "insert_space_check"));
	tab_width_spin = GTK_WIDGET (gtk_builder_get_object (builder, "tab_width_spin"));
	display_line_num_check = GTK_WIDGET (gtk_builder_get_object (builder, "display_line_num_check"));
	highlight_line_check = GTK_WIDGET (gtk_builder_get_object (builder, "highlight_line_check"));
	enable_text_wrap_check = GTK_WIDGET (gtk_builder_get_object (builder, "enable_text_wrap_check"));
	enable_auto_indent_check = GTK_WIDGET (gtk_builder_get_object (builder, "enable_auto_indent_check"));
	plugins_tree_view = GTK_WIDGET (gtk_builder_get_object (builder, "prefs_plugins_tree_view"));
	configure_button = GTK_WIDGET (gtk_builder_get_object (builder, "pref_plugins_configure_button"));
	
	g_signal_connect (G_OBJECT(configure_button), "clicked",
					  G_CALLBACK(on_pref_plugins_configure_button_clicked),
					  plugins_tree_view);
	
	g_signal_connect (G_OBJECT(plugins_tree_view), "cursor-changed",
					  G_CALLBACK(on_prefs_plugins_tree_view_cursor_changed),
					  configure_button);
	
	g_signal_connect (G_OBJECT(datetime_format_entry), "focus-out-event",
					  G_CALLBACK(on_pref_datetime_entry_focus_out),
					  NULL);
	g_signal_connect (G_OBJECT(datetime_preview_button), "clicked",
					  G_CALLBACK(on_pref_datetime_preview),
					  datetime_preview_entry);
	gconf_gchar_value = gsql_conf_value_get_string (GSQL_CONF_DATETIME_FORMAT);
	
	if (!gconf_gchar_value)
	{
		gsql_conf_value_set_string (GSQL_CONF_DATETIME_FORMAT, 
									GSQL_DATETIME_FORMAT_DEFAULT);
		gconf_gchar_value = GSQL_DATETIME_FORMAT_DEFAULT;
	}
	
	gtk_entry_set_text (GTK_ENTRY (datetime_format_entry), gconf_gchar_value);
	
	// create prefs widget for the every engine;
	gsql_engines_foreach (gsql_conf_engines_conf_widget_create,
							conf_notebook);
	
	plugins_list = gtk_list_store_new (3, 
									   GDK_TYPE_PIXBUF, // plugin icon
									   G_TYPE_STRING, // plugin name
									   G_TYPE_POINTER // plugin->conf_dialog_create
									   );

	renderer = gtk_cell_renderer_pixbuf_new();
	column = gtk_tree_view_column_new_with_attributes ("Icon",
						renderer,
						NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (plugins_tree_view), column);
	gtk_tree_view_column_add_attribute (column, renderer, "pixbuf", 0);
	
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes ("Name",
						renderer,
						NULL);
	gtk_tree_view_column_add_attribute (column, renderer, "markup", 1);
	gtk_tree_view_append_column (GTK_TREE_VIEW (plugins_tree_view), column);
	
	gsql_plugins_foreach (gsql_conf_plugins_list_create,
							plugins_list);
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (plugins_tree_view),
							 GTK_TREE_MODEL (plugins_list));

	gconf_bool_value = gsql_conf_value_get_boolean (GSQL_CONF_WORKSPACE_SHOW_NAVIGATE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (show_navarea_check), gconf_bool_value);

    
	gconf_bool_value = gsql_conf_value_get_boolean (GSQL_CONF_WORKSPACE_SHOW_MESSAGES);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (show_messarea_check), gconf_bool_value);
        
	gconf_bool_value = gsql_conf_value_get_boolean (GSQL_CONF_UI_RESTORE_SIZE_POS);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (restore_sizepos_check), gconf_bool_value);
	
	gconf_bool_value = gsql_conf_value_get_boolean (GSQL_CONF_WORKSPACE_WIDESCREEN);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widescreen_layout_check), gconf_bool_value);
	
	
	gconf_int_value = gsql_conf_value_get_int (GSQL_CONF_SQL_FETCH_STEP);
	
	if (!gconf_int_value)
		gconf_int_value = GSQL_EDITOR_FETCH_STEP_DEFAULT;
	
	fetch_limit_step = GTK_WIDGET (gtk_builder_get_object (builder, "fetch_limit_step"));
	g_signal_connect (G_OBJECT (fetch_limit_step), "value-changed",
					  G_CALLBACK (on_fetch_limit_step_changed), fetch_limit_step);

	gtk_spin_button_set_value (GTK_SPIN_BUTTON (fetch_limit_step), gconf_int_value);
	
	gconf_int_value = gsql_conf_value_get_int (GSQL_CONF_SQL_FETCH_MAX);
	
	if (!gconf_int_value)
		gconf_int_value = GSQL_EDITOR_FETCH_MAX_DEFAULT;
	
	fetch_limit_max = GTK_WIDGET (gtk_builder_get_object (builder, "fetch_limit_max"));
	g_signal_connect (G_OBJECT (fetch_limit_max), "value-changed",
					  G_CALLBACK (on_fetch_limit_max_changed), fetch_limit_max);
	
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (fetch_limit_max), gconf_int_value);
	   
	gconf_gchar_value = gsql_conf_value_get_string  (GSQL_CONF_EDITOR_FONT_NAME);
	gconf_bool_value  = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_USE_SYSTEM_FONT);
	
	if (gconf_gchar_value)
		gtk_font_button_set_font_name (GTK_FONT_BUTTON (font_button), gconf_gchar_value);
	
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (use_system_font_check), gconf_bool_value);
    gtk_widget_set_sensitive (font_button, !gconf_bool_value);
	
	HOOKUP_OBJECT_NO_REF(use_system_font_check, font_button, "font_button");
	
	color_scheme = gtk_list_store_new (3, G_TYPE_STRING, G_TYPE_STRING,
									   G_TYPE_STRING);
	
	manager = gtk_source_style_scheme_manager_get_default();
	styles = gtk_source_style_scheme_manager_get_scheme_ids (manager);
	
	gconf_gchar_value = gsql_conf_value_get_string (GSQL_CONF_EDITOR_COLOR_SCHEME);
	
	for (style = styles; *style != NULL; style++)
	{
		GtkTreeIter iter;
		GtkSourceStyleScheme* scheme;
		const gchar* id;
		
		scheme = gtk_source_style_scheme_manager_get_scheme (manager, *style);
		id = gtk_source_style_scheme_get_id (scheme);
		
		gtk_list_store_append (color_scheme, &iter);
		gtk_list_store_set (color_scheme, &iter,
							0, gtk_source_style_scheme_get_name (scheme),
							1, gtk_source_style_scheme_get_description (scheme),
							2, id,
							-1);
		
		if ((gconf_gchar_value) && (g_str_equal (id, gconf_gchar_value)))
		{
			s_iter = gtk_tree_iter_copy (&iter);
		}
	}
	
	gtk_combo_box_set_model (GTK_COMBO_BOX (combo_scheme), GTK_TREE_MODEL (color_scheme));
	
	if (s_iter)
	{
		gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo_scheme),
									   s_iter);
		
		gtk_tree_iter_free (s_iter);
	}
	
	g_signal_connect (combo_scheme, "changed", G_CALLBACK (on_color_scheme_changed), 
					  color_scheme);
	
	gtk_cell_layout_clear (GTK_CELL_LAYOUT(combo_scheme));
	
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(combo_scheme), renderer, TRUE);
	gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(combo_scheme), renderer,
								   "text", 0);
	
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(combo_scheme), renderer, FALSE);
	gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(combo_scheme), renderer,
								   "text", 1);
	g_object_set (renderer, "style", PANGO_STYLE_ITALIC, NULL);
	
	gconf_bool_value = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_USE_SPACE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (insert_space_check), gconf_bool_value);
	
	gconf_int_value = gsql_conf_value_get_int (GSQL_CONF_EDITOR_TAB_WIDTH);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (tab_width_spin), gconf_int_value);
    
	gconf_bool_value = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_SHOW_LINE_NUM);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (display_line_num_check), gconf_bool_value);
    
	gconf_bool_value = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_HIGHLIGHT_LINE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (highlight_line_check), gconf_bool_value);
	
	gconf_bool_value = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_WRAPPING);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (enable_text_wrap_check), gconf_bool_value);
	
	gconf_bool_value = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_AUTO_INDENT);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (enable_auto_indent_check), gconf_bool_value);
	
	gtk_dialog_run (dialog);
	
	// free prefs widget for the every engine;
	gsql_engines_foreach (gsql_conf_engines_conf_widget_free,
							conf_notebook);
	
	gtk_widget_destroy ((GtkWidget *) dialog);
	g_object_unref(G_OBJECT(builder));

}


static void
gsql_conf_engines_conf_widget_create (gpointer key, 
								gpointer value,
								gpointer user_data)
{
	GSQL_TRACE_FUNC;

	GSQLEngine	*engine = (GSQLEngine *) value;
	GtkWidget	*label;
	GtkWidget 	*widget;
	GtkNotebook *notebook = GTK_NOTEBOOK(user_data);

	g_return_if_fail (engine->conf_widget_new != NULL);

	widget = engine->conf_widget_new ();
	
	if (widget != NULL)
	{
		label = gtk_label_new (engine->info.name);
		gtk_notebook_append_page (notebook, widget, label);
		HOOKUP_OBJECT (notebook, widget, engine->info.id);
	}

}


static void
gsql_conf_engines_conf_widget_free (gpointer key, 
								gpointer value,
								gpointer user_data)
{
	GSQL_TRACE_FUNC;

	GtkWidget	*widget;
	GtkNotebook *notebook = (GtkNotebook *) user_data;
	GSQLEngine	*engine = (GSQLEngine *) value;

	widget = (GtkWidget *) g_object_get_data (G_OBJECT (notebook), engine->info.id);
	
	g_return_if_fail(engine->conf_widget_free != NULL);
	
	engine->conf_widget_free (widget);

}

static void
gsql_conf_plugins_list_create (gpointer key, 
								gpointer value,
								gpointer user_data)
{
	GSQL_TRACE_FUNC;

	GtkListStore *plugins_list = user_data;
	GtkTreeIter iter;
	GSQLPlugin *plugin = value;
	gchar tmp[256];
	
	g_return_if_fail (plugin != NULL);

	gtk_list_store_append (GTK_LIST_STORE(plugins_list), 
						   &iter);
	g_snprintf(tmp, 256 ,"<b>%s</b>\n%s", plugin->info.name, plugin->info.desc);

	gtk_list_store_set(GTK_LIST_STORE(plugins_list), &iter,
					   0, create_pixbuf (plugin->file_logo),
					   1, tmp,
					   2, plugin,
					   -1);

}
