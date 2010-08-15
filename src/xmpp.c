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

#include <taningia/mem.h>
#include <taningia/xmpp.h>
#include <taningia/log.h>
#include <taningia/list.h>

#include "hashtable.h"
#include "hashtable-utils.h"

struct _ta_xmpp_client_t {
  ta_object_t parent;
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

  hashtable_t *events;
};

struct hook_data {
  ta_xmpp_client_hook_t hook;
  void *data;
  ta_free_func_t free_data_func;
};

struct watch_data {
  char *stanza_id;
  iksrule *rule;
  ta_xmpp_client_t *client;
  ta_xmpp_client_answer_cb_t callback;
  void *data;
  ta_free_func_t free_data_func;
};

/* Prototypes of some local functions */

static int _ta_xmpp_client_hook   (void *data, int type, iks *node);

static int _ta_xmpp_client_do_run (void *user_data);

/* hook_data helpers */

static struct hook_data *
hdata_new (ta_xmpp_client_hook_t hook, void *data, ta_free_func_t free_data)
{
  struct hook_data *hdata;
  hdata = malloc (sizeof (struct hook_data));
  hdata->hook = hook;
  hdata->data = data;
  hdata->free_data_func = free_data;
  return hdata;
}

static void
hdata_free (void *val)
{
  struct hook_data *hdata = (struct hook_data *) val;
  if (hdata->free_data_func)
    hdata->free_data_func (hdata->data);
  free (hdata);
}

/* watch_data helpers */

static struct watch_data *
wdata_new (const char *stanza_id, ta_xmpp_client_t *client,
           ta_xmpp_client_answer_cb_t cb, void *data,
           ta_free_func_t free_data)
{
  struct watch_data *wdata;
  wdata = malloc (sizeof (struct watch_data));
  wdata->stanza_id = strdup (stanza_id);
  wdata->client = client;
  wdata->callback = cb;
  wdata->data = data;
  wdata->free_data_func = free_data;
  wdata->rule = NULL;           /* Must be filled by hand. */
  return wdata;
}

static void
wdata_free (void *val)
{
  struct watch_data *wdata = (struct watch_data *) val;
  if (wdata->free_data_func && wdata->data)
    wdata->free_data_func (wdata->data);
  if (wdata->stanza_id)
    free (wdata->stanza_id);
  free (wdata);
}

/* Look for an entry called `event' in the client event hash table and
 * then executes all hooks added to the list in that entry. If a hook
 * returns a true value, the iteration is stopped. */
