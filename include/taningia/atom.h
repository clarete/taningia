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

#include <time.h>
#include <iksemel.h>
#include <taningia/iri.h>
#include <taningia/error.h>
#include <taningia/list.h>

#define TA_ATOM_NS "http://www.w3.org/2005/Atom"
#define TA_ATOM_THREADING_NS "http://purl.org/syndication/thread/1.0"

typedef struct _ta_atom_feed_t         ta_atom_feed_t;
typedef struct _ta_atom_entry_t        ta_atom_entry_t;
typedef struct _ta_atom_category_t     ta_atom_category_t;
typedef struct _ta_atom_person_t       ta_atom_person_t;
typedef struct _ta_atom_content_t      ta_atom_content_t;
typedef struct _ta_atom_link_t         ta_atom_link_t;
typedef struct _ta_atom_simple_element_t  ta_atom_simple_element_t;
typedef struct _ta_atom_in_reply_to_t  ta_atom_in_reply_to_t;

typedef enum {
  TA_ATOM_LOAD_ERROR,
  TA_ATOM_PARSING_ERROR
} ta_atom_error_t;

/* -- Atom Simple Ext Element -- */

/**
 * @name: ta_atom_simple_element_new
 * @type: constructor ta_atom_simple_element
 */
ta_atom_simple_element_t *ta_atom_simple_element_new (const char *name,
                                                      const char *value);

/**
 * @name: ta_atom_simple_element_free
 * @type: destructor ta_atom_simple_element
 */
void ta_atom_simple_element_free (ta_atom_simple_element_t *see);

/**
 * @name: ta_atom_simple_element_to_iks
 * @type: method ta_atom_simple_element
 *
 * Returns an iks representation of a simple element.
 */
iks *ta_atom_simple_element_to_iks (ta_atom_simple_element_t *see);

/**
 * @name: ta_atom_simple_element_to_string
 * @type: method ta_atom_simple_element
 *
 * Returns a string representation of a simple element.
 */
char *ta_atom_simple_element_to_string (ta_atom_simple_element_t *sse);

/**
 * @name: ta_atom_simple_element_get_name
 * @type: getter ta_atom_simple_element:name
 */
const char *ta_atom_simple_element_get_name (ta_atom_simple_element_t *sse);

/**
 * @name: ta_atom_simple_element_set_name
 * @type: setter ta_atom_simple_element:name
 */
void ta_atom_simple_element_set_name (ta_atom_simple_element_t *sse,
                                      const char *name);

/**
 * @name: ta_atom_simple_element_get_value
 * @type: getter ta_atom_simple_element:value
 */
const char *ta_atom_simple_element_get_value (ta_atom_simple_element_t *sse);

/**
 * @name: ta_atom_simple_element_set_value
 * @type: setter ta_atom_simple_element:value
 */
void ta_atom_simple_element_set_value (ta_atom_simple_element_t *sse,
                                       const char *value);

/* -- Atom Link -- */

/**
 * @name: ta_atom_link_new
 * @type: constructor ta_atom_link
 * @param href: Link target.
 */
ta_atom_link_t *ta_atom_link_new (ta_iri_t *href);

/**
 * @name: ta_atom_link_free
 * @type: destructor ta_atom_link
 */
void ta_atom_link_free (ta_atom_link_t *link);

/**
 * @name: ta_atom_link_to_iks
 * @type: method ta_atom_link
 *
 * This method returns an iks representation of the link instance.
 */
iks *ta_atom_link_to_iks (ta_atom_link_t *link);

/**
 * @name: ta_atom_link_to_string
 * @type: method ta_atom_link
 *
 * This method returns an xml representation of the link instance.
 */
char *ta_atom_link_to_string (ta_atom_link_t *link);

/**
 * @name: ta_atom_link_get_href
 * @type: getter ta_atom_link:href
 */
ta_iri_t *ta_atom_link_get_href (ta_atom_link_t *link);

/**
 * @name: ta_atom_link_set_href
 * @type: setter ta_atom_link:href
 */
void ta_atom_link_set_href (ta_atom_link_t *link, ta_iri_t *href);

