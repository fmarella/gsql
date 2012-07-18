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

 
#ifndef _GSQLCONF_H
#define _GSQLCONF_H

#include <libgsql/common.h>
#include <libgsql/conf.h>

#define GSQL_CONF_UI_RESTORE_SIZE_POS		GSQL_SCHEMA_UI, "restore-size-and-position"
#define GSQL_CONF_UI_SIZE_X			GSQL_SCHEMA_UI, "size-x"
#define GSQL_CONF_UI_SIZE_Y			GSQL_SCHEMA_UI, "size-y"
#define GSQL_CONF_UI_POS_X			GSQL_SCHEMA_UI, "pos-x"
#define GSQL_CONF_UI_POS_Y			GSQL_SCHEMA_UI, "pos-y"
#define GSQL_CONF_UI_MAXIMIZED			GSQL_SCHEMA_UI, "window-maximized"
#define GSQL_CONF_UI_FULLSCREEN			GSQL_SCHEMA_UI, "window-fullscreen"

G_BEGIN_DECLS

void
gsql_conf_init();
    
void
gsql_conf_dialog();

G_END_DECLS

#endif /* _GSQLCONF_H */
