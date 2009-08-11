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
#include <jarvis/filter.h>

struct _JFilter
{
  GHashTable *hash;
  void *data;
};

JFilter *
j_filter_new (void *data)
{
  JFilter *filter;
  filter = g_slice_new (JFilter);
  filter->data = data;
  filter->hash =
    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  return filter;
}

void
j_filter_free (JFilter *filter)
{
  g_hash_table_unref (filter->hash);
  g_slice_free (JFilter, filter);
}

void
j_filter_add (JFilter         *filter,
              const char      *name,
              JFilterCallback  cb,
              void            *data)
{
  GHook *hook;
  GHookList *hlist;
  if (!g_hash_table_lookup_extended (filter->hash, name, NULL, (gpointer)&hlist))
    {
      hlist = g_slice_new (GHookList);
      g_hook_list_init (hlist, sizeof (GHook));
      g_hash_table_insert (filter->hash, g_strdup (name), hlist);
    }
  hook = g_hook_alloc (hlist);
  hook->func = cb;
  hook->data = data;
  g_hook_append (hlist, hook);
}

static gboolean
j_filter_marshal (GHook *hook, gpointer *data)
{

  JFilterCallback cb;
  cb = hook->func;
  cb (data[0], hook->data, data[1]);
  return 1;
}

int
j_filter_call (JFilter    *filter,
               const char *name,
               void       *packet)
{
  GHookList *hlist;
  gpointer data[2] = {filter->data, packet};

  if (name == NULL)
    return 0;

  if (g_hash_table_lookup_extended (filter->hash, name, NULL, (gpointer *)&hlist))
    {
      g_hook_list_marshal_check (hlist, FALSE,
                                 (GHookCheckMarshaller) j_filter_marshal,
                                 data);
      return 1;
    }

  return 0;
}
