#include "stdafx.h"
#include "Settings.h"
#include "PaletteDialog.h"
#include "PainterDoc.h"
#include "ColorPicker.h"

#include <Grafik/Palette.h>

#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CPaletteDialog::CPaletteDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPaletteDialog::IDD, pParent),
  m_pCurrentPal( NULL )
{
	//{{AFX_DATA_INIT(CPaletteDialog)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}



void CPaletteDialog::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(CPaletteDialog)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CPaletteDialog, CDialog)
	//{{AFX_MSG_MAP(CPaletteDialog)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_VERLAUF, OnButtonVerlauf)
	ON_BN_CLICKED(IDC_BUTTON_WIN20, OnButtonWin20)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON_SHIFT_PAL, OnButtonShiftPal)
	ON_BN_CLICKED(IDC_BUTTON_GRAYSCALE, OnButtonGrayscale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CPaletteDialog::OnPaint() 
{
	CPaintDC        dc( this ); // device context for painting

  HBRUSH          hBrush,
                  hOldBrush;

  RECT            rc;

  DWORD           i,
                  j;

  GR::Graphic::Palette*   pPal = pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() );


  for ( i = 0; i < 16; i++ )
  {
    for ( j = 0; j < 16; j++ )
    {
      hBrush = CreateSolidBrush( RGB( (WORD)pPal->Red( (BYTE)i + (BYTE)j * 16 ),
                                      (WORD)pPal->Green( (BYTE)i + (BYTE)j * 16 ),
                                      (WORD)pPal->Blue( (BYTE)i + (BYTE)j * 16 ) ) );
      rc.top = 10 + j * 10;
      rc.left = 10 + i * 10;
      rc.right = 10 + i * 10 + 10;
      rc.bottom = 10 + j * 10 + 10;
      if ( m_Color1 == i + j * 16 )
      {
        rc.top++;
        rc.left++;
        rc.right--;
        rc.bottom--;
      }
      FillRect( dc, &rc, hBrush );
      DeleteObject( hBrush );
    }
  }
  hBrush = (HBRUSH)CreatePen( PS_SOLID, 1, RGB( 255, 255, 255 ) );
  hOldBrush = (HBRUSH)SelectObject( dc, hBrush );
  MoveToEx( dc, 10 + ( m_Color1 % 16 ) * 10,
            10 + ( m_Color1 / 16 ) * 10, NULL );
  LineTo( dc, 10 + ( m_Color1 % 16 ) * 10 + 9,
              10 + ( m_Color1 / 16 ) * 10 );
  LineTo( dc, 10 + ( m_Color1 % 16 ) * 10 + 9,
              10 + ( m_Color1 / 16 ) * 10 + 9 );
  LineTo( dc, 10 + ( m_Color1 % 16 ) * 10,
              10 + ( m_Color1 / 16 ) * 10 + 9 );
  LineTo( dc, 10 + ( m_Color1 % 16 ) * 10,
              10 + ( m_Color1 / 16 ) * 10 );
  SelectObject( dc, hOldBrush );
  DeleteObject( hBrush );
  hBrush = (HBRUSH)CreatePen( PS_SOLID, 1, RGB( 0, 0, 0) );
  hOldBrush = (HBRUSH)SelectObject( dc, hBrush );
  MoveToEx( dc, 10 + ( m_Color1 % 16 ) * 10 + 1,
                10 + ( m_Color1 / 16 ) * 10 + 1, NULL );
  LineTo( dc, 10 + ( m_Color1 % 16 ) * 10 + 8,
              10 + ( m_Color1 / 16 ) * 10 + 1 );
  LineTo( dc, 10 + ( m_Color1 % 16 ) * 10 + 8,
              10 + ( m_Color1 / 16 ) * 10 + 8 );
  LineTo( dc, 10 + ( m_Color1 % 16 ) * 10 + 1,
              10 + ( m_Color1 / 16 ) * 10 + 8 );
  LineTo( dc, 10 + ( m_Color1 % 16 ) * 10 + 1,
              10 + ( m_Color1 / 16 ) * 10 + 1 );
  SelectObject( dc, hOldBrush );
  DeleteObject( hBrush );

  // zweite Farbe
  hBrush = (HBRUSH)CreatePen( PS_SOLID, 1, RGB( 255, 255, 255 ) );
  hOldBrush = (HBRUSH)SelectObject( dc, hBrush );
  MoveToEx( dc, 10 + ( m_Color2 % 16 ) * 10 + 2,
                10 + ( m_Color2 / 16 ) * 10 + 2, NULL );
  LineTo( dc, 10 + ( m_Color2 % 16 ) * 10 + 7,
              10 + ( m_Color2 / 16 ) * 10 + 2 );
  LineTo( dc, 10 + ( m_Color2 % 16 ) * 10 + 7,
              10 + ( m_Color2 / 16 ) * 10 + 7 );
  LineTo( dc, 10 + ( m_Color2 % 16 ) * 10 + 2,
              10 + ( m_Color2 / 16 ) * 10 + 7 );
  LineTo( dc, 10 + ( m_Color2 % 16 ) * 10 + 2,
              10 + ( m_Color2 / 16 ) * 10 + 2 );
  SelectObject( dc, hOldBrush );
  DeleteObject( hBrush );
  hBrush = (HBRUSH)CreatePen( PS_SOLID, 1, RGB( 0, 0, 0) );
  hOldBrush = (HBRUSH)SelectObject( dc, hBrush );
  MoveToEx( dc, 10 + ( m_Color2 % 16 ) * 10 + 3,
                10 + ( m_Color2 / 16 ) * 10 + 3, NULL );
  LineTo( dc, 10 + ( m_Color2 % 16 ) * 10 + 6,
              10 + ( m_Color2 / 16 ) * 10 + 3 );
  LineTo( dc, 10 + ( m_Color2 % 16 ) * 10 + 6,
              10 + ( m_Color2 / 16 ) * 10 + 6 );
  LineTo( dc, 10 + ( m_Color2 % 16 ) * 10 + 3,
              10 + ( m_Color2 / 16 ) * 10 + 6 );
  LineTo( dc, 10 + ( m_Color2 % 16 ) * 10 + 3,
              10 + ( m_Color2 / 16 ) * 10 + 3 );
  SelectObject( dc, hOldBrush );
  DeleteObject( hBrush );
}



