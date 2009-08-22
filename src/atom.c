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

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iksemel.h>
#include <glib.h>               /* to g_time_val_to_iso8601 */
#include <taningia/atom.h>
#include <taningia/iri.h>

struct _TAtomContent
{
  char *type;
  char *content;
  int   len;
  TIri *src;
};

struct _TAtomAuthor
{
  char *name;
  char *email;
  char *iri;
};

struct _TAtomCategory
{
  char *label;
  char *term;
  char *scheme;
};

struct _TAtomEntry
{
  char         *id;
  char         *title;
  time_t        updated;
  GPtrArray    *authors;
  GPtrArray    *categories;
  char         *summary;
  TAtomContent *content;
};

/* TAtomContent */

TAtomContent *
t_atom_content_new (const char *type,
                    const char *content,
                    int         len)
{
  TAtomContent *ct;
  ct = malloc (sizeof (TAtomContent));
  ct->type = strdup (type);
  ct->content = content ? strndup (content, len) :  NULL;
  ct->src = NULL;
  ct->len = len;
  return ct;
}

void
t_atom_content_free (TAtomContent *content)
{
  if (content->type)
    free (content->type);
  if (content->content)
    free (content->content);
  if (content->src)
    t_iri_free (content->src);
  free (content);
}

iks *
t_atom_content_to_iks (TAtomContent *content)
{
  iks *ct;
  ct = iks_new ("content");
  iks_insert_attrib (ct, "type", content->type);
  if (content->src != NULL)
    {
      char *iri;
      iri = t_iri_to_string (content->src);
      iks_insert_attrib (ct, "src", iri);
      free (iri);
    }
  else if (content->content)
    iks_insert_cdata (ct, content->content, content->len);
  return ct;
}

char *
t_atom_content_to_string (TAtomContent *content)
{
  iks *ik = t_atom_content_to_iks (content);
  return iks_string (iks_stack (ik), ik);
}

TIri *
t_atom_content_get_src (TAtomContent *content)
{
  return content->src;
}

void
t_atom_content_set_src (TAtomContent *content,
                        TIri         *src)
{
  if (content->src)
    {
      t_iri_free (content->src);
      if (src != NULL && content->content)
        {
          free (content->content);
          content->content = NULL;
        }
    }
  content->src = src;
}

const char *
t_atom_content_get_content (TAtomContent *content,
                            int          *len)
{
  if (len)
    *len = content->len;
  return content->content;
}

void
t_atom_content_set_content (TAtomContent *content,
                            const char   *text,
                            int           len)
{
  if (content->content)
    {
      free (content->content);
      if (text != NULL && content->src)
        {
          t_iri_free (content->src);
          content->src = NULL;
        }
    }
  content->content = strdup (text);
}


/* TAtomAuthor */

TAtomAuthor *
t_atom_author_new (const char *name,
                   const char *email,
                   const char *iri)
{
  TAtomAuthor *author;
  author = malloc (sizeof (TAtomAuthor));
  author->name = strdup (name);
  author->email = email ? strdup (email) : NULL;
  author->iri = iri ? strdup (iri) : NULL;
  return author;
}

void
t_atom_author_free (TAtomAuthor *author)
{
  if (author->name)
    free (author->name);
  if (author->email)
    free (author->email);
  if (author->iri)
    free (author->iri);
  free (author);
}

iks *
t_atom_author_to_iks (TAtomAuthor *author)
{
  iks *ik;
  ik = iks_new ("author");
  iks_insert_cdata (iks_insert (ik, "name"), author->name, 0);
  if (author->email)
    iks_insert_cdata (iks_insert (ik, "email"), author->email, 0);
  if (author->iri)
    iks_insert_cdata (iks_insert (ik, "uri"), author->iri, 0);
  return ik;
}

char *
t_atom_author_to_string (TAtomAuthor *author)
{
  iks *ik = t_atom_author_to_iks (author);
  return iks_string (iks_stack (ik), ik);
}

const char *
t_atom_author_get_name (TAtomAuthor *author)
{
  return (const char *) author->name;
}

