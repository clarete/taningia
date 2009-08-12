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

typedef struct _JPubsub JPubsub;

JPubsub *j_pubsub_new                        (const char   *from,
                                              const char   *to);

void     j_pubsub_free                       (JPubsub      *ctx);

iks     *j_pubsub_query_features             (JPubsub      *ctx);

iks     *j_pubsub_query_node_info            (JPubsub      *ctx,
                                              const char   *node);

iks     *j_pubsub_query_node_items           (JPubsub      *ctx,
                                              const char   *node);

iks     *j_pubsub_query_node_subscriptions   (JPubsub      *ctx,
                                              const char   *node);

iks     *j_pubsub_query_node_affiliations    (JPubsub      *ctx);

iks     *j_pubsub_node_subscribe             (JPubsub      *ctx,
                                              const char   *node,
                                              const char   *jid);

iks     *j_pubsub_node_unsubscribe           (JPubsub      *ctx,
                                              const char   *node,
                                              const char   *jid);

iks     *j_pubsub_node_retrieve_items        (JPubsub      *ctx,
                                              const char   *node,
                                              int           max_items);

iks     *j_pubsub_publish_item_text          (JPubsub      *ctx,
                                              const char   *node,
                                              const char   *id,
                                              const char   *body,
                                              int           len);

iks     *j_pubsub_publish_item_iks           (JPubsub      *ctx,
                                              const char   *node,
                                              const char   *id,
                                              iks          *child);

iks     *j_pubsub_delete_item                (JPubsub      *ctx,
                                              const char   *node,
                                              const char   *id);

iks     *j_pubsub_node_create                (JPubsub      *ctx,
                                              const char   *node,
                                              int           bare,
                                              ...);

#endif /* _JARVIS_PUBSUB_H */
