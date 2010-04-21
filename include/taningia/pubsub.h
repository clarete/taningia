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

#ifdef __cplusplus
extern "C" {
#endif

#include <iksemel.h>

#define TA_PUBSUB_NS "http://jabber.org/protocol/pubsub"

/* -- Pubsub -- */

/**
 * @name: ta_pubsub_query_features
 * @type: function
 *
 * Build a stanza to be sent to an xmpp server that query for all
 * available pubsub features.
 */
iks *ta_pubsub_query_features (const char *from, const char *to);

/**
 * @name: ta_pubsub_query_affiliations
 * @type: function
 *
 * Build a stanza that queries all affiliations in a pubsub service.
 */
iks *ta_pubsub_query_affiliations (const char *from, const char *to);

/* -- Pubsub Node -- */

/**
 * @name: ta_pubsub_node_query_info
 * @type: function
 *
 * Build a stanza to query node info.
 */
iks *ta_pubsub_node_query_info (const char *from,
                                const char *to,
                                const char *node);

/**
 * @name: ta_pubsub_node_subscriptions
 * @type: function
 *
 * Build a stanza to request all node subscriptions.
 */
iks *ta_pubsub_node_query_subscriptions (const char *from,
                                         const char *to,
                                         const char *node);

/**
 * @name: ta_pubsub_node_affiliations
 * @type: function
 *
 * Build a stanza to request all node affiliations.
 */
iks *ta_pubsub_node_query_affiliations (const char *from,
                                        const char *to,
                                        const char *node);

/**
 * @name: ta_pubsub_node_subscribe
 * @type: function
 * @param jid (optional): The jid to be subscribed.
 *
 * Build a stanza to subscribe to a node. If no `jid' is passed, the
 * jid set in context will be used.
 */
iks *ta_pubsub_node_subscribe (const char *from,
                               const char *to,
                               const char *node,
                               const char *jid);

/**
 * @name: ta_pubsub_node_unsubscribe
 * @type: function
 * @param jid (optional): The jid to be unsubscribed.
 *
 * Build a stanza to unsubscribe from a node. If no `jid' is passed,
 * the jid set in context will be used.
 */
iks *ta_pubsub_node_unsubscribe (const char *from,
                                 const char *to,
                                 const char *node,
                                 const char *jid);

/**
 * @name: ta_pubsub_node_query_nodes
 * @type: function
 *
 * Discover all nodes in a node instance.
 */
iks *ta_pubsub_node_query_nodes (const char *from,
                                 const char *to,
                                 const char *node);

/**
 * @name: ta_pubsub_node_items
 * @type: function
 * @param max_items: Maximum entries to be returned from the server.
 *
 * Build a stanza to get the list of entries of a node.
 */
iks *ta_pubsub_node_items (const char *from,
                           const char *to,
                           const char *node,
                           int max_items);

/**
 * @name: ta_pubsub_node_publish_text
 * @type: method function
 * @param id (nullable): The id of published entry.
 * @param body: The text body to be published.
 * @param len (optional): The length of the text body.
 *
 * Build a stanza to publish a bunch of text in the node.
 */
iks *ta_pubsub_node_publish_text (const char *from,
                                  const char *to,
                                  const char *node,
                                  const char *id,
                                  const char *body,
                                  int len);

/**
 * @name: ta_pubsub_node_publish_iks
 * @type: function
 * @param id (nullable): The id of published entry.
 * @param child: The iks object to be published.
 *
 * Build a stanza to publish an iks object in the node.
 */
iks *ta_pubsub_node_publish_iks (const char *from,
                                 const char *to,
                                 const char *node,
                                 const char *id,
                                 iks *child);

/**
 * @name: ta_pubsub_node_delete
 * @type: function
 * @param id: The id of the node to be deleted.
 *
 * Builds a stanza to delete a node.
 */
iks *ta_pubsub_node_delete (const char *from,
                            const char *to,
                            const char *node);

/**
 * @name: ta_pubsub_node_delete_item
 * @type: function
 * @param id: The id of the object to be deleted.
 *
 * Build a stanza to delete an entry from a node.
 */
iks *ta_pubsub_node_delete_item (const char *from,
                                 const char *to,
                                 const char *node,
                                 const char *id);

/**
 * @name: ta_pubsub_node_create
 * @type: function
 *
 * Build a stanza to create a node. This function allows to create
 * <strong>and</strong> configure a ndoe at once. To do it, pass the
 * configuration information to the var args and end it with a NULL
 * value, like this:
 *
 * <pre>
 * ta_pubsub_node_create ("test@blah", "pubsub.blah", "/mynode",
 *                        "type", "leaf", NULL);
 * </pre>
 */
iks *ta_pubsub_node_create (const char *from,
                            const char *to,
                            const char *node,
                            ...);

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
 * ta_pubsub_node_createv ("test@blah", "pubsub.blah", "/mynode", params);
 * </pre>
 */
iks *ta_pubsub_node_createv (const char *from,
                             const char *to,
                             const char *node,
                             const char **conf_params);

#ifdef __cplusplus
}
#endif

#endif /* _TANINGIA_PUBSUB_H */
