#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([jarvis], [0.1], [http://puzzlepipe.org])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE([-Wall])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.

# Check for .pc packages
PKG_CHECK_MODULES([IKSEMEL], [iksemel])
AC_SUBST([IKSEMEL_CFLAGS])
AC_SUBST([IKSEMEL_LIBS])

AC_CONFIG_FILES([
  Makefile
  include/Makefile
  include/jarvis/Makefile
  src/Makefile
])

AC_OUTPUT
