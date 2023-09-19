#include "stdafx.h"
#include <math.h>
#include "ViewInfo.h"
#include "DocumentInfo.h"
#include "PainterGfxTools.h"
#include "Settings.h"
#include "TextDialog.h"
#include "BrushTip.h"

#include "PainterFontView.h"

#include "PainterEnums.h"

#include "FunctionFill.h"

#include <Misc/Misc.h>

#include <debug/debugclient.h>

#include <utility>



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


ViewInfo::ViewInfo() :
    m_ZoomFaktor( 100 ),
    m_RedrawFlags( 0 ),
    m_pView( NULL ),
    m_pImageFloatingSelection( NULL ),
    m_pImageFloatingSelectionMask( NULL ),
    m_FloatingSelection( false ),
    m_NeedToReleaseMouseButton( false ),
    m_pDocInfo( NULL ),
    m_Dragging( FALSE ),
    m_Selecting( FALSE ),
    m_DrawFirstSelectingFrame( false ),
    m_pPage( NULL ),
    m_pPalette( NULL ),
    m_Type( VI_INVALID )
{
  unsigned char         ucPalColors[768] = { 0,0,0,0,0,42,0,42,0,0,42,42,42,0,0,42,0,42,42,21,0,42,42,42,21,21,21,21,21,63,21,
                                             63,21,21,63,63,63,21,21,63,21,63,63,63,21,63,63,63,59,59,59,55,55,55,52,52,52,48,
                                             48,48,45,45,45,42,42,42,38,38,38,35,35,35,31,31,31,28,28,28,25,25,25,21,21,21,18,
                                             18,18,14,14,14,11,11,11,8,8,8,63,0,0,59,0,0,56,0,0,53,0,0,50,0,0,47,0,0,44,0,0,41,
                                             0,0,38,0,0,34,0,0,31,0,0,28,0,0,25,0,0,22,0,0,19,0,0,16,0,0,63,54,54,63,46,46,63,
                                             39,39,63,31,31,63,23,23,63,16,16,63,8,8,63,0,0,63,42,23,63,38,16,63,34,8,63,30,0,
                                             57,27,0,51,24,0,45,21,0,39,19,0,63,63,54,63,63,46,63,63,39,63,63,31,63,62,23,63,61,
                                             16,63,61,8,63,61,0,57,54,0,51,49,0,45,43,0,39,39,0,33,33,0,28,27,0,22,21,0,16,16,
                                             0,52,63,23,49,63,16,45,63,8,40,63,0,36,57,0,32,51,0,29,45,0,24,39,0,54,63,54,47,63,
                                             46,39,63,39,32,63,31,24,63,23,16,63,16,8,63,8,0,63,0,0,63,0,0,59,0,0,56,0,0,53,0,
                                             1,50,0,1,47,0,1,44,0,1,41,0,1,38,0,1,34,0,1,31,0,1,28,0,1,25,0,1,22,0,1,19,0,1,16,
                                             0,54,63,63,46,63,63,39,63,63,31,63,62,23,63,63,16,63,63,8,63,63,0,63,63,0,57,57,0,
                                             51,51,0,45,45,0,39,39,0,33,33,0,28,28,0,22,22,0,16,16,23,47,63,16,44,63,8,42,63,0,
                                             39,63,0,35,57,0,31,51,0,27,45,0,23,39,54,54,63,46,47,63,39,39,63,31,32,63,23,24,63,
                                             16,16,63,8,9,63,0,1,63,0,0,63,0,0,59,0,0,56,0,0,53,0,0,50,0,0,47,0,0,44,0,0,41,0,
                                             0,38,0,0,34,0,0,31,0,0,28,0,0,25,0,0,22,0,0,19,0,0,16,60,54,63,57,46,63,54,39,63,
                                             52,31,63,50,23,63,47,16,63,45,8,63,42,0,63,38,0,57,32,0,51,29,0,45,24,0,39,20,0,33,
                                             17,0,28,13,0,22,10,0,16,63,54,63,63,46,63,63,39,63,63,31,63,63,23,63,63,16,63,63,
                                             8,63,63,0,63,56,0,57,50,0,51,45,0,45,39,0,39,33,0,33,27,0,28,22,0,22,16,0,16,63,58,
                                             55,63,56,52,63,54,49,63,53,47,63,51,44,63,49,41,63,47,39,63,46,36,63,44,32,63,41,
                                             28,63,39,24,60,37,23,58,35,22,55,34,21,52,32,20,50,31,19,47,30,18,45,28,17,42,26,
                                             16,40,25,15,39,24,14,36,23,13,34,22,12,32,20,11,29,19,10,27,18,9,23,16,8,21,15,7,
                                             18,14,6,16,12,6,14,11,5,10,8,3,38,38,47,33,33,41,28,28,36,23,23,31,19,19,25,14,14,
                                             20,9,9,15,5,5,10,63,59,9,63,55,7,63,52,6,63,48,5,63,45,3,63,41,2,63,38,1,63,35,0,
                                             57,31,0,52,27,0,46,23,0,41,20,0,36,16,0,30,12,0,25,8,0,20,5,0,63,35,0,63,29,0,63,
                                             23,0,63,17,0,63,11,0,63,5,0,63,0,0,0,0,0 };

  m_pDefaultPalette = new GR::Graphic::Palette();
  m_pAlphaPalette = new GR::Graphic::Palette( GR::Graphic::Palette::AlphaPalette() );
  for ( int i = 0; i < 256; i++ )
  {
    m_pDefaultPalette->SetColor( i, ucPalColors[i * 3] << 2, ucPalColors[i * 3 + 1] << 2, ucPalColors[i * 3 + 2] << 2 );
  }
  m_pPage = NULL;

  m_ptScrollOffset.x = 0;
  m_ptScrollOffset.y = 0;
}



ViewInfo::~ViewInfo()
{
  SafeDelete( m_pImageFloatingSelection );
  SafeDelete( m_pImageFloatingSelectionMask );
  SafeDelete( m_pPage );
  SafeDelete( m_pPalette );
  SafeDelete( m_pAlphaPalette );
  SafeDelete( m_pDefaultPalette );
}



void ViewInfo::InitializePage( eViewInfoType vNewType )
{
  SafeDelete( m_pPage );
  SafeDelete( m_pPalette );

  if ( vNewType != VI_DEFAULT )
  {
    m_Type = vNewType;
  }
  switch ( m_Type )
  {
    case VI_RGB:
      m_pPage = new GR::Graphic::GDIPage();
      m_pPage->Create( m_pView->GetParentFrame()->GetSafeHwnd(), m_pDocInfo->Width(), m_pDocInfo->Height(), m_pDocInfo->m_BitDepth );
      m_pDocInfo->m_BitDepth = m_pPage->GetDepth();

      if ( m_pDocInfo->m_BitDepth <= 8 )
      {
        if ( ( m_pDocInfo == NULL )
        ||   ( m_pDocInfo->GetPalette( m_pDocInfo->CurrentFrame() ) == NULL ) )
        {
          m_pPage->SetPalette( m_pDefaultPalette );
        }
        else
        {
          m_pPage->SetPalette( m_pDocInfo->GetPalette( m_pDocInfo->CurrentFrame() ) );
        }
        m_pPalette = new GR::Graphic::Palette( 1 << m_pDocInfo->m_BitDepth );
        m_pPage->GetPalette( m_pPalette );
      }
      break;
    case VI_ALPHA:
      m_pPage = new GR::Graphic::GDIPage();
      m_pPage->Create( m_pView->GetParentFrame()->GetSafeHwnd(), m_pDocInfo->Width(), m_pDocInfo->Height(), 8 );

      m_pPalette = new GR::Graphic::Palette( *m_pAlphaPalette );
      m_pPage->SetPalette( m_pPalette );
      break;
  }

  RedrawView();

}



