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


#include <libgsql/editor.h>
#include <libgsql/conf.h>
#include <libgsql/common.h>
#include <libgsql/menu.h>
#include <libgsql/stock.h>
#include <libgsql/session.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdkkeysyms.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


static GtkActionGroup *editor_actions = NULL;

static void gsql_source_editor_property_set (gpointer source);
static void gsql_markers_clear_for_each (gpointer data, gpointer user_data);

static gboolean on_editor_focus_in (GtkWidget *widget, GdkEventFocus *event,
							 gpointer user_data);
static gboolean on_editor_focus_out (GtkWidget *widget, GdkEventFocus *event,
							 gpointer user_data);


static gchar editor_ui[] =
"<ui>   "
"  <menubar name=\"MenuMain\">	"
"    <menu name=\"MenuEdit\" action=\"ActionMenuEdit\">	"
"	   		<menuitem name=\"Undo\" action=\"ActionEditUndo\" />	"
"	   		<menuitem name=\"Redo\" action=\"ActionEditRedo\" />	"
"	   		<separator name=\"SeparatorA\" />	"
"	   		<menuitem name=\"Cut\" action=\"ActionEditCut\" />	"
"	   		<menuitem name=\"Copy\" action=\"ActionEditCopy\" />	"
"	   		<menuitem name=\"Paste\" action=\"ActionEditPaste\" />	"
"	   		<menuitem name=\"Delete\" action=\"ActionEditDelete\" />	"
/*"	   		<separator name=\"SeparatorB\" />	"
"	   		<menuitem name=\"Indent\" action=\"ActionEditIndent\" />	"
"	   		<menuitem name=\"Unindent\" action=\"ActionEditUnindent\" />	"
"	   		<menuitem name=\"Comment\" action=\"ActionEditComment\" />	"
"	   		<menuitem name=\"Uncomment\" action=\"ActionEditUncomment\" />	"
"	   		<menuitem name=\"UpperCase\" action=\"ActionEditUpperCase\" />	"
"	   		<menuitem name=\"LowerCase\" action=\"ActionEditLowerCase\" />	"
"	   		<menuitem name=\"KeyUpperCase\" action=\"ActionEditKeyUpper\" />	"
"	   		<menuitem name=\"KeyLowerCase\" action=\"ActionEditKeyLower\" />	" */
"    </menu>	"
"  </menubar>	"
"</ui>	";

void on_editor_undo_activate (GtkMenuItem *mi, gpointer data);
void on_editor_redo_activate (GtkMenuItem *mi, gpointer data);
void on_editor_copy_activate (GtkMenuItem *mi, gpointer data);
void on_editor_cut_activate (GtkMenuItem *mi, gpointer data);
void on_editor_paste_activate (GtkMenuItem *mi, gpointer data);
void on_editor_delete_activate (GtkMenuItem *mi, gpointer data);
void on_editor_indent_activate (GtkMenuItem *mi, gpointer data);
void on_editor_unindent_activate (GtkMenuItem *mi, gpointer data);
void on_editor_comment_activate (GtkMenuItem *mi, gpointer data);
void on_editor_uncomment_activate (GtkMenuItem *mi, gpointer data);
void on_editor_upper_case_activate (GtkMenuItem *mi, gpointer data);
void on_editor_lower_case_activate (GtkMenuItem *mi, gpointer data);

static GtkActionEntry editor_acts[] = 
{
	{ "ActionEditUndo", GTK_STOCK_UNDO, N_("Undo"), "<control>Z", N_("Undo"), G_CALLBACK(on_editor_undo_activate) },
	{ "ActionEditRedo", GTK_STOCK_REDO, N_("Redo"), "<control>R", N_("Redo"), G_CALLBACK(on_editor_redo_activate) },
	{ "ActionEditCopy", GTK_STOCK_COPY, N_("Copy"), "<control>C", N_("Copy"), G_CALLBACK(on_editor_copy_activate) },
	{ "ActionEditCut", GTK_STOCK_CUT, N_("Cut"), "<control>X", N_("Cut"), G_CALLBACK(on_editor_cut_activate) },
	{ "ActionEditPaste", GTK_STOCK_PASTE, N_("Paste"), "<control>V", N_("Paste"), G_CALLBACK(on_editor_paste_activate) },
	{ "ActionEditDelete", GTK_STOCK_CLEAR, N_("Clear"), NULL, N_("Clear"), G_CALLBACK(on_editor_delete_activate) }

/*	{ "ActionEditIndent", GTK_STOCK_INDENT, N_("Indent"), "<control>I", N_("Indent"), G_CALLBACK(on_editor_indent_activate) },
	{ "ActionEditUnindent", GTK_STOCK_UNINDENT, N_("Unindent"), "<control>U", N_("Unindent"), G_CALLBACK(on_editor_unindent_activate) },
	{ "ActionEditComment", NULL, N_("Comment"), NULL, N_("Comment"), G_CALLBACK(on_editor_comment_activate) },
	{ "ActionEditUncomment", NULL, N_("Uncomment"), NULL, N_("Uncomment"), G_CALLBACK(on_editor_uncomment_activate) },
	{ "ActionEditUpperCase", NULL, N_("Upper Case"), NULL, N_("Upper Case"), G_CALLBACK(on_editor_upper_case_activate) },
	{ "ActionEditLowerCase", NULL, N_("Lower Case"), NULL, N_("Lower Case"), G_CALLBACK(on_editor_lower_case_activate) }	*/
};



