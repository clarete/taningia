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

#ifndef _TANINGIA_ATOM_H_
#define _TANINGIA_ATOM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <iksemel.h>
#include <taningia/object.h>
#include <taningia/iri.h>
#include <taningia/list.h>

#define TA_ATOM_NS "http://www.w3.org/2005/Atom"
#define TA_ATOM_THREADING_NS "http://purl.org/syndication/thread/1.0"

typedef struct
{
  ta_object_t parent;
  char *name;
  char *value;
} ta_atom_simple_element_t;

typedef struct
{
  ta_object_t parent;
  ta_iri_t *ref;
  ta_iri_t *href;
  ta_iri_t *source;
  char *type;
} ta_atom_in_reply_to_t;

typedef struct
{
  ta_object_t parent;
  ta_iri_t *href;
  char *title;
  char *rel;
  char *type;
  char *length;
} ta_atom_link_t;

typedef struct
{
  ta_object_t parent;
  char *type;
  char *content;
  int len;
  ta_iri_t *src;
} ta_atom_content_t;

typedef struct
{
  ta_object_t parent;
  char *name;
  char *email;
  ta_iri_t *iri;
  ta_list_t *ext_elements;
} ta_atom_person_t;

typedef struct
{
  ta_object_t parent;
  char *label;
  char *term;
  ta_iri_t *scheme;
} ta_atom_category_t;

typedef struct
{
  ta_object_t parent;
  ta_iri_t *id;
  char *title;
  time_t updated;
  time_t published;
  char *rights;
  ta_list_t *authors;
  ta_list_t *categories;
  ta_list_t *links;
  char *summary;
  ta_atom_content_t *content;
  ta_list_t *ext_elements;
  ta_list_t *in_reply_to;
} ta_atom_entry_t;

typedef struct
{
  ta_object_t parent;
  ta_iri_t *id;
  char *title;
  time_t updated;
  ta_list_t *authors;
  ta_list_t *categories;
  ta_list_t *entries;
  ta_list_t *links;
  ta_list_t *ext_elements;
} ta_atom_feed_t;

/* -- Atom Simple Ext Element -- */

/**
 * @name: ta_atom_simple_element::new
 * @type: constructor
 */
ta_atom_simple_element_t *ta_atom_simple_element_new (const char *name,
                                                      const char *value);

/**
 * @name: ta_atom_simple_element::init
 * @type: initializer
 */
void ta_atom_simple_element_init (ta_atom_simple_element_t *see,
                                  const char *name, const char *value);

/**
 * @name: ta_atom_simple_element::to_iks
 * @type: method
 *
 * Returns an iks representation of a simple element.
 */
iks *ta_atom_simple_element_to_iks (ta_atom_simple_element_t *see);

/**
 * @name: ta_atom_simple_element::to_string
 * @type: method
 *
 * Returns a string representation of a simple element.
 */
char *ta_atom_simple_element_to_string (ta_atom_simple_element_t *sse);

/**
 * @name: ta_atom_simple_element::get_name
 * @type: getter
 */
const char *ta_atom_simple_element_get_name (ta_atom_simple_element_t *sse);

/**
 * @name: ta_atom_simple_element::set_name
 * @type: setter
 */
void ta_atom_simple_element_set_name (ta_atom_simple_element_t *sse,
                                      const char *name);

/**
 * @name: ta_atom_simple_element::get_value
 * @type: getter
 */
const char *ta_atom_simple_element_get_value (ta_atom_simple_element_t *sse);

/**
 * @name: ta_atom_simple_element::set_value
 * @type: setter
 */
void ta_atom_simple_element_set_value (ta_atom_simple_element_t *sse,
                                       const char *value);

/* -- Atom Link -- */

/**
 * @name: ta_atom_link::new
 * @type: constructor
 * @param href: Link target.
 */
ta_atom_link_t *ta_atom_link_new (ta_iri_t *href);

