/*
 * Copyright (C) 2009 Lincoln de Sousa <lincoln@minaslivre.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _JARVIS_PUBSUB_H
#define _JARVIS_PUBSUB_H

#include <iksemel.h>

#define PUBSUB_NS "http://jabber.org/protocol/pubsub"

typedef struct _JPsCtx JPsCtx;

JPsCtx *j_ps_new                        (const char   *from,
                                         const char   *to);

void    j_ps_free                       (JPsCtx       *ctx);

iks    *j_ps_query_features             (JPsCtx       *ctx);

iks    *j_ps_query_node_info            (JPsCtx       *ctx,
                                         const char   *node);

iks    *j_ps_query_node_items           (JPsCtx       *ctx,
                                         const char   *node);

iks    *j_ps_query_node_subscriptions   (JPsCtx       *ctx,
                                         const char   *node);

iks    *j_ps_query_node_affiliations    (JPsCtx       *ctx);

iks    *j_ps_node_subscribe             (JPsCtx       *ctx,
                                         const char   *node,
                                         const char   *jid);

iks    *j_ps_node_unsubscribe           (JPsCtx       *ctx,
                                         const char   *node,
                                         const char   *jid);

iks    *j_ps_node_retrieve_items        (JPsCtx       *ctx,
                                         const char   *node,
                                         int           max_items);

iks    *j_ps_publish_item_text          (JPsCtx       *ctx,
                                         const char   *node,
                                         const char   *id,
                                         const char   *body,
                                         int           len);

iks    *j_ps_publish_item_iks           (JPsCtx       *ctx,
                                         const char   *node,
                                         const char   *id,
                                         iks          *child);

iks    *j_ps_delete_item                (JPsCtx       *ctx,
                                         const char   *node,
                                         const char   *id);

iks    *j_ps_node_create                (JPsCtx       *ctx,
                                         const char   *node,
                                         int           bare,
                                         ...);

#endif /* _JARVIS_PUBSUB_H */
