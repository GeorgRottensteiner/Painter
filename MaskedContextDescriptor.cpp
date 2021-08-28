// MaskedContextDescriptor.cpp: Implementierung der Klasse CMaskedContextDescriptor.
//
//////////////////////////////////////////////////////////////////////

#include "MaskedContextDescriptor.h"
#include <Grafik/Palette.h>
#include <Grafik/GfxPage.h>

#include <debug/debugclient.h>

#include <Misc/Misc.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMaskedContextDescriptor::CMaskedContextDescriptor()
: GR::Graphic::ContextDescriptor(),
  /*
  m_pPalette( NULL ),
  m_pData( NULL ),
  */
  m_pMaskData( NULL )
{

}


CMaskedContextDescriptor::CMaskedContextDescriptor( GR::Graphic::Image* pImage, GR::Graphic::Palette* pPalette ) :
  GR::Graphic::ContextDescriptor( pImage, pPalette ),
  m_pMaskData( NULL )
{
}

CMaskedContextDescriptor::CMaskedContextDescriptor( GR::Graphic::GFXPage* pPage, GR::Graphic::Palette* pPalette ) :
  GR::Graphic::ContextDescriptor( pPage, pPalette ),
  m_pMaskData( NULL )
{
}

CMaskedContextDescriptor::~CMaskedContextDescriptor()
{

  GR::Graphic::ContextDescriptor::~ContextDescriptor();

}



void CMaskedContextDescriptor::PutPixel( int iX, int iY, DWORD dwColor )
{

  if ( m_pData == NULL )
  {
    return;
  }
  if ( ( iX < 0 )
  ||   ( iY < 0 )
  ||   ( iX >= m_Width )
  ||   ( iY >= m_Height ) )
  {
    return;
  }
  if ( ( ( m_pMaskData )
  &&     ( ( (BYTE*)m_pMaskData )[iX + iY * m_Width] ) )
  ||   ( !m_pMaskData ) )
  {
    GR::Graphic::ContextDescriptor::PutPixel( iX, iY, dwColor );
    /*
    switch ( m_dwBitDepth )
    {
      case 1:
        {
          BYTE*   pTempData = ((BYTE*)m_pData ) + ( iX / 8 ) + iY * m_iLineOffsetInBytes;

          BYTE    ucMask = 1 << ( 7 - iX % 8 );

          if ( dwColor )
          {
            *pTempData |= ucMask;
          }
          else
          {
            *pTempData &= ~ucMask;
          }
        }
        break;
      case 4:
        {
          BYTE*   pData = ((BYTE*)m_pData ) + ( iX >> 1 ) + iY * m_iLineOffsetInBytes;
          if ( iX & 1 )
          {
            *pData &= 0xf0;
            *pData |= ( dwColor & 0x0f );
          }
          else
          {
            *pData &= 0x0f;
            *pData |= ( dwColor << 4 );
          }
        }
        break;
      case 8:
        ((BYTE*)m_pData )[iX + iY * m_iLineOffsetInBytes] = (BYTE)dwColor;//SeekColor( dwColor );
        break;
      case 15:
        ((WORD*)m_pData )[iX + iY * ( m_iLineOffsetInBytes >> 1 )] = 
               (WORD)( ( ( ( dwColor & 0xff0000 ) >> 19 ) << 10 )
                     + ( ( ( dwColor & 0x00ff00 ) >> 11 ) <<  5 )
                     + ( ( ( dwColor & 0x0000ff ) >>  3 ) ) );
        break;
      case 16:
        ((WORD*)m_pData )[iX + iY * ( m_iLineOffsetInBytes >> 1 )] = 
               (WORD)( ( ( ( dwColor & 0xff0000 ) >> 19 ) << 11 )
                     + ( ( ( dwColor & 0x00ff00 ) >> 10 ) <<  5 )
                     + ( ( ( dwColor & 0x0000ff ) >>  3 ) ) );
        break;
      case 24:
        ((BYTE*)m_pData )[3 * iX + iY * m_iLineOffsetInBytes ]     = (BYTE)( dwColor & 0x0ff );
        ((BYTE*)m_pData )[3 * iX + iY * m_iLineOffsetInBytes + 1] = (BYTE)( ( dwColor & 0x0ff00 ) >> 8 );
        ((BYTE*)m_pData )[3 * iX + iY * m_iLineOffsetInBytes + 2] = (BYTE)( ( dwColor & 0x0ff0000 ) >> 16 );
        break;
      case 32:
        ((DWORD*)m_pData )[iX + iY * ( m_iLineOffsetInBytes >> 2 )] = dwColor;
        break;
      default:
        dh::Log( "CMaskedContextDescriptor::PutPixel unsupported depth %d\n", m_dwBitDepth );
        break;
    }
    */
  }

}



/*-PutDirectPixel-------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

/*
void CMaskedContextDescriptor::PutDirectPixel( int iX, int iY, DWORD dwColor )
{

  if ( m_pData == NULL )
  {
    return;
  }
  if ( ( iX < 0 )
  ||   ( iY < 0 )
  ||   ( iX >= m_iWidth )
  ||   ( iY >= m_iHeight ) )
  {
    return;
  }

  switch ( m_dwBitDepth )
  {
    case 8:
      ((BYTE*)m_pData )[iX + iY * m_iLineOffsetInBytes] = (BYTE)dwColor;
      break;
    case 15:
    case 16:
      ((WORD*)m_pData )[iX + iY * ( m_iLineOffsetInBytes >> 1 )] = (WORD)dwColor;
      break;
    case 24:
      ((BYTE*)m_pData )[3 * iX + iY * m_iLineOffsetInBytes ]     = (BYTE)( dwColor & 0x0ff );
      ((BYTE*)m_pData )[3 * iX + iY * m_iLineOffsetInBytes + 1] = (BYTE)( ( dwColor & 0x0ff00 ) >> 8 );
      ((BYTE*)m_pData )[3 * iX + iY * m_iLineOffsetInBytes + 2] = (BYTE)( ( dwColor & 0x0ff0000 ) >> 16 );
      break;
    case 32:
      ((DWORD*)m_pData )[iX + iY * ( m_iLineOffsetInBytes >> 2 )] = dwColor;
      break;
  }

}
*/



GR::u8 CMaskedContextDescriptor::GetMaskValueAt( int iX, int iY )
{

  if ( m_pMaskData == NULL )
  {
    return 255;
  }

  if ( ( iX < 0 )
  ||   ( iY < 0 )
  ||   ( iX >= m_Width )
  ||   ( iY >= m_Height ) )
  {
    return 0;
  }

  return ( (BYTE*)m_pMaskData )[iX + iY * m_Width];

}