void
t_atom_author_set_name (TAtomAuthor *author,
                        const char  *name)
{
  if (author->name)
    free (author->name);
  author->name = strdup (name);
}

const char *
t_atom_author_get_email (TAtomAuthor *author)
{
  return (const char *) author->email;
}

void
t_atom_author_set_email (TAtomAuthor *author,
                         const char  *email)
{
  if (author->email)
    free (author->email);
  author->email = strdup (email);
}

const char *
t_atom_author_get_iri (TAtomAuthor *author)
{
  return (const char *) author->iri;
}

void
t_atom_author_set_iri (TAtomAuthor *author,
                       const char  *iri)
{
  if (author->iri)
    free (author->iri);
  author->iri = strdup (iri);
}

/* TAtomCategory */

TAtomCategory *
t_atom_category_new (const char *term,
                     const char *label,
                     const char *scheme)
{
  TAtomCategory *cat;
  cat = malloc (sizeof (TAtomCategory));
  cat->term = strdup (term);
  cat->label = label ? strdup (label) : NULL;
  cat->scheme = scheme ? strdup (scheme) : NULL;
  return cat;
}

void
t_atom_category_free (TAtomCategory  *category)
{
  if (category->term)
    free (category->term);
  if (category->label)
    free (category->label);
  if (category->scheme)
    free (category->scheme);
  free (category);
}

iks *
t_atom_category_to_iks (TAtomCategory *category)
{
  iks *ik;
  ik = iks_new ("category");
  iks_insert_attrib (ik, "term", category->term);
  if (category->label)
    iks_insert_attrib (ik, "label", category->label);
  if (category->scheme)
    iks_insert_attrib (ik, "scheme", category->scheme);
  return ik;
}

char *
t_atom_category_to_string (TAtomCategory  *category)
{
  iks *ik = t_atom_category_to_iks (category);
  return iks_string (iks_stack (ik), ik);
}

const char *
t_atom_category_get_label (TAtomCategory *category)
{
  return (const char *) category->label;
}

void
t_atom_category_set_label (TAtomCategory *category,
                           const char    *label)
{
  if (category->label)
    free (category->label);
  if (label)
    category->label = strdup (label);
  else
    category->label = NULL;
}

const char *
t_atom_category_get_term (TAtomCategory  *category)
{
  return (const char *) category->term;
}

void
t_atom_category_set_term (TAtomCategory *category,
                          const char    *term)
{
  if (category->term)
    free (category->term);
  category->term = strdup (term);
}

const char *
t_atom_category_get_scheme (TAtomCategory *category)
{
  return (const char *) category->scheme;
}

void
t_atom_category_set_scheme (TAtomCategory *category,
                            const char    *scheme)
{
  if (category->scheme)
    free (category->scheme);
  if (scheme)
    category->scheme = strdup (scheme);
  else
    category->scheme = NULL;
}

/* TAtomEntry */

TAtomEntry *
t_atom_entry_new (const char *title)
{
  TAtomEntry *entry;
  entry = malloc (sizeof (TAtomEntry));
  entry->title = strdup (title);
  entry->id = NULL;
  entry->updated = time (0);
  entry->authors = NULL;
  entry->categories = NULL;
  entry->summary = NULL;
  entry->content = NULL;
  return entry;
}

void
t_atom_entry_free (TAtomEntry *entry)
{
  if (entry->title)
    free (entry->title);
  if (entry->id)
    free (entry->id);
  if (entry->authors)
    t_atom_entry_del_authors (entry);
  if (entry->categories)
    t_atom_entry_del_categories (entry);
  if (entry->summary)
    free (entry->summary);
  if (entry->content)
    t_atom_content_free (entry->content);
  free (entry);
}

static char *
time_to_iso8601 (time_t t)
{
  GTimeVal gtv;
  gtv.tv_sec = t;
  gtv.tv_usec = 0;
  return g_time_val_to_iso8601 (&gtv);
}

