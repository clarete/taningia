DEFSDIR = $(top_srcdir)/bindings/defs
DEFSFILE = $(DEFSDIR)/$(PACKAGE).json

pyexecdir = $(pythondir)
pyexec_LTLIBRARIES = taningia.la

taningia_la_SOURCES = \
	taningiamodule.c \
	taningiamodule.h

taningiamodule.c:
	PYTHONPATH=$(DEFSDIR) $(PYTHON) writer.py $(DEFSFILE)

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(IKSEMEL_CFLAGS) \
	$(PYTHON_CFLAGS) \
	$(PYIKSEMEL_CFLAGS) \
	-I$(top_srcdir)/include \
	-DPyTaningiaBUILD_CORE

AM_LDFLAGS = \
	-module -avoid-version \
	$(GLIB_LIBS) \
	$(IKSEMEL_LIBS) \
	$(PYTHON_LIBS) \
	$(PYIKSEMEL_LIBS) \
	$(top_builddir)/src/libtaningia.la

EXTRA_DIST = $(taningia_la_SOURCES)

CLEANFILES = $(taningia_la_SOURCES)
