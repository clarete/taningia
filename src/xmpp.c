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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <iksemel.h>

#include <taningia/xmpp.h>
#include <taningia/log.h>

struct _ta_xmpp_client_t {
  char *jid;
  char *password;
  char *host;
  int port;

  iksparser *parser;
  iksid *id;
  iksfilter *filter;

  int features;
  int authenticated;
  int running;

  ta_log_t *log;
  ta_error_t *error;

  iksFilterHook *on_auth_success;
  void *on_auth_success_data;
  iksFilterHook *on_auth_failure;
  void *on_auth_failure_data;
};

/* Prototypes of some local functions */

static int _ta_xmpp_client_hook   (void *data, int type, iks *node);

static int _ta_xmpp_client_do_run (void *user_data);

#ifdef DEBUG

static void
_xmpp_clienta_log_hook (ta_xmpp_client_t *client, const char *data,
                        size_t size, int is_incoming)
{
  if (iks_is_secure (client->parser))
    fprintf (stderr, "[SEC:");
  else
    fprintf (stderr, "[");
  if (is_incoming)
    fprintf (stderr, "RECV] ");
  else
    fprintf (stderr, "SEND] ");
  fprintf (stderr, "%s\n\n", data);
}

#endif

/* Allocate memory and initialize stuff necessary to create a new xmpp
 * Client.
 */
ta_xmpp_client_t *
ta_xmpp_client_new (const char *jid,
                    const char *password,
                    const char *host,
                    int         port)
{
  ta_xmpp_client_t *client;
  client = malloc (sizeof (ta_xmpp_client_t));
  client->jid = strdup (jid);
  client->password = strdup (password);

  /* Control flags */
  client->features = 0;
  client->authenticated = 0;
  client->running = 0;

  /* Iksemel stuff */
  client->parser = iks_stream_new (IKS_NS_CLIENT, client, _ta_xmpp_client_hook);
  client->id = iks_id_new (iks_parser_stack (client->parser), jid);
  client->filter = iks_filter_new ();

  /* Handling optional parameters */
  if (host == NULL)
    client->host = strdup (client->id->server);
  else
    client->host = strdup (host);
  if (port)
    client->port = port;
  else
    client->port = IKS_JABBER_PORT;

  /* taningia stuff */
  client->log = ta_log_new ("xmpp-client");
  client->error = NULL;

#ifdef DEBUG
  iks_seta_log_hook (client->parser, (iksLogHook *) _xmpp_clienta_log_hook);
#endif

  return client;
}

/* Free the memory used by a XmppContext struct.
 */
void
ta_xmpp_client_free (ta_xmpp_client_t *client)
{
  if (client->jid)
    free (client->jid);
  if (client->password)
    free (client->password);
  if (client->host)
    free (client->host);
  if (client->parser)
    iks_parser_delete (client->parser);
  if (client->filter)
    iks_filter_delete (client->filter);
  if (client->log)
    ta_log_free (client->log);
  if (client->error)
    ta_error_free (client->error);
  free (client);
}

const char *
ta_xmpp_client_get_jid (ta_xmpp_client_t *client)
{
  return client->jid;
}

void
ta_xmpp_client_set_jid (ta_xmpp_client_t *client, const char *jid)
{
  if (client->jid)
    free (client->jid);
  client->jid = strdup (jid);
}

const char *
ta_xmpp_client_get_password (ta_xmpp_client_t *client)
{
  return client->password;
}

void
ta_xmpp_client_set_password (ta_xmpp_client_t *client, const char *password)
{
  if (client->password)
    free (client->password);
  client->password = strdup (password);
}

const char *
ta_xmpp_client_get_host (ta_xmpp_client_t *client)
{
  return client->host;
}

void
ta_xmpp_client_set_host (ta_xmpp_client_t *client, const char *host)
{
  if (client->host)
    free (client->host);
  client->host = strdup (host);
}

int
ta_xmpp_client_get_port (ta_xmpp_client_t *client)
{
  return client->port;
}

void
ta_xmpp_client_set_port (ta_xmpp_client_t *client, int port)
{
  client->port = port;
}

ta_log_t *
ta_xmpp_client_get_logger (ta_xmpp_client_t *client)
{
  return client->log;
}

ta_error_t *
ta_xmpp_client_get_error (ta_xmpp_client_t *client)
{
  return client->error;
}

iksfilter *
ta_xmpp_client_get_filter (ta_xmpp_client_t *client)
{
  return client->filter;
}

int
ta_xmpp_client_is_running (ta_xmpp_client_t *client)
{
  return client->running;
}

int
ta_xmpp_client_send (ta_xmpp_client_t *client, iks *node)
{
  int err;
  if ((err = iks_send (client->parser, node)) != IKS_OK)
    {
      ta_log_warn (client->log, "Fail to send the stanza");
      if (client->error)
        ta_error_free (client->error);
      client->error = ta_error_new ();
      ta_error_set_name (client->error, "NetworkError");
      ta_error_set_message (client->error, "Failed to send the stanza");
      ta_error_set_code (client->error, XMPP_SEND_ERROR);
    }
  return err;
}

