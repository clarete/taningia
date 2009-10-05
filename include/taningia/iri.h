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

#include <taningia/error.h>

#define TA_IRI(o) (o->parent)

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
  ta_iri_t *parent;
  char *authority;
  char *date;
  char *specific;
} ta_tag_t;

typedef enum {
  TA_IRI_PARSING_ERROR,
  TA_TAG_PARSING_ERROR
} ta_iri_error_t;

/**
 * @name: ta_iri_new
 * @type: constructor ta_iri
 */
ta_iri_t *ta_iri_new (void);

/**
 * @name: ta_iri_free
 * @type: destructor ta_iri
 */
void ta_iri_free (ta_iri_t *iri);

/**
 * @name: ta_iri_get_scheme
 * @type: getter ta_iri:scheme
 */
const char *ta_iri_get_scheme (ta_iri_t *iri);

/**
 * @name: ta_iri_set_scheme
 * @type: setter ta_iri:scheme
 */
void ta_iri_set_scheme (ta_iri_t *iri, const char *scheme);

/**
 * @name: ta_iri_get_user
 * @type: getter ta_iri:user
 */
const char *ta_iri_get_user (ta_iri_t *iri);

/**
 * @name: ta_iri_set_user
 * @type: setter ta_iri:user
 */
void ta_iri_set_user (ta_iri_t *iri, const char *user);

/**
 * @name: ta_iri_get_host
 * @type: getter ta_iri:host
 */
const char *ta_iri_get_host (ta_iri_t *iri);

/**
 * @name: ta_iri_set_host
 * @type: setter ta_iri:host
 */
void ta_iri_set_host (ta_iri_t *iri, const char *host);

/**
 * @name: ta_iri_get_port
 * @type: getter ta_iri:port
 */
int ta_iri_get_port (ta_iri_t *iri);

/**
 * @name: ta_iri_set_port
 * @type: setter ta_iri:port
 */
void ta_iri_set_port (ta_iri_t *iri, int port);

/**
 * @name: ta_iri_get_path
 * @type: getter ta_iri:path
 */
const char *ta_iri_get_path (ta_iri_t *iri);

/**
 * @name: ta_iri_set_path
 * @type: setter ta_iri:path
 */
void ta_iri_set_path (ta_iri_t *iri, const char *path);

/**
 * @name: ta_iri_get_query
 * @type: getter ta_iri:query
 */
const char *ta_iri_get_query (ta_iri_t *iri);

/**
 * @name: ta_iri_set_query
 * @type: setter ta_iri:query
 */
void ta_iri_set_query (ta_iri_t *iri, const char *query);

/**
 * @name: ta_iri_get_fragment
 * @type: getter ta_iri:fragment
 */
const char *ta_iri_get_fragment (ta_iri_t *iri);

/**
 * @name: ta_iri_set_fragment
 * @type: setter ta_iri:fragment
 */
void ta_iri_set_fragment (ta_iri_t *iri, const char *fragment);

/**
 * @name: ta_iri_to_string
 * @type: method ta_iri
 *
 * Returns a string representation of an iri instance.
 */
char *ta_iri_to_string (ta_iri_t *iri);

/**
 * @name: ta_iri_get_error
 * @type: getter ta_iri:error
 */
ta_error_t *ta_iri_get_error (ta_iri_t *iri);

/**
 * @name: ta_iri_set_from_string
 * @type: method ta_iri
 * @raises: TA_IRI_PARSING_ERROR
 *
 * Parses a string into an iri.
 */
int ta_iri_set_from_string (ta_iri_t *iri, const char *iristr);

/**
 * @name: ta_tag_new
 * @type: constructor ta_tag
 */
ta_tag_t *ta_tag_new (void);

/**
 * @name: ta_tag_free
 * @type: destructor ta_tag
 */
void ta_tag_free (ta_tag_t *tag);

/**
 * @name: ta_tag_get_authority
 * @type: getter ta_tag:authority
 */
const char *ta_tag_get_authority (ta_tag_t *tag);

/**
 * @name: ta_tag_set_authority
 * @type: setter ta_tag:authority
 */
void ta_tag_set_authority (ta_tag_t *tag, const char *authority);

/**
 * @name: ta_tag_get_date
 * @type: getter ta_tag:date
 */
const char *ta_tag_get_date (ta_tag_t *tag);

/**
 * @name: ta_tag_set_date
 * @type: setter ta_tag:date
 */
void ta_tag_set_date (ta_tag_t *tag, const char *date);

/**
 * @name: ta_tag_get_specific
 * @type: getter ta_tag:specific
 */
const char *ta_tag_get_specific (ta_tag_t *tag);

/**
 * @name: ta_tag_set_specific
 * @type: setter ta_tag:specific
 */
void ta_tag_set_specific (ta_tag_t *tag, const char *specific);

/**
 * @name: ta_tag_set_from_string
 * @type: method ta_tag
 * @raises: TA_IRI_PARSING_ERROR, TA_TAG_PARSING_ERROR
 *
 * Parses a string into a tag iri.
 */
int ta_tag_set_from_string (ta_tag_t *tag, const char *tagstr);

#endif /* _TANINGIA_IRI_H_ */
