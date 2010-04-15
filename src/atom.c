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

#define _GNU_SOURCE
#define _XOPEN_SOURCE       /* glibc 2.0 needs this to use strptime */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iksemel.h>
#include <taningia/object.h>
#include <taningia/atom.h>
#include <taningia/iri.h>
#include <taningia/error.h>
#include <taningia/list.h>

/* Forward prototypes */

static char   *time_to_iso8601   (time_t t);

static time_t  iso8601_to_time   (const char *dt);

/* helper functions */

static void
_free_iks_childs (iks *root)
{
  iks *child = NULL, *tmp = NULL;

  child = iks_child (root);
  while (child)
    {
      if (child != root)
        _free_iks_childs (child);
      if (iks_stack (root) == iks_stack (child))
        {
          child = iks_next (child);
          continue;
        }
      else
        {
          tmp = child;
          child = iks_next (child);
          iks_delete (tmp);
        }
    }
}

static char *
_get_iks_string (iks *ik)
{
  if (ik)
    {
      char *ret = strdup (iks_string (iks_stack (ik), ik));
      _free_iks_childs (ik);
      iks_delete (ik);
      return ret;
    }
  return NULL;
}

static void
_ta_atom_free_ext_elements (ta_list_t *list)
{
  ta_list_t *node = list, *tmp = NULL;
  while (node)
    {
      tmp = node;
      node = node->next;
      ta_object_unref (tmp->data);
      free (tmp);
    }
}

/* ta_atom_in_reply_to_t */

static void
ta_atom_in_reply_to_free (ta_atom_in_reply_to_t *irt)
{
  if (irt->ref)
    ta_object_unref (irt->ref);
  if (irt->href)
    ta_object_unref (irt->href);
  if (irt->source)
    ta_object_unref (irt->source);
  if (irt->type)
    free (irt->type);
  free (irt);
}

void
ta_atom_in_reply_to_init (ta_atom_in_reply_to_t *irt, ta_iri_t *ref)
{
  ta_object_init (TA_CAST_OBJECT (irt),
                  (ta_free_func_t) ta_atom_in_reply_to_free);
  irt->ref = ref;
  irt->href = NULL;
  irt->source = NULL;
  irt->type = NULL;
}

ta_atom_in_reply_to_t *
ta_atom_in_reply_to_new (ta_iri_t *ref)
{
  ta_atom_in_reply_to_t *irt;
  irt = malloc (sizeof (ta_atom_in_reply_to_t));
  ta_atom_in_reply_to_init (irt, ref);
  return irt;
}

iks *
ta_atom_in_reply_to_to_iks (ta_atom_in_reply_to_t *irt)
{
  iks *iksirt;
  char *iri;
  iksirt = iks_new ("in-reply-to");
  iks_insert_attrib (iksirt, "xmlns", TA_ATOM_THREADING_NS);

  iri = ta_iri_to_string (irt->ref);
  iks_insert_attrib (iksirt, "ref", iri);
  free (iri);

  if (irt->href)
    {
      iri = ta_iri_to_string (irt->href);
      iks_insert_attrib (iksirt, "href", iri);
      free (iri);
    }
  if (irt->source)
    {
      iri = ta_iri_to_string (irt->source);
      iks_insert_attrib (iksirt, "source", iri);
      free (iri);
    }
  if (irt->type)
    iks_insert_attrib (iksirt, "type", irt->type);
  return iksirt;
}

char *
ta_atom_in_reply_to_to_string (ta_atom_in_reply_to_t *irt)
{
  iks *ik = ta_atom_in_reply_to_to_iks (irt);
  return _get_iks_string (ik);
}

ta_iri_t *
ta_atom_in_reply_to_get_ref (ta_atom_in_reply_to_t *irt)
{
  return irt->ref;
}

void
ta_atom_in_reply_to_set_ref (ta_atom_in_reply_to_t *irt, ta_iri_t *ref)
{
  if (irt->ref)
    ta_object_unref (irt->ref);
  irt->ref = ref;
}

ta_iri_t *
ta_atom_in_reply_to_get_href (ta_atom_in_reply_to_t *irt)
{
  return irt->href;
}

void
ta_atom_in_reply_to_set_href (ta_atom_in_reply_to_t *irt, ta_iri_t *href)
{
  if (irt->href)
    ta_object_unref (irt->href);
  irt->href = href;
}

ta_iri_t *
ta_atom_in_reply_to_get_source (ta_atom_in_reply_to_t *irt)
{
  return irt->source;
}

void
ta_atom_in_reply_to_set_source (ta_atom_in_reply_to_t *irt, ta_iri_t *source)
{
  if (irt->source)
    ta_object_unref (irt->source);
  irt->source = source;
}

const char *
ta_atom_in_reply_to_get_type (ta_atom_in_reply_to_t *irt)
{
  return irt->type;
}

void
ta_atom_in_reply_to_set_type (ta_atom_in_reply_to_t *irt, const char *type)
{
  if (irt->type)
    free (irt->type);
  irt->type = strdup (type);
}

/* ta_atom_simple_element_t */

static void
ta_atom_simple_element_free (ta_atom_simple_element_t *see)
{
  if (see->name)
    free (see->name);
  if (see->value)
    free (see->value);
  free (see);
}

void
ta_atom_simple_element_init (ta_atom_simple_element_t *see,
                             const char *name,
                             const char *value)
{
  ta_object_init (TA_CAST_OBJECT (see),
                  (ta_free_func_t) ta_atom_simple_element_free);
  see->name = strdup (name);
  see->value = strdup (value);
}

ta_atom_simple_element_t *
ta_atom_simple_element_new (const char *name, const char *value)
{
  ta_atom_simple_element_t *see;
  see = malloc (sizeof (ta_atom_simple_element_t));
  ta_atom_simple_element_init (see, name, value);
  return see;
}

iks *
ta_atom_simple_element_to_iks (ta_atom_simple_element_t *see)
{
  iks *iksee;
  iksee = iks_new (see->name);
  iks_insert_cdata (iksee, see->value, 0);
  return iksee;
}

char *
ta_atom_simple_element_to_string (ta_atom_simple_element_t *see)
{
  iks *ik = ta_atom_simple_element_to_iks (see);
  return _get_iks_string (ik);
}

