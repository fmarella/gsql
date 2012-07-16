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

 
#ifndef _CONF_H
#define _CONF_H

#include <glib.h>
#include <gtk/gtk.h>

#define GSQL_SCHEMA              "org.gsql"
#define GSQL_SCHEMA_UI           "org.gsql.ui"
#define GSQL_SCHEMA_EDITOR       "org.gsql.editor"
#define GSQL_SCHEMA_WORKSPACE    "org.gsql.workspace"

#define GSQL_CONF_ROOT_KEY       "/org/gsql"
#define GSQL_CONF_ENGINES_ROOT_KEY GSQL_CONF_ROOT_KEY "/engines"
#define GSQL_CONF_PLUGINS_ROOT_KEY GSQL_CONF_ROOT_KEY "/plugins"

typedef void (*GSQLConfNotifyFunc) (gpointer userdata);

G_BEGIN_DECLS
 
void
gsql_conf_init();

GSettings *
gsql_conf_add_schema (gchar *schema);

GSettings *
gsql_conf_get_schema (gchar *schema);

gint
gsql_conf_value_get_int (gchar *schema, gchar *path);

gchar *
gsql_conf_value_get_string (gchar *schema, gchar *path);

gchar *
gsql_conf_value_get_string_at_root (gchar *schema, gchar *path);

gboolean
gsql_conf_value_get_boolean (gchar *schema, gchar *path);


void
gsql_conf_value_set_int (gchar *schema, gchar *path, gint value);

void
gsql_conf_value_set_string (gchar *schema, gchar *path, gchar *value);

void
gsql_conf_value_set_boolean (gchar *schema, gchar *path, gboolean value);

GSList *
gsql_conf_dir_list (gchar *path);

gboolean
gsql_conf_dir_exist (gchar *path);

void
gsql_conf_value_unset (gchar *path, gboolean recursive);

void
gsql_conf_nitify_add (gchar *path, GSQLConfNotifyFunc func, gpointer userdata);

G_END_DECLS

#endif /* _CONF_H */

 