static void
_ta_xmpp_client_call_event_hooks (ta_xmpp_client_t *client,
                                  const char *event, void *data)
{
  ta_list_t *tmp;
  ta_list_t *hooks = hashtable_get (client->events, event);
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

/* Callback fired when a <message /> is received by the xmpp
 * client. */
static int
_ta_xmpp_client_ikshook_message_received (void *data, ikspak *pak)
{
  ta_xmpp_client_t *client;
  client = (ta_xmpp_client_t *) data;
  _ta_xmpp_client_call_event_hooks (client, "message-received", pak);
  return IKS_FILTER_EAT;
}

/* Callback fired when a <presence /> is received by the xmpp
 * client. */
static int
_ta_xmpp_client_ikshook_presence_noticed (void *data, ikspak *pak)
{
  ta_xmpp_client_t *client;
  client = (ta_xmpp_client_t *) data;
  _ta_xmpp_client_call_event_hooks (client, "presence-noticed", pak);
  return IKS_FILTER_EAT;
}

/* This callback is installed by `ta_xmpp_client_send_and_filter()'
 * function. It is a wrapper to the user defined callback. It is
 * registered in the `client->filter' property and is called when an
 * instance with the same id as the sent one is answered.
 *
 * After called, it frees callback data, stanza id and removes itself
 * from `client->filter' property. */
static int
_ta_xmpp_client_ikshook_watcher (void *data, ikspak *pak)
{
  struct watch_data *wdata = (struct watch_data *) data;

  /* Calling the user defined callback. */
  wdata->callback (wdata->client, pak->x, wdata->data);

  /* Removing this callback from the client filter and then freeing
   * allocated data. */
  if (wdata->client->filter && wdata->rule != NULL)
    iks_filter_remove_rule (wdata->client->filter, wdata->rule);
  wdata_free (wdata);
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

static void
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
    ta_object_unref (client->log);
  if (client->error)
    ta_object_unref (client->error);

  /* Freeing all hooks for all events. Maybe it is better to be done
   * automatically. */
  ta_xmpp_client_event_disconnect (client, "connected", NULL);
  ta_xmpp_client_event_disconnect (client, "authenticated", NULL);
  ta_xmpp_client_event_disconnect (client, "authentication-failed", NULL);
  ta_xmpp_client_event_disconnect (client, "message-received", NULL);
  ta_xmpp_client_event_disconnect (client, "presence-noticed", NULL);
  if (client->events)
    hashtable_destroy (client->events);
}

void
ta_xmpp_client_init (ta_xmpp_client_t *client,
                     const char *jid,
                     const char *password,
                     const char *host,
                     int port)
{
  int jid_len;
  ta_object_init (TA_CAST_OBJECT (client),
                  (ta_free_func_t) ta_xmpp_client_free);
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
    client->host = NULL;
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
  client->events = hashtable_create (hash_string, string_equal, NULL, NULL);
  hashtable_set (client->events, "connected", NULL);
  hashtable_set (client->events, "authenticated", NULL);
  hashtable_set (client->events, "authentication-failed", NULL);
  hashtable_set (client->events, "message-received", NULL);
  hashtable_set (client->events, "presence-noticed", NULL);
}

ta_xmpp_client_t *
ta_xmpp_client_new (const char *jid,
                    const char *password,
                    const char *host,
                    int         port)
{
  ta_xmpp_client_t *client;
  client = malloc (sizeof (ta_xmpp_client_t));
  ta_xmpp_client_init (client, jid, password, host, port);
  return client;
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
        ta_object_unref (client->error);
      client->error = ta_error_new ();
      ta_error_set_name (client->error, "NetworkError");
      ta_error_set_message (client->error, "Failed to send the stanza");
      ta_error_set_code (client->error, XMPP_SEND_ERROR);
    }
  return err;
}