const char *
ta_atom_simple_element_get_name (ta_atom_simple_element_t *see)
{
  return see->name;
}

void
ta_atom_simple_element_set_name (ta_atom_simple_element_t *see,
                                 const char *name)
{
  if (see->name)
    free (see->name);
  see->name = strdup (name);
}

const char *
ta_atom_simple_element_get_value (ta_atom_simple_element_t *see)
{
  return see->value;
}

void
ta_atom_simple_element_set_value (ta_atom_simple_element_t *see,
                                  const char *value)
{
  if (see->value)
    free (see->value);
  see->value = strdup (value);
}

/* ta_atom_link_t */

static void
ta_atom_link_free (ta_atom_link_t *link)
{
  if (link->href)
    ta_object_unref (link->href);
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

void
ta_atom_link_init (ta_atom_link_t *lnk, ta_iri_t *href)
{
  ta_object_init (TA_CAST_OBJECT (lnk), (ta_free_func_t) ta_atom_link_free);
  lnk->href = href;
  lnk->title = NULL;
  lnk->rel = NULL;
  lnk->type = NULL;
  lnk->length = NULL;
}

ta_atom_link_t *
ta_atom_link_new (ta_iri_t *href)
{
  ta_atom_link_t *lnk;
  lnk = malloc (sizeof (ta_atom_link_t));
  ta_atom_link_init (lnk, href);
  return lnk;
}

iks *
ta_atom_link_to_iks (ta_atom_link_t *link)
{
  iks *lnk;
  char *href;
  lnk = iks_new ("link");
  href = ta_iri_to_string (link->href);
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
ta_atom_link_to_string (ta_atom_link_t *link)
{
  iks *ik = ta_atom_link_to_iks (link);
  return _get_iks_string (ik);
}

ta_iri_t *
ta_atom_link_get_href (ta_atom_link_t *link)
{
  return link->href;
}

void
ta_atom_link_set_href (ta_atom_link_t *link,
                       ta_iri_t      *href)
{
  if (link->href)
    ta_object_unref (link->href);
  link->href = href;
}

const char *
ta_atom_link_get_title (ta_atom_link_t *link)
{
  return link->title;
}

void
ta_atom_link_set_title (ta_atom_link_t  *link,
                        const char *title)
{
  if (link->title)
    free (link->title);
  link->title = strdup (title);
}

const char *
ta_atom_link_get_rel (ta_atom_link_t *link)
{
  return link->rel;
}

void
ta_atom_link_set_rel (ta_atom_link_t  *link,
                      const char *rel)
{
  if (link->rel)
    free (link->rel);
  link->rel = strdup (rel);
}

const char *
ta_atom_link_get_type (ta_atom_link_t *link)
{
  return link->type;
}

void
ta_atom_link_set_type (ta_atom_link_t  *link,
                       const char *type)
{
  if (link->type)
    free (link->type);
  link->type = strdup (type);
}

const char *
ta_atom_link_get_length (ta_atom_link_t *link)
{
  return link->length;
}

void
ta_atom_link_set_length (ta_atom_link_t  *link,
                         const char *length)
{
  if (link->length)
    free (link->length);
  link->length = strdup (length);
}

/* ta_atom_content_t */

static void
ta_atom_content_free (ta_atom_content_t *content)
{
  if (content->type)
    free (content->type);
  if (content->content)
    free (content->content);
  if (content->src)
    ta_object_unref (content->src);
  free (content);
}

void
ta_atom_content_init (ta_atom_content_t *ct, const char *type)
{
  ta_object_init (TA_CAST_OBJECT (ct), (ta_free_func_t) ta_atom_content_free);
  ct->type = strdup (type ? type : "text");
  ct->content = NULL;
  ct->src = NULL;
  ct->len = 0;
}

ta_atom_content_t *
ta_atom_content_new (const char *type)
{
  ta_atom_content_t *ct;
  ct = malloc (sizeof (ta_atom_content_t));
  ta_atom_content_init (ct, type);
  return ct;
}

iks *
ta_atom_content_to_iks (ta_atom_content_t *content)
{
  iks *ct;
  ct = iks_new ("content");
  iks_insert_attrib (ct, "type", content->type);
  if (content->src != NULL)
    {
      char *iri;
      iri = ta_iri_to_string (content->src);
      iks_insert_attrib (ct, "src", iri);
      free (iri);
    }
  else if (content->content)
    {
      /* Section 4.1.3 of the RFC is clear, if the content type is
       * "text/xhtml" an xhtml div element should be created */
      if (!strcmp (content->type, "xhtml"))
        {
          iks *divel = iks_insert (ct, "div");
          iks_insert_attrib (divel, "xmlns", "http://www.w3.org/1999/xhtml");
          iks_insert_cdata (divel, content->content, content->len);
        }
      else
        {
          iks_insert_cdata (ct, content->content, content->len);
        }
    }
  return ct;
}

char *
ta_atom_content_to_string (ta_atom_content_t *content)
{
  iks *ik = ta_atom_content_to_iks (content);
  return _get_iks_string (ik);
}

const char *
ta_atom_content_get_type (ta_atom_content_t *content)
{
  return content->type;
}

void
ta_atom_content_set_type (ta_atom_content_t *content,
                          const char   *type)
{
  if (content->type)
    free (content->type);
  content->type = strdup (type);
}

ta_iri_t *
ta_atom_content_get_src (ta_atom_content_t *content)
{
  return content->src;
}

void
ta_atom_content_set_src (ta_atom_content_t *content,
                         ta_iri_t         *src)
{
  if (content->src)
    {
      ta_object_unref (content->src);
      if (src != NULL && content->content)
        {
          free (content->content);
          content->content = NULL;
        }
    }
  content->src = src;
}

const char *
ta_atom_content_get_content (ta_atom_content_t *content,
                             int          *len)
{
  if (len)
    *len = content->len;
  return content->content;
}

void
ta_atom_content_set_content (ta_atom_content_t *content,
                             const char *text, int len)
{
  if (content->content)
    {
      free (content->content);
      if (text != NULL && content->src)
        {
          ta_object_unref (content->src);
          content->src = NULL;
        }
    }
  if (text && len)
    {
      content->content = malloc (len + 1);
      memcpy (content->content, text, len);
    }
}

/* ta_atom_person_t */

static void
ta_atom_person_free (ta_atom_person_t *person)
{
  if (person->name)
    free (person->name);
  if (person->email)
    free (person->email);
  if (person->iri)
    ta_object_unref (person->iri);
  if (person->ext_elements)
    _ta_atom_free_ext_elements (person->ext_elements);
  free (person);
}

void
ta_atom_person_init (ta_atom_person_t *person,
                     const char *name,
                     const char *email,
                     ta_iri_t *iri)
{
  ta_object_init (TA_CAST_OBJECT (person),
                  (ta_free_func_t) ta_atom_person_free);
  person->name = strdup (name);
  person->email = email ? strdup (email) : NULL;
  person->iri = iri;
  person->ext_elements = NULL;
}

ta_atom_person_t *
ta_atom_person_new (const char *name,
                    const char *email,
                    ta_iri_t *iri)
{
  ta_atom_person_t *person;
  person = malloc (sizeof (ta_atom_person_t));
  ta_atom_person_init (person, name, email, iri);
  return person;
}

iks *
ta_atom_person_to_iks (ta_atom_person_t *person, const char *element)
{
  iks *ik;
  ik = iks_new (element);
  iks_insert_cdata (iks_insert (ik, "name"), person->name, 0);
  if (person->email)
    iks_insert_cdata (iks_insert (ik, "email"), person->email, 0);
  if (person->iri)
    {
      char *iri;
      iri = ta_iri_to_string (person->iri);
      iks_insert_cdata (iks_insert (ik, "uri"), iri, 0);
      free (iri);
    }
  if (person->ext_elements)
    {
      ta_list_t *tmp;
      for (tmp = person->ext_elements; tmp; tmp = tmp->next)
        {
          iks *ext_elements =
            ta_atom_simple_element_to_iks (tmp->data);
          iks_insert_node (ik, ext_elements);
        }
    }
  return ik;
}

char *
ta_atom_person_to_string (ta_atom_person_t *person, const char *element)
{
  iks *ik = ta_atom_person_to_iks (person, element);
  return _get_iks_string (ik);
}

const char *
ta_atom_person_get_name (ta_atom_person_t *person)
{
  return person->name;
}

void
ta_atom_person_set_name (ta_atom_person_t *person,
                         const char  *name)
{
  if (person->name)
    free (person->name);
  person->name = strdup (name);
}

const char *
ta_atom_person_get_email (ta_atom_person_t *person)
{
  return person->email;
}

void
ta_atom_person_set_email (ta_atom_person_t *person,
                          const char  *email)
{
  if (person->email)
    free (person->email);
  person->email = strdup (email);
}

ta_iri_t *
ta_atom_person_get_iri (ta_atom_person_t *person)
{
  return person->iri;
}

void
ta_atom_person_set_iri (ta_atom_person_t *person,
                        ta_iri_t        *iri)
{
  if (person->iri)
    ta_object_unref (person->iri);
  person->iri = iri;
}

void
ta_atom_person_add_see (ta_atom_person_t *person,
                        ta_atom_simple_element_t *element)
{
  person->ext_elements = ta_list_append (person->ext_elements, element);
}

void
ta_atom_person_del_see (ta_atom_person_t *person)
{
  _ta_atom_free_ext_elements (person->ext_elements);
  person->ext_elements = NULL;
}

ta_list_t *
ta_atom_person_get_see (ta_atom_person_t *person)
{
  return person->ext_elements;
}


/* ta_atom_category_t */

static void
ta_atom_category_free (ta_atom_category_t *category)
{
  if (category->term)
    free (category->term);
  if (category->label)
    free (category->label);
  if (category->scheme)
    ta_object_unref (category->scheme);
  free (category);
}

void
ta_atom_category_init (ta_atom_category_t *cat,
                       const char *term,
                       const char *label,
                       ta_iri_t *scheme)
{
  ta_object_init (TA_CAST_OBJECT (cat),
                  (ta_free_func_t) ta_atom_category_free);
  cat->term = strdup (term);
  cat->label = label ? strdup (label) : NULL;
  cat->scheme = scheme ? scheme : NULL;
}

ta_atom_category_t *
ta_atom_category_new (const char *term, const char *label, ta_iri_t *scheme)
{
  ta_atom_category_t *cat;
  cat = malloc (sizeof (ta_atom_category_t));
  ta_atom_category_init (cat, term, label, scheme);
  return cat;
}

iks *
ta_atom_category_to_iks (ta_atom_category_t *category)
{
  iks *ik;
  ik = iks_new ("category");
  iks_insert_attrib (ik, "term", category->term);
  if (category->label)
    iks_insert_attrib (ik, "label", category->label);
  if (category->scheme)
    {
      char *iri;
      iri = ta_iri_to_string (category->scheme);
      iks_insert_attrib (ik, "scheme", iri);
      free (iri);
    }
  return ik;
}

char *
ta_atom_category_to_string (ta_atom_category_t  *category)
{
  iks *ik = ta_atom_category_to_iks (category);
  return _get_iks_string (ik);
}

const char *
ta_atom_category_get_label (ta_atom_category_t *category)
{
  return (const char *) category->label;
}

void
ta_atom_category_set_label (ta_atom_category_t *category,
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
ta_atom_category_get_term (ta_atom_category_t  *category)
{
  return (const char *) category->term;
}

void
ta_atom_category_set_term (ta_atom_category_t *category,
                           const char    *term)
{
  if (category->term)
    free (category->term);
  category->term = strdup (term);
}

ta_iri_t *
ta_atom_category_get_scheme (ta_atom_category_t *category)
{
  return category->scheme;
}

void
ta_atom_category_set_scheme (ta_atom_category_t *category,
                             ta_iri_t          *scheme)
{
  if (category->scheme)
    ta_object_unref (category->scheme);
  if (scheme)
    category->scheme = scheme;
  else
    category->scheme = NULL;
}

/* ta_atom_entry_t */

static void
ta_atom_entry_free (ta_atom_entry_t *entry)
{
  if (entry->title)
    free (entry->title);
  if (entry->id)
    ta_object_unref (entry->id);
  if (entry->rights)
    free (entry->rights);
  if (entry->authors)
    ta_atom_entry_del_authors (entry);
  if (entry->categories)
    ta_atom_entry_del_categories (entry);
  if (entry->in_reply_to)
    ta_atom_entry_del_inreplyto (entry);
  if (entry->summary)
    free (entry->summary);
  if (entry->content)
    ta_object_unref (entry->content);
  if (entry->error)
    ta_object_unref (entry->error);
  free (entry);
}

void
ta_atom_entry_init (ta_atom_entry_t *entry, const char *title)
{
  ta_object_init (TA_CAST_OBJECT (entry), (ta_free_func_t) ta_atom_entry_free);
  entry->title = title ? strdup (title) : NULL;
  entry->id = NULL;
  entry->updated = time (0);
  entry->published = 0;
  entry->rights = NULL;
  entry->authors = NULL;
  entry->categories = NULL;
  entry->links = NULL;
  entry->in_reply_to = NULL;
  entry->summary = NULL;
  entry->content = NULL;
  entry->error = NULL;
}

ta_atom_entry_t *
ta_atom_entry_new (const char *title)
{
  ta_atom_entry_t *entry;
  entry = malloc (sizeof (ta_atom_entry_t));
  ta_atom_entry_init (entry, title);
  return entry;
}

int
ta_atom_entry_set_from_file (ta_atom_entry_t *entry,
                             const char *fname)
{
  iks *ik;
  int err, result;
  if ((err = iks_load (fname, &ik)) != IKS_OK)
    {
      if (entry->error)
        ta_object_unref (entry->error);
      entry->error = ta_error_new ();
      switch (err)
        {
        case IKS_NOMEM:
          ta_error_set_full (entry->error, TA_ATOM_LOAD_ERROR, "LoadError",
                             "Not enough memory to load file");
        case IKS_BADXML:
          ta_error_set_full (entry->error, TA_ATOM_LOAD_ERROR, "LoadError",
                             "Unable to parse xml file");
        default:
          ta_error_set_full (entry->error, TA_ATOM_LOAD_ERROR, "LoadError",
                             "Unknown error");
        }
      return 0;
    }
  result = ta_atom_entry_set_from_iks (entry, ik);
  iks_delete (ik);
  return result;
}

int
ta_atom_entry_set_from_iks (ta_atom_entry_t *entry,
                            iks        *ik)
{
  ta_iri_t *eid;
  iks *child, *content;
  char *id, *title, *updated, *published, *summary, *rights;

  if (strcmp (iks_name (ik), "entry") ||
      !iks_has_children (ik))
    {
      iks_delete (ik);
      if (entry->error)
        ta_object_unref (entry->error);
      entry->error = ta_error_new ();
      ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR, "ParsingError",
                         "Wrong root entry element");
      return 0;
    }
  id = iks_find_cdata (ik, "id");
  if (!id)
    {
      if (entry->error)
        ta_object_unref (entry->error);
      entry->error = ta_error_new ();
      ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR, "ParsingError",
                         "No <id> element found");
      return 0;
    }
  eid = ta_iri_new ();
  if (!ta_iri_set_from_string (eid, id))
    {
      if (entry->error)
        ta_object_unref (entry->error);
      entry->error = ta_error_new ();
      ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR, "ParsingError",
                         "Invalid <id> iri");
      ta_object_unref (eid);
      return 0;
    }
  title = iks_find_cdata (ik, "title");
  if (!title)
    {
      if (entry->error)
        ta_object_unref (entry->error);
      entry->error = ta_error_new ();
      ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR, "ParsingError",
                         "No <title> element found");
      return 0;
    }

  /* Here we have almost all required fields */
  ta_atom_entry_set_id (entry, eid);
  ta_atom_entry_set_title (entry, title);

  updated = iks_find_cdata (ik, "updated");
  if (updated)
    {
      time_t updatedt;
      updatedt = iso8601_to_time (updated);
      ta_atom_entry_set_updated (entry, updatedt);
    }

  published = iks_find_cdata (ik, "published");
  if (published)
    {
      time_t publishedt;
      publishedt = iso8601_to_time (published);
      ta_atom_entry_set_published (entry, publishedt);
    }

  summary = iks_find_cdata (ik, "summary");
  if (summary)
    ta_atom_entry_set_summary (entry, summary);

  rights = iks_find_cdata (ik, "rights");
  if (rights)
    ta_atom_entry_set_rights (entry, rights);

  content = iks_find (ik, "content");
  if (content)
    {
      ta_atom_content_t *ct;
      char *type = NULL, *src = NULL, *scontent = NULL;
      int content_len = 0;
      type = iks_find_attrib (content, "type");
      src = iks_find_attrib (content, "src");
      scontent = iks_find_cdata (ik, "content");
      if (!type)
        {
          if (entry->error)
            ta_object_unref (entry->error);
          entry->error = ta_error_new ();
          ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                             "ParsingError",
                             "No \"type\" attribute specified for content "
                             "element");
          return 0;
        }

      /* When content is filled, entry content should have no src
       * attribute */
      if (src && scontent)
        {
          if (entry->error)
            ta_object_unref (entry->error);
          entry->error = ta_error_new ();
          ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                             "ParsingError",
                             "Invalid content, it has the src attribute set "
                             "and content tag is filled");
          return 0;
        }
      if (!src && !scontent)
        goto look_for_children;

      ct = ta_atom_content_new (type);
      if (scontent)
        {
          content_len = iks_cdata_size (iks_child (content));
          ta_atom_content_set_content (ct, scontent, content_len);
        }
      if (src)
        {
          ta_iri_t *srci;
          srci = ta_iri_new ();
          if (!ta_iri_set_from_string (srci, src))
            {
              if (entry->error)
                ta_object_unref (entry->error);
              entry->error = ta_error_new ();
              ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                                 "ParsingError",
                                 "Invalid iri in content src attribute");
              ta_object_unref (srci);
              ta_object_unref (ct);
              return 0;
            }
          ta_atom_content_set_src (ct, srci);
        }
      if (scontent)
        ta_atom_content_set_content (ct, scontent, content_len);
      ta_atom_entry_set_content (entry, ct);
    }

 look_for_children:

  /* Looking for more structured data */
  for (child = iks_child (ik); child; child = iks_next (child))
    {
      if (!strcmp (iks_name (child), "author"))
        {
          char *name, *email = NULL, *uri = NULL;
          ta_iri_t *iri = NULL;
          ta_atom_person_t *author;
          name = iks_find_cdata (child, "name");

          /* Specification is clear, an ta_atom:author element *MUST*
           * have a name. */
          if (!name)
            {
              if (entry->error)
                ta_object_unref (entry->error);
              entry->error = ta_error_new ();
              ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                                 "ParsingError", "Author with no name");
              return 0;
            }
          email = iks_find_cdata (child, "email");
          uri = iks_find_cdata (child, "uri");
          iri = ta_iri_new ();

          /* Like above, specification denies invalid iris in an uri
           * of a person object. */
          if (uri && !ta_iri_set_from_string (iri, uri))
            {
              if (entry->error)
                ta_object_unref (entry->error);
              entry->error = ta_error_new ();
              ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                                 "ParsingError",
                                 "Author with an invalid iri in uri field");
              ta_object_unref (iri);
              return 0;
            }
          author = ta_atom_person_new (name, email, iri);
          ta_atom_entry_add_author (entry, author);
        }
      if (!strcmp (iks_name (child), "category"))
        {
          ta_atom_category_t *cat;
          ta_iri_t *iri = NULL;
          char *term = NULL, *label = NULL, *scheme = NULL;
          term = iks_find_attrib (child, "term");
          label = iks_find_attrib (child, "label");
          scheme = iks_find_attrib (child, "scheme");
          if (!term)
            {
              if (entry->error)
                ta_object_unref (entry->error);
              entry->error = ta_error_new ();
              ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                                 "ParsingError",
                                 "Category with no term attribute");
              return 0;
            }
          if (scheme)
            {
              iri = ta_iri_new ();
              if (!ta_iri_set_from_string (iri, scheme))
                {
                  if (entry->error)
                    ta_object_unref (entry->error);
                  entry->error = ta_error_new ();
                  ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                                     "ParsingError",
                                     "Category scheme attribute is not a "
                                     "valid iri");
                  ta_object_unref (iri);
                  return 0;
                }
            }
          cat = ta_atom_category_new (term, label, iri);
          ta_atom_entry_add_category (entry, cat);
        }
      if (!strcmp (iks_name (child), "in-reply-to"))
        {
          ta_atom_in_reply_to_t *irt;
          ta_iri_t *iri_ref;
          char *ref, *href, *source, *type;
          ref = iks_find_attrib (child, "ref");
          href = iks_find_attrib (child, "href");
          source = iks_find_attrib (child, "source");
          type = iks_find_attrib (child, "type");
          if (!ref)
            {
              if (entry->error)
                ta_object_unref (entry->error);
              entry->error = ta_error_new ();
              ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                                 "ParsingError",
                                 "InReplyTo element with no ref attribute.");
              return 0;
            }

          iri_ref = ta_iri_new ();
          if (!ta_iri_set_from_string (iri_ref, ref))
            {
              const char *error_message;
              if (entry->error)
                ta_object_unref (entry->error);
              entry->error = ta_error_new ();
              error_message =
                ta_error_get_message (ta_iri_get_error (iri_ref));
              ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                                 "ParsingError",
                                 "InReplyTo element with an invalid ref "
                                 "attribute: %s",
                                 error_message);
              return 0;
            }
          else
            irt = ta_atom_in_reply_to_new (iri_ref);

          if (href)
            {
              ta_iri_t *iri_href;
              iri_href = ta_iri_new ();
              if (!ta_iri_set_from_string (iri_href, href))
                {
                  const char *error_message;
                  if (entry->error)
                    ta_object_unref (entry->error);
                  entry->error = ta_error_new ();
                  error_message =
                    ta_error_get_message (ta_iri_get_error (iri_href));
                  ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                                     "ParsingError",
                                     "InReplyTo element with an invalid href "
                                     "attribute: %s",
                                     error_message);
                  ta_object_unref (irt);
                }
              else
                ta_atom_in_reply_to_set_href (irt, iri_href);
            }

          if (source)
            {
              ta_iri_t *iri_source;
              iri_source = ta_iri_new ();
              if (!ta_iri_set_from_string (iri_source, source))
                {
                  const char *error_message;
                  if (entry->error)
                    ta_object_unref (entry->error);
                  entry->error = ta_error_new ();
                  error_message =
                    ta_error_get_message (ta_iri_get_error (iri_source));
                  ta_error_set_full (entry->error, TA_ATOM_PARSING_ERROR,
                                     "ParsingError",
                                     "InReplyTo element with an invalid "
                                     "source attribute: %s",
                                     error_message);
                  ta_object_unref (irt);
                }
              else
                ta_atom_in_reply_to_set_source (irt, iri_source);
            }

          if (type)
            ta_atom_in_reply_to_set_type (irt, type);
        }
    }
  return 1;
}

