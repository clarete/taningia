lib_LTLIBRARIES = libjarvis.la
libjarvis_la_SOURCES = xmpp.c pubsub.c filter.c atom.c
libjarvis_la_LDFLAGS = -version-info 0:1 -no-undefined
libjarvis_la_CFLAGS = $(IKSEMEL_CFLAGS) $(GLIB_CFLAGS) -I$(top_srcdir)/include
libjarvis_la_LIBADD = $(IKSEMEL_LIBS) $(GLIB_LIBS)
