#include "stdafx.h"
#include "DlgPToolFarben.h"

#include "Settings.h"
#include "ColorPicker.h"

#include "PainterEnums.h"

#include "DocumentInfo.h"
#include "ViewInfo.h"
#include "PainterGfxTools.h"

#include <Grafik/Palette.h>

#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgPToolFarben::CDlgPToolFarben(CWnd* pParent /*=NULL*/)
  : CDialog(CDlgPToolFarben::IDD, pParent),
    m_ColorSwitchRect( 86, 138, 100, 153 )
{
	//{{AFX_DATA_INIT(CDlgPToolFarben)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

  m_pViewInfo = NULL;
  pPage = NULL;

}


void CDlgPToolFarben::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPToolFarben)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPToolFarben, CDialog)
	//{{AFX_MSG_MAP(CDlgPToolFarben)
	ON_WM_DRAWITEM()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CDlgPToolFarben::OnInitDialog() 
{
	CDialog::OnInitDialog();

  RECT          rc;


  rc.top = 4;
  rc.left = 4;
  rc.right = 191 + 4 + 2;
  rc.bottom = 166 + 2;
  wndPalette.CreateEx( WS_EX_CLIENTEDGE, _T( "STATIC" ), _T( "unknown" ), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, rc, this, 11, NULL );
  wndPalette.ShowWindow( SW_SHOW );

  wndPalette.ModifyStyle( 0, SS_OWNERDRAW );
  wndPalette.ModifyStyleEx( 0, WS_EX_CLIENTEDGE, SWP_DRAWFRAME | SWP_SHOWWINDOW );
	
	pPage = new GR::Graphic::GDIPage();
  pPage->Create( wndPalette.GetSafeHwnd(), 192, 165, 24 );

	return TRUE;
}



void CDlgPToolFarben::UpdateControls()
{
  wndPalette.RedrawWindow();
}



