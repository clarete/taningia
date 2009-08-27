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

typedef struct _TAtomFeed     TAtomFeed;
typedef struct _TAtomEntry    TAtomEntry;
typedef struct _TAtomCategory TAtomCategory;
typedef struct _TAtomPerson   TAtomPerson;
typedef struct _TAtomContent  TAtomContent;

/* TAtomContent */

TAtomContent       *t_atom_content_new             (const char     *type _optional_,
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

/* TAtomPerson */

TAtomPerson        *t_atom_person_new              (const char     *name,
                                                    const char     *email _optional_,
                                                    TIri           *iri _optional_);
void                t_atom_person_free             (TAtomPerson    *person);
iks                *t_atom_person_to_iks           (TAtomPerson    *person,
                                                    const char     *element);
char               *t_atom_person_to_string        (TAtomPerson    *person,
                                                    const char     *element);
const char         *t_atom_person_get_name         (TAtomPerson    *person);
void                t_atom_person_set_name         (TAtomPerson    *person,
                                                    const char     *name);
const char         *t_atom_person_get_email        (TAtomPerson    *person);
void                t_atom_person_set_email        (TAtomPerson    *person,
                                                    const char     *email);
TIri               *t_atom_person_get_iri          (TAtomPerson    *person);
void                t_atom_person_set_iri          (TAtomPerson    *person,
                                                    TIri           *iri);

/* TAtomCategory */

TAtomCategory      *t_atom_category_new            (const char     *term,
                                                    const char     *label _optional_,
                                                    TIri           *scheme _optional_);
void                t_atom_category_free           (TAtomCategory  *category);
iks                *t_atom_category_to_iks         (TAtomCategory  *category);
char               *t_atom_category_to_string      (TAtomCategory  *category);
const char         *t_atom_category_get_label      (TAtomCategory  *category);
void                t_atom_category_set_label      (TAtomCategory  *category,
                                                    const char     *label);
const char         *t_atom_category_get_term       (TAtomCategory  *category);
void                t_atom_category_set_term       (TAtomCategory  *category,
                                                    const char     *term);
TIri               *t_atom_category_get_scheme     (TAtomCategory  *category);
void                t_atom_category_set_scheme     (TAtomCategory  *category,
                                                    TIri           *scheme);

/* TAtomEntry */

TAtomEntry         *t_atom_entry_new               (const char       *title _optional_);
int                 t_atom_entry_set_from_file     (TAtomEntry       *entry,
                                                    const char       *fname);
void                t_atom_entry_free              (TAtomEntry       *entry);
iks                *t_atom_entry_to_iks            (TAtomEntry       *entry);
char               *t_atom_entry_to_string         (TAtomEntry       *entry);
int                 t_atom_entry_to_file           (TAtomEntry       *entry,
                                                    const char       *fname);
const char         *t_atom_entry_get_title         (TAtomEntry       *entry);
void                t_atom_entry_set_title         (TAtomEntry       *entry,
                                                    const char       *title);
TIri               *t_atom_entry_get_id            (TAtomEntry       *entry);
void                t_atom_entry_set_id            (TAtomEntry       *entry,
                                                    TIri             *id);
time_t              t_atom_entry_get_updated       (TAtomEntry       *entry);
void                t_atom_entry_set_updated       (TAtomEntry       *entry,
                                                    time_t            updated);
time_t              t_atom_entry_get_published     (TAtomEntry       *entry);
void                t_atom_entry_set_published     (TAtomEntry       *entry,
                                                    time_t            published _nullable_);
const char         *t_atom_entry_get_rights        (TAtomEntry       *entry);
void                t_atom_entry_set_rights        (TAtomEntry       *entry,
                                                    const char       *rights);
void                t_atom_entry_get_authors       (TAtomEntry       *entry,
                                                    TAtomPerson    ***authors _out_,
                                                    int              *len _out_);
void                t_atom_entry_add_author        (TAtomEntry       *entry,
                                                    TAtomPerson      *author);
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

/* TAtomFeed */

TAtomFeed          *t_atom_feed_new                (const char       *title _optional_);
void                t_atom_feed_free               (TAtomFeed        *feed);

iks                *t_atom_feed_to_iks             (TAtomFeed        *feed);
char               *t_atom_feed_to_string          (TAtomFeed        *feed);
int                 t_atom_feed_to_file            (TAtomFeed        *feed,
                                                    const char       *fname);

const char         *t_atom_feed_get_title          (TAtomFeed        *feed);
void                t_atom_feed_set_title          (TAtomFeed        *feed,
                                                    const char       *title);

TIri               *t_atom_feed_get_id             (TAtomFeed        *feed);
void                t_atom_feed_set_id             (TAtomFeed        *feed,
                                                    TIri             *id);

time_t              t_atom_feed_get_updated        (TAtomFeed        *feed);
void                t_atom_feed_set_updated        (TAtomFeed        *feed,
                                                    time_t            updated);

void                t_atom_feed_get_authors        (TAtomFeed        *feed,
                                                    TAtomPerson    ***authors _out_,
                                                    int              *len _out_);
void                t_atom_feed_add_author         (TAtomFeed        *feed,
                                                    TAtomPerson       *author);
void                t_atom_feed_del_authors        (TAtomFeed        *feed);


void                t_atom_feed_get_categories     (TAtomFeed        *feed,
                                                    TAtomCategory  ***categories _out_,
                                                    int              *len _out_);
void                t_atom_feed_add_category       (TAtomFeed        *feed,
                                                    TAtomCategory    *category);
void                t_atom_feed_del_categories     (TAtomFeed        *feed);


void                t_atom_feed_get_entries        (TAtomFeed        *feed,
                                                    TAtomEntry     ***entries _out_,
                                                    int              *len _out_);
void                t_atom_feed_add_entry          (TAtomFeed        *feed,
                                                    TAtomEntry       *entry);
void                t_atom_feed_del_entries        (TAtomFeed        *feed);



#endif /* _TANINGIA_ATOM_H_ */