void ViewInfo::AdjustWindowSize( int iCenteredX, int iCenteredY )
{

  RECT        rc;
  int         iWidth,
              iHeight;


  m_pView->SetScrollSizes( MM_TEXT, CSize( ( m_pDocInfo->Width() * m_ZoomFaktor ) / 100, ( m_pDocInfo->Height() * m_ZoomFaktor ) / 100 ) );
  m_pView->GetClientRect( &rc );
  if ( !( GetWindowLong( m_pView->GetParent()->GetSafeHwnd(), GWL_STYLE ) & WS_MAXIMIZE ) )
  {
    if ( m_pDocInfo->m_DocType == DT_FONT )
    {
      GR::Graphic::Image*    pImage = m_pDocInfo->GetImage( m_pDocInfo->CurrentFrame(), 0 );

      if ( ( (int)pImage->GetWidth() * m_ZoomFaktor / 100 < rc.right - rc.left )
      &&   ( (int)pImage->GetHeight() * m_ZoomFaktor / 100 < rc.bottom - rc.top ) )
      {
        iWidth = 200 + pImage->GetWidth() * m_ZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CXSIZEFRAME ) + 2 * GetSystemMetrics( SM_CXEDGE );
        if ( iWidth < 400 )
        {
          iWidth = 400;
        }
        iHeight = pImage->GetHeight() * m_ZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CYSIZEFRAME ) + 2 * GetSystemMetrics( SM_CYEDGE ) + GetSystemMetrics( SM_CYCAPTION );
        if ( iHeight < 200 )
        {
          iHeight = 200;
        }
        m_pView->GetParentFrame()->SetWindowPos( &CWnd::wndTop, 0, 0, iWidth, iHeight, SWP_NOMOVE );
      }
    }
    else
    {
      iWidth = m_pDocInfo->Width() * m_ZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CXSIZEFRAME ) + 2 * GetSystemMetrics( SM_CXEDGE );
      iHeight = m_pDocInfo->Height() * m_ZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CYSIZEFRAME ) + 2 * GetSystemMetrics( SM_CYEDGE ) + GetSystemMetrics( SM_CYCAPTION );
      if ( m_pDocInfo->m_DocType == DT_FONT )
      {
        iWidth += 200;
      }

      iWidth += 2 * 4;
      iHeight += 2 * 4;

      bool      bCanZoom = true;

      RECT    rcMDIFrame;

      // HIER IST RICHTIG!!
      m_pView->GetParentFrame()->GetParent()->GetWindowRect( &rcMDIFrame );

      if ( ( iWidth < rcMDIFrame.right - rcMDIFrame.left )
      &&   ( iHeight < rcMDIFrame.bottom - rcMDIFrame.top ) )
      {
        m_pView->GetParentFrame()->SetWindowPos( &CWnd::wndTop, 0, 0, iWidth, iHeight, SWP_NOMOVE );
      }
      else
      {
        bool    bTooBig = false;

        int     iSmallerWidth = iWidth,
                iSmallerHeight = iHeight;

        if ( iSmallerWidth > iWidth )
        {
          iSmallerHeight = iWidth;
          bTooBig = true;
        }
        if ( iSmallerHeight > iHeight )
        {
          iSmallerHeight = iHeight;
          bTooBig = true;
        }

        if ( iSmallerWidth > rcMDIFrame.right - rcMDIFrame.left )
        {
          iSmallerWidth = rcMDIFrame.right - rcMDIFrame.left;
          bTooBig = true;
        }
        if ( iSmallerHeight > rcMDIFrame.bottom - rcMDIFrame.top )
        {
          iSmallerHeight = rcMDIFrame.bottom - rcMDIFrame.top;
          bTooBig = true;
        }

        if ( bTooBig )
        {
          m_pView->GetParentFrame()->SetWindowPos( &CWnd::wndTop, 0, 0, iSmallerWidth, iSmallerHeight, SWP_NOMOVE );
        }

        if ( iCenteredX != -1 )
        {
          int   iWidth = rcMDIFrame.right - rcMDIFrame.left;
          iWidth = iWidth * 100 / m_ZoomFaktor;

          int   iX = iCenteredX - iWidth / 2;
          if ( iX < 0 )
          {
            iX = 0;
          }

          int   iHeight = rcMDIFrame.bottom - rcMDIFrame.top;
          iHeight = iHeight * 100 / m_ZoomFaktor;

          int   iY = iCenteredY - iHeight / 2;
          if ( iY < 0 )
          {
            iY = 0;
          }

          m_pView->SetScrollPos( SB_HORZ, iX * m_ZoomFaktor / 100, TRUE );
          m_pView->SetScrollPos( SB_VERT, iY * m_ZoomFaktor / 100, TRUE );
        }
      }
    }
  }

}



void ViewInfo::AddUndoRect( int iX1, int iY1, int iX2, int iY2, int iFrame, int iLayer, BOOL bStartNewUndoGroup )
{
  if ( m_Type == VI_ALPHA )
  {
    m_pDocInfo->AddUndoRect( iX1, iY1, iX2, iY2, iFrame, iLayer, true, bStartNewUndoGroup );
  }
  else
  {
    m_pDocInfo->AddUndoRect( iX1, iY1, iX2, iY2, iFrame, iLayer, false, bStartNewUndoGroup );
  }
}



void ViewInfo::DrawFloatingSelection( CLayer* pLayer, GR::Graphic::ContextDescriptor& cdTarget, int iX, int iY, bool bInsert )
{
  if ( !m_FloatingSelection )
  {
    return;
  }

  GR::u32 dwTransparentColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 );


  if ( m_pImageFloatingSelectionMask )
  {
    GR::Graphic::ContextDescriptor    cdFloating( m_pImageFloatingSelection, m_pPalette );
    GR::Graphic::ContextDescriptor    cdFloatingMask( m_pImageFloatingSelectionMask );

    if ( ( bInsert )
    &&   ( pLayer->m_HasMask ) )
    {
      // wenn es eine Alpha-Maske im Layer gibt, kommt Alpha da rein
      GR::Graphic::ContextDescriptor      cdAlphaMask( pLayer->GetMask(), &GR::Graphic::Palette::AlphaPalette() );

      cdFloating.CopyAreaAlpha( 
              0, 0, 
              cdFloating.Width(), cdFloating.Height(),
              m_pDocInfo->m_ActX, m_pDocInfo->m_ActY,
              pLayer->m_Alpha, 
              &cdTarget );
      cdFloatingMask.CopyArea(
              0, 0, 
              cdFloating.Width(), cdFloating.Height(),
              m_pDocInfo->m_ActX, m_pDocInfo->m_ActY,
              &cdAlphaMask );
    }
    else
    {
      cdFloating.CopyAreaAlphaAndMask(
              0, 0, 
              cdFloating.Width(), cdFloating.Height(),
              m_pDocInfo->m_ActX, m_pDocInfo->m_ActY,
              pLayer->m_Alpha, &cdFloatingMask,
              &cdTarget );
    }
  }
  else if ( m_pDocInfo->m_SelectionFlags & CSettings::FLOAT_TRANSPARENT )
  {

    GR::Graphic::ContextDescriptor    cdFloating( m_pImageFloatingSelection, m_pPalette );

    cdFloating.CopyAreaAlphaTransparent( 0, 0, cdFloating.Width(), cdFloating.Height(),
                          m_pDocInfo->m_ActX, m_pDocInfo->m_ActY,
                          pLayer->m_Alpha,
                          pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 ),
                          &cdTarget );
  }
  else
  {
    GR::Graphic::ContextDescriptor    cdFloating( m_pImageFloatingSelection, m_pPalette );

    cdFloating.CopyAreaAlpha( 0, 0, cdFloating.Width(), cdFloating.Height(),
                              m_pDocInfo->m_ActX, m_pDocInfo->m_ActY,
                              pLayer->m_Alpha,
                              &cdTarget );
  }

}




