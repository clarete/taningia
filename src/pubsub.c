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
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <taningia/pubsub.h>
#include <taningia/log.h>

#define NS_INFO "http://jabber.org/protocol/disco#info"
#define NS_ITEM "http://jabber.org/protocol/disco#items"
#define NS_PS_CONFIG "http://jabber.org/protocol/pubsub#node_config"

#ifdef DEBUG
# define debugiq(x) fprintf (stderr, "[ps] %s\n", iks_string (iks_stack (x), x))
#else
# define debugiq(x)
#endif

struct _TPubsub
{
  char *from;
  char *to;
  char *node_prefix;
  int   node_prefix_len;
  TLog *log;
};

struct _TPubsubNode
{
  TPubsub *ctx;
  char *name;
};

/* Generic help functions */

static iks *
createiq (TPubsub *ctx, enum iksubtype type, const char *ns)
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
createiqps (TPubsub *ctx, enum iksubtype type)
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

/* TPubsub */

static char *
_t_pubsub_get_node_prefix (TPubsub *ctx)
{
  ikstack *stack;
  iksid *id;
  int path_size;
  char *real_name;
  stack = iks_stack_new (128, 0);
  id = iks_id_new (stack, ctx->from);
  iks_stack_delete (stack);
  path_size =
    strlen (id->server) +
    strlen (id->user) + 8 + 1;
  real_name = malloc (path_size);
  snprintf (real_name, path_size, "/home/%s/%s/",
           id->server, id->user);
  ctx->node_prefix_len = path_size;
  return real_name;
}

TPubsub *
t_pubsub_new (const char *from, const char *to)
{
  TPubsub *ctx;
  ctx = malloc (sizeof (TPubsub));
  ctx->from = strdup (from);
  ctx->to = strdup (to);
  ctx->log = t_log_new ("pubsub");
  ctx->node_prefix_len = 0;     /* Will be filled in the next line. */
  ctx->node_prefix = _t_pubsub_get_node_prefix (ctx);
  return ctx;
}

void
t_pubsub_free (TPubsub *ctx)
{
  free (ctx->from);
  free (ctx->to);
  t_log_free (ctx->log);
  if (ctx->node_prefix)
    free (ctx->node_prefix);
  free (ctx);
}

const char *
t_pubsub_get_node_prefix (TPubsub *ctx)
{
  return ctx->node_prefix;
}

void
t_pubsub_set_node_prefix (TPubsub    *ctx,
                          const char *prefix)
{
  if (ctx->node_prefix)
    free (ctx->node_prefix);
  ctx->node_prefix = strdup (prefix);
}

iks *
t_pubsub_query_features (TPubsub *ctx)
{
  iks *iq = createiq (ctx, IKS_TYPE_GET, NS_INFO);
  debugiq (iq);
  return iq;
}

iks *
t_pubsub_query_affiliations (TPubsub *ctx)
{
  iks *iq, *sub;
  iq = createiqps (ctx, IKS_TYPE_GET);
  sub = iks_insert (iks_child (iq), "affiliations");
  debugiq (iq);
  return iq;
}

/* TPubsubNode */

TPubsubNode *
t_pubsub_node_new (TPubsub *ctx, const char *name)
{
  TPubsubNode *node;
  node = malloc (sizeof (TPubsubNode));
  node->ctx = ctx;
  node->name = name ? strdup (name) : NULL;
  return node;
}

const char  *
t_pubsub_node_get_name (TPubsubNode *node)
{
  return node->name;
}

void
t_pubsub_node_set_name (TPubsubNode *node,
                        const char  *name)
{
  if (node->name)
    free (node->name);
  node->name = strdup (name);
}

void
t_pubsub_node_free (TPubsubNode *node)
{
  if (node->name)
    free (node->name);
  free (node);
}

iks *
t_pubsub_node_query_info (TPubsubNode *node)
{
  if (node->name)
    {
      iks *iq = createiq (node->ctx, IKS_TYPE_GET, NS_INFO);
      iks_insert_attrib (iks_child (iq), "node", node->name);
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.query_info with no name set");
      return NULL;
    }
}

iks *
t_pubsub_node_subscriptions (TPubsubNode *node)
{
  if (node->name)
    {
      iks *iq, *sub;
      iq = createiqps (node->ctx, IKS_TYPE_GET);
      sub = iks_insert (iks_child (iq), "subscriptions");
      if (node != NULL)
        iks_insert_attrib (sub, "node", node->name);
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.subscriptions with no name set");
      return NULL;
    }
}

iks *
t_pubsub_node_affiliations (TPubsubNode *node)
{
  if (node->name)
    {
      iks *iq, *sub;
      iq = createiqps (node->ctx, IKS_TYPE_GET);
      sub = iks_insert (iks_child (iq), "affiliations");
      iks_insert_attrib (sub, "node", node->name);
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.affiliations method with no name set");
      return NULL;
    }
}