void CPaletteDialog::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
  int             iDummy;

  GR::tChar       hs[100];

  RECT            rc;

  GR::Graphic::Palette*   pPal = pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() );
	
  if ( pScrollBar->GetDlgCtrlID() == IDC_SLIDER_R )
  {
    iDummy = 255 - (int)SendDlgItemMessage( IDC_SLIDER_R, TBM_GETPOS, 0, 0 );
    wsprintf( hs, _T( "R:%2d" ), iDummy );
    SetDlgItemText( IDC_TEXT_ROT, hs );
    pPal->SetColor( (BYTE)m_Color1, 
              iDummy, 
              pPal->Green( (BYTE)m_Color1 ),
              pPal->Blue( (BYTE)m_Color1 ) );
    rc.left = 10;
    rc.top = 10;
    rc.right = 10 + 150 + 10;
    rc.bottom = 10 + 150 + 10,
    InvalidateRect( &rc, FALSE );
  }
  else if ( pScrollBar->GetDlgCtrlID() == IDC_SLIDER_G )
  {
    iDummy = 255 - (int)SendDlgItemMessage( IDC_SLIDER_G, TBM_GETPOS, 0, 0 );
    wsprintf( hs, _T( "G:%2d" ), iDummy );
    SetDlgItemText( IDC_TEXT_GRUEN, hs );
    pPal->SetColor( (BYTE)m_Color1, 
              pPal->Red( (BYTE)m_Color1 ),
              iDummy,
              pPal->Blue( (BYTE)m_Color1 ) );
    rc.left = 10;
    rc.top = 10;
    rc.right = 10 + 150 + 10;
    rc.bottom = 10 + 150 + 10,
    InvalidateRect( &rc, FALSE );
  }
  else if ( pScrollBar->GetDlgCtrlID() == IDC_SLIDER_B )
  {
    iDummy = 255 - (int)SendDlgItemMessage( IDC_SLIDER_B, TBM_GETPOS, 0, 0 );
    wsprintf( hs, _T( "B:%2d" ), iDummy );
    SetDlgItemText( IDC_TEXT_BLAU, hs );
    pPal->SetColor( (BYTE)m_Color1, 
              pPal->Red( (BYTE)m_Color1 ),
              pPal->Green( (BYTE)m_Color1 ),
              iDummy );
    rc.left = 10;
    rc.top = 10;
    rc.right = 10 + 150 + 10;
    rc.bottom = 10 + 150 + 10,
    InvalidateRect( &rc, FALSE );
  }
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}



