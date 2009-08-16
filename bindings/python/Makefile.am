DEFSDIR = $(top_srcdir)/bindings/defs
DEFSFILE = $(DEFSDIR)/$(PACKAGE).json

pyexecdir = $(pythondir)
pyexec_LTLIBRARIES = jarvismodule.la

jarvismodule_la_SOURCES = \
	jarvismodule.c \
	jarvismodule.h

jarvismodule.c:
	PYTHONPATH=$(DEFSDIR) $(PYTHON) writer.py $(DEFSFILE)

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(IKSEMEL_CFLAGS) \
	$(PYTHON_CFLAGS) \
	$(PYIKSEMEL_CFLAGS) \
	-I$(top_srcdir)/include \
	-DPyJarvisBUILD_CORE

AM_LDFLAGS = \
	-module -avoid-version \
	$(GLIB_LIBS) \
	$(IKSEMEL_LIBS) \
	$(PYTHON_LIBS) \
	$(PYIKSEMEL_LIBS)

EXTRA_DIST = $(jarvismodule_la_SOURCES)

DISTCLEANFILES = $(jarvismodule_la_SOURCES)
