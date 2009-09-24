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

#ifndef _TANINGIA_ATOM_H_
#define _TANINGIA_ATOM_H_

#include <time.h>
#include <iksemel.h>
#include <taningia/taningia.h>
#include <taningia/iri.h>
#include <taningia/error.h>
#include <taningia/list.h>

#define J_ATOM_NS "http://www.w3.org/2005/Atom"

typedef struct _atom_feed_t         atom_feed_t;
typedef struct _atom_entry_t        atom_entry_t;
typedef struct _atom_category_t     atom_category_t;
typedef struct _atom_person_t       atom_person_t;
typedef struct _atom_content_t      atom_content_t;
typedef struct _atom_link_t         atom_link_t;
typedef struct _atom_simple_element_t  atom_simple_element_t;

typedef enum
{
  ATOM_LOAD_ERROR,
  ATOM_PARSING_ERROR
} atom_error_t;

/* -- Atom Simple Ext Element -- */

/**
 * @name: atom_simple_element_new
 * @type: constructor atom_simple_element
 */
atom_simple_element_t *atom_simple_element_new (const char *name,
                                                const char *value);

/**
 * @name: atom_simple_element_free
 * @type: constructor atom_simple_element
 */
void atom_simple_element_free (atom_simple_element_t *see);

/**
 * @name: atom_simple_element_to_iks
 * @type: method atom_simple_element
 *
 * Returns an iks representation of a simple element.
 */
iks *atom_simple_element_to_iks (atom_simple_element_t *see);

/**
 * @name: atom_simple_element_to_iks
 * @type: method atom_simple_element
 *
 * Returns a string representation of a simple element.
 */
char *atom_simple_element_to_string (atom_simple_element_t *sse);

/**
 * @name: atom_simple_element_get_name
 * @type: getter atom_simple_element:name
 */
const char *atom_simple_element_get_name (atom_simple_element_t *sse);

/**
 * @name: atom_simple_element_set_name
 * @type: setter atom_simple_element:name
 */
void atom_simple_element_set_name (atom_simple_element_t *sse,
                                   const char *name);

/**
 * @name: atom_simple_element_get_value
 * @type: getter atom_simple_element:value
 */
const char *atom_simple_element_get_value (atom_simple_element_t *sse);

/**
 * @name: atom_simple_element_set_value
 * @type: setter atom_simple_element:value
 */
void atom_simple_element_set_value (atom_simple_element_t *sse,
                                    const char *value);

/* -- Atom Link -- */

/**
 * @name: atom_link_new
 * @type: constructor atom_link
 * @param href: Link target.
 */
atom_link_t *atom_link_new (iri_t *href);

/**
 * @name: atom_link_free
 * @type: destructor atom_link
 */
void atom_link_free (atom_link_t *link);

/**
 * @name: atom_link_to_iks
 * @type: method atom_link
 *
 * This method returns an iks representation of the link instance.
 */
iks *atom_link_to_iks (atom_link_t *link);

/**
 * @name: atom_link_to_string
 * @type: method atom_link
 *
 * This method returns an xml representation of the link instance.
 */
char *atom_link_to_string (atom_link_t *link);

/**
 * @name: atom_link_get_href
 * @type: getter atom_link:href
 */
iri_t *atom_link_get_href (atom_link_t *link);

/**
 * @name: atom_link_set_href
 * @type: setter atom_link:href
 */
void atom_link_set_href (atom_link_t *link, iri_t *href);

/**
 * @name: atom_link_get_title
 * @type: getter atom_link:title
 */
const char *atom_link_get_title (atom_link_t *link);

/**
 * @name: atom_link_set_title
 * @type: setter atom_link:title
 */
void atom_link_set_title (atom_link_t *link, const char *title);

/**
 * @name: atom_link_get_rel
 * @type: getter atom_link:rel
 */
const char *atom_link_get_rel (atom_link_t *link);

/**
 * @name: atom_link_set_rel
 * @type: setter atom_link:rel
 */
void atom_link_set_rel (atom_link_t *link, const char *rel);

/**
 * @name: atom_link_get_type
 * @type: getter atom_link:type
 */
const char *atom_link_get_type (atom_link_t *link);

/**
 * @name: atom_link_set_type
 * @type: setter atom_link:type
 */
void atom_link_set_type (atom_link_t *link, const char *type);

/**
 * @name: atom_link_get_length
 * @type: getter atom_link:length
 */
const char *atom_link_get_length (atom_link_t *link);

/**
 * @name: atom_link_set_length
 * @type: setter atom_link:length
 */
void atom_link_set_length (atom_link_t *link, const char *length);

/* -- Atom Content -- */

