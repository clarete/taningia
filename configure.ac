#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([taningia], [0.1], [http://puzzlepipe.org])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE(foreign)

# Python stuff
AM_PATH_PYTHON(2.5.0)
PY_PREFIX=`$PYTHON -c 'import sys ; print sys.prefix'`
PYTHON_LIBS="-lpython$PYTHON_VERSION"
PYTHON_CFLAGS="-I$PY_PREFIX/include/python$PYTHON_VERSION"
AC_SUBST([PYTHON])
AC_SUBST([PYTHON_LIBS])
AC_SUBST([PYTHON_CFLAGS])

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checking for optional bindings

AC_ARG_WITH([python],
  [AS_HELP_STRING([--with-python], [Build python bindings. @<:@default=no@:>@])],
  [with_python_bindings=yes], [with_python_bindings=no])
AM_CONDITIONAL([PYTHON_BINDINGS], [test "x$with_python_bindings" != "xno"])

AS_IF([test "x$with_python_bindings" != "xno"],
  [PKG_CHECK_MODULES([PYIKSEMEL], [python-iksemel],
    [with_pyiksemel=yes],
    [AC_MSG_ERROR([coudn't find python-iksemel])])])
AC_SUBST([PYIKSEMEL_CFLAGS])
AC_SUBST([PYIKSEMEL_LIBS])

# Checks for .pc packages
PKG_CHECK_MODULES([IKSEMEL], [iksemel])
AC_SUBST([IKSEMEL_CFLAGS])
AC_SUBST([IKSEMEL_LIBS])

PKG_CHECK_MODULES([GLIB], [glib-2.0])
AC_SUBST([GLIB_CFLAGS])
AC_SUBST([GLIB_LIBS])

AC_CONFIG_FILES([
  Makefile
  include/Makefile
  include/taningia/Makefile
  src/Makefile
  bindings/Makefile
  bindings/defs/Makefile
  bindings/python/Makefile
  examples/Makefile
  taningia.pc
])

AC_OUTPUT
