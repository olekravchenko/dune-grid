// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_GRIDVIEW_HH
#define DUNE_GRIDVIEW_HH

#include <dune/common/geometrytype.hh>
#include <dune/grid/common/datahandleif.hh>
#include <dune/grid/common/gridenums.hh>

namespace Dune
{

  template< int, int, class, class >
  class GridDefaultImplementation;



  /** \addtogroup GIGridView
   *
   *  Though a DUNE grid is hierarchic, one often only needs access to
   *  a certain level of the grid. These views are provided by an
   *  implementation of GridView. Each grid exports a LevelGridView and
   *  a LeafGridView, corresponding to the two different types of levels.
   *
   *  A grid view provides the following functionality:
   *  - The index set for the grid level can be accessed by the indexSet()
   *    method.
   *  - A pair of begin() / end() methods provide iterators for each
   *    codimension.
   *  - A pair of ibegin() / iend() methods return suitable intersection
   *    iterators for a given entity of codimension 0.
   *  - For parallel computations, a suitable communicate() method is provided.
   *  - The underlying grid can be accessed through the grid() method.
   *  .
   *
   *  A GridView is obtained from the grid by calling one of the levelView() or
   *  leafView() methods.
   */


  /** \brief Grid view abstract base class
   *  \ingroup GIGridView
   *
   *  Interface class for view on grids. Grids return two types of view,
   *  a view of the leaf grid and of a level grid, which both satisfy
   *  the same interface. Through the view the user has access to the
   *  iterators, the intersections and the index set.
   *
   *  The interface is implemented using the engine concept.
   */
  template< class ViewTraits >
  class GridView
  {
    typedef GridView< ViewTraits > ThisType;

  public:
    typedef typename ViewTraits :: GridViewImp GridViewImp;

  protected:
    typedef GridViewImp ImplementationType;

  public:
    /** \brief Traits class */
    typedef ViewTraits Traits;

    /** \brief type of the grid */
    typedef typename Traits :: Grid Grid;

    /** \brief type of the index set */
    typedef typename Traits :: IndexSet IndexSet;

    /** \brief type of the intersection */
    typedef typename Traits :: Intersection Intersection;

    /** \brief type of the intersection iterator */
    typedef typename Traits :: IntersectionIterator IntersectionIterator;

    /** \brief type of the collective communication */
    typedef typename Traits :: CollectiveCommunication CollectiveCommunication;

    /** \brief Codim Structure */
    template< int cd >
    struct Codim : public Traits :: template Codim<cd> {};

    /** \brief Export if this grid view is conforming */
    enum { conforming = Traits :: conforming };

    /** \brief type used for coordinates in grid */
    typedef typename Grid::ctype ctype;

    /** \brief Dimension of the grid */
    enum { dimension = Grid :: dimension };

    /** \brief Dimension of the world */
    enum { dimensionworld = Grid :: dimensionworld };

  public:
    GridView ( const GridViewImp& imp)
      : imp_( imp )
    {}

    /** \brief Copy constructor */
    GridView ( const ThisType &other )
      : imp_( other.imp_ )
    {}

    /** \brief assignment operator */
    ThisType &operator= ( const ThisType &other )
    {
      imp_ = other.imp_;
      return *this;
    }

  public:
    /** \brief obtain a const reference to the underlying hierarchic grid */
    const Grid &grid () const
    {
      return asImp().grid();
    }

    /** \brief obtain the index set */
    const IndexSet &indexSet () const
    {
      return asImp().indexSet();
    }

    /** \brief obtain numer of entities in a given codimension */
    int size ( int codim ) const
    {
      return asImp().size( codim );
    }

    /** \brief obtain number of entities with a given geometry type */
    int size ( const GeometryType &type ) const
    {
      return asImp().size( type );
    }

    /** \brief obtain begin iterator for this view */
    template< int cd >
    typename Codim< cd > :: Iterator begin () const
    {
      return asImp().template begin<cd>();
    }

    /** \brief obtain end iterator for this view */
    template< int cd >
    typename Codim< cd > :: Iterator end () const
    {
      return asImp().template end<cd>();
    }

    /** \brief obtain begin iterator for this view */
    template< int cd , PartitionIteratorType pitype >
    typename Codim< cd > :: template Partition< pitype > :: Iterator
    begin () const
    {
      return asImp().template begin<cd,pitype>();
    }

    /** \brief obtain end iterator for this view */
    template< int cd, PartitionIteratorType pitype >
    typename Codim< cd > :: template Partition< pitype > :: Iterator
    end () const
    {
      return asImp().template end<cd,pitype>();
    }

    /** \brief obtain begin intersection iterator with respect to this view */
    IntersectionIterator
    ibegin ( const typename Codim< 0 > :: Entity &entity ) const
    {
      return asImp().ibegin(entity);
    }

    /** \brief obtain end intersection iterator with respect to this view */
    IntersectionIterator
    iend ( const typename Codim< 0 > :: Entity &entity ) const
    {
      return asImp().iend(entity);
    }

    /** \brief obtain collective communication object */
    const CollectiveCommunication &comm () const
    {
      return asImp().comm();
    }

    /** \brief Return size of the overlap region for a given codim on the grid view.  */
    int overlapSize(int codim) const
    {
      return asImp().overlapSize(codim);
    }

    /** \brief Return size of the ghost region for a given codim on the grid view.  */
    int ghostSize(int codim) const
    {
      return asImp().ghostSize(codim);
    }

    /** communicate data on this view */
    template< class DataHandleImp, class DataType >
    void communicate ( CommDataHandleIF< DataHandleImp, DataType > &data,
                       InterfaceType iftype,
                       CommunicationDirection dir ) const
    {
      asImp().communicate(data,iftype,dir);
    }

  protected:
    ImplementationType &getRealImp ()
    {
      return imp_;
    }

    const ImplementationType &getRealImp () const
    {
      return imp_;
    }

    GridViewImp& asImp ()
    {
      return imp_;
    }

    const GridViewImp& asImp () const
    {
      return imp_;
    }

  private:
    friend class GridDefaultImplementation
    < dimension, dimensionworld, ctype, typename Grid::GridFamily >;

    GridViewImp imp_;
  };

}

#endif
