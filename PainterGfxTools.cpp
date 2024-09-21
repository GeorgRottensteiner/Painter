#include "stdafx.h"
#include "PainterGfxTools.h"
#include "Settings.h"
#include "ConvertOptions.h"
#include "ColorReducer.h"
#include "BrushTip.h"

#include <Debug\debugclient.h>

#include <Grafik\ContextDescriptor.h>
#include <Grafik/GFXHelper.h>
#include <Grafik/Image.h>

#include <Codec/Base64.h>

#include <Misc/Misc.h>

#include "Resampler.h"
#include "Painter.h"


#include <math.h>



void BrightenPixel( GR::Graphic::Image *pImage, GR::Graphic::Palette *pPalette, int iX, int iY )
{

  DWORD             iR,
                    iG,
                    iB;

  GR::Graphic::ContextDescriptor    cdImage( pImage, pPalette );

  DWORD             dwDummy;

  dwDummy = cdImage.GetPixel( iX, iY );

  iR = ( ( dwDummy & 0xff0000 ) >> 16 ) * 6 / 5;
  iG = ( ( dwDummy & 0x00ff00 ) >> 6 ) * 6 / 5;
  iB = ( dwDummy & 0xff ) * 6 / 5;

  if ( iR > 255 )
  {
    iR = 255;
  }
  if ( iG > 255 )
  {
    iG = 255;
  }
  if ( iB > 255 )
  {
    iB = 255;
  }

  cdImage.PutPixel( iX, iY, ( iR << 16 ) + ( iG << 8 ) + iB );

}



void DarkenPixel( GR::Graphic::Image *pImage, GR::Graphic::Palette *pPalette, int iX, int iY )
{

  DWORD             iR,
                    iG,
                    iB;

  GR::Graphic::ContextDescriptor    cdImage( pImage, pPalette );

  DWORD             dwDummy;

  dwDummy = cdImage.GetPixel( iX, iY );

  iR = ( ( dwDummy & 0xff0000 ) >> 16 ) * 5 / 6;
  iG = ( ( dwDummy & 0x00ff00 ) >> 6 ) * 5 / 6;
  iB = ( dwDummy & 0xff ) * 5 / 6;

  if ( iR > 255 )
  {
    iR = 255;
  }
  if ( iG > 255 )
  {
    iG = 255;
  }
  if ( iB > 255 )
  {
    iB = 255;
  }

  cdImage.PutPixel( iX, iY, ( iR << 16 ) + ( iG << 8 ) + iB );

}