COLORREF CDlgPToolFarben::GetQuadrantColor( int iQuadrant, unsigned char ucHCPalette, int iX, int iY )
{
  int         iRed,
              iGreen,
              iR1,
              iBlue,
              iG1,
              iB1,
              iR2,
              iG2,
              iB2,
              iMax,
              iOben,
              iUnten;


  DWORD     dwBitDepth = GetDisplayBitDepth();
  if ( ( dwBitDepth == 15 )
  ||   ( dwBitDepth == 16 ) )
  {
    iMax = 31;
  }
  else if ( ( dwBitDepth == 24 )
  ||        ( dwBitDepth == 32 ) )
  {
    iMax = 31;
  }
  iX <<= 3;
  iY <<= 3;
  iMax = 255;
  iOben = iMax;
  iUnten = 0;

  if ( ucHCPalette == 0 )
  {
    iR1 = iOben;
    iG1 = 0;
    iB1 = 0;

    iR2 = 0;
    iG2 = iOben;
    iB2 = 0;
  }
  else if ( ucHCPalette == 1 )
  {
    iR1 = 0;
    iG1 = iOben;
    iB1 = 0;

    iR2 = 0;
    iG2 = 0;
    iB2 = iOben;
  }
  else if ( ucHCPalette == 2 )
  {
    iR1 = 0;
    iG1 = 0;
    iB1 = iOben;

    iR2 = iOben;
    iG2 = 0;
    iB2 = 0;
  }
  else if ( ucHCPalette == 3 )
  {
    iR1 = iMax / 2 + 1;
    iG1 = iMax / 2 + 1;
    iB1 = iMax / 2 + 1;

    iR2 = iMax / 2 + 1;
    iG2 = iMax / 2 + 1;
    iB2 = iMax / 2 + 1;
  }

  if ( iQuadrant == 0 )
  {
    iRed = iR1;
    if ( iR2 )
    {
      iRed = iX;
    }
    iGreen = iG1;
    if ( iG2 )
    {
      iGreen = iX;
    }
    iBlue = iB1;
    if ( iB2 )
    {
      iBlue = iX;
    }

    iRed = ( ( iRed * iY ) + ( iOben * ( iMax - iY ) ) ) / iMax;
    iGreen = ( ( iGreen * iY ) + ( iOben * ( iMax - iY ) ) ) / iMax;
    iBlue = ( ( iBlue * iY ) + ( iOben * ( iMax - iY ) ) ) / iMax;
  }
  else if ( iQuadrant == 1 )
  {
    // 2. Quadrat
    iRed = iR2;
    if ( iR1 )
    {
      iRed = iOben - iX;
    }
    iGreen = iG2;
    if ( iG1 )
    {
      iGreen = iOben - iX;
    }
    iBlue = iB2;
    if ( iB1 )
    {
      iBlue = iOben - iX;
    }

    iRed = ( ( iRed * iY ) + ( iOben * ( iMax - iY ) ) ) / iMax;
    iGreen = ( ( iGreen * iY ) + ( iOben * ( iMax - iY ) ) ) / iMax;
    iBlue = ( ( iBlue * iY ) + ( iOben * ( iMax - iY ) ) ) / iMax;
  }
  else if ( iQuadrant == 2 )
  {
    // 3. Quadrat
    iRed = iR1;
    if ( iR2 )
    {
      iRed = iX;
    }
    iGreen = iG1;
    if ( iG2 )
    {
      iGreen = iX;
    }
    iBlue = iB1;
    if ( iB2 )
    {
      iBlue = iX;
    }

    iRed = ( iRed * ( iMax - iY ) ) / iMax;
    iGreen = ( iGreen * ( iMax - iY ) ) / iMax;
    iBlue = ( iBlue * ( iMax - iY ) ) / iMax;
  }
  else
  {
    // 4. Quadrat
    iRed = iR2;
    if ( iR1 )
    {
      iRed = iOben - iX;
    }
    iGreen = iG2;
    if ( iG1 )
    {
      iGreen = iOben - iX;
    }
    iBlue = iB2;
    if ( iB1 )
    {
      iBlue = iOben - iX;
    }

    iRed = ( iRed * ( iMax - iY ) ) / iMax;
    iGreen = ( iGreen * ( iMax - iY ) ) / iMax;
    iBlue = ( iBlue * ( iMax - iY ) ) / iMax;
  }

  if ( ( dwBitDepth == 15 )
  ||   ( dwBitDepth == 16 )
  ||   ( dwBitDepth == 24 )
  ||   ( dwBitDepth == 32 ) )
  {
    // BAUSTELLE Alpha!
    //return ( ( iRed << 3 ) << 16 ) + ( ( iGreen << 3 ) << 8 ) + ( iBlue << 3 );
    return ( iRed << 16 ) + ( iGreen << 8 ) + iBlue;
  }
  return 0;
}



