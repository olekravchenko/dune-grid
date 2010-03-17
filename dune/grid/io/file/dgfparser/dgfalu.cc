// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
/**\cond */
namespace Dune
{

  bool DGFGridFactory< ALUSimplexGrid< 3, 3 > >
  ::generate( std::istream &file, MPICommunicatorType communicator, const std::string &filename )
  {
    const int dimworld = 3;
    dgf_.element = DuneGridFormatParser::Simplex;
    dgf_.dimgrid = dimworld;
    dgf_.dimw = dimworld;
    int rank = 0;
#if ALU3DGRID_PARALLEL
    MPI_Comm_rank( communicator, &rank );
#endif

    if( !dgf_.readDuneGrid( file, dimworld, dimworld ) )
      return false;

    if( dgf_.dimw != dimworld )
      DUNE_THROW( DGFException, "Macrofile is for dimension " << dgf_.dimw
                                                              << " and cannot be used to initialize an "
                                                              << "ALUGrid of dimension " << dimension << ".");
    if (dimworld == 3)
      dgf_.setOrientation( 2, 3 );

    dgf::GridParameterBlock parameter( file );
    if( rank == 0 )
    {
      for( int n = 0; n < dgf_.nofvtx; ++n )
      {
        FieldVector< double, dimworld > pos;
        for( int i = 0; i < dimworld; ++i )
          pos[ i ] = dgf_.vtx[ n ][ i ];
        factory_.insertVertex( pos );
      }

      GeometryType elementType( GeometryType::simplex, dimworld );
      for( int n = 0; n < dgf_.nofelements; ++n )
      {
        factory_.insertElement( elementType, dgf_.elements[ n ] );
        for( int face = 0; face <= dimworld; ++face )
        {
          typedef DuneGridFormatParser::facemap_t::key_type Key;
          typedef DuneGridFormatParser::facemap_t::iterator Iterator;

          const Key key = ElementFaceUtil::generateFace( dimworld, dgf_.elements[ n ], face );
          const Iterator it = dgf_.facemap.find( key );
          if( it != dgf_.facemap.end() )
            factory_.insertBoundary( n, face, it->second );
        }
      }

      dgf::ProjectionBlock projectionBlock( file, dimworld );
      const DuneBoundaryProjection< dimworld > *projection
        = projectionBlock.defaultProjection< dimworld >();
      if( projection != 0 )
        factory_.insertBoundaryProjection( *projection );
      const size_t numBoundaryProjections = projectionBlock.numBoundaryProjections();
      for( size_t i = 0; i < numBoundaryProjections; ++i )
      {
        GeometryType type( GeometryType::simplex, dimworld-1 );
        const std::vector< unsigned int > &vertices = projectionBlock.boundaryFace( i );
        const DuneBoundaryProjection< dimworld > *projection
          = projectionBlock.boundaryProjection< dimworld >( i );
        factory_.insertBoundaryProjection( type, vertices, projection );
      }
    }

    if ( ! parameter.dumpFileName().empty() )
      grid_ = factory_.createGrid( dgf_.facemap.empty(), false, parameter.dumpFileName() );
    else
      grid_ = factory_.createGrid( dgf_.facemap.empty(), true, filename );
    return true;
  }
  bool DGFGridFactory< ALUCubeGrid< 3, 3 > >
  ::generate( std::istream &file, MPICommunicatorType communicator, const std::string &filename )
  {
    const int dimworld = 3;
    dgf_.element = DuneGridFormatParser::Cube;
    dgf_.dimgrid = dimworld;
    dgf_.dimw = dimworld;
    int rank = 0;
#if ALU3DGRID_PARALLEL
    MPI_Comm_rank( communicator, &rank );
#endif

    if( !dgf_.readDuneGrid( file, dimworld, dimworld ) )
      return false;

    if( dgf_.dimw != dimworld )
      DUNE_THROW( DGFException, "Macrofile is for dimension " << dgf_.dimw
                                                              << " and cannot be used to initialize an "
                                                              << "ALUGrid of dimension " << dimension << ".");

    dgf::GridParameterBlock parameter( file );
    if( rank == 0 )
    {
      for( int n = 0; n < dgf_.nofvtx; ++n )
      {
        FieldVector< double, dimworld > pos;
        for( int i = 0; i < dimworld; ++i )
          pos[ i ] = dgf_.vtx[ n ][ i ];
        factory_.insertVertex( pos );
      }

      GeometryType elementType( GeometryType::cube, dimworld );
      for( int n = 0; n < dgf_.nofelements; ++n )
      {
        factory_.insertElement( elementType, dgf_.elements[ n ] );
        for( int face = 0; face < 2*dimworld; ++face )
        {
          typedef DuneGridFormatParser::facemap_t::key_type Key;
          typedef DuneGridFormatParser::facemap_t::iterator Iterator;

          const Key key = ElementFaceUtil::generateFace( dimworld, dgf_.elements[ n ], face );
          const Iterator it = dgf_.facemap.find( key );
          if( it != dgf_.facemap.end() )
            factory_.insertBoundary( n, face, it->second );
        }
      }

      dgf::ProjectionBlock projectionBlock( file, dimworld );
      const DuneBoundaryProjection< dimworld > *projection
        = projectionBlock.defaultProjection< dimworld >();
      if( projection != 0 )
        factory_.insertBoundaryProjection( *projection );
      const size_t numBoundaryProjections = projectionBlock.numBoundaryProjections();
      for( size_t i = 0; i < numBoundaryProjections; ++i )
      {
        GeometryType type( GeometryType::cube, dimworld-1 );
        const std::vector< unsigned int > &vertices = projectionBlock.boundaryFace( i );
        const DuneBoundaryProjection< dimworld > *projection
          = projectionBlock.boundaryProjection< dimworld >( i );
        factory_.insertBoundaryProjection( type, vertices, projection );
      }
    }

    if ( ! parameter.dumpFileName().empty() )
      grid_ = factory_.createGrid( dgf_.facemap.empty(), false, parameter.dumpFileName() );
    else
      grid_ = factory_.createGrid( dgf_.facemap.empty(), true, filename );
    return true;
  }