/*-MFCGFXStretchUpdate--------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void MFCGFXStretchUpdate( HWND hWnd, HDC hDC,
                          signed long slTargetXOffset, signed long slTargetYOffset,
                          signed long slTargetWidth, signed long slTargetHeight,
                          GR::Graphic::GDIPage *pPage,
                          signed long slSourceXOffset, signed long slSourceYOffset,
                          signed long slSourceWidth, signed long slSourceHeight )
{

  RECT                  rc;


  if ( pPage->GetPaletteObject() != NULL )
  {
    SelectPalette( hDC, (HPALETTE)pPage->GetPaletteObject(), FALSE );
    RealizePalette( hDC );
  }
  GetClientRect( hWnd, &rc );
  SetStretchBltMode( hDC, COLORONCOLOR );
  StretchBlt( hDC, slTargetXOffset, slTargetYOffset, slTargetWidth, slTargetHeight,
              pPage->GetDC(),
              slSourceXOffset, slSourceYOffset, slSourceWidth, slSourceHeight, SRCCOPY );

}



/*-RotateImage----------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *RotateImage( GR::Graphic::Image *pOldImage )
{

  GR::Graphic::Image     *pNewImage;

  int           i,
                j;


  pNewImage = new GR::Graphic::Image( pOldImage->GetHeight(), pOldImage->GetWidth(), pOldImage->GetDepth(), 0, 0 );

  // TODO( "weniger als 8!" )
  if ( pNewImage->GetDepth() == 8 )
  {
    for ( i = 0; i < pOldImage->GetHeight(); i++ )
    {
      for ( j = 0; j < pOldImage->GetWidth(); j++ )
      {
        ( (BYTE*)pNewImage->GetData() )[i + j * pOldImage->GetHeight()] = 
          ( (BYTE*)pOldImage->GetData() )[j + ( pOldImage->GetHeight() - 1 - i ) * pOldImage->GetWidth()];
      }
    }
  }
  else if ( ( pNewImage->GetDepth() == 15 )
  ||        ( pNewImage->GetDepth() == 16 ) )
  {
    for ( i = 0; i < pOldImage->GetHeight(); i++ )
    {
      for ( j = 0; j < pOldImage->GetWidth(); j++ )
      {
        ( (WORD*)pNewImage->GetData() )[i + j * pOldImage->GetHeight()] = 
          ( (WORD*)pOldImage->GetData() )[j + ( pOldImage->GetHeight() - 1 - i ) * pOldImage->GetWidth()];
      }
    }
  }
  else if ( pNewImage->GetDepth() == 24 )
  {
    for ( i = 0; i < pOldImage->GetHeight(); i++ )
    {
      for ( j = 0; j < pOldImage->GetWidth(); j++ )
      {
        ( (BYTE*)pNewImage->GetData() )[3 * i + 3 * j * pOldImage->GetHeight()] = 
          ( (BYTE*)pOldImage->GetData() )[3 * j + ( pOldImage->GetHeight() - 1 - i ) * 3 * pOldImage->GetWidth()];
        ( (BYTE*)pNewImage->GetData() )[3 * i + 3 * j * pOldImage->GetHeight() + 1] = 
          ( (BYTE*)pOldImage->GetData() )[3 * j + ( pOldImage->GetHeight() - 1 - i ) * 3 * pOldImage->GetWidth() + 1];
        ( (BYTE*)pNewImage->GetData() )[3 * i + 3 * j * pOldImage->GetHeight() + 2] = 
          ( (BYTE*)pOldImage->GetData() )[3 * j + ( pOldImage->GetHeight() - 1 - i ) * 3 * pOldImage->GetWidth() + 2];
      }
    }
  }
  else if ( pNewImage->GetDepth() == 32 )
  {
    for ( i = 0; i < pOldImage->GetHeight(); i++ )
    {
      for ( j = 0; j < pOldImage->GetWidth(); j++ )
      {
        ( (DWORD*)pNewImage->GetData() )[i + j * pOldImage->GetHeight()] = 
          ( (DWORD*)pOldImage->GetData() )[j + ( pOldImage->GetHeight() - 1 - i ) * pOldImage->GetWidth()];
      }
    }
  }

  return pNewImage;

}



/*-HMirrorImage---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void HMirrorImage( GR::Graphic::Image *pImage )
{

  if ( pImage == NULL )
  {
    return;
  }


  int           i,
                j;

  BYTE          bDummy[3];

  DWORD         dwDummy;

  WORD          wWidth,
                wHeight,
                wDummy;


  wWidth = pImage->GetWidth();
  wHeight = pImage->GetHeight();

  // TODO( "weniger als 8!" )
  if ( pImage->GetDepth() == 8 )
  {
    for ( j = 0; j < (int)wHeight; j++ )
    {
      for ( i = 0; i < (int)wWidth / 2; i++ )
      {
        bDummy[0] = ( (BYTE*)pImage->GetData() )[i + j * wWidth];
        ( (BYTE*)pImage->GetData() )[i + j * wWidth] = ( (BYTE*)pImage->GetData() )[( wWidth - 1 - i ) + j * wWidth];
        ( (BYTE*)pImage->GetData() )[( wWidth - 1 - i ) + j * wWidth] = bDummy[0];
      }
    }
  }
  else if ( ( pImage->GetDepth() == 15 )
  ||        ( pImage->GetDepth() == 16 ) )
  {
    for ( j = 0; j < (int)wHeight; j++ )
    {
      for ( i = 0; i < (int)wWidth / 2; i++ )
      {
        wDummy = ( (WORD*)pImage->GetData() )[i + j * wWidth];
        ( (WORD*)pImage->GetData() )[i + j * wWidth] = ( (WORD*)pImage->GetData() )[( wWidth - 1 - i ) + j * wWidth];
        ( (WORD*)pImage->GetData() )[( wWidth - 1 - i ) + j * wWidth] = wDummy;
      }
    }
  }
  else if ( pImage->GetDepth() == 24 )
  {
    for ( j = 0; j < (int)wHeight; j++ )
    {
      for ( i = 0; i < (int)wWidth / 2; i++ )
      {
        bDummy[0] = ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth];
        bDummy[1] = ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth + 1];
        bDummy[2] = ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth + 2];
        ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth]      = ( (BYTE*)pImage->GetData() )[3 * ( wWidth - 1 - i ) + 3 * j * wWidth];
        ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth + 1]  = ( (BYTE*)pImage->GetData() )[3 * ( wWidth - 1 - i ) + 3 * j * wWidth + 1];
        ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth + 2]  = ( (BYTE*)pImage->GetData() )[3 * ( wWidth - 1 - i ) + 3 * j * wWidth + 2];
        ( (BYTE*)pImage->GetData() )[3 * ( wWidth - 1 - i ) + 3 * j * wWidth] = bDummy[0];
        ( (BYTE*)pImage->GetData() )[3 * ( wWidth - 1 - i ) + 3 * j * wWidth + 1] = bDummy[1];
        ( (BYTE*)pImage->GetData() )[3 * ( wWidth - 1 - i ) + 3 * j * wWidth + 2] = bDummy[2];
      }
    }
  }
  else if ( pImage->GetDepth() == 32 )
  {
    for ( j = 0; j < (int)wHeight; j++ )
    {
      for ( i = 0; i < (int)wWidth / 2; i++ )
      {
        dwDummy = ( (DWORD*)pImage->GetData() )[i + j * wWidth];
        ( (DWORD*)pImage->GetData() )[i + j * wWidth] = ( (DWORD*)pImage->GetData() )[( wWidth - 1 - i ) + j * wWidth];
        ( (DWORD*)pImage->GetData() )[( wWidth - 1 - i ) + j * wWidth] = dwDummy;
      }
    }
  }

}



/*-VMirrorImage---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void VMirrorImage( GR::Graphic::Image *pImage )
{

  int           i,
                j;

  BYTE          bDummy[3];

  DWORD         dwDummy;

  WORD          wWidth,
                wHeight,
                wDummy;


  wWidth = pImage->GetWidth();
  wHeight = pImage->GetHeight();

  // TODO( "weniger als 8!" )
  if ( pImage->GetDepth() == 8 )
  {
    for ( j = 0; j < (int)wHeight / 2; j++ )
    {
      for ( i = 0; i < (int)wWidth; i++ )
      {
        bDummy[0] = ( (BYTE*)pImage->GetData() )[i + j * wWidth];
        ( (BYTE*)pImage->GetData() )[i + j * wWidth] = ( (BYTE*)pImage->GetData() )[i + ( wHeight - 1 - j ) * wWidth];
        ( (BYTE*)pImage->GetData() )[i + ( wHeight - 1 - j ) * wWidth] = bDummy[0];
      }
    }
  }
  else if ( ( pImage->GetDepth() == 15 )
  ||        ( pImage->GetDepth() == 16 ) )
  {
    for ( j = 0; j < (int)wHeight / 2; j++ )
    {
      for ( i = 0; i < (int)wWidth; i++ )
      {
        wDummy = ( (WORD*)pImage->GetData() )[i + j * wWidth];
        ( (WORD*)pImage->GetData() )[i + j * wWidth] = ( (WORD*)pImage->GetData() )[i + ( wHeight - 1 - j ) * wWidth];
        ( (WORD*)pImage->GetData() )[i + ( wHeight - 1 - j ) * wWidth] = wDummy;
      }
    }
  }
  else if ( pImage->GetDepth() == 24 )
  {
    for ( j = 0; j < (int)wHeight / 2; j++ )
    {
      for ( i = 0; i < (int)wWidth; i++ )
      {
        bDummy[0] = ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth];
        bDummy[1] = ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth + 1];
        bDummy[2] = ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth + 2];
        ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth]      = ( (BYTE*)pImage->GetData() )[3 * i + 3 * ( wHeight - 1 - j ) * wWidth];
        ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth + 1]  = ( (BYTE*)pImage->GetData() )[3 * i + 3 * ( wHeight - 1 - j ) * wWidth + 1];
        ( (BYTE*)pImage->GetData() )[3 * i + 3 * j * wWidth + 2]  = ( (BYTE*)pImage->GetData() )[3 * i + 3 * ( wHeight - 1 - j ) * wWidth + 2];
        ( (BYTE*)pImage->GetData() )[3 * i + 3 * ( wHeight - 1 - j ) * wWidth] = bDummy[0];
        ( (BYTE*)pImage->GetData() )[3 * i + 3 * ( wHeight - 1 - j ) * wWidth + 1] = bDummy[1];
        ( (BYTE*)pImage->GetData() )[3 * i + 3 * ( wHeight - 1 - j ) * wWidth + 2] = bDummy[2];
      }
    }
  }
  else if ( pImage->GetDepth() == 32 )
  {
    for ( j = 0; j < (int)wHeight / 2; j++ )
    {
      for ( i = 0; i < (int)wWidth; i++ )
      {
        dwDummy = ( (DWORD*)pImage->GetData() )[i + j * wWidth];
        ( (DWORD*)pImage->GetData() )[i + j * wWidth] = ( (DWORD*)pImage->GetData() )[i + ( wHeight - 1 - j ) * wWidth];
        ( (DWORD*)pImage->GetData() )[i + ( wHeight - 1 - j ) * wWidth] = dwDummy;
      }
    }
  }

}



GR::Graphic::Image* ConvertImage1to16( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette )
{
  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           15, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource, pPalette );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;
}



GR::Graphic::Image* ConvertImage4to16( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette )
{
  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           15, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource, pPalette );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;
}



/*-Convert8to16---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage8to16( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           15, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource, pPalette );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert1to24---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage1to24( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           24, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource, pPalette );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert4to24---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage4to24( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           24, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource, pPalette );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert8to24---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage8to24( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           24, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource, pPalette );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert1to32---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image* ConvertImage1to32( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           32, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource, pPalette );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert4to32---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage4to32( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           32, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource, pPalette );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert8to32---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage8to32( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           32, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource, pPalette );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert16to24--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage16to24( GR::Graphic::Image *pImageSource )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           24, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert16to32--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage16to32( GR::Graphic::Image *pImageSource )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           32, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert24to16--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage24to16( GR::Graphic::Image *pImageSource )
{

  int             i,
                  j;

  WORD            wWidth,
                  wHeight;

  GR::Graphic::Image       *pNewImage;

  unsigned char   ucPart[3];

  static BYTE     ucPattern[8][4][4] =
  { { { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 0, 0 } },
  { { 1, 0, 0, 0 },
  { 0, 0, 0, 0 },
  { 0, 0, 1, 0 },
  { 0, 0, 0, 0 } },
  { { 0, 0, 0, 0 },
  { 0, 1, 0, 1 },
  { 0, 0, 0, 0 },
  { 0, 1, 0, 1 } },
  { { 1, 0, 1, 0 },
  { 0, 0, 0, 1 },
  { 1, 0, 1, 0 },
  { 0, 1, 0, 0 } },
  { { 1, 0, 1, 0 },
  { 0, 1, 0, 1 },
  { 1, 0, 1, 0 },
  { 0, 1, 0, 1 } },
  { { 0, 1, 0, 1 },
  { 1, 1, 1, 0 },
  { 0, 1, 0, 1 },
  { 1, 0, 1, 1 } },
  { { 0, 1, 0, 1 },
  { 1, 1, 1, 1 },
  { 0, 1, 0, 1 },
  { 1, 1, 1, 1 } },
    { { 1, 1, 1, 1 },
  { 1, 0, 1, 1 },
  { 1, 1, 1, 1 },
  { 1, 1, 1, 0 } } };


  CConvertOptions   coDlg;

  coDlg.DoModal();


  wWidth = pImageSource->GetWidth();
  wHeight = pImageSource->GetHeight();
  pNewImage = new GR::Graphic::Image( wWidth, wHeight, 15, 0, 0 );
  int iR,iG,iB;
  for ( j = 0; j < (int)wHeight; j++ )
  {
    for ( i = 0; i < (int)wWidth; i++ )
    {
      ucPart[0] = ( (BYTE*)pImageSource->GetData() )[3 * i + j * 3 * wWidth];
      ucPart[1] = ( (BYTE*)pImageSource->GetData() )[3 * i + j * 3 * wWidth + 1];
      ucPart[2] = ( (BYTE*)pImageSource->GetData() )[3 * i + j * 3 * wWidth + 2];

      switch ( coDlg.m_dwConvertMode )
      {
        case CONVERT::TYPE::DITHER:
          // Dithering
          iR = ( ucPart[0] >> 3 ) + ucPattern[ucPart[0] & 0x07][i % 4][j % 4];
          iG = ( ucPart[1] >> 3 ) + ucPattern[ucPart[1] & 0x07][i % 4][j % 4];
          iB = ( ucPart[2] >> 3 ) + ucPattern[ucPart[2] & 0x07][i % 4][j % 4];
          if ( iR > 31 )
          {
            iR = 31;
          }
          if ( iG > 31 )
          {
            iG = 31;
          }
          if ( iB > 31 )
          {
            iB = 31;
          }
          ( (WORD*)pNewImage->GetData() )[i + j * wWidth] = 
            ( iR ) + 
            ( iG << 5 ) + 
            ( iB << 10 );
          break;
        case CONVERT::TYPE::STRAIGHT:
          ( (WORD*)pNewImage->GetData() )[i + j * wWidth] = 
            ( ( ucPart[0] >> 3 ) ) + 
            ( ( ucPart[1] >> 3 ) << 5 ) + 
            ( ( ucPart[2] >> 3 ) << 10 );
          break;
      }
    }
  }
  return pNewImage;

}



/*-Convert24to32--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage24to32( GR::Graphic::Image *pImageSource )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           32, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert32to16--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage32to16( GR::Graphic::Image *pImageSource )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           15, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



/*-Convert32to24--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ConvertImage32to24( GR::Graphic::Image *pImageSource )
{

  GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pImageSource->GetWidth(), 
                                           pImageSource->GetHeight(), 
                                           24, 0, 0 );

  GR::Graphic::ContextDescriptor  cdImage( pImageSource );
  
  if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
  {
    dh::Log( "Conversion failed\n" );
  }
  return pNewImage;

}



FIBITMAP* ConvertImageToFreeImageDIB( GR::Graphic::Image *pImage, GR::Graphic::Palette *pPalette )
{

  FIBITMAP        *dib = NULL;



  switch ( pImage->GetDepth() )
  {
    case 4:
      {
        dib = FreeImage_Allocate( pImage->GetWidth(), pImage->GetHeight(), 4 );

        for ( int i = 0; i < pImage->GetHeight(); i++ )
        {
          memcpy( FreeImage_GetScanLine( dib, pImage->GetHeight() - 1 - i ),
                  ( (BYTE*)pImage->GetData() ) + i * pImage->GetLineSize(),
                  pImage->GetLineSize() );
        }

        BITMAPINFOHEADER* pbmHeader = FreeImage_GetInfoHeader( dib );
        RGBQUAD* pRGB = FreeImage_GetPalette( dib );

        for ( int i = 0; i < 16; i++ )
        {
          if ( pPalette )
          {
            pRGB->rgbRed = ( pPalette->Red( i ) );
            pRGB->rgbGreen = ( pPalette->Green( i ) );
            pRGB->rgbBlue = ( pPalette->Blue( i ) );
          }
          else
          {
            pRGB->rgbRed = i;
            pRGB->rgbGreen = i;
            pRGB->rgbBlue = i;
          }
          pRGB++;
        }
      }
      break;
    case 8:
      {
        dib = FreeImage_Allocate( pImage->GetWidth(), pImage->GetHeight(), 8 );

        for ( int i = 0; i < pImage->GetHeight(); i++ )
        {
          memcpy( FreeImage_GetScanLine( dib, pImage->GetHeight() - 1 - i ),
                  ( (BYTE*)pImage->GetData() ) + i * pImage->GetWidth(),
                  pImage->GetWidth() );
        }

        BITMAPINFOHEADER    *pbmHeader = FreeImage_GetInfoHeader( dib );
        RGBQUAD             *pRGB = FreeImage_GetPalette( dib );

        for ( int i = 0; i < 256; i++ )
        {
          if ( pPalette )
          {
          pRGB->rgbRed    = ( pPalette->Red( i ) );
          pRGB->rgbGreen  = ( pPalette->Green( i ) );
          pRGB->rgbBlue   = ( pPalette->Blue( i ) );
          }
          else
          {
            pRGB->rgbRed    = i;
            pRGB->rgbGreen  = i;
            pRGB->rgbBlue   = i;
          }
          pRGB++;
        }
      }
      break;
    case 15:
    case 16:
      {
        dib = FreeImage_Allocate( pImage->GetWidth(), pImage->GetHeight(), 16 );

        for ( int i = 0; i < pImage->GetHeight(); i++ )
        {
          memcpy( FreeImage_GetScanLine( dib, pImage->GetHeight() - 1 - i ),
                  ( (BYTE*)pImage->GetData() ) + i * pImage->GetWidth() * 2,
                  pImage->GetWidth() * 2 );
        }
      }
      break;
    case 24:
      {
        dib = FreeImage_Allocate( pImage->GetWidth(), pImage->GetHeight(), 24 );

        for ( int i = 0; i < pImage->GetHeight(); i++ )
        {
          memcpy( FreeImage_GetScanLine( dib, pImage->GetHeight() - 1 - i ),
                  ( (BYTE*)pImage->GetData() ) + i * pImage->GetWidth() * 3,
                  pImage->GetWidth() * 3 );
        }
      }
      break;
    case 32:
      {
        dib = FreeImage_Allocate( pImage->GetWidth(), pImage->GetHeight(), 32 );

        for ( int i = 0; i < pImage->GetHeight(); i++ )
        {
          memcpy( FreeImage_GetScanLine( dib, pImage->GetHeight() - 1 - i ),
                  ( (BYTE*)pImage->GetData() ) + i * pImage->GetWidth() * 4,
                  pImage->GetWidth() * 4 );
        }
      }
      break;
  }

  return dib;

}



GR::Graphic::Image* MapImageToPalette( CPainterImagePackage* pPackSource, GR::Graphic::Palette* pPalette )
{

  CColorReducer                 Reducer;

  CPainterImagePackage*         pIPResult = NULL;


  Reducer.AddSourceToColorCube( pPalette );
  Reducer.Calculate();
  pIPResult = Reducer.Reduce( pPackSource );

  GR::Graphic::Image*                    pImageResult = pIPResult->m_pImage;

  pIPResult->m_pImage = NULL;
  SafeDelete( pIPResult );

  return pImageResult;

}



GR::Graphic::Image *GetImageFromBitmap( HBITMAP hBitmap )
{

  DIBSECTION    dibs;

  GR::Graphic::Image     *pImage;

  unsigned char ucBitDepth;

  DWORD         dwWidth,
                dwHeight;

  int           i,
                j,
                iLO;

  if ( !::GetObject( hBitmap, sizeof( dibs ), &dibs ) )
  {
    pSettings->Log( CMisc::printf( "Getobject failed %d", GetLastError() ) );
    return NULL;
  }

  BITMAP&  bm = dibs.dsBm;

  ucBitDepth = (unsigned char)bm.bmBitsPixel;
  if ( ucBitDepth < 8 )
  {
    // weniger als 256 Farben werden nicht geladen
    return NULL;
  }
  if ( bm.bmBits == 0 )
  {
    // keine Daten????????
    pSettings->Log( "GetImageFromBitmap - bmBits = 0" );
    return NULL;
  }
  dwWidth = bm.bmWidth;
  dwHeight = abs( bm.bmHeight );

  pImage = NULL;
  pImage = new GR::Graphic::Image( (WORD)dwWidth, (WORD)dwHeight, ucBitDepth, 0, 0 );

  // TODO( "weniger als 8!" )
  if ( ucBitDepth == 8 )
  {
    iLO = dwWidth;
    if ( iLO % 4 )
    {
      iLO += ( 4 - iLO % 4 );
    }
    for ( j = 0; j < (int)dwHeight; j++ )
    {
      for ( i = 0; i < (int)dwWidth; i++ )
      {
        if ( bm.bmHeight < 0 )
        {
          ( (BYTE*)pImage->GetData() )[i + j * dwWidth] = ((unsigned char *)bm.bmBits)[i + j * iLO];
        }
        else
        {
          ( (BYTE*)pImage->GetData() )[i + ( abs( bm.bmHeight ) - j - 1 ) * bm.bmWidth] = ((unsigned char *)bm.bmBits)[i + j * iLO];
        }
      }
    }
  }
  else if ( ucBitDepth == 16 )
  {
    iLO = dwWidth * 2;
    if ( iLO % 4 )
    {
      iLO += ( 4 - iLO % 4 );
    }
    WORD    wDummy;

    for ( j = 0; j < abs( bm.bmHeight ); j++ )
    {
      for ( i = 0; i < (int)dwWidth; i++ )
      {

        wDummy = (WORD)((unsigned char *)bm.bmBits)[i * 2 + j * iLO];

        if ( bm.bmHeight < 0 )
        {
          (WORD)*( (WORD*)pImage->GetData() + i + j * dwWidth ) = ((unsigned char *)bm.bmBits)[i + j * iLO / 2];
        }
        else
        {
          (WORD)*( (WORD*)pImage->GetData() + i + ( abs( bm.bmHeight ) - j - 1 ) * dwWidth ) = ((unsigned char *)bm.bmBits)[i + j * iLO / 2];
        }
      }
    }
  }
  else if ( ucBitDepth == 24 )
  {
    // ein 24-bit-DIB
    DWORD     dwDummy;

    iLO = dwWidth * 3;
    if ( iLO % 4 )
    {
      iLO += ( 4 - iLO % 4 );
    }
    for ( j = 0; j < abs( bm.bmHeight ); j++ )
    {
      for ( i = 0; i < (int)dwWidth; i++ )
      {
        dwDummy = ((unsigned char *)bm.bmBits + i * 3 + j * iLO )[0] + 
                  ( ((unsigned char *)bm.bmBits + i * 3 + j * iLO )[1] << 8 ) +
                  ( ((unsigned char *)bm.bmBits + i * 3 + j * iLO )[2] << 16 );
        if ( bm.bmHeight < 0 )
        {
          (BYTE)*( (BYTE*)pImage->GetData() + i * 3     + 3 * j * dwWidth ) = 
                          (BYTE)( dwDummy & 0xff );
          (BYTE)*( (BYTE*)pImage->GetData() + i * 3 + 1 + 3 * j * dwWidth ) = 
                          (BYTE)( ( dwDummy & 0xff00 ) >> 8 );
          (BYTE)*( (BYTE*)pImage->GetData() + i * 3 + 2 + 3 * j * dwWidth ) = 
                          (BYTE)( ( dwDummy & 0xff0000 ) >> 16 );
        }
        else
        {
          (BYTE)*( (BYTE*)pImage->GetData() + 3 * i + 3 * ( abs( bm.bmHeight ) - j - 1 ) * dwWidth ) = 
                          (BYTE)( dwDummy & 0xff );
          (BYTE)*( (BYTE*)pImage->GetData() + 3 * i + 3 * ( abs( bm.bmHeight ) - j - 1 ) * dwWidth + 1 ) = 
                          (BYTE)( ( dwDummy & 0xff00 ) >> 8 );
          (BYTE)*( (BYTE*)pImage->GetData() + 3 * i + 3 * ( abs( bm.bmHeight ) - j - 1 ) * dwWidth + 2 ) = 
                          (BYTE)( ( dwDummy & 0xff0000 ) >> 16 );
        }
      }
    }
  }
  else if ( ucBitDepth == 32 )
  {
    // gibt es nicht
  }
  DeleteObject( hBitmap );

  return pImage;

}



/*-CreateImageFromHDIB--------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

 typedef BYTE * LPHUGEBYTE;
typedef BITMAPINFOHEADER *LPHUGEBITMAPINFOHEADER;  
typedef BITMAPCOREHEADER *LPHUGEBITMAPCOREHEADER;  
typedef BITMAPCOREINFO *LPHUGEBITMAPCOREINFO;
typedef BITMAPINFO *LPHUGEBITMAPINFO;
#define HDIB HANDLE
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))  
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)


WORD DIBNumColors(LPHUGEBYTE lpDIB)
{
   WORD wBitCount;  /* DIB bit count */

   /*  If this is a Windows-style DIB, the number of colors in the
    *  color table can be less than the number of bits per pixel
    *  allows for (i.e. lpbi->biClrUsed can be set to some value).
    *  If this is the case, return the appropriate value.
    */

   if (IS_WIN30_DIB(lpDIB))
   {
      DWORD dwClrUsed;

      dwClrUsed = ((LPBITMAPINFOHEADER)lpDIB)->biClrUsed;
      if (dwClrUsed)
     return (WORD)dwClrUsed;
   }

   /*  Calculate the number of colors in the color table based on
    *  the number of bits per pixel for the DIB.
    */
   if (IS_WIN30_DIB(lpDIB))
      wBitCount = ((LPBITMAPINFOHEADER)lpDIB)->biBitCount;
   else
      wBitCount = ((LPBITMAPCOREHEADER)lpDIB)->bcBitCount;

   /* return number of colors based on bits per pixel */
   switch (wBitCount)
      {
   case 1:
      return 2;

   case 4:
      return 16;

   case 8:
      return 256;

   default:
      return 0;
      }
}
WORD PaletteSize(BYTE* lpDIB)
{ 
   WORD	size; 
   
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpDIB)) {
      size = (DIBNumColors(lpDIB) * sizeof(RGBQUAD));
   } else {
      size = (DIBNumColors(lpDIB) * sizeof(RGBTRIPLE)); 
   }

   return size;
}