/*-DrawSelectionFrame---------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void ViewInfo::DrawSelectionFrame( HDC hdc )
{

  if ( !m_pDocInfo )
  {
    return;
  }

  CDC   *pDC = new CDC();
  
  pDC->Attach( hdc );


  if ( pSettings->m_dwFunction == CSettings::F_LINE )
  {
    // eine Selektions-Linie!
    // Das Selecting-Rechteck
    if ( m_Selecting )
    {
      // der User zieht gerade einen Rahmen hoch
      GR::tRect   rcNew;

      SIZE    siz;

      siz.cx = 1;
      siz.cy = 1;

      rcNew.Left =  m_pDocInfo->m_StartX;
      rcNew.Top =   m_pDocInfo->m_StartY;
      rcNew.Right = m_pDocInfo->m_ActX;
      rcNew.Bottom = m_pDocInfo->m_ActY;

      if ( rcNew.Left < 0 )
      {
        rcNew.Left = 0;
      }
      if ( rcNew.Top < 0 )
      {
        rcNew.Top = 0;
      }
      if ( rcNew.Right > (int)m_pDocInfo->Width() )
      {
        rcNew.Right = m_pDocInfo->Width();
      }
      if ( rcNew.Bottom > (int)m_pDocInfo->Height() )
      {
        rcNew.Bottom = m_pDocInfo->Height();
      }

      /*
      if ( m_pDocInfo->m_StartX > m_pDocInfo->m_ActX )
      {
        // die linke Kante ist rechts
        rcNew.right   = m_pDocInfo->m_StartX + 1;
        rcNew.left    = m_pDocInfo->m_ActX;
      }
      if ( m_pDocInfo->m_StartY > m_pDocInfo->m_ActY )
      {
        // die obere Kante ist unten
        rcNew.bottom  = m_pDocInfo->m_StartY + 1;
        rcNew.top     = m_pDocInfo->m_ActY;
      }
      */

      rcNew.Left *= m_ZoomFaktor / 100;
      rcNew.Left += m_ZoomFaktor / 200;
      rcNew.Top *= m_ZoomFaktor / 100;
      rcNew.Top += m_ZoomFaktor / 200;
      rcNew.Right *= m_ZoomFaktor / 100;
      rcNew.Right += m_ZoomFaktor / 200;
      rcNew.Bottom *= m_ZoomFaktor / 100;
      rcNew.Bottom += m_ZoomFaktor / 200;


      //OffsetRect( &rcNew, -m_ptScrollOffset.x, -m_ptScrollOffset.y );

      if ( m_DrawFirstSelectingFrame )
      {
        HPEN    hPen = CreatePen( PS_SOLID, 1, 0xff00ff ),
                hOldPen;

        int iOldMode = pDC->SetROP2( R2_XORPEN );

        // kein LastRect
        hOldPen = (HPEN)pDC->SelectObject( hPen );
        pDC->MoveTo( rcNew.Left, rcNew.Top );
        pDC->LineTo( rcNew.Right, rcNew.Bottom );

        pDC->SetROP2( iOldMode );
        pDC->SelectObject( hOldPen );

        DeleteObject( hPen );
        m_DrawFirstSelectingFrame = false;
      } 
      else
      {
        HPEN    hPen = CreatePen( PS_SOLID, 1, 0xff00ff ),
                hOldPen;

        int iOldMode = pDC->SetROP2( R2_XORPEN );

        hOldPen = (HPEN)pDC->SelectObject( hPen );

        // alte Linie entfernen
        pDC->MoveTo( m_LastSelectingFrame.Left, m_LastSelectingFrame.Top );
        pDC->LineTo( m_LastSelectingFrame.Right, m_LastSelectingFrame.Bottom );

        // neue Linie zeichnen
        pDC->MoveTo( rcNew.Left, rcNew.Top );
        pDC->LineTo( rcNew.Right, rcNew.Bottom );

        pDC->SetROP2( iOldMode );

        pDC->SelectObject( hOldPen );

        DeleteObject( hPen );
      }
      m_LastSelectingFrame = rcNew;
    }
    /*
    else
    {
      m_DrawFirstSelectingFrame = true;
    }
    */
  }
  else if ( ( pSettings->m_dwFunction == CSettings::F_CIRCLE )
  ||        ( pSettings->m_dwFunction == CSettings::F_FILLEDCIRCLE ) )
  {
    // ein Selektions-Kreis
    if ( m_Selecting )
    {
      // der User zieht gerade einen Rahmen hoch
      GR::tRect    rcNew;

      SIZE    siz;

      siz.cx = 1;
      siz.cy = 1;

      rcNew.Left =  m_pDocInfo->m_StartX;
      rcNew.Top =   m_pDocInfo->m_StartY;
      rcNew.Right = m_pDocInfo->m_ActX;
      rcNew.Bottom = m_pDocInfo->m_ActY;

      if ( rcNew.Left < 0 )
      {
        rcNew.Left = 0;
      }
      if ( rcNew.Top < 0 )
      {
        rcNew.Top = 0;
      }
      if ( rcNew.Right > (int)m_pDocInfo->Width() )
      {
        rcNew.Right = m_pDocInfo->Width();
      }
      if ( rcNew.Bottom > (int)m_pDocInfo->Height() )
      {
        rcNew.Bottom = m_pDocInfo->Height();
      }

      rcNew.Left *= m_ZoomFaktor / 100;
      rcNew.Left += m_ZoomFaktor / 200;
      rcNew.Top *= m_ZoomFaktor / 100;
      rcNew.Top += m_ZoomFaktor / 200;
      rcNew.Right *= m_ZoomFaktor / 100;
      rcNew.Right += m_ZoomFaktor / 200;
      rcNew.Bottom *= m_ZoomFaktor / 100;
      rcNew.Bottom += m_ZoomFaktor / 200;

      //OffsetRect( &rcNew, -m_ptScrollOffset.x, -m_ptScrollOffset.y );

      if ( m_DrawFirstSelectingFrame )
      {
        HPEN    hPen = CreatePen( PS_SOLID, 1, 0xff00ff ),
                hOldPen;

        int iOldMode = pDC->SetROP2( R2_XORPEN );

        // kein LastRect
        hOldPen = (HPEN)pDC->SelectObject( hPen );

        {
          int         z,
                      e,
                      f,
                      g,
                      h,
                      aq,
                      bq,
                      dx,
                      dy,
                      r,
                      s,
                      t,

                      a = abs( rcNew.Right - rcNew.Left ),
                      b = abs( rcNew.Bottom - rcNew.Top ),

                      x = rcNew.Left,
                      y = rcNew.Top;


          pDC->MoveTo( x + a, y );
          pDC->LineTo( x + a + 1, y + 1 );
          pDC->MoveTo( x - a, y );
          pDC->LineTo( x - a + 1, y + 1 );

          e = x - a;
          g = y;
          f = x + a;
          h = y;
          aq = a * a;
          bq = b * b;
          dx = ( aq << 1 );
          dy = ( bq << 1 );
          r = a * bq;
          s = ( r << 1 );
          t = 0;
          z = a;
          while ( z > 0 )
          {
            if ( r > 0 )
            {
              g++;
              h--;
              t += dx;
              r -= t;
            }
            if ( r <= 0 )
            {
              z--;
              e++;
              f--;
              s -= dy;
              r += s;
            }

            pDC->MoveTo( e, g );
            pDC->LineTo( e + 1, g + 1 );
            pDC->MoveTo( e, h );
            pDC->LineTo( e + 1, h + 1 );
            pDC->MoveTo( f, g );
            pDC->LineTo( f + 1, g + 1 );
            pDC->MoveTo( f, h );
            pDC->LineTo( f + 1, h + 1 );
          }
        }

        pDC->SetROP2( iOldMode );
        pDC->SelectObject( hOldPen );

        DeleteObject( hPen );
        m_DrawFirstSelectingFrame = false;
      } 
      else
      {
        HPEN    hPen = CreatePen( PS_SOLID, 1, 0xff00ff ),
                hOldPen;

        int iOldMode = pDC->SetROP2( R2_XORPEN );

        hOldPen = (HPEN)pDC->SelectObject( hPen );

        // alte Linie entfernen
        {
          int         z,
                      e,
                      f,
                      g,
                      h,
                      aq,
                      bq,
                      dx,
                      dy,
                      r,
                      s,
                      t,

                      a = abs( m_LastSelectingFrame.Right - m_LastSelectingFrame.Left ),
                      b = abs( m_LastSelectingFrame.Bottom - m_LastSelectingFrame.Top ),

                      x = m_LastSelectingFrame.Left,
                      y = m_LastSelectingFrame.Top;


          pDC->MoveTo( x + a, y );
          pDC->LineTo( x + a + 1, y + 1 );
          pDC->MoveTo( x - a, y );
          pDC->LineTo( x - a + 1, y + 1 );

          e = x - a;
          g = y;
          f = x + a;
          h = y;
          aq = a * a;
          bq = b * b;
          dx = ( aq << 1 );
          dy = ( bq << 1 );
          r = a * bq;
          s = ( r << 1 );
          t = 0;
          z = a;
          while ( z > 0 )
          {
            if ( r > 0 )
            {
              g++;
              h--;
              t += dx;
              r -= t;
            }
            if ( r <= 0 )
            {
              z--;
              e++;
              f--;
              s -= dy;
              r += s;
            }

            pDC->MoveTo( e, g );
            pDC->LineTo( e + 1, g + 1 );
            pDC->MoveTo( e, h );
            pDC->LineTo( e + 1, h + 1 );
            pDC->MoveTo( f, g );
            pDC->LineTo( f + 1, g + 1 );
            pDC->MoveTo( f, h );
            pDC->LineTo( f + 1, h + 1 );
          }
        }


        {
          int         z,
                      e,
                      f,
                      g,
                      h,
                      aq,
                      bq,
                      dx,
                      dy,
                      r,
                      s,
                      t,

                      a = abs( rcNew.Right - rcNew.Left ),
                      b = abs( rcNew.Bottom - rcNew.Top ),

                      x = rcNew.Left,
                      y = rcNew.Top;


          pDC->MoveTo( x + a, y );
          pDC->LineTo( x + a + 1, y + 1 );
          pDC->MoveTo( x - a, y );
          pDC->LineTo( x - a + 1, y + 1 );

          e = x - a;
          g = y;
          f = x + a;
          h = y;
          aq = a * a;
          bq = b * b;
          dx = ( aq << 1 );
          dy = ( bq << 1 );
          r = a * bq;
          s = ( r << 1 );
          t = 0;
          z = a;
          while ( z > 0 )
          {
            if ( r > 0 )
            {
              g++;
              h--;
              t += dx;
              r -= t;
            }
            if ( r <= 0 )
            {
              z--;
              e++;
              f--;
              s -= dy;
              r += s;
            }

            pDC->MoveTo( e, g );
            pDC->LineTo( e + 1, g + 1 );
            pDC->MoveTo( e, h );
            pDC->LineTo( e + 1, h + 1 );
            pDC->MoveTo( f, g );
            pDC->LineTo( f + 1, g + 1 );
            pDC->MoveTo( f, h );
            pDC->LineTo( f + 1, h + 1 );
          }
        }

        pDC->SetROP2( iOldMode );

        pDC->SelectObject( hOldPen );

        DeleteObject( hPen );
      }

      m_LastSelectingFrame = rcNew;
    }
    /*
    else
    {
      m_DrawFirstSelectingFrame = true;
    }
    */
  }
  else
  {
    // Das Selecting-Rechteck
    if ( m_Selecting )
    {
      // der User zieht gerade einen Rahmen hoch
      GR::tRect    rcNew;

      SIZE    siz;

      siz.cx = 1;
      siz.cy = 1;

      rcNew.Left =  m_pDocInfo->m_StartX;
      rcNew.Top =   m_pDocInfo->m_StartY;
      rcNew.Right = ( m_pDocInfo->m_ActX + 1 );
      rcNew.Bottom = ( m_pDocInfo->m_ActY + 1 );

      if ( m_pDocInfo->m_StartX > m_pDocInfo->m_ActX )
      {
        // die linke Kante ist rechts
        rcNew.Right   = m_pDocInfo->m_StartX + 1;
        rcNew.Left    = m_pDocInfo->m_ActX;
      }
      if ( m_pDocInfo->m_StartY > m_pDocInfo->m_ActY )
      {
        // die obere Kante ist unten
        rcNew.Bottom  = m_pDocInfo->m_StartY + 1;
        rcNew.Top     = m_pDocInfo->m_ActY;
      }

      if ( rcNew.Left < 0 )
      {
        rcNew.Left = 0;
      }
      if ( rcNew.Top < 0 )
      {
        rcNew.Top = 0;
      }
      if ( rcNew.Right > (int)m_pDocInfo->Width() )
      {
        rcNew.Right = m_pDocInfo->Width();
      }
      if ( rcNew.Bottom > (int)m_pDocInfo->Height() )
      {
        rcNew.Bottom = m_pDocInfo->Height();
      }


      rcNew.Left *= m_ZoomFaktor / 100;
      rcNew.Top *= m_ZoomFaktor / 100;
      rcNew.Right *= m_ZoomFaktor / 100;
      rcNew.Bottom *= m_ZoomFaktor / 100;

      rcNew.offset( -m_ptScrollOffset.x, -m_ptScrollOffset.y );

      RECT  rectNew;
      SetRect( &rectNew, rcNew.Left, rcNew.Top, rcNew.Right, rcNew.Bottom );

      if ( m_DrawFirstSelectingFrame )
      {
        // kein LastRect
        pDC->DrawDragRect( &rectNew, siz, NULL, siz );
        m_DrawFirstSelectingFrame = false;
      } 
      else
      {
        RECT  rectLast;
        SetRect( &rectLast, m_LastSelectingFrame.Left, m_LastSelectingFrame.Top, m_LastSelectingFrame.Right, m_LastSelectingFrame.Bottom );
        pDC->DrawDragRect( &rectNew, siz, &rectLast, siz );
      }
      m_LastSelectingFrame = rcNew;
    }
    else
    {
      m_DrawFirstSelectingFrame = false;
    }
  }

  if ( m_pDocInfo->HasSelection() )
  {
    int iValue = ( GetTickCount() & 0xc00 ) >> 10;

    GR::u32   dwDotColor[4] = { 0xffffff, 0, 0xffffff, 0xffffff };

    while ( iValue )
    {
      GR::u32 dwTemp = dwDotColor[0];
      memcpy( dwDotColor, &dwDotColor[1], 3 * sizeof( GR::u32 ) );
      dwDotColor[3] = dwTemp;
      iValue--;
    }

    int   iFaktor = m_ZoomFaktor / 100;

    CPen    aPen[4],
            *pOldPen;

    for ( int i = 0; i < 4;i++ )
    {
      aPen[i].CreatePen( PS_DOT, 0, dwDotColor[i] );
    }

    pDC->SetBkColor( 0 );

    pOldPen = pDC->SelectObject( &aPen[0] );

    for ( size_t iAnt = 0; iAnt < m_pDocInfo->m_MarchingAntCount; iAnt++ )
    {
      CMarchingAntDot &dot = m_pDocInfo->m_MarchingAnts[iAnt];

      pDC->SelectObject( &aPen[dot.ucDotType & 0x07] );

      if ( dot.ucDotType & 0x10 )
      {
        // Linie oben
        pDC->MoveTo( iFaktor * dot.pt.x, iFaktor * dot.pt.y );
        pDC->LineTo( iFaktor * dot.pt.x + iFaktor, iFaktor * dot.pt.y );
      }
      if ( dot.ucDotType & 0x20 )
      {
        // Linie unten
        pDC->MoveTo( iFaktor * dot.pt.x, iFaktor * dot.pt.y + iFaktor - 1 );
        pDC->LineTo( iFaktor * dot.pt.x + iFaktor, iFaktor * dot.pt.y + iFaktor - 1 );
      }
      if ( dot.ucDotType & 0x40 )
      {
        // Linie links
        pDC->MoveTo( iFaktor * dot.pt.x, iFaktor * dot.pt.y );
        pDC->LineTo( iFaktor * dot.pt.x, iFaktor * dot.pt.y + iFaktor );
      }
      if ( dot.ucDotType & 0x80 )
      {
        // Linie rechts
        pDC->MoveTo( iFaktor * dot.pt.x + iFaktor - 1, iFaktor * dot.pt.y );
        pDC->LineTo( iFaktor * dot.pt.x + iFaktor - 1, iFaktor * dot.pt.y + iFaktor );
      }
    }

    pDC->SelectObject( pOldPen );
  }

  pDC->Detach();
  delete pDC;

}