/**
 * @name: ta_atom_link_get_title
 * @type: getter ta_atom_link:title
 */
const char *ta_atom_link_get_title (ta_atom_link_t *link);

/**
 * @name: ta_atom_link_set_title
 * @type: setter ta_atom_link:title
 */
void ta_atom_link_set_title (ta_atom_link_t *link, const char *title);

/**
 * @name: ta_atom_link_get_rel
 * @type: getter ta_atom_link:rel
 */
const char *ta_atom_link_get_rel (ta_atom_link_t *link);

/**
 * @name: ta_atom_link_set_rel
 * @type: setter ta_atom_link:rel
 */
void ta_atom_link_set_rel (ta_atom_link_t *link, const char *rel);

/**
 * @name: ta_atom_link_get_type
 * @type: getter ta_atom_link:type
 */
const char *ta_atom_link_get_type (ta_atom_link_t *link);

/**
 * @name: ta_atom_link_set_type
 * @type: setter ta_atom_link:type
 */
void ta_atom_link_set_type (ta_atom_link_t *link, const char *type);

/**
 * @name: ta_atom_link_get_length
 * @type: getter ta_atom_link:length
 */
const char *ta_atom_link_get_length (ta_atom_link_t *link);

/**
 * @name: ta_atom_link_set_length
 * @type: setter ta_atom_link:length
 */
void ta_atom_link_set_length (ta_atom_link_t *link, const char *length);

/* -- Atom Content -- */

/**
 * @name: ta_atom_content_new
 * @type: constructor ta_atom_content
 * @param type (optional): Mime type of the content.
 */
ta_atom_content_t *ta_atom_content_new (const char *type);

/**
 * @name: ta_atom_content_free
 * @type: destructor ta_atom_content
 */
void ta_atom_content_free (ta_atom_content_t *content);

/**
 * @name: ta_atom_content_to_iks
 * @type: method ta_atom_content
 *
 * This method returns an iks representation of the content
 * instance.
 */
iks *ta_atom_content_to_iks (ta_atom_content_t *content);

/**
 * @name: ta_atom_content_to_string
 * @type: method ta_atom_content
 *
 * This method returns an xml representation of the content
 * instance.
 */
char *ta_atom_content_to_string (ta_atom_content_t *entry);

/**
 * @name: ta_atom_content_get_type
 * @type: getter ta_atom_content:type
 */
const char *ta_atom_content_get_type (ta_atom_content_t *content);

/**
 * @name: ta_atom_content_set_type
 * @type: setter ta_atom_content:type
 */
void ta_atom_content_set_type (ta_atom_content_t *content, const char *type);

/**
 * @name: ta_atom_content_get_src
 * @type: getter ta_atom_content:src
 */
ta_iri_t *ta_atom_content_get_src (ta_atom_content_t *content);

/**
 * @name: ta_atom_content_set_src
 * @type: setter ta_atom_content:src
 */
void ta_atom_content_set_src (ta_atom_content_t *content, ta_iri_t *src);

/**
 * @name: ta_atom_content_get_content
 * @type: getter ta_atom_content:content
 */
const char *ta_atom_content_get_content (ta_atom_content_t *content, int *len);

/**
 * @name: ta_atom_content_set_content
 * @type: setter ta_atom_content:content
 */
void ta_atom_content_set_content (ta_atom_content_t *content, const char *text,
                                  int len);

/* -- Atom Person -- */

/**
 * @name: ta_atom_person_new
 * @type: constructor ta_atom_person
 * @param name: The person name.
 * @param email (optional): Person's email address.
 * @param iri (optional): The iri of the person's website.
 */
ta_atom_person_t *ta_atom_person_new (const char *name, const char *email,
                                      ta_iri_t *iri);

/**
 * @name: ta_atom_person_free
 * @type: destructor ta_atom_person
 */
void ta_atom_person_free (ta_atom_person_t *person);

/**
 * @name: ta_atom_person_to_iks
 * @type: method ta_atom_person
 * @param element: The element name.
 *
 * This method returns an iks representation of the person
 * instance. And the `element' parameter will be used as its
 * root node name.
 */