void CDlgPToolFarben::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
  if ( lpDrawItemStruct->hwndItem != wndPalette.GetSafeHwnd() )
  {
    CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
    return;
  }

  int             i,
                  j,
                  k;

  DWORD           dwDummy;


  if ( pPage == NULL )
  {
    return;
  }
  DWORD     dwBitDepth = GetDisplayBitDepth();
  pPage->Box( 0, 0, 191, 164, 
               pPage->GetRGB( (BYTE)( ( GetSysColor( COLOR_3DFACE ) & 0xff0000 ) >> 19 ),
                       (BYTE)( ( GetSysColor( COLOR_3DFACE ) & 0xff00 ) >> 11 ),
                       (BYTE)( ( GetSysColor( COLOR_3DFACE ) & 0xff ) >> 3 ) ) );
  if ( dwBitDepth <= 8 )
  {
    int   iIndices = 1 << dwBitDepth;

    int   iLengthX = (int)sqrtf( (float)iIndices );
    int   iLengthY = iLengthX;
    if ( iLengthX * iLengthY != iIndices )
    {
      iLengthX = iIndices / iLengthX;
    }
    int   iRectLengthX = 160 / iLengthX,
          iRectLengthY = 160 / iLengthY;

    if ( m_pViewInfo->m_pPalette != NULL )
    {
      for ( i = 0; i < iLengthX; i++ )
      {
        for ( j = 0; j < iLengthY; j++ )
        {
          // GEORG 10.7.2002
          // Paletten haben jetzt volle 256-Weite
          pPage->Box( i * iRectLengthX, j * iRectLengthY, i * iRectLengthX + iRectLengthX - 1, j * iRectLengthY + iRectLengthY - 1, 
                 pPage->GetRGB( ( m_pViewInfo->m_pPalette->Red( i + j * iLengthX ) ),
                                ( m_pViewInfo->m_pPalette->Green( i + j * iLengthX ) ),
                                ( m_pViewInfo->m_pPalette->Blue( i + j * iLengthX ) ) ) );
        }
      }
      size_t iIndex = 0;
      if ( m_pViewInfo->m_pDocInfo == NULL )
      {
        iIndex = m_pViewInfo->m_pPalette->FindNearestIndex( pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR ) );
      }
      else
      {
        iIndex = m_pViewInfo->ToLocalColor( CSettings::ColorCategory::WORKCOLOR );
      }
      // erste Farbe
      pPage->Rectangle( ( iIndex % iLengthX ) * iRectLengthX,
                 ( iIndex / iLengthX ) * iRectLengthY,
                 ( iIndex % iLengthX ) * iRectLengthX + iRectLengthX - 1,
                 ( iIndex / iLengthX ) * iRectLengthY + iRectLengthY - 1,
                 pPage->GetRGB( 255, 255, 255 ) );
      pPage->Rectangle( ( iIndex % iLengthX ) * iRectLengthX + 1,
                 ( iIndex / iLengthX ) * iRectLengthY + 1,
                 ( iIndex % iLengthX ) * iRectLengthX + iRectLengthX - 2,
                 ( iIndex / iLengthX ) * iRectLengthY + iRectLengthY - 2,
                 0 );

      // zweite Farbe
      int iIndex2 = 0;
      if ( m_pViewInfo->m_pDocInfo == NULL )
      {
        iIndex2 = m_pViewInfo->m_pPalette->FindNearestIndex( pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 ) );
      }
      else
      {
        iIndex2 = m_pViewInfo->ToLocalColor( CSettings::ColorCategory::WORKCOLOR_2 );
      }
      pPage->Rectangle( ( iIndex2 % iLengthX ) * iRectLengthX + 2,
                 ( iIndex2 / iLengthX ) * iRectLengthY + 2,
                 ( iIndex2 % iLengthX ) * iRectLengthX + iRectLengthX - 3,
                 ( iIndex2 / iLengthX ) * iRectLengthY + iRectLengthY - 3,
                 pPage->GetRGB( 255, 255, 255 ) );
      pPage->Rectangle( ( iIndex2 % iLengthX ) * iRectLengthX + 3,
                 ( iIndex2 / iLengthX ) * iRectLengthY + 3,
                 ( iIndex2 % iLengthX ) * iRectLengthX + iRectLengthX - 4,
                 ( iIndex2 / iLengthX ) * iRectLengthY + iRectLengthY - 4,
                 0 );
    }
  }
  else if ( ( dwBitDepth == 15 )
  ||        ( dwBitDepth == 16 )
  ||        ( dwBitDepth == 24 )
  ||        ( dwBitDepth == 32 ) )
  {
    for ( i = 0; i < 32; i++ )
    {
      for ( j = 0; j < 32; j++ )
      {
        for ( k = 0; k < 3; k++ )
        {
          // 1. Quadrat
          dwDummy = GetQuadrantColor( 0, k, i, j );
          pPage->Box( k * 64 + i, j * 2, k * 64 + i, j * 2 + 1, dwDummy );
          if ( j == 31 )
          {
            pPage->Box( k * 64 + i, 64, k * 64 + i, 68, dwDummy );
          }

          // 2. Quadrat
          dwDummy = GetQuadrantColor( 1, k, i, j );
          pPage->Box( k * 64 + 32 + i, j * 2, k * 64 + 32 + i, j * 2 + 1, dwDummy );
          if ( j == 31 )
          {
            pPage->Box( k * 64 + 32 + i, 64, k * 64 + 32 + i, 68, dwDummy );
          }

          // 3. Quadrat
          dwDummy = GetQuadrantColor( 2, k, i, j );
          pPage->Box( k * 64 + i, 5 + 64 + j * 2, k * 64 + i, 5 + 64 + j * 2 + 1, dwDummy );

          // 4. Quadrat
          dwDummy = GetQuadrantColor( 3, k, i, j );
          pPage->Box( k * 64 + 32 + i, 5 + 64 + j * 2, k * 64 + 32 + i, 5 + 64 + j * 2 + 1, dwDummy );
        }
      }
    }
    pPage->Box( 1, 135, 21, 155, pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR ) );

    GR::tChar    szHex[100];

    DWORD dwColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR );

    wsprintf( szHex, _T( "#%02X%02X%02X" ), ( dwColor & 0xff0000 ) >> 16, ( dwColor & 0xff00 ) >> 8, dwColor & 0xff );

    SelectObject( pPage->GetDC(), GetStockObject( DEFAULT_GUI_FONT ) );
    SetTextColor( pPage->GetDC(), 0xffffff );
    SetBkMode( pPage->GetDC(), TRANSPARENT );
    TextOut( pPage->GetDC(), 27, 140, szHex, 1 );
    TextOut( pPage->GetDC(), 35, 140, szHex + 1, 2 );
    TextOut( pPage->GetDC(), 51, 140, szHex + 3, 2 );
    TextOut( pPage->GetDC(), 67, 140, szHex + 5, 2 );

    pPage->Box( 105, 135, 125, 155, pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 ) );

    dwColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 );
    wsprintf( szHex, _T( "#%02X%02X%02X" ), ( dwColor & 0xff0000 ) >> 16, ( dwColor & 0xff00 ) >> 8, dwColor & 0xff );

    SetTextColor( pPage->GetDC(), 0xffffff );
    SetBkMode( pPage->GetDC(), TRANSPARENT );
    TextOut( pPage->GetDC(), 105 + 26, 140, szHex, 1 );
    TextOut( pPage->GetDC(), 105 + 36, 140, szHex + 1, 2 );
    TextOut( pPage->GetDC(), 105 + 52, 140, szHex + 3, 2 );
    TextOut( pPage->GetDC(), 105 + 68, 140, szHex + 5, 2 );

    CRect   rc( 1, 135, 22, 156 );
    DrawEdge( pPage->GetDC(),
              &rc,
              BDR_RAISEDOUTER,
              BF_RECT );
    CRect   rc2( 105, 135, 126, 156 );
    DrawEdge( pPage->GetDC(),
              &rc2,
              BDR_RAISEDOUTER,
              BF_RECT );

    // Color-Switch-Button
    {
      DrawEdge( pPage->GetDC(),
                &m_ColorSwitchRect,
                BDR_RAISEDOUTER,
                BF_RECT );
      pPage->Line( 90, 142, 96, 142, pPage->GetRGB256( 0xffffff ) );
      pPage->Line( 96, 142, 94, 140, pPage->GetRGB256( 0xffffff ) );
      pPage->Line( 96, 142, 94, 144, pPage->GetRGB256( 0xffffff ) );

      pPage->Line( 90, 149, 96, 149, pPage->GetRGB256( 0xffffff ) );
      pPage->Line( 90, 149, 92, 147, pPage->GetRGB256( 0xffffff ) );
      pPage->Line( 90, 149, 92, 151, pPage->GetRGB256( 0xffffff ) );
    }
  }

  BitBlt( lpDrawItemStruct->hDC, 0, 0, 192, 165, pPage->GetDC(), 0, 0, SRCCOPY );
}



