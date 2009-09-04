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
#define _XOPEN_SOURCE       /* glibc 2.0 needs this to use strptime */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iksemel.h>
#include <glib.h>               /* to g_time_val_to_iso8601 */
#include <taningia/atom.h>
#include <taningia/iri.h>

struct _TAtomLink
{
  TIri *href;
  char *title;
  char *rel;
  char *type;
  char *length;
};

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
  time_t        published;
  char         *rights;
  GPtrArray    *authors;
  GPtrArray    *categories;
  GPtrArray    *links;
  char         *summary;
  TAtomContent *content;
};

struct _TAtomFeed
{
  TIri         *id;
  char         *title;
  time_t        updated;
  GPtrArray    *authors;
  GPtrArray    *categories;
  GPtrArray    *entries;
  GPtrArray    *links;
};

/* Forward prototypes */

static char   *time_to_iso8601   (time_t t);

static time_t  iso8601_to_time   (const char *dt);

/* TAtomLink */

TAtomLink *
t_atom_link_new (TIri *href)
{
  TAtomLink *lnk;
  lnk = malloc (sizeof (TAtomLink));
  lnk->href = href;
  lnk->title = NULL;
  lnk->rel = NULL;
  lnk->type = NULL;
  lnk->length = NULL;
  return lnk;
}

void
t_atom_link_free (TAtomLink *link)
{
  if (link->href)
    t_iri_free (link->href);
  if (link->title)
    free (link->title);
  if (link->rel)
    free (link->rel);
  if (link->type)
    free (link->type);
  if (link->length)
    free (link->length);
  free (link);
}

iks *
t_atom_link_to_iks (TAtomLink *link)
{
  iks *lnk;
  char *href;
  lnk = iks_new ("link");
  href = t_iri_to_string (link->href);
  iks_insert_attrib (lnk, "href", href);
  free (href);
  if (link->rel)
    iks_insert_attrib (lnk, "rel", link->rel);
  if (link->title)
    iks_insert_attrib (lnk, "title", link->title);
  if (link->type)
    iks_insert_attrib (lnk, "type", link->type);
  if (link->length)
    iks_insert_attrib (lnk, "length", link->length);
  return lnk;
}

char *
t_atom_link_to_string (TAtomLink *link)
{
  iks *ik = t_atom_link_to_iks (link);
  return iks_string (iks_stack (ik), ik);
}

TIri *
t_atom_link_get_href (TAtomLink *link)
{
  return link->href;
}

void
t_atom_link_set_href (TAtomLink *link,
                      TIri      *href)
{
  if (link->href)
    t_iri_free (link->href);
  link->href = href;
}

const char *
t_atom_link_get_title (TAtomLink *link)
{
  return link->title;
}

void
t_atom_link_set_title (TAtomLink  *link,
                       const char *title)
{
  if (link->title)
    free (link->title);
  link->title = strdup (title);
}

const char *
t_atom_link_get_rel (TAtomLink *link)
{
  return link->rel;
}

void
t_atom_link_set_rel (TAtomLink  *link,
                     const char *rel)
{
  if (link->rel)
    free (link->rel);
  link->rel = strdup (rel);
}

const char *
t_atom_link_get_type (TAtomLink *link)
{
  return link->type;
}

void
t_atom_link_set_type (TAtomLink  *link,
                      const char *type)
{
  if (link->type)
    free (link->type);
  link->type = strdup (type);
}

const char *
t_atom_link_get_length (TAtomLink *link)
{
  return link->length;
}

void
t_atom_link_set_length (TAtomLink  *link,
                        const char *length)
{
  if (link->length)
    free (link->length);
  link->length = strdup (length);
}

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
    {
      /* Section 4.1.3 of the RFC is clear, if the content type is
       * "text/xhtml" an xhtml div element should be created */
      if (!strcmp (content->type, "xhtml"))
        {
          iks *div = iks_insert (ct, "div");
          iks_insert_attrib (div, "xmlns", "http://www.w3.org/1999/xhtml");
          iks_insert_cdata (div, content->content, content->len);
        }
      else
        {
          iks_insert_cdata (ct, content->content, content->len);
        }
    }
  return ct;
}

char *
t_atom_content_to_string (TAtomContent *content)
{
  iks *ik = t_atom_content_to_iks (content);
  return iks_string (iks_stack (ik), ik);
}

const char *
t_atom_content_get_type (TAtomContent *content)
{
  return content->type;
}

