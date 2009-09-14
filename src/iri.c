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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <taningia/iri.h>
#include <taningia/error.h>

struct _TIri
{
  char *scheme;
  char *user;
  char *host;
  int port;
  char *path;
  char *query;
  char *fragment;
  TError *error;
};

TIri *
t_iri_new (void)
{
  TIri *iri;
  iri = malloc (sizeof (TIri));
  iri->scheme = NULL;
  iri->user = NULL;
  iri->host = NULL;
  iri->port = 0;
  iri->path = NULL;
  iri->query = NULL;
  iri->fragment = NULL;
  iri->error = NULL;
  return iri;
}

void
t_iri_free (TIri *iri)
{
  if (iri->scheme)
    free (iri->scheme);
  if (iri->user)
    free (iri->user);
  if (iri->host)
    free (iri->host);
  if (iri->path)
    free (iri->path);
  if (iri->error)
    t_error_free (iri->error);
  free (iri);
}

TError *
t_iri_get_error (TIri *iri)
{
  return iri->error;
}

const char *
t_iri_get_scheme (TIri *iri)
{
  return iri->scheme;
}

void
t_iri_set_scheme (TIri *iri, const char *scheme)
{
  if (iri->scheme)
    free (iri->scheme);
  iri->scheme = strdup (scheme);
}

const char *
t_iri_get_user (TIri *iri)
{
  return iri->user;
}

void
t_iri_set_user (TIri *iri, const char *user)
{
  if (iri->user)
    free (iri->user);
  iri->user = strdup (user);
}

const char *
t_iri_get_host (TIri *iri)
{
  return iri->host;
}

void
t_iri_set_host (TIri *iri, const char *host)
{
  if (iri->host)
    free (iri->host);
  iri->host = strdup (host);
}

int
t_iri_get_port (TIri *iri)
{
  return iri->port;
}

void
t_iri_set_port (TIri *iri, int port)
{
  iri->port = port;
}

const char *
t_iri_get_path (TIri *iri)
{
  return iri->path;
}

void
t_iri_set_path (TIri *iri, const char *path)
{
  if (iri->path)
    free (iri->path);
  iri->path = strdup (path);
}

const char *
t_iri_get_query (TIri *iri)
{
  return iri->query;
}

void
t_iri_set_query (TIri *iri, const char *query)
{
  if (iri->query)
    free (iri->query);
  iri->query = strdup (query);
}

const char *
t_iri_get_fragment (TIri *iri)
{
  return iri->fragment;
}

void
t_iri_set_fragment (TIri *iri, const char *fragment)
{
  if (iri->fragment)
    free (iri->fragment);
  iri->fragment = strdup (fragment);
}

char *
t_iri_to_string (TIri *iri)
{
  size_t static_part;
  size_t scheme_size,
    host_size,
    user_size = 0,
    path_size = 0,
    query_size = 0,
    fragment_size = 0;
  size_t rlen;
  char *ret;
  int position = 0;

  /* Things really necessary to continue */
  if (!iri->scheme || !iri->host)
    return NULL;
  scheme_size = strlen (iri->scheme);
  host_size = strlen (iri->host),

  /* Calculating the size of our return var */
  static_part = 3 + 4;          /* ://, portnumber */
  if (iri->user)
    {
      user_size = strlen (iri->user);
      static_part++;            /* @ */
    }
  if (iri->port)
    static_part++;              /* : */
  if (iri->path)
    path_size = strlen (iri->path);
  if (iri->query)
    {
      query_size = strlen (iri->query);
      static_part++;            /* ? */
    }
  if (iri->fragment)
    {
      fragment_size = strlen (iri->fragment);
      static_part++;            /* # */
    }

  rlen = scheme_size + user_size +
    host_size +  path_size +
    query_size + fragment_size +
    static_part + 1;
  ret = malloc (rlen);

  /* Copying scheme to the ret var. */
  if (!iri->scheme)
    goto error;
  memcpy (ret, iri->scheme, scheme_size);
  position += scheme_size;
  memcpy ((ret+position), "://", 3);
  position += 3;

  /* User handling*/
  if (iri->user)
    {
      memcpy ((ret+position), iri->user, user_size);
      position += user_size;
      memcpy ((ret+position), "@", 1);
      position++;
    }

  /* Host */
  if (!iri->host)
    goto error;
  memcpy ((ret+position), iri->host, host_size);
  position += host_size;

  /* Port number */
  if (iri->port)
    {
      char port[4];
      size_t port_size;
      memcpy ((ret+position), ":", 1);
      position++;
      sprintf (port, "%d", iri->port);
      port_size = strlen (port);
      memcpy ((ret+position), port, port_size);
      position += port_size;
    }
  if (iri->path)
    {
      memcpy ((ret+position), iri->path, path_size);
      position += path_size;
    }
  if (iri->query)
    {
      memcpy ((ret+position), "?", 1);
      position++;
      memcpy ((ret+position), iri->query, query_size);
      position += query_size;
    }
  if (iri->fragment)
    {
      memcpy ((ret+position), "#", 1);
      position++;
      memcpy ((ret+position), iri->fragment, fragment_size);
      position += fragment_size;
    }

  ret[position] = '\0';
  return ret;

 error:
  free (ret);
  return NULL;
}

