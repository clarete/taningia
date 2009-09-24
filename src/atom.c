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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iksemel.h>
#include <glib.h>               /* to g_time_val_to_iso8601 */
#include <taningia/atom.h>
#include <taningia/iri.h>
#include <taningia/error.h>
#include <taningia/list.h>

struct _atom_simple_element_t
{
  char *name;
  char *value;
};

struct _atom_link_t
{
  iri_t *href;
  char *title;
  char *rel;
  char *type;
  char *length;
};

struct _atom_content_t
{
  char *type;
  char *content;
  int   len;
  iri_t *src;
};

struct _atom_person_t
{
  char *name;
  char *email;
  iri_t *iri;
  list_t *ext_elements;
};

struct _atom_category_t
{
  char *label;
  char *term;
  iri_t *scheme;
};

struct _atom_entry_t
{
  iri_t         *id;
  char         *title;
  time_t        updated;
  time_t        published;
  char         *rights;
  list_t        *authors;
  list_t        *categories;
  list_t        *links;
  char         *summary;
  atom_content_t *content;
  error_t       *error;
  list_t        *ext_elements;
};

struct _atom_feed_t
{
  iri_t   *id;
  char   *title;
  time_t  updated;
  list_t  *authors;
  list_t  *categories;
  list_t  *entries;
  list_t  *links;
  error_t *error;
  list_t  *ext_elements;
};

/* Forward prototypes */

static char   *time_to_iso8601   (time_t t);

static time_t  iso8601_to_time   (const char *dt);

/* helper functions */

void
_atom_free_ext_elements (list_t *list)
{
  list_t *tmp;
  for (tmp = list; tmp; tmp = tmp->next)
    atom_simple_element_free (tmp->data);
  list_free (list);
}

/* atom_simple_element_t */

atom_simple_element_t *
atom_simple_element_new (const char *name, const char *value)
{
  atom_simple_element_t *see;
  see = malloc (sizeof (atom_simple_element_t));
  see->name = strdup (name);
  see->value = strdup (value);
  return see;
}

void
atom_simple_element_free (atom_simple_element_t *see)
{
  if (see->name)
    free (see->name);
  if (see->value)
    free (see->value);
  free (see);
}

iks *
atom_simple_element_to_iks (atom_simple_element_t *see)
{
  iks *iksee;
  iksee = iks_new (see->name);
  iks_insert_cdata (iksee, see->value, 0);
  return iksee;
}

char *
atom_simple_element_to_string (atom_simple_element_t *see)
{
  iks *ik = atom_simple_element_to_iks (see);
  return iks_string (iks_stack (ik), ik);
}

const char *
atom_simple_element_get_name (atom_simple_element_t *see)
{
  return see->name;
}

void
atom_simple_element_set_name (atom_simple_element_t *see, const char *name)
{
  if (see->name)
    free (see->name);
  see->name = strdup (name);
}

const char *
atom_simple_element_get_value (atom_simple_element_t *see)
{
  return see->value;
}

void
atom_simple_element_set_value (atom_simple_element_t *see, const char *value)
{
  if (see->value)
    free (see->value);
  see->value = strdup (value);
}

/* atom_link_t */

atom_link_t *
atom_link_new (iri_t *href)
{
  atom_link_t *lnk;
  lnk = malloc (sizeof (atom_link_t));
  lnk->href = href;
  lnk->title = NULL;
  lnk->rel = NULL;
  lnk->type = NULL;
  lnk->length = NULL;
  return lnk;
}