void
t_atom_content_set_type (TAtomContent *content,
                         const char   *type)
{
  if (content->type)
    free (content->type);
  content->type = strdup (type);
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
  entry->title = title ? strdup (title) : NULL;
  entry->id = NULL;
  entry->updated = time (0);
  entry->published = 0;
  entry->rights = NULL;
  entry->authors = NULL;
  entry->categories = NULL;
  entry->links = NULL;
  entry->summary = NULL;
  entry->content = NULL;
  return entry;
}

int
t_atom_entry_set_from_file (TAtomEntry *entry,
                            const char *fname)
{
  iks *ik;
  int err;
  if ((err = iks_load (fname, &ik)) != IKS_OK)
    return 0;
  return t_atom_entry_set_from_iks (entry, ik);
}

int
t_atom_entry_set_from_iks (TAtomEntry *entry,
                           iks        *ik)
{
  TIri *eid;
  iks *child;
  char *id, *title, *updated, *published, *summary, *rights;

  if (strcmp (iks_name (ik), "entry") ||
      !iks_has_children (ik))
    {
      iks_delete (ik);
      printf ("Wrong root entry element\n");
      return 0;
    }
  id = iks_find_cdata (ik, "id");
  if (!id)
    {
      printf ("No id\n");
      return 0;
    }
  eid = t_iri_new (id);
  if (!eid)
    {
      printf ("Invalid id iri\n");
      return 0;
    }
  title = iks_find_cdata (ik, "title");
  if (!title)
    {
      printf ("No title\n");
      return 0;
    }

  /* Here we have almost all required fields */
  t_atom_entry_set_id (entry, eid);
  t_atom_entry_set_title (entry, title);

  updated = iks_find_cdata (ik, "updated");
  if (updated)
    {
      time_t updatedt;
      updatedt = iso8601_to_time (updated);
      t_atom_entry_set_updated (entry, updatedt);
    }

  published = iks_find_cdata (ik, "published");
  if (published)
    {
      time_t publishedt;
      publishedt = iso8601_to_time (published);
      t_atom_entry_set_published (entry, publishedt);
    }

  summary = iks_find_cdata (ik, "summary");
  if (summary)
    t_atom_entry_set_summary (entry, summary);

  rights = iks_find_cdata (ik, "rights");
  if (rights)
    t_atom_entry_set_rights (entry, rights);

  /* Looking for more structured data */
  for (child = iks_child (ik); child; child = iks_next (child))
    {
      if (!strcmp (iks_name (child), "author"))
        {
          char *name, *email = NULL, *uri = NULL;
          TIri *iri = NULL;
          TAtomPerson *author;
          name = iks_find_cdata (child, "name");

          /* Specification is clear, an atom:author element *MUST*
           * have a name. */
          if (!name)
            {
              printf ("Author with no name\n");
              return 0;
            }
          email = iks_find_cdata (child, "email");
          uri = iks_find_cdata (child, "uri");
          if (uri)
            iri = t_iri_new (uri);

          /* Like above, specification denies invalid iris in an atom
           * person. */
          if (uri && !iri)
            {
              printf ("Author with an invalid iri in uri field\n");
              return 0;
            }
          author = t_atom_person_new (name, email, iri);
          t_atom_entry_add_author (entry, author);
        }
      if (!strcmp (iks_name (child), "category"))
        {
          TAtomCategory *cat;
          TIri *iri = NULL;
          char *term = NULL, *label = NULL, *scheme = NULL;
          term = iks_find_attrib (child, "term");
          label = iks_find_attrib (child, "label");
          scheme = iks_find_attrib (child, "scheme");
          if (!term)
            {
              printf ("Category with no term attribute\n");
              return 0;
            }
          if (scheme)
            {
              iri = t_iri_new (scheme);
              if (!iri)
                {
                  printf ("Category scheme attribute is not a valid iri");
                  return 0;
                }
            }
          cat = t_atom_category_new (term, label, iri);
          t_atom_entry_add_category (entry, cat);
        }
      if (!strcmp (iks_name (child), "content"))
        {
          TAtomContent *ct;
          char *type = NULL, *src = NULL, *content = NULL;
          int content_len = 0;
          type = iks_find_attrib (child, "type");
          src = iks_find_attrib (child, "src");
          content = iks_cdata (iks_child (child));
          if (!type)
            {
              printf ("No type attribute specified for content");
              return 0;
            }

          /* When content is filled, entry content should have no src
           * attribute */
          if (src && content)
            {
              printf ("Invalid content, it has src "
                      "attribute and content is filled\n");
              return 0;
            }
          if (!src && !content)
            {
              printf ("No src attribute or content in content tag");
              return 0;
            }
          if (content)
            content_len = iks_cdata_size (iks_child (child));
          ct = t_atom_content_new (type, content, content_len);
          if (src)
            {
              TIri *srci;
              srci = t_iri_new (src);
              if (!srci)
                {
                  printf ("Invalid iri in content src attribute");
                  t_atom_content_free (ct);
                  return 0;
                }
              t_atom_content_set_src (ct, srci);
            }
          if (content)
            t_atom_content_set_content (ct, content, content_len);
          t_atom_entry_set_content (entry, ct);
        }
    }
  return 1;
}