/**
 * @name: ta_atom_link::init
 * @type: initializer
 */
void ta_atom_link_init (ta_atom_link_t *link, ta_iri_t *href);

/**
 * @name: ta_atom_link::to_iks
 * @type: method
 *
 * This method returns an iks representation of the link instance.
 */
iks *ta_atom_link_to_iks (ta_atom_link_t *link);

/**
 * @name: ta_atom_link::to_string
 * @type: method
 *
 * This method returns an xml representation of the link instance.
 */
char *ta_atom_link_to_string (ta_atom_link_t *link);

/**
 * @name: ta_atom_link::get_href
 * @type: getter
 */
ta_iri_t *ta_atom_link_get_href (ta_atom_link_t *link);

/**
 * @name: ta_atom_link::set_href
 * @type: setter
 */
void ta_atom_link_set_href (ta_atom_link_t *link, ta_iri_t *href);

/**
 * @name: ta_atom_link::get_title
 * @type: getter
 */
const char *ta_atom_link_get_title (ta_atom_link_t *link);

/**
 * @name: ta_atom_link::set_title
 * @type: setter
 */
void ta_atom_link_set_title (ta_atom_link_t *link, const char *title);

/**
 * @name: ta_atom_link::get_rel
 * @type: getter
 */
const char *ta_atom_link_get_rel (ta_atom_link_t *link);

/**
 * @name: ta_atom_link::set_rel
 * @type: setter
 */
void ta_atom_link_set_rel (ta_atom_link_t *link, const char *rel);

/**
 * @name: ta_atom_link::get_type
 * @type: getter
 */
const char *ta_atom_link_get_type (ta_atom_link_t *link);

/**
 * @name: ta_atom_link::set_type
 * @type: setter
 */
void ta_atom_link_set_type (ta_atom_link_t *link, const char *type);

/**
 * @name: ta_atom_link::get_length
 * @type: getter
 */
const char *ta_atom_link_get_length (ta_atom_link_t *link);

/**
 * @name: ta_atom_link::set_length
 * @type: setter
 */
void ta_atom_link_set_length (ta_atom_link_t *link, const char *length);

/* -- Atom Content -- */

/**
 * @name: ta_atom_content::new
 * @type: constructor
 * @param type (optional): Mime type of the content.
 */
ta_atom_content_t *ta_atom_content_new (const char *type);

/**
 * @name: ta_atom_content::init
 * @type: initializer
 */
void ta_atom_content_init (ta_atom_content_t *content, const char *type);

/**
 * @name: ta_atom_content::to_iks
 * @type: method
 *
 * This method returns an iks representation of the content
 * instance.
 */
iks *ta_atom_content_to_iks (ta_atom_content_t *content);

/**
 * @name: ta_atom_content::to_string
 * @type: method
 *
 * This method returns an xml representation of the content
 * instance.
 */
char *ta_atom_content_to_string (ta_atom_content_t *content);

/**
 * @name: ta_atom_content::get_type
 * @type: getter
 */
const char *ta_atom_content_get_type (ta_atom_content_t *content);

/**
 * @name: ta_atom_content::set_type
 * @type: setter
 */
void ta_atom_content_set_type (ta_atom_content_t *content, const char *type);

/**
 * @name: ta_atom_content::get_src
 * @type: getter
 */
ta_iri_t *ta_atom_content_get_src (ta_atom_content_t *content);

/**
 * @name: ta_atom_content::set_src
 * @type: setter
 */
void ta_atom_content_set_src (ta_atom_content_t *content, ta_iri_t *src);

/**
 * @name: ta_atom_content::get_content
 * @type: getter
 */
const char *ta_atom_content_get_content (ta_atom_content_t *content, int *len);

/**
 * @name: ta_atom_content::set_content
 * @type: setter
 */
void ta_atom_content_set_content (ta_atom_content_t *content, const char *text,
                                  int len);

/* -- Atom Person -- */

