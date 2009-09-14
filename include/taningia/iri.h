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

typedef struct _TIri TIri;

typedef enum
{
  IRI_PARSING_ERROR
} TIriError;

TIri        *t_iri_new          (void);

void         t_iri_free         (TIri *iri);

const char  *t_iri_get_scheme   (TIri *iri);

void         t_iri_set_scheme   (TIri *iri, const char *scheme);

const char  *t_iri_get_user     (TIri *iri);

void         t_iri_set_user     (TIri *iri, const char *user);

const char  *t_iri_get_host     (TIri *iri);

void         t_iri_set_host     (TIri *iri, const char *host);

int          t_iri_get_port     (TIri *iri);

void         t_iri_set_port     (TIri *iri, int port);

const char  *t_iri_get_path     (TIri *iri);

void         t_iri_set_path     (TIri *iri, const char *path);

const char  *t_iri_get_query    (TIri *iri);

void         t_iri_set_query    (TIri *iri, const char *query);

const char  *t_iri_get_fragment (TIri *iri);

void         t_iri_set_fragment (TIri *iri, const char *fragment);

char        *t_iri_to_string    (TIri *iri);

TError      *t_iri_get_error    (TIri *iri);

int          t_iri_set_from_string (TIri *iri, const char *iristr);

#endif /* _TANINGIA_IRI_H_ */
