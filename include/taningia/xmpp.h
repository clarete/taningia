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

#ifndef _TANINGIA_XMPP_H_
#define _TANINGIA_XMPP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <iksemel.h>
#include <taningia/taningia.h>
#include <taningia/log.h>
#include <taningia/error.h>

typedef enum {
  XMPP_CONNECTION_ERROR,
  XMPP_NO_SUCH_EVENT_ERROR,
  XMPP_SEND_ERROR
} ta_xmpp_error_t;

typedef struct _ta_xmpp_client_t ta_xmpp_client_t;

typedef int (*ta_xmpp_client_hook_t) (ta_xmpp_client_t *, void *, void *);

typedef void (*ta_xmpp_client_answer_cb_t) (ta_xmpp_client_t *, iks *, void *);

/**
 * @name: ta_xmpp_client_new
 * @type: constructor ta_xmpp_client
 * @param jid: XMPP jid to be used in the client connection.
 * @param password: Password of the jid.
 * @param host (optional): Host to connect to.
 * @param port (optional): XMPP host port to connect to.
 *
 * Creates a new instance of the xmpp client. If host is ommited, it
 * will be extracted from the jid. If port is ommited, the default
 * XMPP port (5222) will be used.
 */
ta_xmpp_client_t *ta_xmpp_client_new (const char *jid, const char *password,
                                      const char *host, int port);

/**
 * @name: ta_xmpp_client_free
 * @type: destructor ta_xmpp_client
 */
void ta_xmpp_client_free (ta_xmpp_client_t *ctx);

/**
 * @name: ta_xmpp_client_get_jid
 * @type: getter ta_xmpp_client:jid
 */
const char *ta_xmpp_client_get_jid (ta_xmpp_client_t *ctx);

/**
 * @name: ta_xmpp_client_set_jid
 * @type: setter ta_xmpp_client:jid
 */
void ta_xmpp_client_set_jid (ta_xmpp_client_t *ctx, const char *jid);

/**
 * @name: ta_xmpp_client_get_password
 * @type: getter ta_xmpp_client:password
 */
const char *ta_xmpp_client_get_password (ta_xmpp_client_t *ctx);

/**
 * @name: ta_xmpp_client_set_password
 * @type: setter ta_xmpp_client:password
 */
void ta_xmpp_client_set_password (ta_xmpp_client_t *ctx, const char *password);

/**
 * @name: ta_xmpp_client_get_host
 * @type: getter ta_xmpp_client:host
 */
const char *ta_xmpp_client_get_host (ta_xmpp_client_t *ctx);

/**
 * @name: ta_xmpp_client_set_host
 * @type: setter ta_xmpp_client:host
 */
void ta_xmpp_client_set_host (ta_xmpp_client_t *ctx, const char *host);

/**
 * @name: ta_xmpp_client_get_port
 * @type: getter ta_xmpp_client:port
 */
int ta_xmpp_client_get_port (ta_xmpp_client_t *ctx);

/**
 * @name: ta_xmpp_client_set_port
 * @type: setter ta_xmpp_client:port
 */
void ta_xmpp_client_set_port (ta_xmpp_client_t *ctx, int port);

/**
 * @name: ta_xmpp_client_get_logger
 * @type: getter ta_xmpp_client:log
 */
ta_log_t *ta_xmpp_client_get_logger (ta_xmpp_client_t *ctx);

/**
 * @name: ta_xmpp_client_get_error
 * @type: getter ta_xmpp_client:error
 */
ta_error_t *ta_xmpp_client_get_error (ta_xmpp_client_t *ctx);

/**
 * @name: ta_xmpp_client_get_filter
 * @type: getter ta_xmpp_client:filter
 */
iksfilter *ta_xmpp_client_get_filter (ta_xmpp_client_t *client);

/**
 * @name: ta_xmpp_client_connect
 * @type: method ta_xmpp_client
 * @raise: XMPP_CONNECTION_ERROR
 *
 * Connects the client to the host and port specified in the
 * constructor.
 */
int ta_xmpp_client_connect (ta_xmpp_client_t *client);