DWORD CDlgPToolFarben::MakeColor( DWORD dwRed, DWORD dwGreen, DWORD dwBlue, WORD wIntensity )
{
  DWORD        dwResult;

  DWORD     dwBitDepth = GetDisplayBitDepth();


  if ( ( dwBitDepth == 15 )
  ||   ( dwBitDepth == 16 )
  ||   ( dwBitDepth == 24 )
  ||   ( dwBitDepth == 32 ) )
  {
    dwRed =   ( dwRed * wIntensity ) / 255;
    dwGreen = ( dwGreen * wIntensity ) / 255;
    dwBlue =  ( dwBlue * wIntensity ) / 255;

    dwResult = (DWORD)( dwRed << 16 ) 
             + (DWORD)( dwGreen << 8 ) 
             + (DWORD)( dwBlue << 0 );
  }


  return dwResult;
}



DWORD CDlgPToolFarben::GetDisplayBitDepth() 
{
  DWORD   dwDepth = 32;

  if ( m_pViewInfo != NULL )
  {
    if ( m_pViewInfo->m_Type == ViewInfo::VI_ALPHA )
    {
      dwDepth = 8;
    }
    else if ( m_pViewInfo->m_pDocInfo != NULL )
    {
      dwDepth = m_pViewInfo->m_pDocInfo->GetBitDepth();
    }
  }
  return dwDepth;
}