CPainterImagePackage *CreateImagePackFromHDIB( HGLOBAL hmem )
{

  if ( hmem == NULL )
  {
    return NULL;
  }

  BITMAPINFOHEADER *pBIH;

  pBIH = (BITMAPINFOHEADER*)GlobalLock(hmem);

  BYTE    *pData = ( (BYTE*)pBIH ) + pBIH->biSize + PaletteSize( (BYTE*)pBIH );

  DWORD   dwMask[3];

  memset( dwMask, 0, sizeof( dwMask ) );

  if ( pBIH->biCompression == BI_BITFIELDS )
  {
    // dann sind noch 3 DWORDs (die Masken) mit drin
    dwMask[0] = ( (DWORD*)pData )[0];
    dwMask[1] = ( (DWORD*)pData )[1];
    dwMask[2] = ( (DWORD*)pData )[2];
    pData += 12;

    /*
    dh::Log( "Masks %x  %x  %x\n",
        dwMask[0], dwMask[1], dwMask[2] );
    */
  }


  CPainterImagePackage   *pPack = new CPainterImagePackage();

  switch ( pBIH->biBitCount )
  {
    case 1:
      {
        int             i,
                        j,
                        iLO;



        iLO = pBIH->biWidth / 8;
        if ( pBIH->biWidth & 7 )
        {
          iLO++;
        }
        if ( iLO % 4 )
        {
          iLO += ( 4 - iLO % 4 );
        }

        int   iWidth = pBIH->biWidth,
              iHeight = pBIH->biHeight;

        pPack->m_pImage = new GR::Graphic::Image( (WORD)iWidth, (WORD)iHeight, 1, 0, 0 );
        for ( j = 0; j < abs( iHeight ); j++ )
        {
          for ( i = 0; i < ( iWidth + 7 ) / 8; i++ )
          {
            if ( iHeight < 0 )
            {
              ( (BYTE*)pPack->m_pImage->GetData() )[i + j * pPack->m_pImage->GetLineSize()] = pData[i + j * iLO];
            }
            else
            {
              ( (BYTE*)pPack->m_pImage->GetData() )[i + ( abs( iHeight ) - j - 1 ) * pPack->m_pImage->GetLineSize()] = pData[i + j * iLO];
            }
          }
        }

        // Palette holen
        {
          pPack->m_Palette.Create( 2 );
          RGBQUAD                 *bmiColor;

          bmiColor = (RGBQUAD*)( (BYTE*)pBIH + pBIH->biSize );

          for ( int i = 0; i < 2; i++ )
          {
            pPack->m_Palette.SetColor( i, bmiColor[i].rgbRed, bmiColor[i].rgbGreen, bmiColor[i].rgbBlue );
          }
        }
      }
      break;
    case 4:
      {
        int             i,
                        j,
                        iLO;



        iLO = pBIH->biWidth / 2;
        if ( pBIH->biWidth & 1 )
        {
          iLO++;
        }
        if ( iLO % 4 )
        {
          iLO += ( 4 - iLO % 4 );
        }

        int   iWidth = pBIH->biWidth,
              iHeight = pBIH->biHeight;

        pPack->m_pImage = new GR::Graphic::Image( (WORD)iWidth, (WORD)iHeight, 4, 0, 0 );
        for ( j = 0; j < abs( iHeight ); j++ )
        {
          for ( i = 0; i < ( iWidth + 1 ) / 2; i++ )
          {
            if ( iHeight < 0 )
            {
              ( (BYTE*)pPack->m_pImage->GetData() )[i + j * pPack->m_pImage->GetLineSize()] = pData[i + j * iLO];
            }
            else
            {
              ( (BYTE*)pPack->m_pImage->GetData() )[i + ( abs( iHeight ) - j - 1 ) * pPack->m_pImage->GetLineSize()] = pData[i + j * iLO];
            }
          }
        }

        // Palette holen
        {
          pPack->m_Palette.Create( 16 );
          RGBQUAD                 *bmiColor;

          bmiColor = (RGBQUAD*)( (BYTE*)pBIH + pBIH->biSize );

          for ( int i = 0; i < 16; i++ )
          {
            pPack->m_Palette.SetColor( i, bmiColor[i].rgbRed, bmiColor[i].rgbGreen, bmiColor[i].rgbBlue );
          }
        }
      }
      break;
    case 8:
      {
        int             i,
                        j,
                        iDummy,
                        iLO;



        iLO = pBIH->biWidth;
        if ( iLO % 4 )
        {
          iLO += ( 4 - iLO % 4 );
        }

        iDummy = 256;

        int   iWidth = pBIH->biWidth,
              iHeight = pBIH->biHeight;

        pPack->m_pImage = new GR::Graphic::Image( (WORD)iWidth, (WORD)iHeight, 8, 0, 0 );
        for ( j = 0; j < abs( iHeight ); j++ )
        {
          for ( i = 0; i < iWidth; i++ )
          {
            if ( iHeight < 0 )
            {
              ( (BYTE*)pPack->m_pImage->GetData() )[i + j * iWidth] = pData[i + j * iLO];
            }
            else
            {
              ( (BYTE*)pPack->m_pImage->GetData() )[i + ( abs( iHeight ) - j - 1 ) * iWidth] = pData[i + j * iLO];
            }
          }
        }

        // Palette holen
        {
          pPack->m_Palette.Create();
          RGBQUAD                 *bmiColor;

          bmiColor = (RGBQUAD*)( (BYTE*)pBIH + pBIH->biSize );

          for ( int i = 0; i < 256; i++ )
          {
            pPack->m_Palette.SetColor( i, bmiColor[i].rgbRed, bmiColor[i].rgbGreen, bmiColor[i].rgbBlue );
          }
        }
      }
      break;
    case 16:
      {
        int             i,
                        j,
                        iLO;



        iLO = pBIH->biWidth * 2;
        if ( iLO % 4 )
        {
          iLO += ( 4 - iLO % 4 );
        }

        int   iWidth = pBIH->biWidth,
              iHeight = pBIH->biHeight;

        pPack->m_pImage = new GR::Graphic::Image( (WORD)iWidth, (WORD)abs( iHeight ), 15, 0, 0 );

        WORD    wDummy;

        for ( j = 0; j < abs( iHeight ); j++ )
        {
          for ( i = 0; i < iWidth; i++ )
          {
            wDummy = ( (WORD*)pData )[i + j * iLO / 2];

            if ( dwMask[0] == 0xf800 )
            {
              // von 16 auf 15 bit runter
              wDummy = ( ( wDummy & 0xf800 ) >> 1 ) + ( ( wDummy & 0x7c0 ) >> 1 ) + ( wDummy & 0x1f );
            }

            if ( iHeight < 0 )
            {
              (WORD)*( (WORD*)pPack->m_pImage->GetData() + i + j * iWidth ) = wDummy;
            }
            else
            {
              (WORD)*( (WORD*)pPack->m_pImage->GetData() + i + abs( iHeight - j - 1 ) * iWidth ) = wDummy;
            }
          }
        }
      }
      break;
    case 24:
      {
        int             i,
                        j,
                        iLO;



        iLO = pBIH->biWidth * 3;
        if ( iLO % 4 )
        {
          iLO += ( 4 - iLO % 4 );
        }

        int   iWidth = pBIH->biWidth,
              iHeight = pBIH->biHeight;

        pPack->m_pImage = new GR::Graphic::Image( (WORD)iWidth, (WORD)abs( iHeight ), 24, 0, 0 );

        for ( j = 0; j < abs( iHeight ); j++ )
        {
          for ( i = 0; i < iWidth; i++ )
          {
            if ( iHeight < 0 )
            {
              ( (BYTE*)pPack->m_pImage->GetData() )[( i + j * iWidth ) * 3 + 0] = pData[i * 3 + j * iLO + 0];
              ( (BYTE*)pPack->m_pImage->GetData() )[( i + j * iWidth ) * 3 + 1] = pData[i * 3 + j * iLO + 1];
              ( (BYTE*)pPack->m_pImage->GetData() )[( i + j * iWidth ) * 3 + 2] = pData[i * 3 + j * iLO + 2];
            }
            else
            {
              ( (BYTE*)pPack->m_pImage->GetData() )[( i + abs( iHeight - j - 1 ) * iWidth ) * 3 + 0] = pData[i * 3 + j * iLO + 0];
              ( (BYTE*)pPack->m_pImage->GetData() )[( i + abs( iHeight - j - 1 ) * iWidth ) * 3 + 1] = pData[i * 3 + j * iLO + 1];
              ( (BYTE*)pPack->m_pImage->GetData() )[( i + abs( iHeight - j - 1 ) * iWidth ) * 3 + 2] = pData[i * 3 + j * iLO + 2];
            }
          }
        }
      }
      break;
    case 32:
      {
        int             i,
                        j,
                        iLO;



        iLO = pBIH->biWidth * 4;

        int   iWidth = pBIH->biWidth,
              iHeight = pBIH->biHeight;

        pPack->m_pImage = new GR::Graphic::Image( (WORD)iWidth, (WORD)abs( iHeight ), 32, 0, 0 );
        
        DWORD   dwDummy;

        for ( j = 0; j < abs( iHeight ); j++ )
        {
          for ( i = 0; i < iWidth; i++ )
          {
            dwDummy = ( (DWORD*)pData )[i + j * iLO / 4];
            if ( iHeight < 0 )
            {
              ( (DWORD*)pPack->m_pImage->GetData() )[i + j * iWidth] = dwDummy;
            }
            else
            {
              ( (DWORD*)pPack->m_pImage->GetData() )[i + abs( iHeight - j - 1 ) * iWidth] = dwDummy;
            }
          }
        }
      }
      break;
    default:
      // nicht unterstützte Farbtiefe
      {
        TRACE( "HDIB unsupported %d\n", pBIH->biBitCount );
        SafeDelete( pPack );
      }
      break;
  }
  return pPack;

}




