#include "stdafx.h"
#include "Layer.h"

#include <Misc/Misc.h>

#include <Grafik/ContextDescriptor.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CLayer::CLayer() :
  m_PImage(),
  m_Transparent( false ),
  m_Transparenz( 0 ),
  m_Visible( true ),
  m_MaskEnabled( false ),
  m_HasMask( false ),
  m_Alpha( 255 ),
  m_Type( LT_NORMAL )
{

}

CLayer::~CLayer()
{

}



void CLayer::DrawLayerWithMask1( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage* pPage, GR::Graphic::Palette *pPalette, const GR::tRect& rc )
{

  int                 i,
                      j,
                      iR[2],
                      iG[2],
                      iB[2];

  BYTE                *pMask = NULL,
                      *pPImage,
                      *pPPage;

  GR::Graphic::ContextDescriptor  cdImage( m_PImage.m_pImage, pPalette );

  DWORD               dwTransparentColor = m_Transparenz;


  if ( m_PImage.m_pImageMask == NULL )
  {
    if ( !m_Transparent )
    {
      m_PImage.m_pImage->PutImage( pPage, 0, 0 );
      return;
    }

    for ( j = rc.Top; j < rc.Bottom; j++ )
    {
      if ( m_PImage.m_pImageMask )
      {
        pMask = (BYTE*)m_PImage.m_pImageMask->GetData() + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
      }
      pPImage = ( (BYTE*)m_PImage.m_pImage->GetData() ) + j * m_PImage.m_pImage->GetWidth() + rc.Left;
      pPPage = ( (BYTE*)pPage->GetData() ) + j * pPage->GetLineOffset() + rc.Left;
      for ( i = rc.Left; i < rc.Right; i++ )
      {
        if ( cdImage.GetDirectPixel( i, j ) == dwTransparentColor )
        {
          pPPage++;
          continue;
        }
        pPage->PutPixel( i, j, cdImage.GetPixel( i, j ) );
        pPPage++;
      }
    }

    return;
  }
  for ( j = rc.Top; j < rc.Bottom; j++ )
  {
    if ( m_PImage.m_pImageMask )
    {
      pMask = (BYTE*)m_PImage.m_pImageMask->GetData() + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
    }
    pPImage = ( (BYTE*)m_PImage.m_pImage->GetData() ) + j * m_PImage.m_pImage->GetWidth() + rc.Left;
    pPPage = ( (BYTE*)pPage->GetData() ) + j * pPage->GetLineOffset() + rc.Left;
    for ( i = rc.Left; i < rc.Right; i++ )
    {
      if ( !*pMask )
      {
        pMask++;
        pPPage++;
        continue;
      }
      else if ( ( m_Transparent )
      &&        ( cdImage.GetDirectPixel( i, j ) == dwTransparentColor ) )
      {
        pMask++;
        pPPage++;
        continue;
      }
      else if ( *pMask == 255 )
      {
        pPage->PutPixel( i, j, cdImage.GetPixel( i, j ) );
        pPPage++;
      }
      else
      {
        // hier Alpha reinrechnen
        BYTE    bPixel = (BYTE)cdImage.GetPixel( i, j );
        iR[0] = pPalette->Red( bPixel );
        iG[0] = pPalette->Green( bPixel );
        iB[0] = pPalette->Blue( bPixel );
        iR[1] = pPalette->Red( *pPPage );
        iG[1] = pPalette->Green( *pPPage );
        iB[1] = pPalette->Blue( *pPPage );
        *pPPage++ = (BYTE)cdImage.MapColorToPalette( 
              ( ( ( ( iR[0] * *pMask ) + ( iR[1] * ( 255 - *pMask ) ) ) >> 8 ) << 16 ) +
              ( ( ( ( iG[0] * *pMask ) + ( iG[1] * ( 255 - *pMask ) ) ) >> 8 ) << 8 ) +
              ( ( ( iB[0] * *pMask ) + ( iB[1] * ( 255 - *pMask ) ) ) >> 8 ) );
      }
      pMask++;
    }
  }
}



