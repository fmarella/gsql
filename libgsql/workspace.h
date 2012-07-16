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

 
#ifndef _WORKSPACE_H
#define _WORKSPACE_H


typedef struct _GSQLWorkspace 			GSQLWorkspace;
typedef struct _GSQLWorkspaceClass		GSQLWorkspaceClass;
typedef struct _GSQLWorkspacePrivate	GSQLWorkspacePrivate;

#define GSQL_WORKSPACE_TYPE 			(gsql_workspace_get_type ())
#define GSQL_WORKSPACE(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GSQL_WORKSPACE_TYPE, GSQLWorkspace))
#define GSQL_WORKSPACE_CLASS(klass)	(G_TYPE_CHECK_INSTANCE_CAST ((klass), GSQL_WORKSPACE_TYPE, GSQLWorkspaceClass))

#define GSQL_IS_WORKSPACE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSQL_WORKSPACE_TYPE))
#define GSQL_IS_WORKSPACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSQL_WORKSPACE_TYPE))


#define GSQL_CONF_WORKSPACE_SHOW_NAVIGATE	GSQL_SCHEMA_WORKSPACE, "show-navarea"
#define GSQL_CONF_WORKSPACE_SHOW_MESSAGES	GSQL_SCHEMA_WORKSPACE, "show-messarea"
#define GSQL_CONF_WORKSPACE_WIDESCREEN		GSQL_SCHEMA_WORKSPACE, "widescreen"

#include <libgsql/content.h>
#include <libgsql/navigation.h>
#include <libgsql/session.h>
#include <libgsql/conf.h>

struct _GSQLWorkspace
{
	GtkContainer	parent;
	
	
	GSQLWorkspacePrivate *private;
};

struct _GSQLWorkspaceClass
{
	GtkContainerClass parent;
	
	/* signals */
	void (*on_close) 	(GSQLWorkspace *workspace);
	void (*on_activate)	(GSQLWorkspace *workspace);
};

typedef enum {
	GSQL_MESSAGE_NORMAL,
	GSQL_MESSAGE_NOTICE,
	GSQL_MESSAGE_WARNING,
	GSQL_MESSAGE_ERROR,
	GSQL_MESSAGE_OUTPUT
} GSQLMessageType;

#define GSQL_MESSAGE_LEN 2048

G_BEGIN_DECLS

GType gsql_workspace_get_type (void);

GSQLWorkspace *
gsql_workspace_new (GSQLSession *session);

GSQLSession *
gsql_workspace_get_session (GSQLWorkspace *workspace);

void
gsql_workspace_add_content (GSQLWorkspace *workspace, GSQLContent *content);

GList *
gsql_workspace_get_content_list (GSQLWorkspace *workspace);

GSQLContent *
gsql_workspace_get_current_content (GSQLWorkspace *workspace);

void
gsql_workspace_next_content (GSQLWorkspace *workspace);

void
gsql_workspace_prev_content (GSQLWorkspace *workspace);

void
gsql_workspace_set_content_page (GSQLWorkspace *workspace, GSQLContent *content);

GSQLNavigation *
gsql_workspace_get_navigation (GSQLWorkspace *workspace);

GtkTreeView *
gsql_workspace_get_details (GSQLWorkspace *workspace);

void
gsql_workspace_set_details (GSQLWorkspace *workspace,
							GtkListStore *details_store);

void
gsql_workspace_set_navigate_visible (GSQLWorkspace *workspace, gboolean visible);

void
gsql_workspace_set_messages_visible (GSQLWorkspace *workspace, gboolean visible);

gboolean
gsql_workspace_get_navigate_visible (GSQLWorkspace *workspace);

gboolean
gsql_workspace_get_messages_visible (GSQLWorkspace *workspace);

void 
gsql_message_add(GSQLWorkspace *workspace, gint type, gchar * msg);

G_END_DECLS

#endif /* _WORKSPACE_H */