/**
 * @name: atom_content_new
 * @type: constructor atom_content
 * @param type (optional): Mime type of the content.
 */
atom_content_t *atom_content_new (const char *type);

/**
 * @name: atom_content_free
 * @type: destructor atom_content
 */
void atom_content_free (atom_content_t *content);

/**
 * @name: atom_content_to_iks
 * @type: method atom_content
 *
 * This method returns an iks representation of the content
 * instance.
 */
iks *atom_content_to_iks (atom_content_t *content);

/**
 * @name: atom_content_to_string
 * @type: method atom_content
 *
 * This method returns an xml representation of the content
 * instance.
 */
char *atom_content_to_string (atom_content_t *entry);

/**
 * @name: atom_content_get_type
 * @type: getter atom_content:type
 */
const char *atom_content_get_type (atom_content_t *content);

/**
 * @name: atom_content_set_type
 * @type: setter atom_content:type
 */
void atom_content_set_type (atom_content_t *content, const char *type);

/**
 * @name: atom_content_get_src
 * @type: getter atom_content:src
 */
iri_t *atom_content_get_src (atom_content_t *content);

/**
 * @name: atom_content_set_src
 * @type: setter atom_content:src
 */
void atom_content_set_src (atom_content_t *content, iri_t *src);

/**
 * @name: atom_content_get_content
 * @type: getter atom_content:content
 */
const char *atom_content_get_content (atom_content_t *content, int *len);

/**
 * @name: atom_content_set_content
 * @type: setter atom_content:content
 */
void atom_content_set_content (atom_content_t *content, const char *text, int len);

/* -- Atom Person -- */

/**
 * @name: atom_person_new
 * @type: constructor atom_person
 * @param name: The person name.
 * @param email (optional): Person's email address.
 * @param iri (optional): The iri of the person's website.
 */
atom_person_t *atom_person_new (const char *name, const char *email, iri_t *iri);

/**
 * @name: atom_person_free
 * @type: destructor atom_person
 */
void atom_person_free (atom_person_t *person);

/**
 * @name: atom_person_to_iks
 * @type: method atom_person
 * @param element: The element name.
 *
 * This method returns an iks representation of the person
 * instance. And the `element' parameter will be used as its
 * root node name.
 */
iks *atom_person_to_iks (atom_person_t *person, const char *element);

/**
 * @name: atom_person_to_string
 * @type: method atom_person
 * @param element: The element name.
 *
 * This method returns an xml representation of the person
 * instance. The root node of this representation is named with the
 * `element' parameter.
 */
char *atom_person_to_string (atom_person_t *person, const char *element);

/**
 * @name: atom_person_get_name
 * @type: getter atom_person:name
 */
const char *atom_person_get_name (atom_person_t *person);

/**
 * @name: atom_person_set_name
 * @type: setter atom_person:name
 */
void atom_person_set_name (atom_person_t *person, const char *name);

/**
 * @name: atom_person_get_email
 * @type: getter atom_person:email
 */
const char *atom_person_get_email (atom_person_t *person);

/**
 * @name: atom_person_set_email
 * @type: setter atom_person:email
 */
void atom_person_set_email (atom_person_t *person, const char *email);

/**
 * @name: atom_person_get_iri
 * @type: getter atom_person:iri
 */
iri_t *atom_person_get_iri (atom_person_t *person);

/**
 * @name: atom_person_set_iri
 * @type: setter atom_person:iri
 */
void atom_person_set_iri (atom_person_t *person, iri_t *iri);

/**
 * @name: atom_person_add_see
 * @type: method atom_person
 * @type: The element that will be added to the person.
 *
 * Adds an atom simple ext element to the person instance.
 */
void atom_person_add_see (atom_person_t *person, atom_simple_element_t *element);

/**
 * @name: atom_person_del_see
 * @type: method atom_person
 *
 * Removes all atom simple ext elements from the person instance.
 */
void atom_person_del_see (atom_person_t *person);

/**
 * @name: atom_person_get_see
 * @type: method atom_person
 * @param person: A person instance.
 * @return: list (atom_simple_element)
 *
 * Gets all simple extension elements associated with a person
 * instance.
 **/
list_t *t_atom_person_get_see (atom_person_t *person);

/* -- Atom Category -- */

/**
 * @name: atom_category_new
 * @type: constructor atom_category
 * @param term: The category term.
 * @param label (optional): A human readable string containing the category label.
 * @param scheme (optional): An iri pointing to a reachable category resource.
 */
atom_category_t *atom_category_new (const char *term, const char *label, iri_t *scheme);

/**
 * @name: atom_category_free
 * @type: destructor atom_category
 */