int
ta_xmpp_client_send_and_filter (ta_xmpp_client_t *client, iks *node,
                                ta_xmpp_client_answer_cb_t cb, void *data,
                                ta_free_func_t free_cb)
{
  int err;
  char *id;
  iksrule *rule;
  struct watch_data *wdata;

  /* Getting stanza id */
  if ((id = iks_find_attrib (node, "id")) == NULL)
    return 1;

  /* Now that all search fields were filled, it is time to build the
   * struct that will hold data received from params and found here
   * and pass it to `_ta_xmpp_client_ikshook_watcher()' function. */
  wdata = wdata_new (id, client, cb, data, free_cb);

  /* Registering rule in the client filter. This assignment seems
   * confusing because `wdata' is in the `iks_filter_add_rule()
   * parameter list. But the `rule' member of `wdata' will only be
   * used after the `iks_send()' call. There is *NO* race condition
   * here.
   *
   * This not-actually-pretty code is here because iksemel filter/rule
   * API sucks. The `iksrule' struct should have constructor,
   * destructor and methods to manipulate it public, which is not the
   * case.
   */
  rule =
    iks_filter_add_rule (client->filter,
                         (iksFilterHook *) _ta_xmpp_client_ikshook_watcher,
                         wdata, IKS_RULE_ID, id, IKS_RULE_DONE);
  wdata->rule = rule;
  wdata->stanza_id = strdup ("Meu pau");

  /* Finnaly, we're trying to send the stanza. With the filter
   * properly registered. */
  if ((err = iks_send (client->parser, node)) != IKS_OK)
    {
      ta_log_warn (client->log, "Fail to send the stanza");
      if (client->error)
        ta_object_unref (client->error);
      client->error = ta_error_new ();
      ta_error_set_name (client->error, "NetworkError");
      ta_error_set_message (client->error, "Failed to send the stanza");
      ta_error_set_code (client->error, XMPP_SEND_ERROR);

      iks_filter_remove_rule (client->filter, wdata->rule);
      wdata_free (wdata);
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

  if (client->host)
    err = iks_connect_via (client->parser, client->host,
                           client->port, client->id->server);
  else
    err = iks_connect_tcp (client->parser, client->id->server, client->port);
  if (err != IKS_OK)
    {
      /* Something didnt't work properly here, so we need to handle
       * the error and send some useful result to the user. */
      if (client->error)
        {
          ta_object_unref (client->error);
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
                   client->host == NULL ? client->id->server : client->host,
                   client->port);

      /* Calling user defined hooks for the `connected' event. */
      _ta_xmpp_client_call_event_hooks (client, "connected", NULL);

      /* Adding authentication handling rules to iksemel filter. This
       * stuff will be integrated with our simple event system. The
       * delcared callbacks only calls the user defined hook list. */

      iks_filter_add_rule (client->filter,
                           (iksFilterHook *)
                             _ta_xmpp_client_ikshook_authenticated,
                           client,
                           IKS_RULE_TYPE, IKS_PAK_IQ,
                           IKS_RULE_SUBTYPE, IKS_TYPE_RESULT,
                           IKS_RULE_ID, "auth",
                           IKS_RULE_DONE);

      iks_filter_add_rule (client->filter,
                           (iksFilterHook *)
                             _ta_xmpp_client_ikshook_message_received,
                           client,
                           IKS_RULE_TYPE, IKS_PAK_MESSAGE,
                           IKS_RULE_DONE);

      iks_filter_add_rule (client->filter,
                           (iksFilterHook *)
                             _ta_xmpp_client_ikshook_presence_noticed,
                           client,
                           IKS_RULE_TYPE, IKS_PAK_PRESENCE,
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
  ta_list_t *hooks = NULL;
  struct hook_data *hdata;
  hdata = hdata_new (hook, user_data, NULL);

  /* Looking for an already created list. If already exists, we just
   * append a new value. Otherwise, we create it and re-add the entry
   * to the hash table. */
  if (!hashtable_get_test (client->events, event, (void **) &hooks))
    {
      if (client->error)
        ta_object_unref (client->error);
      client->error = ta_error_new ();
      ta_error_set_name (client->error, "NoSuchEvent");
      ta_error_set_message (client->error,
                            "XMPP client has no event called %s", event);
      ta_error_set_code (client->error, XMPP_NO_SUCH_EVENT_ERROR);
      return 0;
    }
  if (hooks == NULL)
    {
      hooks = ta_list_append (hooks, hdata);
      hashtable_set (client->events, (void *) event, hooks);
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
  ta_list_t *hooks = NULL, *tmp = NULL, *elm = NULL;

  /* Looks for the hook list in event hash table. If it is found, the
   * hook is removed from the list and then the hook list is
   * re-inserted in the hash table. This only works because the
   * _insert function of the hash table replaces entries with same
   * keys. */
  if (!hashtable_get_test (client->events, event, (void **) &hooks))
    {
      if (client->error)
        ta_object_unref (client->error);
      client->error = ta_error_new ();
      ta_error_set_name (client->error, "NoSuchEvent");
      ta_error_set_message (client->error,
                            "XMPP client has no event called %s", event);
      ta_error_set_code (client->error, XMPP_NO_SUCH_EVENT_ERROR);
      return 0;
    }

  /* Nothing will work if hooks is NULL. */
  tmp = hooks;
  while (tmp)
    {
      /* The `tmp' element should be preserved because of the loop,
       * let's cache its current value in the `elm' var and continue
       * to iterate over the list. */
      elm = tmp;
      tmp = tmp->next;

       /* If hook is null, all events are going to match this and will
        * be deleted. */
      if (hook != NULL && ((struct hook_data *) elm->data)->hook != hook)
        continue;

      /* We cannot use ta_list_remove function directly because we
       * receive the hook reference that is just a member of the
       * hook_data struct stored in the list iter. So, before removing
       * we have to find the right element. It is done by the above
       * `if' statement. */
      hooks = ta_list_remove (hooks, elm->data, hdata_free);
    }

  /* Updating callback list in the event hashtable. */
  hashtable_set (client->events, (void *) event, hooks);
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
          ta_log_info (client->log,
                       "Client not connected, stopping main loop");
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
