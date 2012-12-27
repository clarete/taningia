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
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <taningia/pubsub.h>

#define NS_INFO              "http://jabber.org/protocol/disco#info"
#define NS_ITEM              "http://jabber.org/protocol/disco#items"
#define NS_PS_CONFIG         "http://jabber.org/protocol/pubsub#node_config"
#define NS_PS_OWNER          "http://jabber.org/protocol/pubsub#owner"
#define NODE_SIZE_MAX        256 /* Max size of a node name */

/* Generic help functions */

static iks *
createiq (const char *from,
          const char *to,
          enum iksubtype type,
          const char *ns)
{
  iks *iq;
  char sid[32];
  static int nid = 0;
  snprintf (sid, 32, "query%d", nid++);
  iq = iks_make_iq (type, ns);
  iks_insert_attrib (iq, "from", from);
  iks_insert_attrib (iq, "to", to);
  iks_insert_attrib (iq, "id", sid);
  return iq;
}

static iks *
createiqps (const char *from, const char *to, enum iksubtype type)
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
  iks_insert_attrib (iq, "from", from);
  iks_insert_attrib (iq, "to", to);
  iks_insert_attrib (iq, "id", sid);
  iks_insert_attrib (iks_insert (iq, "pubsub"), "xmlns", TA_PUBSUB_NS);
  return iq;
}

/* general stuff */

iks *
ta_pubsub_query_features (const char *from, const char *to)
{
  iks *iq = createiq (from, to, IKS_TYPE_GET, NS_INFO);
  return iq;
}

iks *
ta_pubsub_query_affiliations (const char *from, const char *to)
{
  iks *iq;
  iq = createiqps (from, to, IKS_TYPE_GET);
  iks_insert (iks_child (iq), "affiliations");
  return iq;
}

/* node stuff */

iks *
ta_pubsub_node_query_info (const char *from, const char *to, const char *node)
{
  iks *iq = createiq (from, to, IKS_TYPE_GET, NS_INFO);
  iks_insert_attrib (iks_child (iq), "node", node);
  return iq;
}

iks *
ta_pubsub_node_query_subscriptions (const char *from,
                                    const char *to,
                                    const char *node)
{
  iks *iq, *sub;
  iq = createiqps (from, to, IKS_TYPE_GET);
  sub = iks_insert (iks_child (iq), "subscriptions");
  if (node != NULL)
    iks_insert_attrib (sub, "node", node);
  return iq;
}

iks *
ta_pubsub_node_query_affiliations (const char *from,
                                   const char *to,
                                   const char *node)
{
  iks *iq, *sub;
  iq = createiqps (from, to, IKS_TYPE_GET);
  sub = iks_insert (iks_child (iq), "affiliations");
  iks_insert_attrib (sub, "node", node);
  return iq;
}

iks *
ta_pubsub_node_subscribe (const char *from,
                          const char *to,
                          const char *node,
                          const char *jid)
{
  iks *iq, *sub;
  iq = createiqps (from, to, IKS_TYPE_SET);
  sub = iks_insert (iks_child (iq), "subscribe");
  iks_insert_attrib (sub, "node", node);
  if (jid != NULL)
    iks_insert_attrib (sub, "jid", jid);
  else
    iks_insert_attrib (sub, "jid", from);
  return iq;
}

iks *
ta_pubsub_node_unsubscribe (const char *from,
                            const char *to,
                            const char *node,
                            const char *jid)
{
  iks *iq, *unsub;
  iq = createiqps (from, to, IKS_TYPE_SET);
  unsub = iks_insert (iks_child (iq), "unsubscribe");
  iks_insert_attrib (unsub, "node", node);
  if (jid != NULL)
    iks_insert_attrib (unsub, "jid", jid);
  else
    iks_insert_attrib (unsub, "jid", from);
  return iq;
}

iks *
ta_pubsub_node_query_nodes (const char *from, const char *to, const char *node)
{
  iks *iq = createiq (from, to, IKS_TYPE_GET, NS_ITEM);
  if (node)
    iks_insert_attrib (iks_child (iq), "node", node);
  return iq;
}