  template <int dimw>
  bool DGFGridFactory< ALUSimplexGrid< 2, dimw > >
  ::generate( std::istream &file, MPICommunicatorType communicator, const std::string &filename )
  {
    const int dimworld = dimw;
    dgf_.element = DuneGridFormatParser::Simplex;
    dgf_.dimgrid = 2;
    dgf_.dimw = dimworld;
    int rank = 0;
#if ALU2DGRID_PARALLEL
    MPI_Comm_rank( communicator, &rank );
#endif

    if( !dgf_.readDuneGrid( file, 2, dimworld ) )
      return false;

    if( dgf_.dimw != dimworld )
      DUNE_THROW( DGFException, "Macrofile is for dimension " << dgf_.dimw
                                                              << " and cannot be used to initialize an "
                                                              << "ALUGrid of dimension " << dimension << ".");

    dgf::GridParameterBlock parameter( file );

    if( rank == 0 )
    {
      for( int n = 0; n < dgf_.nofvtx; ++n )
      {
        FieldVector< double, dimworld > pos;
        for( int i = 0; i < dimworld; ++i )
          pos[ i ] = dgf_.vtx[ n ][ i ];
        factory_.insertVertex( pos );
      }

      GeometryType elementType( GeometryType::simplex, 2 );
      for( int n = 0; n < dgf_.nofelements; ++n )
      {
        factory_.insertElement( elementType, dgf_.elements[ n ] );
        for( int face = 0; face <= dimworld; ++face )
        {
          typedef typename DuneGridFormatParser::facemap_t::key_type Key;
          typedef typename DuneGridFormatParser::facemap_t::iterator Iterator;

          const Key key = ElementFaceUtil::generateFace( dimworld, dgf_.elements[ n ], face );
          const Iterator it = dgf_.facemap.find( key );
          if( it != dgf_.facemap.end() )
            factory_.insertBoundary( n, face, it->second );
        }
      }

      dgf::ProjectionBlock projectionBlock( file, dimworld );
      const DuneBoundaryProjection< dimworld > *projection
        = projectionBlock.defaultProjection< dimworld >();
      if( projection != 0 )
        factory_.insertBoundaryProjection( *projection );
      const size_t numBoundaryProjections = projectionBlock.numBoundaryProjections();
      for( size_t i = 0; i < numBoundaryProjections; ++i )
      {
        GeometryType type( GeometryType::simplex, dimworld-1 );
        const std::vector< unsigned int > &vertices = projectionBlock.boundaryFace( i );
        const DuneBoundaryProjection< dimworld > *projection
          = projectionBlock.boundaryProjection< dimworld >( i );
        factory_.insertBoundaryProjection( type, vertices, projection );
      }
    }

    if ( ! parameter.dumpFileName().empty() )
      grid_ = factory_.createGrid( dgf_.facemap.empty(), false, parameter.dumpFileName() );
    else
      grid_ = factory_.createGrid( dgf_.facemap.empty(), true, filename );
    return true;
  }