ta_error_t *
ta_atom_entry_get_error (ta_atom_entry_t *entry)
{
  return entry->error;
}

static char *
time_to_iso8601 (time_t t)
{
  struct tm *tm;
  size_t bufsize = 20;
  char *date_iso;

  tm = gmtime (&t);
  if ((date_iso = malloc (bufsize)) == NULL)
    return NULL;
  snprintf (date_iso, bufsize,
            "%4d-%02d-%02dT%02d:%02d:%02dZ",
            tm->tm_year + 1900,
            tm->tm_mon + 1,
            tm->tm_mday,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec);
  return date_iso;
}

static time_t
iso8601_to_time (const char *dt)
{
  struct tm tm = {};
  strptime (dt, "%Y-%m-%dT%H:%M:%SZ", &tm);
  return mktime (&tm);
}

iks *
ta_atom_entry_to_iks (ta_atom_entry_t *entry)
{
  iks *ik;
  char *updated, *id_iri;
  ta_list_t *tmp;

  if (entry->id == NULL)
    return NULL;

  updated = time_to_iso8601 (entry->updated);
  id_iri = ta_iri_to_string (entry->id);

  ik = iks_new ("entry");
  iks_insert_attrib (ik, "xmlns", TA_ATOM_NS);
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
      free (published);
    }

  if (entry->rights)
    iks_insert_cdata (iks_insert (ik, "rights"), entry->rights, 0);

  if (entry->authors)
    for (tmp = entry->authors; tmp; tmp = tmp->next)
      {
        iks *authors =
          ta_atom_person_to_iks (tmp->data, "author");
        iks_insert_node (ik, authors);
      }
  if (entry->categories)
    for (tmp = entry->categories; tmp; tmp = tmp->next)
      {
        iks *categories =
          ta_atom_category_to_iks (tmp->data);
        iks_insert_node (ik, categories);
      }
  if (entry->links)
    for (tmp = entry->links; tmp; tmp = tmp->next)
      {
        iks *links =
          ta_atom_link_to_iks (tmp->data);
        iks_insert_node (ik, links);
      }
  if (entry->in_reply_to)
    for (tmp = entry->in_reply_to; tmp; tmp = tmp->next)
      {
        iks *irt = ta_atom_in_reply_to_to_iks (tmp->data);
        iks_insert_node (ik, irt);
      }
  if (entry->summary)
    iks_insert_cdata (iks_insert (ik, "summary"), entry->summary, 0);
  if (entry->content)
    iks_insert_node (ik, ta_atom_content_to_iks (entry->content));
  return ik;
}

