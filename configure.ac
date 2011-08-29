#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([taningia], [0.2.2], [http://projects.comum.org/taningia])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE(foreign)
LT_INIT

AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([resolv], [res_querydomain],
              enable_libresolv=yes,
              enable_libresolv=no)
AM_CONDITIONAL([ENABLE_LIBRESOLV], [test "$enable_libresolv" = "yes"])

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strchr strdup strndup strrchr])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_C_INLINE

# Checks for .pc packages
PKG_CHECK_MODULES([IKSEMEL], [iksemel])
AC_SUBST([IKSEMEL_CFLAGS])
AC_SUBST([IKSEMEL_LIBS])

AC_CONFIG_FILES([
  Makefile
  include/Makefile
  include/taningia/Makefile
  src/Makefile
  examples/Makefile
  taningia.pc
])

AC_OUTPUT