iks *
t_pubsub_node_subscribe (TPubsubNode *node,
                         const char  *jid)
{
  if (node->name)
    {
      iks *iq, *sub;
      iq = createiqps (node->ctx, IKS_TYPE_SET);
      sub = iks_insert (iks_child (iq), "subscribe");
      iks_insert_attrib (sub, "node", node->name);
      if (jid != NULL)
        iks_insert_attrib (sub, "jid", jid);
      else
        iks_insert_attrib (sub, "jid", node->ctx->from);
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.subscribe method with no name set");
      return NULL;
    }
}

iks *
t_pubsub_node_unsubscribe (TPubsubNode *node,
                           const char  *jid)
{
  if (node->name)
    {
      iks *iq, *unsub;
      iq = createiqps (node->ctx, IKS_TYPE_SET);
      unsub = iks_insert (iks_child (iq), "unsubscribe");
      iks_insert_attrib (unsub, "node", node->name);
      if (jid != NULL)
        iks_insert_attrib (unsub, "jid", jid);
      else
        iks_insert_attrib (unsub, "jid", node->ctx->from);
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.unsubscribe method with no name set");
      return NULL;
    }
}

iks *
t_pubsub_query_node_items (TPubsubNode *node)
{
  if (node->name)
    {
      iks *iq = createiq (node->ctx, IKS_TYPE_GET, NS_ITEM);
      if (node != NULL)
        iks_insert_attrib (iks_child (iq), "node", node->name);
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.items method with no name set");
      return NULL;
    }
}

iks *
t_pubsub_node_items (TPubsubNode *node,
                     int          max_items)
{
  if (node->name)
    {
      iks *iq, *items;
      char num[32];
      iq = createiqps (node->ctx, IKS_TYPE_GET);
      items = iks_insert (iks_child (iq), "items");
      iks_insert_attrib (items, "node", node->name);
      if (max_items > 0)
        {
          snprintf (num, sizeof (num), "%d", max_items);
          iks_insert_attrib (items, "max_items", num);
        }
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.items method with no name set");
      return NULL;
    }
}

iks *
t_pubsub_node_publish_text (TPubsubNode *node,
                            const char  *id,
                            const char  *body,
                            int          len)
{
  if (node->name)
    {
      iks *iq, *publish, *item;
      iq = createiqps (node->ctx, IKS_TYPE_SET);
      publish = iks_insert (iks_child (iq), "publish");
      iks_insert_attrib (publish, "node", node->name);
      item = iks_insert (publish, "item");
      if (id)
        iks_insert_attrib (item, "id", id);
      iks_insert_cdata (item, body, len);
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.publish_text method with no name set");
      return NULL;
    }
}

iks *
t_pubsub_node_publish_iks (TPubsubNode *node,
                           const char  *id,
                           iks         *child)
{
  if (node->name)
    {
      iks *iq, *publish, *item;
      iq = createiqps (node->ctx, IKS_TYPE_SET);
      publish = iks_insert (iks_child (iq), "publish");
      iks_insert_attrib (publish, "node", node->name);
      item = iks_insert (publish, "item");
      if (id)
        iks_insert_attrib (item, "id", id);
      iks_insert_node (item, child);
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.publish_iks method with no name set");
      return NULL;
    }
}

iks *
t_pubsub_node_delete_item (TPubsubNode *node,
                           const char  *id)
{
  if (node->name)
    {
      iks *iq, *retract;
      iq = createiqps (node->ctx, IKS_TYPE_SET);
      retract = iks_insert (iks_child (iq), "retract");
      iks_insert_attrib (retract, "node", node->name);
      iks_insert_attrib (iks_insert (retract, "item"), "id", id);
      debugiq (iq);
      return iq;
    }
  else
    {
      t_log_warn (node->ctx->log,
                  "Calling Node.delete_item method with no name set");
      return NULL;
    }
}

iks *
t_pubsub_node_create (TPubsubNode *node,
                      ...)
{
  iks *iq, *create, *config, *form;
  va_list args;
  char *arg;
  int nargs, nvals, i;
  iq = createiqps (node->ctx, IKS_TYPE_SET);
  create = iks_insert (iks_child (iq), "create");

  if (node->name)
    {
      if (node->ctx->node_prefix)
        {
          size_t name_size = strlen (node->name);
          size_t fsize = node->ctx->node_prefix_len + name_size;
          char *full_name = malloc (fsize);
          memcpy (full_name, node->ctx->node_prefix,
                  node->ctx->node_prefix_len);
          memcpy ((full_name+node->ctx->node_prefix_len),
                  node->name, name_size);
          iks_insert_attrib (create, "node", full_name);
          free (full_name);
        }
      else
        iks_insert_attrib (create, "node", node->name);
    }

  /* Building configuration */
  config = iks_insert (iks_child (iq), "configure");
  form = NULL;
  nargs = nvals = i = 0;
  va_start (args, node);

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