char *
ta_atom_entry_to_string (ta_atom_entry_t *entry)
{
  iks *ik = ta_atom_entry_to_iks (entry);
  return _get_iks_string (ik);
}

int
ta_atom_entry_to_file (ta_atom_entry_t *entry,
                       const char *fname)
{
  return iks_save (fname, ta_atom_entry_to_iks (entry));
}

const char *
ta_atom_entry_get_title (ta_atom_entry_t *entry)
{
  return (const char *) entry->title;
}

void
ta_atom_entry_set_title (ta_atom_entry_t *entry,
                         const char *title)
{
  if (entry->title)
    free (entry->title);
  entry->title = strdup (title);
}

ta_iri_t *
ta_atom_entry_get_id (ta_atom_entry_t *entry)
{
  return entry->id;
}

void
ta_atom_entry_set_id (ta_atom_entry_t *entry, ta_iri_t *id)
{
  if (entry->id)
    ta_object_unref (entry->id);
  entry->id = id;
}

time_t
ta_atom_entry_get_updated (ta_atom_entry_t *entry)
{
  return entry->updated;
}

void
ta_atom_entry_set_updated (ta_atom_entry_t *entry,
                           time_t      updated)
{
  entry->updated = updated;
}

time_t
ta_atom_entry_get_published (ta_atom_entry_t *entry)
{
  return entry->published;
}

