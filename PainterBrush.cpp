#include "stdafx.h"
#include "PainterBrush.h"
#include "Settings.h"
#include "MaskedContextDescriptor.h"
#include "PainterGFXTools.h"
#include "BrushTip.h"

#include <Misc/Misc.h>

#include <Grafik/GFXHelper.h>
#include <Grafik/ImageFormate/ImageFormatManager.h>

#include <math\vector3.h>
#include <math\plane.h>

#include <set>



PainterBrush::PainterBrush()
  : Type( CSettings::BR_SOLID ),
    Angle( 0.0f ),
    pPattern( NULL )
{
}



PainterBrush::~PainterBrush()
{
}



void PainterBrush::SetPattern( CPattern *pP )
{
  // das alte rauswerfen
  if ( pPattern )
  {
    if ( pPattern->m_pImagePack )
    {
      SafeDelete( pPattern->m_pImagePack->m_pImage );
      SafeDelete( pPattern->m_pImagePack->m_pImageMask );
      SafeDelete( pPattern->m_pImagePack );
    }
    pPattern = NULL;
  }

  pPattern = pP;
  if ( !pP )
  {
    return;
  }

  if ( !pP->m_pImagePack )
  {
    GR::Graphic::ImageData*   pData = ImageFormatManager::Instance().LoadData( pP->m_FileName.c_str() );
    if ( pData )
    {
      pP->m_pImagePack->m_pImage    = new GR::Graphic::Image( *pData );
      pP->m_pImagePack->m_Palette   = pData->Palette();

      delete pData;
    }
  }
}