void
atom_link_free (atom_link_t *link)
{
  if (link->href)
    iri_free (link->href);
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
atom_link_to_iks (atom_link_t *link)
{
  iks *lnk;
  char *href;
  lnk = iks_new ("link");
  href = iri_to_string (link->href);
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
atom_link_to_string (atom_link_t *link)
{
  iks *ik = atom_link_to_iks (link);
  return iks_string (iks_stack (ik), ik);
}

iri_t *
atom_link_get_href (atom_link_t *link)
{
  return link->href;
}

void
atom_link_set_href (atom_link_t *link,
                      iri_t      *href)
{
  if (link->href)
    iri_free (link->href);
  link->href = href;
}

const char *
atom_link_get_title (atom_link_t *link)
{
  return link->title;
}

void
atom_link_set_title (atom_link_t  *link,
                       const char *title)
{
  if (link->title)
    free (link->title);
  link->title = strdup (title);
}

const char *
atom_link_get_rel (atom_link_t *link)
{
  return link->rel;
}

void
atom_link_set_rel (atom_link_t  *link,
                     const char *rel)
{
  if (link->rel)
    free (link->rel);
  link->rel = strdup (rel);
}

const char *
atom_link_get_type (atom_link_t *link)
{
  return link->type;
}

void
atom_link_set_type (atom_link_t  *link,
                      const char *type)
{
  if (link->type)
    free (link->type);
  link->type = strdup (type);
}

const char *
atom_link_get_length (atom_link_t *link)
{
  return link->length;
}

void
atom_link_set_length (atom_link_t  *link,
                        const char *length)
{
  if (link->length)
    free (link->length);
  link->length = strdup (length);
}

/* atom_content_t */

atom_content_t *
atom_content_new (const char *type)
{
  atom_content_t *ct;
  ct = malloc (sizeof (atom_content_t));
  ct->type = strdup (type ? type : "text");
  ct->content = NULL;
  ct->src = NULL;
  ct->len = 0;
  return ct;
}

void
atom_content_free (atom_content_t *content)
{
  if (content->type)
    free (content->type);
  if (content->content)
    free (content->content);
  if (content->src)
    iri_free (content->src);
  free (content);
}

iks *
atom_content_to_iks (atom_content_t *content)
{
  iks *ct;
  ct = iks_new ("content");
  iks_insert_attrib (ct, "type", content->type);
  if (content->src != NULL)
    {
      char *iri;
      iri = iri_to_string (content->src);
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
atom_content_to_string (atom_content_t *content)
{
  iks *ik = atom_content_to_iks (content);
  return iks_string (iks_stack (ik), ik);
}

const char *
atom_content_get_type (atom_content_t *content)
{
  return content->type;
}

void
atom_content_set_type (atom_content_t *content,
                         const char   *type)
{
  if (content->type)
    free (content->type);
  content->type = strdup (type);
}

iri_t *
atom_content_get_src (atom_content_t *content)
{
  return content->src;
}

void
atom_content_set_src (atom_content_t *content,
                        iri_t         *src)
{
  if (content->src)
    {
      iri_free (content->src);
      if (src != NULL && content->content)
        {
          free (content->content);
          content->content = NULL;
        }
    }
  content->src = src;
}

const char *
atom_content_get_content (atom_content_t *content,
                            int          *len)
{
  if (len)
    *len = content->len;
  return content->content;
}

void
atom_content_set_content (atom_content_t *content,
                            const char   *text,
                            int           len)
{
  if (content->content)
    {
      free (content->content);
      if (text != NULL && content->src)
        {
          iri_free (content->src);
          content->src = NULL;
        }
    }
  content->content = strdup (text);
}


/* atom_person_t */

atom_person_t *
atom_person_new (const char *name,
                   const char *email,
                   iri_t       *iri)
{
  atom_person_t *person;
  person = malloc (sizeof (atom_person_t));
  person->name = strdup (name);
  person->email = email ? strdup (email) : NULL;
  person->iri = iri;
  person->ext_elements = NULL;
  return person;
}

void
atom_person_free (atom_person_t *person)
{
  if (person->name)
    free (person->name);
  if (person->email)
    free (person->email);
  if (person->iri)
    iri_free (person->iri);
  if (person->ext_elements)
    _atom_free_ext_elements (person->ext_elements);
  free (person);
}

iks *
atom_person_to_iks (atom_person_t *person, const char *element)
{
  iks *ik;
  ik = iks_new (element);
  iks_insert_cdata (iks_insert (ik, "name"), person->name, 0);
  if (person->email)
    iks_insert_cdata (iks_insert (ik, "email"), person->email, 0);
  if (person->iri)
    {
      char *iri;
      iri = iri_to_string (person->iri);
      iks_insert_cdata (iks_insert (ik, "uri"), iri, 0);
      free (iri);
    }
  if (person->ext_elements)
    {
      list_t *tmp;
      for (tmp = person->ext_elements; tmp; tmp = tmp->next)
        {
          iks *ext_elements =
            atom_simple_element_to_iks (tmp->data);
          iks_insert_node (ik, ext_elements);
        }
    }
  return ik;
}

char *
atom_person_to_string (atom_person_t *person, const char *element)
{
  iks *ik = atom_person_to_iks (person, element);
  return strdup (iks_string (iks_stack (ik), ik));
}

const char *
atom_person_get_name (atom_person_t *person)
{
  return person->name;
}

void
atom_person_set_name (atom_person_t *person,
                        const char  *name)
{
  if (person->name)
    free (person->name);
  person->name = strdup (name);
}

const char *
atom_person_get_email (atom_person_t *person)
{
  return person->email;
}

void
atom_person_set_email (atom_person_t *person,
                         const char  *email)
{
  if (person->email)
    free (person->email);
  person->email = strdup (email);
}

iri_t *
atom_person_get_iri (atom_person_t *person)
{
  return person->iri;
}

void
atom_person_set_iri (atom_person_t *person,
                       iri_t        *iri)
{
  if (person->iri)
    iri_free (person->iri);
  person->iri = iri;
}

void
atom_person_add_see (atom_person_t *person, atom_simple_element_t *element)
{
  person->ext_elements = list_append (person->ext_elements, element);
}

void
atom_person_del_see (atom_person_t *person)
{
  _atom_free_ext_elements (person->ext_elements);
  person->ext_elements = NULL;
}

list_t *
atom_person_get_see (atom_person_t *person)
{
  return person->ext_elements;
}


/* atom_category_t */

atom_category_t *
atom_category_new (const char *term, const char *label, iri_t *scheme)
{
  atom_category_t *cat;
  cat = malloc (sizeof (atom_category_t));
  cat->term = strdup (term);
  cat->label = label ? strdup (label) : NULL;
  cat->scheme = scheme ? scheme : NULL;
  return cat;
}

void
atom_category_free (atom_category_t  *category)
{
  if (category->term)
    free (category->term);
  if (category->label)
    free (category->label);
  if (category->scheme)
    iri_free (category->scheme);
  free (category);
}

iks *
atom_category_to_iks (atom_category_t *category)
{
  iks *ik;
  ik = iks_new ("category");
  iks_insert_attrib (ik, "term", category->term);
  if (category->label)
    iks_insert_attrib (ik, "label", category->label);
  if (category->scheme)
    {
      char *iri;
      iri = iri_to_string (category->scheme);
      iks_insert_attrib (ik, "scheme", iri);
    }
  return ik;
}

char *
atom_category_to_string (atom_category_t  *category)
{
  iks *ik = atom_category_to_iks (category);
  return iks_string (iks_stack (ik), ik);
}

const char *
atom_category_get_label (atom_category_t *category)
{
  return (const char *) category->label;
}

void
atom_category_set_label (atom_category_t *category,
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
atom_category_get_term (atom_category_t  *category)
{
  return (const char *) category->term;
}

void
atom_category_set_term (atom_category_t *category,
                          const char    *term)
{
  if (category->term)
    free (category->term);
  category->term = strdup (term);
}

iri_t *
atom_category_get_scheme (atom_category_t *category)
{
  return category->scheme;
}

void
atom_category_set_scheme (atom_category_t *category,
                            iri_t          *scheme)
{
  if (category->scheme)
    iri_free (category->scheme);
  if (scheme)
    category->scheme = scheme;
  else
    category->scheme = NULL;
}

/* atom_entry_t */

atom_entry_t *
atom_entry_new (const char *title)
{
  atom_entry_t *entry;
  entry = malloc (sizeof (atom_entry_t));
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
  entry->error = NULL;
  return entry;
}

int
atom_entry_set_from_file (atom_entry_t *entry,
                            const char *fname)
{
  iks *ik;
  int err;
  if ((err = iks_load (fname, &ik)) != IKS_OK)
    {
      if (entry->error)
        error_free (entry->error);
      entry->error = error_new ();
      switch (err)
        {
        case IKS_NOMEM:
          error_set_full (entry->error, ATOM_LOAD_ERROR, "LoadError",
                            "Not enough memory to load file");
        case IKS_BADXML:
          error_set_full (entry->error, ATOM_LOAD_ERROR, "LoadError",
                            "Unable to parse xml file");
        default:
          error_set_full (entry->error, ATOM_LOAD_ERROR, "LoadError",
                            "Unknown error");
        }
      return 0;
    }
  return atom_entry_set_from_iks (entry, ik);
}

int
atom_entry_set_from_iks (atom_entry_t *entry,
                           iks        *ik)
{
  iri_t *eid;
  iks *child, *content;
  char *id, *title, *updated, *published, *summary, *rights;

  if (strcmp (iks_name (ik), "entry") ||
      !iks_has_children (ik))
    {
      iks_delete (ik);
      if (entry->error)
        error_free (entry->error);
      entry->error = error_new ();
      error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                        "Wrong root entry element");
      return 0;
    }
  id = iks_find_cdata (ik, "id");
  if (!id)
    {
      if (entry->error)
        error_free (entry->error);
      entry->error = error_new ();
      error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                        "No <id> element found");
      return 0;
    }
  eid = iri_new ();
  if (!iri_set_from_string (eid, id))
    {
      if (entry->error)
        error_free (entry->error);
      entry->error = error_new ();
      error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                        "Invalid <id> iri");
      iri_free (eid);
      return 0;
    }
  title = iks_find_cdata (ik, "title");
  if (!title)
    {
      if (entry->error)
        error_free (entry->error);
      entry->error = error_new ();
      error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                        "No <title> element found");
      return 0;
    }

  /* Here we have almost all required fields */
  atom_entry_set_id (entry, eid);
  atom_entry_set_title (entry, title);

  updated = iks_find_cdata (ik, "updated");
  if (updated)
    {
      time_t updatedt;
      updatedt = iso8601_to_time (updated);
      atom_entry_set_updated (entry, updatedt);
    }

  published = iks_find_cdata (ik, "published");
  if (published)
    {
      time_t publishedt;
      publishedt = iso8601_to_time (published);
      atom_entry_set_published (entry, publishedt);
    }

  summary = iks_find_cdata (ik, "summary");
  if (summary)
    atom_entry_set_summary (entry, summary);

  rights = iks_find_cdata (ik, "rights");
  if (rights)
    atom_entry_set_rights (entry, rights);

  content = iks_find (ik, "content");
  if (content)
    {
      atom_content_t *ct;
      char *type = NULL, *src = NULL, *scontent = NULL;
      int content_len = 0;
      type = iks_find_attrib (content, "type");
      src = iks_find_attrib (content, "src");
      scontent = iks_cdata (iks_child (content));
      if (!type)
        {
          if (entry->error)
            error_free (entry->error);
          entry->error = error_new ();
          error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                            "No \"type\" attribute specified for content "
                            "element");
          return 0;
        }

      /* When content is filled, entry content should have no src
       * attribute */
      if (src && scontent)
        {
          if (entry->error)
            error_free (entry->error);
          entry->error = error_new ();
          error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                            "Invalid content, it has the src attribute set "
                            "and content tag is filled");
          return 0;
        }
      if (!src && !scontent)
        {
          if (entry->error)
            error_free (entry->error);
          entry->error = error_new ();
          error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                            "No src attribute or content in content tag");
          return 0;
        }
      if (scontent)
        content_len = iks_cdata_size (iks_child (content));
      ct = atom_content_new (type);
      atom_content_set_content (ct, scontent, content_len);
      if (src)
        {
          iri_t *srci;
          srci = iri_new ();
          if (!iri_set_from_string (srci, src))
            {
              if (entry->error)
                error_free (entry->error);
              entry->error = error_new ();
              error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                                "Invalid iri in content src attribute");
              iri_free (srci);
              atom_content_free (ct);
              return 0;
            }
          atom_content_set_src (ct, srci);
        }
      if (scontent)
        atom_content_set_content (ct, scontent, content_len);
      atom_entry_set_content (entry, ct);
    }

  /* Looking for more structured data */
  for (child = iks_child (ik); child; child = iks_next (child))
    {
      if (!strcmp (iks_name (child), "author"))
        {
          char *name, *email = NULL, *uri = NULL;
          iri_t *iri = NULL;
          atom_person_t *author;
          name = iks_find_cdata (child, "name");

          /* Specification is clear, an atom:author element *MUST*
           * have a name. */
          if (!name)
            {
              if (entry->error)
                error_free (entry->error);
              entry->error = error_new ();
              error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                                "Author with no name");
              return 0;
            }
          email = iks_find_cdata (child, "email");
          uri = iks_find_cdata (child, "uri");
          iri = iri_new ();

          /* Like above, specification denies invalid iris in an uri
           * of a person object. */
          if (uri && !iri_set_from_string (iri, uri))
            {
              if (entry->error)
                error_free (entry->error);
              entry->error = error_new ();
              error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                                "Author with an invalid iri in uri field");
              iri_free (iri);
              return 0;
            }
          author = atom_person_new (name, email, iri);
          atom_entry_add_author (entry, author);
        }
      if (!strcmp (iks_name (child), "category"))
        {
          atom_category_t *cat;
          iri_t *iri = NULL;
          char *term = NULL, *label = NULL, *scheme = NULL;
          term = iks_find_attrib (child, "term");
          label = iks_find_attrib (child, "label");
          scheme = iks_find_attrib (child, "scheme");
          if (!term)
            {
              if (entry->error)
                error_free (entry->error);
              entry->error = error_new ();
              error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                                "Category with no term attribute");
              return 0;
            }
          if (scheme)
            {
              iri = iri_new ();
              if (!iri_set_from_string (iri, scheme))
                {
                  if (entry->error)
                    error_free (entry->error);
                  entry->error = error_new ();
                  error_set_full (entry->error, ATOM_PARSING_ERROR, "ParsingError",
                                    "Category scheme attribute is not a valid iri");
                  iri_free (iri);
                  return 0;
                }
            }
          cat = atom_category_new (term, label, iri);
          atom_entry_add_category (entry, cat);
        }
    }
  return 1;
}

