DEFSDIR = $(top_srcdir)/bindings/defs
DEFSFILE = $(DEFSDIR)/$(PACKAGE).json

all:
	PYTHONPATH=$(DEFSDIR) $(PYTHON) writer.py $(DEFSFILE)