void CLayer::DrawLayerWithMask4( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage* pPage, GR::Graphic::Palette* pPalette, const GR::tRect& rc )
{
  int                 i,
                      j,
                      iR[2],
                      iG[2],
                      iB[2];

  BYTE                *pMask = NULL;

  GR::Graphic::ContextDescriptor    cdImage( m_PImage.m_pImage, pPalette );
  GR::Graphic::ContextDescriptor    cdPage( pPage );

  DWORD               dwTransparentColor = m_Transparenz;


  if ( ( m_PImage.m_pImageMask == NULL )
  ||   ( !m_MaskEnabled ) )
  {
    if ( !m_Transparent )
    {
      GR::Graphic::ContextDescriptor    cdImage( m_PImage.m_pImage );

      cdImage.CopyArea( 0, 0, cdImage.Width(), cdImage.Height(), 0, 0, &cdPage );
      return;
    }

    for ( j = rc.Top; j < rc.Bottom; j++ )
    {
      if ( m_PImage.m_pImageMask )
      {
        pMask = (BYTE*)m_PImage.m_pImageMask->GetData() + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
      }
      BYTE* pPPage = ( (BYTE*)pPage->GetData() ) + j * pPage->GetLineOffset() + rc.Left;
      for ( i = rc.Left; i < rc.Right; i++ )
      {
        if ( cdImage.GetDirectPixel( i, j ) == dwTransparentColor )
        {
          pPPage++;
          continue;
        }
        pPage->PutPixel( i, j, cdImage.GetDirectPixel( i, j ) );
        pPPage++;
      }
    }

    return;
  }
  for ( j = rc.Top; j < rc.Bottom; j++ )
  {
    if ( m_PImage.m_pImageMask )
    {
      pMask = (BYTE*)m_PImage.m_pImageMask->GetData() + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
    }
    for ( i = rc.Left; i < rc.Right; i++ )
    {
      if ( !*pMask )
      {
        pMask++;
        continue;
      }
      else if ( ( m_Transparent )
      &&        ( cdImage.GetDirectPixel( i, j ) == dwTransparentColor ) )
      {
        pMask++;
        continue;
      }
      else if ( *pMask == 255 )
      {
        pPage->PutPixel( i, j, cdImage.GetDirectPixel( i, j ) );
      }
      else
      {
        // hier Alpha reinrechnen
        BYTE    bPixel = BYTE( cdImage.GetDirectPixel( i, j ) );
        BYTE    targetPixel = cdPage.GetDirectPixel( i, j );

        iR[0] = pPalette->Red( bPixel );
        iG[0] = pPalette->Green( bPixel );
        iB[0] = pPalette->Blue( bPixel );
        iR[1] = pPalette->Red( targetPixel );
        iG[1] = pPalette->Green( targetPixel );
        iB[1] = pPalette->Blue( targetPixel );

        GR::u32     targetColor = ( ( ( ( iR[0] * *pMask ) + ( iR[1] * ( 255 - *pMask ) ) ) >> 8 ) << 16 ) +
          ( ( ( ( iG[0] * *pMask ) + ( iG[1] * ( 255 - *pMask ) ) ) >> 8 ) << 8 ) +
          ( ( ( iB[0] * *pMask ) + ( iB[1] * ( 255 - *pMask ) ) ) >> 8 );

        cdPage.PutPixel( i, j, targetColor );
      }
      pMask++;
    }
  }
}



