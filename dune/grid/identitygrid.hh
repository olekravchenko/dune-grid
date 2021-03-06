// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_GRID_IDENTITYGRID_HH
#define DUNE_GRID_IDENTITYGRID_HH

/** \file
 * \brief The IdentityGrid class
 */

#include <string>
#include <map>

#include <dune/common/deprecated.hh>
#include <dune/common/parallel/collectivecommunication.hh>
#include <dune/grid/common/capabilities.hh>
#include <dune/grid/common/grid.hh>

// The components of the IdentityGrid interface
#include "identitygrid/identitygridgeometry.hh"
#include "identitygrid/identitygridentity.hh"
#include "identitygrid/identitygridentitypointer.hh"
#include "identitygrid/identitygridentityseed.hh"
#include "identitygrid/identitygridintersectioniterator.hh"
#include "identitygrid/identitygridleveliterator.hh"
#include "identitygrid/identitygridleafiterator.hh"
#include "identitygrid/identitygridhierarchiciterator.hh"
#include "identitygrid/identitygridindexsets.hh"

namespace Dune
{
  // Forward declaration
  template <class HostGrid>
  class IdentityGrid;

  // External forward declarations
  template< class Grid >
  struct HostGridAccess;

  namespace {

    // This is just a placeholder to stuff into the standard
    // GridTraits, as those expect an EntityPointer implementation
    // with two template parameters, but our EntityPointer uses three.
    template<int,typename>
    struct DummyIdentityGridEntityPointer
    {};

  }


  template<int dim, class HostGrid>
  struct IdentityGridFamily
  {

  private:

    // The type signature of our EntityPointer implementation does not fit into
    // the scheme expected by GridTraits, so we have to provide our own Traits
    // On the other hand, we don't want to retype everything, so we just inherit
    // from the default Traits and just tweak the EntityPointer type in the derived
    // Traits class

    typedef GridTraits<
        dim,
        HostGrid::dimensionworld,
        Dune::IdentityGrid<HostGrid>,
        IdentityGridGeometry,
        IdentityGridEntity,
        DummyIdentityGridEntityPointer, // placeholder
        IdentityGridLevelIterator,
        IdentityGridLeafIntersection,
        IdentityGridLevelIntersection,
        IdentityGridLeafIntersectionIterator,
        IdentityGridLevelIntersectionIterator,
        IdentityGridHierarchicIterator,
        IdentityGridLeafIterator,
        IdentityGridLevelIndexSet< const IdentityGrid<HostGrid> >,
        IdentityGridLeafIndexSet< const IdentityGrid<HostGrid> >,
        IdentityGridGlobalIdSet< const IdentityGrid<HostGrid> >,
        typename HostGrid::Traits::GlobalIdSet::IdType,
        IdentityGridLocalIdSet< const IdentityGrid<HostGrid> >,
        typename HostGrid::Traits::LocalIdSet::IdType,
        CollectiveCommunication<IdentityGrid<HostGrid> >,
        DefaultLevelGridViewTraits,
        DefaultLeafGridViewTraits,
        IdentityGridEntitySeed
        > BaseTraits;

  public:

    // inherit everyting from the default Traits...
    struct Traits
      : public BaseTraits
    {

      // Except for the Codim struct, which we reimplement, but which
      // again inherits everything from the base class...
      template<int codim>
      struct Codim
        : public BaseTraits::template Codim<codim>
      {

        // ... except for the EntityPointer, which we override with our
        // actual implementation
        typedef Dune::EntityPointer<
          const typename BaseTraits::Grid,
          IdentityGridEntityPointer<
            codim,
            const typename BaseTraits::Grid,
            typename HostGrid::template Codim<codim>::EntityPointer
            >
          > EntityPointer;

      };

    };

  };

