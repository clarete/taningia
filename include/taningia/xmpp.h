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

#ifndef _TANINGIA_XMPP_H_
#define _TANINGIA_XMPP_H_

#include <iksemel.h>
#include <taningia/taningia.h>
#include <taningia/log.h>
#include <taningia/error.h>

typedef enum
{
  XMPP_CONNECTION_ERROR,
  XMPP_SEND_ERROR
} xmpp_error_t;

typedef struct _xmpp_client_t xmpp_client_t;

/**
 * @name: xmpp_client_new
 * @type: constructor xmpp_client
 * @param jid: XMPP jid to be used in the client connection.
 * @param password: Password of the jid.
 * @param host (optional): Host to connect to.
 * @param port (optional): XMPP host port to connect to.
 *
 * Creates a new instance of the xmpp client. If host is ommited, it
 * will be extracted from the jid. If port is ommited, the default
 * XMPP port (5222) will be used.
 */
xmpp_client_t *xmpp_client_new (const char *jid, const char *password,
                                const char *host, int port);

/**
 * @name: xmpp_client_free
 * @type: destructor xmpp_client
 */
void xmpp_client_free (xmpp_client_t *ctx);

/**
 * @name: xmpp_client_get_jid
 * @type: getter xmpp_client:jid
 */
const char *xmpp_client_get_jid (xmpp_client_t *ctx);

/**
 * @name: xmpp_client_set_jid
 * @type: setter xmpp_client:jid
 */
void xmpp_client_set_jid (xmpp_client_t *ctx, const char *jid);

/**
 * @name: xmpp_client_get_password
 * @type: getter xmpp_client:password
 */
const char *xmpp_client_get_password (xmpp_client_t *ctx);

/**
 * @name: xmpp_client_set_password
 * @type: setter xmpp_client:password
 */
void xmpp_client_set_password (xmpp_client_t *ctx, const char *password);

/**
 * @name: xmpp_client_get_host
 * @type: getter xmpp_client:host
 */
const char *xmpp_client_get_host (xmpp_client_t *ctx);

/**
 * @name: xmpp_client_set_host
 * @type: setter xmpp_client:host
 */
void xmpp_client_set_host (xmpp_client_t *ctx,
                           const char *host);

/**
 * @name: xmpp_client_get_port
 * @type: getter xmpp_client:port
 */
int xmpp_client_get_port (xmpp_client_t *ctx);

/**
 * @name: xmpp_client_set_port
 * @type: setter xmpp_client:port
 */
void xmpp_client_set_port (xmpp_client_t *ctx, int port);

/**
 * @name: xmpp_client_get_logger
 * @type: getter xmpp_client:log
 */
log_t *xmpp_client_get_logger (xmpp_client_t *ctx);

/**
 * @name: xmpp_client_get_error
 * @type: getter xmpp_client:error
 */
error_t *xmpp_client_get_error (xmpp_client_t *ctx);

/**
 * @name: xmpp_client_get_filter
 * @type: getter xmpp_client:filter
 */
iksfilter *xmpp_client_get_filter (xmpp_client_t *client);

/**
 * @name: xmpp_client_connect
 * @type: method xmpp_client
 * @raise: XMPP_CONNECTION_ERROR
 *
 * Connects the client to the host and port specified in the
 * constructor.
 */
int xmpp_client_connect (xmpp_client_t *client);

/**
 * @name: xmpp_client_disconnect
 * @type: method xmpp_client
 *
 * Disconnects the client from the host.
 */
void xmpp_client_disconnect (xmpp_client_t *ctx);

/**
 * @name: xmpp_client_send
 * @type: method xmpp_client
 * @param node: The iks node to be sent to the XMPP server.
 *
 * Sends iks nodes to the XMPP server. Only call this function after
 * making sure that client is running properly. To do it, use the
 * `xmpp_client_is_running' function.
 */
int xmpp_client_send (xmpp_client_t *ctx, iks *node);

/**
 * @name: xmpp_client_run
 * @type: method xmpp_client
 * @param detach: Define if client's main loop will run in a separated
 * thread.
 *
 * Starts the client main loop. If `detach' is true, returns
 * -1. Otherwise returns the error code of the package read function.
 */
int xmpp_client_run (xmpp_client_t *ctx, int detach);

/**
 * @name: xmpp_client_is_running
 * @type: method xmpp_client
 * @return: bool
 *
 * Returns the state of the XMPP client, if it is running or not.
 */
int xmpp_client_is_running (xmpp_client_t *ctx);

/* Filter callbacks */

void xmpp_client_set_auth_success_cb (xmpp_client_t *client,
                                      iksFilterHook *cb,
                                      void *user_data);

void xmpp_client_set_auth_failure_cb (xmpp_client_t *client,
                                      iksFilterHook *cb,
                                      void *user_data);

#endif /* _TANINGIA_XMPP_CLIENT_H_ */
