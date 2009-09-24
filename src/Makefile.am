lib_LTLIBRARIES = libtaningia.la
libtaningia_la_SOURCES = error.c xmpp.c pubsub.c iri.c atom.c log.c list.c
libtaningia_la_LDFLAGS = -version-info 0:1 -no-undefined
libtaningia_la_CFLAGS = $(IKSEMEL_CFLAGS) $(GLIB_CFLAGS) -I$(top_srcdir)/include
libtaningia_la_LIBADD = $(IKSEMEL_LIBS) $(GLIB_LIBS) -lpthread
