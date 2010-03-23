// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#include <config.h>

#if HAVE_ALBERTA

#include <dune/grid/albertagrid/meshpointer.hh>

namespace Dune
{

  namespace Alberta
  {

    // Implementation of MeshPointer
    // -----------------------------

    template< int dim >
    template< int dimWorld >
    unsigned int MeshPointer< dim >::Library< dimWorld >::boundaryCount = 0;

    template< int dim >
    template< int dimWorld >
    const void *MeshPointer< dim >::Library< dimWorld >::projectionFactory = 0;



    template< int dim >
    template< int dimWorld >
    void MeshPointer< dim >::Library< dimWorld >::release ( MeshPointer &ptr )
    {
      if( ptr.mesh_ == NULL )
        return;

      // free projections
      const MacroIterator eit = ptr.end();
      for( MacroIterator it = ptr.begin(); it != eit; ++it )
      {
        MacroElement &macroEl = const_cast< MacroElement & >( it.macroElement() );
        for( int i = 0; i <= dim+1; ++i )
        {
          if( macroEl.projection[ i ] != NULL )
          {
            delete static_cast< BasicNodeProjection * >( macroEl.projection[ i ] );
            macroEl.projection[ i ] = NULL;
          }
        }
      }

      // free mesh
      ALBERTA free_mesh( ptr.mesh_ );
      ptr.mesh_ = NULL;
    }



    // Instantiation
    // -------------

    template struct Dune::Alberta::MeshPointer< 1 >::Library< dimWorld >;
#if ALBERTA_DIM >= 2
    template struct Dune::Alberta::MeshPointer< 2 >::Library< dimWorld >;
#endif // #if ALBERTA_DIM >= 2
#if ALBERTA_DIM >= 3
    template struct Dune::Alberta::MeshPointer< 3 >::Library< dimWorld >;
#endif // #if ALBERTA_DIM >= 3

  }

}

#endif // #if HAVE_ALBERTA
