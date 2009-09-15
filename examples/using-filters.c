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

#include <stdio.h>
#include <unistd.h>             /* sleep() */
#include <iksemel.h>
#include <taningia/xmpp.h>
#include <taningia/log.h>
#include <taningia/filter.h>

/* Filters were created to give the user the possibility to grab the
 * answer of the sent stanza. Today, in our xmpp client
 * implementation, there are two default filters. One that handle
 * stanza ids and other to handle some misc events, like the receiving
 * of a message, of an error and stuff. We'll document these events
 * better someday.
 *
 * This example is very close to the xmpp-client.c because the usage
 * of filters makes more sense with a client, it was basically
 * copy-n-pasted from xmpp-client.c example, so if you have already
 * read that example, keep only the filter stuff in mind, the rest
 * will be the same.
 */

int
callback (void *p1, void *p2, void *p3)
{
  printf ("There was a call!!!\n");
  return 1;
}

int
main (int argc, char **argv)
{
  TXmppClient *xmpp;
  TLog *logger;
  TFilter *filter;
  const char *jid, *passwd, *host;

  /* Grabbing connection parameters from the command line */
  if (argc < 3)
    {
      fprintf (stderr, "Usage: %s: <jid> <passwd> <host>\n", argv[0]);
      return 1;
    }

  jid = argv[1];
  passwd = argv[2];
  host = argv[3];

  /* To initialize a client, you can pass the jid, password, host and
   * port number. Host can be NULL, in this case the domain part of
   * jid will be used. And if port is 0, it will default to 5222 */
  xmpp = t_xmpp_client_new (jid, passwd, host, 5222);

  /* You can access and configure log object attached to the brand new
   * xmpp client instance. To know more about the log object, look at
   * the `log-system.c' example */
  logger = t_xmpp_client_get_logger (xmpp);
  t_log_set_level (logger, t_log_get_level (logger) | TLOG_INFO | TLOG_DEBUG);
  t_log_set_use_colors (logger, 1);

  /* Connecting to the the server and detaching the main loop
   * thread. */
  if (t_xmpp_client_run (xmpp) != IKS_OK)
    {
      fprintf (stderr, "Unable to run client\n");
      return 1;
    }

  /* First step to work with the client filters.  You need to know
   * what you want to listen.  If you want to listen to id based
   * answers, use the "ids" filter, otherwise use the "events" filter.
   * In this example, we'll listen to a stanza type based answer, so
   * let's move on: */
  filter = t_xmpp_client_get_filter_events (xmpp);

  /* After getting the filter, add your handler with the name that you
   * want to listent to and a callback to process the answer. So, in
   * the next 10 seconds, every message received by this client will
   * call the `callback' function */
  t_filter_add (filter, "message", callback, NULL);

  sleep (10);

  /* Cleaning up things */
  t_xmpp_client_free (xmpp);
  return 0;
}