/**
 * @name: ta_xmpp_client_disconnect
 * @type: method ta_xmpp_client
 *
 * Disconnects the client from the host.
 */
void ta_xmpp_client_disconnect (ta_xmpp_client_t *ctx);

/**
 * @name: ta_xmpp_client_send
 * @type: method ta_xmpp_client
 * @param node: The iks node to be sent to the XMPP server.
 *
 * Sends iks nodes to the XMPP server. Only call this function after
 * making sure that client is running properly. To do it, use the
 * `ta_xmpp_client_is_running' function.
 */
int ta_xmpp_client_send (ta_xmpp_client_t *ctx, iks *node);

/**
 * @name: ta_xmpp_client_send_and_filter
 * @type: method ta_xmpp_client
 * @param node: The iks node to be sent to the XMPP server.
 * @param cb: Callback to be called when the sent message is answered
 * by the server.
 * @param data: Extra field for passing context vars to the custom
 * callback (if none, NULL should be used).
 * @param free_cb: Function to free data. Pass NULL when `data' is
 * NULL too.
 *
 * Sends iks nodes to the XMPP server. Only call this function after
 * making sure that client is running properly. To do it, use the
 * `ta_xmpp_client_is_running' function.
 */
int
ta_xmpp_client_send_and_filter (ta_xmpp_client_t *client, iks *node,
                                ta_xmpp_client_answer_cb_t cb, void *data,
                                ta_free_func_t free_cb);

/**
 * @name: ta_xmpp_client_run
 * @type: method ta_xmpp_client
 * @param detach: Define if client's main loop will run in a separated
 * thread.
 *
 * Starts the client main loop. If `detach' is true, returns
 * -1. Otherwise returns the error code of the package read function.
 */
int ta_xmpp_client_run (ta_xmpp_client_t *ctx, int detach);

/**
 * @name: ta_xmpp_client_is_running
 * @type: method ta_xmpp_client
 * @return: bool
 *
 * Returns the state of the XMPP client, if it is running or not.
 */
int ta_xmpp_client_is_running (ta_xmpp_client_t *ctx);

/**
 * @name: ta_xmpp_client_event_connect
 * @type: method ta_xmpp_client
 * @param event: Name of the event to connected the hook.
 * @param hook (callable): The hook to be
 * connected to the event.
 * @param user_data: User defined value to be passed to the hook.
 * @raise: XMPP_NO_SUCH_EVENT_ERROR
 *
 * Connects a hook to an already defined event in the client.  A
 * common case of this function is while connecting, that you connect
 * a hook to the <em>authenticated</em> event. To do it, you can do
 * some thing like this:
 *
 * <pre>
 * int
 * auth_cb (ta_xmpp_client_t *client, void *user_data)
 * {
 *   printf ("Auth sucessful!");
 *   return 1;
 * }
 *
 * ta_xmpp_client_t *client;
 * client = ta_xmpp_client_new ("lincoln@localhost", "passwd", NULL, 0);
 * ta_xmpp_client_connect (client);
 * ta_xmpp_client_event_connect (client, "authenticated", auth_cb, NULL);
 * ta_xmpp_client_run (client, 1);
 * </pre>
 *
 * This method will <strong>not</strong> modify or free value passed
 * in `user_data' parameter.
 */
int ta_xmpp_client_event_connect (ta_xmpp_client_t *client,
                                  const char *event,
                                  ta_xmpp_client_hook_t hook,
                                  void *user_data);

/**
 * @name: ta_xmpp_client_event_disconnect
 * @type: method ta_xmpp_client
 * @param event: The event name
 * @param hook (callable): Hook to be disconnected from the event. If
 * it is `NULL', all hooks will be deleted.
 * @raise: XMPP_NO_SUCH_EVENT_ERROR
 *
 * Disconnects a previously connected hook from an event.
 *
 * This method will <strong>not</strong> modify or free value passed
 * in `user_data' parameter.
 */
int ta_xmpp_client_event_disconnect (ta_xmpp_client_t *client,
                                     const char *event,
                                     ta_xmpp_client_hook_t hook);

#ifdef __cplusplus
}
#endif

#endif /* _TANINGIA_XMPP_H_ */