int
ta_xmpp_client_connect (ta_xmpp_client_t *client)
{
  int err;
  if ((err = iks_connect_via (client->parser, client->host,
                              client->port, client->id->server)) != IKS_OK)
    {
      /* Something didnt't work properly here, so we need to handle
       * the error and send some useful result to the user. */
      if (client->error)
        {
          ta_error_free (client->error);
          client->error = NULL;
        }
      client->error = ta_error_new ();
      ta_error_set_name (client->error, "ConnectionError");
      ta_error_set_code (client->error, XMPP_CONNECTION_ERROR);

      switch (err)
        {
        case IKS_NET_NODNS:
          ta_error_set_message (client->error, "hostname lookup failed");
          break;

        case IKS_NET_NOCONN:
          ta_error_set_message (client->error, "connection failed");
          break;

        default:
          ta_error_set_message (client->error, "io error");
          break;
        }
      return 0;
    }
  else
    {
      ta_log_info (client->log, "Connected to xmpp:%s:%d",
                   client->host, client->port);
      return 1;
    }
}

int
ta_xmpp_client_run (ta_xmpp_client_t *client, int detach)
{
  /* Detaching our main loop thread if requested */
  if (detach)
    {
      pthread_t th;
      pthread_create (&th, NULL, (void *) _ta_xmpp_client_do_run, (void *) client);
      pthread_detach (th);
      ta_log_info (client->log, "Detaching xmpp client main loop thread");
      return -1;
    }
  else
    return (_ta_xmpp_client_do_run ((void *) client));
}

void
ta_xmpp_client_disconnect (ta_xmpp_client_t *client)
{
  client->running = 0;
  iks_disconnect (client->parser);
  ta_log_info (client->log, "Disconnected");
}

void
ta_xmpp_client_set_auth_success_cb (ta_xmpp_client_t *client,
                                    iksFilterHook *cb,
                                    void *user_data)
{
  client->on_auth_success = cb;
  client->on_auth_success_data = user_data;
  iks_filter_add_rule (client->filter, (iksFilterHook *) cb, user_data,
                       IKS_RULE_TYPE, IKS_PAK_IQ,
                       IKS_RULE_SUBTYPE, IKS_TYPE_RESULT,
                       IKS_RULE_ID, "auth",
                       IKS_RULE_DONE);
}

void
ta_xmpp_client_set_auth_failure_cb (ta_xmpp_client_t  *client,
                                    iksFilterHook *cb,
                                    void *user_data)
{
  client->on_auth_failure = cb;
  client->on_auth_failure_data = user_data;
  iks_filter_add_rule (client->filter, cb, user_data,
                       IKS_RULE_TYPE, IKS_PAK_IQ,
                       IKS_RULE_SUBTYPE, IKS_TYPE_ERROR,
                       IKS_RULE_ID, "auth",
                       IKS_RULE_DONE);
}

/* ---- End of public API ---- */

static int
_ta_xmpp_client_hook (void *data, int type, iks *node)
{
  ta_xmpp_client_t *client;
  char *name;
  client = (ta_xmpp_client_t *) data;
  name = iks_name (node);

  switch (type)
    {
    case IKS_NODE_START:
      if (!iks_is_secure (client->parser))
        {
          iks_start_tls (client->parser);
          break;
        }
    case IKS_NODE_NORMAL:
      if (strcmp (name, "stream:features") == 0)
        {
          client->features = iks_stream_features (node);
          if (!iks_is_secure (client->parser))
            break;
          if (client->authenticated)
            {
              if (client->features & IKS_STREAM_BIND)
                {
                  iks *bind;
                  bind = iks_make_resource_bind (client->id);
                  iks_send (client->parser, bind);
                  iks_delete (bind);
                }
              if (client->features & IKS_STREAM_SESSION)
                {
                  iks *session;
                  session = iks_make_session ();
                  iks_insert_attrib (session, "id", "auth");
                  iks_send (client->parser, session);
                  iks_delete (session);
                }
            }
          else
            {
              if (client->features & IKS_STREAM_SASL_MD5)
                iks_start_sasl (client->parser, IKS_SASL_DIGEST_MD5,
                                client->id->user, client->password);
              else if (client->features & IKS_STREAM_SASL_PLAIN)
                iks_start_sasl (client->parser, IKS_SASL_PLAIN,
                                client->id->user, client->password);
            }
        }
      else if (strcmp (name, "failure") == 0)
        {
          ta_log_info (client->log, "authentication failed");
          if (client->on_auth_failure)
            {
              ikspak *pak;
              pak = iks_packet (node);
              client->on_auth_failure (client->on_auth_failure_data, pak);
            }
        }
      else if (strcmp (name, "success") == 0)
        {
          client->authenticated = 1;
          iks_send_header (client->parser, client->id->server);
          ta_log_info (client->log, "authentication successful");
        }
      else
        {
          ikspak *pak;
          pak = iks_packet (node);
          iks_filter_packet (client->filter, pak);
        }
      break;
    case IKS_NODE_ERROR:
      {
        char *node_str;
        node_str = iks_string (iks_stack (node), node);
        ta_log_error (client->log, "Error stream: %s", node_str);
        break;
      }
    }

  if (node)
    iks_delete (node);

  return IKS_OK;
}

static int
_ta_xmpp_client_do_run (void *user_data)
{
  ta_xmpp_client_t *client;
  int ret = 1;

  client = (ta_xmpp_client_t *) user_data;
  client->running = 1;

  while (client->running)
    {
      switch (iks_recv (client->parser, -1))
        {
        case IKS_HOOK:
        case IKS_OK:
          break;

        case IKS_NET_NOCONN:
          ta_log_info (client->log, "Client not connected, stopping main loop");
          client->running = 0;
          break;

        case IKS_NET_RWERR:
          ta_log_error (client->log, "Network error");
          client->running = 0;
          break;

        case IKS_NET_TLSFAIL:
          ta_log_error (client->log, "TLS handshake failed");
          client->running = 0;
          break;

        default:
          ta_log_error (client->log, "IO error");
          client->running = 0;
          break;
        }
    }
  return ret;
}
