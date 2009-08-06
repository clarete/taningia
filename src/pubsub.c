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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "pubsub.h"

#define NS_INFO "http://jabber.org/protocol/disco#info"
#define NS_ITEM "http://jabber.org/protocol/disco#items"
#define NS_PS_CONFIG "http://jabber.org/protocol/pubsub#node_config"
#define debugiq(x) g_message (iks_string (iks_stack (x), x))

struct _ps_ctx {
  char *from;
  char *to;
};

ps_ctx *
ps_ctx_new (const char *from, const char *to)
{
  ps_ctx *ctx;
  ctx = g_slice_new (ps_ctx);
  ctx->from = g_strdup (from);
  ctx->to = g_strdup (to);
  return ctx;
}

void
ps_ctx_free (ps_ctx *ctx)
{
  g_free (ctx->from);
  ctx->from = NULL;
  g_free (ctx->to);
  ctx->to = NULL;
  g_slice_free (ps_ctx, ctx);
}

static iks *
createiq (ps_ctx *ctx, enum iksubtype type, const char *ns)
{
  iks *iq;
  char sid[32];
  static int nid = 0;
  snprintf (sid, 32, "query%d", nid++);
  iq = iks_make_iq (type, ns);
  iks_insert_attrib (iq, "to", ctx->to);
  iks_insert_attrib (iq, "from", ctx->from);
  iks_insert_attrib (iq, "id", sid);
  return iq;
}

static iks *
createiqps (ps_ctx *ctx, enum iksubtype type)
{
  iks *iq;
  char sid[32];
  char *t = NULL;
  static int nid = 0;
  snprintf (sid, 32, "ps%d", nid++);
  iq = iks_new ("iq");
  switch (type) {
  case IKS_TYPE_GET: t = "get"; break;
  case IKS_TYPE_SET: t = "set"; break;
  case IKS_TYPE_RESULT: t = "result"; break;
  case IKS_TYPE_ERROR: t = "error"; break;
  default: break;
  }
  if (t) iks_insert_attrib (iq, "type", t);
  iks_insert_attrib (iq, "to", ctx->to);
  iks_insert_attrib (iq, "from", ctx->from);
  iks_insert_attrib (iq, "id", sid);
  iks_insert_attrib (iks_insert (iq, "pubsub"), "xmlns", PUBSUB_NS);
  return iq;
}

iks *
ps_query_features (ps_ctx *ctx)
{
  iks *iq = createiq (ctx, IKS_TYPE_GET, NS_INFO);
  debugiq (iq);
  return iq;
}

iks *
ps_query_node_info (ps_ctx     *ctx,
                    const char *node)
{
  iks *iq = createiq (ctx, IKS_TYPE_GET, NS_INFO);
  iks_insert_attrib (iks_child (iq), "node", node);
  debugiq (iq);
  return iq;
}

iks *
ps_query_node_items (ps_ctx     *ctx,
                     const char *node)
{
  iks *iq = createiq (ctx, IKS_TYPE_GET, NS_ITEM);
  if (node != NULL)
    iks_insert_attrib (iks_child (iq), "node", node);
  debugiq (iq);
  return iq;
}

iks *
ps_query_node_subscriptions (ps_ctx     *ctx,
                             const char *node)
{
  iks *iq, *sub;
  iq = createiqps (ctx, IKS_TYPE_GET);
  sub = iks_insert (iks_child (iq), "subscriptions");
  if (node != NULL)
    iks_insert_attrib (sub, "node", node);
  debugiq (iq);
  return iq;
}

iks *
ps_query_node_affiliations (ps_ctx *ctx)
{
  iks *iq, *sub;
  iq = createiqps (ctx, IKS_TYPE_GET);
  sub = iks_insert (iks_child (iq), "subscriptions");
  debugiq (iq);
  return iq;
}

iks *
ps_node_subscribe (ps_ctx     *ctx,
                   const char *node,
                   const char *jid)
{
  iks *iq, *sub;
  iq = createiqps (ctx, IKS_TYPE_SET);
  sub = iks_insert (iks_child (iq), "subscribe");
  iks_insert_attrib (sub, "node", node);
  if (jid != NULL)
    iks_insert_attrib (sub, "jid", jid);
  else
    iks_insert_attrib (sub, "jid", ctx->from);
  debugiq (iq);
  return iq;
}