/**
 * @name: ta_atom_person::new
 * @type: constructor
 * @param name: The person name.
 * @param email (optional): Person's email address.
 * @param iri (optional): The iri of the person's website.
 */
ta_atom_person_t *ta_atom_person_new (const char *name,
                                      const char *email,
                                      ta_iri_t *iri);

/**
 * @name: ta_atom_person::init
 * @type: initializer
 */
void ta_atom_person_init (ta_atom_person_t *person,
                          const char *name,
                          const char *email,
                          ta_iri_t *iri);

/**
 * @name: ta_atom_person::to_iks
 * @type: method
 * @param element: The element name.
 *
 * This method returns an iks representation of the person
 * instance. And the `element' parameter will be used as its
 * root node name.
 */
iks *ta_atom_person_to_iks (ta_atom_person_t *person, const char *element);

/**
 * @name: ta_atom_person::to_string
 * @type: method
 * @param element: The element name.
 *
 * This method returns an xml representation of the person
 * instance. The root node of this representation is named with the
 * `element' parameter.
 */
char *ta_atom_person_to_string (ta_atom_person_t *person, const char *element);

/**
 * @name: ta_atom_person::get_name
 * @type: getter
 */
const char *ta_atom_person_get_name (ta_atom_person_t *person);

/**
 * @name: ta_atom_person::set_name
 * @type: setter
 */
void ta_atom_person_set_name (ta_atom_person_t *person, const char *name);

/**
 * @name: ta_atom_person::get_email
 * @type: getter
 */
const char *ta_atom_person_get_email (ta_atom_person_t *person);

/**
 * @name: ta_atom_person::set_email
 * @type: setter
 */
void ta_atom_person_set_email (ta_atom_person_t *person, const char *email);

/**
 * @name: ta_atom_person::get_iri
 * @type: getter
 */
ta_iri_t *ta_atom_person_get_iri (ta_atom_person_t *person);

/**
 * @name: ta_atom_person::set_iri
 * @type: setter
 */
void ta_atom_person_set_iri (ta_atom_person_t *person, ta_iri_t *iri);

/**
 * @name: ta_atom_person::add_see
 * @type: method
 * @param element: The element that will be added to the person.
 *
 * Adds an atom simple ext element to the person instance.
 */
void ta_atom_person_add_see (ta_atom_person_t *person,
                             ta_atom_simple_element_t *element);

/**
 * @name: ta_atom_person::del_see
 * @type: method
 *
 * Removes all atom simple ext elements from the person instance.
 */
void ta_atom_person_del_see (ta_atom_person_t *person);

/**
 * @name: ta_atom_person::get_see
 * @type: method
 * @param person: A person instance.
 * @return: ta_list (ta_atom_simple_element)
 *
 * Gets all simple extension elements associated with a person
 * instance.
 **/
ta_list_t *ta_atom_person_get_see (ta_atom_person_t *person);

/* -- Atom Category -- */

/**
 * @name: ta_atom_category::new
 * @type: constructor
 * @param term: The category term.
 * @param label (optional): A human readable string containing the
 * category label.
 * @param scheme (optional): An iri pointing to a reachable category resource.
 */
ta_atom_category_t *ta_atom_category_new (const char *term, const char *label,
                                          ta_iri_t *scheme);

/**
 * @name: ta_atom_category::init
 * @type: initializer
 */
void ta_atom_category_init (ta_atom_category_t *category,
                            const char *term,
                            const char *label,
                            ta_iri_t *scheme);

/**
 * @name: ta_atom_category::to_iks
 * @type: method
 *
 * This method returns an iks representation of the category
 * instance.
 */
iks *ta_atom_category_to_iks (ta_atom_category_t *category);

/**
 * @name: ta_atom_category::to_string
 * @type: method
 *
 * This method returns an xml representation of the category
 * instance.
 */
char *ta_atom_category_to_string (ta_atom_category_t *category);

