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

#include <stdio.h>
#include <stdlib.h>
#include <taningia/atom.h>
#include <taningia/iri.h>

/* This example shows how our atom implementation works.
 *
 * == Basic usage of the API ==
 *
 * It is actually quite simple, the only thing we do is to provide an
 * `object' for each piece of an atom document. For example, an atom
 * person can be represented with a name, an email address and an uri,
 * so to abstract this idea, the AtomPerson class was created and can
 * be used like this:
 *
 *   >>> TAtomPerson *person;
 *   >>> person = t_atom_person_new ("lincoln", NULL, NULL);
 *
 * The only required argument in the above constructor is the
 * `name'. Email and uri fields can be nullable.
 *
 * And now, to retrieve the value of these attributes you need to do:
 *
 *   >>> const char *name;
 *   >>> name = t_atom_person_get_name (person);
 *   >>> printf ("%s\n", name);
 *   lincoln
 *
 * To free the person object and all its attributes, use the
 * destructor. That is a function named with the class full name,
 * separated by underscores and with the `_free' sufix.
 *
 *   >>> t_atom_person_free (person);
 *
 * So, that's easy, all the rest of the API follows these
 * standards. All of the available objects has a constructor with the
 * `_new' sufix, getters and setters with the full class name, the
 * attribute name and the `_get' or the `_set' sufix.
 *
 * The only current exception is in the list getters, that uses out
 * parameters, like in `t_atom_person_get_see' that returns a list of
 * all simple extension elements. This is its prototype:
 *
 *   >>> void
 *   >>> t_atom_person_get_see (TAtomPerson *person,
 *   ...                        TAtomSimpleExtElement ***elements,
 *   ...                        int *len);
 *
 * Soon we'll implement a List type in our library and we'll change it
 * to the above cited standard.
 *
 * == Uri/Iri consideratinos ==
 *
 * All fields that were designed to store uris, like Person.uri,
 * Link.href, Content.src and others, are actually using iri's. This
 * was done to follow recommendations in the RFC4287. So, to fill
 * these attributes You'll need to use our iri implementation, like
 * this:
 *
 *   >>> TAtomLink *link;
 *   >>> TIri *href;
 *   >>> href = t_iri_new ("http://gnu.org");
 *   >>> link = t_link_new (href);
 *
 * Remembering that when passing href parameter to the link
 * constructor, you should not free href, since link is using the
 * reference you just created. The `href' var will be freed by the
 * `t_atom_link_free' destructor. All setters of this lib follows this
 * standard. Maybe in the future we should use reference count
 * strategy. =P
 *
 */

int
main (int argc, char **argv)
{
  TAtomFeed *feed;
  TAtomEntry *entry;
  TAtomContent *content;
  TAtomPerson *author;
  TError *error;
  TIri *feed_id, *entry_id;
  char *feed_string;

  /* This usually is the first step, create a feed instance: */
  feed = t_atom_feed_new ("My cool atom feed");

  /* Creating a random id and setting it to our feed object */
  feed_id = t_iri_new ();
  t_iri_set_from_string (feed_id, "urn:uuid:981db243-7aca-4467-8e38-9641429eba37");

  /* Since this iri is hardcoded we're sure that it will be parsed
   * successfuly, but in a real example, you should validate it, like
   * this: */
  if ((error = t_iri_get_error (feed_id)) != NULL)
    {
      /* This frees the already set attributes and the feed object
       * itself. */
      t_atom_feed_free (feed);

      /* Giving some feedback to the user. */
      fprintf (stderr, "Invalid uri: %s: %s\n", t_error_get_name (error),
               t_error_get_message (error));

      /* We had no time to set the iri in the feed object, so, we
       * should free it manually. But only do it when done with the
       * error object it will be freed here too. */
      t_iri_free (feed_id);
      return 1;
    }
  t_atom_feed_set_id (feed, feed_id);

  /* Adding an author to the feed object. This can be done multiple
   * times. */
  author = t_atom_person_new ("Lincoln", "lincoln@minaslivre.org", NULL);
  t_atom_feed_add_author (feed, author);

  /* Now is the time to create an entry and set a random id to it */
  entry = t_atom_entry_new ("Blah!");
  entry_id = t_iri_new ();
  t_iri_set_from_string (entry_id, "urn:uuid:0a5866b1-1c53-4323-9370-1f7c6a0c3f66");
  t_atom_entry_set_id (entry, entry_id);

  /* Setting the rights attribute of the entry */
  t_atom_entry_set_rights (entry, "GNU FDL... bleh bleh bleh...");

  /* Adding a content to our entry, without it, it will not work. */
  content = t_atom_content_new ("text/plain", "This is the entry content!", 26);
  t_atom_entry_set_content (entry, content);

  /* Finally adding the entry to our feed object. */
  t_atom_feed_add_entry (feed, entry);

  /* Generating the xml string of our feed object. Here you can use
   * the `t_atom_feed_to_file' to save it to a file instead of
   * printing it. Or use the `t_atom_feed_to_iks' to generate an iks
   * instance and, maybe, send the entry in a pubsub message. */
  feed_string = t_atom_feed_to_string (feed);
  printf ("%s\n", feed_string);

  /* The only object that should be freed is the toplevel one. Don't
   * worry about freeing iri's entries or content. */
  t_atom_feed_free (feed);
  free (feed_string);
  return 0;
}
