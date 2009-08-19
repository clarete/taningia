lib_LTLIBRARIES = libtaningia.la
libtaningia_la_SOURCES = xmpp.c pubsub.c filter.c atom.c log.c
libtaningia_la_LDFLAGS = -version-info 0:1 -no-undefined
libtaningia_la_CFLAGS = $(IKSEMEL_CFLAGS) $(GLIB_CFLAGS) -I$(top_srcdir)/include
libtaningia_la_LIBADD = $(IKSEMEL_LIBS) $(GLIB_LIBS) -lpthread
