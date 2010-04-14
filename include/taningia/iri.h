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

#ifndef _TANINGIA_IRI_H_
#define _TANINGIA_IRI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <taningia/object.h>
#include <taningia/error.h>

#define TA_CAST_IRI(o) ((ta_iri_t *) (o))

/**
 * @name: ta_iri
 * @type: class
 *
 * Implementation of the Internationalized Resource Identifiers
 * standard defined by the RFC 3987. This implementation is not
 * complete yet, there are some really important things missing, like
 * transform an iri in an uri.
 */
typedef struct
{
  ta_object_t parent;
  char *scheme;
  char *user;
  char *host;
  int port;
  char *path;
  char *query;
  char *fragment;
  ta_error_t *error;
} ta_iri_t;

/**
 * @name: ta_tag
 * @type: class
 * @super: ta_iri
 * @since: 0.2
 *
 * Implementation of the Tag Unique Resource Identifier Scheme,
 * defined in RFC 4151.
 */
typedef struct
{
  ta_iri_t parent;
  char *authority;
  char *date;
  char *specific;
} ta_tag_t;

typedef enum {
  TA_IRI_PARSING_ERROR,
  TA_TAG_PARSING_ERROR
} ta_iri_error_t;

/**
 * @name: ta_iri::new
 * @type: constructor
 */
ta_iri_t *ta_iri_new (void);

/**
 * @name: ta_iri::init
 * @type: initializer
 */
void ta_iri_init (ta_iri_t *iri);

/**
 * @name: ta_iri::get_scheme
 * @type: getter
 */
const char *ta_iri_get_scheme (ta_iri_t *iri);

/**
 * @name: ta_iri::set_scheme
 * @type: setter
 */
void ta_iri_set_scheme (ta_iri_t *iri, const char *scheme);

/**
 * @name: ta_iri::get_user
 * @type: getter
 */
const char *ta_iri_get_user (ta_iri_t *iri);

/**
 * @name: ta_iri::set_user
 * @type: setter
 */
void ta_iri_set_user (ta_iri_t *iri, const char *user);

/**
 * @name: ta_iri::get_host
 * @type: getter
 */
const char *ta_iri_get_host (ta_iri_t *iri);

/**
 * @name: ta_iri::set_host
 * @type: setter
 */
void ta_iri_set_host (ta_iri_t *iri, const char *host);

/**
 * @name: ta_iri::get_port
 * @type: getter
 */
int ta_iri_get_port (ta_iri_t *iri);

/**
 * @name: ta_iri::set_port
 * @type: setter
 */
void ta_iri_set_port (ta_iri_t *iri, int port);

/**
 * @name: ta_iri::get_path
 * @type: getter
 */
const char *ta_iri_get_path (ta_iri_t *iri);

/**
 * @name: ta_iri::set_path
 * @type: setter
 */
void ta_iri_set_path (ta_iri_t *iri, const char *path);

/**
 * @name: ta_iri::get_query
 * @type: getter
 */
const char *ta_iri_get_query (ta_iri_t *iri);

/**
 * @name: ta_iri::set_query
 * @type: setter
 */
void ta_iri_set_query (ta_iri_t *iri, const char *query);

/**
 * @name: ta_iri::get_fragment
 * @type: getter
 */
const char *ta_iri_get_fragment (ta_iri_t *iri);

/**
 * @name: ta_iri::set_fragment
 * @type: setter
 */
void ta_iri_set_fragment (ta_iri_t *iri, const char *fragment);

/**
 * @name: ta_iri::to_string
 * @type: method
 *
 * Returns a string representation of an iri instance.
 */
char *ta_iri_to_string (ta_iri_t *iri);

/**
 * @name: ta_iri::get_error
 * @type: getter
 */
ta_error_t *ta_iri_get_error (ta_iri_t *iri);

/**
 * @name: ta_iri::set_from_string
 * @type: method
 * @raises: TA_IRI_PARSING_ERROR
 *
 * Parses a string into an iri.
 */
int ta_iri_set_from_string (ta_iri_t *iri, const char *iristr);

/**
 * @name: ta_tag::new
 * @type: constructor
 */
ta_tag_t *ta_tag_new (void);

/**
 * @name: ta_tag::init
 * @type: initializer
 */
void ta_tag_init (ta_tag_t *tag);

/**
 * @name: ta_tag::get_authority
 * @type: getter
 */
const char *ta_tag_get_authority (ta_tag_t *tag);

/**
 * @name: ta_tag::set_authority
 * @type: setter
 */
void ta_tag_set_authority (ta_tag_t *tag, const char *authority);

/**
 * @name: ta_tag::get_date
 * @type: getter
 */
const char *ta_tag_get_date (ta_tag_t *tag);

/**
 * @name: ta_tag::set_date
 * @type: setter
 */
void ta_tag_set_date (ta_tag_t *tag, const char *date);

/**
 * @name: ta_tag::get_specific
 * @type: getter
 */
const char *ta_tag_get_specific (ta_tag_t *tag);

/**
 * @name: ta_tag::set_specific
 * @type: setter
 */
void ta_tag_set_specific (ta_tag_t *tag, const char *specific);

/**
 * @name: ta_tag::set_from_string
 * @type: method
 * @raises: TA_IRI_PARSING_ERROR, TA_TAG_PARSING_ERROR
 *
 * Parses a string into a tag iri.
 */
int ta_tag_set_from_string (ta_tag_t *tag, const char *tagstr);

#ifdef __cplusplus
}
#endif

#endif /* _TANINGIA_IRI_H_ */
