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

#ifndef _JARVIS_ATOM_H_
#define _JARVIS_ATOM_H_

#include <time.h>
#include <iksemel.h>

#define J_ATOM_NS "http://www.w3.org/2005/Atom"

typedef struct _JAtomFeed     JAtomFeed;
typedef struct _JAtomEntry    JAtomEntry;
typedef struct _JAtomCategory JAtomCategory;
typedef struct _JAtomAuthor   JAtomAuthor;
typedef struct _JAtomContent  JAtomContent;

/* JAtomContent */

JAtomContent       *j_atom_content_new             (const char     *type,
                                                    const char     *content,
                                                    int             len);
void                j_atom_content_free            (JAtomContent   *content);
iks                *j_atom_content_to_iks          (JAtomContent   *content);
char               *j_atom_content_to_string       (JAtomContent   *entry);
const char         *j_atom_content_get_src         (JAtomContent   *content);
void                j_atom_content_set_src         (JAtomContent   *content,
                                                    const char     *src);

/* JAtomAuthor */

JAtomAuthor        *j_atom_author_new              (const char     *name,
                                                    const char     *email,
                                                    const char     *iri);
void                j_atom_author_free             (JAtomAuthor    *author);
iks                *j_atom_author_to_iks           (JAtomAuthor    *author);
char               *j_atom_author_to_string        (JAtomAuthor    *author);
const char         *j_atom_author_get_name         (JAtomAuthor    *author);
void                j_atom_author_set_name         (JAtomAuthor    *author,
                                                    const char     *name);
const char         *j_atom_author_get_email        (JAtomAuthor    *author);
void                j_atom_author_set_email        (JAtomAuthor    *author,
                                                    const char     *email);
const char         *j_atom_author_get_iri          (JAtomAuthor    *author);
void                j_atom_author_set_iri          (JAtomAuthor    *author,
                                                    const char     *iri);

/* JAtomCategory */

JAtomCategory      *j_atom_category_new            (const char     *term,
                                                    const char     *label,
                                                    const char     *scheme);
void                j_atom_category_free           (JAtomCategory  *category);
iks                *j_atom_category_to_iks         (JAtomCategory  *category);
char               *j_atom_category_to_string      (JAtomCategory  *category);
const char         *j_atom_category_get_label      (JAtomCategory  *category);
void                j_atom_category_set_label      (JAtomCategory  *category,
                                                    const char     *label);
const char         *j_atom_category_get_term       (JAtomCategory  *category);
void                j_atom_category_set_term       (JAtomCategory  *category,
                                                    const char     *term);
const char         *j_atom_category_get_scheme     (JAtomCategory  *category);
void                j_atom_category_set_scheme     (JAtomCategory  *category,
                                                    const char     *scheme);

/* JAtomEntry */

JAtomEntry         *j_atom_entry_new               (const char       *title);
void                j_atom_entry_free              (JAtomEntry       *entry);
iks                *j_atom_entry_to_iks            (JAtomEntry       *entry);
char               *j_atom_entry_to_string         (JAtomEntry       *entry);
const char         *j_atom_entry_get_title         (JAtomEntry       *entry);
void                j_atom_entry_set_title         (JAtomEntry       *entry,
                                                    const char       *title);
time_t              j_atom_entry_get_updated       (JAtomEntry       *entry);
void                j_atom_entry_set_updated       (JAtomEntry       *entry,
                                                    time_t            updated);
void                j_atom_entry_get_authors       (JAtomEntry       *entry,
                                                    JAtomAuthor    ***authors,
                                                    int              *len);
void                j_atom_entry_add_author        (JAtomEntry       *entry,
                                                    JAtomAuthor      *author);
void                j_atom_entry_del_authors       (JAtomEntry       *entry);
void                j_atom_entry_get_categories    (JAtomEntry       *entry,
                                                    JAtomCategory  ***categories,
                                                    int              *len);

void                j_atom_entry_add_category      (JAtomEntry       *entry,
                                                    JAtomCategory    *category);
void                j_atom_entry_del_categories    (JAtomEntry       *entry);
const char         *j_atom_entry_get_summary       (JAtomEntry       *entry);
void                j_atom_entry_set_summary       (JAtomEntry       *entry,
                                                    const char       *summary);
JAtomContent       *j_atom_entry_get_content       (JAtomEntry       *entry);
void                j_atom_entry_set_content       (JAtomEntry       *entry,
                                                    JAtomContent     *content);

#endif /* _JARVIS_ATOM_H_ */