void CLayer::DrawLayerWithMask8( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage *pPage, GR::Graphic::Palette *pPalette, const GR::tRect& rc )
{

  int                 i,
                      j,
                      iR[2],
                      iG[2],
                      iB[2];

  BYTE                *pMask = NULL,
                      *pPImage,
                      *pPPage;

  GR::Graphic::ContextDescriptor  cdImage( m_PImage.m_pImage, pPalette );

  DWORD               dwTransparentColor = m_Transparenz;


  if ( m_PImage.m_pImageMask == NULL )
  {
    if ( !m_Transparent )
    {
      m_PImage.m_pImage->PutImage( pPage, 0, 0 );
      return;
    }

    for ( j = rc.Top; j < rc.Bottom; j++ )
    {
      if ( m_PImage.m_pImageMask )
      {
        pMask = (BYTE*)m_PImage.m_pImageMask->GetData() + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
      }
      pPImage = ( (BYTE*)m_PImage.m_pImage->GetData() ) + j * m_PImage.m_pImage->GetWidth() + rc.Left;
      pPPage = ( (BYTE*)pPage->GetData() ) + j * pPage->GetLineOffset() + rc.Left;
      for ( i = rc.Left; i < rc.Right; i++ )
      {
        if ( cdImage.GetDirectPixel( i, j ) == dwTransparentColor )
        {
          pPPage++;
          continue;
        }
        pPage->PutPixel( i, j, cdImage.GetDirectPixel( i, j ) );
        pPPage++;
      }
    }

    return;
  }
  for ( j = rc.Top; j < rc.Bottom; j++ )
  {
    if ( m_PImage.m_pImageMask )
    {
      pMask = (BYTE*)m_PImage.m_pImageMask->GetData() + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
    }
    pPImage = ( (BYTE*)m_PImage.m_pImage->GetData() ) + j * m_PImage.m_pImage->GetWidth() + rc.Left;
    pPPage = ( (BYTE*)pPage->GetData() ) + j * pPage->GetLineOffset() + rc.Left;
    for ( i = rc.Left; i < rc.Right; i++ )
    {
      if ( !*pMask )
      {
        pMask++;
        pPPage++;
        continue;
      }
      else if ( ( m_Transparent )
      &&        ( cdImage.GetDirectPixel( i, j ) == dwTransparentColor ) )
      {
        pMask++;
        pPPage++;
        continue;
      }
      else if ( *pMask == 255 )
      {
        pPage->PutPixel( i, j, cdImage.GetDirectPixel( i, j ) );
        pPPage++;
      }
      else
      {
        // hier Alpha reinrechnen
        BYTE    bPixel = BYTE( cdImage.GetDirectPixel( i, j ) );

        iR[0] = pPalette->Red( bPixel );
        iG[0] = pPalette->Green( bPixel );
        iB[0] = pPalette->Blue( bPixel );
        iR[1] = pPalette->Red( *pPPage );
        iG[1] = pPalette->Green( *pPPage );
        iB[1] = pPalette->Blue( *pPPage );
        *pPPage++ = (BYTE)cdImage.MapColorToPalette( 
              ( ( ( ( iR[0] * *pMask ) + ( iR[1] * ( 255 - *pMask ) ) ) >> 8 ) << 16 ) +
              ( ( ( ( iG[0] * *pMask ) + ( iG[1] * ( 255 - *pMask ) ) ) >> 8 ) << 8 ) +
              ( ( ( iB[0] * *pMask ) + ( iB[1] * ( 255 - *pMask ) ) ) >> 8 ) );
      }
      pMask++;
    }
  }
}



void CLayer::DrawLayerWithMask555( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage *pPage, const GR::tRect& rc )
{

  int                 i,
                      j,
                      iLayerAlpha = m_Alpha,
                      iAlpha,
                      iR[3],
                      iG[3],
                      iB[3];

  BYTE*               pMask = NULL;

  WORD                *pPImage,
                      *pPPage;


  GR::Graphic::ContextDescriptor  cdImage( m_PImage.m_pImage );

  DWORD               dwTransparentColor = cdImage.MapColor( m_Transparenz );

  // da muﬂ noch mal was getan werden!
  if ( iLayerAlpha == 256 )
  {
    iLayerAlpha = 255;
  }
  for ( j = rc.Top; j < rc.Bottom; j++ )
  {
    if ( m_PImage.m_pImageMask )
    {
      pMask = (BYTE*)m_PImage.m_pImageMask->GetData() + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
    }
    pPImage = ( (WORD*)m_PImage.m_pImage->GetData() ) + j * m_PImage.m_pImage->GetWidth() + rc.Left;
    pPPage = ( (WORD*)pPage->GetData() ) + j * pPage->GetLineOffset() / 2 + rc.Left;
    for ( i = rc.Left; i < rc.Right; i++ )
    {
      if ( ( m_MaskEnabled )
      &&   ( m_HasMask ) )
      {
        iAlpha = ( *pMask * iLayerAlpha ) / 255;
      }
      else
      {
        iAlpha = iLayerAlpha;
      }
      if ( !iAlpha )
      {
        // die Maske hat 0
        pMask++;
        ++pPPage;
        ++pPImage;
        continue;
      }
      else if ( ( m_Transparent )
      &&        ( *pPImage == dwTransparentColor ) )
      {
        pMask++;
        ++pPPage;
        ++pPImage;
        continue;
      }
      else if ( iAlpha == 255 )
      {
        *pPPage++ = *pPImage++;
      }
      else
      {
        // hier Alpha reinrechnen
        iR[0] = ( ( *pPImage & 0x07c00 ) >> 10 );
        iG[0] = ( ( *pPImage & 0x003e0 ) >> 5 );
        iB[0] =   ( *pPImage & 0x0001f );
        iR[1] = ( ( *pPPage & 0x07c00 ) >> 10 );
        iG[1] = ( ( *pPPage & 0x003e0 ) >> 5 );
        iB[1] =   ( *pPPage & 0x0001f );

        iR[2] = ( ( ( iR[0] * iAlpha ) + ( iR[1] * ( 255 - iAlpha ) ) ) >> 8 );
        iG[2] = ( ( ( iG[0] * iAlpha ) + ( iG[1] * ( 255 - iAlpha ) ) ) >> 8 );
        iB[2] = ( ( ( iB[0] * iAlpha ) + ( iB[1] * ( 255 - iAlpha ) ) ) >> 8 );

        *pPPage++ = (WORD)( ( ( iR[2] & 0x1f ) << 10 ) + ( ( iG[2] & 0x1f ) << 5 ) + ( ( iB[2] & 0x1f ) ) );
        ++pPImage;
      }
      ++pMask;
    }
  }
}



