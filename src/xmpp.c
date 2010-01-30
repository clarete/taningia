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
#include <taningia/list.h>

/* To use GHashTable. Soon it will be replaced. */
#include <glib.h>

struct _ta_xmpp_client_t {
  char *jid;
  char *password;
  char *host;
  int port;

  iksparser *parser;
  ikstack *idstack;
  iksid *id;
  iksfilter *filter;

  int features;
  int authenticated;
  int running;

  ta_log_t *log;
  ta_error_t *error;

  GHashTable *events;
};

struct hook_data {
  ta_xmpp_client_hook_t hook;
  void *data;
};

/* Prototypes of some local functions */

static int _ta_xmpp_client_hook   (void *data, int type, iks *node);

static int _ta_xmpp_client_do_run (void *user_data);

/* Look for an entry called `event' in the client event hash table and
 * then executes all hooks added to the list in that entry. If a hook
 * returns a true value, the iteration is stopped. */
static void
_ta_xmpp_client_call_event_hooks (ta_xmpp_client_t *client,
                                  const char *event, void *data)
{
  ta_list_t *tmp;
  ta_list_t *hooks = g_hash_table_lookup (client->events, event);
  struct hook_data *hdata;
  for (tmp = hooks; tmp; tmp = tmp->next)
    {
      hdata = (struct hook_data *) tmp->data;
      if ((*hdata->hook) (client, data, hdata->data))
        return;
    }
}

/* Definition of the callback that is called when our client
 * successfully authenticates in an XMPP server.  This callback just
 * call any declared hook against `authenticated' event. */
static int
_ta_xmpp_client_ikshook_authenticated (void *data, ikspak *pak)
{
  ta_xmpp_client_t *client;
  client = (ta_xmpp_client_t *) data;
  _ta_xmpp_client_call_event_hooks (client, "authenticated", pak);
  return IKS_FILTER_EAT;
}

static int
_ta_xmpp_client_ikshook_message_received (void *data, ikspak *pak)
{
  ta_xmpp_client_t *client;
  client = (ta_xmpp_client_t *) data;
  _ta_xmpp_client_call_event_hooks (client, "message-received", pak);
  return IKS_FILTER_EAT;
}

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
  int jid_len;
  client = malloc (sizeof (ta_xmpp_client_t));
  client->jid = strdup (jid);
  client->password = strdup (password);
  jid_len = strlen (jid);

  /* Control flags */
  client->features = 0;
  client->authenticated = 0;
  client->running = 0;

  /* iksemel stuff */
  client->parser = NULL;
  client->filter = NULL;
  client->idstack = iks_stack_new (jid_len, jid_len);
  client->id = iks_id_new (client->idstack, client->jid);

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

  /* Initializing hash table that holds event hooks and adding all
   * currently supported events. We actually don't free anything but
   * the `hook_data' struct, so it is up to the caller to free the
   * data field. */
  client->events = g_hash_table_new_full (g_str_hash, g_str_equal,
                                          NULL, g_free);
  g_hash_table_insert (client->events, "connected", NULL);
  g_hash_table_insert (client->events, "authenticated", NULL);
  g_hash_table_insert (client->events, "authentication-failed", NULL);
  g_hash_table_insert (client->events, "message-received", NULL);
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
  if (client->idstack)
    iks_stack_delete (client->idstack);
  if (client->parser)
    iks_parser_delete (client->parser);
  if (client->filter)
    iks_filter_delete (client->filter);
  if (client->log)
    ta_log_free (client->log);
  if (client->error)
    ta_error_free (client->error);

  /* Freeing all hooks for all events. Maybe it is better to be done
   * automatically. */
  ta_xmpp_client_event_disconnect_all (client, "connected");
  ta_xmpp_client_event_disconnect_all (client, "authenticated");
  ta_xmpp_client_event_disconnect_all (client, "authentication-failed");
  ta_xmpp_client_event_disconnect_all (client, "message-received");
  if (client->events)
    g_hash_table_unref (client->events);
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
  client->id = iks_id_new (client->idstack, jid);
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

  /* Iksemel stuff */
  client->parser = iks_stream_new (IKS_NS_CLIENT, client,
                                   _ta_xmpp_client_hook);
  client->filter = iks_filter_new ();

#ifdef DEBUG
  iks_set_log_hook (client->parser, (iksLogHook *) _xmpp_clienta_log_hook);