void
atom_entry_free (atom_entry_t *entry)
{
  if (entry->title)
    free (entry->title);
  if (entry->id)
    iri_free (entry->id);
  if (entry->rights)
    free (entry->rights);
  if (entry->authors)
    atom_entry_del_authors (entry);
  if (entry->categories)
    atom_entry_del_categories (entry);
  if (entry->summary)
    free (entry->summary);
  if (entry->content)
    atom_content_free (entry->content);
  if (entry->error)
    error_free (entry->error);
  free (entry);
}

error_t *
atom_entry_get_error (atom_entry_t *entry)
{
  return entry->error;
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
atom_entry_to_iks (atom_entry_t *entry)
{
  iks *ik;
  char *updated, *id_iri;
  list_t *tmp;

  if (entry->id == NULL)
    return NULL;

  updated = time_to_iso8601 (entry->updated);
  id_iri = iri_to_string (entry->id);

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
    for (tmp = entry->authors; tmp; tmp = tmp->next)
      {
        iks *authors =
          atom_person_to_iks (tmp->data, "author");
        iks_insert_node (ik, authors);
      }
  if (entry->categories)
    for (tmp = entry->categories; tmp; tmp = tmp->next)
      {
        iks *categories =
          atom_category_to_iks (tmp->data);
        iks_insert_node (ik, categories);
      }
  if (entry->links)
    for (tmp = entry->links; tmp; tmp = tmp->next)
      {
        iks *links =
          atom_link_to_iks (tmp->data);
        iks_insert_node (ik, links);
      }
  if (entry->summary)
    iks_insert_cdata (iks_insert (ik, "summary"), entry->summary, 0);
  if (entry->content)
    iks_insert_node (ik, atom_content_to_iks (entry->content));
  return ik;
}

char *
atom_entry_to_string (atom_entry_t *entry)
{
  iks *ik = atom_entry_to_iks (entry);
  if (ik)
    return strdup (iks_string (iks_stack (ik), ik));
  return NULL;
}

int
atom_entry_to_file (atom_entry_t *entry,
                      const char *fname)
{
  return iks_save (fname, atom_entry_to_iks (entry));
}

const char *
atom_entry_get_title (atom_entry_t *entry)
{
  return (const char *) entry->title;
}

void
atom_entry_set_title (atom_entry_t *entry,
                        const char *title)
{
  if (entry->title)
    free (entry->title);
  entry->title = strdup (title);
}

iri_t *
atom_entry_get_id (atom_entry_t *entry)
{
  return entry->id;
}

void
atom_entry_set_id (atom_entry_t *entry, iri_t *id)
{
  if (entry->id)
    iri_free (entry->id);
  entry->id = id;
}

time_t
atom_entry_get_updated (atom_entry_t *entry)
{
  return entry->updated;
}

void
atom_entry_set_updated (atom_entry_t *entry,
                          time_t      updated)
{
  entry->updated = updated;
}

time_t
atom_entry_get_published (atom_entry_t *entry)
{
  return entry->published;
}

void
atom_entry_set_published (atom_entry_t *entry,
                            time_t      published)
{
  entry->published = published;
}

const char *
atom_entry_get_rights (atom_entry_t *entry)
{
  return entry->rights;
}

void
atom_entry_set_rights (atom_entry_t *entry,
                         const char *rights)
{
  if (entry->rights)
    free (entry->rights);
  entry->rights = strdup (rights);
}

list_t *
atom_entry_get_authors (atom_entry_t *entry)
{
  return entry->authors;
}

void
atom_entry_add_author (atom_entry_t  *entry,
                         atom_person_t *author)
{
  entry->authors = list_append (entry->authors, author);
}

void
atom_entry_del_authors (atom_entry_t *entry)
{
  list_t *tmp;
  for (tmp = entry->authors; tmp; tmp = tmp->next)
    atom_person_free (tmp->data);
  list_free (entry->authors);
  entry->authors = NULL;
}

list_t *
atom_entry_get_categories (atom_entry_t *entry)
{
  return entry->categories;
}

void
atom_entry_add_category (atom_entry_t    *entry,
                           atom_category_t *category)
{
  entry->categories = list_append (entry->categories, category);
}

void
atom_entry_del_categories (atom_entry_t *entry)
{
  list_t *tmp;
  for (tmp = entry->categories; tmp; tmp = tmp->next)
    atom_category_free (tmp->data);
  list_free (entry->categories);
  entry->categories = NULL;
}

list_t *
atom_entry_get_links (atom_entry_t *entry)
{
  return entry->links;
}

void
atom_entry_add_link (atom_entry_t *entry,
                       atom_link_t  *link)
{
  entry->links = list_append (entry->links, link);
}

void
atom_entry_del_links (atom_entry_t *entry)
{
  list_t *tmp;
  for (tmp = entry->links; tmp; tmp = tmp->next)
    atom_link_free (tmp->data);
  list_free (entry->links);
  entry->links = NULL;
}

const char *
atom_entry_get_summary (atom_entry_t *entry)
{
  return (const char *) entry->summary;
}

void
atom_entry_set_summary (atom_entry_t *entry,
                          const char *summary)
{
  if (entry->summary)
    free (entry->summary);
  if (summary)
    entry->summary = strdup (summary);
  else
    entry->summary = NULL;
}

atom_content_t *
atom_entry_get_content (atom_entry_t *entry)
{
  return entry->content;
}

void
atom_entry_set_content (atom_entry_t   *entry,
                          atom_content_t *content)
{
  if (entry->content)
    atom_content_free (entry->content);
  entry->content = content;
}

void
atom_entry_add_see (atom_entry_t *entry, atom_simple_element_t *element)
{
  entry->ext_elements = list_append (entry->ext_elements, element);
}

void
atom_entry_del_see (atom_entry_t *entry)
{
  list_t *tmp;
  for (tmp = entry->ext_elements; tmp; tmp = tmp->next)
    atom_simple_element_free (tmp->data);
  list_free (entry->ext_elements);
  entry->ext_elements = NULL;
}

list_t *
atom_entry_get_see (atom_entry_t *entry)
{
  return entry->ext_elements;
}

/* atom_feed_t */

atom_feed_t *
atom_feed_new (const char *title)
{
  atom_feed_t *feed;
  feed = malloc (sizeof (atom_feed_t));
  feed->title = title ? strdup (title) : NULL;
  feed->id = NULL;
  feed->updated = time (0);
  feed->authors = NULL;
  feed->categories = NULL;
  feed->entries = NULL;
  return feed;
}

int
atom_feed_set_from_file (atom_feed_t  *feed,
                           const char *fname)
{
  iri_t *eid;
  iks *ik, *child;
  char *id, *title, *updated;
  int err;

  if ((err = iks_load (fname, &ik)) != IKS_OK)
    return 0;
  if (strcmp (iks_name (ik), "feed") ||
      !iks_has_children (ik))
    {
      iks_delete (ik);
      if (feed->error)
        error_free (feed->error);
      feed->error = error_new ();
      error_set_full (feed->error, ATOM_PARSING_ERROR, "ParsingError",
                        "Wrong root feed element");
      return 0;
    }
  id = iks_find_cdata (ik, "id");
  if (!id)
    {
      if (feed->error)
        error_free (feed->error);
      feed->error = error_new ();
      error_set_full (feed->error, ATOM_PARSING_ERROR, "ParsingError",
                        "No <id> element found");
      return 0;
    }
  eid = iri_new ();
  if (!iri_set_from_string (eid, id))
    {
      if (feed->error)
        error_free (feed->error);
      feed->error = error_new ();
      error_set_full (feed->error, ATOM_PARSING_ERROR, "ParsingError",
                        "Invalid <id> iri");
      iri_free (eid);
      return 0;
    }
  title = iks_find_cdata (ik, "title");
  if (!title)
    {
      if (feed->error)
        error_free (feed->error);
      feed->error = error_new ();
      error_set_full (feed->error, ATOM_PARSING_ERROR, "ParsingError",
                        "No <title> element found");
      return 0;
    }

  /* Here we have almost all required fields */
  atom_feed_set_id (feed, eid);
  atom_feed_set_title (feed, title);

  updated = iks_find_cdata (ik, "updated");
  if (updated)
    {
      time_t updatedt;
      updatedt = iso8601_to_time (updated);
      atom_feed_set_updated (feed, updatedt);
    }

  /* Looking for more structured data */
  for (child = iks_child (ik); child; child = iks_next (child))
    {
      if (!strcmp (iks_name (child), "author"))
        {
          char *name, *email = NULL, *uri = NULL;
          iri_t *iri = NULL;
          atom_person_t *author;
          name = iks_find_cdata (child, "name");

          /* Specification is clear, an atom:author element *MUST*
           * have a name. */
          if (!name)
            {
              if (feed->error)
                error_free (feed->error);
              feed->error = error_new ();
              error_set_full (feed->error, ATOM_PARSING_ERROR, "ParsingError",
                                "Author with no name");
              return 0;
            }
          email = iks_find_cdata (child, "email");
          uri = iks_find_cdata (child, "uri");
          iri = iri_new ();
          if (uri)
            iri_set_from_string (iri, uri);

          /* Like above, specification denies invalid iris in an atom
           * person. */
          if (uri && !iri_set_from_string (iri, uri))
            {
              if (feed->error)
                error_free (feed->error);
              feed->error = error_new ();
              error_set_full (feed->error, ATOM_PARSING_ERROR, "ParsingError",
                                "Author with an invalid iri in uri field");
              iri_free (iri);
              return 0;
            }
          author = atom_person_new (name, email, iri);
          atom_feed_add_author (feed, author);
        }
      if (!strcmp (iks_name (child), "category"))
        {
          atom_category_t *cat;
          iri_t *iri = NULL;
          char *term = NULL, *label = NULL, *scheme = NULL;
          term = iks_find_attrib (child, "term");
          label = iks_find_attrib (child, "label");
          scheme = iks_find_attrib (child, "scheme");
          if (!term)
            {
              if (feed->error)
                error_free (feed->error);
              feed->error = error_new ();
              error_set_full (feed->error, ATOM_PARSING_ERROR, "ParsingError",
                                "Category with no term attribute");
              return 0;
            }
          if (scheme)
            {
              iri = iri_new ();
              if (!iri_set_from_string (iri, scheme))
                {
                  if (feed->error)
                    error_free (feed->error);
                  feed->error = error_new ();
                  error_set_full (feed->error, ATOM_PARSING_ERROR, "ParsingError",
                                    "Category scheme attribute is not a valid iri");
                  iri_free (iri);
                  return 0;
                }
            }
          cat = atom_category_new (term, label, iri);
          atom_feed_add_category (feed, cat);
        }
    }
  return 1;
}

void
atom_feed_free (atom_feed_t *feed)
{
  if (feed->id)
    iri_free (feed->id);
  if (feed->title)
    free (feed->title);
  if (feed->authors)
    atom_feed_del_authors (feed);
  if (feed->categories)
    atom_feed_del_categories (feed);
  if (feed->entries)
    atom_feed_del_entries (feed);
  if (feed->error)
    error_free (feed->error);
  free (feed);
}

error_t *
atom_feed_get_error (atom_feed_t *feed)
{
  return feed->error;
}

iks *
atom_feed_to_iks (atom_feed_t *feed)
{
  iks *ik;
  char *updated, *id_iri;
  list_t *tmp;

  if (feed->id == NULL)
    return NULL;

  updated = time_to_iso8601 (feed->updated);
  id_iri = iri_to_string (feed->id);

  ik = iks_new ("feed");
  iks_insert_attrib (ik, "xmlns", J_ATOM_NS);
  iks_insert_cdata (iks_insert (ik, "id"), id_iri, 0);
  iks_insert_cdata (iks_insert (ik, "title"), feed->title, 0);
  iks_insert_cdata (iks_insert (ik, "updated"), updated, 0);
  free (updated);
  free (id_iri);
  if (feed->authors)
    for (tmp = feed->authors; tmp; tmp = tmp->next)
      {
        iks *authors =
          atom_person_to_iks (tmp->data, "author");
        iks_insert_node (ik, authors);
      }
  if (feed->categories)
    for (tmp = feed->categories; tmp; tmp = tmp->next)
      {
        iks *categories =
          atom_category_to_iks (tmp->data);
        iks_insert_node (ik, categories);
      }
  if (feed->entries)
    for (tmp = feed->entries; tmp; tmp = tmp->next)
      {
        iks *entries =
          atom_entry_to_iks (tmp->data);
        iks_insert_node (ik, entries);
      }
  return ik;
}

char *
atom_feed_to_string (atom_feed_t *feed)
{
  iks *ik = atom_feed_to_iks (feed);
  if (ik)
    return strdup (iks_string (iks_stack (ik), ik));
  return NULL;
}

int
atom_feed_to_file (atom_feed_t  *feed,
                     const char *fname)
{
  return iks_save (fname, atom_feed_to_iks (feed));
}

const char *
atom_feed_get_title (atom_feed_t *feed)
{
  return feed->title;
}

void
atom_feed_set_title (atom_feed_t  *feed,
                       const char *title)
{
  if (feed->title)
    free (feed->title);
  feed->title = strdup (title);
}

iri_t *
atom_feed_get_id (atom_feed_t *feed)
{
  return feed->id;
}

void
atom_feed_set_id (atom_feed_t *feed, iri_t *id)
{
  if (feed->id)
    iri_free (feed->id);
  feed->id = id;
}

time_t
atom_feed_get_updated (atom_feed_t *feed)
{
  return feed->updated;
}

void
atom_feed_set_updated (atom_feed_t *feed,
                         time_t     updated)
{
  feed->updated = updated;
}

list_t *
atom_feed_get_authors (atom_feed_t *feed)
{
  return feed->authors;
}

void
atom_feed_add_author (atom_feed_t  *feed,
                         atom_person_t *author)
{
  feed->authors = list_append (feed->authors, author);
}

void
atom_feed_del_authors (atom_feed_t *feed)
{
  list_t *tmp;
  for (tmp = feed->authors; tmp; tmp = tmp->next)
    atom_person_free (tmp->data);
  list_free (feed->authors);
  feed->authors = NULL;
}

list_t *
atom_feed_get_categories (atom_feed_t *feed)
{
  return feed->categories;
}

void
atom_feed_add_category (atom_feed_t    *feed,
                           atom_category_t *category)
{
  feed->categories = list_append (feed->categories, category);
}

void
atom_feed_del_categories (atom_feed_t *feed)
{
  list_t *tmp;
  for (tmp = feed->categories; tmp; tmp = tmp->next)
    atom_category_free (tmp->data);
  list_free (feed->categories);
  feed->categories = NULL;
}

list_t *
atom_feed_get_links (atom_feed_t *feed)
{
  return feed->links;
}

void
atom_feed_add_link (atom_feed_t *feed, atom_link_t *link)
{
  feed->links = list_append (feed->links, link);
}

void
atom_feed_del_links (atom_feed_t *feed)
{
  list_t *tmp;
  for (tmp = feed->links; tmp; tmp = tmp->next)
    atom_link_free (tmp->data);
  list_free (feed->links);
  feed->links = NULL;
}

list_t *
atom_feed_get_entries (atom_feed_t *feed)
{
  return feed->entries;
}

void
atom_feed_add_entry (atom_feed_t  *feed,
                       atom_entry_t *entry)
{
  feed->entries = list_append (feed->entries, entry);
}

void
atom_feed_del_entries (atom_feed_t *feed)
{
  list_t *tmp;
  for (tmp = feed->entries; tmp; tmp = tmp->next)
    atom_entry_free (tmp->data);
  list_free (feed->entries);
  feed->entries = NULL;
}