void
ta_atom_entry_set_published (ta_atom_entry_t *entry,
                             time_t      published)
{
  entry->published = published;
}

const char *
ta_atom_entry_get_rights (ta_atom_entry_t *entry)
{
  return entry->rights;
}

void
ta_atom_entry_set_rights (ta_atom_entry_t *entry,
                          const char *rights)
{
  if (entry->rights)
    free (entry->rights);
  entry->rights = strdup (rights);
}

ta_list_t *
ta_atom_entry_get_authors (ta_atom_entry_t *entry)
{
  return entry->authors;
}

void
ta_atom_entry_add_author (ta_atom_entry_t  *entry,
                          ta_atom_person_t *author)
{
  entry->authors = ta_list_append (entry->authors, author);
}

void
ta_atom_entry_del_authors (ta_atom_entry_t *entry)
{
  ta_list_t *tmp;
  for (tmp = entry->authors; tmp; tmp = tmp->next)
    ta_object_unref (tmp->data);
  ta_list_free (entry->authors);
  entry->authors = NULL;
}

ta_list_t *
ta_atom_entry_get_categories (ta_atom_entry_t *entry)
{
  return entry->categories;
}

void
ta_atom_entry_add_category (ta_atom_entry_t    *entry,
                            ta_atom_category_t *category)
{
  entry->categories = ta_list_append (entry->categories, category);
}

