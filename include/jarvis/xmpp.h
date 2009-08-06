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

#ifndef _RB_XMPP_H_
#define _RB_XMPP_H_

#include <glib.h>
#include <iksemel.h>

typedef struct _XmppContext {
  iksparser *parser;
  iksid *id;
  char *pubsub;
  char *jid;
  char *password;
} XmppContext;

XmppContext       *rb_xmpp_new                      (const char          *jid,
                                                     const char          *password,
                                                     const char          *pubsub);

void               rb_xmpp_free                     (XmppContext         *ctx);

void               rb_xmpp_run                      (XmppContext         *ctx);

#endif /* _RB_XMPP_H_ */