iks *ta_atom_person_to_iks (ta_atom_person_t *person, const char *element);

/**
 * @name: ta_atom_person_to_string
 * @type: method ta_atom_person
 * @param element: The element name.
 *
 * This method returns an xml representation of the person
 * instance. The root node of this representation is named with the
 * `element' parameter.
 */
char *ta_atom_person_to_string (ta_atom_person_t *person, const char *element);

/**
 * @name: ta_atom_person_get_name
 * @type: getter ta_atom_person:name
 */
const char *ta_atom_person_get_name (ta_atom_person_t *person);

/**
 * @name: ta_atom_person_set_name
 * @type: setter ta_atom_person:name
 */
void ta_atom_person_set_name (ta_atom_person_t *person, const char *name);

/**
 * @name: ta_atom_person_get_email
 * @type: getter ta_atom_person:email
 */
const char *ta_atom_person_get_email (ta_atom_person_t *person);

/**
 * @name: ta_atom_person_set_email
 * @type: setter ta_atom_person:email
 */
void ta_atom_person_set_email (ta_atom_person_t *person, const char *email);

/**
 * @name: ta_atom_person_get_iri
 * @type: getter ta_atom_person:iri
 */
ta_iri_t *ta_atom_person_get_iri (ta_atom_person_t *person);

/**
 * @name: ta_atom_person_set_iri
 * @type: setter ta_atom_person:iri
 */
void ta_atom_person_set_iri (ta_atom_person_t *person, ta_iri_t *iri);

/**
 * @name: ta_atom_person_add_see
 * @type: method ta_atom_person
 * @param element: The element that will be added to the person.
 *
 * Adds an atom simple ext element to the person instance.
 */
void ta_atom_person_add_see (ta_atom_person_t *person,
                             ta_atom_simple_element_t *element);

/**
 * @name: ta_atom_person_del_see
 * @type: method ta_atom_person
 *
 * Removes all atom simple ext elements from the person instance.
 */
void ta_atom_person_del_see (ta_atom_person_t *person);

/**
 * @name: ta_atom_person_get_see
 * @type: method ta_atom_person
 * @param person: A person instance.
 * @return: ta_list (ta_atom_simple_element)
 *
 * Gets all simple extension elements associated with a person
 * instance.
 **/
ta_list_t *ta_atom_person_get_see (ta_atom_person_t *person);

/* -- Atom Category -- */

/**
 * @name: ta_atom_category_new
 * @type: constructor ta_atom_category
 * @param term: The category term.
 * @param label (optional): A human readable string containing the
 * category label.
 * @param scheme (optional): An iri pointing to a reachable category resource.
 */
ta_atom_category_t *ta_atom_category_new (const char *term, const char *label,
                                          ta_iri_t *scheme);

/**
 * @name: ta_atom_category_free
 * @type: destructor ta_atom_category
 */
void ta_atom_category_free (ta_atom_category_t  *category);

/**
 * @name: ta_atom_category_to_iks
 * @type: method ta_atom_category
 *
 * This method returns an iks representation of the category
 * instance.
 */
iks *ta_atom_category_to_iks (ta_atom_category_t *category);

/**
 * @name: ta_atom_category_to_string
 * @type: method ta_atom_category
 *
 * This method returns an xml representation of the category
 * instance.
 */
char *ta_atom_category_to_string (ta_atom_category_t *category);

/**
 * @name: ta_atom_category_get_label
 * @type: getter ta_atom_category:label
 */
const char *ta_atom_category_get_label (ta_atom_category_t  *category);

/**
 * @name: ta_atom_category_set_label
 * @type: setter ta_atom_category:label
 */
void ta_atom_category_set_label (ta_atom_category_t *category,
                                 const char *label);

/**
 * @name: ta_atom_category_get_term
 * @type: getter ta_atom_category:term
 */
const char *ta_atom_category_get_term (ta_atom_category_t *category);

/**
 * @name: ta_atom_category_set_term
 * @type: setter ta_atom_category:term
 */
