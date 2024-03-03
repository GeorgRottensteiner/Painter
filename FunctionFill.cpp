#include "stdafx.h"

#include "DocumentInfo.h"

#include "functionfill.h"

#include "Settings.h"
#include "ViewInfo.h"


#include "MaskedContextDescriptor.h"

#include <Misc/Misc.h>



bool CFunctionFill::CompareColors( GR::u32 Color1, GR::u32 Color2 )
{
  int   dr = abs( (int)( ( ( Color1 & 0xff0000 ) >> 16 ) - ( ( Color2 & 0xff0000 ) >> 16 ) ) );
  int   dg = abs( (int)( ( ( Color1 & 0x00ff00 ) >>  8 ) - ( ( Color2 & 0x00ff00 ) >>  8 ) ) );
  int   db = abs( (int)( ( Color1 & 0xff ) - ( Color2 & 0xff ) ) );

  int   difference = dr * dr + dg * dg + db * db;

  int   logVal = pSettings->GetSetting( "FillToleranzLog", 0 );

  if ( difference > logVal * 195075.0f / 100.0f )
  {
    return false;
  }
  return true;
}



GR::Graphic::Image* CFunctionFill::GetFillMask( CMaskedContextDescriptor* pCD, int x, int y, GR::tRect& Bounds )
{
  int               bufzeiger,
                    xobenr,
                    xuntenr,
                    x1,
                    max;

  DWORD             grf;

  unsigned int      paintbufferx[30000],
                    paintbuffery[30000];


  Bounds.Clear();
  if ( !pSettings->GetSetting( "FillClipGradient" ) )
  {
    Bounds.Set( 0, 0, pCD->Width(), pCD->Height() );
  }

  if ( pCD == NULL )
  {
    return NULL;
  }

  if ( ( x < 0 )
  ||   ( x >= pCD->Width() )
  ||   ( y < 0 )
  ||   ( y >= pCD->Height() ) )
  {
    return NULL;
  }

  GR::Graphic::Image*           pImageMask = new GR::Graphic::Image( pCD->Width(), pCD->Height(), 8 );

  GR::Graphic::ContextDescriptor    cdMask( pImageMask );


  grf = pCD->GetPixel( x, y );
  bufzeiger = 1;
  max = 0;
  paintbufferx[bufzeiger] = x;
  paintbuffery[bufzeiger] = y;
  do
  {
    x = paintbufferx[bufzeiger];
    y = paintbuffery[bufzeiger];
    bufzeiger--;
    while ( ( CompareColors( pCD->GetPixel( x - 1, y ), grf ) )
    &&      ( cdMask.GetDirectPixel( x - 1, y ) != 255 )
    &&      ( x > 0 ) )
    {
      x--;
    }
    x1 = x;
    xobenr = 0;
    xuntenr = 0;
    if ( y == 0 )
    {
      xobenr = pCD->Width() - 1;
    }
    if ( y == (int)pCD->Height() - 1 )
    {
      xuntenr = pCD->Width() - 1;
    }
    do
    {
      if ( x >= xobenr )
      {
        if ( ( CompareColors( pCD->GetPixel( x, y - 1 ), grf ) )
        &&   ( cdMask.GetDirectPixel( x, y - 1 ) != 255 )
        &&   ( y >= 0 ) )
        {
          bufzeiger++;
          if ( bufzeiger >= 30000 )
          {
            // Buffer-Überlauf !
            MessageBox( NULL, _T( "FILL BufferOverflow!" ), _T( "Fehler!" ), 0 );
            return pImageMask;
          }
          if ( bufzeiger > max )
          {
            max = bufzeiger;
          }
          paintbufferx[bufzeiger] = x;
          paintbuffery[bufzeiger] = y - 1;
          xobenr = x;
          while ( ( xobenr < (int)pCD->Width() - 1 )
          &&      ( CompareColors( pCD->GetPixel( xobenr + 1, y - 1 ), grf ) ) )
          {
            xobenr++;
          }
        }
      }
      if ( x >= xuntenr )
      {
        if ( ( CompareColors( pCD->GetPixel( x, y + 1 ), grf ) )
        &&   ( cdMask.GetDirectPixel( x, y + 1 ) != 255 )
        &&   ( y < pCD->Height() - 1 ) )
        {
          bufzeiger++;
          if ( bufzeiger >= 30000 )
          {
            // Buffer-Überlauf!
            MessageBoxA( NULL, "FILL BufferOverflow!", "Fehler!", 0 );
            return pImageMask;
          }
          if ( bufzeiger > max )
          {
            max = bufzeiger;
          }
          paintbufferx[bufzeiger] = x;
          paintbuffery[bufzeiger] = y + 1;
          xuntenr = x;
          while ( ( xuntenr < pCD->Width() - 1 )
          &&      ( cdMask.GetDirectPixel( xuntenr + 1, y + 1 ) != 255 )
          &&      ( CompareColors( pCD->GetPixel( xuntenr + 1, y + 1 ), grf ) ) )
          {
            xuntenr++;
          }
        }
      }
      if ( ( xuntenr > x )
      &&   ( xobenr > x ) )
      {
        x = xuntenr;
        if ( x > xobenr )
        {
          x = xobenr;
        }
      }
      x++;
    } 
    while ( !( ( !CompareColors( pCD->GetPixel( x, y ), grf ) )
    ||         ( cdMask.GetDirectPixel( x, y ) == 255 )
    ||         ( x > pCD->Width() - 1 ) ) );
    for ( int i = x1; i < x; i++ )
    {
      cdMask.PutDirectPixel( i, y, 255 );
    }
    Bounds.Combine( GR::tRect( x1, y, x - x1, 1 ) );
  } 
  while ( bufzeiger > 0 );
  return pImageMask;
}



