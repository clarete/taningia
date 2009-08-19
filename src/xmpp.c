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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <iksemel.h>

#include <jarvis/xmpp.h>
#include <jarvis/filter.h>
#include <jarvis/log.h>

struct _JXmpp {
  iksparser *parser;
  iksid *id;
  char *jid;
  char *password;
  char *host;
  int port;
  JFilter *events;
  JFilter *ids;
  JLog *log;
  int running;
};

/* Prototypes of some local functions */

static int      _j_xmpp_hook       (void *data, int type, iks *node);

static void    *_j_xmpp_do_run     (void *user_data);

static void
_j_xmpp_log_hook (void *user_data, const char *data, size_t size, int is_incoming)
{
  /* fprintf (stderr, "%s %s\n", is_incoming ? "[in]" : "[out]", data); */
}

/**
 * Allocate memory and initialize stuff necessary to create a new
 * XmppContext.
 */
JXmpp *
j_xmpp_new (const char *jid,
            const char *password,
            const char *host,
            int         port)
{
  JXmpp *ctx;
  ctx = malloc (sizeof (JXmpp));
  ctx->jid = strdup (jid);
  ctx->password = strdup (password);
  ctx->parser = iks_stream_new (IKS_NS_CLIENT, ctx, _j_xmpp_hook);
  ctx->id = iks_id_new (iks_parser_stack (ctx->parser), ctx->jid);
  ctx->events = j_filter_new (ctx);
  ctx->ids = j_filter_new (ctx);
  ctx->log = j_log_new ("xmpp-client");
  ctx->running = 0;

  /* Handling optional arguments */

  if (host == NULL)
    ctx->host = strdup (ctx->id->server);
  else
    ctx->host = strdup (host);

  if (port)
    ctx->port = port;
  else
    ctx->port = IKS_JABBER_PORT;

  /* Useful for debugging */

  iks_set_log_hook (ctx->parser, _j_xmpp_log_hook);
  return ctx;
}

/**
 * Free the memory used by a XmppContext struct.
 */
void
j_xmpp_free (JXmpp *ctx)
{
  free (ctx->jid);
  free (ctx->password);
  iks_parser_delete (ctx->parser);
  j_filter_free (ctx->events);
  j_filter_free (ctx->ids);
  j_log_free (ctx->log);
  free (ctx);
}

const char *
j_xmpp_get_jid (JXmpp *ctx)
{
  return ctx->jid;
}

void
j_xmpp_set_jid (JXmpp *ctx, const char *jid)
{
  if (ctx->jid)
    free (ctx->jid);
  ctx->jid = strdup (jid);
}

const char *
j_xmpp_get_password (JXmpp *ctx)
{
  return ctx->password;
}

void
j_xmpp_set_password (JXmpp *ctx, const char *password)
{
  if (ctx->password)
    free (ctx->password);
  ctx->password = strdup (password);
}

const char *
j_xmpp_get_host (JXmpp *ctx)
{
  return ctx->host;
}

void
j_xmpp_set_host (JXmpp *ctx, const char *host)
{
  if (ctx->host)
    free (ctx->host);
  ctx->host = strdup (host);
}

int
j_xmpp_get_port (JXmpp *ctx)
{
  return ctx->port;
}

void
j_xmpp_set_port (JXmpp *ctx, int port)
{
  ctx->port = port;
}

JFilter *
j_xmpp_get_filter_events (JXmpp *ctx)
{
  return ctx->events;
}

JFilter *
j_xmpp_get_filter_ids (JXmpp *ctx)
{
  return ctx->ids;
}

JLog *
j_xmpp_get_logger (JXmpp *ctx)
{
  return ctx->log;
}

int
j_xmpp_is_running (JXmpp *ctx)
{
  return ctx->running;
}

int
j_xmpp_send (JXmpp *ctx, iks *node)
{
  int err;
  if ((err = iks_send (ctx->parser, node)) != IKS_OK)
    j_log_warn (ctx->log, "Fail to send packet through j_xmpp_send");
  return err;
}

/**
 * This function starts the main xmpp (iksemel) loop in another
 * thread. To avoid problems with our loved GMainLoop used by soup.
 */