void PainterBrush::PutBrush( ViewInfo* pViewInfo, CMaskedContextDescriptor* pCD, int iX, int iY, int iAlphaValue, GR::tRect* pRCBounds  )
{
  if ( iAlphaValue == 0 )
  {
    return;
  }

  DWORD   dwWorkColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR ),
          dwWorkColor2 = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 );

  if ( ( pViewInfo )
  &&   ( pViewInfo->m_Type == ViewInfo::VI_ALPHA ) )
  {
    dwWorkColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_8BIT );
    dwWorkColor2 = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2_8BIT );

    dwWorkColor = ( pViewInfo->m_pPalette->Red( dwWorkColor ) << 16 ) 
                + ( pViewInfo->m_pPalette->Green( dwWorkColor ) << 8 ) 
                + pViewInfo->m_pPalette->Blue( dwWorkColor );
    dwWorkColor2 = ( pViewInfo->m_pPalette->Red( dwWorkColor2 ) << 16 ) 
                + ( pViewInfo->m_pPalette->Green( dwWorkColor2 ) << 8 ) 
                + pViewInfo->m_pPalette->Blue( dwWorkColor2 );
  }

  GR::tRect    rcBounds( 0, 0, pCD->Width(), pCD->Height() );
  if ( pRCBounds )
  {
    rcBounds = *pRCBounds;
  }

  switch ( Type )
  {
    case CSettings::BR_SOLID:
      pCD->PutPixelAlpha( iX, iY, dwWorkColor, iAlphaValue );
      break;
    case CSettings::BR_GRADIENT:
      {
        int   iGradientType = pSettings->GetSetting( "GradientType" );
        if ( iGradientType == 0 )
        {
          // Linear
          int       iWinkel = int( Angle );

          
          math::vector3   vertex[4],
                          rotVector( 1, 0, 0 );

          rotVector.RotateZ( float( -iWinkel ) );

          
          switch ( ( iWinkel % 360 ) / 90 )
          {
            case 0:
              vertex[0].x = (float)rcBounds.Left;
              vertex[0].y = (float)rcBounds.Bottom - 1;
            
              vertex[3].x = (float)rcBounds.Right - 1;
              vertex[3].y = (float)rcBounds.Top;
              break;
            case 1:
              vertex[0].x = (float)rcBounds.Right - 1;
              vertex[0].y = (float)rcBounds.Bottom - 1;

              vertex[3].x = (float)rcBounds.Left;
              vertex[3].y = (float)rcBounds.Top;
              break;
            case 2:
              vertex[0].x = (float)rcBounds.Right - 1;
              vertex[0].y = (float)rcBounds.Top;
                            
              vertex[3].x = (float)rcBounds.Left;
              vertex[3].y = (float)rcBounds.Bottom - 1;
              break;
            case 3:
              vertex[0].x = (float)rcBounds.Left;
              vertex[0].y = (float)rcBounds.Top;
                            
              vertex[3].x = (float)rcBounds.Right - 1;
              vertex[3].y = (float)rcBounds.Bottom - 1;
              break;
          }

          vertex[2] = vertex[0] + math::vector3( 0, 0, 1 );
          vertex[1] = vertex[0] + rotVector.RotatedZ( -90 );

          math::plane    plane( vertex[0], vertex[2], vertex[1] );

          math::vector3   vertPunkt( float( iX ), float( iY ), 0 );

          float fMax = plane.Distance( vertex[3] );
          if ( fMax <= 0.0f )
          {
            return;
          }
          float fPos = plane.Distance( vertPunkt );
          

          pCD->PutPixelAlpha( iX, iY, 
                              pSettings->m_Gradient.GetColorAt( ( fPos * pSettings->m_Gradient.Length() ) / fMax ),
                              iAlphaValue );
        }
        else if ( iGradientType == 1 )
        {
          // Circular
          GR::tFPoint     ptCenter;

          if ( pViewInfo )
          {
            ptCenter.set( (float)pViewInfo->m_pDocInfo->m_StartX, (float)pViewInfo->m_pDocInfo->m_StartY );
          }
          else
          {
            ptCenter.set( (float)rcBounds.width(), (float)rcBounds.height() );
          }

          GR::tFPoint     ptPos( (float)iX, (float)iY );

          // TODO - nicht richtig
          std::set<float>   setLength;

          setLength.insert( ( ptCenter - GR::tFPoint( (float)rcBounds.Left, (float)rcBounds.Top ) ).length_squared() );
          setLength.insert( ( ptCenter - GR::tFPoint( (float)rcBounds.Right, (float)rcBounds.Top ) ).length_squared() );
          setLength.insert( ( ptCenter - GR::tFPoint( (float)rcBounds.Left, (float)rcBounds.Bottom ) ).length_squared() );
          setLength.insert( ( ptCenter - GR::tFPoint( (float)rcBounds.Right, (float)rcBounds.Bottom ) ).length_squared() );

          float     fMax = *setLength.rbegin();

          float     fPos = ( ptPos - ptCenter ).length_squared();

          if ( fMax == 0.0f )
          {
            fMax = 1.0f;
          }

          pCD->PutPixelAlpha( iX, iY, 
                              pSettings->m_Gradient.GetColorAt( ( fPos * pSettings->m_Gradient.Length() ) / fMax ),
                              iAlphaValue );
        }
      }
      break;
    case CSettings::BR_PATTERN:
      {
        if ( ( pPattern == NULL )
        ||   ( pPattern->m_pImagePack == NULL ) )
        {
          return;
        }
        math::vector3   vertPunkt( float( iX ), float( iY ), 0.0f );

        vertPunkt.RotateZ( -Angle );
        
        CMaskedContextDescriptor* pCDPattern = new CMaskedContextDescriptor( pPattern->m_pImagePack->m_pImage, &pPattern->m_pImagePack->m_Palette );

        int   iNewX = ( (int)vertPunkt.x ),
              iNewY = ( (int)vertPunkt.y );

        while ( iNewX < 0 )
        {
          iNewX += pCDPattern->Width();
        }
        while ( iNewY < 0 )
        {
          iNewY += pCDPattern->Height();
        }
        pCD->PutPixelAlpha( iX, iY, pCDPattern->GetPixel( iNewX % pCDPattern->Width(), iNewY % pCDPattern->Height() ), iAlphaValue );

        delete pCDPattern;
      }
      break;
    case CSettings::BR_BRIGHTEN:
      {
        if ( pViewInfo == NULL )
        {
          pCD->PutPixelAlpha( iX, iY, GFX::ColorAdjustBrightness( dwWorkColor, 130 ), iAlphaValue );
        }
        else
        {
          int   iValue = pViewInfo->m_pDocInfo->m_pImageEffectMask->GetPixel( iX, iY );
          if ( iValue < 30 )
          {
            int   iDelta = ( 30 * iAlphaValue ) / 255;
            iValue += iDelta;
            if ( iValue > 30 )
            {
              iDelta -= iValue - 30;
              iValue = 30;
            }
            pViewInfo->m_pDocInfo->m_pImageEffectMask->SetPixel( iX, iY, iValue );
            pCD->PutPixelAlpha( iX, iY, GFX::ColorAdjustBrightness( pCD->GetPixel( iX, iY ), 100 + iDelta ), iAlphaValue );
          }
        }
      }
      break;
    case CSettings::BR_DARKEN:
      {
        if ( pViewInfo == NULL )
        {
          pCD->PutPixelAlpha( iX, iY, GFX::ColorAdjustBrightness( dwWorkColor, 70 ), iAlphaValue );
        }
        else
        {
          int   iValue = pViewInfo->m_pDocInfo->m_pImageEffectMask->GetPixel( iX, iY );
          if ( iValue < 30 )
          {
            int   iDelta = ( 30 * iAlphaValue ) / 255;
            iValue += iDelta;
            if ( iValue > 30 )
            {
              iDelta -= iValue - 30;
              iValue = 30;
            }
            pViewInfo->m_pDocInfo->m_pImageEffectMask->SetPixel( iX, iY, iValue );
            pCD->PutPixelAlpha( iX, iY, GFX::ColorAdjustBrightness( pCD->GetPixel( iX, iY ), 100 - iDelta ), iAlphaValue );
          }
        }
      }
      break;
    case CSettings::BR_SMEAR:
      {
        if ( pViewInfo == NULL )
        {
          pCD->PutPixelAlpha( iX, iY, dwWorkColor, iAlphaValue );
        }
        else
        {
          int   iValue = pViewInfo->m_pDocInfo->m_pImageEffectMask->GetPixel( iX, iY );
          if ( iValue < 30 )
          {
            int   iDelta = ( 30 * iAlphaValue ) / 255;
            iValue += iDelta;
            if ( iValue > 30 )
            {
              iDelta -= iValue - 30;
              iValue = 30;
            }
            int   iDX = 1 - 2 * ( rand() % 2 );
            int   iDY = 1 - 2 * ( rand() % 2 );

            DWORD   dwPixel1 = pCD->GetPixel( iX + iDX, iY + iDY );
            DWORD   dwPixel2 = pCD->GetPixel( iX, iY );

            pCD->PutPixelAlpha( iX, iY, dwPixel1, iDelta );
            pCD->PutPixelAlpha( iX + iDX, iY + iDY, dwPixel2, iDelta );
          }
        }
      }
      break;
  }
}



