// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_GENERICGEOMETRY_GEOMETRY_HH
#define DUNE_GENERICGEOMETRY_GEOMETRY_HH

#include <dune/grid/common/geometry.hh>

#include <dune/grid/genericgeometry/mappingprovider.hh>
#include <dune/grid/genericgeometry/geometrytraits.hh>

namespace Dune
{

  namespace GenericGeometry
  {

    // BasicGeometry
    // -------------

    /** \class   BasicGeometry
     *  \ingroup GenericGeometry
     *  \brief   generic implementation of DUNE geometries
     *
     *  This class is provides a generic implementation of a DUNE geometry.
     *
     *  Parameters shared by all codimensions are summarized into one class
     *  parameter called Traits. The following default implementation can be
     *  used (via derivation) to provide the necessary information. It contains
     *  exactly the required fields:
     *  \code
     *  template< class ctype, int dimG, int dimW >
     *  struct DefaultGeometryTraits
     *  {
     *    typedef DuneCoordTraits< ctype > CoordTraits;
     *
     *    static const int dimGrid = dimG;
     *    static const int dimWorld = dimW;
     *
     *    //   hybrid   [ true if Codim 0 is hybrid ]
     *    static const bool hybrid = true;
     *    //   dunetype [ for Codim 0, needed for (hybrid=false) ]
     *    // static const GeometryType :: BasicType dunetype = GeometryType :: simplex;
     *
     *    // what basic geometry type shall the line be considered?
     *    static const GeometryType :: BasicType linetype = GeometryType :: simplex;
     *
     *    template< class Topology >
     *    struct Mapping
     *    {
     *      typedef MappingTraits< CoordTraits, Topology :: dimension, dimWorld > Traits;
     *      typedef CoordPointerStorage< Topology, typename Traits :: GlobalCoordType >
     *        CornerStorage;
     *      typedef CornerMapping< Topology, Traits, CornerStorage > type;
     *    };
     *
     *    struct Caching
     *    {
     *      static const EvaluationType evaluateJacobianTransposed = ComputeOnDemand;
     *      static const EvaluationType evaluateJacobianInverseTransposed = ComputeOnDemand;
     *      static const EvaluationType evaluateIntegrationElement = ComputeOnDemand;
     *      static const EvaluationType evaluateNormal = ComputeOnDemand;
     *    };
     *  };
     *  \endcode
     *
     *  \note This class cannot be used directly a an implementation of
     *        Dune::Geometry. Its template parameter list differs from what
     *        is expected there. Use one of the following derived classes
     *        instead:
     *        - Dune::GenericGeometry::Geometry
     *        - Dune::GenericGeometry::LocalGemetry
     *        .
     */
    template< int mydim, class Traits >
    class BasicGeometry
    {
      typedef typename Traits :: CoordTraits CoordTraits;

      static const int dimGrid = Traits :: dimGrid;

      template< int, class > friend class BasicGeometry;

    public:
      static const int mydimension = mydim;
      static const int coorddimension = Traits :: dimWorld;

      typedef typename CoordTraits :: ctype ctype;

      typedef FieldVector< ctype, mydimension > LocalCoordinate;
      typedef FieldVector< ctype, coorddimension > GlobalCoordinate;
      typedef FieldMatrix< ctype, coorddimension, mydimension > Jacobian;

    private:
      dune_static_assert( (0 <= mydimension) && (mydimension <= dimGrid),
                          "Invalid geometry dimension." );

      static const int codimension = dimGrid - mydimension;

      template< bool >
      struct Hybrid
      {
        typedef HybridMapping< dimGrid, Traits > Mapping;
      };

      template< bool >
      struct NonHybrid
      {
        typedef typename Convert< Traits :: dunetype, dimGrid > :: type Topology;
        typedef GenericGeometry :: CachedMapping< Topology, Traits > Mapping;
      };

      typedef GenericGeometry :: DuneGeometryTypeProvider< mydimension, Traits :: linetype >
      DuneGeometryTypeProvider;

      typedef typename ProtectedIf< Traits :: hybrid, Hybrid, NonHybrid > :: Mapping
      ElementMapping;
      typedef GenericGeometry :: MappingProvider< ElementMapping, codimension >
      MappingProvider;

    protected:
      typedef typename MappingProvider :: Mapping Mapping;

    private:
      Mapping *mapping_;

    public:
      BasicGeometry ()
        : mapping_( 0 )
      {}

#if 0
      explicit BasicGeometry ( Mapping &mapping )
        : mapping_( &mapping )
      {
        ++mapping_->referenceCount;
      }
#endif

      template< class CoordVector >
      BasicGeometry ( const GeometryType &type, const CoordVector &coords )
        : mapping_( MappingProvider :: mapping( type, coords ) )
      {
        mapping_->referenceCount = 1;
      }

      template< int fatherdim >
      BasicGeometry ( const BasicGeometry< fatherdim, Traits > &father, int i )
        : mapping_( subMapping( father, i ) )
      {
        mapping_->referenceCount = 1;
      }