void
ta_atom_entry_del_categories (ta_atom_entry_t *entry)
{
  ta_list_t *tmp;
  for (tmp = entry->categories; tmp; tmp = tmp->next)
    ta_object_unref (tmp->data);
  ta_list_free (entry->categories);
  entry->categories = NULL;
}

ta_list_t *
ta_atom_entry_get_links (ta_atom_entry_t *entry)
{
  return entry->links;
}

void
ta_atom_entry_add_link (ta_atom_entry_t *entry,
                        ta_atom_link_t  *link)
{
  entry->links = ta_list_append (entry->links, link);
}

void
ta_atom_entry_del_links (ta_atom_entry_t *entry)
{
  ta_list_t *tmp;
  for (tmp = entry->links; tmp; tmp = tmp->next)
    ta_object_unref (tmp->data);
  ta_list_free (entry->links);
  entry->links = NULL;
}

const char *
ta_atom_entry_get_summary (ta_atom_entry_t *entry)
{
  return (const char *) entry->summary;
}

void
ta_atom_entry_set_summary (ta_atom_entry_t *entry,
                           const char *summary)
{
  if (entry->summary)
    free (entry->summary);
  if (summary)
    entry->summary = strdup (summary);
  else
    entry->summary = NULL;
}

ta_atom_content_t *
ta_atom_entry_get_content (ta_atom_entry_t *entry)
{
  return entry->content;
}

void
ta_atom_entry_set_content (ta_atom_entry_t   *entry,
                           ta_atom_content_t *content)
{
  if (entry->content)
    ta_object_unref (entry->content);
  entry->content = content;
}

void
ta_atom_entry_add_see (ta_atom_entry_t *entry,
                       ta_atom_simple_element_t *element)
{
  entry->ext_elements = ta_list_append (entry->ext_elements, element);
}

void
ta_atom_entry_del_see (ta_atom_entry_t *entry)
{
  ta_list_t *tmp;
  for (tmp = entry->ext_elements; tmp; tmp = tmp->next)
    ta_atom_simple_element_free (tmp->data);
  ta_list_free (entry->ext_elements);
  entry->ext_elements = NULL;
}

ta_list_t *
ta_atom_entry_get_see (ta_atom_entry_t *entry)
{
  return entry->ext_elements;
}

void
ta_atom_entry_add_inreplyto (ta_atom_entry_t *entry,
                             ta_atom_in_reply_to_t *irt)
{
  entry->in_reply_to = ta_list_append (entry->in_reply_to, irt);
}

void
ta_atom_entry_del_inreplyto (ta_atom_entry_t *entry)
{
  ta_list_t *tmp;
  for (tmp = entry->in_reply_to; tmp; tmp = tmp->next)
    ta_object_unref (tmp->data);
  ta_list_free (entry->in_reply_to);
  entry->ext_elements = NULL;
}

