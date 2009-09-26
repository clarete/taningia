/*
 * Copyright (C) 2009  Lincoln de Sousa <lincoln@minaslivre.org>
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

#define TA_PUBSUB_NS "http://jabber.org/protocol/pubsub"

typedef struct _ta_pubsub_t ta_pubsub_t;
typedef struct _ta_pubsub_node_t ta_pubsub_node_t;

/* -- Pubsub -- */

/**
 * @name: ta_pubsub_new
 * @type: constructor ta_pubsub
 */
ta_pubsub_t *ta_pubsub_new (const char *from, const char *to);

/**
 * @name: ta_pubsub_free
 * @type: destructor ta_pubsub
 */
void ta_pubsub_free (ta_pubsub_t *ctx);

/**
 * @name: ta_pubsub_get_node_prefix
 * @type: getter ta_pubusb:node_prefix
 */
const char *ta_pubsub_get_node_prefix (ta_pubsub_t *ctx);

/**
 * @name: ta_pubsub_set_node_prefix
 * @type: setter ta_pubusb:node_prefix
 */
void ta_pubsub_set_node_prefix (ta_pubsub_t *ctx, const char *prefix);

/**
 * @name: ta_pubsub_query_features
 * @type: method ta_pubsub
 *
 * Build a stanza to be sent to an xmpp server that query for all
 * available pubsub features.
 */
iks *ta_pubsub_query_features (ta_pubsub_t *ctx);

/**
 * @name: ta_pubsub_query_affiliations
 * @type: method ta_pubsub
 *
 * Build a stanza that queries all affiliations in a pubsub service.
 */
iks *ta_pubsub_query_affiliations (ta_pubsub_t *ctx);

/* -- Pubsub Node -- */

/**
 * @name: ta_pubsub_node_new
 * @type: constructor ta_pubsub_node
 * @param name (optional): Node name
 */
ta_pubsub_node_t *ta_pubsub_node_new (ta_pubsub_t *ctx, const char *name);

/**
 * @name: ta_pubsub_node_free
 * @type: destructor ta_pubsub_node
 */
void ta_pubsub_node_free (ta_pubsub_node_t *node);

/**
 * @name: ta_pubsub_node_get_name
 * @type: getter ta_pubsub_node:name
 */
const char *ta_pubsub_node_get_name (ta_pubsub_node_t *node);

/**
 * @name: ta_pubsub_node_set_name
 * @type: setter ta_pubsub_node:name
 */
void ta_pubsub_node_set_name (ta_pubsub_node_t *node, const char *name);

/**
 * @name: ta_pubsub_node_query_info
 * @type: method ta_pubsub_node
 *
 * Build a stanza to query node info.
 */
iks *ta_pubsub_node_query_info (ta_pubsub_node_t *node);

/**
 * @name: ta_pubsub_node_subscriptions
 * @type: method ta_pubsub_node
 *
 * Build a stanza to request all node subscriptions.
 */
iks *ta_pubsub_node_subscriptions (ta_pubsub_node_t *node);

/**
 * @name: ta_pubsub_node_affiliations
 * @type: method ta_pubsub_node
 *
 * Build a stanza to request all node affiliations.
 */
iks *ta_pubsub_node_affiliations (ta_pubsub_node_t *node);

/**
 * @name: ta_pubsub_node_subscribe
 * @type: method ta_pubsub_node
 * @param jid (optional): The jid to be subscribed.
 *
 * Build a stanza to subscribe to a node. If no `jid' is passed, the
 * jid set in context will be used.
 */
iks *ta_pubsub_node_subscribe (ta_pubsub_node_t *node, const char *jid);

/**
 * @name: ta_pubsub_node_unsubscribe
 * @type: method ta_pubsub_node
 * @param jid (optional): The jid to be unsubscribed.
 *
 * Build a stanza to unsubscribe from a node. If no `jid' is passed,
 * the jid set in context will be used.
 * 
 */
iks *ta_pubsub_node_unsubscribe (ta_pubsub_node_t *node, const char *jid);

/**
 * @name: ta_pubsub_node_nodes
 * @type: method ta_pubsub_node
 *
 * Discover all nodes in a node instance.
 */
iks *ta_pubsub_node_nodes (ta_pubsub_node_t *node);

/**
 * @name: ta_pubsub_node_items
 * @type: method ta_pubsub_node
 * @param max_items: Maximum entries to be returned from the server.
 *
 * Build a stanza to get the list of entries of a node.
 */
iks *ta_pubsub_node_items (ta_pubsub_node_t *node, int max_items);

/**
 * @name: ta_pubsub_node_publish_text
 * @type: method ta_pubsub_node
 * @param id (nullable): The id of published entry.
 * @param body: The text body to be published.
 * @param len (optional): The length of the text body.
 *
 * Build a stanza to publish a bunch of text in the node.
 */
iks *ta_pubsub_node_publish_text (ta_pubsub_node_t *node, const char *id,
                                  const char *body, int len);

/**
 * @name: ta_pubsub_node_publish_iks
 * @type: method ta_pubsub_node
 * @param id (nullable): The id of published entry.
 * @param child: The iks object to be published.
 *
 * Build a stanza to publish an iks object in the node.
 */
iks *ta_pubsub_node_publish_iks (ta_pubsub_node_t *node, const char *id, iks *child);

/**
 * @name: ta_pubsub_node_delete_item
 * @type: method ta_pubsub_node
 * @param id: The id of the object to be deleted.
 *
 * Build a stanza to delete an entry from a node.
 */
iks *ta_pubsub_node_delete_item (ta_pubsub_node_t *node, const char *id);

/**
 * @name: ta_pubsub_node_create
 * @type: method ta_pubsub_node
 *
 * Build a stanza to create a node. This function allows to create
 * <strong>and</strong> configure a ndoe at once. To do it, pass the
 * configuration information to the var args and end it with a NULL
 * value, like this:
 *
 * <pre>
 * ta_pubsub_node_create (mynode, "type", "leaf", NULL);
 * </pre>
 */
iks *ta_pubsub_node_create (ta_pubsub_node_t *node, ...);

/**
 * @name: ta_pubsub_node_createv
 * @type: method ta_pubsub_node
 * @param conf_params: Configuration parameters.
 * @see: ta_pubsub_node_create
 *
 * Same of `ta_pubsub_node_create' but without using var args to receive
 * config parameters. Use it like this:
 *
 * <pre>
 * const char** params = { "type", "leaf", NULL };
 * ta_pubsub_node_createv (mynode, params);
 * </pre>
 */
iks *ta_pubsub_node_createv (ta_pubsub_node_t *node, const char **conf_params);

#endif /* _TANINGIA_PUBSUB_H */
