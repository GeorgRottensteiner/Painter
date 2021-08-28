// ColorRangeEdit.cpp : implementation file
//

#include "stdafx.h"
#include "painter.h"
#include "ColorRangeEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorRangeEdit

CColorRangeEdit::CColorRangeEdit() :
  CRangeEdit()
{

  m_iMinimum  = 0;
  m_iMaximum  = 1536;
  m_iStep     = 1;

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

  for ( int i = 0; i < 1536; ++i )
  {
    ::SetBkColor( hdc, GetColor( i ) );
    rcSmall.left = i;
    rcSmall.right = i + 1;
    ::ExtTextOut( hdc, 0, 0, ETO_OPAQUE, &rcSmall, NULL, 0, NULL);
  }

  DeleteDC( hdc );

}

CColorRangeEdit::~CColorRangeEdit()
{
  DeleteObject( m_hbmColorBar );
  CRangeEdit::~CRangeEdit();
}


BEGIN_MESSAGE_MAP(CColorRangeEdit, CRangeEdit)
	//{{AFX_MSG_MAP(CColorRangeEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CColorRangeEdit message handlers

void CColorRangeEdit::SetRange( int iMin, int iMax, int iStep )
{

  m_iMinimum  = 0;
  m_iMaximum  = 1536;
  m_iStep     = 2;

}

void CColorRangeEdit::SetRange( float fMin, float fMax, float fStep, int iVorKomma, int iNachKomma )
{

  m_iMinimum  = 0;
  m_iMaximum  = 1536;
  m_iStep     = 2;

}

void CColorRangeEdit::SetPosition( float fPos )
{
}



COLORREF CColorRangeEdit::GetColor( int iPos )
{

  int         iFarbWert,
              iFarbR,
              iFarbG,
              iFarbB;


  iFarbWert = iPos;
  if ( iFarbWert < 0 )
  {
    iFarbWert = 0;
  }
  if ( iFarbWert >= 1536 )
  {
    iFarbWert = 1535;
  }
  iFarbR = 0;
  iFarbG = 0;
  iFarbB = 0;
  if ( ( iFarbWert < 256 )
  ||   ( iFarbWert >= 1280 ) )
  {
    iFarbR = 255;
  }
  if ( ( iFarbWert >= 256 )
  &&   ( iFarbWert < 2 * 256 ) )
  {
    iFarbR = 2 * 256 - 1 - iFarbWert;
  }
  if ( ( iFarbWert >= 4 * 256 )
  &&   ( iFarbWert < 5 * 256 ) )
  {
    iFarbR = iFarbWert - 4 * 256;
  }
  if ( ( iFarbWert >= 256 )
  &&   ( iFarbWert < 3 * 256 ) )
  {
    iFarbG = 255;
  }
  if ( ( iFarbWert >= 3 * 256 )
  &&   ( iFarbWert < 4 * 256 ) )
  {
    iFarbG = 4 * 256 - 1 - iFarbWert;
  }
  if ( ( iFarbWert >= 0 )
  &&   ( iFarbWert < 256 ) )
  {
    iFarbG = iFarbWert;
  }

  if ( ( iFarbWert >= 3 * 256 )
  &&   ( iFarbWert < 5 * 256 ) )
  {
    iFarbB = 255;
  }
  if ( ( iFarbWert >= 5 * 256 )
  &&   ( iFarbWert < 6 * 256 ) )
  {
    iFarbB = 6 * 256 - 1 - iFarbWert;
  }
  if ( ( iFarbWert >= 2 * 256 )
  &&   ( iFarbWert < 3 * 256 ) )
  {
    iFarbB = iFarbWert - 2 * 256;
  }

  return ( iFarbB << 16 ) | ( iFarbG << 8 ) | iFarbR;

}


void CColorRangeEdit::DrawPopup( HDC hdc, const RECT& rcTarget )
{

  HDC   hdcBitmap = CreateCompatibleDC( hdc );

  SelectObject( hdcBitmap, m_hbmColorBar );
  SetStretchBltMode( hdc, COLORONCOLOR );
  StretchBlt( hdc, rcTarget.left, rcTarget.top, rcTarget.right - rcTarget.left, rcTarget.bottom - rcTarget.top,
              hdcBitmap, 0, 0, 1536, 1, SRCCOPY );
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