      BasicGeometry ( const BasicGeometry &other )
        : mapping_( other.mapping_ )
      {
        if( mapping_ != 0 )
          ++(mapping_->referenceCount);
      }

      ~BasicGeometry ()
      {
        if( (mapping_ != 0) && ((--mapping_->referenceCount) == 0) )
          delete mapping_;
      }

      BasicGeometry &operator= ( const BasicGeometry &other )
      {
        if( other.mapping_ != 0 )
          ++(other.mapping_->referenceCount);
        if( (mapping_ != 0) && (--(mapping_->referenceCount) == 0) )
          delete mapping_;
        mapping_ = other.mapping_;
        return *this;
      }

    public:
      bool operator! () const
      {
        return (mapping_ == 0);
      }

      GeometryType type () const
      {
        return DuneGeometryTypeProvider :: type( mapping().topologyId() );
      }

      int corners () const
      {
        return mapping().numCorners();
      }

      const GlobalCoordinate &operator[] ( int i ) const
      {
        return mapping().corner( i );
      }

      GlobalCoordinate global ( const LocalCoordinate &local ) const
      {
        return mapping().global( local );
      }

      LocalCoordinate local ( const GlobalCoordinate &global ) const
      {
        return mapping().local( global );
      }

      bool checkInside ( const LocalCoordinate &local ) const
      {
        return mapping().checkInside( local );
      }

      bool affine () const
      {
        return mapping().affine();
      }

      ctype integrationElement ( const LocalCoordinate &local ) const
      {
        return mapping().integrationElement( local );
      }

      ctype volume () const
      {
        return mapping().volume();
      }

      const Jacobian &jacobianInverseTransposed ( const LocalCoordinate &local ) const
      {
        return mapping().jacobianInverseTransposed( local );
      }

      GlobalCoordinate normal ( int face, const LocalCoordinate &local ) const
      {
        const unsigned int tid = mapping().topologyId();
        const unsigned int i = MapNumberingProvider< mydimension >
                               :: template dune2generic< 1 >( tid, face );
        return mapping().normal( i, local );
      }

    private:
      const Mapping &mapping () const
      {
        assert( mapping_ != 0 );
        return *mapping_;
      }

      template< int fatherdim >
      Mapping *
      subMapping ( const BasicGeometry< fatherdim, Traits > &father, int i )
      {
        const unsigned int codim = fatherdim - mydim;
        const unsigned int ftid = father.mapping().topologyId();
        const unsigned int j = MapNumberingProvider< fatherdim >
                               :: template dune2generic< codim >( ftid, i );
        return father.mapping().template trace< codim >( j );
      }
    };



    // Geometry
    // --------

    /** \class   Geometry
     *  \ingroup GenericGeometry
     *  \brief   generic implementation of a DUNE (global) geometry
     *
     *  Geometry inherits all its features from Geometry. It only add
     *  GlobalGeometryTraits< Grid > as Traits parameter to the template
     *  parameter list.
     */
    template< int mydim, int cdim, class Grid >
    class Geometry
      : public BasicGeometry< mydim, GlobalGeometryTraits< Grid > >
    {
      typedef BasicGeometry< mydim, GlobalGeometryTraits< Grid > > Base;

    protected:
      typedef typename Base :: Mapping Mapping;

    public:
      Geometry ()
        : Base()
      {}

      explicit Geometry ( Mapping &mapping )
        : Base( mapping )
      {}

      template< class Geo >
      explicit Geometry ( const Geo &geo )
        : Base( geo.type(), geo )
      {}

      template< class CoordVector >
      Geometry ( const GeometryType &type,
                 const CoordVector &coords )
        : Base( type, coords )
      {}

      template< int fatherdim >
      Geometry ( const Geometry< fatherdim, cdim, Grid > &father, int i )
        : Base( father, i )
      {}
    };



    // LocalGeometry
    // -------------

    /** \class   LocalGeometry
     *  \ingroup GenericGeometry
     *  \brief   generic implementation of a DUNE (local) geometry
     *
     *  Geometry inherits all its features from Geometry. It only add
     *  LocalGeometryTraits< Grid > as Traits parameter to the template
     *  parameter list.
     */
    template< int mydim, int cdim, class Grid >
    class LocalGeometry
      : public BasicGeometry< mydim, LocalGeometryTraits< Grid > >
    {
      typedef BasicGeometry< mydim, LocalGeometryTraits< Grid > > Base;

    protected:
      typedef typename Base :: Mapping Mapping;

    public:
      LocalGeometry ()
        : Base()
      {}

      explicit LocalGeometry ( Mapping &mapping )
        : Base( mapping )
      {}

      template< class Geo >
      explicit LocalGeometry ( const Geo &geo )
        : Base( geo.type(), geo )
      {}

      template< class CoordVector >
      LocalGeometry ( const GeometryType &type, const CoordVector &coords )
        : Base( type, coords )
      {}

      template< int fatherdim >
      LocalGeometry ( const Geometry< fatherdim, cdim, Grid > &father, int i )
        : Base( father, i )
      {}
    };

  }

}

#endif
