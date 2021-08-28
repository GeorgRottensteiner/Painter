// GradientRangeEdit.cpp : implementation file
//

#include "stdafx.h"
#include "painter.h"
#include "GradientRangeEdit.h"

#include <Grafik/GFXHelper.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorRangeEdit

CGradientRangeEdit::CGradientRangeEdit() :
  CRangeEdit()
{

  m_iMinimum  = 0;
  m_iMaximum  = 255;
  m_iStep     = 1;

  SetColors( 0xff000000, 0xffffffff );

}

CGradientRangeEdit::~CGradientRangeEdit()
{
  DeleteObject( m_hbmColorBar );
  CRangeEdit::~CRangeEdit();
}


BEGIN_MESSAGE_MAP(CGradientRangeEdit, CRangeEdit)
	//{{AFX_MSG_MAP(CGradientRangeEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGradientRangeEdit message handlers

void CGradientRangeEdit::SetRange( int iMin, int iMax, int iStep )
{

  m_iMinimum  = 0;
  m_iMaximum  = 255;
  m_iStep     = 1;

}

void CGradientRangeEdit::SetRange( float fMin, float fMax, float fStep, int iVorKomma, int iNachKomma )
{

  m_iMinimum  = 0;
  m_iMaximum  = 255;
  m_iStep     = 1;

}

void CGradientRangeEdit::SetPosition( float fPos )
{
}



COLORREF CGradientRangeEdit::GetColor( int iPos )
{

  DWORD   dwColor   = CGFXHelper::FarbVerlauf( m_dwColorGradient1, m_dwColorGradient2, iPos * 100 / 255 );

  return RGB( ( dwColor & 0xff0000 ) >> 16,
              ( dwColor & 0x00ff00 ) >> 8,
              ( dwColor & 0x0000ff ) );

}


void CGradientRangeEdit::DrawPopup( HDC hdc, const RECT& rcTarget )
{

  HDC   hdcBitmap = CreateCompatibleDC( hdc );

  SelectObject( hdcBitmap, m_hbmColorBar );
  SetStretchBltMode( hdc, COLORONCOLOR );
  StretchBlt( hdc, rcTarget.left, rcTarget.top, rcTarget.right - rcTarget.left, rcTarget.bottom - rcTarget.top,
              hdcBitmap, 0, 0, 256, 1, SRCCOPY );
  SelectObject( hdcBitmap, NULL );

  int iX = 0;
  if ( m_dwMode == 0 )
  {
    iX = rcTarget.left + ( m_iPosition - m_iMinimum ) * ( rcTarget.right - rcTarget.left - 1 ) / ( m_iMaximum - m_iMinimum );
  }
  else if ( m_dwMode == 1 )
  {
    iX = int( rcTarget.left + ( m_fPosition - m_fMinimum ) * ( rcTarget.right - rcTarget.left - 1 ) / ( m_fMaximum - m_fMinimum ) );
  }
  
  RECT    rc = rcTarget;
  rc.left = iX;
  rc.right = iX + 1;
	::SetBkColor( hdc, GetSysColor( COLOR_HIGHLIGHT ) );
	::ExtTextOut( hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

  DeleteDC( hdcBitmap );
  
}



void CGradientRangeEdit::SetColors( DWORD dwColor1, DWORD dwColor2 )
{

  m_dwColorGradient1 = dwColor1;
  m_dwColorGradient2 = dwColor2;

  HWND  hwndDesktop = ::GetDesktopWindow();
  HDC   hdcDesktop = ::GetDC( hwndDesktop );
  HDC   hdc = CreateCompatibleDC( hdcDesktop );
  m_hbmColorBar = CreateCompatibleBitmap( hdcDesktop, 1536, 1 );
  ::ReleaseDC( hwndDesktop, hdcDesktop );

  BITMAP bm;
  GetObject( m_hbmColorBar, sizeof( BITMAP ), &bm );

  SelectObject( hdc, m_hbmColorBar );

  RECT    rcSmall;

  rcSmall.top     = 0;
  rcSmall.bottom  = 1;

  for ( int i = 0; i < 256; ++i )
  {
    ::SetBkColor( hdc, GetColor( i ) );
    rcSmall.left = i;
    rcSmall.right = i + 1;
    ::ExtTextOut( hdc, 0, 0, ETO_OPAQUE, &rcSmall, NULL, 0, NULL);
  }

  DeleteDC( hdc );

}