/**
 * @name: ta_atom_category::get_label
 * @type: getter
 */
const char *ta_atom_category_get_label (ta_atom_category_t  *category);

/**
 * @name: ta_atom_category::set_label
 * @type: setter
 */
void ta_atom_category_set_label (ta_atom_category_t *category,
                                 const char *label);

/**
 * @name: ta_atom_category::get_term
 * @type: getter
 */
const char *ta_atom_category_get_term (ta_atom_category_t *category);

/**
 * @name: ta_atom_category::set_term
 * @type: setter
 */
void ta_atom_category_set_term (ta_atom_category_t *category,
                                const char *term);

/**
 * @name: ta_atom_category::get_scheme
 * @type: getter
 */
ta_iri_t *ta_atom_category_get_scheme (ta_atom_category_t *category);

/**
 * @name: ta_atom_category::set_scheme
 * @type: setter
 */
void ta_atom_category_set_scheme (ta_atom_category_t *category,
                                  ta_iri_t *scheme);

/* -- Atom Thread In reply to -- */

/**
 * @name: ta_atom_in_reply_to::new
 * @type: constructor
 * @param ref: The IRI reference to the entry that is responsed to.
 */
ta_atom_in_reply_to_t *ta_atom_in_reply_to_new (ta_iri_t *ref);

/**
 * @name: ta_atom_in_reply_to::init
 * @type: initializer
 */
void ta_atom_in_reply_to_init (ta_atom_in_reply_to_t *irt,
                               ta_iri_t *ref);

/**
 * @name: ta_atom_in_reply_to::to_iks
 * @type: method
 *
 * Returns an iks representation of the in reply to instance.
 */
