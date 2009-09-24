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
 *   >>> atom_person_t *person;
 *   >>> person = atom_person_new ("lincoln", NULL, NULL);
 *
 * The only required argument in the above constructor is the
 * `name'. Email and uri fields can be nullable.
 *
 * And now, to retrieve the value of these attributes you need to do:
 *
 *   >>> const char *name;
 *   >>> name = atom_person_get_name (person);
 *   >>> printf ("%s\n", name);
 *   lincoln
 *
 * To free the person object and all its attributes, use the
 * destructor. That is a function named with the class full name,
 * separated by underscores and with the `_free' sufix.
 *
 *   >>> atom_person_free (person);
 *
 * So, that's easy, all the rest of the API follows these
 * standards. All of the available objects has a constructor with the
 * `_new' sufix, getters and setters with the full class name, the
 * attribute name and the `_get' or the `_set' sufix.
 *
 * The only current exception is in the list getters, that uses out
 * parameters, like in `atom_person_get_see' that returns a list of
 * all simple extension elements. This is its prototype:
 *
 *   >>> void
 *   >>> atom_person_get_see (atom_person_t *person,
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
 *   >>> iri_t *href;
 *   >>> href = iri_new ("http://gnu.org");
 *   >>> link = t_link_new (href);
 *
 * Remembering that when passing href parameter to the link
 * constructor, you should not free href, since link is using the
 * reference you just created. The `href' var will be freed by the
 * `atom_link_free' destructor. All setters of this lib follows this
 * standard. Maybe in the future we should use reference count
 * strategy. =P
 *
 */

int
gen_feed (void)
{
  atom_feed_t *feed;
  atom_entry_t *entry;
  atom_content_t *content;
  atom_person_t *author;
  error_t *error;
  iri_t *feed_id, *entry_id;
  char *feed_string;

  /* This usually is the first step, create a feed instance: */
  feed = atom_feed_new ("My cool atom feed");

  /* Creating a random id and setting it to our feed object */
  feed_id = iri_new ();
  iri_set_from_string (feed_id, "urn:uuid:981db243-7aca-4467-8e38-9641429eba37");

  /* Since this iri is hardcoded we're sure that it will be parsed
   * successfuly, but in a real example, you should validate it, like
   * this: */
  if ((error = iri_get_error (feed_id)) != NULL)
    {
      /* This frees the already set attributes and the feed object
       * itself. */
      atom_feed_free (feed);

      /* Giving some feedback to the user. */
      fprintf (stderr, "Invalid uri: %s: %s\n", error_get_name (error),
               error_get_message (error));

      /* We had no time to set the iri in the feed object, so, we
       * should free it manually. But only do it when done with the
       * error object it will be freed here too. */
      iri_free (feed_id);
      return 1;
    }
  atom_feed_set_id (feed, feed_id);

  /* Adding an author to the feed object. This can be done multiple
   * times. */
  author = atom_person_new ("Lincoln", "lincoln@minaslivre.org", NULL);
  atom_feed_add_author (feed, author);

  /* Now is the time to create an entry and set a random id to it */
  entry = atom_entry_new ("Blah!");
  entry_id = iri_new ();
  iri_set_from_string (entry_id, "urn:uuid:0a5866b1-1c53-4323-9370-1f7c6a0c3f66");
  atom_entry_set_id (entry, entry_id);

  /* Setting the rights attribute of the entry */
  atom_entry_set_rights (entry, "GNU FDL... bleh bleh bleh...");

  /* Adding a content to our entry, without it, it will not work. */
  content = atom_content_new ("text/plain");
  atom_content_set_content (content, "This is the entry content!", 26);
  atom_entry_set_content (entry, content);

  /* Finally adding the entry to our feed object. */
  atom_feed_add_entry (feed, entry);

  /* Generating the xml string of our feed object. Here you can use
   * the `atom_feed_to_file' to save it to a file instead of
   * printing it. Or use the `atom_feed_to_iks' to generate an iks
   * instance and, maybe, send the entry in a pubsub message. */
  feed_string = atom_feed_to_string (feed);
  printf ("%s\n", feed_string);

  /* The only object that should be freed is the toplevel one. Don't
   * worry about freeing iri's entries or content. */
  atom_feed_free (feed);
  free (feed_string);
  return 0;
}

/* Simple extension of some atom objects.
 *
 * As said in the atom spec, it was designed to be extended easily
 * when needed. There are three objects that accepts simple extension
 * elements, they are: Person, Entry and Feed. And our library
 * provides an API to do it. There are two kinds of extension
 * elements: The simple and the structured elements. At this moment,
 * our library only provides the first way.
 *
 * A `SimpleExtensionElement' is just a key/value structure that is
 * added to an internal array of objects that suports this kind of
 * extension. Its representation in an xml file is very simple, like
 * this:
 *
 *   >>> TAtomSimpleExtElement *ext_element;
 *   >>> ext_element = atom_simple_ext_element_new ("key", "value");
 *   >>> printf ("%s\n", atom_simple_ext_element_to_string (ext_element));
 *   <key>value</key>
 *
 * To use it in an object that supports it, you should use the
 * `_add_see', `_del_see' and `_get_see' methods. Take a look at the
 * function `simple_extension_example'. This example uses an atom
 * person to demonstrate the extension, but you can do it with an atom
 * entry or an atom feed too.
 */

void
simple_extension_example (void)
{
  atom_person_t *person;
  atom_simple_element_t *state, *city;
  iri_t *uri;
  char *person_string;

  /* Here we're creating a new person instance with name, email and
   * uri fields filled. These are the only fields specified by the
   * atom rfc. */
  uri = iri_new ();
  iri_set_from_string (uri, "http://lincoln.comum.org");
  person = atom_person_new ("Lincoln", "lincoln@minaslivre.org", uri);

  /* Now, we're going to add simple extension objects to the person
   * instance. */
  state = atom_simple_element_new ("state", "MG");
  atom_person_add_see (person, state);
  city = atom_simple_element_new ("city", "Belo Horizonte");
  atom_person_add_see (person, city);

  /* When executing this example, you'll see all required fields
   * (name, email and uri), but both the state and city fields were
   * added too. */
  person_string = atom_person_to_string (person, "person");
  printf ("%s\n", person_string);
  free (person_string);

  /* like in other places, freeing an instance that holds references
   * for other objects is enough. You should not try to free state,
   * city or uri vars */
  atom_person_free (person);
}

int
main (int argc, char **argv)
{
  printf ("Generating an atom feed:\n");
  gen_feed ();
  printf ("\n");

  printf ("Extending an atom element:\n");
  simple_extension_example ();
  return 0;
}
