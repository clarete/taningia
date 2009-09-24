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

typedef struct _pubsub_t pubsub_t;
typedef struct _pubsub_node_t pubsub_node_t;

/* -- Pubsub -- */

/**
 * @name: pubsub_new
 * @type: constructor pubsub
 */
pubsub_t *pubsub_new (const char *from, const char *to);

/**
 * @name: pubsub_free
 * @type: destructor pubsub
 */
void pubsub_free (pubsub_t *ctx);

/**
 * @name: pubsub_get_node_prefix
 * @type: getter pubusb:node_prefix
 */
const char *pubsub_get_node_prefix (pubsub_t *ctx);

/**
 * @name: pubsub_set_node_prefix
 * @type: setter pubusb:node_prefix
 */
void pubsub_set_node_prefix (pubsub_t *ctx, const char *prefix);

/**
 * @name: pubsub_query_features
 * @type: method pubsub
 *
 * Build a stanza to be sent to an xmpp server that query for all
 * available pubsub features.
 */
iks *pubsub_query_features (pubsub_t *ctx);

/**
 * @name: pubsub_query_affiliations
 * @type: method pubsub
 *
 * Build a stanza that queries all affiliations in a pubsub service.
 */
iks *pubsub_query_affiliations (pubsub_t *ctx);

/* -- Pubsub Node -- */

/**
 * @name: pubsub_node_new
 * @type: constructor pubsub_node
 * @param name (optional): Node name
 */
pubsub_node_t *pubsub_node_new (pubsub_t *ctx, const char *name);

/**
 * @name: pubsub_node_free
 * @type: destructor pubsub_node
 */
void pubsub_node_free (pubsub_node_t *node);

/**
 * @name: pubsub_node_get_name
 * @type: getter pubsub_node:name
 */
const char *pubsub_node_get_name (pubsub_node_t *node);

/**
 * @name: pubsub_node_set_name
 * @type: setter pubsub_node:name
 */
void pubsub_node_set_name (pubsub_node_t *node, const char *name);

/**
 * @name: pubsub_node_query_info
 * @type: method pubsub_node
 *
 * Build a stanza to query node info.
 */
iks *pubsub_node_query_info (pubsub_node_t *node);

/**
 * @name: pubsub_node_subscriptions
 * @type: method pubsub_node
 *
 * Build a stanza to request all node subscriptions.
 */
iks *pubsub_node_subscriptions (pubsub_node_t *node);

/**
 * @name: pubsub_node_affiliations
 * @type: method pubsub_node
 *
 * Build a stanza to request all node affiliations.
 */
iks *pubsub_node_affiliations (pubsub_node_t *node);

/**
 * @name: pubsub_node_subscribe
 * @type: method pubsub_node
 * @param jid (optional): The jid to be subscribed.
 *
 * Build a stanza to subscribe to a node. If no `jid' is passed, the
 * jid set in context will be used.
 */
iks *pubsub_node_subscribe (pubsub_node_t *node, const char *jid);

/**
 * @name: pubsub_node_unsubscribe
 * @type: method pubsub_node
 * @param jid (optional): The jid to be unsubscribed.
 *
 * Build a stanza to unsubscribe from a node. If no `jid' is passed,
 * the jid set in context will be used.
 * 
 */
iks *pubsub_node_unsubscribe (pubsub_node_t *node, const char *jid);

/*
  iks *pubsub_node_items (pubsub_node_t *node);
*/

/**
 * @name: pubsub_node_items
 * @type: method pubsub_node
 * @param max_items: Maximum entries to be returned from the server.
 *
 * Build a stanza to get the list of entries of a node.
 */
iks *pubsub_node_items (pubsub_node_t *node, int max_items);

/**
 * @name: pubsub_node_publish_text
 * @type: method pubsub_node
 * @param id (nullable): The id of published entry.
 * @param body: The text body to be published.
 * @param len (optional): The length of the text body.
 *
 * Build a stanza to publish a bunch of text in the node.
 */
iks *pubsub_node_publish_text (pubsub_node_t *node, const char *id,
                               const char *body, int len);

/**
 * @name: pubsub_node_publish_iks
 * @type: method pubsub_node
 * @param id (nullable): The id of published entry.
 * @param child: The iks object to be published.
 *
 * Build a stanza to publish an iks object in the node.
 */
iks *pubsub_node_publish_iks (pubsub_node_t *node, const char *id, iks *child);

/**
 * @name: pubsub_node_delete_item
 * @type: method pubsub_node
 * @param id: The id of the object to be deleted.
 *
 * Build a stanza to delete an entry from a node.
 */
iks *pubsub_node_delete_item (pubsub_node_t *node, const char *id);

/**
 * @name: pubsub_node_create
 * @type: method pubsub_node
 *
 * Build a stanza to create a node. This function allows to create
 * <strong>and</strong> configure a ndoe at once. To do it, pass the
 * configuration information to the var args and end it with a NULL
 * value, like this:
 *
 * <pre>
 * pubsub_node_create (mynode, "type", "leaf", NULL);
 * </pre>
 */
iks *pubsub_node_create (pubsub_node_t *node, ...);

/**
 * @name: pubsub_node_createv
 * @type: method pubsub_node
 * @param conf_params: Configuration parameters.
 * @see: pubsub_node_create
 *
 * Same of `pubsub_node_create' but without using var args to receive
 * config parameters. Use it like this:
 *
 * <pre>
 * const char** params = { "type", "leaf", NULL };
 * pubsub_node_createv (mynode, params);
 * </pre>
 */
iks *pubsub_node_createv (pubsub_node_t *node, const char **conf_params);

#endif /* _TANINGIA_PUBSUB_H */