iks *ta_atom_in_reply_to_to_iks (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to::to_string
 * @type: method
 *
 * Returns an xmp representation of the in reply to instance.
 */
char *ta_atom_in_reply_to_to_string (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to::get_ref
 * @type: getter
 */
ta_iri_t *ta_atom_in_reply_to_get_ref (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to::set_ref
 * @type: setter
 */
void ta_atom_in_reply_to_set_ref (ta_atom_in_reply_to_t *irt, ta_iri_t *ref);

/**
 * @name: ta_atom_in_reply_to::get_href
 * @type: getter
 */
ta_iri_t *ta_atom_in_reply_to_get_href (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to::set_href
 * @type: setter
 */
void ta_atom_in_reply_to_set_href (ta_atom_in_reply_to_t *irt, ta_iri_t *href);

/**
 * @name: ta_atom_in_reply_to::get_source
 * @type: getter
 */
ta_iri_t *ta_atom_in_reply_to_get_source (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to::set_source
 * @type: setter
 */
void ta_atom_in_reply_to_set_source (ta_atom_in_reply_to_t *irt,
                                     ta_iri_t *source);

/**
 * @name: ta_atom_in_reply_to::get_type
 * @type: getter
 */
const char *ta_atom_in_reply_to_get_type (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to::set_type
 * @type: setter
 */
void ta_atom_in_reply_to_set_type (ta_atom_in_reply_to_t *irt,
                                   const char *type);

/* -- Atom Entry -- */

/**
 * @name: ta_atom_entry::new
 * @type: constructor
 * @param title (optional): The entry title
 */
ta_atom_entry_t *ta_atom_entry_new (const char *title);

/**
 * @name: ta_atom_entry::init
 * @type: initializer
 */
void ta_atom_entry_init (ta_atom_entry_t *entry, const char *title);

/**
 * @name: ta_atom_entry::set_from_file
 * @type: method
 * @param fname: Name of the file to be loaded
 * @raise: TA_ATOM_LOAD_ERROR, TA_ATOM_PARSING_ERROR
 *
 * Load a file to an atom entry.
 */
int ta_atom_entry_set_from_file (ta_atom_entry_t *entry, const char *fname);

/**
 * @name: ta_atom_entry::set_from_iks
 * @type: method
 * @param iks: iks object to be parsed
 * @raise: TA_ATOM_PARSING_ERROR
 *
 * Load an iks xml structure to an atom entry.
 */
int ta_atom_entry_set_from_iks (ta_atom_entry_t *entry, iks *ik);

/**
 * @name: ta_atom_entry::to_iks
 * @type: method
 *
 * Returns an iks representation of an atom entry.
 */
iks *ta_atom_entry_to_iks (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::to_string
 * @type: method
 *
 * Returns a xml string representation of an atom entry.
 */
char *ta_atom_entry_to_string (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::to_file
 * @type: method
 * @param fname: Output file name.
 *
 * Saves the xml representation of the entry instance to a file.
 */
int ta_atom_entry_to_file (ta_atom_entry_t *entry, const char *fname);

/**
 * @name: ta_atom_entry::get_title
 * @type: getter
 */
const char *ta_atom_entry_get_title (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::set_title
 * @type: setter
 */
void ta_atom_entry_set_title (ta_atom_entry_t *entry, const char *title);

/**
 * @name: ta_atom_entry::get_id
 * @type: getter
 */
ta_iri_t *ta_atom_entry_get_id (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::set_id
 * @type: setter
 */
void ta_atom_entry_set_id (ta_atom_entry_t *entry, ta_iri_t *id);

/**
 * @name: ta_atom_entry::get_updated
 * @type: getter
 */
time_t ta_atom_entry_get_updated (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::set_updated
 * @type: setter
 */
void ta_atom_entry_set_updated (ta_atom_entry_t *entry, time_t updated);

/**
 * @name: ta_atom_entry::get_published
 * @type: getter
 */
time_t ta_atom_entry_get_published (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::set_published
 * @type: setter (nullable)
 */
void ta_atom_entry_set_published (ta_atom_entry_t *entry, time_t published);

/**
 * @name: ta_atom_entry::get_rights
 * @type: getter
 */
const char *ta_atom_entry_get_rights (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::set_rights
 * @type: setter
 */
void ta_atom_entry_set_rights (ta_atom_entry_t *entry, const char *rights);

/**
 * @name: ta_atom_entry::get_summary
 * @type: getter
 */
const char *ta_atom_entry_get_summary (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::set_summary
 * @type: setter
 */
void ta_atom_entry_set_summary (ta_atom_entry_t *entry, const char *summary);

/**
 * @name: ta_atom_entry::get_content
 * @type: getter
 */
ta_atom_content_t *ta_atom_entry_get_content (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::set_content
 * @type: setter
 */
void ta_atom_entry_set_content (ta_atom_entry_t *entry,
                                ta_atom_content_t *content);

/**
 * @name: ta_atom_entry::get_authors
 * @type: method
 * @return: ta_list (ta_atom_person)
 *
 * Returns the list of authors of an atom entry instance.
 */
ta_list_t *ta_atom_entry_get_authors (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::add_author
 * @type: method
 * @param author: The author instance to be added.
 *
 * Adds an author instance to the entry. After that, you should not
 * free the author instance, it will be done by `ta_atom_entry_free'.
 */
void ta_atom_entry_add_author (ta_atom_entry_t *entry,
                               ta_atom_person_t *author);

/**
 * @name: ta_atom_entry::del_authors
 * @type: method
 *
 * Remove all authors from the entry. They will be freed too.
 */
void ta_atom_entry_del_authors (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::get_categories
 * @type: method
 * @return: ta_list (ta_atom_category)
 *
 * Returns the list of categories of an atom entry instance.
 */
ta_list_t *ta_atom_entry_get_categories (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::add_category
 * @type: method
 * @param category: The category instance to be added.
 *
 * Adds an category instance to the entry. After that, you should not
 * free the category instance, it will be done by `ta_atom_entry_free'.
 */
void ta_atom_entry_add_category (ta_atom_entry_t *entry,
                                 ta_atom_category_t *category);

/**
 * @name: ta_atom_entry::del_categories
 * @type: method
 *
 * Remove all categories from the entry. They will be freed too.
 */
void ta_atom_entry_del_categories (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::get_links
 * @type: method
 * @return: ta_list (ta_atom_link)
 *
 * Returns the list of links of an atom entry instance.
 */
ta_list_t *ta_atom_entry_get_links (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::add_link
 * @type: method
 * @param link: The link instance to be added.
 *
 * Adds an link instance to the entry. After that, you should not
 * free the link instance, it will be done by `ta_atom_entry_free'.
 */
void ta_atom_entry_add_link (ta_atom_entry_t *entry, ta_atom_link_t *link);

/**
 * @name: ta_atom_entry::del_links
 * @type: method
 *
 * Remove all links from the entry. They will be freed too.
 */
void ta_atom_entry_del_links (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::get_see
 * @type: method
 * @return: ta_list (ta_atom_simple_element)
 *
 * Returns the list of simple elements of an atom entry instance.
 */
ta_list_t *ta_atom_entry_get_see (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::add_see
 * @type: method
 * @param element: The element instance to be added.
 *
 * Adds a simple element instance to the entry. After that, you should
 * not free the element instance, it will be done by
 * `ta_atom_entry_free'.
 */
void ta_atom_entry_add_see (ta_atom_entry_t *entry,
                            ta_atom_simple_element_t *element);

/**
 * @name: ta_atom_entry::del_see
 * @type: method
 *
 * Remove all simple elements from the entry. They will be freed too.
 */
void ta_atom_entry_del_see (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::get_inreplyto
 * @type: method
 * @return: ta_list (ta_atom_in_reply_to)
 *
 * Returns the list of <code>in_reply_to</code> elements of an atom
 * entry instance.
 */
ta_list_t *ta_atom_entry_get_inreplyto (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry::add_inreplyto
 * @type: method
 * @param element: The in reply to instance to be added.
 *
 * Adds an <code>in_reply_to</code> instance to the entry. After that,
 * you should not free the added instance, it will be done by
 * `ta_atom_entry_free'.
 */
void ta_atom_entry_add_inreplyto (ta_atom_entry_t *entry,
                                  ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_entry::del_inreplyto
 * @type: method
 *
 * Remove all <em>in_reply_to</em> elements from the entry. They will
 * be freed too.
 */
void ta_atom_entry_del_inreplyto (ta_atom_entry_t *entry);

/* -- Atom Feed -- */

/**
 * @name: ta_atom_feed::new
 * @type: constructor
 * @param title (optional): The feed title
 */
ta_atom_feed_t *ta_atom_feed_new (const char *title);

/**
 * @name: ta_atom_feed::init
 * @type: initializer
 */
void ta_atom_feed_init (ta_atom_feed_t *feed, const char *title);

/**
 * @name: ta_atom_feed::set_from_file
 * @type: method
 * @param fname: Name of the file to be loaded
 * @raise: TA_ATOM_LOAD_ERROR, TA_ATOM_PARSING_ERROR
 *
 * Load a file to an atom feed.
 */
int
ta_atom_feed_set_from_file (ta_atom_feed_t  *feed,
                            const char *fname);

/**
 * @name: ta_atom_feed::set_from_iks
 * @type: method
 * @param iks: iks object to be parsed
 * @raise: TA_ATOM_PARSING_ERROR
 *
 * Load an iks xml structure to an atom feed.
 */
int
ta_atom_feed_set_from_iks (ta_atom_feed_t *feed, iks *ik);

/**
 * @name: ta_atom_feed::to_iks
 * @type: method
 *
 * Returns an iks representation of an atom feed.
 */
iks *ta_atom_feed_to_iks (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::to_string
 * @type: method
 *
 * Returns a xml string representation of an atom feed.
 */
char *ta_atom_feed_to_string (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::to_file
 * @type: method
 * @param fname: Output file name.
 *
 * Saves the xml representation of the feed instance to a file.
 */
int ta_atom_feed_to_file (ta_atom_feed_t *feed, const char *fname);

/**
 * @name: ta_atom_feed::get_title
 * @type: getter
 */
const char *ta_atom_feed_get_title (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::set_title
 * @type: setter
 */
void ta_atom_feed_set_title (ta_atom_feed_t *feed, const char *title);

/**
 * @name: ta_atom_feed::get_id
 * @type: getter
 */
ta_iri_t *ta_atom_feed_get_id (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::set_id
 * @type: setter
 */
void ta_atom_feed_set_id (ta_atom_feed_t *feed, ta_iri_t *id);

/**
 * @name: ta_atom_feed::get_updated
 * @type: getter
 */
time_t ta_atom_feed_get_updated (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::set_updated
 * @type: setter
 */
void ta_atom_feed_set_updated (ta_atom_feed_t *feed, time_t updated);

/**
 * @name: ta_atom_feed::get_authors
 * @type: method
 * @return: ta_list (ta_atom_person)
 *
 * Returns the list of authors of an atom feed instance.
 */
ta_list_t *ta_atom_feed_get_authors (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::add_author
 * @type: method
 * @param author: The author instance to be added.
 *
 * Adds an author instance to the feed. After that, you should not
 * free the author instance, it will be done by `ta_atom_feed_free'.
 */
void ta_atom_feed_add_author (ta_atom_feed_t *feed, ta_atom_person_t *author);

/**
 * @name: ta_atom_feed::del_authors
 * @type: method
 *
 * Remove all authors from the feed. They will be freed too.
 */
void ta_atom_feed_del_authors (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::get_categories
 * @type: method
 * @return: ta_list (ta_atom_category)
 *
 * Returns the list of categories of an atom feed instance.
 */
ta_list_t *ta_atom_feed_get_categories (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::add_category
 * @type: method
 * @param category: The category instance to be added.
 *
 * Adds an category instance to the feed. After that, you should not
 * free the category instance, it will be done by `ta_atom_feed_free'.
 */
void ta_atom_feed_add_category (ta_atom_feed_t *feed,
                                ta_atom_category_t *category);

/**
 * @name: ta_atom_feed::del_categories
 * @type: method
 *
 * Remove all categories from the feed. They will be freed too.
 */
void ta_atom_feed_del_categories (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::get_links
 * @type: method
 * @return: ta_list (ta_atom_link)
 *
 * Returns the list of links of an atom feed instance.
 */
ta_list_t *ta_atom_feed_get_links (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::add_link
 * @type: method
 * @param link: The link instance to be added.
 *
 * Adds an link instance to the feed. After that, you should not
 * free the link instance, it will be done by `ta_atom_feed_free'.
 */
void ta_atom_feed_add_link (ta_atom_feed_t *feed, ta_atom_link_t *link);

/**
 * @name: ta_atom_feed::del_links
 * @type: method
 *
 * Remove all links from the feed. They will be freed too.
 */
void ta_atom_feed_del_links (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::get_entries
 * @type: method
 * @return: ta_list (ta_atom_entry)
 *
 * Returns the list of entries of an atom feed instance.
 */
ta_list_t *ta_atom_feed_get_entries (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed::add_entry
 * @type: method
 * @param entry: The entry instance to be added.
 *
 * Adds an entry instance to the feed. After that, you should not
 * free the entry instance, it will be done by `ta_atom_feed_free'.
 */
void ta_atom_feed_add_entry (ta_atom_feed_t *feed, ta_atom_entry_t *entry);

/**
 * @name: ta_atom_feed::del_entries
 * @type: method
 *
 * Remove all entries from the feed. They will be freed too.
 */
void ta_atom_feed_del_entries (ta_atom_feed_t *feed);

#ifdef __cplusplus
}
#endif

#endif /* _TANINGIA_ATOM_H_ */
