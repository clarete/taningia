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

#include <taningia/taningia.h>
#include <taningia/error.h>

typedef struct _iri_t iri_t;

typedef enum
{
  IRI_PARSING_ERROR
} iri_error_t;

/**
 * @name: iri_new
 * @type: constructor iri
 */
iri_t *iri_new (void);

/**
 * @name: iri_free
 * @type: destructor iri
 */
void iri_free (iri_t *iri);

/**
 * @name: iri_get_scheme
 * @type: getter iri:scheme
 */
const char *iri_get_scheme (iri_t *iri);

/**
 * @name: iri_set_scheme
 * @type: setter iri:scheme
 */
void iri_set_scheme (iri_t *iri, const char *scheme);

/**
 * @name: iri_get_user
 * @type: getter iri:user
 */
const char *iri_get_user (iri_t *iri);

/**
 * @name: iri_set_user
 * @type: setter iri:user
 */
void iri_set_user (iri_t *iri, const char *user);

/**
 * @name: iri_get_host
 * @type: getter iri:host
 */
const char *iri_get_host (iri_t *iri);

/**
 * @name: iri_set_host
 * @type: setter iri:host
 */
void iri_set_host (iri_t *iri, const char *host);

/**
 * @name: iri_get_port
 * @type: getter iri:port
 */
int iri_get_port (iri_t *iri);

/**
 * @name: iri_set_port
 * @type: setter iri:port
 */
void iri_set_port (iri_t *iri, int port);

/**
 * @name: iri_get_path
 * @type: getter iri:path
 */
const char *iri_get_path (iri_t *iri);

/**
 * @name: iri_set_path
 * @type: setter iri:path
 */
void iri_set_path (iri_t *iri, const char *path);

/**
 * @name: iri_get_query
 * @type: getter iri:query
 */
const char *iri_get_query (iri_t *iri);

/**
 * @name: iri_set_query
 * @type: setter iri:query
 */
void iri_set_query (iri_t *iri, const char *query);

/**
 * @name: iri_get_fragment
 * @type: getter iri:fragment
 */
const char *iri_get_fragment (iri_t *iri);

/**
 * @name: iri_set_fragment
 * @type: setter iri:fragment
 */
void iri_set_fragment (iri_t *iri, const char *fragment);

/**
 * @name: iri_to_string
 * @type: method iri
 *
 * Returns a string representation of an iri instance.
 */
char *iri_to_string (iri_t *iri);

/**
 * @name: iri_get_error
 * @type: getter iri:error
 */
error_t *iri_get_error (iri_t *iri);

/**
 * @name: iri_set_from_string
 * @type: method iri
 * @raises: IRI_PARSING_ERROR
 *
 * Parses a string into an iri.
 */
int iri_set_from_string (iri_t *iri, const char *iristr);

#endif /* _TANINGIA_IRI_H_ */