int
t_iri_set_from_string (TIri *iri, const char *string)

{
  const char *p, *ihier_part, *query, *fragment;
  char c;
  size_t size;

  /* This is what we need to parse here:
   *
   * IRI = scheme ":" ihier-part [ "?" iquery ] [ "#" ifragment ]
   *
   */

  p = string;

  if (!isalpha(p[0]))
    {
      if (iri->error)
        t_error_free (iri->error);
      iri->error = t_error_new ();
      t_error_set_full (iri->error, IRI_PARSING_ERROR, "ParsingError",
                        "Schema should start with an alpha char");
      return 0;
    }

  /* Getting "scheme" part */
  while (1)
    {
      c = *p;
      if (c == ':')
        break;
      /* As said in the IRI rfc, the scheme part only accept
       * a-Z0-9+.- chars*/
      if (!isalnum (c) &&
          !(c == '-' || c == '+' || c == '.'))
        {
          if (iri->error)
            t_error_free (iri->error);
          iri->error = t_error_new ();
          t_error_set_full (iri->error, IRI_PARSING_ERROR, "ParsingError",
                            "Schema should only have the following chars: [a-Z][0-9][-+.]");
          return 0;
        }
      c = *p++;
    }
  size = p - string;
  iri->scheme = malloc (size);
  memcpy (iri->scheme, string, size);
  iri->scheme[size] = '\0';

  /* Delimiter to the end of the ihier part */
  query = strchr (p, '?');
  if (query)
    {
      /* Removing the `?' char */
      query++;
      fragment = strchr (query, '#');
      if (fragment)
        {
          /* Removing number sign (#) */
          fragment++;
          iri->query = strndup (query, fragment-1 - query);
          iri->fragment = strdup (fragment);
        }
      else
        iri->query = strdup (query);
    }
  else
    {
      fragment = strchr (p, '#');
      if (fragment)
        iri->fragment = strdup (fragment+1);
    }

  /* Marks the end of the ihier_part */
  ihier_part = p+1;

  /* Parsing the authority section
   *
   * [ iuserinfo "@" ] ihost [ ":" port ]
   *
   */

  if (ihier_part[0] == '/' &&
      ihier_part[1] == '/')
    {
      const char *userinfo, *port, *path;
      const char *port_start;
      char *port_str;
      size_t port_len = 0;

      /* Removing slashes */
      ihier_part += 2;

      /* iuserinfo */
      userinfo = strrchr (ihier_part, '@');
      if (userinfo)
        {
          iri->user = strndup (ihier_part, userinfo - ihier_part);
          port_start = userinfo+1;
        }
      else
        port_start = ihier_part;

      /* Port */
      port = strchr (port_start, ':');
      if (port)
        {
          const char *pp;
          char c;

          pp = port+1;

          /* Only digits are going to be added to the port
           * attribute. This while breaks in the first non digit char
           * and it will be the first char of `iri->path' */
          while ((c = *pp++))
            {
              if (!isdigit (c))
                break;
              port_len++;
            }

          /* We're inside an if that make it sure that the user tried
           * to pass us a port to parse. If no digit is found, there
           * is something wrong, so we can abort parsing the IRI. */
          if (port_len == 0)
            {
              if (iri->error)
                t_error_free (iri->error);
              iri->error = t_error_new ();
              t_error_set_full (iri->error, IRI_PARSING_ERROR, "ParsingError",
                                "Invalid port number");
              return 0;
            }

          port_str = strndup (port+1, port_len);
          port_str[port_len] = '\0';
          iri->port = atoi (port_str);
          free (port_str);
        }

      /* Path */

      path = strchr (ihier_part, '/');

      /* If authority section exists, if it exists, path should start
       * with "/" as said in the following rule:
       *
       *   ipath-abempty = *( "/" isegment )
       *
       */
      if (path && path[0] != '/')
        {
          if (iri->error)
            t_error_free (iri->error);
          iri->error = t_error_new ();
          t_error_set_full (iri->error, IRI_PARSING_ERROR, "ParsingError",
                            "Path should start with a '/' since authority "
                            "section was filled");
          return 0;
        }
      if (path)
        {
          const char *pathp;
          char c;
          int path_len;

          pathp = path;
          path_len = 0;
          while ((c = *pathp++))
            {
              if (c == '?' || c == '#')
                break;
              path_len++;
            }
          iri->path = strndup (path, path_len);
        }

      /* Host */

      /* If a user is found, start host from it, otherwise, start from
       * the ihier_part. If a port is found, end host on it,
       * otherwise, end in the start of path */
      iri->host = strndup ((userinfo ? userinfo + 1 : ihier_part),
                           (port ? port : path) -
                           (userinfo ? userinfo : ihier_part - 1) -
                           1);
    }
  else
    {
      if (query)
        {
          int end = query - 1 - ihier_part;
          iri->path = strndup (ihier_part, end);
        }
      else if (!query && fragment)
        iri->path = strndup (ihier_part, fragment - ihier_part);
      else
        iri->path = strdup (ihier_part);
    }

  return 1;
}