iks *
t_atom_entry_to_iks (TAtomEntry *entry)
{
  iks *ik;
  char *updated;
  int i;
  updated = time_to_iso8601 (entry->updated);
  ik = iks_new ("entry");
  iks_insert_attrib (ik, "xmlns", J_ATOM_NS);
  iks_insert_cdata (iks_insert (ik, "title"), entry->title, 0);
  iks_insert_cdata (iks_insert (ik, "updated"), updated, 0);
  free (updated);
  if (entry->authors)
    for (i = 0; i < entry->authors->len; i++)
      {
        iks *authors =
          t_atom_author_to_iks (g_ptr_array_index (entry->authors, i));
        iks_insert_node (ik, authors);
      }
  if (entry->categories)
    for (i = 0; i < entry->categories->len; i++)
      {
        iks *categories =
          t_atom_category_to_iks (g_ptr_array_index (entry->categories, i));
        iks_insert_node (ik, categories);
      }
  if (entry->summary)
    iks_insert_cdata (iks_insert (ik, "summary"), entry->summary, 0);
  if (entry->content)
    iks_insert_node (ik, t_atom_content_to_iks (entry->content));
  return ik;
}

char *
t_atom_entry_to_string (TAtomEntry *entry)
{
  iks *ik = t_atom_entry_to_iks (entry);
  return iks_string (iks_stack (ik), ik);
}

const char *
t_atom_entry_get_title (TAtomEntry *entry)
{
  return (const char *) entry->title;
}

void
t_atom_entry_set_title (TAtomEntry *entry,
                        const char *title)
{
  if (entry->title)
    free (entry->title);
  entry->title = strdup (title);
}

time_t
t_atom_entry_get_updated (TAtomEntry *entry)
{
  return entry->updated;
}

void
t_atom_entry_set_updated (TAtomEntry *entry,
                          time_t      updated)
{
  entry->updated = updated;
}

void
t_atom_entry_get_authors (TAtomEntry    *entry,
                          TAtomAuthor ***authors,
                          int           *len)
{
  if (len)
    *len = entry->authors->len;
  if (authors)
    *authors = (TAtomAuthor **) entry->authors->pdata;
}

void
t_atom_entry_add_author (TAtomEntry  *entry,
                         TAtomAuthor *author)
{
  if (entry->authors == NULL)
    entry->authors = g_ptr_array_new ();
  g_ptr_array_add (entry->authors, author);
}

void
t_atom_entry_del_authors (TAtomEntry *entry)
{
  int i;
  for (i = 0; i < entry->authors->len; i++)
    t_atom_author_free (g_ptr_array_index (entry->authors, i));
  g_ptr_array_free (entry->authors, TRUE);
}

void
t_atom_entry_get_categories (TAtomEntry      *entry,
                             TAtomCategory ***categories,
                             int             *len)
{
  if (len)
    *len = entry->categories->len;
  if (categories)
    *categories = (TAtomCategory **) entry->categories->pdata;
}

void
t_atom_entry_add_category (TAtomEntry    *entry,
                           TAtomCategory *category)
{
  if (entry->categories == NULL)
    entry->categories = g_ptr_array_new ();
  g_ptr_array_add (entry->categories, category);
}

void
t_atom_entry_del_categories (TAtomEntry *entry)
{
  int i;
  for (i = 0; i < entry->categories->len; i++)
    t_atom_category_free (g_ptr_array_index (entry->categories, i));
  g_ptr_array_free (entry->categories, TRUE);
}

const char *
t_atom_entry_get_summary (TAtomEntry *entry)
{
  return (const char *) entry->summary;
}

void
t_atom_entry_set_summary (TAtomEntry *entry,
                          const char *summary)
{
  if (entry->summary)
    free (entry->summary);
  if (summary)
    entry->summary = strdup (summary);
  else
    entry->summary = NULL;
}

TAtomContent *
t_atom_entry_get_content (TAtomEntry *entry)
{
  return entry->content;
}

void
t_atom_entry_set_content (TAtomEntry   *entry,
                          TAtomContent *content)
{
  if (entry->content)
    t_atom_content_free (entry->content);
  entry->content = content;
}