  //**********************************************************************
  //
  // --IdentityGrid
  //
  //************************************************************************
  /*!
   * \brief Provides a meta grid that is identical to its host
   * \ingroup GridImplementations
   * \ingroup IdentityGrid
   *
   * \tparam HostGrid The host grid type wrapped by the IdentityGrid
   */
  template <class HostGrid>
  class IdentityGrid
  : public GridDefaultImplementation<HostGrid::dimension, HostGrid::dimensionworld,
                                     typename HostGrid::ctype, IdentityGridFamily<HostGrid::dimension, HostGrid> >
  {
    friend class IdentityGridLevelIndexSet<const IdentityGrid<HostGrid> >;
    friend class IdentityGridLeafIndexSet<const IdentityGrid<HostGrid> >;
    friend class IdentityGridGlobalIdSet<const IdentityGrid<HostGrid> >;
    friend class IdentityGridLocalIdSet<const IdentityGrid<HostGrid> >;
    friend class IdentityGridHierarchicIterator<const IdentityGrid<HostGrid> >;
    friend class IdentityGridLevelIntersectionIterator<const IdentityGrid<HostGrid> >;
    friend class IdentityGridLeafIntersectionIterator<const IdentityGrid<HostGrid> >;

    template<int codim, PartitionIteratorType pitype, class GridImp_>
    friend class IdentityGridLevelIterator;

    template<int codim, PartitionIteratorType pitype, class GridImp_>
    friend class IdentityGridLeafIterator;


    template<int codim_, int dim_, class GridImp_>
    friend class IdentityGridEntity;

    friend struct HostGridAccess< IdentityGrid< HostGrid > >;

  public:

    /** \todo Should not be public */
    typedef HostGrid HostGridType;

    //**********************************************************
    // The Interface Methods
    //**********************************************************

    //! type of the used GridFamily for this grid
    typedef IdentityGridFamily<HostGrid::dimension,HostGrid>  GridFamily;

    //! the Traits
    typedef typename IdentityGridFamily<HostGrid::dimension,HostGrid>::Traits Traits;

    //! The type used to store coordinates, inherited from the HostGrid
    typedef typename HostGrid::ctype ctype;


    /** \brief Constructor
     *
     * \param hostgrid The host grid wrapped by the IdentityGrid
     */
    explicit IdentityGrid(HostGrid& hostgrid) :
      hostgrid_(&hostgrid),
      leafIndexSet_(*this),
      globalIdSet_(*this),
      localIdSet_(*this)
    {
      setIndices();
    }

    //! Desctructor
    ~IdentityGrid()
    {
      // Delete level index sets
      for (size_t i=0; i<levelIndexSets_.size(); i++)
        if (levelIndexSets_[i])
          delete (levelIndexSets_[i]);
    }


    /** \brief Return maximum level defined in this grid.
     *
     * Levels are numbered 0 ... maxlevel with 0 the coarsest level.
     */
    int maxLevel() const {
      return hostgrid_->maxLevel();
    }

    //! Iterator to first entity of given codim on level
    template<int codim>
    typename Traits::template Codim<codim>::LevelIterator lbegin (int level) const {
      return IdentityGridLevelIterator<codim,All_Partition, const IdentityGrid<HostGrid> >(this, level);
    }


    //! one past the end on this level
    template<int codim>
    typename Traits::template Codim<codim>::LevelIterator lend (int level) const {
      return IdentityGridLevelIterator<codim,All_Partition, const IdentityGrid<HostGrid> >(this, level, true);
    }


    //! Iterator to first entity of given codim on level
    template<int codim, PartitionIteratorType PiType>
    typename Traits::template Codim<codim>::template Partition<PiType>::LevelIterator lbegin (int level) const {
      return IdentityGridLevelIterator<codim,PiType, const IdentityGrid<HostGrid> >(this, level);
    }


    //! one past the end on this level
    template<int codim, PartitionIteratorType PiType>
    typename Traits::template Codim<codim>::template Partition<PiType>::LevelIterator lend (int level) const {
      return IdentityGridLevelIterator<codim,PiType, const IdentityGrid<HostGrid> >(this, level, true);
    }


    //! Iterator to first leaf entity of given codim
    template<int codim>
    typename Traits::template Codim<codim>::LeafIterator leafbegin() const {
      return IdentityGridLeafIterator<codim,All_Partition, const IdentityGrid<HostGrid> >(this);
    }


    //! one past the end of the sequence of leaf entities
    template<int codim>
    typename Traits::template Codim<codim>::LeafIterator leafend() const {
      return IdentityGridLeafIterator<codim,All_Partition, const IdentityGrid<HostGrid> >(this, true);
    }


    //! Iterator to first leaf entity of given codim
    template<int codim, PartitionIteratorType PiType>
    typename Traits::template Codim<codim>::template Partition<PiType>::LeafIterator leafbegin() const {
      return IdentityGridLeafIterator<codim,PiType, const IdentityGrid<HostGrid> >(this);
    }


    //! one past the end of the sequence of leaf entities
    template<int codim, PartitionIteratorType PiType>
    typename Traits::template Codim<codim>::template Partition<PiType>::LeafIterator leafend() const {
      return IdentityGridLeafIterator<codim,PiType, const IdentityGrid<HostGrid> >(this, true);
    }


    /** \brief Number of grid entities per level and codim
     */
    int size (int level, int codim) const {
      return hostgrid_->size(level,codim);
    }

    /** \brief returns the number of boundary segments within the macro grid
     */
    size_t numBoundarySegments () const {
      return hostgrid_->numBoundarySegments();
    }

    //! number of leaf entities per codim in this process
    int size (int codim) const {
      return leafIndexSet().size(codim);
    }


    //! number of entities per level, codim and geometry type in this process
    int size (int level, GeometryType type) const {
      return levelIndexSets_[level]->size(type);
    }


    //! number of leaf entities per codim and geometry type in this process
    int size (GeometryType type) const
    {
      return leafIndexSet().size(type);
    }


    /** \brief Access to the GlobalIdSet */
    const typename Traits::GlobalIdSet& globalIdSet() const {
      return globalIdSet_;
    }


    /** \brief Access to the LocalIdSet */
    const typename Traits::LocalIdSet& localIdSet() const {
      return localIdSet_;
    }


    /** \brief Access to the LevelIndexSets */
    const typename Traits::LevelIndexSet& levelIndexSet(int level) const
    {
      if (level < 0 || level > maxLevel())
      {
        DUNE_THROW(GridError, "levelIndexSet of nonexisting level " << level << " requested!");
      }
      return *levelIndexSets_[level];
    }


    /** \brief Access to the LeafIndexSet */
    const typename Traits::LeafIndexSet& leafIndexSet() const
    {
      return leafIndexSet_;
    }


    /** \brief Create EntityPointer from EntitySeed
     *
     * \deprecated This method is deprecated and will be removed after the release of
     *             dune-grid 2.4. Please use entity() instead, which will directly return
     *             an Entity object that you can then store for later use. The EntityPointer
     *             concept in general is deprecated and will not be available after
     *             dune-grid 2.4 has been released.
     */
    template < class EntitySeed >
    DUNE_DEPRECATED_MSG("entityPointer() is deprecated and will be removed after the release of dune-grid 2.4. Use entity() instead to directly obtain an Entity object.")
    typename Traits::template Codim<EntitySeed::codimension>::EntityPointer
    entityPointer(const EntitySeed& seed) const
    {
      typedef typename Traits::template Codim<EntitySeed::codimension>::EntityPointer EntityPointer;
      typedef IdentityGridEntityPointer<
        EntitySeed::codimension,
        const typename Traits::Grid,
        typename HostGrid::template Codim<EntitySeed::codimension>::EntityPointer
        > EntityPointerImp;

      return EntityPointer(EntityPointerImp(this, hostgrid_->entity(this->getRealImplementation(seed).hostEntitySeed())));
    }

    /** \brief Create Entity from EntitySeed */
    template < class EntitySeed >
    typename Traits::template Codim<EntitySeed::codimension>::Entity
    entity(const EntitySeed& seed) const
    {
      typedef IdentityGridEntity<
        EntitySeed::codimension,
        HostGrid::dimension,
        const typename Traits::Grid
        > EntityImp;

      return EntityImp(this, hostgrid_->entity(this->getRealImplementation(seed).hostEntitySeed()));
    }


    /** @name Grid Refinement Methods */
    /*@{*/


    /** global refinement
     * \todo optimize implementation
     */
    void globalRefine (int refCount)
    {
      hostgrid_->globalRefine(refCount);
    }

    /** \brief Mark entity for refinement
     *
     * This only works for entities of codim 0.
     * The parameter is currently ignored
     *
     * \return <ul>
     * <li> true, if marking was succesfull </li>
     * <li> false, if marking was not possible </li>
     * </ul>
     */
    bool mark(int refCount, const typename Traits::template Codim<0>::Entity & e)
    {
      return hostgrid_->mark(refCount, getHostEntity<0>(e));
    }

    /** \brief Return refinement mark for entity
     *
     * \return refinement mark (1,0,-1)
     */
    int getMark(const typename Traits::template Codim<0>::Entity & e) const
    {
      return hostgrid_->getMark(getHostEntity<0>(e));
    }

    /** \brief returns true, if at least one entity is marked for adaption */
    bool preAdapt() {
      return hostgrid_->preAdapt();
    }


    //! Triggers the grid refinement process
    bool adapt()
    {
      return hostgrid_->adapt();
    }

    /** \brief Clean up refinement markers */
    void postAdapt() {
      return hostgrid_->postAdapt();
    }

    /*@}*/

    /** \brief Size of the overlap on the leaf level */
    unsigned int overlapSize(int codim) const {
      return hostgrid_->overlapSize(codim);
    }


    /** \brief Size of the ghost cell layer on the leaf level */
    unsigned int ghostSize(int codim) const {
      return hostgrid_->ghostSize(codim);
    }


    /** \brief Size of the overlap on a given level */
    unsigned int overlapSize(int level, int codim) const {
      return hostgrid_->overlapSize(level,codim);
    }


    /** \brief Size of the ghost cell layer on a given level */
    unsigned int ghostSize(int level, int codim) const {
      return hostgrid_->ghostSize(level,codim);
    }


#if 0
    /** \brief Distributes this grid over the available nodes in a distributed machine
     *
     * \param minlevel The coarsest grid level that gets distributed
     * \param maxlevel does currently get ignored
     */
    void loadBalance(int strategy, int minlevel, int depth, int maxlevel, int minelement){
      DUNE_THROW(NotImplemented, "IdentityGrid::loadBalance()");
    }

    /** \brief The communication interface
     *  @param T: array class holding data associated with the entities
     *  @param P: type used to gather/scatter data in and out of the message buffer
     *  @param codim: communicate entites of given codim
     *  @param if: one of the predifined interface types, throws error if it is not implemented
     *  @param level: communicate for entities on the given level
     *
     *  Implements a generic communication function sending an object of type P for each entity
     *  in the intersection of two processors. P has two methods gather and scatter that implement
     *  the protocol. Therefore P is called the "protocol class".
     */
    template<class T, template<class> class P, int codim>
    void communicate (T& t, InterfaceType iftype, CommunicationDirection dir, int level);

    /*! The new communication interface

       communicate objects for all codims on a given level
     */
    template<class DataHandle>
    void communicate (DataHandle& data, InterfaceType iftype, CommunicationDirection dir, int level) const
    {}

    template<class DataHandle>
    void communicate (DataHandle& data, InterfaceType iftype, CommunicationDirection dir) const
    {}
#endif


    /** \brief dummy collective communication */
    const CollectiveCommunication<IdentityGrid>& comm () const
    {
      return ccobj;
    }


    // **********************************************************
    // End of Interface Methods
    // **********************************************************

    //! Returns the hostgrid this IdentityGrid lives in
    HostGridType& getHostGrid() const
    {
      return *hostgrid_;
    }


    //! Returns the hostgrid entity encapsulated in given IdentityGrid entity
    template <int codim>
    DUNE_DEPRECATED_MSG("getHostEntityPointer() is deprecated and will be removed after the release of dune-grid 2.4. Use getHostEntity() instead to obtain a reference to the host entity object.")
    typename HostGrid::Traits::template Codim<codim>::EntityPointer getHostEntityPointer(const typename Traits::template Codim<codim>::Entity& e) const
    {
      return this->getRealImplementation(e).hostEntity_;
    }

    template <int codim>
    const typename HostGrid::Traits::template Codim<codim>::Entity& getHostEntity(const typename Traits::template Codim<codim>::Entity& e) const
    {
      return this->getRealImplementation(e).hostEntity_;
    }

  protected:

    //! The host grid which contains the actual grid hierarchy structure
    HostGrid* hostgrid_;

  private:

    //! compute the grid indices and ids
    void setIndices()
    {
      localIdSet_.update();

      globalIdSet_.update();

      // //////////////////////////////////////////
      //   Create the index sets
      // //////////////////////////////////////////
      for (int i=levelIndexSets_.size(); i<=maxLevel(); i++) {
        IdentityGridLevelIndexSet<const IdentityGrid<HostGrid> >* p
          = new IdentityGridLevelIndexSet<const IdentityGrid<HostGrid> >();
        levelIndexSets_.push_back(p);
      }

      for (int i=0; i<=maxLevel(); i++)
        if (levelIndexSets_[i])
          levelIndexSets_[i]->update(*this, i);

      leafIndexSet_.update(*this);

    }

    //! \todo Please doc me !
    CollectiveCommunication<IdentityGrid> ccobj;

    //! Our set of level indices
    std::vector<IdentityGridLevelIndexSet<const IdentityGrid<HostGrid> >*> levelIndexSets_;

    //! \todo Please doc me !
    IdentityGridLeafIndexSet<const IdentityGrid<HostGrid> > leafIndexSet_;

    //! \todo Please doc me !
    IdentityGridGlobalIdSet<const IdentityGrid<HostGrid> > globalIdSet_;

    //! \todo Please doc me !
    IdentityGridLocalIdSet<const IdentityGrid<HostGrid> > localIdSet_;

  }; // end Class IdentityGrid




  namespace Capabilities
  {
    /** \brief has entities for some codimensions as host grid
     * \ingroup IdentityGrid
     */
    template<class HostGrid, int codim>
    struct hasEntity<IdentityGrid<HostGrid>, codim>
    {
      static const bool v = hasEntity<HostGrid,codim>::v;
    };

    /** \brief has conforming level grids when host grid has
     * \ingroup IdentityGrid
     */
    template<class HostGrid>
    struct isLevelwiseConforming<IdentityGrid<HostGrid> >
    {
      static const bool v = isLevelwiseConforming<HostGrid>::v;
    };

    /** \brief has conforming leaf grids when host grid has
     * \ingroup IdentityGrid
     */
    template<class HostGrid>
    struct isLeafwiseConforming<IdentityGrid<HostGrid> >
    {
      static const bool v = isLeafwiseConforming<HostGrid>::v;
    };
  } // end namespace Capabilities

} // namespace Dune

#endif // DUNE_GRID_IDENTITYGRID_HH