BOOL CPaletteDialog::OnInitDialog() 
{
  GR::tChar                hs[100];


	CDialog::OnInitDialog();
	
  m_pCurrentPal   = pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() );

  m_Color1 = 0;
  m_Color2 = 255;

  // Range auf 0 bis 255 setzen
  SendDlgItemMessage( IDC_SLIDER_R, TBM_SETRANGE, (WPARAM)TRUE, MAKELONG( 0, 255 ) );
  SendDlgItemMessage( IDC_SLIDER_G, TBM_SETRANGE, (WPARAM)TRUE, MAKELONG( 0, 255 ) );
  SendDlgItemMessage( IDC_SLIDER_B, TBM_SETRANGE, (WPARAM)TRUE, MAKELONG( 0, 255 ) );

  // Positionen der Slider setzen
  SendDlgItemMessage( IDC_SLIDER_R, TBM_SETPOS, (WPARAM)TRUE, 255 - m_pCurrentPal->Red( (BYTE)m_Color1 ) );
  SendDlgItemMessage( IDC_SLIDER_G, TBM_SETPOS, (WPARAM)TRUE, 255 - m_pCurrentPal->Green( (BYTE)m_Color1 ) );
  SendDlgItemMessage( IDC_SLIDER_B, TBM_SETPOS, (WPARAM)TRUE, 255 - m_pCurrentPal->Blue( (BYTE)m_Color1 ) );
  
  wsprintf( hs, _T( "R:%2d" ), m_pCurrentPal->Red( (BYTE)m_Color1 ) );
  SetDlgItemText( IDC_TEXT_ROT, hs );
  wsprintf( hs, _T( "G:%2d" ), m_pCurrentPal->Green( (BYTE)m_Color1 ) );
  SetDlgItemText( IDC_TEXT_GRUEN, hs );
  wsprintf( hs, _T( "B:%2d" ), m_pCurrentPal->Blue( (BYTE)m_Color1 ) );
  SetDlgItemText( IDC_TEXT_BLAU, hs );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}



void CPaletteDialog::OnLButtonDown( UINT nFlags, CPoint point ) 
{
  int               i,
                    j;

  RECT              rc;

  GR::tChar         hs[100];


  for ( i = 0; i < 16; i++ )
  {
    for ( j = 0; j < 16; j++ )
    {
      if ( ( point.x >= 10 + i * 10 )
      &&   ( point.y >= 10 + j * 10 )
      &&   ( point.x < 20 + i * 10 )
      &&   ( point.y < 20 + j * 10 ) )
      {
        m_Color1 = ( ( point.x - 10 ) / 10 + ( ( point.y - 10 ) / 10 ) * 16 );
        SendDlgItemMessage( IDC_SLIDER_R, TBM_SETPOS, (WPARAM)TRUE, 255 - m_pCurrentPal->Red( (BYTE)m_Color1 ) );
        SendDlgItemMessage( IDC_SLIDER_G, TBM_SETPOS, (WPARAM)TRUE, 255 - m_pCurrentPal->Green( (BYTE)m_Color1 ) );
        SendDlgItemMessage( IDC_SLIDER_B, TBM_SETPOS, (WPARAM)TRUE, 255 - m_pCurrentPal->Blue( (BYTE)m_Color1 ) );
  
        wsprintf( hs, _T( "R:%2d" ), m_pCurrentPal->Red( (BYTE)m_Color1 ) );
        SetDlgItemText( IDC_TEXT_ROT, hs );
        wsprintf( hs, _T( "G:%2d" ), m_pCurrentPal->Green( (BYTE)m_Color1 ) );
        SetDlgItemText( IDC_TEXT_GRUEN, hs );
        wsprintf( hs, _T( "B:%2d" ), m_pCurrentPal->Blue( (BYTE)m_Color1 ) );
        SetDlgItemText( IDC_TEXT_BLAU, hs );

        rc.left = 10;
        rc.top = 10;
        rc.right = 10 + 150 + 10;
        rc.bottom = 10 + 150 + 10,
        InvalidateRect( &rc, FALSE );
        break;
      }
    }
  }

	CDialog::OnLButtonDown( nFlags, point );
}



void CPaletteDialog::OnRButtonDown( UINT nFlags, CPoint point ) 
{
  int               i,
                    j;

  RECT              rc;


  for ( i = 0; i < 16; i++ )
  {
    for ( j = 0; j < 16; j++ )
    {
      if ( ( point.x >= 10 + i * 10 )
      &&   ( point.y >= 10 + j * 10 )
      &&   ( point.x < 20 + i * 10 )
      &&   ( point.y < 20 + j * 10 ) )
      {
        m_Color2 = ( ( point.x - 10 ) / 10 + ( ( point.y - 10 ) / 10 ) * 16 );

        rc.left = 10;
        rc.top = 10;
        rc.right = 10 + 150 + 10;
        rc.bottom = 10 + 150 + 10,
        InvalidateRect( &rc, FALSE );
        break;
      }
    }
  }

	CDialog::OnRButtonDown( nFlags, point );
}