void CFunctionFill::ApplyAt( ViewInfo* pViewInfo, int iX, int iY )
{
  CMaskedContextDescriptor*   pCD       = pViewInfo->GetCD();

  GR::Graphic::Image*                 pWorkMask = NULL;

  bool    bMask = false;

  if ( pViewInfo->m_Type == ViewInfo::VI_ALPHA )
  {
    bMask = true;
  }


  GR::tRect   rcBounds;


  switch ( pSettings->GetSetting( "FillMode" ) )
  {
    case CSettings::FILL_RGB:
      {
        pWorkMask = GetFillMask( pCD, iX, iY, rcBounds );
      }
      break;
    case CSettings::FILL_FLOOD:
      {
        pWorkMask = pViewInfo->m_pDocInfo->GetSelectionMask();

        if ( ( pViewInfo->m_pDocInfo->HasSelection() )
        &&   ( pSettings->GetSetting( "FillClipGradient" ) ) )
        {
          rcBounds.Set( pViewInfo->m_pDocInfo->m_Selection.left,
                        pViewInfo->m_pDocInfo->m_Selection.top,
                        pViewInfo->m_pDocInfo->m_Selection.right - pViewInfo->m_pDocInfo->m_Selection.left,
                        pViewInfo->m_pDocInfo->m_Selection.bottom - pViewInfo->m_pDocInfo->m_Selection.top );
        }
        else
        {
          rcBounds.Set( 0, 0, pWorkMask->GetWidth(), pWorkMask->GetHeight() );
        }
      }
      break;
  }

  if ( pWorkMask != NULL )
  {
    pViewInfo->AddUndoRect( 0, 0, pViewInfo->m_pDocInfo->Width() - 1, pViewInfo->m_pDocInfo->Height(), -1, -1 );
    pSettings->brushForeground.ApplyBrushTipOnMask( pViewInfo, pCD, pWorkMask, &rcBounds );
    SafeDelete( pWorkMask );
  }

  delete pCD;
}