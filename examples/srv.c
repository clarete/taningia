/*
 * Copyright (C) 2010 Lincoln de Sousa <lincoln@minaslivre.org>
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
#include <stdlib.h>
#include <taningia/srv.h>

int
main (int argc, char **argv)
{
  ta_list_t *head, *n;
  ta_srv_target_t *t;

  /* Reading command line arguments */
  if (argc < 3)
    {
      fprintf (stderr, "Name and/or domain missing\n");
      exit (EXIT_FAILURE);
    }

  /* Initializing libresolv stuff. Like reading resolv.conf */
  ta_srv_init ();

  /* Quering something like _xmpp-client._tcp.jabber-br.org or
   * _http._tcp.comum.org is supported. The _http._tcp and
   * _xmpp-client._tcp must be passed as the first parameter and
   * jabber-br.org and comum.org as the second one. */
  head = ta_srv_query_domain (argv[1], argv[2]);
  for (n = head; n; n = n->next)
    {
      t = (ta_srv_target_t *) n->data;
      printf ("Target found:\n");
      printf (" Host: %s\n", ta_srv_target_get_host (t));
      printf (" Port: %d\n", ta_srv_target_get_port (t));

      /* The ta_srv_query_domain() function allocated all targets in
       * the return list. We must free it. */
      ta_object_unref (t);
    }

  /* All node->data elements were freed, now it is time to free the
   * list itself. */
  ta_list_free (head);
  return 0;
}
