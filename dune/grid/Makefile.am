SUBDIRS = \
  albertagrid \
  common \
  geometrygrid \
  identitygrid \
  io \
  onedgrid \
  test \
  uggrid \
  utility \
  yaspgrid
noinst_LTLIBRARIES = libgrid.la

# always part of lib:
ONEDLIB = onedgrid/libonedgrid.la
DGFPARSERLIB = io/file/dgfparser/libdgfparser.la

# conditional parts:
if UG
  UGLIB = uggrid/libuggrid.la
endif

# construct only from sub-libraries
libgrid_la_SOURCES =
# This forces automake to use the C++ linker
# (see the automake manual, section "Libtool Convenience Libraries")
nodist_EXTRA_libgrid_la_SOURCES = dummy.cc
sourcescheck_DUMMY = dummy.cc
libgrid_la_LIBADD = $(ONEDLIB) $(UGLIB) \
	$(DGFPARSERLIB)				\
        $(AMIRAMESHLIB)                         \
	$(DUNE_LIBS)

# These libraries do not need other libraries besides libdunecommon and libdunegeometry
griddir = $(includedir)/dune/grid
grid_HEADERS = albertagrid.hh \
               identitygrid.hh \
               uggrid.hh \
               yaspgrid.hh \
               onedgrid.hh \
	       geometrygrid.hh

include $(top_srcdir)/am/global-rules

EXTRA_DIST = CMakeLists.txt
