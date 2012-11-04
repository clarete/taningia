TESTS = check_taningia

check_PROGRAMS = check_taningia
check_taningia_SOURCES = check.c check_list.c check_iri.c

check_taningia_CFLAGS = @CHECK_CFLAGS@ -I$(top_srcdir)/include
check_taningia_LDADD = $(top_builddir)/src/libtaningia.la @CHECK_LIBS@