void atom_category_free (atom_category_t  *category);

/**
 * @name: atom_category_to_iks
 * @type: method atom_category
 *
 * This method returns an iks representation of the category
 * instance.
 */
iks *atom_category_to_iks (atom_category_t *category);

/**
 * @name: atom_category_to_string
 * @type: method atom_category
 *
 * This method returns an xml representation of the category
 * instance.
 */
char *atom_category_to_string (atom_category_t *category);

/**
 * @name: atom_category_get_label
 * @type: getter atom_category:label
 */
const char *atom_category_get_label (atom_category_t  *category);

/**
 * @name: atom_category_set_label
 * @type: setter atom_category:label
 */
void atom_category_set_label (atom_category_t *category, const char *label);

/**
 * @name: atom_category_get_term
 * @type: getter atom_category:term
 */
const char *atom_category_get_term (atom_category_t *category);

/**
 * @name: atom_category_set_term
 * @type: setter atom_category:term
 */
void atom_category_set_term (atom_category_t *category, const char *term);

/**
 * @name: atom_category_get_scheme
 * @type: getter atom_category:scheme
 */
iri_t *atom_category_get_scheme (atom_category_t *category);

/**
 * @name: atom_category_set_scheme
 * @type: setter atom_category:scheme
 */
void atom_category_set_scheme (atom_category_t *category, iri_t *scheme);

/* -- Atom Entry -- */

/**
 * @name: atom_entry_new
 * @type: constructor atom_entry
 * @param title (optional): The entry title
 */
atom_entry_t *atom_entry_new (const char *title);

/**
 * @name: atom_entry_free
 * @type: destructor atom_entry
 */
void atom_entry_free (atom_entry_t *entry);

/**
 * @name: atom_entry_set_from_file
 * @type: method atom_entry
 * @param fname: Name of the file to be loaded
 * @rise: ATOM_LOAD_ERROR, ATOM_PARSING_ERROR
 *
 * Load a file to an atom entry.
 */
int atom_entry_set_from_file (atom_entry_t *entry, const char *fname);

/**
 * @name: atom_entry_set_from_iks
 * @type: method atom_entry
 * @param iks: iks object to be parsed
 * @rise: ATOM_PARSING_ERROR
 *
 * Load an iks xml structure to an atom entry.
 */
int atom_entry_set_from_iks (atom_entry_t *entry, iks *ik);

/**
 * @name: atom_entry_to_iks
 * @type: method atom_entry
 *
 * Returns an iks representation of an atom entry.
 */
iks *atom_entry_to_iks (atom_entry_t *entry);

/**
 * @name: atom_entry_to_string
 * @type: method atom_entry
 *
 * Returns a xml string representation of an atom entry.
 */
char *atom_entry_to_string (atom_entry_t *entry);

/**
 * @name: atom_entry_to_file
 * @type: method atom_entry
 * @param fname: Output file name.
 *
 * Saves the xml representation of the entry instance to a file.
 */
int atom_entry_to_file (atom_entry_t *entry, const char *fname);

/**
 * @name: atom_entry_get_error
 * @type: getter atom_entry:error
 */
error_t *atom_entry_get_error (atom_entry_t *entry);

/**
 * @name: atom_entry_get_title
 * @type: getter atom_entry:title
 */
const char *atom_entry_get_title (atom_entry_t *entry);

/**
 * @name: atom_entry_set_title
 * @type: setter atom_entry:title
 */
void atom_entry_set_title (atom_entry_t *entry, const char *title);

/**
 * @name: atom_entry_get_id
 * @type: getter atom_entry:id
 */
iri_t *atom_entry_get_id (atom_entry_t *entry);

/**
 * @name: atom_entry_set_id
 * @type: setter atom_entry:id
 */
void atom_entry_set_id (atom_entry_t *entry, iri_t *id);

/**
 * @name: atom_entry_get_updated
 * @type: getter atom_entry:updated
 */
time_t atom_entry_get_updated (atom_entry_t *entry);

/**
 * @name: atom_entry_set_updated
 * @type: setter atom_entry:updated
 */
void atom_entry_set_updated (atom_entry_t *entry, time_t updated);

/**
 * @name: atom_entry_get_published
 * @type: getter atom_entry:published
 */
time_t atom_entry_get_published (atom_entry_t *entry);

/**
 * @name: atom_entry_set_published
 * @type: setter (nullable) atom_entry:published
 */
void atom_entry_set_published (atom_entry_t *entry, time_t published);

/**
 * @name: atom_entry_get_rights
 * @type: getter atom_entry:rights
 */
const char *atom_entry_get_rights (atom_entry_t *entry);

