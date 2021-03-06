if(UG_FOUND)
  dune_add_library("uggrid" OBJECT
    uggridintersections.cc
    uggrid.cc
    uggridentity.cc
    boundaryextractor.cc
    boundaryextractor.hh
    uggridindexsets.cc
    uggridfactory.cc
    uggridgeometry.cc
    uggridhieriterator.cc)
endif(UG_FOUND)

exclude_all_but_from_headercheck(
  boundaryextractor.hh
  ug_undefs.hh)

set(HEADERS
  uggridfactory.hh
  uggridentitypointer.hh
  uggridentityseed.hh
  uggridentity.hh
  uggridgeometry.hh
  uggridlocalgeometry.hh
  uggridhieriterator.hh
  uggridleveliterator.hh
  uggridviews.hh
  ugincludes.hh
  uggridintersections.hh
  uggridintersectioniterators.hh
  uggridindexsets.hh
  uggridleafiterator.hh
  uggridrenumberer.hh
  ug_undefs.hh
  uglbgatherscatter.hh
  ugmessagebuffer.hh
  ugwrapper.hh)

install(FILES ${HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/grid/uggrid)

# tricks like undefAllMacros.pl don't have to be shipped, have they?
# Oliver Sander: It depends. undefAllMacros.pl creates the file ug_undefs.hh
# which contains an undef line for each macro encountered in the UG
# headers which DUNE includes.  This should be redone every time UG
# changes, and currently I do it and then check in the new ug_undefs.hh.
# An alternative would be to have the DUNE-build system call undefAllMacros
# for the UG it is configured with.  Then it needed to be shipped.")
