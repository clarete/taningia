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

struct _TAtomPerson
{
  char *name;
  char *email;
  TIri *iri;
};

struct _TAtomCategory
{
  char *label;
  char *term;
  TIri *scheme;
};

struct _TAtomEntry
{
  TIri         *id;
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
  ct->type = strdup (type ? type : "text");
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


/* TAtomPerson */

TAtomPerson *
t_atom_person_new (const char *name,
                   const char *email,
                   TIri       *iri)
{
  TAtomPerson *person;
  person = malloc (sizeof (TAtomPerson));
  person->name = strdup (name);
  person->email = email ? strdup (email) : NULL;
  person->iri = iri;
  return person;
}

void
t_atom_person_free (TAtomPerson *person)
{
  if (person->name)
    free (person->name);
  if (person->email)
    free (person->email);
  if (person->iri)
    t_iri_free (person->iri);
  free (person);
}

iks *
t_atom_person_to_iks (TAtomPerson *person, const char *element)
{
  iks *ik;
  ik = iks_new (element);
  iks_insert_cdata (iks_insert (ik, "name"), person->name, 0);
  if (person->email)
    iks_insert_cdata (iks_insert (ik, "email"), person->email, 0);
  if (person->iri)
    {
      char *iri;
      iri = t_iri_to_string (person->iri);
      iks_insert_cdata (iks_insert (ik, "uri"), iri, 0);
      free (iri);
    }
  return ik;
}

char *
t_atom_person_to_string (TAtomPerson *person, const char *element)
{
  iks *ik = t_atom_person_to_iks (person, element);
  return iks_string (iks_stack (ik), ik);
}

const char *
t_atom_person_get_name (TAtomPerson *person)
{
  return person->name;
}

void
t_atom_person_set_name (TAtomPerson *person,
                        const char  *name)
{
  if (person->name)
    free (person->name);
  person->name = strdup (name);
}

const char *
t_atom_person_get_email (TAtomPerson *person)
{
  return person->email;
}

void
t_atom_person_set_email (TAtomPerson *person,
                         const char  *email)
{
  if (person->email)
    free (person->email);
  person->email = strdup (email);
}

TIri *
t_atom_person_get_iri (TAtomPerson *person)
{
  return person->iri;
}

void
t_atom_person_set_iri (TAtomPerson *person,
                       TIri        *iri)
{
  if (person->iri)
    t_iri_free (person->iri);
  person->iri = iri;
}

/* TAtomCategory */

TAtomCategory *
t_atom_category_new (const char *term,
                     const char *label,
                     TIri *scheme)
{
  TAtomCategory *cat;
  cat = malloc (sizeof (TAtomCategory));
  cat->term = strdup (term);
  cat->label = label ? strdup (label) : NULL;
  cat->scheme = scheme ? scheme : NULL;
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
    t_iri_free (category->scheme);
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
    {
      char *iri;
      iri = t_iri_to_string (category->scheme);
      iks_insert_attrib (ik, "scheme", iri);
    }
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

TIri *
t_atom_category_get_scheme (TAtomCategory *category)
{
  return category->scheme;
}

void
t_atom_category_set_scheme (TAtomCategory *category,
                            TIri          *scheme)
{
  if (category->scheme)
    t_iri_free (category->scheme);
  if (scheme)
    category->scheme = scheme;
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
    t_iri_free (entry->id);
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
  char *updated, *id_iri;
  int i;

  if (entry->id == NULL)
    return NULL;

  updated = time_to_iso8601 (entry->updated);
  id_iri = t_iri_to_string (entry->id);

  ik = iks_new ("entry");
  iks_insert_attrib (ik, "xmlns", J_ATOM_NS);
  iks_insert_cdata (iks_insert (ik, "id"), id_iri, 0);
  iks_insert_cdata (iks_insert (ik, "title"), entry->title, 0);
  iks_insert_cdata (iks_insert (ik, "updated"), updated, 0);
  free (updated);
  free (id_iri);
  if (entry->authors)
    for (i = 0; i < entry->authors->len; i++)
      {
        iks *authors =
          t_atom_person_to_iks (g_ptr_array_index (entry->authors, i),
                                "author");
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
  if (ik)
    return iks_string (iks_stack (ik), ik);
  return NULL;
}

int
t_atom_entry_to_file (TAtomEntry *entry,
                      const char *fname)
{
  return iks_save (fname, t_atom_entry_to_iks (entry));
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

TIri *
t_atom_entry_get_id (TAtomEntry *entry)
{
  return entry->id;
}

void
t_atom_entry_set_id (TAtomEntry *entry, TIri *id)
{
  if (entry->id)
    t_iri_free (entry->id);
  entry->id = id;
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
                          TAtomPerson ***authors,
                          int           *len)
{
  if (len)
    *len = entry->authors->len;
  if (authors)
    *authors = (TAtomPerson **) entry->authors->pdata;
}

void
t_atom_entry_add_author (TAtomEntry  *entry,
                         TAtomPerson *author)
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
    t_atom_person_free (g_ptr_array_index (entry->authors, i));
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