/**
 * @name: atom_entry_set_rights
 * @type: setter atom_entry:rights
 */
void atom_entry_set_rights (atom_entry_t *entry, const char *rights);

/**
 * @name: atom_entry_get_summary
 * @type: getter atom_entry:summary
 */
const char *atom_entry_get_summary (atom_entry_t *entry);

/**
 * @name: atom_entry_set_summary
 * @type: setter atom_entry:summary
 */
void atom_entry_set_summary (atom_entry_t *entry, const char *summary);

/**
 * @name: atom_entry_get_content
 * @type: getter atom_entry:content
 */
atom_content_t *atom_entry_get_content (atom_entry_t *entry);

/**
 * @name: atom_entry_set_content
 * @type: setter atom_entry:content
 */
void atom_entry_set_content (atom_entry_t *entry, atom_content_t *content);

/**
 * @name: atom_entry_get_authors
 * @type: method atom_entry
 * @return: list (atom_person)
 *
 * Returns the list of authors of an atom entry instance.
 */
list_t *atom_entry_get_authors (atom_entry_t *entry);

/**
 * @name: atom_entry_add_author
 * @type: method atom_entry
 * @param author: The author instance to be added.
 *
 * Adds an author instance to the entry. After that, you should not
 * free the author instance, it will be done by `atom_entry_free'.
 */
void atom_entry_add_author (atom_entry_t *entry, atom_person_t *author);

/**
 * @name: atom_entry_del_authors
 * @type: method atom_entry
 *
 * Remove all authors from the entry. They will be freed too.
 */
void atom_entry_del_authors (atom_entry_t *entry);

/**
 * @name: atom_entry_get_categories
 * @type: method atom_entry
 * @return: list (atom_category)
 *
 * Returns the list of categories of an atom entry instance.
 */
list_t *atom_entry_get_categories (atom_entry_t *entry);

/**
 * @name: atom_entry_add_category
 * @type: method atom_entry
 * @param category: The category instance to be added.
 *
 * Adds an category instance to the entry. After that, you should not
 * free the category instance, it will be done by `atom_entry_free'.
 */
void atom_entry_add_category (atom_entry_t *entry, atom_category_t *category);

/**
 * @name: atom_entry_del_categories
 * @type: method atom_entry
 *
 * Remove all categories from the entry. They will be freed too.
 */
void atom_entry_del_categories (atom_entry_t *entry);

/**
 * @name: atom_entry_get_links
 * @type: method atom_entry
 * @return: list (atom_link)
 *
 * Returns the list of links of an atom entry instance.
 */
list_t *atom_entry_get_links (atom_entry_t *entry);

/**
 * @name: atom_entry_add_link
 * @type: method atom_entry
 * @param link: The link instance to be added.
 *
 * Adds an link instance to the entry. After that, you should not
 * free the link instance, it will be done by `atom_entry_free'.
 */
void atom_entry_add_link (atom_entry_t *entry, atom_link_t *link);

/**
 * @name: atom_entry_del_links
 * @type: method atom_entry
 *
 * Remove all links from the entry. They will be freed too.
 */
void atom_entry_del_links (atom_entry_t *entry);

/**
 * @name: atom_entry_get_see
 * @type: method atom_entry
 * @return: list (atom_simple_element)
 *
 * Returns the list of simple elements of an atom entry instance.
 */
list_t *atom_entry_get_see (atom_entry_t *entry);

/**
 * @name: atom_entry_add_simple element
 * @type: method atom_entry
 * @param element: The element instance to be added.
 *
 * Adds a simple element instance to the entry. After that, you should
 * not free the element instance, it will be done by
 * `atom_entry_free'.
 */
void atom_entry_add_see (atom_entry_t *entry, atom_simple_element_t *element);

/**
 * @name: atom_entry_del_see
 * @type: method atom_entry
 *
 * Remove all simple elements from the entry. They will be freed too.
 */
void atom_entry_del_see (atom_entry_t *entry);

/* -- Atom Feed -- */

/**
 * @name: atom_feed_new
 * @type: constructor atom_feed
 * @param title (optional): The feed title
 */
atom_feed_t *atom_feed_new (const char *title _optional_);

/**
 * @name: atom_feed_free
 * @type: destructor atom_feed
 */
void atom_feed_free (atom_feed_t *feed);

/**
 * @name: atom_feed_to_iks
 * @type: method atom_feed
 *
 * Returns an iks representation of an atom feed.
 */
iks *atom_feed_to_iks (atom_feed_t *feed);

/**
 * @name: atom_feed_to_string
 * @type: method atom_feed
 *
 * Returns a xml string representation of an atom feed.
 */