CMaskedContextDescriptor* ViewInfo::GetCD()
{

  CMaskedContextDescriptor  *pCD;

  GR::Graphic::Image                     *pImage;
  

  CLayer*   pLayer = m_pDocInfo->ActiveLayer();

  if ( pLayer == NULL )
  {
    return NULL;
  }
  
  switch ( m_Type )
  {
    case VI_RGB:
      pImage = pLayer->GetImage();
      break;
    case VI_ALPHA:
      pImage = pLayer->GetMask();
      break;
  }

  if ( pImage == NULL )
  {
    return NULL;
  }

  pCD = new CMaskedContextDescriptor( pImage, GetActivePalette() );
  if ( pCD )
  {
    if ( m_pDocInfo->HasSelection() )
    {
      //pCD->m_pMaskData = m_pDocInfo->m_pImageSelection->GetData();
    }
  }
  else
  {
    dh::Log( "GetCD - kein CD!\n" );
  }

  return pCD;

}



void ViewInfo::DoFunction( CMaskedContextDescriptor *pCD, GR::u32 dwFunction, int iX, int iY, int iX2, int iY2 )
{

  bool    bMask = false;

  if ( m_Type == ViewInfo::VI_ALPHA )
  {
    bMask = true;
  }

  switch ( dwFunction )
  {
    case CSettings::F_FREEHAND_1:
      m_pDocInfo->AddUndoRect( iX, iY, iX, iY, -1, -1, bMask );
      pSettings->brushForeground.PutBrush( this, pCD, iX, iY, 255 );
      break;
    case CSettings::F_BOX:
      {
        if ( iX > iX2 )
        {
          std::swap( iX, iX2 );
        }
        if ( iY > iY2 )
        {
          std::swap( iY, iY2 );
        }

        GR::Graphic::Image    ImageWorkMask( m_pDocInfo->Width(), m_pDocInfo->Height(), 8 );

        CMaskedContextDescriptor *pCDTemp = new CMaskedContextDescriptor( &ImageWorkMask, &pSettings->m_AlphaPalette );

        pCDTemp->Box( iX, iY, iX2 - iX + 1, iY2 - iY + 1, 0xffffffff );
        delete pCDTemp;

        m_pDocInfo->AddUndoRect( 0, 0, m_pDocInfo->Width() - 1, m_pDocInfo->Height(), -1, -1, bMask );
        
        GR::tRect   rcWork;

        rcWork.Left     = iX;
        rcWork.Top      = iY;
        rcWork.Right    = iX2 + 1;
        rcWork.Bottom   = iY2 + 1;

        pSettings->brushForeground.ApplyBrushTipOnMask( this, pCD, &ImageWorkMask, &rcWork );
        m_pDocInfo->RedrawAllViews( rcWork );
      }
      break;
    case CSettings::F_RECTANGLE:
      {
        if ( iX > iX2 )
        {
          std::swap( iX, iX2 );
        }
        if ( iY > iY2 )
        {
          std::swap( iY, iY2 );
        }

        GR::Graphic::Image *pWorkMask = new GR::Graphic::Image( m_pDocInfo->Width(), m_pDocInfo->Height(), 8 );

        CMaskedContextDescriptor *pCDTemp = new CMaskedContextDescriptor( pWorkMask, &pSettings->m_AlphaPalette );

        pCDTemp->Box( iX, iY, iX2 - iX + 1, 1, 0xffffffff );
        pCDTemp->Box( iX, iY2, iX2 - iX + 1, 1, 0xffffffff );
        for ( int j = iY; j <= iY2; j++ )
        {
          pCDTemp->PutDirectPixel( iX, j, 255 );
          pCDTemp->PutDirectPixel( iX2, j, 255 );
        }
        delete pCDTemp;

        m_pDocInfo->AddUndoRect( 0, 0, m_pDocInfo->Width() - 1, m_pDocInfo->Height(), -1, -1, bMask );

        pSettings->brushForeground.ApplyBrushTipOnMask( this, pCD, pWorkMask );
        SafeDelete( pWorkMask );

        m_pDocInfo->RedrawAllViews();
      }
      break;
    case CSettings::F_LINE:
      {
        GR::Graphic::Image *pWorkMask = new GR::Graphic::Image( m_pDocInfo->Width(), m_pDocInfo->Height(), 8 );

        CMaskedContextDescriptor* pCDWM = new CMaskedContextDescriptor( pWorkMask, &pSettings->m_AlphaPalette );

        if ( pSettings->GetSetting( "LineAA" ) )
        {
          pCDWM->AALine( iX, iY, iX2, iY2, 0xffffffff );
        }
        else
        {
          pCDWM->Line( iX, iY, iX2, iY2, 0xffffffff );
        }

        SafeDelete( pCDWM );

        m_pDocInfo->AddUndoRect( 0, 0, m_pDocInfo->Width() - 1, m_pDocInfo->Height(), -1, -1, bMask );

        pSettings->brushForeground.ApplyBrushTipOnMask( this, pCD, pWorkMask );
        SafeDelete( pWorkMask );
      }
      break;
    case CSettings::F_CIRCLE:
      {
        GR::Graphic::Image *pWorkMask = new GR::Graphic::Image( m_pDocInfo->Width(), m_pDocInfo->Height(), 8 );

        CMaskedContextDescriptor* pCDWM = new CMaskedContextDescriptor( pWorkMask, &pSettings->m_AlphaPalette );

        pCDWM->Ellipse( iX, iY, iX2, iY2, 0xffffffff );

        SafeDelete( pCDWM );

        m_pDocInfo->AddUndoRect( 0, 0, m_pDocInfo->Width() - 1, m_pDocInfo->Height(), -1, -1, bMask );

        pSettings->brushForeground.ApplyBrushTipOnMask( this, pCD, pWorkMask );
        SafeDelete( pWorkMask );
      }
      break;
    case CSettings::F_FILLEDCIRCLE:
      {
        GR::Graphic::Image *pWorkMask = new GR::Graphic::Image( m_pDocInfo->Width(), m_pDocInfo->Height(), 8 );

        CMaskedContextDescriptor* pCDWM = new CMaskedContextDescriptor( pWorkMask, &pSettings->m_AlphaPalette );

        pCDWM->FilledEllipse( iX, iY, iX2, iY2, 0xffffffff );

        SafeDelete( pCDWM );

        m_pDocInfo->AddUndoRect( 0, 0, m_pDocInfo->Width() - 1, m_pDocInfo->Height(), -1, -1, bMask );

        pSettings->brushForeground.ApplyBrushTipOnMask( this, pCD, pWorkMask );
        SafeDelete( pWorkMask );
      }
      break;
    case CSettings::F_FILL:
      {
        if ( ( iX < 0 )
        ||   ( iX >= (int)m_pDocInfo->Width() )
        ||   ( iY < 0 )
        ||   ( iY >= (int)m_pDocInfo->Height() ) )
        {
          return;
        }

        CFunctionFill     FillFunction;

        FillFunction.ApplyAt( this, iX, iY );

      }
      break;
  }

}