void
t_atom_entry_free (TAtomEntry *entry)
{
  if (entry->title)
    free (entry->title);
  if (entry->id)
    t_iri_free (entry->id);
  if (entry->rights)
    free (entry->rights);
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

static time_t
iso8601_to_time (const char *dt)
{
  struct tm tm;
  strptime (dt, "%Y-%m-%dT%H:%M:%SZ", &tm);
  return mktime (&tm);
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

  /* Not required fields */
  if (entry->published)
    {
      char *published;
      published = time_to_iso8601 (entry->published);
      iks_insert_cdata (iks_insert (ik, "published"), published, 0);
    }

  if (entry->rights)
    iks_insert_cdata (iks_insert (ik, "rights"), entry->rights, 0);

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
  if (entry->links)
    for (i = 0; i < entry->links->len; i++)
      {
        iks *links =
          t_atom_link_to_iks (g_ptr_array_index (entry->links, i));
        iks_insert_node (ik, links);
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
    return strdup (iks_string (iks_stack (ik), ik));
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

time_t
t_atom_entry_get_published (TAtomEntry *entry)
{
  return entry->published;
}

void
t_atom_entry_set_published (TAtomEntry *entry,
                            time_t      published)
{
  entry->published = published;
}

const char *
t_atom_entry_get_rights (TAtomEntry *entry)
{
  return entry->rights;
}

void
t_atom_entry_set_rights (TAtomEntry *entry,
                         const char *rights)
{
  if (entry->rights)
    free (entry->rights);
  entry->rights = strdup (rights);
}

void
t_atom_entry_get_authors (TAtomEntry    *entry,
                          TAtomPerson ***authors,
                          int           *len)
{
  if (entry->authors)
    {
      *len = entry->authors->len;
      *authors = (TAtomPerson **) entry->authors->pdata;
    }
  else
    {
      *len = 0;
      *authors = NULL;
    }
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
  if (entry->categories)
    {
      *len = entry->categories->len;
      *categories = (TAtomCategory **) entry->categories->pdata;
    }
  else
    {
      *len = 0;
      *categories = NULL;
    }
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

void
t_atom_entry_get_links (TAtomEntry  *entry,
                        TAtomLink ***links,
                        int         *len)
{
  if (entry->links)
    {
      *len = entry->links->len;
      *links = (TAtomLink **) entry->links->pdata;
    }
  else
    {
      *len = 0;
      *links = NULL;
    }
}

void
t_atom_entry_add_link (TAtomEntry *entry,
                       TAtomLink  *link)
{
  if (entry->links == NULL)
    entry->links = g_ptr_array_new ();
  g_ptr_array_add (entry->links, link);
}

void
t_atom_entry_del_links (TAtomEntry *entry)
{
  int i;
  for (i = 0; i < entry->links->len; i++)
    t_atom_link_free (g_ptr_array_index (entry->links, i));
  g_ptr_array_free (entry->links, TRUE);
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

/* TAtomFeed */

TAtomFeed *
t_atom_feed_new (const char *title)
{
  TAtomFeed *feed;
  feed = malloc (sizeof (TAtomFeed));
  feed->title = title ? strdup (title) : NULL;
  feed->id = NULL;
  feed->updated = time (0);
  feed->authors = NULL;
  feed->categories = NULL;
  feed->entries = NULL;
  return feed;
}

int
t_atom_feed_set_from_file (TAtomFeed  *feed,
                           const char *fname)
{
  TIri *eid;
  iks *ik, *child;
  char *id, *title, *updated;
  int err;

  if ((err = iks_load (fname, &ik)) != IKS_OK)
    return 0;
  if (strcmp (iks_name (ik), "feed") ||
      !iks_has_children (ik))
    {
      iks_delete (ik);
      printf ("Wrong root feed element\n");
      return 0;
    }
  id = iks_find_cdata (ik, "id");
  if (!id)
    {
      printf ("No id\n");
      return 0;
    }
  eid = t_iri_new (id);
  if (!eid)
    {
      printf ("Invalid id iri\n");
      return 0;
    }
  title = iks_find_cdata (ik, "title");
  if (!title)
    {
      printf ("No title\n");
      return 0;
    }

  /* Here we have almost all required fields */
  t_atom_feed_set_id (feed, eid);
  t_atom_feed_set_title (feed, title);

  updated = iks_find_cdata (ik, "updated");
  if (updated)
    {
      time_t updatedt;
      updatedt = iso8601_to_time (updated);
      t_atom_feed_set_updated (feed, updatedt);
    }

  /* Looking for more structured data */
  for (child = iks_child (ik); child; child = iks_next (child))
    {
      if (!strcmp (iks_name (child), "author"))
        {
          char *name, *email = NULL, *uri = NULL;
          TIri *iri = NULL;
          TAtomPerson *author;
          name = iks_find_cdata (child, "name");

          /* Specification is clear, an atom:author element *MUST*
           * have a name. */
          if (!name)
            {
              printf ("Author with no name\n");
              return 0;
            }
          email = iks_find_cdata (child, "email");
          uri = iks_find_cdata (child, "uri");
          if (uri)
            iri = t_iri_new (uri);

          /* Like above, specification denies invalid iris in an atom
           * person. */
          if (uri && !iri)
            {
              printf ("Author with an invalid iri in uri field\n");
              return 0;
            }
          author = t_atom_person_new (name, email, iri);
          t_atom_feed_add_author (feed, author);
        }
      if (!strcmp (iks_name (child), "category"))
        {
          TAtomCategory *cat;
          TIri *iri = NULL;
          char *term = NULL, *label = NULL, *scheme = NULL;
          term = iks_find_attrib (child, "term");
          label = iks_find_attrib (child, "label");
          scheme = iks_find_attrib (child, "scheme");
          if (!term)
            {
              printf ("Category with no term attribute\n");
              return 0;
            }
          if (scheme)
            {
              iri = t_iri_new (scheme);
              if (!iri)
                {
                  printf ("Category scheme attribute is not a valid iri");
                  return 0;
                }
            }
          cat = t_atom_category_new (term, label, iri);
          t_atom_feed_add_category (feed, cat);
        }
    }
  return 1;
}

void
t_atom_feed_free (TAtomFeed *feed)
{
  if (feed->id)
    t_iri_free (feed->id);
  if (feed->title)
    free (feed->title);
  if (feed->authors)
    t_atom_feed_del_authors (feed);
  if (feed->categories)
    t_atom_feed_del_categories (feed);
  if (feed->entries)
    t_atom_feed_del_entries (feed);
  free (feed);
}

iks *
t_atom_feed_to_iks (TAtomFeed *feed)
{
  iks *ik;
  char *updated, *id_iri;
  int i;

  if (feed->id == NULL)
    return NULL;

  updated = time_to_iso8601 (feed->updated);
  id_iri = t_iri_to_string (feed->id);

  ik = iks_new ("feed");
  iks_insert_attrib (ik, "xmlns", J_ATOM_NS);
  iks_insert_cdata (iks_insert (ik, "id"), id_iri, 0);
  iks_insert_cdata (iks_insert (ik, "title"), feed->title, 0);
  iks_insert_cdata (iks_insert (ik, "updated"), updated, 0);
  free (updated);
  free (id_iri);
  if (feed->authors)
    for (i = 0; i < feed->authors->len; i++)
      {
        iks *authors =
          t_atom_person_to_iks (g_ptr_array_index (feed->authors, i),
                                "author");
        iks_insert_node (ik, authors);
      }
  if (feed->categories)
    for (i = 0; i < feed->categories->len; i++)
      {
        iks *categories =
          t_atom_category_to_iks (g_ptr_array_index (feed->categories, i));
        iks_insert_node (ik, categories);
      }
  if (feed->entries)
    for (i = 0; i < feed->entries->len; i++)
      {
        iks *entries =
          t_atom_entry_to_iks (g_ptr_array_index (feed->entries, i));
        iks_insert_node (ik, entries);
      }
  return ik;
}

char *
t_atom_feed_to_string (TAtomFeed *feed)
{
  iks *ik = t_atom_feed_to_iks (feed);
  if (ik)
    return strdup (iks_string (iks_stack (ik), ik));
  return NULL;
}

int
t_atom_feed_to_file (TAtomFeed  *feed,
                     const char *fname)
{
  return iks_save (fname, t_atom_feed_to_iks (feed));
}

const char *
t_atom_feed_get_title (TAtomFeed *feed)
{
  return feed->title;
}

void
t_atom_feed_set_title (TAtomFeed  *feed,
                       const char *title)
{
  if (feed->title)
    free (feed->title);
  feed->title = strdup (title);
}

TIri *
t_atom_feed_get_id (TAtomFeed *feed)
{
  return feed->id;
}

void
t_atom_feed_set_id (TAtomFeed *feed, TIri *id)
{
  if (feed->id)
    t_iri_free (feed->id);
  feed->id = id;
}

time_t
t_atom_feed_get_updated (TAtomFeed *feed)
{
  return feed->updated;
}

void
t_atom_feed_set_updated (TAtomFeed *feed,
                         time_t     updated)
{
  feed->updated = updated;
}

void
t_atom_feed_get_authors (TAtomFeed     *feed,
                         TAtomPerson ***authors,
                         int           *len)
{
  if (feed->authors)
    {
      if (len)
        *len = feed->authors->len;
      if (authors)
        *authors = (TAtomPerson **) feed->authors->pdata;
    }
}

void
t_atom_feed_add_author (TAtomFeed   *feed,
                        TAtomPerson *author)
{
  if (feed->authors == NULL)
    feed->authors = g_ptr_array_new ();
  g_ptr_array_add (feed->authors, author);
}

void
t_atom_feed_del_authors (TAtomFeed *feed)
{
  int i;
  for (i = 0; i < feed->authors->len; i++)
    t_atom_person_free (g_ptr_array_index (feed->authors, i));
  g_ptr_array_free (feed->authors, TRUE);
}

void
t_atom_feed_get_categories (TAtomFeed       *feed,
                            TAtomCategory ***categories,
                            int             *len)
{
  if (feed->categories)
    {
      if (len)
        *len = feed->categories->len;
      if (categories)
        *categories = (TAtomCategory **) feed->categories->pdata;
    }
}

void
t_atom_feed_add_category (TAtomFeed     *feed,
                          TAtomCategory *category)
{
  if (feed->categories == NULL)
    feed->categories = g_ptr_array_new ();
  g_ptr_array_add (feed->categories, category);
}

void
t_atom_feed_del_categories (TAtomFeed *feed)
{
  int i;
  for (i = 0; i < feed->categories->len; i++)
    t_atom_category_free (g_ptr_array_index (feed->categories, i));
  g_ptr_array_free (feed->categories, TRUE);
}

void
t_atom_feed_get_links (TAtomFeed   *feed,
                       TAtomLink ***links,
                       int         *len)
{
  if (feed->links)
    {
      *len = feed->links->len;
      *links = (TAtomLink **) feed->links->pdata;
    }
  else
    {
      *len = 0;
      *links = NULL;
    }
}

void
t_atom_feed_add_link (TAtomFeed  *feed,
                      TAtomLink  *link)
{
  if (feed->links == NULL)
    feed->links = g_ptr_array_new ();
  g_ptr_array_add (feed->links, link);
}

void
t_atom_feed_del_links (TAtomFeed *feed)
{
  int i;
  for (i = 0; i < feed->links->len; i++)
    t_atom_link_free (g_ptr_array_index (feed->links, i));
  g_ptr_array_free (feed->links, TRUE);
}

void
t_atom_feed_get_entries (TAtomFeed    *feed,
                         TAtomEntry ***entries,
                         int          *len)
{
  if (feed->entries)
    {
      if (len)
        *len = feed->entries->len;
      if (entries)
        *entries = (TAtomEntry **) feed->entries->pdata;
    }
}

void
t_atom_feed_add_entry (TAtomFeed  *feed,
                       TAtomEntry *entry)
{
  if (feed->entries == NULL)
    feed->entries = g_ptr_array_new ();
  g_ptr_array_add (feed->entries, entry);
}

void
t_atom_feed_del_entries (TAtomFeed *feed)
{
  int i;
  for (i = 0; i < feed->entries->len; i++)
    t_atom_entry_free (g_ptr_array_index (feed->entries, i));
  g_ptr_array_free (feed->entries, TRUE);
}
