INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = libjarvis.la
libjarvis_la_SOURCES = xmpp.c pubsub.c
libjarvis_la_LDFLAGS = -version-info 0:1
libjarvis_la_CFLAGS = $(IKSEMEL_CFLAGS)
libjarvis_la_LIBADD = $(IKSEMEL_LIBS)
