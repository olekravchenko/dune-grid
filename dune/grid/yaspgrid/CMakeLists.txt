set(HEADERS
  backuprestore.hh
  coordinates.hh
  partitioning.hh
  structuredyaspgridfactory.hh
  torus.hh
  yaspgridentity.hh
  yaspgridentitypointer.hh
  yaspgridentityseed.hh
  yaspgridgeometry.hh
  yaspgridhierarchiciterator.hh
  yaspgridindexsets.hh
  yaspgridintersection.hh
  yaspgridintersectioniterator.hh
  yaspgrididset.hh
  yaspgridleveliterator.hh
  yaspgridpersistentcontainer.hh
  ygrid.hh)

exclude_all_but_from_headercheck(backuprestore.hh torus.hh coordinates.hh ygrid.hh)

install(FILES ${HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/grid/yaspgrid/)