#endif

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

      /* Calling user defined hooks for the `connected' event. */
      _ta_xmpp_client_call_event_hooks (client, "connected", NULL);

      /* Adding authentication handling rules to iksemel filter. This
       * stuff will be integrated with our simple event system. The
       * delcared callbacks only calls the user defined hook list. */

      iks_filter_add_rule (client->filter,
                           (iksFilterHook *) _ta_xmpp_client_ikshook_authenticated,
                           client,
                           IKS_RULE_TYPE, IKS_PAK_IQ,
                           IKS_RULE_SUBTYPE, IKS_TYPE_RESULT,
                           IKS_RULE_ID, "auth",
                           IKS_RULE_DONE);

      iks_filter_add_rule (client->filter,
                           (iksFilterHook *) _ta_xmpp_client_ikshook_message_received,
                           client,
                           IKS_RULE_TYPE, IKS_PAK_MESSAGE,
                           IKS_RULE_DONE);
      return 1;
    }
}

int
ta_xmpp_client_run (ta_xmpp_client_t *client, int detach)
{
  client->running = 1;

  /* Detaching our main loop thread if requested */
  if (detach)
    {
      pthread_t th;
      pthread_create (&th, NULL, (void *) _ta_xmpp_client_do_run,
                      (void *) client);
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

  /* These fields are going to be filled again by the connect
   * method if called again. */
  if (client->parser)
    {
      iks_parser_delete (client->parser);
      client->parser = NULL;
    }
  if (client->filter)
    {
      iks_filter_delete (client->filter);
      client->filter = NULL;
    }
  ta_log_info (client->log, "Disconnected");
}

int
ta_xmpp_client_event_connect (ta_xmpp_client_t *client,
                              const char *event,
                              ta_xmpp_client_hook_t hook,
                              void *user_data)
{
  ta_list_t *hooks;
  struct hook_data *hdata;
  hdata = malloc (sizeof (struct hook_data));
  hdata->hook = hook;
  hdata->data = user_data;

  /* Looking for an already created list. If already exists, we just
   * append a new value. Otherwise, we create it and re-add the entry
   * to the hash table. */
  if (!g_hash_table_lookup_extended (client->events, event, NULL,
                                     (gpointer) &hooks))
    {
      if (client->error)
        ta_error_free (client->error);
      client->error = ta_error_new ();
      ta_error_set_name (client->error, "NoSuchEvent");
      ta_error_set_message (client->error, "XMPP client has no event called %s",
                            event);
      ta_error_set_code (client->error, XMPP_NO_SUCH_EVENT_ERROR);
      return 0;
    }
  if (hooks == NULL)
    {
      hooks = ta_list_append (hooks, hdata);
      g_hash_table_insert (client->events, (gpointer) event, hooks);
    }
  else
    hooks = ta_list_append (hooks, hdata);
  return 1;
}

int
ta_xmpp_client_event_disconnect (ta_xmpp_client_t *client,
                                 const char *event,
                                 ta_xmpp_client_hook_t hook)
{
  ta_list_t *hooks;

  /* Looks for the hook list in event hash table. If it is found, the
   * hook is removed from the list and then the hook list is
   * re-inserted in the hash table. This only works because the
   * _insert function of the hash table replaces entries with same
   * keys. */
  if (!g_hash_table_lookup_extended (client->events, event, NULL,
                                     (gpointer) &hooks))
    {
      if (client->error)
        ta_error_free (client->error);
      client->error = ta_error_new ();
      ta_error_set_name (client->error, "NoSuchEvent");
      ta_error_set_message (client->error, "XMPP client has no event called %s",
                            event);
      ta_error_set_code (client->error, XMPP_NO_SUCH_EVENT_ERROR);
      return 0;
    }
  if (hooks != NULL)
    {
      hooks = ta_list_remove (hooks, hook);
      free (hook);
      g_hash_table_insert (client->events, (gpointer) event, hooks);
    }
  return 1;
}

int
ta_xmpp_client_event_disconnect_all (ta_xmpp_client_t *client,
                                     const char *event)
{
  ta_list_t *hooks;
  if (!g_hash_table_lookup_extended (client->events, event, NULL,
                                     (gpointer) &hooks))
    {
      if (client->error)
        ta_error_free (client->error);
      client->error = ta_error_new ();
      ta_error_set_name (client->error, "NoSuchEvent");
      ta_error_set_message (client->error, "XMPP client has no event called %s",
                            event);
      ta_error_set_code (client->error, XMPP_NO_SUCH_EVENT_ERROR);
      return 0;
    }
  if (hooks != NULL)
    {
      ta_list_t *node;
      for (node = hooks; node; node = node->next)
        if (node->data)
          free (node->data);
    }
  return 1;
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
          ikspak *pak;
          ta_log_info (client->log, "authentication failed");

          /* Calling hooks for `authentication-failed' event. */
          pak = iks_packet (node);
          _ta_xmpp_client_call_event_hooks (client, "authentication-failed",
                                            pak);
        }
      else if (strcmp (name, "success") == 0)
        {
          client->authenticated = 1;
          iks_send_header (client->parser, client->id->server);
          ta_log_info (client->log, "authentication successful");
        }
      else if (client->filter != NULL)
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
  ta_log_info (client->log, "Main loop thread dying");
  return ret;
}