char *atom_feed_to_string (atom_feed_t *feed);

/**
 * @name: atom_feed_to_file
 * @type: method atom_feed
 * @param fname: Output file name.
 *
 * Saves the xml representation of the feed instance to a file.
 */
int atom_feed_to_file (atom_feed_t *feed, const char *fname);

/**
 * @name: atom_feed_get_error
 * @type: getter atom_feed:error
 */
error_t *atom_feed_get_error (atom_feed_t *feed);

/**
 * @name: atom_feed_get_title
 * @type: getter atom_feed:title
 */
const char *atom_feed_get_title (atom_feed_t *feed);

/**
 * @name: atom_feed_set_title
 * @type: setter atom_feed:title
 */
void atom_feed_set_title (atom_feed_t *feed, const char *title);

/**
 * @name: atom_feed_get_id
 * @type: getter atom_feed:id
 */
iri_t *atom_feed_get_id (atom_feed_t *feed);

/**
 * @name: atom_feed_set_id
 * @type: setter atom_feed:id
 */
void atom_feed_set_id (atom_feed_t *feed, iri_t *id);

/**
 * @name: atom_feed_get_updated
 * @type: getter atom_feed:updated
 */
time_t atom_feed_get_updated (atom_feed_t *feed);

/**
 * @name: atom_feed_set_updated
 * @type: setter atom_feed:updated
 */
void atom_feed_set_updated (atom_feed_t *feed, time_t updated);

/**
 * @name: atom_feed_get_authors
 * @type: method atom_feed
 * @return: list (atom_person)
 *
 * Returns the list of authors of an atom feed instance.
 */
list_t *atom_feed_get_authors (atom_feed_t *feed);

/**
 * @name: atom_feed_add_author
 * @type: method atom_feed
 * @param author: The author instance to be added.
 *
 * Adds an author instance to the feed. After that, you should not
 * free the author instance, it will be done by `atom_feed_free'.
 */
void atom_feed_add_author (atom_feed_t *feed, atom_person_t *author);

/**
 * @name: atom_feed_del_authors
 * @type: method atom_feed
 *
 * Remove all authors from the feed. They will be freed too.
 */
void atom_feed_del_authors (atom_feed_t *feed);

/**
 * @name: atom_feed_get_categories
 * @type: method atom_feed
 * @return: list (atom_category)
 *
 * Returns the list of categories of an atom feed instance.
 */
list_t *atom_feed_get_categories (atom_feed_t *feed);

/**
 * @name: atom_feed_add_category
 * @type: method atom_feed
 * @param category: The category instance to be added.
 *
 * Adds an category instance to the feed. After that, you should not
 * free the category instance, it will be done by `atom_feed_free'.
 */
void atom_feed_add_category (atom_feed_t *feed, atom_category_t *category);

/**
 * @name: atom_feed_del_categories
 * @type: method atom_feed
 *
 * Remove all categories from the feed. They will be freed too.
 */
void atom_feed_del_categories (atom_feed_t *feed);

/**
 * @name: atom_feed_get_links
 * @type: method atom_feed
 * @return: list (atom_link)
 *
 * Returns the list of links of an atom feed instance.
 */
list_t *atom_feed_get_links (atom_feed_t *feed);

/**
 * @name: atom_feed_add_link
 * @type: method atom_feed
 * @param link: The link instance to be added.
 *
 * Adds an link instance to the feed. After that, you should not
 * free the link instance, it will be done by `atom_feed_free'.
 */
void atom_feed_add_link (atom_feed_t *feed, atom_link_t *link);

/**
 * @name: atom_feed_del_links
 * @type: method atom_feed
 *
 * Remove all links from the feed. They will be freed too.
 */
void atom_feed_del_links (atom_feed_t *feed);

/**
 * @name: atom_feed_get_entries
 * @type: method atom_feed
 * @return: list (atom_entry)
 *
 * Returns the list of entries of an atom feed instance.
 */
list_t *atom_feed_get_entries (atom_feed_t *feed);

/**
 * @name: atom_feed_add_entry
 * @type: method atom_feed
 * @param entry: The entry instance to be added.
 *
 * Adds an entry instance to the feed. After that, you should not
 * free the entry instance, it will be done by `atom_feed_free'.
 */
void atom_feed_add_entry (atom_feed_t *feed, atom_entry_t *entry);

/**
 * @name: atom_feed_del_entries
 * @type: method atom_feed
 *
 * Remove all entries from the feed. They will be freed too.
 */
void atom_feed_del_entries (atom_feed_t *feed);

#endif /* _TANINGIA_ATOM_H_ */
