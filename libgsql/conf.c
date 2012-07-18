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


#include <libgsql/conf.h>
#include <libgsql/common.h>

static GHashTable *all_settings;

void
gsql_conf_init()
{
  GSQL_TRACE_FUNC;

  all_settings = g_hash_table_new(g_str_hash, g_str_equal);

  gsql_conf_add_schema (GSQL_SCHEMA);
  gsql_conf_add_schema (GSQL_SCHEMA_UI);
}

GSettings *
gsql_conf_add_schema (gchar *schema) {
  GSettings *settings;
  settings = g_settings_new (schema);

  if (settings == NULL) {
    g_warning (_("Cannot initialize configuration manager."));
  } else {
    g_hash_table_insert(all_settings, g_strdup(schema), settings);
  }

  return settings;
}

GSettings *
gsql_conf_get_schema (gchar *schema) {
  GSettings *settings;

  settings = g_hash_table_lookup (all_settings, schema);
  if (settings == NULL) {
    settings = gsql_conf_add_schema (schema);
  }

  return settings;
}

void
gsql_conf_nitify_add (gchar *path, GSQLConfNotifyFunc func, gpointer userdata)
{
	GSQL_TRACE_FUNC;

	GError *error = NULL;
	
	g_return_if_fail (G_IS_OBJECT (userdata));

	HOOKUP_OBJECT_NO_REF(userdata, func, "fhandler");

	/* g_settings_bind (settings, gsql_conf_notify_handler); */

	/* gconf_client_notify_add (gconf_client,  */
	/* 							path, */
	/* 							gsql_conf_notify_handler, */
	/* 							 userdata, NULL, NULL); */
}


/**
 * gsql_conf_value_get_int:
 * @path: path to the key
 *	
 * Get the value by path
 *
 * Returns: int value
 **/

gint
gsql_conf_value_get_int (gchar *schema, gchar *key)
{
	GSQL_TRACE_FUNC;

	gint value;
        
	g_return_val_if_fail (key != NULL, 0);

	value = g_settings_get_int (gsql_conf_get_schema (schema), key);

	return value;
}



/**
 * gsql_conf_value_get_string:
 * @path: path to the key
 *
 * Get the value by path
 *
 * Returns: string value. returned value should be freed by g_free()
 **/

gchar *
gsql_conf_value_get_string (gchar *schema, gchar *key)
{
	GSQL_TRACE_FUNC;

	g_return_val_if_fail (key != NULL, NULL);

	return g_settings_get_string (gsql_conf_get_schema (schema), key);
}

gchar *
gsql_conf_value_get_string_at_root (gchar *schema, gchar *key)
{
	GSQL_TRACE_FUNC;

	g_return_val_if_fail (key != NULL, NULL);
	
	return g_settings_get_string (gsql_conf_get_schema (schema), key);
}

gboolean
gsql_conf_value_get_boolean (gchar *schema, gchar *key)
{
	GSQL_TRACE_FUNC;

	gboolean value;
	
	g_return_val_if_fail (key != NULL, FALSE);
	
	value = g_settings_get_boolean (gsql_conf_get_schema (schema), key);

	return value;
}

void
gsql_conf_value_set_int (gchar *schema, gchar *key, gint value)
{
	GSQL_TRACE_FUNC;

	g_return_if_fail (key);
	
	g_settings_set_int (gsql_conf_get_schema (schema), key, value);
}

void
gsql_conf_value_set_string (gchar *schema, gchar *key, gchar *value)
{
	GSQL_TRACE_FUNC;

	g_return_if_fail (key);
	
	g_settings_set_string (gsql_conf_get_schema (schema), key, value);
}

void
gsql_conf_value_set_boolean (gchar *schema, gchar *key, gboolean value)
{
	GSQL_TRACE_FUNC;

	g_return_if_fail (key);

	g_settings_set_boolean (gsql_conf_get_schema (schema), key, value);
}

GSList *
gsql_conf_dir_list (gchar *path)
{
	GSQL_TRACE_FUNC;

	GSList *ret;

	/* g_return_if_fail (path); */

	/* GError *error = NULL; */

	/* ret = gconf_client_all_dirs (gconf_client, path, &error); */

	/* if (error) g_error_free (error); */

	return ret;
}

gboolean
gsql_conf_dir_exist (gchar *path)
{
	GSQL_TRACE_FUNC;

	/* gboolean ret; */

	/* g_return_if_fail (path); */

	/* GError *error = NULL; */

	/* ret = gconf_client_dir_exists (gconf_client, */
	/* 					  path, */
	/* 					  &error); */
	/* if (error) */
	/* 	g_error_free (error); */

	return FALSE;

}

void
gsql_conf_value_unset (gchar *path, gboolean recursive)
{
	GSQL_TRACE_FUNC;

	/* GError *error = NULL; */
	/* gboolean	ret = TRUE; */

	/* g_debug ("removing: %s", path); */

	/* if (!recursive) */
	/* { */
	/* 	ret = gconf_client_unset (gconf_client, */
	/* 	    				path, */
	/* 	    				&error); */
	/* } else { */

	/* 	gconf_client_recursive_unset (gconf_client, path, 0, &error); */
	/* } */

	/* if (!ret) */
	/* 	g_debug ("Can not to unset the key '%s'", path); */
	
	/* if (error) */
	/* { */
	/* 	g_debug ("%s", error->message); */
	/* 	g_error_free (error); */
	/* } */

	/* gconf_client_suggest_sync (gconf_client, &error); */
}