GtkWidget *
gsql_source_editor_new(gchar * buffer_body)
{
	GSQL_TRACE_FUNC;
		
	GtkWidget * source;        
	static GtkSourceLanguage *lang;
	PangoFontDescription * font_desc;
	GtkSourceLanguageManager * lm;
	const GSList * list_lang;        
	GtkSourceBuffer * buffer;
	GtkTextIter txtiter;
	gchar * font;
	gboolean * use_system_font;
	gchar ** split_buffer;
	gchar ** mark;
	gchar * tmp;
	gchar *lng_path[] = {
		PACKAGE_DATA_DIR "/gsql/",
		NULL
	};
	
	
	buffer = gtk_source_buffer_new(NULL);
	source = GTK_WIDGET (gtk_source_view_new_with_buffer (buffer));
	
	if (!editor_actions)
	{
		editor_actions = gtk_action_group_new ("editor_actions");
		
		gtk_action_group_add_actions (editor_actions, editor_acts, 
								  G_N_ELEMENTS (editor_acts), NULL);
		
		gsql_menu_merge_from_string (editor_ui, editor_actions);
		
		gtk_action_group_set_sensitive (editor_actions, FALSE);
	}
	
	if (buffer_body != NULL)
	{
		split_buffer = g_strsplit (buffer_body, "\n", 10485760);
		mark = split_buffer;
		gtk_source_buffer_begin_not_undoable_action(buffer);
            
		while (*split_buffer)
		{
			gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (buffer), &txtiter);
			tmp = g_strdup_printf ("%s\n", *split_buffer);
			gtk_text_buffer_insert (GTK_TEXT_BUFFER (buffer), &txtiter,
									tmp, g_utf8_strlen(tmp, 10485760));
			split_buffer++;
			g_free (tmp);
		}
		
		gtk_source_buffer_end_not_undoable_action (buffer);
		gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(buffer), &txtiter );
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(buffer), &txtiter);
		
		g_strfreev (mark);        
	}
	
	lm = gtk_source_language_manager_new();
	
	gtk_source_language_manager_set_search_path (lm, lng_path);

	lang = gtk_source_language_manager_get_language (lm, "gsql");
	
	if (!lang)
	{
		GSQL_DEBUG ("Could not find 'gsql' syntax theme. Set default 'sql'");

		lm = gtk_source_language_manager_get_default ();
		lang = gtk_source_language_manager_get_language (lm, "sql");
	}
	
	gtk_source_buffer_set_language  (buffer, lang);

	gtk_source_buffer_set_highlight_syntax (buffer, TRUE);
	gsql_source_editor_property_set (source);

