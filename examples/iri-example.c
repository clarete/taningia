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

#include <stdio.h>              /* printf() */
#include <stdlib.h>             /* free() */
#include <string.h>             /* strcmp() */
#include <assert.h>             /* assert() */
#include <taningia/iri.h>

int
main (int argc, char **argv)
{
  TIri *iri;
  char *iri_string;
  iri = t_iri_new ();

  t_iri_set_scheme (iri, "http");
  t_iri_set_user (iri, "lincoln");
  t_iri_set_host (iri, "localhost");
  t_iri_set_port (iri, 80);
  t_iri_set_path (iri, "/hello/there");

  assert (!strcmp (t_iri_get_scheme (iri), "http"));
  assert (!strcmp (t_iri_get_user (iri), "lincoln"));
  assert (!strcmp (t_iri_get_host (iri), "localhost"));
  assert (t_iri_get_port (iri) == 80);
  assert (!strcmp (t_iri_get_path (iri), "/hello/there"));

  iri_string = t_iri_to_string (iri);
  assert (!strcmp (iri_string,
                   "http://lincoln@localhost:80/hello/there"));
  free (iri_string);
  t_iri_free (iri);

  if (argc == 2)
    {
      TIri *myiri;
      char *mystr;
      myiri = t_iri_new ();
      if (t_iri_set_from_string (myiri, argv[1]))
        {
          printf ("scheme:     %s\n", t_iri_get_scheme (iri));
          printf ("user:       %s\n", t_iri_get_user (iri));
          printf ("host:       %s\n", t_iri_get_host (iri));
          printf ("port:       %d\n", t_iri_get_port (iri));
          printf ("path:       %s\n", t_iri_get_path (iri));
          printf ("query:      %s\n", t_iri_get_query (iri));
          printf ("fragment:   %s\n", t_iri_get_fragment (iri));

          mystr = t_iri_to_string (myiri);
          if (mystr)
            {
              printf ("built IRI:  %s\n", mystr);
              free (mystr);
            }
          t_iri_free (myiri);
        }
      else
        fprintf (stderr, "Invalid iri\n");
    }
  return 0;
}