void ViewInfo::DoLeftButton( POINT point, int iFlags, BOOL bFirstDown )
{
  if ( m_pDocInfo->m_MButton1Released )
  {
    m_pDocInfo->m_StartX = point.x;
    m_pDocInfo->m_StartY = point.y;
    m_pDocInfo->m_ActX = m_pDocInfo->m_StartX;
    m_pDocInfo->m_ActY = m_pDocInfo->m_StartY;
  }

  bool          bMask   = false;

  if ( m_Type == VI_ALPHA )
  {
    bMask = true;
  }

  if ( m_FloatingSelection )
  {
    // Floating Selection -> einsetzen
    GR::Graphic::Image*    pActiveTarget = NULL;

    CLayer*       pLayer  = m_pDocInfo->ActiveLayer();
    if ( pLayer == NULL )
    {
      return;
    }

    if ( m_Type == VI_RGB )
    {
      pActiveTarget = pLayer->GetImage();
    }
    else if ( m_Type == VI_ALPHA )
    {
      pActiveTarget = pLayer->GetMask();
    }
    else
    {
      return;
    }
    if ( pActiveTarget == NULL )
    {
      return;
    }

    m_pDocInfo->AddUndoRect( m_pDocInfo->m_ActX, 
                             m_pDocInfo->m_ActY, 
                             m_pDocInfo->m_ActX + m_pImageFloatingSelection->GetWidth(), 
                             m_pDocInfo->m_ActY + m_pImageFloatingSelection->GetHeight(),
                             -1, -1, 
                             bMask );


    GR::Graphic::ContextDescriptor    cdTarget( pActiveTarget, m_pPalette );

    DrawFloatingSelection( pLayer, cdTarget, m_pDocInfo->m_ActX, m_pDocInfo->m_ActY, true );

    SafeDelete( m_pImageFloatingSelection );
    SafeDelete( m_pImageFloatingSelectionMask );
    m_FloatingSelection = FALSE;
    m_NeedToReleaseMouseButton = true;

    m_pDocInfo->EmptySelection();

    m_pDocInfo->SetModify( TRUE );
    m_pDocInfo->m_MButton1Released = FALSE;

    m_pDocInfo->m_pDoc->UpdateAllViews( NULL );
    m_pDocInfo->RedrawAllViews();
    return;
  }


  if ( ( pSettings->m_dwFunction == CSettings::F_ZOOM )
  &&   ( bFirstDown ) )
  {
    // Zoom-In
    if ( m_ZoomFaktor < 1600 )
    {
      CPoint  ptOffset = m_pView->GetScrollPosition();

      RECT    rc;

      m_pView->GetParentFrame()->GetWindowRect( &rc );

      int   iCenterX = point.x;
    
      int   iCenterY = point.y;

      m_ZoomFaktor *= 2;

      // Fenster anpassen
      AdjustWindowSize( iCenterX, iCenterY );
      RedrawView();
      m_pDocInfo->m_MButton1Released = FALSE;
      m_NeedToReleaseMouseButton = true;
    }
    return;
  }

  if ( m_NeedToReleaseMouseButton )
  {
    return;
  }

  CMaskedContextDescriptor* pCD = GetCD();
  if ( pCD == NULL )
  {
    return;
  }

  switch ( pSettings->m_dwFunction )
  {
    case CSettings::F_SELECTION:
    case CSettings::F_BOX:
    case CSettings::F_RECTANGLE:
    case CSettings::F_LINE:
    case CSettings::F_FILLEDCIRCLE:
    case CSettings::F_CIRCLE:
      {
        if ( bFirstDown )
        {
          if ( !m_Selecting )
          {
            m_Selecting = TRUE;
            m_DrawFirstSelectingFrame = true;
          }
        }
        if ( m_Selecting )
        {
          m_pDocInfo->m_pDoc->UpdateAllViews( NULL, REDRAW_SELECTION );
        }
      }
      break;
      /*
    case CSettings::F_BRIGHTEN:
      if ( ( ( theApp.stOptions.bBrightenWaitButton )
      &&     ( pDocInfo->bMButton1Released ) )
      ||   ( !( theApp.stOptions.bBrightenWaitButton ) ) )
      {
        pDocInfo->AddUndoRect( pDocInfo->m_ActX, pDocInfo->m_ActY, pDocInfo->m_ActX, pDocInfo->m_ActY );

        BrightenPixel( pDocInfo->GetActiveImage(), pDocInfo->pPalette, pDocInfo->m_ActX, pDocInfo->m_ActY );
        InvalidateRect( hwndUpdate, NULL, FALSE );
        pDocInfo->SetModify( TRUE );
      }
      pDocInfo->bMButton1Released = FALSE;
      break;
    case CSettings::F_DARKEN:
      if ( ( ( theApp.stOptions.bDarkenWaitButton )
      &&     ( pDocInfo->bMButton1Released ) )
      ||   ( !( theApp.stOptions.bDarkenWaitButton ) ) )
      {
        pDocInfo->AddUndoRect( pDocInfo->m_ActX, pDocInfo->m_ActY, pDocInfo->m_ActX, pDocInfo->m_ActY );

        DarkenPixel( pDocInfo->GetActiveImage(), pDocInfo->pPalette, pDocInfo->m_ActX, pDocInfo->m_ActY );
        InvalidateRect( hwndUpdate, NULL, FALSE );
        pDocInfo->SetModify( TRUE );
      }
      pDocInfo->bMButton1Released = FALSE;
      break;
      */
    case CSettings::F_SELECTION_COLOR:
      {
        CFunctionFill   FunctionFill;

        GR::Graphic::Image*     pImageWork = m_pDocInfo->GetActiveImage();

        CMaskedContextDescriptor    cdmTemp( pImageWork, m_pDocInfo->GetPalette( m_pDocInfo->CurrentFrame() ) );

        GR::tRect   rcBounds;

        GR::Graphic::Image*   pImageMask = FunctionFill.GetFillMask( 
                              &cdmTemp, 
                              m_pDocInfo->m_ActX, 
                              m_pDocInfo->m_ActY, 
                              rcBounds );
        if ( pImageMask )
        {
          m_pDocInfo->EmptySelection();

          CMaskedContextDescriptor    cdSelection( m_pDocInfo->m_pImageSelection, &pSettings->m_AlphaPalette );

          GR::Graphic::ContextDescriptor          cdNewMask( pImageMask, &pSettings->m_AlphaPalette );

          cdNewMask.CopyArea( 0, 0, cdNewMask.Width(), cdNewMask.Height(), 0, 0, &cdSelection );

          delete pImageMask;

          m_pDocInfo->HasSelection( true );
          m_pDocInfo->UpdateMarchingAnts();
          ClipCursor( NULL );
        }
      }
      break;
    case CSettings::F_FREEHAND_1:
      {
        int   iW = pSettings->m_pCurrentBrushTip->m_ImageTipMask.GetWidth() / 2;
        int   iH = pSettings->m_pCurrentBrushTip->m_ImageTipMask.GetHeight() / 2;

        m_pDocInfo->AddUndoRect( m_pDocInfo->m_ActX - iW, m_pDocInfo->m_ActY - iH, m_pDocInfo->m_ActX + iW, m_pDocInfo->m_ActY + iH, -1, -1, bMask );

        GR::Graphic::Image* pWorkMask = new GR::Graphic::Image( m_pDocInfo->Width(), m_pDocInfo->Height(), 8 );
        
        CMaskedContextDescriptor* pCDWM = new CMaskedContextDescriptor( pWorkMask, &pSettings->m_AlphaPalette );

        pCDWM->PutDirectPixel( m_pDocInfo->m_ActX, m_pDocInfo->m_ActY, 0xffffffff );

        SafeDelete( pCDWM );

        GR::tRect    rcWorkSize;

        rcWorkSize.Left   = m_pDocInfo->m_ActX;
        rcWorkSize.Right  = rcWorkSize.Left + 1;
        rcWorkSize.Top    = m_pDocInfo->m_ActY;
        rcWorkSize.Bottom = rcWorkSize.Top + 1;

        pSettings->brushForeground.ApplyBrushTipOnMask( this, pCD, pWorkMask, &rcWorkSize );

        delete pWorkMask;

        m_pDocInfo->m_StartX = m_pDocInfo->m_ActX;
        m_pDocInfo->m_StartY = m_pDocInfo->m_ActY;

        rcWorkSize.Left -= iW;
        rcWorkSize.Top -= iH;
        rcWorkSize.Right += iW;
        rcWorkSize.Bottom += iH;

        m_pDocInfo->RedrawAllViews( rcWorkSize );
        m_pDocInfo->SetModify( TRUE );
      }
      break;
    case CSettings::F_FREEHAND_2:
      {
        int   iW = pSettings->m_pCurrentBrushTip->m_ImageTipMask.GetWidth() / 2;
        int   iH = pSettings->m_pCurrentBrushTip->m_ImageTipMask.GetHeight() / 2;
        if ( bFirstDown )
        {
          m_pDocInfo->AddUndoRect( m_pDocInfo->m_ActX - iW, m_pDocInfo->m_ActY - iH, m_pDocInfo->m_ActX + iW, m_pDocInfo->m_ActY + iH, -1, -1, bMask );
        }

        GR::Graphic::Image* pWorkMask = new GR::Graphic::Image( m_pDocInfo->Width(), m_pDocInfo->Height(), 8 );
        
        CMaskedContextDescriptor* pCDWM = new CMaskedContextDescriptor( pWorkMask, &pSettings->m_AlphaPalette );

        if ( pSettings->GetSetting( "LineAA" ) )
        {
          pCDWM->AALine( m_pDocInfo->m_StartX, 
                         m_pDocInfo->m_StartY, 
                         m_pDocInfo->m_ActX, 
                         m_pDocInfo->m_ActY, 
                         0xffffffff );
        }
        else
        {
          pCDWM->Line( m_pDocInfo->m_StartX, 
                      m_pDocInfo->m_StartY, 
                      m_pDocInfo->m_ActX, 
                      m_pDocInfo->m_ActY, 
                      0xffffffff );
        }

        SafeDelete( pCDWM );

        GR::tRect    rcWorkSize;

        if ( m_pDocInfo->m_StartX <= m_pDocInfo->m_ActX )
        {
          rcWorkSize.Left   = m_pDocInfo->m_StartX;
          rcWorkSize.Right  = m_pDocInfo->m_ActX;
        }
        else
        {
          rcWorkSize.Left   = m_pDocInfo->m_ActX;
          rcWorkSize.Right  = m_pDocInfo->m_StartX;
        }
        if ( m_pDocInfo->m_StartY <= m_pDocInfo->m_ActY )
        {
          rcWorkSize.Top    = m_pDocInfo->m_StartY;
          rcWorkSize.Bottom = m_pDocInfo->m_ActY;
        }
        else
        {
          rcWorkSize.Top    = m_pDocInfo->m_ActY;
          rcWorkSize.Bottom = m_pDocInfo->m_StartY;
        }
        rcWorkSize.Right++;
        rcWorkSize.Bottom++;

        rcWorkSize.Left -= iW;
        rcWorkSize.Top -= iH;
        rcWorkSize.Right += iW;
        rcWorkSize.Bottom += iH;

        pSettings->brushForeground.ApplyBrushTipOnMask( this, pCD, pWorkMask, &rcWorkSize );

        delete pWorkMask;

        m_pDocInfo->m_StartX = m_pDocInfo->m_ActX;
        m_pDocInfo->m_StartY = m_pDocInfo->m_ActY;

        m_pDocInfo->RedrawAllViews( rcWorkSize );
        m_pDocInfo->SetModify( TRUE );
      }
      break;
    case CSettings::F_TEXT:
      if ( m_pDocInfo->m_MButton1Released )
      {
        ClipCursor( NULL );


        CTextDialog   dlgText;

        dlgText.pViewInfo = this;

        dlgText.DoModal();
      }
      break;
  }

  delete pCD;
  m_pDocInfo->m_MButton1Released = FALSE;
}