iks *
ta_pubsub_node_items (const char *from,
                      const char *to,
                      const char *node,
                      int max_items)
{
  iks *iq, *items;
  char num[32];
  iq = createiqps (from, to, IKS_TYPE_GET);
  items = iks_insert (iks_child (iq), "items");
  iks_insert_attrib (items, "node", node);
  if (max_items > 0)
    {
      snprintf (num, sizeof (num), "%d", max_items);
      iks_insert_attrib (items, "max_items", num);
    }
  return iq;
}

iks *
ta_pubsub_node_publish_text (const char *from,
                             const char *to,
                             const char *node,
                             const char *id,
                             const char *body,
                             int len)
{
  iks *iq, *publish, *item;
  iq = createiqps (from, to, IKS_TYPE_SET);
  publish = iks_insert (iks_child (iq), "publish");
  iks_insert_attrib (publish, "node", node);
  item = iks_insert (publish, "item");
  if (id)
    iks_insert_attrib (item, "id", id);
  iks_insert_cdata (item, body, len);
  return iq;
}

iks *
ta_pubsub_node_publish_iks (const char *from,
                            const char *to,
                            const char *node,
                            const char *id,
                            iks *child)
{
  iks *iq, *publish, *item;
  iq = createiqps (from, to, IKS_TYPE_SET);
  publish = iks_insert (iks_child (iq), "publish");
  iks_insert_attrib (publish, "node", node);
  item = iks_insert (publish, "item");
  if (id)
    iks_insert_attrib (item, "id", id);
  iks_insert_node (item, child);
  return iq;
}

iks *
ta_pubsub_node_delete (const char *from, const char *to, const char *node)
{
  iks *iq, *ps, *del;
  iq = createiqps (from, to, IKS_TYPE_SET);
  ps = iks_child (iq);
  iks_insert_attrib (ps, "xmlns", NS_PS_OWNER);
  del = iks_new ("delete");
  iks_insert_attrib (del, "node", node);
  iks_insert_node (ps, del);
  return iq;
}

iks *
ta_pubsub_node_delete_item (const char *from,
                            const char *to,
                            const char *node,
                            const char *id)
{
  iks *iq, *retract;
  iq = createiqps (from, to, IKS_TYPE_SET);
  retract = iks_insert (iks_child (iq), "retract");
  iks_insert_attrib (retract, "node", node);
  iks_insert_attrib (iks_insert (retract, "item"), "id", id);
  return iq;
}

iks *
ta_pubsub_node_create (const char *from,
                       const char *to,
                       const char *node,
                       ...)
{
  iks *iq, *create, *config, *form;
  va_list args;
  char *arg;
  int nargs, nvals, i;
  iq = createiqps (from, to, IKS_TYPE_SET);
  create = iks_insert (iks_child (iq), "create");

  if (node)
    iks_insert_attrib (create, "node", node);

  /* Building configuration */
  form = NULL;
  config = iks_insert (iks_child (iq), "configure");
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
          char fname[NODE_SIZE_MAX];
          snprintf (fname, NODE_SIZE_MAX, "pubsub#%s", arg);
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
  return iq;
}

iks *
ta_pubsub_node_createv (const char *from,
                        const char *to,
                        const char *node,
                        const char **conf_params)
{
  iks *iq, *create, *config, *form;
  const char *arg;
  int nargs, nvals, i;
  iq = createiqps (from, to, IKS_TYPE_SET);
  create = iks_insert (iks_child (iq), "create");

  if (node)
    iks_insert_attrib (create, "node", node);

  /* Building configuration */
  config = iks_insert (iks_child (iq), "configure");
  form = NULL;
  nargs = nvals = i = 0;

  /* Creating the dataform to hold config info */
  while (conf_params && (arg = conf_params[i]) != NULL)
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
          char fname[NODE_SIZE_MAX];
          snprintf (fname, NODE_SIZE_MAX, "pubsub#%s", arg);
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

  /* Making sure that user didn't forget to build `pairs' of keys and
   * vals */
  assert (nargs == nvals);
  return iq;
}