ta_list_t *
ta_atom_entry_get_inreplyto (ta_atom_entry_t *entry)
{
  return entry->in_reply_to;
}

/* ta_atom_feed_t */

static void
ta_atom_feed_free (ta_atom_feed_t *feed)
{
  if (feed->id)
    ta_object_unref (feed->id);
  if (feed->title)
    free (feed->title);
  if (feed->authors)
    ta_atom_feed_del_authors (feed);
  if (feed->categories)
    ta_atom_feed_del_categories (feed);
  if (feed->entries)
    ta_atom_feed_del_entries (feed);
  if (feed->error)
    ta_object_unref (feed->error);
  free (feed);
}

void
ta_atom_feed_init (ta_atom_feed_t *feed, const char *title)
{
  ta_object_init (TA_CAST_OBJECT (feed), (ta_free_func_t) ta_atom_feed_free);
  feed->title = title ? strdup (title) : NULL;
  feed->id = NULL;
  feed->updated = time (0);
  feed->authors = NULL;
  feed->categories = NULL;
  feed->entries = NULL;
  feed->error = NULL;
}

ta_atom_feed_t *
ta_atom_feed_new (const char *title)
{
  ta_atom_feed_t *feed;
  feed = malloc (sizeof (ta_atom_feed_t));
  ta_atom_feed_init (feed, title);
  return feed;
}

int
ta_atom_feed_set_from_file (ta_atom_feed_t *feed, const char *fname)
{
  iks *ik;
  int err, result;
  if ((err = iks_load (fname, &ik)) != IKS_OK)
    {
      if (feed->error)
        ta_object_unref (feed->error);
      feed->error = ta_error_new ();
      switch (err)
        {
        case IKS_NOMEM:
          ta_error_set_full (feed->error, TA_ATOM_LOAD_ERROR, "LoadError",
                             "Not enough memory to load file");
        case IKS_BADXML:
          ta_error_set_full (feed->error, TA_ATOM_LOAD_ERROR, "LoadError",
                             "Unable to parse xml file");
        default:
          ta_error_set_full (feed->error, TA_ATOM_LOAD_ERROR, "LoadError",
                             "Unknown error");
        }
      return 0;
    }
  result = ta_atom_feed_set_from_iks (feed, ik);
  iks_delete (ik);
  return result;
}

int
ta_atom_feed_set_from_iks (ta_atom_feed_t *feed, iks *ik)
{
  ta_iri_t *eid;
  iks *child;
  char *id, *title, *updated;

  if (strcmp (iks_name (ik), "feed") ||
      !iks_has_children (ik))
    {
      iks_delete (ik);
      if (feed->error)
        ta_object_unref (feed->error);
      feed->error = ta_error_new ();
      ta_error_set_full (feed->error, TA_ATOM_PARSING_ERROR, "ParsingError",
                         "Wrong root feed element");
      return 0;
    }
  id = iks_find_cdata (ik, "id");
  if (!id)
    {
      if (feed->error)
        ta_object_unref (feed->error);
      feed->error = ta_error_new ();
      ta_error_set_full (feed->error, TA_ATOM_PARSING_ERROR, "ParsingError",
                         "No <id> element found");
      return 0;
    }
  eid = ta_iri_new ();
  if (!ta_iri_set_from_string (eid, id))
    {
      if (feed->error)
        ta_object_unref (feed->error);
      feed->error = ta_error_new ();
      ta_error_set_full (feed->error, TA_ATOM_PARSING_ERROR, "ParsingError",
                         "Invalid <id> iri");
      ta_object_unref (eid);
      return 0;
    }
  title = iks_find_cdata (ik, "title");
  if (!title)
    {
      if (feed->error)
        ta_object_unref (feed->error);
      feed->error = ta_error_new ();
      ta_error_set_full (feed->error, TA_ATOM_PARSING_ERROR, "ParsingError",
                         "No <title> element found");
      return 0;
    }

  /* Here we have almost all required fields */
  ta_atom_feed_set_id (feed, eid);
  ta_atom_feed_set_title (feed, title);

  updated = iks_find_cdata (ik, "updated");
  if (updated)
    {
      time_t updatedt;
      updatedt = iso8601_to_time (updated);
      ta_atom_feed_set_updated (feed, updatedt);
    }

  /* Looking for more structured data */
  for (child = iks_child (ik); child; child = iks_next (child))
    {
      if (!strcmp (iks_name (child), "author"))
        {
          char *name, *email = NULL, *uri = NULL;
          ta_iri_t *iri = NULL;
          ta_atom_person_t *author;
          name = iks_find_cdata (child, "name");

          /* Specification is clear, an ta_atom:author element *MUST*
           * have a name. */
          if (!name)
            {
              if (feed->error)
                ta_object_unref (feed->error);
              feed->error = ta_error_new ();
              ta_error_set_full (feed->error, TA_ATOM_PARSING_ERROR,
                                 "ParsingError", "Author with no name");
              return 0;
            }
          email = iks_find_cdata (child, "email");
          uri = iks_find_cdata (child, "uri");
          iri = ta_iri_new ();
          if (uri)
            ta_iri_set_from_string (iri, uri);

          /* Like above, specification denies invalid iris in an ta_atom
           * person. */
          if (uri && !ta_iri_set_from_string (iri, uri))
            {
              if (feed->error)
                ta_object_unref (feed->error);
              feed->error = ta_error_new ();
              ta_error_set_full (feed->error, TA_ATOM_PARSING_ERROR,
                                 "ParsingError",
                                 "Author with an invalid iri in uri field");
              ta_object_unref (iri);
              return 0;
            }
          author = ta_atom_person_new (name, email, iri);
          ta_atom_feed_add_author (feed, author);
        }
      else if (!strcmp (iks_name (child), "category"))
        {
          ta_atom_category_t *cat;
          ta_iri_t *iri = NULL;
          char *term = NULL, *label = NULL, *scheme = NULL;
          term = iks_find_attrib (child, "term");
          label = iks_find_attrib (child, "label");
          scheme = iks_find_attrib (child, "scheme");
          if (!term)
            {
              if (feed->error)
                ta_object_unref (feed->error);
              feed->error = ta_error_new ();
              ta_error_set_full (feed->error, TA_ATOM_PARSING_ERROR,
                                 "ParsingError",
                                 "Category with no term attribute");
              return 0;
            }
          if (scheme)
            {
              iri = ta_iri_new ();
              if (!ta_iri_set_from_string (iri, scheme))
                {
                  if (feed->error)
                    ta_object_unref (feed->error);
                  feed->error = ta_error_new ();
                  ta_error_set_full (feed->error, TA_ATOM_PARSING_ERROR,
                                     "ParsingError",
                                     "Category scheme attribute is not a "
                                     "valid iri");
                  ta_object_unref (iri);
                  return 0;
                }
            }
          cat = ta_atom_category_new (term, label, iri);
          ta_atom_feed_add_category (feed, cat);
        }
      else if (!strcmp (iks_name (child), "entry"))
        {
          ta_atom_entry_t *entry;
          ta_error_t *error;

          entry = ta_atom_entry_new (NULL);
          ta_atom_entry_set_from_iks (entry, child);
          if ((error = ta_atom_entry_get_error (entry)) != NULL)
            {
              ta_object_unref (entry);
              continue;
            }
          ta_atom_feed_add_entry (feed, entry);
        }
    }
  return 1;
}