void ViewInfo::DoLeftButtonUp( POINT point, int nFlags )
{
  m_pDocInfo->m_ActX = point.x;
  m_pDocInfo->m_ActY = point.y;

  if ( m_NeedToReleaseMouseButton )
  {
    m_NeedToReleaseMouseButton = false;
    return;
  }

  CMaskedContextDescriptor *pCD = GetCD();

  if ( pCD == NULL )
  {
    return;
  }

  bool      bMask = false;

  GR::tRect      rcRedrawRect( 0, 0, m_pDocInfo->Width(), m_pDocInfo->Height() );

  GR::u32   dwColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR );

  if ( m_Type == VI_ALPHA )
  {
    dwColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_8BIT );
    bMask = true;
  }
  else if ( m_Type == VI_RGB )
  {
    if ( m_pDocInfo->m_BitDepth <= 8 )
    {
      dwColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_8BIT );
    }
  }

  switch ( pSettings->m_dwFunction )
  {
    case CSettings::F_RECTANGLE:
      {
        m_Selecting = FALSE;
        m_DrawFirstSelectingFrame = false;
        DoFunction( pCD, CSettings::F_RECTANGLE, m_pDocInfo->m_StartX, m_pDocInfo->m_StartY, m_pDocInfo->m_ActX, m_pDocInfo->m_ActY );

        GR::tRect   rcChanges( m_pDocInfo->m_StartX, m_pDocInfo->m_StartY,
                               m_pDocInfo->m_ActX - m_pDocInfo->m_StartX + 1, m_pDocInfo->m_ActY - m_pDocInfo->m_StartY + 1 );
        rcChanges.normalize();
        rcRedrawRect = rcChanges;

        m_pDocInfo->SetModify( TRUE );
      }
      break;
    case CSettings::F_BOX:
      {
        m_Selecting = FALSE;
        m_DrawFirstSelectingFrame = false;
        DoFunction( pCD, CSettings::F_BOX, m_pDocInfo->m_StartX, m_pDocInfo->m_StartY, m_pDocInfo->m_ActX, m_pDocInfo->m_ActY );

        GR::tRect   rcChanges( m_pDocInfo->m_StartX, m_pDocInfo->m_StartY,
                               m_pDocInfo->m_ActX - m_pDocInfo->m_StartX + 1, m_pDocInfo->m_ActY - m_pDocInfo->m_StartY + 1 );
        rcChanges.normalize();
        rcRedrawRect = rcChanges;

        m_pDocInfo->SetModify( TRUE );
      }
      break;
    case CSettings::F_SELECTION:
      if ( ( m_pDocInfo->m_MButton1Released )
      &&   ( m_Selecting ) )
      {
        m_Selecting = FALSE;
        m_DrawFirstSelectingFrame = false;

        int   iX1 = m_pDocInfo->m_StartX,
              iX2 = m_pDocInfo->m_ActX,
              iY1 = m_pDocInfo->m_StartY,
              iY2 = m_pDocInfo->m_ActY;

        if ( iX1 > iX2 )
        {
          iX1 = iX2;
          iX2 = m_pDocInfo->m_StartX;
        }
        if ( iY1 > iY2 )
        {
          iY1 = iY2;
          iY2 = m_pDocInfo->m_StartY;
        }

        if ( iX1 < 0 )
        {
          iX1 = 0;
        }
        if ( iY1 < 0 )
        {
          iY1 = 0;
        }
        if ( iX2 >= (int)m_pDocInfo->Width() )
        {
          iX2 = m_pDocInfo->Width() - 1;
        }
        if ( iY2 >= (int)m_pDocInfo->Height() )
        {
          iY2 = m_pDocInfo->Height() - 1;
        }

        m_pDocInfo->m_SelectionX = iX1;
        m_pDocInfo->m_SelectionY = iY1;

        m_pDocInfo->m_SelectionWidth = abs( iX2 - iX1 ) + 1;
        m_pDocInfo->m_SelectionHeight = abs( iY2 - iY1 ) + 1;

        if ( ( !( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) )
        &&   ( !( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) ) )
        {
          m_pDocInfo->EmptySelection();
        }

        if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
        {
          m_pDocInfo->RemoveRectSelection( m_pDocInfo->m_SelectionX, 
                                           m_pDocInfo->m_SelectionY,
                                           m_pDocInfo->m_SelectionWidth,
                                           m_pDocInfo->m_SelectionHeight );
        }
        else
        {
          m_pDocInfo->AddRectSelection( m_pDocInfo->m_SelectionX, 
                                        m_pDocInfo->m_SelectionY,
                                        m_pDocInfo->m_SelectionWidth,
                                        m_pDocInfo->m_SelectionHeight );
        }
        m_pDocInfo->ShowAllViews();
      }
      break;
    case CSettings::F_FREEHAND_1:
      m_pDocInfo->AddUndoRect( m_pDocInfo->m_ActX, m_pDocInfo->m_ActY, m_pDocInfo->m_ActX, m_pDocInfo->m_ActY, -1, -1, bMask );

      DoFunction( pCD, CSettings::F_FREEHAND_1, m_pDocInfo->m_ActX, m_pDocInfo->m_ActY );

      rcRedrawRect.set( m_pDocInfo->m_ActX, m_pDocInfo->m_ActY, 1, 1 );

      m_pDocInfo->SetModify( TRUE );
      break;
    case CSettings::F_FILL:
      DoFunction( pCD, CSettings::F_FILL, m_pDocInfo->m_ActX, m_pDocInfo->m_ActY );
      m_pDocInfo->SetModify( TRUE );
      break;
    case CSettings::F_FREEHAND_2:
    case CSettings::F_LINE:
      {
        m_Selecting = FALSE;
        m_DrawFirstSelectingFrame = false;

        if ( pSettings->m_dwFunction == CSettings::F_LINE )
        {
          m_pDocInfo->AddUndoRect( 0, 0, m_pDocInfo->Width() - 1, m_pDocInfo->Height(), -1, -1, bMask );
        }

        GR::Graphic::Image  ImageWorkMask( m_pDocInfo->Width(), m_pDocInfo->Height(), 8 );
        
        CMaskedContextDescriptor* pCDWM = new CMaskedContextDescriptor( &ImageWorkMask, &pSettings->m_AlphaPalette );

        if ( pSettings->GetSetting( "LineAA" ) )
        {
          pCDWM->AALine( m_pDocInfo->m_StartX, 
                         m_pDocInfo->m_StartY, 
                         m_pDocInfo->m_ActX, 
                         m_pDocInfo->m_ActY, 
                         0xffffffff );
        }
        else
        {
          pCDWM->Line( m_pDocInfo->m_StartX, 
                      m_pDocInfo->m_StartY, 
                      m_pDocInfo->m_ActX, 
                      m_pDocInfo->m_ActY, 
                      0xffffffff );
        }

        SafeDelete( pCDWM );

        GR::tRect    rcWorkSize;

        if ( m_pDocInfo->m_StartX <= m_pDocInfo->m_ActX )
        {
          rcWorkSize.Left   = m_pDocInfo->m_StartX;
          rcWorkSize.Right  = m_pDocInfo->m_ActX;
        }
        else
        {
          rcWorkSize.Left   = m_pDocInfo->m_ActX;
          rcWorkSize.Right  = m_pDocInfo->m_StartX;
        }
        if ( m_pDocInfo->m_StartY <= m_pDocInfo->m_ActY )
        {
          rcWorkSize.Top    = m_pDocInfo->m_StartY;
          rcWorkSize.Bottom = m_pDocInfo->m_ActY;
        }
        else
        {
          rcWorkSize.Top    = m_pDocInfo->m_ActY;
          rcWorkSize.Bottom = m_pDocInfo->m_StartY;
        }
        rcWorkSize.Right++;
        rcWorkSize.Bottom++;

        pSettings->brushForeground.ApplyBrushTipOnMask( this, pCD, &ImageWorkMask, &rcWorkSize );

        m_pDocInfo->m_StartX = m_pDocInfo->m_ActX;
        m_pDocInfo->m_StartY = m_pDocInfo->m_ActY;

        rcRedrawRect = rcWorkSize;
        m_pDocInfo->SetModify( TRUE );
      }
      break;
    case CSettings::F_CIRCLE:
      {
        m_Selecting = FALSE;
        m_DrawFirstSelectingFrame = false;
        m_pDocInfo->AddUndoRect( 0, 0, m_pDocInfo->Width() - 1, m_pDocInfo->Height(), -1, -1, bMask );
        DoFunction( pCD, CSettings::F_CIRCLE, 
                    m_pDocInfo->m_StartX, m_pDocInfo->m_StartY, 
                    m_pDocInfo->m_ActX - m_pDocInfo->m_StartX, m_pDocInfo->m_ActY - m_pDocInfo->m_StartY );

        m_pDocInfo->SetModify( TRUE );
      }
      break;
    case CSettings::F_FILLEDCIRCLE:
      {
        m_Selecting = FALSE;
        m_DrawFirstSelectingFrame = false;
        m_pDocInfo->AddUndoRect( 0, 0, m_pDocInfo->Width() - 1, m_pDocInfo->Height(), -1, -1, bMask );
        DoFunction( pCD, CSettings::F_FILLEDCIRCLE, 
                    m_pDocInfo->m_StartX, m_pDocInfo->m_StartY, 
                    m_pDocInfo->m_ActX - m_pDocInfo->m_StartX, m_pDocInfo->m_ActY - m_pDocInfo->m_StartY );
        m_pDocInfo->SetModify( TRUE );
      }
      break;
  }
  SafeDelete( pCD );

  m_pDocInfo->RedrawAllViews( rcRedrawRect );
}



