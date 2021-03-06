// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#include "config.h"

#include <algorithm>

#include <dune/grid/uggrid.hh>
#include <dune/grid/uggrid/uggridgeometry.hh>

///////////////////////////////////////////////////////////////////////
//
// General implementation of UGGridGeometry mydim-> coorddim
//
///////////////////////////////////////////////////////////////////////


template< int mydim, int coorddim, class GridImp>
Dune::GeometryType Dune::UGGridGeometry<mydim,coorddim,GridImp>::type() const
{
  switch (mydim)
  {
  case 0 : return GeometryType(0);
  case 1 : return GeometryType(1);
  case 2 :

    switch (UG_NS<coorddim>::Tag(target_)) {
    case UG::D2::TRIANGLE :
      return GeometryType(GeometryType::simplex,2);
    case UG::D2::QUADRILATERAL :
      return GeometryType(GeometryType::cube,2);
    default :
      DUNE_THROW(GridError, "UGGridGeometry::type():  ERROR:  Unknown type "
                 << UG_NS<coorddim>::Tag(target_) << " found!");
    }

  case 3 :
    switch (UG_NS<coorddim>::Tag(target_)) {

    case UG::D3::TETRAHEDRON :
      return GeometryType(GeometryType::simplex,3);
    case UG::D3::PYRAMID :
      return GeometryType(GeometryType::pyramid,3);
    case UG::D3::PRISM :
      return GeometryType(GeometryType::prism,3);
    case UG::D3::HEXAHEDRON :
      return GeometryType(GeometryType::cube,3);
    default :
      DUNE_THROW(GridError, "UGGridGeometry::type():  ERROR:  Unknown type "
                 << UG_NS<coorddim>::Tag(target_) << " found!");

    }
  }

}


template<int mydim, int coorddim, class GridImp>
Dune::FieldVector<typename GridImp::ctype, coorddim> Dune::UGGridGeometry<mydim,coorddim,GridImp>::
corner(int i) const
{
  // This geometry is a vertex
  if (mydim==0) {
    Dune::FieldVector<typename GridImp::ctype, coorddim> result;
    for (size_t j=0; j<coorddim; j++)
      // The cast is here to make the code compile even when target_ is not a node
      result[j] = ((typename UG_NS<coorddim>::Node*)target_)->myvertex->iv.x[j];
    return result;
  }

  // ////////////////////////////////
  //  This geometry is an element
  // ////////////////////////////////
  assert(mydim==coorddim);

  i = UGGridRenumberer<mydim>::verticesDUNEtoUG(i,type());

  Dune::FieldVector<typename GridImp::ctype, coorddim> result;
  for (size_t j=0; j<coorddim; j++)
    // The cast is here to make the code compile even when target_ is not an element
    result[j] = UG_NS<coorddim>::Corner(((typename UG_NS<coorddim>::Element*)target_),i)->myvertex->iv.x[j];
  return result;
}

template< int mydim, int coorddim, class GridImp>
Dune::FieldVector<typename GridImp::ctype, coorddim> Dune::UGGridGeometry<mydim,coorddim,GridImp>::
global(const FieldVector<UGCtype, mydim>& local) const
{
  FieldVector<UGCtype, coorddim> globalCoord(0.0);

  // we are an actual element in UG
  // coorddim*coorddim is an upper bound for the number of vertices
  UGCtype* cornerCoords[coorddim*coorddim];
  UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);

  // Actually do the computation
  UG_NS<coorddim>::Local_To_Global(corners(), cornerCoords, local, globalCoord);

  return globalCoord;
}


// Maps a global coordinate within the element to a
// local coordinate in its reference element
template< int mydim, int coorddim, class GridImp>
Dune::FieldVector<typename GridImp::ctype, mydim> Dune::UGGridGeometry<mydim,coorddim, GridImp>::
local (const Dune::FieldVector<typename GridImp::ctype, coorddim>& global) const
{
  FieldVector<UGCtype, mydim> result;

  // do nothing for a vertex
  if (mydim==0)
    return result;

  // coorddim*coorddim is an upper bound for the number of vertices
  UGCtype* cornerCoords[coorddim*coorddim];
  UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);

  // Actually do the computation
  /** \todo Why is this const_cast necessary? */
  UG_NS<coorddim>::GlobalToLocal(corners(), const_cast<const double**>(cornerCoords), &global[0], &result[0]);

  return result;
}


template< int mydim, int coorddim, class GridImp>
typename GridImp::ctype Dune::UGGridGeometry<mydim,coorddim,GridImp>::
integrationElement (const Dune::FieldVector<typename GridImp::ctype, mydim>& local) const
{
  if (mydim==0)
    return 1;
  else
    /** \todo No need to recompute the determinant every time on a simplex */
    return std::abs(1/jacobianInverseTransposed(local).determinant());
}


template< int mydim, int coorddim, class GridImp>
Dune::FieldMatrix<typename GridImp::ctype, coorddim,mydim> Dune::UGGridGeometry<mydim,coorddim, GridImp>::
jacobianInverseTransposed (const Dune::FieldVector<typename GridImp::ctype, mydim>& local) const
{
  FieldMatrix<UGCtype,coorddim,mydim> jIT;

  // compile array of pointers to corner coordinates
  // coorddim*coorddim is an upper bound for the number of vertices
  UGCtype* cornerCoords[coorddim*coorddim];
  UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);

  // compute the transformation onto the reference element (or vice versa?)
  UG_NS<coorddim>::Transformation(corners(), cornerCoords, local, jIT);

  return jIT;
}
template< int mydim, int coorddim, class GridImp>
Dune::FieldMatrix<typename GridImp::ctype, mydim,coorddim> Dune::UGGridGeometry<mydim,coorddim, GridImp>::
jacobianTransposed (const Dune::FieldVector<typename GridImp::ctype, mydim>& local) const
{
  FieldMatrix<UGCtype,mydim,coorddim> jac;

  // compile array of pointers to corner coordinates
  // coorddim*coorddim is an upper bound for the number of vertices
  UGCtype* cornerCoords[coorddim*coorddim];
  UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);

  // compute the transformation onto the reference element (or vice versa?)
  UG_NS<coorddim>::JacobianTransformation(corners(), cornerCoords, local, jac);

  return jac;
}


/////////////////////////////////////////////////////////////////////////////////
//   Explicit template instantiations
/////////////////////////////////////////////////////////////////////////////////

template class Dune::UGGridGeometry<0,2, const Dune::UGGrid<2> >;
template class Dune::UGGridGeometry<2,2, const Dune::UGGrid<2> >;

template class Dune::UGGridGeometry<0,3, const Dune::UGGrid<3> >;
template class Dune::UGGridGeometry<3,3, const Dune::UGGrid<3> >;
