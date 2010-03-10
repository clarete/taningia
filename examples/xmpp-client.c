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

static int
connected_cb (ta_xmpp_client_t *client, void *data)
{
  fprintf (stderr, "We're connected!\n");
  return 0;
}

static int
auth_cb (ta_xmpp_client_t *client, void *data)
{
  iks *node;
  printf ("connected: =)\n");
  printf ("Sending presence and sleeping some time\n");

  /* Sending presence info */
  node = iks_make_pres (IKS_SHOW_AVAILABLE, "Client connected!");
  ta_xmpp_client_send (client, node);
  iks_delete (node);

  /* Sleeping some time before disconnection */
  sleep (3);
  ta_xmpp_client_disconnect (client);
  return 0;
}

static int
auth_failed_cb (ta_xmpp_client_t *client, void *data)
{
  ikspak *pak = (ikspak *) data;
  char *node_str = iks_string (iks_stack (pak->x), pak->x);
  printf ("not connected: =(\n");
  printf ("%s\n", node_str);
  ta_xmpp_client_disconnect (client);
  return 0;
}

int
main (int argc, char **argv)
{
  ta_xmpp_client_t *xmpp;
  ta_log_t *logger;
  const char *jid, *passwd, *host = NULL;

  /* Getting connection parameters from the command line */
  if (argc < 3)
    {
      fprintf (stderr, "Usage: %s: <jid> <passwd> [<host>]\n", argv[0]);
      return 1;
    }

  jid = argv[1];
  passwd = argv[2];

  if (argc == 4)
    host = argv[3];

  /* To initialize a client, you can pass the jid, password, host and
   * port number. Host can be NULL, in this case the domain part of
   * jid will be used. And if port is 0, it will default to 5222 */
  xmpp = ta_xmpp_client_new (jid, passwd, host, 5222);

  /* Connecting callbacks to some events dispatched by our client.  If
   * you try to connect to an unknown event, `event_connect' method
   * returns `0' and an error will be set. */
  ta_xmpp_client_event_connect (xmpp, "connected",
                                (ta_xmpp_client_hook_t) connected_cb,
                                NULL);

  ta_xmpp_client_event_connect (xmpp, "authenticated",
                                (ta_xmpp_client_hook_t) auth_cb,
                                NULL);

  ta_xmpp_client_event_connect (xmpp, "authentication-failed",
                                (ta_xmpp_client_hook_t) auth_failed_cb,
                                NULL);

  /* You can access and configure log object attached to the brand new
   * xmpp client instance. To know more about the log object, look at
   * the `log-system.c' example */
  logger = ta_xmpp_client_get_logger (xmpp);
  ta_log_set_level (logger, TA_LOG_DEBUG);
  ta_log_set_use_colors (logger, 1);

  /* Connecting to the host */
  if (!ta_xmpp_client_connect (xmpp))
    {
      ta_error_t *error;
      error = ta_xmpp_client_get_error (xmpp);
      fprintf (stderr, "%s: %s\n", ta_error_get_name (error),
               ta_error_get_message (error));
      ta_xmpp_client_free (xmpp);
      return 1;
    }

  /* Starting client main loop. Requesting to do not detach the loop
   * to another thread. Authentication will be done here too. */
  if (!ta_xmpp_client_run (xmpp, 0))
    {
      ta_error_t *error;
      error = ta_xmpp_client_get_error (xmpp);
      fprintf (stderr, "%s: %s\n", ta_error_get_name (error),
               ta_error_get_message (error));
      ta_xmpp_client_free (xmpp);
      return 1;
    }

  /* Cleaning up things */
  ta_xmpp_client_free (xmpp);
  return 0;
}