/*	Deprecated ?
	
	gtk_source_view_set_mark_category_pixbuf (GTK_SOURCE_VIEW (source), GSQL_EDITOR_MARKER_COMPLETE,
									   // FIXME. make another icon.
									   create_pixbuf ("msg_notice.png"));
	gtk_source_view_set_mark_category_pixbuf (GTK_SOURCE_VIEW (source), GSQL_EDITOR_MARKER_FAILED,
									   // FIXME. make another icon.
									   create_pixbuf ("msg_error.png"));
*/
	gtk_source_view_set_show_line_marks (GTK_SOURCE_VIEW (source), TRUE);	
	

	/* gsql_conf_nitify_add (GSQL_CONF_EDITOR_USE_SYSTEM_FONT, */
	/* 							gsql_source_editor_property_set, */
	/* 							(gpointer) source); */
	/* gsql_conf_nitify_add (GSQL_CONF_EDITOR_FONT_NAME, */
	/* 							gsql_source_editor_property_set, */
	/* 							(gpointer) source); */
	/* gsql_conf_nitify_add (GSQL_CONF_EDITOR_COLOR_SCHEME, */
	/* 							gsql_source_editor_property_set, */
	/* 							(gpointer) source); */
	/* gsql_conf_nitify_add (GSQL_CONF_EDITOR_USE_SPACE, */
	/* 							gsql_source_editor_property_set, */
	/* 							(gpointer) source); */
	/* gsql_conf_nitify_add (GSQL_CONF_EDITOR_TAB_WIDTH, */
	/* 							gsql_source_editor_property_set, */
	/* 							(gpointer) source); */
	/* gsql_conf_nitify_add (GSQL_CONF_EDITOR_SHOW_LINE_NUM, */
	/* 							gsql_source_editor_property_set, */
	/* 							(gpointer) source); */
	/* gsql_conf_nitify_add (GSQL_CONF_EDITOR_HIGHLIGHT_LINE, */
	/* 							gsql_source_editor_property_set, */
	/* 							(gpointer) source); */
	/* gsql_conf_nitify_add (GSQL_CONF_EDITOR_WRAPPING, */
	/* 							gsql_source_editor_property_set, */
	/* 							(gpointer) source); */
	/* gsql_conf_nitify_add (GSQL_CONF_EDITOR_AUTO_INDENT, */
	/* 							gsql_source_editor_property_set, */
	/* 							(gpointer) source); */
	
	g_signal_connect (G_OBJECT (source), "focus-in-event",
					  G_CALLBACK (on_editor_focus_in), NULL);
	
	g_signal_connect (G_OBJECT (source), "focus-out-event",
					  G_CALLBACK (on_editor_focus_out), NULL);
	
	
	gtk_widget_show (source);
	
	return source;	
}



void 
gsql_source_editor_marker_set (GtkTextIter *iter, gchar *marker_type)
{
	GSQL_TRACE_FUNC;

	GtkSourceMark *pixmark;
	GtkSourceBuffer *buffer;
	GList *markers = NULL;
	buffer = (GtkSourceBuffer *) gtk_text_iter_get_buffer (iter);
	
	if (buffer)
	{
		pixmark = gtk_source_mark_new (NULL, marker_type);
		
		gtk_text_buffer_add_mark (GTK_TEXT_BUFFER (buffer), GTK_TEXT_MARK (pixmark), iter);
		
		markers = g_object_get_data (G_OBJECT (buffer), "markers");
		markers = g_list_append (markers, pixmark);
		
		HOOKUP_OBJECT_NO_REF (buffer, markers, "markers");
	}

}

void
gsql_source_editor_markers_clear (GtkSourceBuffer *buffer)
{
	GSQL_TRACE_FUNC;

	GList *markers = NULL;
	
	markers = g_object_get_data (G_OBJECT (buffer), "markers");
	
	if (markers == NULL)
	{
		GSQL_DEBUG ("No markers found. markers == NULL.");
		return;
	}
	
	g_list_foreach (markers, 
					gsql_markers_clear_for_each,
					buffer);
	
	g_list_free (markers);
	
	HOOKUP_OBJECT_NO_REF (buffer, NULL, "markers");
	
}


/*
 *  Static section:
 *  gsql_markers_clear_for_each
 *  gsql_source_editor_property_set
 *
 *  on_editor_focus_in
 *  on_editor_focus_out
 *  on_editor_undo_activate
 *  on_editor_redo_activate
 *  on_editor_copy_activate
 *  on_editor_cut_activate
 *  on_editor_paste_activate
 *  on_editor_delete_activate
 *  on_editor_indent_activate
 *  on_editor_unindent_activate
 *  on_editor_comment_activate
 *  on_editor_uncomment_activate
 *  on_editor_upper_case_activate
 *  on_editor_lower_case_activate
 *
 */

static void
gsql_markers_clear_for_each (gpointer data, gpointer user_data)
{
	GSQL_TRACE_FUNC;
	
	GtkTextBuffer *buffer = user_data;
	gtk_text_buffer_delete_mark (buffer,
									 (GtkTextMark *) data);
	
}