void CDlgPToolFarben::OnLButtonDown( UINT nFlags, CPoint point ) 
{
  if ( GetDisplayBitDepth() > 8 )
  {
    point.x -= 6;
    point.y -= 6;

    if ( m_ColorSwitchRect.PtInRect( point ) )
    {
      // Arbeitsfarben tauschen
      DWORD   dwTemp = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR );
      pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR, pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 ) );
      pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_2, dwTemp );

      Notify( NF_ACTIVE_COLOR_CHANGED );
      UpdateControls();

      CWnd::Invalidate( FALSE );
      CDialog::OnLButtonDown( nFlags, point );
      return;
    }
  }

  DoLButtonDown( nFlags, point );
	
	CDialog::OnLButtonDown( nFlags, point );
}



void CDlgPToolFarben::DoLButtonDown( UINT nFlags, CPoint point ) 
{

  int               i,                    j,
                    k;


  DWORD     dwBitDepth = GetDisplayBitDepth();
  point.x -= 6;
  point.y -= 6;
  if ( dwBitDepth <= 8 )
  {
    int   iIndices = 1 << dwBitDepth;

    int   iLengthX = (int)sqrtf( (float)iIndices );
    int   iLengthY = iLengthX;
    if ( iLengthX * iLengthY != iIndices )
    {
      iLengthX = iIndices / iLengthX;
    }
    int   iRectLengthX = 160 / iLengthX,
          iRectLengthY = 160 / iLengthY;

    for ( i = 0; i < iLengthX; i++ )
    {
      for ( j = 0; j < iLengthY; j++ )
      {
        if ( ( point.x >= i * iRectLengthX )
        &&   ( point.y >= j * iRectLengthY )
        &&   ( point.x < iRectLengthX + i * iRectLengthX )
        &&   ( point.y < iRectLengthY + j * iRectLengthY ) )
        {
          pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR, 
              m_pViewInfo->GetRGBColor( i + j * iLengthX ) );
          pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_8BIT, i + j * iLengthX );
          pSettings->SetSetting( "GetColorDepth", dwBitDepth );

          //pSettings->Propagate( "Farben" );
          Notify( NF_ACTIVE_COLOR_CHANGED );

          UpdateControls();
          return;
        }
      }
    }
  }
  else if ( ( dwBitDepth == 15 )
  ||        ( dwBitDepth == 16 )
  ||        ( dwBitDepth == 24 )
  ||        ( dwBitDepth == 32 ) )
  {
    for ( k = 0; k < 3; k++ )
    {
      if ( ( point.x >= k * 64 + 0 )
      &&   ( point.y >= 0 )
      &&   ( point.x < k * 64 + 32 )
      &&   ( point.y < 69 ) )
      {
        // 1.Quadrant
        if ( point.y > 63 )
        {
          point.y = 63;
        }
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR, GetQuadrantColor( 0, k, point.x - k * 64, point.y / 2 ) );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
      }
      else if ( ( point.x >= k * 64 + 32 )
      &&        ( point.y >= 0 )
      &&        ( point.x < k * 64 + 64 )
      &&        ( point.y < 69 ) )
      {
        // 2.Quadrant
        if ( point.y > 63 )
        {
          point.y = 63;
        }
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR, GetQuadrantColor( 1, k, point.x - 32 - k * 64, point.y / 2 ) );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
      }
      else if ( ( point.x >= k * 64 + 0 )
      &&        ( point.y >= 64 + 5 )
      &&        ( point.x < k * 64 + 32 )
      &&        ( point.y < 128 + 5 ) )
      {
        // 3.Quadrant
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR, GetQuadrantColor( 2, k, point.x - k * 64, ( point.y - 64 - 5 ) / 2 ) );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
      }
      else if ( ( point.x >= k * 64 + 32 )
      &&        ( point.y >= 64 + 5 )
      &&        ( point.x < k * 64 + 64 )
      &&        ( point.y < 128 + 5 ) )
      {
        // 4.Quadrant
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR, GetQuadrantColor( 3, k, point.x - 32 - k * 64, ( point.y - 64 - 5 ) / 2 ) );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
      }
    }
    if ( ( point.x >= 1 )
    &&   ( point.y >= 135 )
    &&   ( point.x < 21 )
    &&   ( point.y < 155 ) )
    {
      CColorPicker    *dlgCP = new CColorPicker();

      dlgCP->m_WorkColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR );
      if ( dlgCP->DoModal() == IDOK )
      {
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR, dlgCP->m_WorkColor );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
        wndPalette.Invalidate();
      }
      delete dlgCP;
    }
    else if ( ( point.x >= 105 )
    &&        ( point.y >= 135 )
    &&        ( point.x < 125 )
    &&        ( point.y < 155 ) )
    {
      CColorPicker    *dlgCP = new CColorPicker();

      dlgCP->m_WorkColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 );
      if ( dlgCP->DoModal() == IDOK )
      {
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_2, dlgCP->m_WorkColor );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
        wndPalette.Invalidate();
      }
      delete dlgCP;
    }
    UpdateControls();
    return;
  }

}