GR::Graphic::Palette* ViewInfo::GetActivePalette() 
{
  if ( m_Type == VI_ALPHA )
  {
    return m_pAlphaPalette;
  }
  else if ( m_pDocInfo->m_BitDepth <= 8 )
  {
    return m_pPalette;
  }
  return NULL;
}



GR::u32 ViewInfo::GetRGBColor( GR::u32 dwOrigIndex )
{

  GR::u32   dwResult = dwOrigIndex;


  switch ( m_Type )
  {
    case VI_RGB:
      if ( m_pDocInfo->m_BitDepth <= 8 )
      {
        dwResult =  ( m_pPalette->Red( dwOrigIndex ) << 16 )
                  + ( m_pPalette->Green( dwOrigIndex ) << 8 )
                  + ( m_pPalette->Blue( dwOrigIndex ) );
      }
      else if ( ( m_pDocInfo->m_BitDepth == 15 )
      ||        ( m_pDocInfo->m_BitDepth == 16 ) )
      {
        dwResult =   ( ( ( ( ( dwResult & 0x7c00 ) >> 10 ) * 255 ) / 31 ) << 16 )
                   + ( ( ( ( ( dwResult & 0x03e0 ) >>  5 ) * 255 ) / 31 ) << 8 )
                   + ( ( ( ( ( dwResult & 0x001f )       ) * 255 ) / 31 ) );
        dwResult |= 0x070707;
      }
      else
      {
        m_pDocInfo->m_BitDepth = (BYTE)dwOrigIndex;
      }
      break;
    case VI_ALPHA:
      dwResult =  ( ( ( m_pAlphaPalette->Red( dwOrigIndex ) * 255 ) / 63 ) << 16 )
                + ( ( ( m_pAlphaPalette->Green( dwOrigIndex ) * 255 ) / 63 ) << 8 )
                + ( ( ( m_pAlphaPalette->Blue( dwOrigIndex ) * 255 ) / 63 ) );
      dwResult |= 0x070707;
      break;
  }

  return dwResult;

}



/*-GetActiveImage-------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::Image *ViewInfo::GetActiveImage()
{

  switch ( m_Type )
  {
    case VI_RGB:
      return m_pDocInfo->GetActiveImage();
    case VI_ALPHA:
      {
        CLayer*   pLayer = m_pDocInfo->ActiveLayer();
        if ( pLayer )
        {
          return pLayer->GetMask();
        }
      }
  }
  return NULL;

}



GR::u32 ViewInfo::GetViewBitDepth()
{
  // BAUSTELLE - aber kann man erstmal so lassen
  if ( m_pPage == NULL )
  {
    return 0;
  }
  return m_pPage->GetDepth();
}



void ViewInfo::CopyToClipboard()
{
  GR::Graphic::Image*   pWorkImage = GetWorkImage();
  if ( pWorkImage == NULL )
  {
    MessageBeep( MB_ICONEXCLAMATION );
    return;
  }

  if ( AfxGetMainWnd()->OpenClipboard() )
  {
    EmptyClipboard();

    // bei einer Selection auch eine Maske erzeugen
    if ( ( m_pDocInfo->HasSelection() )
    ||   ( m_pDocInfo->ActiveLayer()->m_HasMask ) )
    {
      GR::Graphic::Image            ImageTemp( *pWorkImage );

      GR::Graphic::ContextDescriptor    cdTemp( &ImageTemp, GetActivePalette() );

      GR::Graphic::Image            ImageMask( pWorkImage->GetWidth(), pWorkImage->GetHeight(), 8 );

      GR::Graphic::ContextDescriptor    cdMask( &ImageMask, &GR::Graphic::Palette::AlphaPalette() );

      if ( m_pDocInfo->ActiveLayer()->m_HasMask )
      {
        GR::Graphic::Image*   pImageMask = m_pDocInfo->ActiveLayer()->GetMask();

        GR::Graphic::ContextDescriptor    cdLayerMask( pImageMask, &GR::Graphic::Palette::AlphaPalette() );

        if ( m_pDocInfo->HasSelection() )
        {
          cdLayerMask.CopyArea( m_pDocInfo->m_Selection.left, 
                                m_pDocInfo->m_Selection.top,
                                m_pDocInfo->m_Selection.right - m_pDocInfo->m_Selection.left + 1,
                                m_pDocInfo->m_Selection.bottom - m_pDocInfo->m_Selection.top + 1,
                                0, 0,
                                &cdMask );
        }
        else
        {
          cdLayerMask.CopyArea( 0, 0, cdLayerMask.Width(), cdLayerMask.Height(), 0, 0, &cdMask );
        }
      }
      else
      {
        GR::Graphic::ContextDescriptor    cdSelection( m_pDocInfo->m_pImageSelection );

        cdSelection.CopyArea( m_pDocInfo->m_Selection.left, 
                              m_pDocInfo->m_Selection.top,
                              m_pDocInfo->m_Selection.right - m_pDocInfo->m_Selection.left + 1,
                              m_pDocInfo->m_Selection.bottom - m_pDocInfo->m_Selection.top + 1,
                              0, 0,
                              &cdMask );
      }

      size_t   iSize = 4 + 4 + 4 + cdTemp.DataSize() + cdMask.DataSize();

      if ( cdTemp.BitsProPixel() <= 8 )
      {
        iSize += cdTemp.Palette().Entries() * 3;
      }

      HANDLE    hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, iSize );

      BYTE*   pData = (BYTE*)GlobalLock( hGlobal );

      *(GR::u32*)pData = cdTemp.Width();
      *(GR::u32*)(pData + 4 ) = cdTemp.Height();
      *(GR::u32*)(pData + 8 ) = cdTemp.BitsProPixel();

      int   iMaskOffset = 12 + cdTemp.DataSize();

      memcpy( pData + 12, cdTemp.Data(), cdTemp.DataSize() );
      if ( cdTemp.BitsProPixel() <= 8 )
      {
        memcpy( pData + 12 + cdTemp.DataSize(), cdTemp.Palette().Data(), 3 * cdTemp.Palette().Entries() );
        iMaskOffset += 3 * cdTemp.Palette().Entries();
      }
      memcpy( pData + iMaskOffset, cdMask.Data(), cdMask.DataSize() );

      GlobalUnlock( hGlobal );

      if ( SetClipboardData( pSettings->m_dwClipboardFormat, hGlobal ) == NULL )
      {
        dh::Log( CMisc::printf( "error %d", GetLastError() ) );
        MessageBoxA( NULL, "failed to set clipboard data", "mist", MB_OK | MB_APPLMODAL );
        CloseClipboard();
        return;
      }
    }

    HANDLE  hDib = CreateHDIBFromImage( pWorkImage, GetActivePalette() );
    SafeDelete( pWorkImage );
    if ( hDib == NULL )
    {
      CloseClipboard();
      MessageBoxA( NULL, "failed to create hdib", "mist", MB_OK | MB_APPLMODAL );
      return;
    }

    if ( SetClipboardData( CF_DIB, hDib ) == NULL )
    {
      pSettings->Log( CMisc::printf( "error %d", GetLastError() ) );
      MessageBoxA( NULL, "failed to set clipboard data", "mist", MB_OK | MB_APPLMODAL );
      CloseClipboard();
      return;
    }

    CloseClipboard();
  }
  else
  {
    MessageBoxA( AfxGetMainWnd()->GetSafeHwnd(), "failed to open clipboard", "mist", MB_OK | MB_APPLMODAL );
  }

}



GR::Graphic::Image *ViewInfo::GetWorkImage()
{

  GR::Graphic::Image       *pImage,
                  *pActiveImage;


  pActiveImage = GetActiveImage();

  if ( pActiveImage == NULL )
  {
    return NULL;
  }

  if ( !m_pDocInfo->HasSelection() )
  {
    pImage = new GR::Graphic::Image( (WORD)pActiveImage->GetWidth(), 
                            (WORD)pActiveImage->GetHeight(), 
                            pActiveImage->GetDepth(), 0, 0 );

    GR::Graphic::ContextDescriptor    cdImage( pActiveImage );
    GR::Graphic::ContextDescriptor    cdImageTarget( pImage );

    cdImage.CopyArea( 0, 0, cdImage.Width(), cdImage.Height(), 0, 0, &cdImageTarget );
  }
  else
  {
    pImage = new GR::Graphic::Image( m_pDocInfo->m_Selection.right - m_pDocInfo->m_Selection.left + 1,
                            m_pDocInfo->m_Selection.bottom - m_pDocInfo->m_Selection.top + 1,
                            m_pPage->GetDepth(), 0, 0 );

    CMaskedContextDescriptor  *pCDOrigImage = new CMaskedContextDescriptor( pActiveImage, m_pPalette ),
                        *pCDImage     = new CMaskedContextDescriptor( pImage, m_pPalette ),
                        *pCDSel       = new CMaskedContextDescriptor( m_pDocInfo->m_pImageSelection, &pSettings->m_AlphaPalette );

    if ( pImage->GetDepth() <= 8 )
    {
      GR::Graphic::ContextDescriptor    cdImage( pActiveImage );
      GR::Graphic::ContextDescriptor    cdImageTarget( pImage );

      /*
      cdImage.CopyArea( m_pDocInfo->m_Selection.left, m_pDocInfo->m_Selection.top, 
                  pImage->GetWidth(), pImage->GetHeight(), 0, 0, &cdImageTarget );
      */

      for ( int j = 0; j < pCDImage->Height(); j++ )
      {
        for ( int i = 0; i < pCDImage->Width(); i++ )
        {
          if ( pCDSel->GetDirectPixel( m_pDocInfo->m_Selection.left + i, m_pDocInfo->m_Selection.top + j ) )
          {
            pCDImage->PutDirectPixel( i, j, pCDOrigImage->GetDirectPixel( m_pDocInfo->m_Selection.left + i, m_pDocInfo->m_Selection.top + j ) );
          }
        }
      }
    }
    else
    {
      for ( int j = 0; j < pCDImage->Height(); j++ )
      {
        for ( int i = 0; i < pCDImage->Width(); i++ )
        {
          if ( pCDSel->GetDirectPixel( m_pDocInfo->m_Selection.left + i, m_pDocInfo->m_Selection.top + j ) )
          {
            pCDImage->PutPixel( i, j, pCDOrigImage->GetPixel( m_pDocInfo->m_Selection.left + i, m_pDocInfo->m_Selection.top + j ) );
          }
        }
      }
    }
    delete pCDSel;
    delete pCDImage;
    delete pCDOrigImage;
  }
  return pImage;

}