void ta_atom_category_set_term (ta_atom_category_t *category,
                                const char *term);

/**
 * @name: ta_atom_category_get_scheme
 * @type: getter ta_atom_category:scheme
 */
ta_iri_t *ta_atom_category_get_scheme (ta_atom_category_t *category);

/**
 * @name: ta_atom_category_set_scheme
 * @type: setter ta_atom_category:scheme
 */
void ta_atom_category_set_scheme (ta_atom_category_t *category,
                                  ta_iri_t *scheme);

/* -- Atom Thread In reply to -- */

/**
 * @name: ta_atom_in_reply_to_new
 * @type: constructor ta_atom_in_reply_to
 * @param ref: The IRI reference to the entry that is responsed to.
 */
ta_atom_in_reply_to_t *ta_atom_in_reply_to_new (ta_iri_t *ref);

/**
 * @name: ta_atom_in_reply_to_free
 * @type: destructor ta_atom_in_reply_to
 */
void ta_atom_in_reply_to_free (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to_to_iks
 * @type: method ta_atom_in_reply_to
 *
 * Returns an iks representation of the in reply to instance.
 */
iks *ta_atom_in_reply_to_to_iks (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to_to_string
 * @type: method ta_atom_in_reply_to
 *
 * Returns an xmp representation of the in reply to instance.
 */
char *ta_atom_in_reply_to_to_string (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to_get_ref
 * @type: getter ta_atom_in_reply_to:ref
 */
ta_iri_t *ta_atom_in_reply_to_get_ref (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to_set_ref
 * @type: setter ta_atom_in_reply_to:ref
 */
void ta_atom_in_reply_to_set_ref (ta_atom_in_reply_to_t *irt, ta_iri_t *ref);

/**
 * @name: ta_atom_in_reply_to_get_href
 * @type: getter ta_atom_in_reply_to:href
 */
ta_iri_t *ta_atom_in_reply_to_get_href (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to_set_href
 * @type: setter ta_atom_in_reply_to:href
 */
void ta_atom_in_reply_to_set_href (ta_atom_in_reply_to_t *irt, ta_iri_t *href);

/**
 * @name: ta_atom_in_reply_to_get_source
 * @type: getter ta_atom_in_reply_to:source
 */
ta_iri_t *ta_atom_in_reply_to_get_source (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to_set_source
 * @type: setter ta_atom_in_reply_to:source
 */
void ta_atom_in_reply_to_set_source (ta_atom_in_reply_to_t *irt,
                                     ta_iri_t *source);

/**
 * @name: ta_atom_in_reply_to_get_type
 * @type: getter ta_atom_in_reply_to:type
 */
const char *ta_atom_in_reply_to_get_type (ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_in_reply_to_set_type
 * @type: setter ta_atom_in_reply_to:type
 */
void ta_atom_in_reply_to_set_type (ta_atom_in_reply_to_t *irt,
                                   const char *type);

/* -- Atom Entry -- */

/**
 * @name: ta_atom_entry_new
 * @type: constructor ta_atom_entry
 * @param title (optional): The entry title
 */
ta_atom_entry_t *ta_atom_entry_new (const char *title);

/**
 * @name: ta_atom_entry_free
 * @type: destructor ta_atom_entry
 */
void ta_atom_entry_free (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_set_from_file
 * @type: method ta_atom_entry
 * @param fname: Name of the file to be loaded
 * @raise: TA_ATOM_LOAD_ERROR, TA_ATOM_PARSING_ERROR
 *
 * Load a file to an atom entry.
 */
int ta_atom_entry_set_from_file (ta_atom_entry_t *entry, const char *fname);

/**
 * @name: ta_atom_entry_set_from_iks
 * @type: method ta_atom_entry
 * @param iks: iks object to be parsed
 * @raise: TA_ATOM_PARSING_ERROR
 *
 * Load an iks xml structure to an atom entry.
 */
int ta_atom_entry_set_from_iks (ta_atom_entry_t *entry, iks *ik);

/**
 * @name: ta_atom_entry_to_iks
 * @type: method ta_atom_entry
 *
 * Returns an iks representation of an atom entry.
 */
iks *ta_atom_entry_to_iks (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_to_string
 * @type: method ta_atom_entry
 *
 * Returns a xml string representation of an atom entry.
 */
char *ta_atom_entry_to_string (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_to_file
 * @type: method ta_atom_entry
 * @param fname: Output file name.
 *
 * Saves the xml representation of the entry instance to a file.
 */
int ta_atom_entry_to_file (ta_atom_entry_t *entry, const char *fname);

/**
 * @name: ta_atom_entry_get_error
 * @type: getter ta_atom_entry:error
 */
ta_error_t *ta_atom_entry_get_error (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_get_title
 * @type: getter ta_atom_entry:title
 */
const char *ta_atom_entry_get_title (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_set_title
 * @type: setter ta_atom_entry:title
 */
void ta_atom_entry_set_title (ta_atom_entry_t *entry, const char *title);

/**
 * @name: ta_atom_entry_get_id
 * @type: getter ta_atom_entry:id
 */
ta_iri_t *ta_atom_entry_get_id (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_set_id
 * @type: setter ta_atom_entry:id
 */
void ta_atom_entry_set_id (ta_atom_entry_t *entry, ta_iri_t *id);

/**
 * @name: ta_atom_entry_get_updated
 * @type: getter ta_atom_entry:updated
 */
time_t ta_atom_entry_get_updated (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_set_updated
 * @type: setter ta_atom_entry:updated
 */
void ta_atom_entry_set_updated (ta_atom_entry_t *entry, time_t updated);

/**
 * @name: ta_atom_entry_get_published
 * @type: getter ta_atom_entry:published
 */
time_t ta_atom_entry_get_published (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_set_published
 * @type: setter (nullable) ta_atom_entry:published
 */
void ta_atom_entry_set_published (ta_atom_entry_t *entry, time_t published);

/**
 * @name: ta_atom_entry_get_rights
 * @type: getter ta_atom_entry:rights
 */
const char *ta_atom_entry_get_rights (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_set_rights
 * @type: setter ta_atom_entry:rights
 */
void ta_atom_entry_set_rights (ta_atom_entry_t *entry, const char *rights);

/**
 * @name: ta_atom_entry_get_summary
 * @type: getter ta_atom_entry:summary
 */
const char *ta_atom_entry_get_summary (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_set_summary
 * @type: setter ta_atom_entry:summary
 */
void ta_atom_entry_set_summary (ta_atom_entry_t *entry, const char *summary);

/**
 * @name: ta_atom_entry_get_content
 * @type: getter ta_atom_entry:content
 */
ta_atom_content_t *ta_atom_entry_get_content (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_set_content
 * @type: setter ta_atom_entry:content
 */
void ta_atom_entry_set_content (ta_atom_entry_t *entry,
                                ta_atom_content_t *content);

/**
 * @name: ta_atom_entry_get_authors
 * @type: method ta_atom_entry
 * @return: ta_list (ta_atom_person)
 *
 * Returns the list of authors of an atom entry instance.
 */
ta_list_t *ta_atom_entry_get_authors (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_add_author
 * @type: method ta_atom_entry
 * @param author: The author instance to be added.
 *
 * Adds an author instance to the entry. After that, you should not
 * free the author instance, it will be done by `ta_atom_entry_free'.
 */
void ta_atom_entry_add_author (ta_atom_entry_t *entry,
                               ta_atom_person_t *author);

/**
 * @name: ta_atom_entry_del_authors
 * @type: method ta_atom_entry
 *
 * Remove all authors from the entry. They will be freed too.
 */
void ta_atom_entry_del_authors (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_get_categories
 * @type: method ta_atom_entry
 * @return: ta_list (ta_atom_category)
 *
 * Returns the list of categories of an atom entry instance.
 */
ta_list_t *ta_atom_entry_get_categories (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_add_category
 * @type: method ta_atom_entry
 * @param category: The category instance to be added.
 *
 * Adds an category instance to the entry. After that, you should not
 * free the category instance, it will be done by `ta_atom_entry_free'.
 */
void ta_atom_entry_add_category (ta_atom_entry_t *entry,
                                 ta_atom_category_t *category);

/**
 * @name: ta_atom_entry_del_categories
 * @type: method ta_atom_entry
 *
 * Remove all categories from the entry. They will be freed too.
 */
void ta_atom_entry_del_categories (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_get_links
 * @type: method ta_atom_entry
 * @return: ta_list (ta_atom_link)
 *
 * Returns the list of links of an atom entry instance.
 */
ta_list_t *ta_atom_entry_get_links (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_add_link
 * @type: method ta_atom_entry
 * @param link: The link instance to be added.
 *
 * Adds an link instance to the entry. After that, you should not
 * free the link instance, it will be done by `ta_atom_entry_free'.
 */
void ta_atom_entry_add_link (ta_atom_entry_t *entry, ta_atom_link_t *link);

/**
 * @name: ta_atom_entry_del_links
 * @type: method ta_atom_entry
 *
 * Remove all links from the entry. They will be freed too.
 */
void ta_atom_entry_del_links (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_get_see
 * @type: method ta_atom_entry
 * @return: ta_list (ta_atom_simple_element)
 *
 * Returns the list of simple elements of an atom entry instance.
 */
ta_list_t *ta_atom_entry_get_see (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_add_see
 * @type: method ta_atom_entry
 * @param element: The element instance to be added.
 *
 * Adds a simple element instance to the entry. After that, you should
 * not free the element instance, it will be done by
 * `ta_atom_entry_free'.
 */
void ta_atom_entry_add_see (ta_atom_entry_t *entry,
                            ta_atom_simple_element_t *element);

/**
 * @name: ta_atom_entry_del_see
 * @type: method ta_atom_entry
 *
 * Remove all simple elements from the entry. They will be freed too.
 */
void ta_atom_entry_del_see (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_get_inreplyto
 * @type: method ta_atom_entry
 * @return: ta_list (ta_atom_in_reply_to)
 *
 * Returns the list of <code>in_reply_to</code> elements of an atom
 * entry instance.
 */
ta_list_t *ta_atom_entry_get_inreplyto (ta_atom_entry_t *entry);

/**
 * @name: ta_atom_entry_add_inreplyto
 * @type: method ta_atom_entry
 * @param element: The in reply to instance to be added.
 *
 * Adds an <code>in_reply_to</code> instance to the entry. After that,
 * you should not free the added instance, it will be done by
 * `ta_atom_entry_free'.
 */
void ta_atom_entry_add_inreplyto (ta_atom_entry_t *entry,
                                  ta_atom_in_reply_to_t *irt);

/**
 * @name: ta_atom_entry_del_inreplyto
 * @type: method ta_atom_entry
 *
 * Remove all <em>in_reply_to</em> elements from the entry. They will
 * be freed too.
 */
void ta_atom_entry_del_inreplyto (ta_atom_entry_t *entry);

/* -- Atom Feed -- */

/**
 * @name: ta_atom_feed_new
 * @type: constructor ta_atom_feed
 * @param title (optional): The feed title
 */
ta_atom_feed_t *ta_atom_feed_new (const char *title);

/**
 * @name: ta_atom_feed_free
 * @type: destructor ta_atom_feed
 */
void ta_atom_feed_free (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_to_iks
 * @type: method ta_atom_feed
 *
 * Returns an iks representation of an atom feed.
 */
iks *ta_atom_feed_to_iks (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_to_string
 * @type: method ta_atom_feed
 *
 * Returns a xml string representation of an atom feed.
 */
char *ta_atom_feed_to_string (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_to_file
 * @type: method ta_atom_feed
 * @param fname: Output file name.
 *
 * Saves the xml representation of the feed instance to a file.
 */
int ta_atom_feed_to_file (ta_atom_feed_t *feed, const char *fname);

/**
 * @name: ta_atom_feed_get_error
 * @type: getter ta_atom_feed:error
 */
ta_error_t *ta_atom_feed_get_error (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_get_title
 * @type: getter ta_atom_feed:title
 */
const char *ta_atom_feed_get_title (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_set_title
 * @type: setter ta_atom_feed:title
 */
void ta_atom_feed_set_title (ta_atom_feed_t *feed, const char *title);

/**
 * @name: ta_atom_feed_get_id
 * @type: getter ta_atom_feed:id
 */
ta_iri_t *ta_atom_feed_get_id (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_set_id
 * @type: setter ta_atom_feed:id
 */
void ta_atom_feed_set_id (ta_atom_feed_t *feed, ta_iri_t *id);

/**
 * @name: ta_atom_feed_get_updated
 * @type: getter ta_atom_feed:updated
 */
time_t ta_atom_feed_get_updated (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_set_updated
 * @type: setter ta_atom_feed:updated
 */
void ta_atom_feed_set_updated (ta_atom_feed_t *feed, time_t updated);

/**
 * @name: ta_atom_feed_get_authors
 * @type: method ta_atom_feed
 * @return: ta_list (ta_atom_person)
 *
 * Returns the list of authors of an atom feed instance.
 */
ta_list_t *ta_atom_feed_get_authors (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_add_author
 * @type: method ta_atom_feed
 * @param author: The author instance to be added.
 *
 * Adds an author instance to the feed. After that, you should not
 * free the author instance, it will be done by `ta_atom_feed_free'.
 */
void ta_atom_feed_add_author (ta_atom_feed_t *feed, ta_atom_person_t *author);

/**
 * @name: ta_atom_feed_del_authors
 * @type: method ta_atom_feed
 *
 * Remove all authors from the feed. They will be freed too.
 */
void ta_atom_feed_del_authors (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_get_categories
 * @type: method ta_atom_feed
 * @return: ta_list (ta_atom_category)
 *
 * Returns the list of categories of an atom feed instance.
 */
ta_list_t *ta_atom_feed_get_categories (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_add_category
 * @type: method ta_atom_feed
 * @param category: The category instance to be added.
 *
 * Adds an category instance to the feed. After that, you should not
 * free the category instance, it will be done by `ta_atom_feed_free'.
 */
void ta_atom_feed_add_category (ta_atom_feed_t *feed,
                                ta_atom_category_t *category);

/**
 * @name: ta_atom_feed_del_categories
 * @type: method ta_atom_feed
 *
 * Remove all categories from the feed. They will be freed too.
 */
void ta_atom_feed_del_categories (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_get_links
 * @type: method ta_atom_feed
 * @return: ta_list (ta_atom_link)
 *
 * Returns the list of links of an atom feed instance.
 */
ta_list_t *ta_atom_feed_get_links (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_add_link
 * @type: method ta_atom_feed
 * @param link: The link instance to be added.
 *
 * Adds an link instance to the feed. After that, you should not
 * free the link instance, it will be done by `ta_atom_feed_free'.
 */
void ta_atom_feed_add_link (ta_atom_feed_t *feed, ta_atom_link_t *link);

/**
 * @name: ta_atom_feed_del_links
 * @type: method ta_atom_feed
 *
 * Remove all links from the feed. They will be freed too.
 */
void ta_atom_feed_del_links (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_get_entries
 * @type: method ta_atom_feed
 * @return: ta_list (ta_atom_entry)
 *
 * Returns the list of entries of an atom feed instance.
 */
ta_list_t *ta_atom_feed_get_entries (ta_atom_feed_t *feed);

/**
 * @name: ta_atom_feed_add_entry
 * @type: method ta_atom_feed
 * @param entry: The entry instance to be added.
 *
 * Adds an entry instance to the feed. After that, you should not
 * free the entry instance, it will be done by `ta_atom_feed_free'.
 */
void ta_atom_feed_add_entry (ta_atom_feed_t *feed, ta_atom_entry_t *entry);

/**
 * @name: ta_atom_feed_del_entries
 * @type: method ta_atom_feed
 *
 * Remove all entries from the feed. They will be freed too.
 */
void ta_atom_feed_del_entries (ta_atom_feed_t *feed);

#endif /* _TANINGIA_ATOM_H_ */
