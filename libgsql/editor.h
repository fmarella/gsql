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


#ifndef _GSQLEDITOR_H
#define _GSQLEDITOR_H

#include <gtk/gtk.h>
#include <glib.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <libgsql/conf.h>

#define GSQL_CONF_EDITOR_USE_SYSTEM_FONT	GSQL_SCHEMA_EDITOR, "use-system-font"
#define GSQL_CONF_EDITOR_FONT_NAME		GSQL_SCHEMA_EDITOR, "font-name"
#define GSQL_CONF_EDITOR_COLOR_SCHEME		GSQL_SCHEMA_EDITOR, "color-scheme"
#define GSQL_CONF_EDITOR_USE_SPACE		GSQL_SCHEMA_EDITOR, "use-space-instead-tab"
#define GSQL_CONF_EDITOR_TAB_WIDTH		GSQL_SCHEMA_EDITOR, "tab-width"
#define GSQL_CONF_EDITOR_SHOW_LINE_NUM		GSQL_SCHEMA_EDITOR, "show-line-number"
#define GSQL_CONF_EDITOR_HIGHLIGHT_LINE		GSQL_SCHEMA_EDITOR, "highlight-line"
#define GSQL_CONF_EDITOR_WRAPPING		GSQL_SCHEMA_EDITOR, "enable-text-wrapping"
#define GSQL_CONF_EDITOR_AUTO_INDENT		GSQL_SCHEMA_EDITOR, "enable-auto-indent"

#define GSQL_CONF_EDITOR_TAB_WIDTH_DEFAULT  4

// default fetch rows limit 
#define SQL_EDITOR_CUSTOM_FETCH_LIMIT	100

#define GSQL_EDITOR_MARKER_COMPLETE "complete"
#define GSQL_EDITOR_MARKER_FAILED   "error"

GtkWidget *
gsql_source_editor_new(gchar * buffer_body);

void 
gsql_source_editor_marker_set (GtkTextIter *iter, gchar *marker_type);

#endif /* _GSQLEDITOR_H */
