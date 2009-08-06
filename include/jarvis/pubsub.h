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

#ifndef _PUBSUB_H
#define _PUBSUB_H

#include <iksemel.h>

#define PUBSUB_NS "http://jabber.org/protocol/pubsub"

typedef struct _ps_ctx ps_ctx;

ps_ctx *ps_ctx_new                      (const char   *from,
                                         const char   *to);

void    ps_ctx_free                     (ps_ctx       *ctx);

iks    *ps_query_features               (ps_ctx       *ctx);

iks    *ps_query_node_info              (ps_ctx       *ctx,
                                         const char   *node);

iks    *ps_query_node_items             (ps_ctx       *ctx,
                                         const char   *node);

iks    *ps_query_node_subscriptions     (ps_ctx       *ctx,
                                         const char   *node);

iks    *ps_query_node_affiliations      (ps_ctx       *ctx);

iks    *ps_node_subscribe               (ps_ctx       *ctx,
                                         const char   *node,
                                         const char   *jid);

iks    *ps_node_unsubscribe             (ps_ctx       *ctx,
                                         const char   *node,
                                         const char   *jid);

iks    *ps_node_retrieve_items          (ps_ctx       *ctx,
                                         const char   *node,
                                         int           max_items);

iks    *ps_publish_item_text            (ps_ctx       *ctx,
                                         const char   *node,
                                         const char   *id,
                                         const char   *body,
                                         int           len);

iks    *ps_delete_item                  (ps_ctx       *ctx,
                                         const char   *node,
                                         const char   *id);

iks    *ps_node_create                  (ps_ctx       *ctx,
                                         const char   *node,
                                         int           bare,
                                         ...);

#endif /* _PUBSUB_H */
