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

#include <glib.h>
#include <iksemel.h>

#include <jarvis/xmpp.h>

/* Prototypes of some local functions */

static int      _rb_xmpp_hook       (void *data, int type, iks *node);

static void     _rb_xmpp_do_run     (XmppContext *ctx);

/**
 * Allocate memory and initialize stuff necessary to create a new
 * XmppContext.
 */
XmppContext *
rb_xmpp_new (const char *jid,
             const char *password,
             const char *pubsub)
{
  XmppContext *ctx;
  g_return_val_if_fail (jid != NULL, NULL);
  g_return_val_if_fail (password != NULL, NULL);
  g_return_val_if_fail (pubsub != NULL, NULL);
  ctx = g_slice_new (XmppContext);
  ctx->pubsub = g_strdup (pubsub);
  ctx->jid = g_strdup (jid);
  ctx->password = g_strdup (password);
  ctx->parser = iks_stream_new (IKS_NS_CLIENT, ctx, _rb_xmpp_hook);
  ctx->id = iks_id_new (iks_parser_stack (ctx->parser), ctx->jid);
  return ctx;
}

/**
 * Free the memory used by a XmppContext struct.
 */
void
rb_xmpp_free (XmppContext *ctx)
{
  g_return_if_fail (ctx != NULL);
  g_free (ctx->jid);
  ctx->jid = NULL;
  g_free (ctx->password);
  ctx->password = NULL;
  g_free (ctx->pubsub);
  ctx->pubsub = NULL;
  iks_parser_delete (ctx->parser);
  g_slice_free (XmppContext, ctx);
}

/**
 * This function starts the main xmpp (libstrophe) loop in another
 * thread. To avoid problems with our loved GMainLoop used by soup.
 */
void
rb_xmpp_run (XmppContext *ctx)
{
  g_return_if_fail (ctx != NULL);
  g_thread_create ((GThreadFunc) _rb_xmpp_do_run, ctx, FALSE, NULL);
  g_message ("Running xmpp client");
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
  else if (!iks_strncmp (id, "ps", 2))
    {
      g_message (iks_string (iks_stack (node), node));
    }
  return 1;
}

static int
xmpp_features_hook (XmppContext *ctx, iks *node)
{
  iks *feat;
  char *ns;

  g_return_val_if_fail (ctx != NULL, 1);

  for (feat = iks_child (node); feat != NULL; feat = iks_next (feat))
    {
      ns = iks_find_attrib (feat, "xmlns");
      if (!iks_strcmp (ns, "urn:ietf:params:xml:ns:xmpp-tls"))
        {
          iks_start_tls (ctx->parser);
          return 0;
        }
      else if (!iks_strcmp (ns, "urn:ietf:params:xml:ns:xmpp-sasl"))
        {
          iks_start_sasl (ctx->parser, IKS_SASL_DIGEST_MD5,
                          ctx->id->user, ctx->password);
          return 0;
        }
      else if (!iks_strcmp (ns, "urn:ietf:params:xml:ns:xmpp-bind"))
        {
          xmpp_bind_hook (ctx->parser, node);
          return 0;
        }
    }
  return 1;
}

int
xmpp_other_hook (XmppContext *ctx, iks *node, char *ns)
{
  if (!iks_strcmp (ns, "urn:ietf:params:xml:ns:xmpp-sasl"))
    {
      if (!iks_strcmp (iks_name (node), "success"))
        iks_send_header (ctx->parser, ctx->id->server);
      else if (!iks_strcmp (iks_name (node), "failure"))
        g_message ("failture to authenticate: %s",
                   iks_string (iks_stack (node), node));
      return 0;
    }
  return 1;
}

static int
_rb_xmpp_hook (void *data, int type, iks *node)
{
  char *name;
  char *id;
  char *ns;
  iksparser *parser;
  XmppContext *ctx;

  ctx = (XmppContext *) data;
  parser = ctx->parser;
  name = iks_name (node);
  id = iks_find_attrib (node, "id");
  ns = iks_find_attrib (node, "xmlns"); 

  if (!iks_strcmp (name, "iq"))
    {
      if (xmpp_id_hook (parser, node, id) == 0)
        return IKS_OK;
      xmpp_iq_error (parser, node);
    }
  else if (!iks_strcmp (name, "presence"))
    {
      char *from;
      from = iks_find_attrib (node, "from");
      g_message ("I sense a disturbance in the force: %s!", from);
    }
  else if (!iks_strcmp (name, "message"))
    {
      char *from;
      char *body;
      from = iks_find_attrib (node, "from");
      body = iks_find_cdata (node, "body");
      g_message ("Xmpp message from '%s':\n%s", from, body);
    }
  else if (!iks_strcmp (name, "stream:features"))
    {
      if (xmpp_features_hook (ctx, node) == 0)
        return IKS_OK;
    }
  else if (!iks_strcmp (name, "stream:error"))
    {
      g_warning ("streamerror: %s\n", iks_string (iks_stack (node), node));
    }
  else
    {
      if (xmpp_other_hook (ctx, node, ns) == 0)
        return IKS_OK;
      g_message ("No no: %s", name);
    }
  return IKS_OK;
}

/* The main loop of our xmpp client. */
static void
_rb_xmpp_do_run (XmppContext *ctx)
{
  g_return_if_fail (ctx != NULL);
  iks_connect_tcp (ctx->parser, ctx->id->server, 5222);

  while (TRUE)
    iks_recv (ctx->parser, -1);
}