ta_error_t *
ta_atom_feed_get_error (ta_atom_feed_t *feed)
{
  return feed->error;
}

iks *
ta_atom_feed_to_iks (ta_atom_feed_t *feed)
{
  iks *ik;
  char *updated, *id_iri;
  ta_list_t *tmp;

  if (feed->id == NULL)
    return NULL;

  updated = time_to_iso8601 (feed->updated);
  id_iri = ta_iri_to_string (feed->id);

  ik = iks_new ("feed");
  iks_insert_attrib (ik, "xmlns", TA_ATOM_NS);
  iks_insert_cdata (iks_insert (ik, "id"), id_iri, 0);
  iks_insert_cdata (iks_insert (ik, "title"), feed->title, 0);
  iks_insert_cdata (iks_insert (ik, "updated"), updated, 0);
  free (updated);
  free (id_iri);
  if (feed->authors)
    for (tmp = feed->authors; tmp; tmp = tmp->next)
      {
        iks *authors =
          ta_atom_person_to_iks (tmp->data, "author");
        iks_insert_node (ik, authors);
      }
  if (feed->categories)
    for (tmp = feed->categories; tmp; tmp = tmp->next)
      {
        iks *categories =
          ta_atom_category_to_iks (tmp->data);
        iks_insert_node (ik, categories);
      }
  if (feed->entries)
    for (tmp = feed->entries; tmp; tmp = tmp->next)
      {
        iks *entries =
          ta_atom_entry_to_iks (tmp->data);
        iks_insert_node (ik, entries);
      }
  return ik;
}

char *
ta_atom_feed_to_string (ta_atom_feed_t *feed)
{
  iks *ik = ta_atom_feed_to_iks (feed);
  return _get_iks_string (ik);
}

int
ta_atom_feed_to_file (ta_atom_feed_t  *feed,
                      const char *fname)
{
  return iks_save (fname, ta_atom_feed_to_iks (feed));
}

const char *
ta_atom_feed_get_title (ta_atom_feed_t *feed)
{
  return feed->title;
}

void
ta_atom_feed_set_title (ta_atom_feed_t  *feed,
                        const char *title)
{
  if (feed->title)
    free (feed->title);
  feed->title = strdup (title);
}

ta_iri_t *
ta_atom_feed_get_id (ta_atom_feed_t *feed)
{
  return feed->id;
}

void
ta_atom_feed_set_id (ta_atom_feed_t *feed, ta_iri_t *id)
{
  if (feed->id)
    ta_object_unref (feed->id);
  feed->id = id;
}

time_t
ta_atom_feed_get_updated (ta_atom_feed_t *feed)
{
  return feed->updated;
}

void
ta_atom_feed_set_updated (ta_atom_feed_t *feed,
                          time_t     updated)
{
  feed->updated = updated;
}

ta_list_t *
ta_atom_feed_get_authors (ta_atom_feed_t *feed)
{
  return feed->authors;
}

void
ta_atom_feed_add_author (ta_atom_feed_t  *feed,
                         ta_atom_person_t *author)
{
  feed->authors = ta_list_append (feed->authors, author);
}

void
ta_atom_feed_del_authors (ta_atom_feed_t *feed)
{
  ta_list_t *node = feed->authors, *tmp = NULL;
  while (node)
    {
      tmp = node;
      node = node->next;
      ta_object_unref (tmp->data);
      free (tmp);
    }
  feed->authors = NULL;
}

ta_list_t *
ta_atom_feed_get_categories (ta_atom_feed_t *feed)
{
  return feed->categories;
}

void
ta_atom_feed_add_category (ta_atom_feed_t    *feed,
                           ta_atom_category_t *category)
{
  feed->categories = ta_list_append (feed->categories, category);
}

void
ta_atom_feed_del_categories (ta_atom_feed_t *feed)
{
  ta_list_t *tmp;
  for (tmp = feed->categories; tmp; tmp = tmp->next)
    ta_object_unref (tmp->data);
  ta_list_free (feed->categories);
  feed->categories = NULL;
}

ta_list_t *
ta_atom_feed_get_links (ta_atom_feed_t *feed)
{
  return feed->links;
}

void
ta_atom_feed_add_link (ta_atom_feed_t *feed, ta_atom_link_t *link)
{
  feed->links = ta_list_append (feed->links, link);
}

void
ta_atom_feed_del_links (ta_atom_feed_t *feed)
{
  ta_list_t *tmp;
  for (tmp = feed->links; tmp; tmp = tmp->next)
    ta_object_unref (tmp->data);
  ta_list_free (feed->links);
  feed->links = NULL;
}

ta_list_t *
ta_atom_feed_get_entries (ta_atom_feed_t *feed)
{
  return feed->entries;
}

void
ta_atom_feed_add_entry (ta_atom_feed_t  *feed,
                        ta_atom_entry_t *entry)
{
  feed->entries = ta_list_append (feed->entries, entry);
}

void
ta_atom_feed_del_entries (ta_atom_feed_t *feed)
{
  ta_list_t *node = feed->entries, *tmp = NULL;
  while (node)
    {
      tmp = node;
      node = node->next;
      ta_object_unref (tmp->data);
      free (tmp);
    }
  feed->entries = NULL;
}