void CLayer::DrawLayerWithMask565( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage *pPage, const GR::tRect& rc )
{
  int                 i,
                      j,
                      iLayerAlpha = m_Alpha,
                      iAlpha,
                      iR[3],
                      iG[3],
                      iB[3];

  BYTE                *pMask = NULL;

  WORD                *pPImage,
                      *pPPage;


  GR::Graphic::ContextDescriptor  cdImage( m_PImage.m_pImage );

  DWORD               dwTransparentColor = cdImage.MapColor( m_Transparenz );


  // da muﬂ noch mal was getan werden!
  if ( iLayerAlpha == 256 )
  {
    iLayerAlpha = 255;
  }
  for ( j = rc.Top; j < rc.Bottom; j++ )
  {
    if ( m_PImage.m_pImageMask )
    {
      pMask = ( (BYTE*)m_PImage.m_pImageMask->GetData() ) + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
    }
    pPImage = ( (WORD*)m_PImage.m_pImage->GetData() ) + j * m_PImage.m_pImage->GetWidth() + rc.Left;
    pPPage = ( (WORD*)pPage->GetData() ) + j * pPage->GetLineOffset() / 2 + rc.Left;
    for ( i = rc.Left; i < rc.Right; i++ )
    {
      if ( ( m_MaskEnabled )
      &&   ( m_HasMask ) )
      {
        iAlpha = ( *pMask * iLayerAlpha ) / 255;
      }
      else
      {
        iAlpha = iLayerAlpha;
      }
      if ( !iAlpha )
      {
        // die Maske hat 0
        pMask++;
        ++pPPage;
        ++pPImage;
        continue;
      }
      else if ( ( m_Transparent )
      &&        ( *pPImage == dwTransparentColor ) )
      {
        pMask++;
        ++pPPage;
        ++pPImage;
        continue;
      }
      else if ( iAlpha == 255 )
      {
        *pPPage++ = *pPImage++;
      }
      else
      {
        // hier Alpha reinrechnen
        iR[0] = ( ( *pPImage & 0x0f800 ) >> 11 );
        iG[0] = ( ( *pPImage & 0x007e0 ) >> 5 );
        iB[0] =   ( *pPImage & 0x0001f );
        iR[1] = ( ( *pPPage & 0x0f800 ) >> 11 );
        iG[1] = ( ( *pPPage & 0x007e0 ) >> 5 );
        iB[1] =   ( *pPPage & 0x0001f );

        iR[2] = ( ( ( iR[0] * iAlpha ) + ( iR[1] * ( 255 - iAlpha ) ) ) >> 8 );
        iG[2] = ( ( ( iG[0] * iAlpha ) + ( iG[1] * ( 255 - iAlpha ) ) ) >> 8 );
        iB[2] = ( ( ( iB[0] * iAlpha ) + ( iB[1] * ( 255 - iAlpha ) ) ) >> 8 );

        *pPPage++ = (WORD)( ( ( iR[2] & 0x1f ) << 11 ) + ( ( iG[2] & 0x3f ) << 5 ) + ( ( iB[2] & 0x1f ) ) );
        ++pPImage;
      }
      ++pMask;
    }
  }
}