void PainterBrush::DrawOnPage( GR::Graphic::GFXPage *pPage, GR::u32 dwColor )
{
  GR::Graphic::Palette* pPal = new GR::Graphic::Palette();
  pPage->GetPalette( pPal );
  CMaskedContextDescriptor* pCD = new CMaskedContextDescriptor( pPage, pPal );

  for ( int j = 0; j < pPage->GetHeight(); j++ )
  {
    for ( int i = 0; i < pPage->GetWidth(); i++ )
    {
      PutBrush( NULL, pCD, i, j, 255 );
    }
  }
  
  SafeDelete( pCD );
  SafeDelete( pPal );
}



void PainterBrush::ApplyBrushTipOnMask( ViewInfo* pViewInfo, CMaskedContextDescriptor* pCD, GR::Graphic::Image *pImageMask, GR::tRect* pRectBounds )
{
  GR::tRect   rcWork;

  if ( pRectBounds )
  {
    rcWork = *pRectBounds;
  }
  else
  {
    rcWork.set( 0, 0, pImageMask->GetWidth(), pImageMask->GetHeight() );
  }

  GR::Graphic::Image            imageFillMask( pImageMask->GetWidth(), pImageMask->GetHeight(), 8 );

  GR::Graphic::ContextDescriptor    cdImageMask( pImageMask ),
                        cdImageBrushMask( &imageFillMask );

  int                   iAlphaValue = 0;

  // jeder Pixel der Maske zeigt an, ob die Brush-Spitze dort aufgedrückt wurde (und wie stark)
  GR::Graphic::Image&           ImageBrushTip = pSettings->m_pCurrentBrushTip->m_ImageTipMask;

  for ( int i = rcWork.Left; i < rcWork.Right; i++ )
  {
    for ( int j = rcWork.Top; j < rcWork.Bottom; j++ )
    {
      iAlphaValue = cdImageMask.GetDirectPixel( i, j );

      if ( iAlphaValue == 0 )
      {
        continue;
      }

      for ( int iBrushTipX = 0; iBrushTipX < ImageBrushTip.GetWidth(); ++iBrushTipX )
      {
        for ( int iBrushTipY = 0; iBrushTipY < ImageBrushTip.GetHeight(); ++iBrushTipY )
        {
          int     iPixelX = i + iBrushTipX - ImageBrushTip.GetWidth() / 2,
                  iPixelY = j + iBrushTipY - ImageBrushTip.GetHeight() / 2;

          if ( ( iPixelX < 0 )
          ||   ( iPixelY < 0 )
          ||   ( iPixelX >= pImageMask->GetWidth() )
          ||   ( iPixelY >= pImageMask->GetHeight() ) )
          {
            continue;
          }
          BYTE*   pBrushMaskData = (BYTE*)cdImageBrushMask.GetRowColumnData( iPixelX, iPixelY );

          int     iBrushTip = 255 - (BYTE)ImageBrushTip.GetPixel( iBrushTipX, iBrushTipY );

          if ( iBrushTip == 0 )
          {
            continue;
          }

          iAlphaValue = ( iAlphaValue * iBrushTip ) / 255;

          iAlphaValue += *pBrushMaskData;

          if ( iAlphaValue > 255 )
          {
            iAlphaValue = 255;
          }

          *pBrushMaskData = (BYTE)iAlphaValue;
        }
      }
    }
  }

  // das Boundingrect um die Breite des Brushtip erweitern
  rcWork.inflate( ImageBrushTip.GetWidth() / 2, ImageBrushTip.GetHeight() / 2 );
  if ( rcWork.Left < 0 )
  {
    rcWork.Left = 0;
  }
  if ( rcWork.Top < 0 )
  {
    rcWork.Top = 0;
  }
  if ( rcWork.Right > pImageMask->GetWidth() )
  {
    rcWork.Right = pImageMask->GetWidth();
  }
  if ( rcWork.Bottom > pImageMask->GetHeight() )
  {
    rcWork.Bottom = pImageMask->GetHeight();
  }

  for ( int i = rcWork.Left; i < rcWork.Right; i++ )
  {
    for ( int j = rcWork.Top; j < rcWork.Bottom; j++ )
    {
      PutBrush( pViewInfo, pCD, i, j, cdImageBrushMask.GetDirectPixel( i, j ), pRectBounds );
    }
  }
}