iks *
ps_node_unsubscribe (ps_ctx     *ctx,
                     const char *node,
                     const char *jid)
{
  iks *iq, *unsub;
  iq = createiqps (ctx, IKS_TYPE_SET);
  unsub = iks_insert (iks_child (iq), "unsubscribe");
  iks_insert_attrib (unsub, "node", node);
  if (jid != NULL)
    iks_insert_attrib (unsub, "jid", jid);
  else
    iks_insert_attrib (unsub, "jid", ctx->from);
  debugiq (iq);
  return iq;
}

iks *
ps_node_retrieve_items (ps_ctx     *ctx,
                        const char *node,
                        int         max_items)
{
  iks *iq, *items;
  char num[32];
  iq = createiqps (ctx, IKS_TYPE_GET);
  items = iks_insert (iks_child (iq), "items");
  iks_insert_attrib (items, "node", node);
  if (max_items > 0)
    {
      snprintf (num, 32, "%d", max_items);
      iks_insert_attrib (items, "max_items", num);
    }
  debugiq (iq);
  return iq;
}

iks *
ps_publish_item_text (ps_ctx     *ctx,
                      const char *node,
                      const char *id,
                      const char *body,
                      int         len)
{
  iks *iq, *publish, *item;
  iq = createiqps (ctx, IKS_TYPE_SET);
  publish = iks_insert (iks_child (iq), "publish");
  iks_insert_attrib (publish, "node", node);
  item = iks_insert (publish, "item");
  if (id)
    iks_insert_attrib (item, "id", id);
  iks_insert_cdata (item, body, len);
  debugiq (iq);
  return iq;
}

iks *
ps_delete_item (ps_ctx     *ctx,
                const char *node,
                const char *id)
{
  iks *iq, *retract;
  iq = createiqps (ctx, IKS_TYPE_SET);
  retract = iks_insert (iks_child (iq), "retract");
  iks_insert_attrib (retract, "node", node);
  iks_insert_attrib (iks_insert (retract, "item"), "id", id);
  debugiq (iq);
  return iq;
}

iks *
ps_node_create (ps_ctx     *ctx,
                const char *name,
                int         bare,
                ...)
{
  iks *iq, *create, *config, *form;
  va_list args;
  char *arg;
  int nargs, nvals, i;
  iq = createiqps (ctx, IKS_TYPE_SET);
  create = iks_insert (iks_child (iq), "create");
  if (name && bare == 1)
    iks_insert_attrib (create, "node", name);
  else if (name && bare != 1)
    {
      iksid *id = iks_id_new (iks_stack (iq), ctx->from);
      int path_size = strlen(name) +
        strlen(id->server) +
        strlen(id->user) + 8 + 1;
      char real_name[path_size];
      snprintf(real_name, path_size, "/home/%s/%s/%s",
               id->server, id->user, name);
      iks_insert_attrib (create, "node", real_name);
    }

  /* Building configuration */
  config = iks_insert (iks_child (iq), "configure");
  form = NULL;
  nargs = nvals = i = 0;
  va_start (args, bare);

  /* Creating the dataform to hold config info */
  while ((arg = va_arg (args, char *)) != NULL)
    {
      iks *field;
      if (form == NULL)
        {
          iks *f;
          form = iks_insert (config, "x");
          iks_insert_attrib (form, "xmlns", "jabber:x:data");
          iks_insert_attrib (form, "type", "submit");
          f = iks_insert (form, "field");
          iks_insert_attrib (f, "var", "FORM_TYPE");
          iks_insert_attrib (f, "type", "hidden");
          iks_insert_cdata (iks_insert (f, "value"), NS_PS_CONFIG, 0);
        }
      if (i % 2 == 0)           /* Handling args */
        {
          size_t ssize = strlen (arg)+7+1;
          char fname[ssize];
          snprintf (fname, ssize, "pubsub#%s", arg);
          field = iks_insert (form, "field");
          iks_insert_attrib (field, "var", fname);
          nargs++;
        }
      else                      /* Handling vals */
        {
          iks_insert_cdata (iks_insert (field, "value"), arg, 0);
          nvals++;
        }
      i++;
    }
  va_end (args);
  /* Making sure that user didn't forget to build `pairs' of keys and
   * vals */
  assert (nargs == nvals);
  debugiq (iq);
  return iq;
}
