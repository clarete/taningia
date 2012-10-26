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

#ifndef _TANINGIA_ERROR_H_
#define _TANINGIA_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  int code;
  char *message;
} ta_error_t;

/* General return types */
enum {
  OP_OK = 0,
  OP_ERROR = -1,
};


/* Error codes by their respective feature */
enum {
  TA_ATOM_LOAD_ERROR = 100,
  TA_ATOM_PARSING_ERROR = 101,

  TA_IRI_PARSING_ERROR = 200,
  TA_TAG_PARSING_ERROR = 201,

  XMPP_CONNECTION_ERROR = 300,
  XMPP_NO_SUCH_EVENT_ERROR = 301,
  XMPP_SEND_ERROR = 302,
};


const ta_error_t *ta_error_last (void);
void ta_error_clear (void);
void ta_error_set (int errcode, const char *message, ...);

#ifdef __cplusplus
}
#endif

#endif /* _TANINGIA_ERROR_H_ */
