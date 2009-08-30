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

#ifndef _TANINGIA_PUBSUB_H
#define _TANINGIA_PUBSUB_H

#include <iksemel.h>
#include <taningia/taningia.h>

#define PUBSUB_NS "http://jabber.org/protocol/pubsub"

typedef struct _TPubsub TPubsub;
typedef struct _TPubsubNode TPubsubNode;

/* TPubsub */

TPubsub    *t_pubsub_new                     (const char *from,
                                              const char *to);

void        t_pubsub_free                    (TPubsub    *ctx);

iks        *t_pubsub_query_features          (TPubsub    *ctx);

iks        *t_pubsub_query_affiliations      (TPubsub    *ctx);

const char *t_pubsub_get_node_prefix         (TPubsub    *ctx);

void        t_pubsub_set_node_prefix         (TPubsub    *ctx,
                                              const char *prefix);

/* TPubsubNode */

TPubsubNode *t_pubsub_node_new               (TPubsub    *ctx,
                                              const char *name _optional_);

void         t_pubsub_node_free              (TPubsubNode *node);

const char  *t_pubsub_node_get_name          (TPubsubNode *node);

void         t_pubsub_node_set_name          (TPubsubNode *node,
                                              const char  *name);

iks         *t_pubsub_node_query_info        (TPubsubNode *node);

iks         *t_pubsub_node_subscriptions     (TPubsubNode *node);

iks         *t_pubsub_node_affiliations      (TPubsubNode *node);

iks         *t_pubsub_node_subscribe         (TPubsubNode *node,
                                              const char  *jid _optional_);

iks         *t_pubsub_node_unsubscribe       (TPubsubNode *node,
                                              const char  *jid _optional_);

/*
iks         *t_pubsub_node_items             (TPubsubNode *node);
*/

iks         *t_pubsub_node_items             (TPubsubNode *node,
                                              int          max_items);

iks         *t_pubsub_node_publish_text      (TPubsubNode *node,
                                              const char  *id _optional_,
                                              const char  *body,
                                              int          len _len_);

iks         *t_pubsub_node_publish_iks       (TPubsubNode *node,
                                              const char  *id _optional_,
                                              iks         *child);

iks         *t_pubsub_node_delete_item       (TPubsubNode *node,
                                              const char  *id);

iks         *t_pubsub_node_create            (TPubsubNode *node,
                                              ...);

iks         *t_pubsub_node_createv           (TPubsubNode *node,
                                              const char **conf_params);

#endif /* _TANINGIA_PUBSUB_H */
