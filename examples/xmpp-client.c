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

int
main (int argc, char **argv)
{
  TXmppClient *xmpp;
  TLog *logger;
  iks *node;
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

  /* Giving some time to the thread detached from t_xmpp_runn to
   * connect to the xmpp server and authenticate properly. */
  sleep (1);

  /* Sending some stuff to the server */
  node = iks_make_pres (IKS_SHOW_AVAILABLE, "Client connected!");
  t_xmpp_client_send (xmpp, node);
  sleep (3);

  /* Cleaning up things */
  iks_delete (node);
  t_xmpp_client_free (xmpp);
  return 0;
}
