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

#define J_ATOM_NS "http://www.w3.org/2005/Atom"

typedef struct _TAtomEntry    TAtomEntry;
typedef struct _TAtomCategory TAtomCategory;
typedef struct _TAtomAuthor   TAtomAuthor;
typedef struct _TAtomContent  TAtomContent;

/* TAtomContent */

TAtomContent       *t_atom_content_new             (const char     *type,
                                                    const char     *content _optional_,
                                                    int             len _len_);
void                t_atom_content_free            (TAtomContent   *content);
iks                *t_atom_content_to_iks          (TAtomContent   *content);
char               *t_atom_content_to_string       (TAtomContent   *entry);
TIri               *t_atom_content_get_src         (TAtomContent   *content);
void                t_atom_content_set_src         (TAtomContent   *content,
                                                    TIri           *src);
const char         *t_atom_content_get_content     (TAtomContent   *content,
                                                    int            *len _out_);
void                t_atom_content_set_content     (TAtomContent   *content,
                                                    const char     *text,
                                                    int             len _len_);

/* TAtomAuthor */

TAtomAuthor        *t_atom_author_new              (const char     *name,
                                                    const char     *email _optional_,
                                                    const char     *iri _optional_);
void                t_atom_author_free             (TAtomAuthor    *author);
iks                *t_atom_author_to_iks           (TAtomAuthor    *author);
char               *t_atom_author_to_string        (TAtomAuthor    *author);
const char         *t_atom_author_get_name         (TAtomAuthor    *author);
void                t_atom_author_set_name         (TAtomAuthor    *author,
                                                    const char     *name);
const char         *t_atom_author_get_email        (TAtomAuthor    *author);
void                t_atom_author_set_email        (TAtomAuthor    *author,
                                                    const char     *email);
const char         *t_atom_author_get_iri          (TAtomAuthor    *author);
void                t_atom_author_set_iri          (TAtomAuthor    *author,
                                                    const char     *iri);

/* TAtomCategory */

TAtomCategory      *t_atom_category_new            (const char     *term,
                                                    const char     *label _optional_,
                                                    const char     *scheme _optional_);
void                t_atom_category_free           (TAtomCategory  *category);
iks                *t_atom_category_to_iks         (TAtomCategory  *category);
char               *t_atom_category_to_string      (TAtomCategory  *category);
const char         *t_atom_category_get_label      (TAtomCategory  *category);
void                t_atom_category_set_label      (TAtomCategory  *category,
                                                    const char     *label);
const char         *t_atom_category_get_term       (TAtomCategory  *category);
void                t_atom_category_set_term       (TAtomCategory  *category,
                                                    const char     *term);
const char         *t_atom_category_get_scheme     (TAtomCategory  *category);
void                t_atom_category_set_scheme     (TAtomCategory  *category,
                                                    const char     *scheme);

/* TAtomEntry */

TAtomEntry         *t_atom_entry_new               (const char       *title);
void                t_atom_entry_free              (TAtomEntry       *entry);
iks                *t_atom_entry_to_iks            (TAtomEntry       *entry);
char               *t_atom_entry_to_string         (TAtomEntry       *entry);
const char         *t_atom_entry_get_title         (TAtomEntry       *entry);
void                t_atom_entry_set_title         (TAtomEntry       *entry,
                                                    const char       *title);
time_t              t_atom_entry_get_updated       (TAtomEntry       *entry);
void                t_atom_entry_set_updated       (TAtomEntry       *entry,
                                                    time_t            updated);
void                t_atom_entry_get_authors       (TAtomEntry       *entry,
                                                    TAtomAuthor    ***authors _out_,
                                                    int              *len _out_);
void                t_atom_entry_add_author        (TAtomEntry       *entry,
                                                    TAtomAuthor      *author);
void                t_atom_entry_del_authors       (TAtomEntry       *entry);
void                t_atom_entry_get_categories    (TAtomEntry       *entry,
                                                    TAtomCategory  ***categories _out_,
                                                    int              *len _out_);

void                t_atom_entry_add_category      (TAtomEntry       *entry,
                                                    TAtomCategory    *category);
void                t_atom_entry_del_categories    (TAtomEntry       *entry);
const char         *t_atom_entry_get_summary       (TAtomEntry       *entry);
void                t_atom_entry_set_summary       (TAtomEntry       *entry,
                                                    const char       *summary);
TAtomContent       *t_atom_entry_get_content       (TAtomEntry       *entry);
void                t_atom_entry_set_content       (TAtomEntry       *entry,
                                                    TAtomContent     *content);

#endif /* _TANINGIA_ATOM_H_ */