fi_handle g_load_address;

inline unsigned _stdcall
_ReadProc( void* buffer, unsigned size, unsigned count, fi_handle handle )
{
  BYTE* tmp = (BYTE*)buffer;

  for ( unsigned c = 0; c < count; c++ )
  {
    memcpy( tmp, g_load_address, size );

    g_load_address = (BYTE*)g_load_address + size;

    tmp += size;
  }

  return count;
}

inline unsigned _stdcall
_WriteProc( void* buffer, unsigned size, unsigned count, fi_handle handle )
{
// there's not much use for saving the bitmap into memory now, is there?

  return size;
}

inline int _stdcall
_SeekProc( fi_handle handle, long offset, int origin )
{
  if ( origin == SEEK_SET )
  {
    g_load_address = (BYTE*)handle + offset;
  }
  else
  {
    g_load_address = (BYTE*)g_load_address + offset;
  }

  return 0;
}

inline long _stdcall
_TellProc( fi_handle handle )
{
  return ( (int)g_load_address - (int)handle );
}



CPainterImagePackage* CreateImagePackFromHTMLTag( const GR::String& Tag )
{
  if ( !Tag.StartsWith( "<img " ) )
  {
    return NULL;
  }
  size_t  pos = Tag.find( "src=\"" );
  if ( pos == GR::String::npos )
  {
    return NULL;
  }
  size_t  pos2 = Tag.find( "\"", pos + 5 );
  if ( pos2 == GR::String::npos )
  {
    return NULL;
  }

  GR::String    imageContent = Tag.substr( pos + 5, pos2 - pos - 5 );
  if ( !imageContent.StartsWith( "data:image/png;base64," ) )
  {
    return NULL;
  }
  GR::String    base64Content = imageContent.substr( 22 );

  ByteBuffer    data = Base64::Decode( ByteBuffer( base64Content.c_str(), base64Content.length() ) );

  FreeImageIO io;

  io.read_proc  = _ReadProc;
  io.write_proc = _WriteProc;
  io.tell_proc  = _TellProc;
  io.seek_proc  = _SeekProc;

  g_load_address = data.DataAt( 0 );
  FIBITMAP* dib = FreeImage_LoadFromHandle( FIF_PNG, &io, (fi_handle)data.DataAt( 0 ) );

  if ( dib == NULL )
  {
    return NULL;
  }

  CPainterImagePackage* pImagePackage = new CPainterImagePackage();
  switch ( FreeImage_GetBPP( dib ) )
  {
    case 4:
      {
        FIBITMAP* newdib = FreeImage_ConvertTo8Bits( dib );
        FreeImage_Unload( dib );
        dib = newdib;

        pImagePackage->m_Palette = theApp.m_PainterPalette;
        pImagePackage->m_pImage = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 8 );
        for ( DWORD i = 0; i < FreeImage_GetHeight( dib ); i++ )
        {
          memcpy( ( (BYTE*)pImagePackage->m_pImage->GetData() ) + i * pImagePackage->m_pImage->GetWidth(),
            FreeImage_GetScanLine( dib, i ),
            pImagePackage->m_pImage->GetWidth() );
        }
        BITMAPINFOHEADER* pbmHeader = FreeImage_GetInfoHeader( dib );
        RGBQUAD* pRGB = FreeImage_GetPalette( dib );
        for ( int i = 0; i < (int)pbmHeader->biClrUsed; i++ )
        {
          pImagePackage->m_Palette.SetColor( i, pRGB->rgbRed, pRGB->rgbGreen, pRGB->rgbBlue );
          pRGB++;
        }
      }
      break;
    case 8:
      {
        pImagePackage->m_Palette.Create();
        pImagePackage->m_pImage = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 8 );
        for ( DWORD i = 0; i < FreeImage_GetHeight( dib ); i++ )
        {
          memcpy( ( (BYTE*)pImagePackage->m_pImage->GetData() ) + i * pImagePackage->m_pImage->GetWidth(),
            FreeImage_GetScanLine( dib, i ),
            pImagePackage->m_pImage->GetWidth() );
        }
        BITMAPINFOHEADER* pbmHeader = FreeImage_GetInfoHeader( dib );
        RGBQUAD* pRGB = FreeImage_GetPalette( dib );
        for ( int i = 0; i < (int)pbmHeader->biClrUsed; i++ )
        {
          pImagePackage->m_Palette.SetColor( i, pRGB->rgbRed, pRGB->rgbGreen, pRGB->rgbBlue );
          pRGB++;
        }
      }
      break;
    case 24:
      {
        pImagePackage->m_pImage = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 24 );
        for ( DWORD i = 0; i < FreeImage_GetHeight( dib ); i++ )
        {
          memcpy( ( (BYTE*)pImagePackage->m_pImage->GetData() ) + i * pImagePackage->m_pImage->GetWidth() * 3,
            FreeImage_GetScanLine( dib, i ),
            pImagePackage->m_pImage->GetWidth() * 3 );
        }
      }
      break;
    case 32:
      {
        pImagePackage->m_pImage = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 32 );
        for ( DWORD i = 0; i < FreeImage_GetHeight( dib ); i++ )
        {
          memcpy( ( (BYTE*)pImagePackage->m_pImage->GetData() ) + i * pImagePackage->m_pImage->GetWidth() * 4,
            FreeImage_GetScanLine( dib, i ),
            pImagePackage->m_pImage->GetWidth() * 4 );
        }

        if ( FreeImage_GetColorType( dib ) == FIC_RGBALPHA )
        {
          // in den oberen 8 Bits wird vermutlich Alpha sein
          // Alpha in Maske umwandeln
          pImagePackage->m_pImageMask = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 8 );

          for ( unsigned j = 0; j < FreeImage_GetHeight( dib ); j++ )
          {
            DWORD* pSource = (DWORD*)FreeImage_GetScanLine( dib, j );
            for ( unsigned i = 0; i < FreeImage_GetWidth( dib ); i++ )
            {
              *( ( (BYTE*)pImagePackage->m_pImageMask->GetData() ) + i + j * FreeImage_GetWidth( dib ) ) = (BYTE)( ( *pSource++ & 0xff000000 ) >> 24 );
            }
          }
        }
      }
      break;
    default:
      dh::Log( "unsupported depth %d", FreeImage_GetBPP( dib ) );
      break;
  }

  if ( pImagePackage->m_pImage != NULL )
  {
    VMirrorImage( pImagePackage->m_pImage );
  }
  if ( pImagePackage->m_pImageMask != NULL )
  {
    VMirrorImage( pImagePackage->m_pImageMask );
  }

    // don't forget to free the dib !
  FreeImage_Unload( dib );

  return pImagePackage;
}



