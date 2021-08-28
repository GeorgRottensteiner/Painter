// GFXUtil.cpp: implementation of the CGFXUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Settings.h"
#include "GFXUtil.h"

#include <Grafik\ContextDescriptor.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGFXUtil::CGFXUtil()
{

}

CGFXUtil::~CGFXUtil()
{

}



void CGFXUtil::ReplaceColor( tImageList& list, DWORD dwColor1, DWORD dwColor2, GR::Graphic::Image* pMask )
{

  tImageList::iterator    it( list.begin() );
  while ( it != list.end() )
  {
    GR::Graphic::Image*    pImage = *it;

    if ( pImage == NULL )
    {
      ++it;
      continue;
    }

    GR::Graphic::ContextDescriptor    cdImage( pImage );

    DWORD         dwReplaceColor1 = cdImage.MapColor( dwColor1 ),
                  dwReplaceColor2 = cdImage.MapColor( dwColor2 );

    if ( pImage->GetDepth() <= 8 )
    {
      dwReplaceColor1 = dwColor1;
      dwReplaceColor2 = dwColor2;
    }

    for ( int j = 0; j < cdImage.Height(); ++j )
    {
      for ( int i = 0; i < cdImage.Width(); ++i )
      {
        if ( ( ( pMask )
        &&     ( pMask->GetPixel( i, j ) ) )
        ||   ( pMask == NULL ) )
        {
          if ( cdImage.GetDirectPixel( i, j ) == dwReplaceColor1 )
          {
            cdImage.PutDirectPixel( i, j, dwReplaceColor2 );
          }
        }
      }
    }

    ++it;
  }

}



void CGFXUtil::SwapColor( tImageList& list, GR::u32 Color1, GR::u32 Color2, GR::Graphic::Image* pMask )
{
  tImageList::iterator    it( list.begin() );
  while ( it != list.end() )
  {
    GR::Graphic::Image*    pImage = *it;

    if ( pImage == NULL )
    {
      ++it;
      continue;
    }

    GR::Graphic::ContextDescriptor    cdImage( pImage );

    GR::u32       replaceColor1 = cdImage.MapColor( Color1 ),
                  replaceColor2 = cdImage.MapColor( Color2 );

    if ( pImage->GetDepth() <= 8 )
    {
      replaceColor1 = Color1;
      replaceColor2 = Color2;
    }

    for ( int j = 0; j < cdImage.Height(); ++j )
    {
      for ( int i = 0; i < cdImage.Width(); ++i )
      {
        if ( ( ( pMask )
        &&     ( pMask->GetPixel( i, j ) ) )
        ||   ( pMask == NULL ) )
        {
          if ( cdImage.GetDirectPixel( i, j ) == replaceColor1 )
          {
            cdImage.PutDirectPixel( i, j, replaceColor2 );
          }
          else if ( cdImage.GetDirectPixel( i, j ) == replaceColor2 )
          {
            cdImage.PutDirectPixel( i, j, replaceColor1 );
          }
        }
      }
    }
    ++it;
  }
}