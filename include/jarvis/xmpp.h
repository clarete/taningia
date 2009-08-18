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

#ifndef _JARVIS_XMPP_H_
#define _JARVIS_XMPP_H_

#include <iksemel.h>
#include <jarvis/filter.h>
#include <jarvis/log.h>

typedef struct _JXmpp JXmpp;

JXmpp             *j_xmpp_new                      (const char          *jid,
                                                    const char          *password,
                                                    const char          *host,
                                                    int                  port);
 
void               j_xmpp_free                     (JXmpp               *ctx);
 
const char        *j_xmpp_get_jid                  (JXmpp               *ctx);

void               j_xmpp_set_jid                  (JXmpp               *ctx,
                                                    const char          *jid);

const char        *j_xmpp_get_password             (JXmpp               *ctx);

void               j_xmpp_set_password             (JXmpp               *ctx,
                                                    const char          *password);

const char        *j_xmpp_get_host                 (JXmpp               *ctx);

void               j_xmpp_set_host                 (JXmpp               *ctx,
                                                    const char          *host);

int                j_xmpp_get_port                 (JXmpp               *ctx);

void               j_xmpp_set_port                 (JXmpp               *ctx,
                                                    int                  port);

JFilter           *j_xmpp_get_filter_events        (JXmpp               *ctx);

JFilter           *j_xmpp_get_filter_ids           (JXmpp               *ctx);

JLog              *j_xmpp_get_logger               (JXmpp               *ctx);

int                j_xmpp_run                      (JXmpp               *ctx);

void               j_xmpp_stop                     (JXmpp               *ctx);

int                j_xmpp_reconnect                (JXmpp               *ctx);

int                j_xmpp_is_running               (JXmpp               *ctx);

#endif /* _JARVIS_XMPP_H_ */