void CLayer::DrawLayerWithMask24( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage *pPage, const GR::tRect& rc )
{

  int                 i,
                      j,
                      iLayerAlpha = m_Alpha,
                      iAlpha,
                      iR[2],
                      iG[2],
                      iB[2];

  BYTE                *pMask = NULL,
                      *pPImage,
                      *pPPage;


  GR::Graphic::ContextDescriptor  cdImage( m_PImage.m_pImage );

  DWORD               dwTransparentColor = cdImage.MapColor( m_Transparenz );


  // da muﬂ noch mal was getan werden!
  if ( iLayerAlpha == 256 )
  {
    iLayerAlpha = 255;
  }
  for ( j = rc.Top; j < rc.Bottom; j++ )
  {
    if ( m_PImage.m_pImageMask )
    {
      pMask = (BYTE*)m_PImage.m_pImageMask->GetData() + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
    }
    pPImage = ( (BYTE*)m_PImage.m_pImage->GetData() ) + j * 3 * m_PImage.m_pImage->GetWidth() + rc.Left * 3;
    pPPage = ( (BYTE*)pPage->GetData() ) + j * pPage->GetLineOffset() + rc.Left * 3;
    for ( i = rc.Left; i < rc.Right; i++ )
    {
      if ( ( m_MaskEnabled )
      &&   ( m_HasMask ) )
      {
        iAlpha = ( *pMask * iLayerAlpha ) / 255;
      }
      else
      {
        iAlpha = iLayerAlpha;
      }
      //if ( !*pMask )
      if ( !iAlpha )
      {
        // die Maske hat 0
        pMask++;
        pPPage += 3;
        pPImage += 3;
        continue;
      }
      else if ( ( m_Transparent )
      &&        ( cdImage.GetPixel( i, j ) == dwTransparentColor ) )
      {
        pMask++;
        pPPage += 3;
        pPImage += 3;
        continue;
      }
      else if ( iAlpha == 255 )
      {
        pPage->PutPixel( i, j, cdImage.GetPixel( i, j ) );
        pPPage += 3;
        pPImage += 3;
      }
      else
      {
        // hier Alpha reinrechnen
        iR[0] = pPImage[0];
        iG[0] = pPImage[1];
        iB[0] = pPImage[2];
        iR[1] = pPPage[0];
        iG[1] = pPPage[1];
        iB[1] = pPPage[2];

        *pPPage++ = (BYTE)( ( ( iR[0] * iAlpha ) + ( iR[1] * ( 255 - iAlpha ) ) ) >> 8 );
        *pPPage++ = (BYTE)( ( ( iG[0] * iAlpha ) + ( iG[1] * ( 255 - iAlpha ) ) ) >> 8 );
        *pPPage++ = (BYTE)( ( ( iB[0] * iAlpha ) + ( iB[1] * ( 255 - iAlpha ) ) ) >> 8 );

        pPImage += 3;
      }
      if ( m_PImage.m_pImageMask )
      {
        pMask++;
      }
    }
  }
}



void CLayer::DrawLayerWithMask32( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage *pPage, const GR::tRect& rc )
{

  int                 i,
                      j,
                      iLayerAlpha = m_Alpha,
                      iAlpha,
                      iR[2],
                      iG[2],
                      iB[2];

  GR::u8*             pMask = NULL;
  GR::u8*             pPImage = NULL;
  GR::u8*             pPPage = NULL;


  GR::Graphic::ContextDescriptor  cdImage( m_PImage.m_pImage );

  DWORD               dwTransparentColor = cdImage.MapColor( m_Transparenz );

  // da muﬂ noch mal was getan werden!
  if ( iLayerAlpha == 256 )
  {
    iLayerAlpha = 255;
  }
  for ( j = rc.Top; j < rc.Bottom; j++ )
  {
    if ( m_PImage.m_pImageMask )
    {
      pMask = (BYTE*)m_PImage.m_pImageMask->GetData() + j * m_PImage.m_pImageMask->GetWidth() + rc.Left;
    }
    pPImage = ( (BYTE*)m_PImage.m_pImage->GetData() ) + j * 4 * m_PImage.m_pImage->GetWidth() + rc.Left * 4;
    pPPage = ( (BYTE*)pPage->GetData() ) + j * pPage->GetLineOffset() + rc.Left * 4;
    for ( i = rc.Left; i < rc.Right; i++ )
    {
      if ( ( m_MaskEnabled )
      &&   ( m_HasMask ) )
      {
        iAlpha = ( *pMask * iLayerAlpha ) / 255;
      }
      else
      {
        iAlpha = iLayerAlpha;
      }
      //if ( !*pMask )
      if ( !iAlpha )
      {
        // die Maske hat 0
        pMask++;
        pPPage += 4;
        pPImage += 4;
        continue;
      }
      else if ( ( m_Transparent )
      &&        ( *(DWORD*)pPImage == dwTransparentColor ) )
      {
        pMask++;
        pPPage += 4;
        pPImage += 4;
        continue;
      }
      else if ( iAlpha == 255 )
      {
        *(DWORD*)pPPage = *(DWORD*)pPImage;
        pPPage += 4;
        pPImage += 4;
      }
      else
      {
        // hier Alpha reinrechnen
        iR[0] = pPImage[0];
        iG[0] = pPImage[1];
        iB[0] = pPImage[2];
        iR[1] = pPPage[0];
        iG[1] = pPPage[1];
        iB[1] = pPPage[2];

        *pPPage++ = (BYTE)( ( ( iR[0] * iAlpha ) + ( iR[1] * ( 255 - iAlpha ) ) ) >> 8 );
        *pPPage++ = (BYTE)( ( ( iG[0] * iAlpha ) + ( iG[1] * ( 255 - iAlpha ) ) ) >> 8 );
        *pPPage++ = (BYTE)( ( ( iB[0] * iAlpha ) + ( iB[1] * ( 255 - iAlpha ) ) ) >> 8 );

        pPPage++;
        pPImage += 4;
      }
      if ( m_PImage.m_pImageMask )
      {
        pMask++;
      }
    }
  }

}