void CDlgPToolFarben::OnMouseMove( UINT nFlags, CPoint point ) 
{

  if ( nFlags & MK_LBUTTON )
  {
    DoLButtonDown( nFlags, point );
  }
  if ( nFlags & MK_RBUTTON )
  {
    DoRButtonDown( nFlags, point );
  }
	
	CDialog::OnMouseMove( nFlags, point );

}



void CDlgPToolFarben::OnRButtonDown(UINT nFlags, CPoint point) 
{

  DoRButtonDown( nFlags, point );
	
	CDialog::OnRButtonDown(nFlags, point);

}



void CDlgPToolFarben::DoRButtonDown(UINT nFlags, CPoint point) 
{
  int               i,
                    j,
                    k;

  DWORD             dwDummy;


  DWORD     dwBitDepth = GetDisplayBitDepth();
  point.x -= 6;
  point.y -= 6;

  if ( dwBitDepth <= 8 )
  {
    int   iIndices = 1 << dwBitDepth;

    int   iLengthX = (int)sqrtf( (float)iIndices );
    int   iLengthY = iLengthX;
    if ( iLengthX * iLengthY != iIndices )
    {
      iLengthX = iIndices / iLengthX;
    }
    int   iRectLengthX = 160 / iLengthX,
          iRectLengthY = 160 / iLengthY;

    for ( i = 0; i < iLengthX; i++ )
    {
      for ( j = 0; j < iLengthY; j++ )
      {
        if ( ( point.x >= i * iRectLengthX )
        &&   ( point.y >= j * iRectLengthY )
        &&   ( point.x < iRectLengthX + i * iRectLengthX )
        &&   ( point.y < iRectLengthY + j * iRectLengthY ) )
        {
          pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_2, 
              m_pViewInfo->GetRGBColor( i + j * iLengthX ) );
          pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_2_8BIT,  i + j * iLengthX );
          pSettings->SetSetting( "GetColorDepth2", dwBitDepth );

          //pSettings->Propagate( "Farben" );
          Notify( NF_ACTIVE_COLOR_CHANGED );

          UpdateControls();
          break;
        }
      }
    }
  }
  else if ( ( dwBitDepth == 15 )
  ||        ( dwBitDepth == 16 )
  ||        ( dwBitDepth == 24 )
  ||        ( dwBitDepth == 32 ) )
  {
    for ( k = 0; k < 3; k++ )
    {
      if ( ( point.x >= k * 64 + 0 )
      &&   ( point.y >= 0 )
      &&   ( point.x < k * 64 + 32 )
      &&   ( point.y < 69 ) )
      {
        // 1.Quadrant
        if ( point.y > 63 )
        {
          point.y = 63;
        }
        dwDummy = GetQuadrantColor( 0, k, point.x - k * 64, point.y / 2 );
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_2, MakeColor( (WORD)( ( dwDummy & 0xff0000 ) >> 16 ),
                                          (WORD)( ( dwDummy & 0xff00 ) >> 8 ),
                                          (WORD)( dwDummy & 0xff ),
                                          255 ) );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
      }
      else if ( ( point.x >= k * 64 + 32 )
      &&        ( point.y >= 0 )
      &&        ( point.x < k * 64 + 64 )
      &&        ( point.y < 69 ) )
      {
        // 2.Quadrant
        if ( point.y > 63 )
        {
          point.y = 63;
        }
        dwDummy = GetQuadrantColor( 1, k, point.x - 32 - k * 64, point.y / 2 );
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_2, MakeColor( (WORD)( ( dwDummy & 0xff0000 ) >> 16 ),
                                          (WORD)( ( dwDummy & 0xff00 ) >> 8 ),
                                          (WORD)( dwDummy & 0xff ),
                                          255 ) );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
      }
      else if ( ( point.x >= k * 64 + 0 )
      &&        ( point.y >= 64 + 5 )
      &&        ( point.x < k * 64 + 32 )
      &&        ( point.y < 128 + 5 ) )
      {
        // 3.Quadrant
        dwDummy = GetQuadrantColor( 2, k, point.x - k * 64, ( point.y - 64 - 5 ) / 2 );
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_2, MakeColor( (WORD)( ( dwDummy & 0xff0000 ) >> 16 ),
                                          (WORD)( ( dwDummy & 0xff00 ) >> 8 ),
                                          (WORD)( dwDummy & 0xff ),
                                          255 ) );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
      }
      else if ( ( point.x >= k * 64 + 32 )
      &&        ( point.y >= 64 + 5 )
      &&        ( point.x < k * 64 + 64 )
      &&        ( point.y < 128 + 5 ) )
      {
        // 4.Quadrant
        dwDummy = GetQuadrantColor( 3, k, point.x - 32 - k * 64, ( point.y - 64 - 5 ) / 2 );
        pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_2, MakeColor( (WORD)( ( dwDummy & 0xff0000 ) >> 16 ),
                                          (WORD)( ( dwDummy & 0xff00 ) >> 8 ),
                                          (WORD)( dwDummy & 0xff ),
                                          255 ) );
        //pSettings->Propagate( "Farben" );
        Notify( NF_ACTIVE_COLOR_CHANGED );
      }
    }
    UpdateControls();
  }
	
}