HANDLE BitmapToHDIB( HBITMAP hBitmap, HPALETTE hPal )
{
   BITMAP bm;                   /* bitmap structure */
   BITMAPINFOHEADER bi;         /* bitmap header */
   LPHUGEBITMAPINFOHEADER  lpbi; /* pointer to BITMAPINFOHEADER */
   DWORD dwLen;                 /* size of memory block */
   HANDLE hDIB, h;              /* handle to DIB, temp handle */
   HDC hDC;                     /* handle to DC */
   WORD biBits;                 /* bits per pixel */

   /* check if bitmap handle is valid */

   if (!hBitmap)
      return NULL;

   /* fill in BITMAP structure, return NULL if it didn't work */
   if (!GetObject(hBitmap, sizeof(bm), (LPSTR)&bm))
      return NULL;

   /* if no palette is specified, use default palette */
   if (hPal == NULL)
      hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

   /* calculate bits per pixel */
   biBits = (WORD)(bm.bmPlanes * bm.bmBitsPixel);

   /* make sure bits per pixel is valid */
   if (biBits <= 1)
      biBits = 1;
   else if (biBits <= 4)
      biBits = 4;
   else if (biBits <= 8)
      biBits = 8;
   else /* if greater than 8-bit, force to 24-bit */
      biBits = 24;

   /* initialize BITMAPINFOHEADER */
   bi.biSize = sizeof(BITMAPINFOHEADER);
   bi.biWidth = bm.bmWidth;
   bi.biHeight = bm.bmHeight;
   bi.biPlanes = 1;
   bi.biBitCount = biBits;
   bi.biCompression = BI_RGB;
   bi.biSizeImage = 0;
   bi.biXPelsPerMeter = 0;
   bi.biYPelsPerMeter = 0;
   bi.biClrUsed = 0;
   bi.biClrImportant = 0;

   /* calculate size of memory block required to store BITMAPINFO */
   dwLen = bi.biSize + PaletteSize((LPHUGEBYTE)&bi);

   /* get a DC */
   hDC = GetDC(NULL);

   /* select and realize our palette */
   hPal = SelectPalette(hDC, hPal, FALSE);
   RealizePalette(hDC);

   /* alloc memory block to store our bitmap */
   hDIB = GlobalAlloc(GHND, dwLen);

   /* if we couldn't get memory block */
   if (!hDIB)
   {
      /* clean up and return NULL */
      SelectPalette(hDC, hPal, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
      return NULL;
   }

   /* lock memory and get pointer to it */
   //lpbi = (VOID FAR *)GlobalLock(hDIB);
   lpbi = (LPHUGEBITMAPINFOHEADER)GlobalLock(hDIB);

   /* use our bitmap info. to fill BITMAPINFOHEADER */
   *lpbi = bi;

   /*  call GetDIBits with a NULL lpBits param, so it will calculate the
    *  biSizeImage field for us
    */
   GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, NULL, (LPHUGEBITMAPINFO)lpbi,
         DIB_RGB_COLORS);

   /* get the info. returned by GetDIBits and unlock memory block */
   bi = *lpbi;
   GlobalUnlock(hDIB);

   /* if the driver did not fill in the biSizeImage field, make one up */
   if (bi.biSizeImage == 0)
      bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;

   /* realloc the buffer big enough to hold all the bits */
   dwLen = bi.biSize + PaletteSize((LPHUGEBYTE)&bi) + bi.biSizeImage;  

   h = GlobalReAlloc(hDIB, dwLen, 0);
   if (h)
      hDIB = h;
   else
   {
      /* clean up and return NULL */
      GlobalFree(hDIB);
      hDIB = NULL;
      SelectPalette(hDC, hPal, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
      return NULL;
   }

   /* lock memory block and get pointer to it */  

   lpbi = (LPHUGEBITMAPINFOHEADER)GlobalLock(hDIB);

   /*  call GetDIBits with a NON-NULL lpBits param, and actualy get the
    *  bits this time
    */
   if (GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, (LPHUGEBYTE)lpbi + (WORD)lpbi
         ->biSize + PaletteSize((LPHUGEBYTE)lpbi), (LPHUGEBITMAPINFO)lpbi,
         DIB_RGB_COLORS) == 0)
   {
      /* clean up and return NULL */
      GlobalUnlock(hDIB);
      GlobalFree(hDIB);
      hDIB = NULL;
      SelectPalette(hDC, hPal, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
      return NULL;
   }
   bi = *lpbi;

   /* clean up */
   GlobalUnlock(hDIB);
   SelectPalette(hDC, hPal, TRUE);
   RealizePalette(hDC);
   ReleaseDC(NULL, hDC);

   /* return handle to the DIB */
   return hDIB;
}



HANDLE CreateHDIBFromImage( GR::Graphic::Image *pImage, GR::Graphic::Palette *pPal )
{

  //BITMAP bm;                   /* bitmap structure */
  BITMAPINFOHEADER bi;         /* bitmap header */
  LPHUGEBITMAPINFOHEADER  lpbi; /* pointer to BITMAPINFOHEADER */
  DWORD dwLen;                 /* size of memory block */
  HANDLE hDIB, h;              /* handle to DIB, temp handle */

 /* check if bitmap handle is valid */
  if ( !pImage )
  {
    return NULL;
  }

  if ( ( pImage->GetDepth() != 1 )
  &&   ( pImage->GetDepth() != 2 )
  &&   ( pImage->GetDepth() != 4 )
  &&   ( pImage->GetDepth() != 8 )
  &&   ( pImage->GetDepth() != 15 )
  &&   ( pImage->GetDepth() != 16 )
  &&   ( pImage->GetDepth() != 24 )
  &&   ( pImage->GetDepth() != 32 ) )
  {
    // nicht unterstützte Farbtiefe
    return NULL;
  }

  /* initialize BITMAPINFOHEADER */
  bi.biSize = sizeof( BITMAPINFOHEADER );
  bi.biWidth    = pImage->GetWidth();
  bi.biHeight   = pImage->GetHeight();
  bi.biPlanes   = 1;
  bi.biBitCount = pImage->GetDepth();
  if ( bi.biBitCount == 15 )
  {
    bi.biBitCount = 16;
  }
  bi.biCompression = BI_RGB;
  bi.biSizeImage = WIDTHBYTES( (DWORD)bi.biWidth * bi.biBitCount ) * bi.biHeight;
  bi.biXPelsPerMeter = 0;
  bi.biYPelsPerMeter = 0;
  bi.biClrUsed = 0;
  bi.biClrImportant = 0;

  /* calculate size of memory block required to store BITMAPINFO */
  dwLen = bi.biSize + PaletteSize( (LPHUGEBYTE)&bi );

  /* alloc memory block to store our bitmap */
  hDIB = GlobalAlloc(GHND, dwLen);

  /* if we couldn't get memory block */
  if (!hDIB)
  {
    // uh oh
    return NULL;
  }

  /* realloc the buffer big enough to hold all the bits */
  dwLen = bi.biSize + PaletteSize((LPHUGEBYTE)&bi) + bi.biSizeImage;  

  h = GlobalReAlloc(hDIB, dwLen, 0);
  if (h)
    hDIB = h;
  else
  {
    /* clean up and return NULL */
    GlobalFree(hDIB);
    hDIB = NULL;
    return NULL;
  }

  /* lock memory block and get pointer to it */  

  lpbi = (LPHUGEBITMAPINFOHEADER)GlobalLock(hDIB);

  // Daten in den Puffer kopieren
  *lpbi = bi;

  // Bild-Daten kopieren
  switch ( bi.biBitCount )
  {
    case 1:
      {
        // Palette in DC setzen
        if ( pPal != NULL )
        {
          RGBQUAD                 *bmiColor;

          bmiColor = (RGBQUAD*)( (BYTE*)lpbi + lpbi->biSize );

          for ( int i = 0; i < 2; i++ )
          {
            bmiColor[i].rgbRed    = pPal->Red( i );
            bmiColor[i].rgbGreen  = pPal->Green( i );
            bmiColor[i].rgbBlue   = pPal->Blue( i );
            bmiColor[i].rgbReserved = 0;
          }
        }

        BYTE    *pData = (BYTE*)lpbi + lpbi->biSize + PaletteSize( (BYTE*)lpbi );

        int iLO = pImage->GetWidth() / 8;
        if ( pImage->GetWidth() & 7 )
        {
          iLO++;
        }
        if ( iLO % 4 )
        {
          iLO += 4 - ( iLO % 4 );
        }
        GR::Graphic::ContextDescriptor    cdImage( pImage );
        GR::Graphic::ContextDescriptor    cdTarget;

        cdTarget.Attach( cdImage.Width(), cdImage.Height(), iLO, cdImage.ImageFormat(), pData );

        for ( int j = 0; j < pImage->GetHeight(); j++ )
        {
          //cdTarget.HLine( 0, cdTarget.Width() - 1, j, 1 );
          //cdTarget.HLine( 1, cdTarget.Width() - 2, j, 0 );
          cdImage.CopyLine( 0, j, cdImage.Width(), 0, cdImage.Height() - j - 1, &cdTarget );
        }
      }
      break;
    case 4:
      {
        // Palette in DC setzen
        if ( pPal != NULL )
        {
          RGBQUAD                 *bmiColor;

          bmiColor = (RGBQUAD*)( (BYTE*)lpbi + lpbi->biSize );

          for ( int i = 0; i < 16; i++ )
          {
            bmiColor[i].rgbRed    = pPal->Red( i );
            bmiColor[i].rgbGreen  = pPal->Green( i );
            bmiColor[i].rgbBlue   = pPal->Blue( i );
            bmiColor[i].rgbReserved = 0;
          }
        }

        BYTE    *pData = (BYTE*)lpbi + lpbi->biSize + PaletteSize( (BYTE*)lpbi );

        int iLO = pImage->GetWidth() / 2;
        if ( pImage->GetWidth() & 1 )
        {
          iLO++;
        }
        if ( iLO % 4 )
        {
          iLO += 4 - ( iLO % 4 );
        }
        GR::Graphic::ContextDescriptor    cdImage( pImage );
        GR::Graphic::ContextDescriptor    cdTarget;

        cdTarget.Attach( cdImage.Width(), cdImage.Height(), iLO, cdImage.ImageFormat(), pData );

        for ( int j = 0; j < pImage->GetHeight(); j++ )
        {
          cdImage.CopyLine( 0, j, cdImage.Width(), 0, cdImage.Height() - j - 1, &cdTarget );
        }
      }
      break;
    case 8:
      {
        // Palette in DC setzen
        if ( pPal != NULL )
        {
          RGBQUAD                 *bmiColor;

          bmiColor = (RGBQUAD*)( (BYTE*)lpbi + lpbi->biSize );

          for ( int i = 0; i < 256; i++ )
          {
            bmiColor[i].rgbRed    = pPal->Red( i );
            bmiColor[i].rgbGreen  = pPal->Green( i );
            bmiColor[i].rgbBlue   = pPal->Blue( i );
            bmiColor[i].rgbReserved = 0;
          }
        }

        BYTE    *pData = (BYTE*)lpbi + lpbi->biSize + PaletteSize( (BYTE*)lpbi );

        int iLO = pImage->GetWidth();
        if ( iLO % 4 )
        {
          iLO += 4 - ( iLO % 4 );
        }
        for ( int j = 0; j < pImage->GetHeight(); j++ )
        {
          for ( int i = 0; i < pImage->GetWidth(); i++ )
          {
            ((unsigned char*)pData)[i + ( pImage->GetHeight() - j - 1 ) * iLO] = (BYTE)( (BYTE*)pImage->GetData() )[i + j * pImage->GetWidth()];
          }
        }
      }
      break;
    case 16:
      {
        BYTE    *pData = (BYTE*)lpbi + lpbi->biSize + PaletteSize( (BYTE*)lpbi );

        int iLO = pImage->GetWidth() * 2;
        if ( iLO % 4 )
        {
          iLO += 4 - ( iLO % 4 );
        }
        for ( int j = 0; j < pImage->GetHeight(); j++ )
        {
          for ( int i = 0; i < pImage->GetWidth(); i++ )
          {
            ((WORD*)pData)[i + ( pImage->GetHeight() - j - 1 ) * iLO / 2] = (WORD)*( (WORD*)pImage->GetData() + i + j * pImage->GetWidth() );
          }
        }
      }
      break;
    case 24:
      {
        BYTE    *pData = (BYTE*)lpbi + lpbi->biSize + PaletteSize( (BYTE*)lpbi );

        int iLO = pImage->GetWidth() * 3;
        if ( iLO % 4 )
        {
          iLO += 4 - ( iLO % 4 );
        }
        for ( int j = 0; j < pImage->GetHeight(); j++ )
        {
          for ( int i = 0; i < pImage->GetWidth(); i++ )
          {
            ((BYTE*)pData)[3 * i + ( pImage->GetHeight() - j - 1 ) * iLO] = (BYTE)*( (BYTE*)pImage->GetData() + 3 * ( i + j * pImage->GetWidth() ) );
            ((BYTE*)pData)[3 * i + ( pImage->GetHeight() - j - 1 ) * iLO + 1] = (BYTE)*( (BYTE*)pImage->GetData() + 3 * ( i + j * pImage->GetWidth() ) + 1 );
            ((BYTE*)pData)[3 * i + ( pImage->GetHeight() - j - 1 ) * iLO + 2] = (BYTE)*( (BYTE*)pImage->GetData() + 3 * ( i + j * pImage->GetWidth() ) + 2 );
          }
        }
      }
      break;
    case 32:
      {
        BYTE    *pData = (BYTE*)lpbi + lpbi->biSize + PaletteSize( (BYTE*)lpbi );

        int iLO = pImage->GetWidth();
        for ( int j = 0; j < pImage->GetHeight(); j++ )
        {
          for ( int i = 0; i < pImage->GetWidth(); i++ )
          {
            ((DWORD*)pData)[i + ( pImage->GetHeight() - j - 1 ) * iLO] = (DWORD)*( (DWORD*)pImage->GetData() + i + j * pImage->GetWidth() );
          }
        }
      }
      break;
    default:
      dh::Log( "CreateHDIBFromImage unsupported depth %d\n", bi.biBitCount );
      break;
  }

  bi = *lpbi;

  /* clean up */
  GlobalUnlock(hDIB);

  /* return handle to the DIB */
  return hDIB;

}



GR::Graphic::Image* ResizeImage( GR::Graphic::Image* pSource, GR::Graphic::Palette* pPal, int iWidth, int iHeight, DWORD dwResizeType )
{

  if ( pSource == NULL )
  {
    return NULL;
  }

  GR::Graphic::ContextDescriptor    cdImage( pSource, pPal );

  GR::Graphic::Image* pImageTarget = new GR::Graphic::Image( iWidth, iHeight, pSource->GetDepth() );

  GR::Graphic::ContextDescriptor    cdTarget( pImageTarget, pPal );

  int     iOrigWidth = pSource->GetWidth(),
          iOrigHeight = pSource->GetHeight();
          
  if ( dwResizeType == 0 )
  {
    DWORD   dwPixel;

    for ( int i = 0; i < iWidth; i++ )
    {
      for ( int j = 0; j < iHeight; j++ )
      {
        dwPixel = cdImage.GetDirectPixel( ( i * iOrigWidth ) / iWidth, 
                                    ( j * iOrigHeight ) / iHeight );
        cdTarget.PutDirectPixel( i, j, dwPixel );
      }
    }
  }
  else if ( dwResizeType == 1 )
  {
    int iCount,
        iX1,
        iY1,
        iX2,
        iY2,
        iR,
        iG,
        iB;

    DWORD   dwPixel;

    CMaskedContextDescriptor* pCDSource = new CMaskedContextDescriptor( pSource, pPal ),
                      * pCDTarget = new CMaskedContextDescriptor( pImageTarget, pPal );

    for ( int i = 0; i < iWidth; i++ )
    {
      for ( int j = 0; j < iHeight; j++ )
      {
        iCount = 0;
        iX1 = ( i * iOrigWidth ) / iWidth;
        iY1 = ( j * iOrigHeight ) / iHeight;
        iX2 = ( ( i + 1 ) * iOrigWidth ) / iWidth;
        iY2 = ( ( j + 1 ) * iOrigHeight ) / iHeight;
        if ( iX2 >= iOrigWidth )
        {
          iX2 = iOrigWidth - 1;
        }
        if ( iY2 >= iOrigHeight )
        {
          iY2 = iOrigHeight - 1;
        }
        iR = 0;
        iG = 0;
        iB = 0;
        for ( int k = iX1; k <= iX2; k++ )
        {
          for ( int l = iY1; l <= iY2; l++ )
          {
            dwPixel = pCDSource->GetPixel( k, l );
            iR += ( ( dwPixel & 0xff0000 ) >> 16 );
            iG += ( ( dwPixel & 0x00ff00 ) >>  8 );
            iB += ( dwPixel & 0x0000ff );
            iCount++;
          }
        }
        dwPixel = pCDSource->GetPixel( ( i * iOrigWidth ) / iWidth, ( j * iOrigHeight ) / iHeight );
        iR += ( ( dwPixel & 0xff0000 ) >> 16 );
        iG += ( ( dwPixel & 0x00ff00 ) >>  8 );
        iB += ( dwPixel & 0x0000ff );
        iCount++;
        if ( iCount > 0 )
        {
          pCDTarget->PutPixel( i, j, ( ( iR / iCount ) << 16 ) + ( ( iG / iCount ) << 8 ) + ( iB / iCount ) );
        }
        else
        {
          pCDTarget->PutPixel( i, j, 0 );
        }
      }
    }
    delete pCDSource;
    delete pCDTarget;
  }
  else if ( dwResizeType >= 10 )
  {
    CMaskedContextDescriptor    cdSource( pSource, pPal );
    CMaskedContextDescriptor    cdTarget( pImageTarget, pPal );

    Resampler::Resample( cdSource, cdTarget, (Resampler::eSampleMethod)( dwResizeType - 10 ) );
  }

  return pImageTarget;

}