int
j_xmpp_run (JXmpp *ctx)
{
  int err;
  pthread_t th;

  if ((err = iks_connect_via (ctx->parser, ctx->host,
                              ctx->port, ctx->id->server)) != IKS_OK)
    {
      /* Something didnt't work properly here, so we need to handle
       * the error and send some useful result to the user. */
      return err;
    }
  j_log_info (ctx->log, "Connected to xmpp:%s:%d", ctx->host, ctx->port);

  ctx->running = 1;

  /* Detaching our main loop thread */
  pthread_create (&th, NULL, _j_xmpp_do_run, (void *) ctx);
  pthread_detach (th);
  j_log_info (ctx->log, "Detaching the main loop thread");

  return IKS_OK;
}

void
j_xmpp_stop (JXmpp *ctx)
{
  iks_disconnect (ctx->parser);
  ctx->running = 0;
  j_log_info (ctx->log, "Disconnected");
}

int
j_xmpp_reconnect (JXmpp *ctx)
{
  j_xmpp_stop (ctx);
  return j_xmpp_run (ctx);
}

/* Static (local) functions. A very special thanks to Thadeu
 * Cascardo, his pubsub-bot was a very useful example =D */

int
xmpp_bind_hook (iksparser *parser, iks *node)
{
  iks *iq;
  iq = iks_new ("iq");
  iks_insert_attrib (iq, "type", "set");
  iks_insert_attrib (iq, "id", "bind1");
  iks_insert_attrib (iks_insert (iq, "bind"),
                     "xmlns", "urn:ietf:params:xml:ns:xmpp-bind");
  iks_send (parser, iq);
  iks_delete (iq);
  return 0;
}

int
xmpp_iq_error (iksparser *parser, iks *node)
{
  iks *enode;
  char *to;
  char *from;
  if (!iks_strcmp (iks_find_attrib (node, "type"), "error"))
    return 1;
  to = iks_find_attrib (node, "to");
  from = iks_find_attrib (node, "from");
  if (to)
    iks_insert_attrib (node, "from", to);
  else
    iks_insert_attrib (node, "from", NULL);
  if (from)
    iks_insert_attrib (node, "to", from);
  else
    iks_insert_attrib (node, "to", NULL);
  iks_insert_attrib (node, "type", "error");
  enode = iks_insert (node, "error");
  iks_insert_attrib (enode, "type", "cancel");
  iks_insert_attrib (iks_insert (enode, "feature-not-implemented"),
                     "xmlns", "urn:ietf:params:xml:ns:xmpp-stanzas");
  iks_send (parser, node);
  return 0;
}

int
xmpp_session_hook (iksparser *parser, iks *node)
{
  iks *iq;
  iq = iks_new ("iq");
  iks_insert_attrib (iq, "type", "set");
  iks_insert_attrib (iq, "id", "session1");
  iks_insert_attrib (iks_insert (iq, "session"),
                     "xmlns", "urn:ietf:params:xml:ns:xmpp-session");
  iks_send (parser, iq);
  iks_delete (iq);
  return 0;
}

int
xmpp_initial_presence_hook (iksparser *parser, iks *node)
{
  iks *pres;
  pres = iks_make_pres (IKS_SHOW_AVAILABLE, "System blah!");
  iks_send (parser, pres);
  iks_delete (pres);
  return 0;
}

int
xmpp_id_hook (iksparser *parser, iks *node, char *id)
{
  if (!iks_strcmp (id, "bind1"))
    {
      xmpp_session_hook (parser, node);
      return 0;
    }
  else if (!iks_strcmp (id, "session1"))
    {
      xmpp_initial_presence_hook (parser, node);
      return 0;
    }
  return 1;
}

static int
xmpp_features_hook (JXmpp *ctx, iks *node)
{
  iks *feat;
  char *ns;

  for (feat = iks_child (node); feat != NULL; feat = iks_next (feat))
    {
      ns = iks_find_attrib (feat, "xmlns");
      if (!iks_strcmp (ns, "urn:ietf:params:xml:ns:xmpp-tls"))
        {
          if (iks_start_tls (ctx->parser) != IKS_OK)
            {
              j_log_warn (ctx->log, "TLS negotiation failed");
              return 1;
            }
          else
            {
              j_log_info (ctx->log, "TLS negotiation done");
              return 0;
            }
        }
      else if (!iks_strcmp (ns, "urn:ietf:params:xml:ns:xmpp-sasl"))
        {
          if (iks_start_sasl (ctx->parser, IKS_SASL_DIGEST_MD5,
                              ctx->id->user, ctx->password) != IKS_OK)
            {
              j_log_warn (ctx->log, "SASL negotiation failed");
              return 1;
            }
          else
            {
              j_log_info (ctx->log, "SASL started");
              return 0;
            }
        }
      else if (!iks_strcmp (ns, "urn:ietf:params:xml:ns:xmpp-bind"))
        {
          if (xmpp_bind_hook (ctx->parser, node) == 0)
            {
              j_log_info (ctx->log, "Bind hook sent");
              return 0;
            }
          else
            {
              j_log_warn (ctx->log, "Bind hook failed");
              return 1;
            }
        }
    }
  return 1;
}