  template <int dimw>
  bool DGFGridFactory< ALUCubeGrid< 2, dimw > >
  ::generate( std::istream &file, MPICommunicatorType communicator, const std::string &filename )
  {
    const int dimworld = dimw;
    dgf_.element = DuneGridFormatParser::Cube;
    dgf_.dimgrid = 2;
    dgf_.dimw = dimworld;
    int rank = 0;
#if ALU2DGRID_PARALLEL
    MPI_Comm_rank( communicator, &rank );
#endif

    if( !dgf_.readDuneGrid( file, 2, dimworld ) )
      return false;

    if( dgf_.dimw != dimworld )
      DUNE_THROW( DGFException, "Macrofile is for dimension " << dgf_.dimw
                                                              << " and cannot be used to initialize an "
                                                              << "ALUGrid of dimension " << dimension << ".");

    dgf::GridParameterBlock parameter( file );

    if( rank == 0 )
    {
      for( int n = 0; n < dgf_.nofvtx; ++n )
      {
        FieldVector< double, dimworld > pos;
        for( int i = 0; i < dimworld; ++i )
          pos[ i ] = dgf_.vtx[ n ][ i ];
        factory_.insertVertex( pos );
      }

      GeometryType elementType( GeometryType::cube, 2 );
      for( int n = 0; n < dgf_.nofelements; ++n )
      {
        factory_.insertElement( elementType, dgf_.elements[ n ] );
        for( int face = 0; face <= dimworld; ++face )
        {
          typedef typename DuneGridFormatParser::facemap_t::key_type Key;
          typedef typename DuneGridFormatParser::facemap_t::iterator Iterator;

          const Key key = ElementFaceUtil::generateFace( dimworld, dgf_.elements[ n ], face );
          const Iterator it = dgf_.facemap.find( key );
          if( it != dgf_.facemap.end() )
            factory_.insertBoundary( n, face, it->second );
        }
      }

      dgf::ProjectionBlock projectionBlock( file, dimworld );
      const DuneBoundaryProjection< dimworld > *projection
        = projectionBlock.defaultProjection< dimworld >();
      if( projection != 0 )
        factory_.insertBoundaryProjection( *projection );
      const size_t numBoundaryProjections = projectionBlock.numBoundaryProjections();
      for( size_t i = 0; i < numBoundaryProjections; ++i )
      {
        GeometryType type( GeometryType::simplex, dimworld-1 );
        const std::vector< unsigned int > &vertices = projectionBlock.boundaryFace( i );
        const DuneBoundaryProjection< dimworld > *projection
          = projectionBlock.boundaryProjection< dimworld >( i );
        factory_.insertBoundaryProjection( type, vertices, projection );
      }
    }

    if ( ! parameter.dumpFileName().empty() )
      grid_ = factory_.createGrid( dgf_.facemap.empty(), false, parameter.dumpFileName() );
    else
      grid_ = factory_.createGrid( dgf_.facemap.empty(), true, filename );
    return true;
  }

  template <int dimw>
  bool DGFGridFactory< ALUConformGrid< 2, dimw > >
  ::generate( std::istream &file, MPICommunicatorType communicator, const std::string &filename )
  {
    const int dimworld = dimw;
    dgf_.element = DuneGridFormatParser::Simplex;
    dgf_.dimgrid = 2;
    dgf_.dimw = dimworld;
    int rank = 0;
#if ALU2DGRID_PARALLEL
    MPI_Comm_rank( communicator, &rank );
#endif

    if( !dgf_.readDuneGrid( file, 2, dimworld ) )
      return false;
    if( dgf_.dimw != dimworld )
      DUNE_THROW( DGFException, "Macrofile is for dimension " << dgf_.dimw
                                                              << " and cannot be used to initialize an "
                                                              << "ALUGrid of dimension " << dimension << ".");

    dgf::GridParameterBlock parameter( file );

    if( rank == 0 )
    {
      for( int n = 0; n < dgf_.nofvtx; ++n )
      {
        FieldVector< double, dimworld > pos;
        for( int i = 0; i < dimworld; ++i )
          pos[ i ] = dgf_.vtx[ n ][ i ];
        factory_.insertVertex( pos );
      }

      GeometryType elementType( GeometryType::simplex, 2 );
      for( int n = 0; n < dgf_.nofelements; ++n )
      {
        factory_.insertElement( elementType, dgf_.elements[ n ] );
        for( int face = 0; face <= dimworld; ++face )
        {
          typedef typename DuneGridFormatParser::facemap_t::key_type Key;
          typedef typename DuneGridFormatParser::facemap_t::iterator Iterator;

          const Key key = ElementFaceUtil::generateFace( dimworld, dgf_.elements[ n ], face );
          const Iterator it = dgf_.facemap.find( key );
          if( it != dgf_.facemap.end() )
            factory_.insertBoundary( n, face, it->second );
        }
      }

      dgf::ProjectionBlock projectionBlock( file, dimworld );
      const DuneBoundaryProjection< dimworld > *projection
        = projectionBlock.defaultProjection< dimworld >();
      if( projection != 0 )
        factory_.insertBoundaryProjection( *projection );
      const size_t numBoundaryProjections = projectionBlock.numBoundaryProjections();
      for( size_t i = 0; i < numBoundaryProjections; ++i )
      {
        GeometryType type( GeometryType::cube, dimworld-1 );
        const std::vector< unsigned int > &vertices = projectionBlock.boundaryFace( i );
        const DuneBoundaryProjection< dimworld > *projection
          = projectionBlock.boundaryProjection< dimworld >( i );
        factory_.insertBoundaryProjection( type, vertices, projection );
      }
    }

    if ( ! parameter.dumpFileName().empty() )
      grid_ = factory_.createGrid( dgf_.facemap.empty(), false, parameter.dumpFileName() );
    else
      grid_ = factory_.createGrid( dgf_.facemap.empty(), true, filename );
    return true;
  }

} // end namespace Dune
  /** \endcond */