static void 
gsql_source_editor_property_set (gpointer src)
{
	GSQL_TRACE_FUNC;
	
	GtkWidget *source = src;
	
	PangoFontDescription * font_desc;
	gchar * conf_string;
	gboolean conf_boolean;
	gint conf_int;

	conf_boolean = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_USE_SYSTEM_FONT);
    
	if (conf_boolean)
          conf_string = gsql_conf_value_get_string_at_root (GNOME_SYSTEM_FONT);
	else 
          conf_string = gsql_conf_value_get_string (GSQL_CONF_EDITOR_FONT_NAME);                
	
	if (!conf_string)
          conf_string = gsql_conf_value_get_string_at_root (GNOME_SYSTEM_FONT);
	
	font_desc = pango_font_description_from_string (conf_string);
	gtk_widget_modify_font (GTK_WIDGET (source), font_desc);
	
	conf_string = gsql_conf_value_get_string (GSQL_CONF_EDITOR_COLOR_SCHEME);

	if (conf_string)
	{
		GtkSourceBuffer *buff = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (src)));
		GtkSourceStyleSchemeManager* manager = gtk_source_style_scheme_manager_get_default();
		GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme (manager,
																				   conf_string);
		gtk_source_buffer_set_style_scheme (buff, scheme);
	}
       
	conf_boolean = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_HIGHLIGHT_LINE);
	gtk_source_view_set_highlight_current_line (GTK_SOURCE_VIEW (source), conf_boolean);
	conf_boolean = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_SHOW_LINE_NUM);
	gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW (source), conf_boolean);
	conf_boolean = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_WRAPPING);
	
	if (conf_boolean)
		gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (source), GTK_WRAP_WORD);
	else 
		gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (source), GTK_WRAP_NONE);
        
	conf_boolean = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_USE_SPACE);

	gtk_source_view_set_insert_spaces_instead_of_tabs (GTK_SOURCE_VIEW (source), conf_boolean);
        
	conf_int = gsql_conf_value_get_int (GSQL_CONF_EDITOR_TAB_WIDTH);

	if ((conf_int <= 0) || (conf_int > 8))
		conf_int = GSQL_CONF_EDITOR_TAB_WIDTH_DEFAULT;

	gtk_source_view_set_tab_width (GTK_SOURCE_VIEW (source), conf_int);
        
	conf_boolean = gsql_conf_value_get_boolean (GSQL_CONF_EDITOR_AUTO_INDENT);
	gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW (source), conf_boolean);
	
}

static gboolean 
on_editor_focus_in (GtkWidget *widget, GdkEventFocus *event,
							 gpointer user_data)
{
	GSQL_TRACE_FUNC;
	
	gtk_action_group_set_sensitive (editor_actions, TRUE);
	
	return FALSE;
}

static gboolean 
on_editor_focus_out (GtkWidget *widget, GdkEventFocus *event,
							 gpointer user_data)
{
	GSQL_TRACE_FUNC;
	
	gtk_action_group_set_sensitive (editor_actions, FALSE);
	
	return FALSE;
}


void 
on_editor_undo_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	GtkWidget *widget;
	
	widget = gtk_window_get_focus (GTK_WINDOW (gsql_window));
#if GTK_API_VERSION >= 3
	g_return_if_fail (GTK_SOURCE_IS_VIEW (widget));
#else
	g_return_if_fail (GTK_IS_SOURCE_VIEW (widget));
#endif
	
	g_signal_emit_by_name (widget, "undo");
}

void 
on_editor_redo_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	GtkWidget *widget;
	
	widget = gtk_window_get_focus (GTK_WINDOW (gsql_window));
	
#if GTK_API_VERSION >= 3
	g_return_if_fail (GTK_SOURCE_IS_VIEW (widget));
#else
	g_return_if_fail (GTK_IS_SOURCE_VIEW (widget));
#endif
	
	g_signal_emit_by_name (widget, "redo");
}

void 
on_editor_copy_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	GtkWidget *widget;
	
	widget = gtk_window_get_focus (GTK_WINDOW (gsql_window));
	
	g_return_if_fail (GTK_IS_TEXT_VIEW (widget));
	
	g_signal_emit_by_name (widget, "copy-clipboard");
	
}

void 
on_editor_cut_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	GtkWidget *widget;
	
	widget = gtk_window_get_focus (GTK_WINDOW (gsql_window));
	
	g_return_if_fail (GTK_IS_TEXT_VIEW (widget));
	
	g_signal_emit_by_name (widget, "cut-clipboard");

}

void 
on_editor_paste_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	GtkWidget *widget;
	
	widget = gtk_window_get_focus (GTK_WINDOW (gsql_window));
	
	g_return_if_fail (GTK_IS_TEXT_VIEW (widget));
	
	g_signal_emit_by_name (widget, "paste-clipboard");
}

void 
on_editor_delete_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	GtkWidget *widget;
	
	widget = gtk_window_get_focus (GTK_WINDOW (gsql_window));
	
	g_return_if_fail (GTK_IS_TEXT_VIEW (widget));
	
	g_signal_emit_by_name (widget, "backspace");
}

void 
on_editor_indent_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	
	
	

}

void 
on_editor_unindent_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	
	
	

}

void 
on_editor_comment_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	
	
	

}

void 
on_editor_uncomment_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	
	
	

}

void 
on_editor_upper_case_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	
	
	

}

void 
on_editor_lower_case_activate (GtkMenuItem *mi, gpointer data)
{
	GSQL_TRACE_FUNC;
	
	
	

}

