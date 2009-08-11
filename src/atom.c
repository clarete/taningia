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
#include <jarvis/atom.h>

struct _JAtomContent
{
  char *type;
  char *content;
  char *src;
  int   len;
};

struct _JAtomAuthor
{
  char *name;
  char *email;
  char *iri;
};

struct _JAtomCategory
{
  char *label;
  char *term;
  char *scheme;
};

struct _JAtomEntry
{
  char         *id;
  char         *title;
  time_t        updated;
  GPtrArray    *authors;
  GPtrArray    *categories;
  char         *summary;
  JAtomContent *content;
};

/* JAtomContent */

JAtomContent *
j_atom_content_new (const char *type,
                    const char *content,
                    int         len)
{
  JAtomContent *ct;
  ct = malloc (sizeof (JAtomContent));
  ct->type = strdup (type);
  ct->content = strndup (content, len);
  ct->src = NULL;
  ct->len = len;
  return ct;
}

void
j_atom_content_free (JAtomContent *content)
{
  if (content->type)
    free (content->type);
  if (content->content)
    free (content->content);
  if (content->src)
    free (content->src);
  free (content);
}

iks *
j_atom_content_to_iks (JAtomContent *content)
{
  iks *ct;
  ct = iks_new ("content");
  iks_insert_attrib (ct, "type", content->type);
  if (content->src)
    iks_insert_attrib (ct, "type", content->type);
  else if (content->content)
    iks_insert_cdata (ct, content->content, content->len);
  return ct;
}

char *
j_atom_content_to_string (JAtomContent *content)
{
  iks *ik = j_atom_content_to_iks (content);
  return iks_string (iks_stack (ik), ik);
}

const char *
j_atom_content_get_src (JAtomContent *content)
{
  return content->src;
}

void
j_atom_content_set_src (JAtomContent *content,
                        const char   *src)
{
  if (content->src)
    free (content->src);
  if (src)
    content->src = strdup (src);
  else
    content->src = NULL;
}

/* JAtomAuthor */

JAtomAuthor *
j_atom_author_new (const char *name,
                   const char *email,
                   const char *iri)
{
  JAtomAuthor *author;
  author = malloc (sizeof (JAtomAuthor));
  author->name = strdup (name);
  author->email = email ? strdup (email) : NULL;
  author->iri = iri ? strdup (iri) : NULL;
  return author;
}

void
j_atom_author_free (JAtomAuthor *author)
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
j_atom_author_to_iks (JAtomAuthor *author)
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
j_atom_author_to_string (JAtomAuthor *author)
{
  iks *ik = j_atom_author_to_iks (author);
  return iks_string (iks_stack (ik), ik);
}

const char *
j_atom_author_get_name (JAtomAuthor *author)
{
  return (const char *) author->name;
}

void
j_atom_author_set_name (JAtomAuthor *author,
                        const char  *name)
{
  if (author->name)
    free (author->name);
  author->name = strdup (name);
}

const char *
j_atom_author_get_email (JAtomAuthor *author)
{
  return (const char *) author->email;
}

void
j_atom_author_set_email (JAtomAuthor *author,
                         const char  *email)
{
  if (author->email)
    free (author->email);
  author->email = strdup (email);
}

const char *
j_atom_author_get_iri (JAtomAuthor *author)
{
  return (const char *) author->iri;
}

void
j_atom_author_set_iri (JAtomAuthor *author,
                       const char  *iri)
{
  if (author->iri)
    free (author->iri);
  author->iri = strdup (iri);
}

/* JAtomCategory */

JAtomCategory *
j_atom_category_new (const char *term,
                     const char *label,
                     const char *scheme)
{
  JAtomCategory *cat;
  cat = malloc (sizeof (JAtomCategory));
  cat->term = strdup (term);
  cat->label = label ? strdup (label) : NULL;
  cat->scheme = scheme ? strdup (scheme) : NULL;
  return cat;
}

void
j_atom_category_free (JAtomCategory  *category)
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
j_atom_category_to_iks (JAtomCategory *category)
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
j_atom_category_to_string (JAtomCategory  *category)
{
  iks *ik = j_atom_category_to_iks (category);
  return iks_string (iks_stack (ik), ik);
}

const char *
j_atom_category_get_label (JAtomCategory *category)
{
  return (const char *) category->label;
}