void CPaletteDialog::OnButtonVerlauf() 
{
  DWORD             wStart,
                    wEnde,
                    i;

  int               iR,
                    iG,
                    iB;


  if ( m_Color2 < m_Color1 )
  {
    wStart = m_Color2;
    wEnde = m_Color1;
  }
  else if ( m_Color2 > m_Color1 )
  {
    wStart = m_Color1;
    wEnde = m_Color2;
  }
  else
  {
    // dieselbe Farbe, keine Berechnung nötig!
    return;
  }
  for ( i = wStart + 1; i <= wEnde - 1; i++ )
  {
    iR = ( m_pCurrentPal->Red( (BYTE)wStart ) * ( wEnde - i )   + m_pCurrentPal->Red( (BYTE)wEnde ) *   ( i - wStart ) ) / ( wEnde - wStart );
    iG = ( m_pCurrentPal->Green( (BYTE)wStart ) * ( wEnde - i ) + m_pCurrentPal->Green( (BYTE)wEnde ) * ( i - wStart ) ) / ( wEnde - wStart );
    iB = ( m_pCurrentPal->Blue( (BYTE)wStart ) * ( wEnde - i )  + m_pCurrentPal->Blue( (BYTE)wEnde ) *  ( i - wStart ) ) / ( wEnde - wStart );
    m_pCurrentPal->SetColor( (BYTE)i, iR, iG, iB );
  }
  pDoc->diInfo.RecreateAllViews();

  pSettings->m_pActiveDocInfo->RedrawAllViews();
  Invalidate( FALSE );
}



void CPaletteDialog::OnButtonWin20() 
{
  m_pCurrentPal->SetColor( 0,  0,  0,  0 );
  m_pCurrentPal->SetColor( 1, 128,  0,  0 );
  m_pCurrentPal->SetColor( 2,  0, 128,  0 );
  m_pCurrentPal->SetColor( 3, 128, 128,  0 );
  m_pCurrentPal->SetColor( 4,  0,  0, 128 );
  m_pCurrentPal->SetColor( 5, 128,  0, 128 );
  m_pCurrentPal->SetColor( 6,  0, 128, 128 );
  m_pCurrentPal->SetColor( 7, 192, 192, 192 );
  m_pCurrentPal->SetColor( 8, 192, 223, 192 );
  m_pCurrentPal->SetColor( 9, 166, 202, 243 );
  m_pCurrentPal->SetColor( 246, 255, 252, 242 );
  m_pCurrentPal->SetColor( 247, 162, 162, 164 );
  m_pCurrentPal->SetColor( 248, 128, 128, 128 );
  m_pCurrentPal->SetColor( 249, 255,  0,  0 );
  m_pCurrentPal->SetColor( 250,  0, 255,  0 );
  m_pCurrentPal->SetColor( 251, 255, 255,  0 );
  m_pCurrentPal->SetColor( 252,  0,  0, 255 );
  m_pCurrentPal->SetColor( 253, 255,  0, 255 );
  m_pCurrentPal->SetColor( 254,  0, 255, 255 );
  m_pCurrentPal->SetColor( 255, 255, 255, 255 );
  Invalidate( FALSE );
}




void CPaletteDialog::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
  if ( pDoc == NULL )
  {
    return;
  }
  if ( pDoc->diInfo.m_BitDepth <= 8 )
  {
    int   iIndices = 1 << pDoc->diInfo.m_BitDepth;

    int   iLength = (int)sqrtf( (float)iIndices );
    int   iRectLength = 160 / iLength;

    for ( int i = 0; i < iLength; i++ )
    {
      for ( int j = 0; j < iLength; j++ )
      {
        if ( ( point.x >= i * iRectLength )
        &&   ( point.y >= j * iRectLength )
        &&   ( point.x < iRectLength + i * iRectLength )
        &&   ( point.y < iRectLength + j * iRectLength ) )
        {
          int   iIndex = i + j * iLength;

          CColorPicker    *dlgCP = new CColorPicker();

          dlgCP->m_WorkColor = 
              ( m_pCurrentPal->Red( iIndex ) << 16 )
            + ( m_pCurrentPal->Green( iIndex ) << 8 )
            + ( m_pCurrentPal->Blue ( iIndex ) );
          dlgCP->DoModal();

          delete dlgCP;
          break;
        }
      }
    }
  }
	
	CDialog::OnLButtonDblClk( nFlags, point );
}



void CPaletteDialog::OnButtonShiftPal() 
{
  int   iR,iG,iB;

  for ( int i = 0; i < 256; i++ )
  {
    iR = ( m_pCurrentPal->Red( i ) << 2 );
    iG = ( m_pCurrentPal->Green( i ) << 2 );
    iB = ( m_pCurrentPal->Blue( i ) << 2 );
    m_pCurrentPal->SetColor( i, iR, iG, iB );
  }
  pDoc->diInfo.RecreateAllViews();

  pSettings->m_pActiveDocInfo->RedrawAllViews();
  Invalidate( FALSE );
}



void CPaletteDialog::OnButtonGrayscale() 
{
  for ( int i = 0; i < 256; i++ )
  {
    m_pCurrentPal->SetColor( i, i, i, i );
  }
  pDoc->diInfo.RecreateAllViews();

  pSettings->m_pActiveDocInfo->RedrawAllViews();
  Invalidate( FALSE );
}
