# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

DUNE_AC_INIT # gets module version from dune.module file
AC_CONFIG_SRCDIR([dune/grid/common/grid.hh])
AC_CONFIG_HEADERS([config.h])

# --enable-extra-utils
DUNE_EXTRA_UTILS

# check all dune-module stuff
DUNE_CHECK_ALL

# set up flags for the automated test system
DUNE_AUTOBUILD_FLAGS

# preset variable to path such that #include <dune/...> works
AC_SUBST(AM_CPPFLAGS, '$(DUNE_CPPFLAGS) -I$(top_srcdir)')
AC_SUBST(AM_LDFLAGS, '$(DUNE_LDFLAGS)')
AC_SUBST([LDADD], '$(top_builddir)/dune/grid/libgrid.la $(DUNE_LIBS)')

# make DUNE_GRID_LIBDIR available as it is needed for the alberta libs
AC_SUBST(DUNE_GRID_LIBDIR, '$(top_builddir)/lib')

DUNE_SUMMARY_ALL

# write output
AC_CONFIG_FILES([Makefile
    lib/Makefile
    cmake/Makefile
    cmake/modules/Makefile
    doc/doxygen/Makefile
    doc/doxygen/Doxyfile
    doc/grids/Makefile
    doc/grids/amc/Makefile
    doc/grids/amiramesh/Makefile
    doc/grids/dgf/Makefile
    doc/grids/gridfactory/Makefile
    doc/grids/gmsh/Makefile
    doc/grids/starcd/Makefile
    doc/Makefile
    dune/Makefile
    dune/grid/Makefile
    dune/grid/test/Makefile
    dune/grid/common/test/Makefile
    dune/grid/common/Makefile
    dune/grid/uggrid/Makefile
    dune/grid/albertagrid/Makefile
    dune/grid/albertagrid/test/Makefile
    dune/grid/yaspgrid/Makefile
    dune/grid/onedgrid/Makefile
    dune/grid/utility/Makefile
    dune/grid/utility/test/Makefile
    dune/grid/identitygrid/Makefile
    dune/grid/io/file/test/Makefile
    dune/grid/io/file/amiramesh/Makefile
    dune/grid/io/file/dgfparser/Makefile
    dune/grid/io/file/dgfparser/blocks/Makefile
    dune/grid/io/file/dgfparser/test/Makefile
    dune/grid/io/file/dgfparser/utils/Makefile
    dune/grid/io/file/gnuplot/Makefile
    dune/grid/io/file/vtk/Makefile
    dune/grid/io/file/Makefile
    dune/grid/io/Makefile
    dune/grid/geometrygrid/Makefile
    m4/Makefile
    dune-grid.pc
    src/Makefile
    src/gridinfo-gmsh/Makefile
    ])
AC_CONFIG_FILES([dune/grid/io/file/test/mpivtktest],
    [chmod +x dune/grid/io/file/test/mpivtktest])
AC_OUTPUT