void
j_atom_category_set_label (JAtomCategory *category,
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
j_atom_category_get_term (JAtomCategory  *category)
{
  return (const char *) category->term;
}

void
j_atom_category_set_term (JAtomCategory *category,
                          const char    *term)
{
  if (category->term)
    free (category->term);
  category->term = strdup (term);
}

const char *
j_atom_category_get_scheme (JAtomCategory *category)
{
  return (const char *) category->scheme;
}

void
j_atom_category_set_scheme (JAtomCategory *category,
                            const char    *scheme)
{
  if (category->scheme)
    free (category->scheme);
  if (scheme)
    category->scheme = strdup (scheme);
  else
    category->scheme = NULL;
}

/* JAtomEntry */

JAtomEntry *
j_atom_entry_new (const char *title)
{
  JAtomEntry *entry;
  entry = malloc (sizeof (JAtomEntry));
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
j_atom_entry_free (JAtomEntry *entry)
{
  if (entry->title)
    free (entry->title);
  if (entry->id)
    free (entry->id);
  if (entry->authors)
    j_atom_entry_del_authors (entry);
  if (entry->categories)
    j_atom_entry_del_categories (entry);
  if (entry->summary)
    free (entry->summary);
  if (entry->content)
    j_atom_content_free (entry->content);
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
j_atom_entry_to_iks (JAtomEntry *entry)
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
          j_atom_author_to_iks (g_ptr_array_index (entry->authors, i));
        iks_insert_node (ik, authors);
      }
  if (entry->categories)
    for (i = 0; i < entry->categories->len; i++)
      {
        iks *categories =
          j_atom_category_to_iks (g_ptr_array_index (entry->categories, i));
        iks_insert_node (ik, categories);
      }
  if (entry->summary)
    iks_insert_cdata (iks_insert (ik, "summary"), entry->summary, 0);
  if (entry->content)
    iks_insert_node (ik, j_atom_content_to_iks (entry->content));
  return ik;
}

char *
j_atom_entry_to_string (JAtomEntry *entry)
{
  iks *ik = j_atom_entry_to_iks (entry);
  return iks_string (iks_stack (ik), ik);
}

const char *
j_atom_entry_get_title (JAtomEntry *entry)
{
  return (const char *) entry->title;
}

void
j_atom_entry_set_title (JAtomEntry *entry,
                        const char *title)
{
  if (entry->title)
    free (entry->title);
  entry->title = strdup (title);
}

time_t
j_atom_entry_get_updated (JAtomEntry *entry)
{
  return entry->updated;
}

void
j_atom_entry_set_updated (JAtomEntry *entry,
                          time_t      updated)
{
  entry->updated = updated;
}

void
j_atom_entry_get_authors (JAtomEntry    *entry,
                          JAtomAuthor ***authors,
                          int           *len)
{
  if (len)
    *len = entry->authors->len;
  if (authors)
    *authors = (JAtomAuthor **) entry->authors->pdata;
}

void
j_atom_entry_add_author (JAtomEntry  *entry,
                         JAtomAuthor *author)
{
  if (entry->authors == NULL)
    entry->authors = g_ptr_array_new ();
  g_ptr_array_add (entry->authors, author);
}

void
j_atom_entry_del_authors (JAtomEntry *entry)
{
  int i;
  for (i = 0; i < entry->authors->len; i++)
    j_atom_author_free (g_ptr_array_index (entry->authors, i));
  g_ptr_array_free (entry->authors, TRUE);
}

void
j_atom_entry_get_categories (JAtomEntry      *entry,
                             JAtomCategory ***categories,
                             int             *len)
{
  if (len)
    *len = entry->categories->len;
  if (categories)
    *categories = (JAtomCategory **) entry->categories->pdata;
}

void
j_atom_entry_add_category (JAtomEntry    *entry,
                           JAtomCategory *category)
{
  if (entry->categories == NULL)
    entry->categories = g_ptr_array_new ();
  g_ptr_array_add (entry->categories, category);
}

void
j_atom_entry_del_categories (JAtomEntry *entry)
{
  int i;
  for (i = 0; i < entry->categories->len; i++)
    j_atom_category_free (g_ptr_array_index (entry->categories, i));
  g_ptr_array_free (entry->categories, TRUE);
}

const char *
j_atom_entry_get_summary (JAtomEntry *entry)
{
  return (const char *) entry->summary;
}

void
j_atom_entry_set_summary (JAtomEntry *entry,
                          const char *summary)
{
  if (entry->summary)
    free (entry->summary);
  if (summary)
    entry->summary = strdup (summary);
  else
    entry->summary = NULL;
}

JAtomContent *
j_atom_entry_get_content (JAtomEntry *entry)
{
  return entry->content;
}

void
j_atom_entry_set_content (JAtomEntry   *entry,
                          JAtomContent *content)
{
  if (entry->content)
    j_atom_content_free (entry->content);
  entry->content = content;
}
