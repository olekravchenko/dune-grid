ALLTESTS = vtktest gnuplottest vtksequencetest subsamplingvtktest gmshtest

GRIDDIM=2
GRIDTYPE=YASPGRID

# The directory where the example grid files used for testing are located
AM_CPPFLAGS+=-DDUNE_GRID_EXAMPLE_GRIDS_PATH=\"$(top_srcdir)/doc/grids/\"

noinst_HEADERS =				\
	checkvtkfile.hh

# run amiramesh test only if amiramesh was found
if AMIRAMESH
if PSURFACE
ALLTESTS += amirameshtest
endif
endif

if ALBERTA
ALLTESTS += gmshtest-alberta2d
ALLTESTS += gmshtest-alberta3d
endif

# Currently, Star-CD files can only be read into UGGrid objects
if UG
ALLTESTS += starcdreadertest
endif

# programs just to build when "make check" is used
check_PROGRAMS = $(ALLTESTS)

# list of tests to run
TESTS = $(ALLTESTS) mpivtktest

ALLTESTS += conformvolumevtktest
conformvolumevtktest_SOURCES = conformvolumevtktest.cc

ALLTESTS += nonconformboundaryvtktest
nonconformboundaryvtktest_SOURCES = nonconformboundaryvtktest.cc

vtktest_SOURCES = vtktest.cc
vtktest_CPPFLAGS = $(AM_CPPFLAGS)		\
	$(DUNEMPICPPFLAGS)
vtktest_LDFLAGS = $(AM_LDFLAGS)			\
	$(DUNEMPILDFLAGS)
vtktest_LDADD =					\
	$(DUNEMPILIBS)				\
	$(LDADD)

vtksequencetest_SOURCES = vtksequencetest.cc

gnuplottest_SOURCES = gnuplottest.cc

subsamplingvtktest_SOURCES = subsamplingvtktest.cc test-linking.cc
subsamplingvtktest_CPPFLAGS = $(AM_CPPFLAGS)	\
	$(DUNEMPICPPFLAGS)
subsamplingvtktest_LDFLAGS = $(AM_LDFLAGS)	\
	$(DUNEMPILDFLAGS)
subsamplingvtktest_LDADD =			\
	$(DUNEMPILIBS)				\
	$(LDADD)

amirameshtest_SOURCES = amirameshtest.cc
amirameshtest_CPPFLAGS = $(AM_CPPFLAGS)		\
	$(AMIRAMESH_CPPFLAGS)			\
	$(UG_CPPFLAGS)				\
	$(ALBERTA_CPPFLAGS)			\
	$(PSURFACE_CPPFLAGS)
amirameshtest_LDFLAGS = $(AM_LDFLAGS)		\
	$(AMIRAMESH_LDFLAGS)			\
	$(UG_LDFLAGS)				\
	$(ALBERTA_LDFLAGS)			\
	$(PSURFACE_LDFLAGS)
amirameshtest_LDADD =				\
	$(PSURFACE_LIBS)			\
	$(ALBERTA_LIBS)				\
	$(UG_LIBS)				\
	$(AMIRAMESH_LIBS)			\
	$(LDADD)

gmshtest_SOURCES  = gmshtest.cc
gmshtest_CPPFLAGS = $(AM_CPPFLAGS)  $(ALL_PKG_CPPFLAGS)
gmshtest_LDFLAGS  = $(AM_LDFLAGS)   $(ALL_PKG_LDFLAGS)
gmshtest_LDADD    = $(ALL_PKG_LIBS) $(LDADD)

gmshtest_alberta2d_SOURCES  = gmshtest.cc
gmshtest_alberta2d_CPPFLAGS = $(AM_CPPFLAGS) $(ALBERTA_CPPFLAGS) -DGRIDDIM=$(ALBERTA_DIM)
gmshtest_alberta2d_LDFLAGS  = $(AM_LDFLAGS)  $(ALBERTA_LDFLAGS)
gmshtest_alberta2d_LDADD    = $(ALBERTA_LIBS) $(LDADD)

gmshtest_alberta3d_SOURCES  = gmshtest.cc
gmshtest_alberta3d_CPPFLAGS = $(AM_CPPFLAGS) $(ALBERTA3D_CPPFLAGS)
gmshtest_alberta3d_LDFLAGS  = $(AM_LDFLAGS)  $(ALBERTA3D_LDFLAGS)
gmshtest_alberta3d_LDADD    = $(ALBERTA3D_LIBS) $(LDADD)

starcdreadertest_SOURCES = starcdreadertest.cc
starcdreadertest_CPPFLAGS = $(AM_CPPFLAGS)	\
	$(DUNEMPICPPFLAGS)			\
	$(UG_CPPFLAGS)
starcdreadertest_LDFLAGS = $(AM_LDFLAGS)	\
	$(DUNEMPILDFLAGS)			\
	$(UG_LDFLAGS)
starcdreadertest_LDADD =			\
	$(DUNEMPILIBS)				\
	$(UG_LIBS)				\
	$(LDADD)

include $(top_srcdir)/am/global-rules

CLEANFILES = *.vtu *.vtp *.data sgrid*.am *.pvtu *.pvtp *.pvd

EXTRA_DIST = CMakeLists.txt