/*-GetWorkCD------------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

GR::Graphic::ContextDescriptor* ViewInfo::GetWorkCD()
{

  GR::Graphic::Image*       pActiveImage;


  pActiveImage = GetActiveImage();

  GR::Graphic::ContextDescriptor      cdDummy( m_pPage, GetActivePalette() );

  GR::Graphic::ContextDescriptor*     pCD = new GR::Graphic::ContextDescriptor();

  if ( !m_pDocInfo->HasSelection() )
  {
    pCD->CreateData( pActiveImage->GetWidth(), pActiveImage->GetHeight(), cdDummy.ImageFormat() );

    GR::Graphic::ContextDescriptor      cdImage( pActiveImage, GetActivePalette() );

    cdImage.CopyArea( 0, 0, cdImage.Width(), cdImage.Height(), 0, 0, pCD );
    pCD->SetPalette( *m_pPalette );
  }
  else
  {
    pCD->CreateData( m_pDocInfo->m_Selection.right - m_pDocInfo->m_Selection.left + 1, 
                     m_pDocInfo->m_Selection.bottom - m_pDocInfo->m_Selection.top + 1, 
                     cdDummy.ImageFormat() );
    pCD->SetPalette( *m_pPalette );

    CMaskedContextDescriptor  *pCDOrigImage = new CMaskedContextDescriptor( pActiveImage, m_pPalette ),
                              *pCDSel       = new CMaskedContextDescriptor( m_pDocInfo->m_pImageSelection, &pSettings->m_AlphaPalette );

    for ( int j = 0; j < pCD->Height(); j++ )
    {
      for ( int i = 0; i < pCD->Width(); i++ )
      {
        if ( pCDSel->GetDirectPixel( m_pDocInfo->m_Selection.left + i, m_pDocInfo->m_Selection.top + j ) )
        {
          pCD->PutPixel( i, j, pCDOrigImage->GetDirectPixel( m_pDocInfo->m_Selection.left + i, m_pDocInfo->m_Selection.top + j ) );
        }
      }
    }
    delete pCDSel;
    delete pCDOrigImage;
  }

  return pCD;

}



void ViewInfo::GetWorkColorFromPixel( int iX, int iY )
{

  CMaskedContextDescriptor  *pCD = NULL;

  GR::Graphic::Image*    pImage = GetActiveImage();

  if ( pImage == NULL )
  {
    dh::Log( "CViewInfo::GetWorkColorFromPixel no active image" );
    return;
  }
  pCD = new CMaskedContextDescriptor( pImage, m_pPalette );

  pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR,     pCD->GetPixel( iX, iY ) );
  if ( pImage->GetDepth() <= 8 )
  {
    pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_8BIT,  pCD->GetDirectPixel( iX, iY ) );
  }
  pSettings->SetSetting( "GetColorDepth", pImage->GetDepth() );

  pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );

  delete pCD;
}



void ViewInfo::RedrawView()
{
  if ( !m_pDocInfo )
  {
    return;
  }
  if ( m_pDocInfo->m_LayeredFrames.empty() )
  {
    return;
  }

  m_RedrawFlags = REDRAW_ALL;

  m_rectRedraw.set( 0, 0, m_pDocInfo->Width(), m_pDocInfo->Height() );

  if ( m_pDocInfo->m_DocType == DT_FONT )
  {
    // BAUSTELLE
    CPainterFontView *pView = (CPainterFontView*)m_pView;

    pView->DoUpdate( m_RedrawFlags );
  }
  else
  {
    m_pView->Invalidate( FALSE );
  }
}



void ViewInfo::DisplayPage()
{
  if ( m_pPage == NULL )
  {
    // kommt beim Initialisieren vor
    return;
  }
  if ( m_pPage->GetDC() == NULL )
  {
    return;
  }

  RECT      rc;

  GR::u32   dwWidth,
            dwHeight;


  m_pView->GetClientRect( &rc );

  dwWidth = rc.right * 100 / m_ZoomFaktor;

  GR::Graphic::Image*    pTopLayerImage = m_pDocInfo->GetImage( m_pDocInfo->CurrentFrame(), 0 );

  if ( m_pDocInfo->m_DocType == DT_FONT )
  {
    if ( rc.right >= 200 )
    {
      dwWidth = ( rc.right - 200 ) * 100 / m_ZoomFaktor;
    }
    else
    {
      dwWidth = 0;
    }
    if ( ( pTopLayerImage )
    &&   ( dwWidth > (GR::u32)pTopLayerImage->GetWidth() ) )
    {
      dwWidth = pTopLayerImage->GetWidth();
    }
  }
  if ( dwWidth > (GR::u32)m_pPage->GetWidth() )
  {
    dwWidth = m_pPage->GetWidth();
  }
  dwHeight = rc.bottom * 100 / m_ZoomFaktor;
  if ( m_pDocInfo->m_DocType == DT_FONT )
  {
    if ( ( pTopLayerImage )
    &&   ( dwHeight > (GR::u32)pTopLayerImage->GetHeight() ) )
    {
      dwHeight = pTopLayerImage->GetHeight();
    }
  }


  if ( m_pPage->GetDepth() <= 8 )
  {
    if ( m_Type == VI_ALPHA )
    {
      m_pPage->SetPalette( m_pAlphaPalette );
    }
    else
    {
      if ( m_pDocInfo->GetPalette( m_pDocInfo->CurrentFrame() ) != NULL )
      {
        m_pPage->SetPalette( m_pDocInfo->GetPalette( m_pDocInfo->CurrentFrame() ) );
      }
    }
  }

  if ( ( m_RedrawFlags == REDRAW_ALL )
  ||   ( m_RedrawFlags == REDRAW_RECT ) )
  {
    // Standard-Karo-Hintergrund
    {
      SetTextColor( m_pPage->GetDC(), 0xa05050 );
      SetBkColor( m_pPage->GetDC(), 0xa0a0a0 );

      RECT    rc;

      SetRect( &rc, m_rectRedraw.Left, m_rectRedraw.Top, m_rectRedraw.Right, m_rectRedraw.Bottom );

      FillRect( m_pPage->GetDC(), &rc, pSettings->m_brushBackground );
    }

    // m_vectLayers
    if ( m_pDocInfo->CurrentFrame() < m_pDocInfo->m_LayeredFrames.size() )
    {
      CLayer *pLayer = NULL;
      for ( size_t i = 0; i < m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount(); i++ )
      {
        pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), i );

        if ( pLayer->m_Visible )
        {
          if ( m_Type == VI_ALPHA )
          {
            if ( pLayer->m_HasMask )
            {
              pLayer->GetMask()->PutImage( m_pPage, 0, 0 );
            }
          }
          else
          {
            pLayer->DrawImage( m_pPage, m_pPalette, m_rectRedraw );
          }

          // nur im aktiven Layer Arbeits-Linien einzeichnen
          if ( i == m_pDocInfo->CurrentLayer() )
          {
            GR::Graphic::ContextDescriptor    cdPage( m_pPage, m_pPalette );

            DrawFloatingSelection( pLayer, cdPage, m_pDocInfo->m_ActX, m_pDocInfo->m_ActY );
          }
        }
      }
    }
  }
  m_rectRedraw.clear();
}



void ViewInfo::Invert()
{
  if ( m_Type != VI_ALPHA )
  {
    m_pDocInfo->InvertColor();
    m_pDocInfo->RedrawAllViews();
    return;
  }

  CLayer*   pLayer = m_pDocInfo->ActiveLayer();

  for ( GR::u32 j = 0; j < m_pDocInfo->Height(); j++ )
  {
    BYTE*   pData = ( (BYTE*)pLayer->GetMask()->GetData() ) + j * m_pDocInfo->Width();

    for ( GR::u32 i = 0; i < m_pDocInfo->Width(); i++ )
    {
      *pData = 255 - *pData;
      pData++;
    }
  }

  m_pDocInfo->RedrawAllViews();
}



GR::u32 ViewInfo::ToLocalColor( CSettings::ColorCategory Color )
{
  if ( GetViewBitDepth() <= 8 )
  {
    return pSettings->GetRGBColor( (CSettings::ColorCategory)( (int)Color + 2 ) );
  }
  return pSettings->GetRGBColor( Color );
}