void CDlgPToolFarben::OnDestroy() 
{

	CDialog::OnDestroy();
	
  if ( pPage != NULL )
  {
    delete pPage;
    pPage = NULL;
  }

}



void CDlgPToolFarben::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
  DWORD     dwBitDepth = GetDisplayBitDepth();
  point.x -= 6;
  point.y -= 6;

  if ( dwBitDepth <= 8 )
  {
    int   iIndices = 1 << dwBitDepth;

    int   iLengthX = (int)sqrtf( (float)iIndices );
    int   iLengthY = iLengthX;
    if ( iLengthX * iLengthY != iIndices )
    {
      iLengthX = iIndices / iLengthX;
    }
    int   iRectLengthX = 160 / iLengthX,
          iRectLengthY = 160 / iLengthY;

    for ( int i = 0; i < iLengthX; i++ )
    {
      for ( int j = 0; j < iLengthY; j++ )
      {
        if ( ( point.x >= i * iRectLengthX )
        &&   ( point.y >= j * iRectLengthY )
        &&   ( point.x < iRectLengthX + i * iRectLengthX )
        &&   ( point.y < iRectLengthY + j * iRectLengthY ) )
        {
          int   iIndex = i + j * iLengthX;

          CColorPicker    *dlgCP = new CColorPicker();

          // GEORG 10.7.2002
          // Paletten haben jetzt volle 256-Weite

          int   iR = m_pViewInfo->m_pPalette->Red( iIndex ),
                iG = m_pViewInfo->m_pPalette->Green( iIndex ),
                iB = m_pViewInfo->m_pPalette->Blue( iIndex );
          dlgCP->m_WorkColor = 
              ( iR << 16 )
            + ( iG << 8 )
            + ( iB );
          if ( dlgCP->DoModal() == IDOK )
          {
            iR = ( ( dlgCP->m_WorkColor & 0xff0000 ) >> 16 );
            iG = ( ( dlgCP->m_WorkColor & 0xff00 ) >> 8 );
            iB = dlgCP->m_WorkColor & 0xff;

            // GEORG 10.7.2002
            // Paletten haben jetzt volle 256-Weite
            m_pViewInfo->m_pPalette->SetColor( iIndex, iR, iG, iB );
            m_pViewInfo->m_pDocInfo->GetPalette( m_pViewInfo->m_pDocInfo->CurrentFrame() )->SetColor( iIndex, iR, iG, iB );
            m_pViewInfo->m_pPage->SetPalette( m_pViewInfo->m_pPalette );
            m_pViewInfo->m_pDocInfo->RedrawAllViews();

            wndPalette.Invalidate();

            //pSettings->Propagate( "Farben" );
            Notify( NF_ACTIVE_COLOR_CHANGED );
          }

          delete dlgCP;
          break;
        }
      }
    }
  }
	
	CDialog::OnLButtonDblClk(nFlags, point);

  OnLButtonDown( nFlags, point );
}



void CDlgPToolFarben::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember )
{
  switch ( NotifyMessage )
  {
    case NF_ACTIVE_COLOR_CHANGED:
    case NF_LAYER_CHANGED:
    case NF_FRAME_CHANGED:
      wndPalette.Invalidate( FALSE );
      break;
    case NF_ACTIVE_VIEW_CHANGED:
      m_pViewInfo = pSettings->m_pActiveViewInfo;
      if ( wndPalette.GetSafeHwnd() )
      {
        wndPalette.Invalidate( FALSE );
      }
      break;
  }
}



void CDlgPToolFarben::OnOK()
{
}

void CDlgPToolFarben::OnCancel()
{
}