int
xmpp_other_hook (JXmpp *ctx, iks *node, char *ns)
{
  if (!iks_strcmp (ns, "urn:ietf:params:xml:ns:xmpp-sasl"))
    {
      if (!iks_strcmp (iks_name (node), "success"))
        iks_send_header (ctx->parser, ctx->id->server);
      else if (!iks_strcmp (iks_name (node), "failure"))
        j_log_warn (ctx->log, "Authentication failed");
      return 0;
    }
  return 1;
}

static int
_j_xmpp_hook (void *data, int type, iks *node)
{
  char *name;
  char *id;
  char *ns;
  iksparser *parser;
  JXmpp *ctx;

  ctx = (JXmpp *) data;
  parser = ctx->parser;
  name = iks_name (node);
  id = iks_find_attrib (node, "id");
  ns = iks_find_attrib (node, "xmlns");

  /* Dispatching a notification to id handlers. */
  if (id)
    j_filter_call (ctx->ids, id, node);

  if (!iks_strcmp (name, "iq"))
    {
      j_filter_call (ctx->events, "iq", node);
      if (xmpp_id_hook (parser, node, id) == 0)
        return IKS_OK;
      xmpp_iq_error (parser, node);
    }
  else if (!iks_strcmp (name, "presence"))
    {
      char *from;
      from = iks_find_attrib (node, "from");
      j_filter_call (ctx->events, "presence", node);
      j_log_info (ctx->log, "I sense a disturbance in the force: %s!", from);
    }
  else if (!iks_strcmp (name, "message"))
    {
      char *from;
      char *body;
      from = iks_find_attrib (node, "from");
      body = iks_find_cdata (node, "body");
      j_filter_call (ctx->events, "message", node);
      j_log_info (ctx->log, "Xmpp message from '%s':\n%s", from, body);
    }
  else if (!iks_strcmp (name, "stream:features"))
    {
      /* Sending notification about incoming features */
      j_filter_call (ctx->events, "features", node);

      /* Calling our standard feature hook with tls, sasl and
       * binding operations. */
      if (xmpp_features_hook (ctx, node) == 0)
        {
          return IKS_OK;
        }
      else
        {
          j_log_critical (ctx->log, "Something wrong has happened:\n%s",
                          iks_string (iks_stack (node), node));
        }
    }
  else if (!iks_strcmp (name, "stream:error"))
    {
      char *inner_name;
      inner_name = iks_name (iks_child (node));

      /* Dispatching an event notifiying the error. */
      j_filter_call (ctx->events, "error", node);

      /* Trying to give more info about the error to the user. Some
       * kinds of errors can change the behaviour of the
       * connection. In this first example, that we're handling a
       * `host-unknown' error, we'll interrupt the recv loop since
       * we'll never get anything without connecting to the host. */
      if (!iks_strcmp (inner_name, "host-unknown"))
        {
          j_xmpp_stop (ctx);
          j_log_critical (ctx->log, "Unknown Host, aborting main loop");
        }
      else
        {
          j_log_warn (ctx->log, "streamerror: %s",
                      iks_string (iks_stack (node), node));
        }
    }
  else
    {
      if (xmpp_other_hook (ctx, node, ns) == 0)
        return IKS_OK;
      j_log_warn (ctx->log, "Unhandled hook: \"%s\"", name);
    }
  return IKS_OK;
}

/* The main loop of our xmpp client. */
static void *
_j_xmpp_do_run (void *user_data)
{
  int err;
  JXmpp *ctx = (JXmpp *) user_data;

  while (ctx->running)
    {
      if ((err = iks_recv (ctx->parser, -1)) != IKS_OK)
        {
          switch (err)
            {
            case IKS_NET_NOCONN:
              j_log_critical (ctx->log, "iks_recv: IKS_NET_NOCONN.");
              break;

            case IKS_NET_RWERR:
              j_log_critical (ctx->log, "iks_recv: IKS_NET_RWERR.");
              break;

            default:
              j_log_critical (ctx->log, "iks_recv: Unhandled error.");
              break;
            }
        }
    }
  j_log_info (ctx->log, "Main loop finished, calling disconnect.");
  iks_disconnect (ctx->parser);
  return NULL;
}