void CLayer::DrawImage( GR::Graphic::GFXPage *pPage, GR::Graphic::Palette *pPalette, const GR::tRect& rectOrig, GR::Graphic::Image* pImage, int iX, int iY )
{
  if ( m_PImage.m_pImage == NULL )
  {
    return;
  }
  if ( m_Alpha == 0 )
  {
    // dieses Bild sieht man ¸berhaupt nicht!
    return;
  }
  if ( pImage == NULL )
  {
    pImage = m_PImage.m_pImage;
  }

  GR::tRect    rect = rectOrig;

  if ( rect.Right > pImage->GetWidth() )
  {
    rect.Right = pImage->GetWidth();
  }
  if ( rect.Bottom > pImage->GetHeight() )
  {
    rect.Bottom = pImage->GetHeight();
  }

  switch ( pImage->GetDepth() )
  {
    case 1:
      DrawLayerWithMask1( pImage, iX, iY, pPage, pPalette, rect );
      break;
    case 4:
      DrawLayerWithMask4( pImage, iX, iY, pPage, pPalette, rect );
      break;
    case 8:
      DrawLayerWithMask8( pImage, iX, iY, pPage, pPalette, rect );
      break;
    case 15:
      DrawLayerWithMask555( pImage, iX, iY, pPage, rect );
      break;
    case 16:
      DrawLayerWithMask565( pImage, iX, iY, pPage, rect );
      break;
    case 24:
      DrawLayerWithMask24( pImage, iX, iY, pPage, rect );
      break;
    case 32:
      DrawLayerWithMask32( pImage, iX, iY, pPage, rect );
      break;
  }
}



void CLayer::SetTransparencyIndex( bool Transparent, GR::u32 TransparentColor )
{
  m_Transparent   = Transparent;
  m_Transparenz   = TransparentColor;
}



void CLayer::SetLayerImage( GR::Graphic::Image* pImage )
{
  if ( m_PImage.m_pImage )
  {
    delete m_PImage.m_pImage;
  }

  m_PImage.m_pImage = pImage;
}



void CLayer::SetLayerMask( GR::Graphic::Image* pImageMask )
{

  bool    bHadMaskBefore = false;

  if ( m_PImage.m_pImageMask )
  {
    bHadMaskBefore = true;
    delete m_PImage.m_pImageMask;
  }

  m_PImage.m_pImageMask = pImageMask;

  if ( pImageMask )
  {
    m_HasMask = true;
    if ( !bHadMaskBefore )
    {
      m_MaskEnabled = true;
    }
  }
  else
  {
    m_HasMask = false;
    m_MaskEnabled = false;
  }

}



GR::Graphic::Image* CLayer::GetImage()
{

  return m_PImage.m_pImage;

}



GR::Graphic::Image* CLayer::GetMask()
{

  if ( !m_HasMask )
  {
    return NULL;
  }
  return m_PImage.m_pImageMask;

}



void CLayer::Clear()
{

  SafeDelete( m_PImage.m_pImage );
  SafeDelete( m_PImage.m_pImageMask );

}