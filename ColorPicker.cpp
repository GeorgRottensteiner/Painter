#include "stdafx.h"
#include "ColorPicker.h"
#include "Painter.h"

#include <Grafik/GDIPage.h>
#include <Grafik/GfxHelper.h>
#include <Grafik/Image.h>

#include <Misc/Misc.h>

#include <String/Convert.h>

#include <math.h>

#include <math/math_util.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




CColorPicker::CColorPicker(CWnd* pParent /*=NULL*/)
	: CDialog(CColorPicker::IDD, pParent),
    m_PickMode( false )
{
	//{{AFX_DATA_INIT(CColorPicker)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

  m_pPage                 = NULL;
  m_pImageWheel           = NULL;

  m_CaptureColorBox       = false;
  m_CaptureColorWheel     = false;

  m_WheelPos              = 0;

  m_DoNotUpdate           = false;

  m_GradientColor1        = 0xff000000;
  m_GradientColor2        = 0xffffffff;

}


void CColorPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPicker)
	DDX_Control(pDX, IDC_BUTTON_CP_OLD_COLOR, m_ButtonOldColor);
	DDX_Control(pDX, IDC_IBUTTON_PICKER, m_ButtonPicker);
	DDX_Control(pDX, IDC_EDIT_CP_HTML, m_EditHTML);
	DDX_Control(pDX, IDC_REDIT_CP_SAT, m_REditSat);
	DDX_Control(pDX, IDC_REDIT_CP_LIGHT, m_REditLight);
	DDX_Control(pDX, IDC_REDIT_CP_HUE, m_REditHue);
	DDX_Control(pDX, IDC_BUTTON_CP_NEW_COLOR, m_ButtonNewColor);
	DDX_Control(pDX, IDC_REDIT_CP_ROT, m_REditRot);
	DDX_Control(pDX, IDC_REDIT_CP_GRUEN, m_REditGruen);
	DDX_Control(pDX, IDC_REDIT_CP_BLAU, m_REditBlau);
  DDX_Control(pDX, IDC_REDIT_GRADIENT, m_REditVerlauf);
  DDX_Control(pDX, IDC_BUTTON_CP_VERLAUF_1, m_buttonGradient1 );
  DDX_Control(pDX, IDC_BUTTON_CP_VERLAUF_2, m_buttonGradient2 );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorPicker, CDialog)
	//{{AFX_MSG_MAP(CColorPicker)
	ON_WM_DESTROY()
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ACTIVATEAPP()
	ON_EN_CHANGE(IDC_REDIT_CP_ROT, OnChangeREditRot)
	ON_EN_CHANGE(IDC_REDIT_CP_GRUEN, OnChangeREditGruen)
	ON_EN_CHANGE(IDC_REDIT_CP_BLAU, OnChangeREditBlau)
	ON_EN_CHANGE(IDC_REDIT_CP_HUE, OnChangeREditHue)
	ON_EN_CHANGE(IDC_REDIT_CP_SAT, OnChangeREditSaturation)
	ON_EN_CHANGE(IDC_REDIT_CP_LIGHT, OnChangeREditLight)
	ON_WM_RBUTTONDOWN()
  ON_BN_HILITE(IDC_IBUTTON_PICKER, OnPickerPushed)
	ON_BN_CLICKED(IDC_BUTTON_CP_OLD_COLOR, OnButtonCpOldColor)
	ON_EN_CHANGE(IDC_EDIT_CP_HTML, OnChangeEditCpHtml)
  ON_EN_CHANGE(IDC_REDIT_GRADIENT, OnChangeREditVerlauf)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BUTTON_CP_VERLAUF_2, OnBnClickedButtonCpVerlauf2)
  ON_BN_CLICKED(IDC_BUTTON_CP_VERLAUF_1, OnBnClickedButtonCpVerlauf1)
END_MESSAGE_MAP()




// H = 0.0 to 360.0 (corresponding to 0..360.0 degrees around hexcone)
// S = 0.0 (shade of gray) to 1.0 (pure color)
// V = 0.0 (black) to 1.0 {white)

// Based on C Code in "Computer Graphics -- Principles and Practice,"
// Foley et al, 1996, p. 592. 

void RGBToHSV( int iR, int iG, int iB, int *iH, int *iS, int *iV )
{
  float   Delta,
          Min;

  float   H,
          S,
          V;

  float   R = (float)iR / 255.0f,
          G = (float)iG / 255.0f,
          B = (float)iB / 255.0f;


  Min = R;
  if ( G < Min )
  {
    Min = G;
  }
  if ( B < Min )
  {
    Min = B;
  }
  V = R;
  if ( G > V )
  {
    V = G;
  }
  if ( B > V )
  {
    V = B;
  }

  Delta = V - Min;

  // Calculate saturation: saturation is 0 if r, g and b are all 0
  if ( V == 0 )
  {
    S = 0;
  }
  else
  {
    S = ( 255 * Delta ) / V;
  }

  if ( S == 0 )
  {
    // Achromatic: When s = 0, h is undefined
    H = 0;
  }
  else
  {
    if ( R == V )
    {
      // between yellow and magenta [degrees]
      H = 60.0f * ( G - B ) / Delta;
    }
    else if ( G == V )
    {
      // between cyan and yellow
      H = 120 + 60 * ( B - R ) / Delta;
    }
    else if ( B == V )
    {
      // between magenta and cyan
      H = 240 + 60 * ( R - G ) / Delta;
    }
    if ( H < 0 )
    {
      H += 360;
    }
  }


  *iV = (int)( 255.0f * V );
  *iH = (int)H;
  *iS = (int)S;

}



// H = 0.0 to 360.0 (corresponding to 0..360 degrees around hexcone)
// NaN (undefined) for S = 0
// S = 0.0 (shade of gray) to 1.0 (pure color)
// V = 0.0 (black) to 1.0 (white)

void HSVToRGB( int iH, int iS, int iV, int *iR, int *iG, int *iB )
{

  float   hTemp,
          f,
          S,
          V;

  int     i;

  float   p,
          q,
          t,
          R,
          G,
          B;


  S = (float)iS / 255.0f;
  V = (float)iV / 255.0f;
  if ( iS == 0 )
  {
    R = V;
    G = V;
    B = V;
  }
  else
  {
    if ( iH == 360 )
    {
      hTemp = 0;
    }
    else
    {
      hTemp = (float)iH;
    }

    hTemp = hTemp / 60.0f;     // h is now IN [0,6)
    i = (int)hTemp;        // largest integer <= h
    f = hTemp - i;                  // fractional part of h

    p = (float)( V * ( 1.0 - S) );
    q = (float)( V * ( 1.0 - (S * f)) );
    t = (float)( V * ( 1.0 - (S * (1.0 - f))) );

    switch ( i )
    {
      case 0:
        {
          R = V;
          G = t;
          B = p;
        }
        break;
      case 1:
        {
          R = q;
          G = V;
          B = p;
        }
        break;
      case 2:
        {
          R = p;
          G = V;
          B = t;
        }
        break;
      case 3:
        {
          R = p;
          G = q;
          B = V;
        }
        break;
      case 4:
        {
          R = t;
          G = p;
          B = V;
        }
        break;
      case 5:
        {
          R = V;
          G = p;
          B = q;
        }
        break;
    }

  }

  *iR = (int)( R * 255 );
  *iG = (int)( G * 255 );
  *iB = (int)( B * 255 );

}



BOOL CColorPicker::OnInitDialog() 
{

  RECT          rc;


	CDialog::OnInitDialog();

  m_ButtonPicker.SetIcon( (HICON)LoadImage( (HINSTANCE)GetModuleHandle( NULL ),
                                      MAKEINTRESOURCE( IDI_ICON_PICKER ),
                                      IMAGE_ICON,
                                      0,
                                      0,
                                      LR_DEFAULTCOLOR ) );

  for ( int i = 0; i < 6; i++ )
  {
    for ( int j = 0; j < 6; j++ )
    {
      rc.left = 10 + i * 16;
      rc.top = 10 + j * 16;
      rc.right = rc.left + 16;
      rc.bottom = rc.top + 16;
      buttonColor[i][j].Create( _T( "" ), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, rc, this, 0 );
    }
  }

  for ( int i = 0; i < 6; i++ )
  {
    for ( int j = 0; j < 6; j++ )
    {
      rc.left = 10 + i * 16;
      rc.top = 120 + 10 + j * 16;
      rc.right = rc.left + 16;
      rc.bottom = rc.top + 16;
      buttonCustomColor[i][j].Create( _T( "" ), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, rc, this, 0 );
      m_CustomColor[i][j] = pSettings->GetSetting( CMisc::printf( "CustomColor%d%d", i, j ), 0 );
    }
  }

  m_StartColor = m_WorkColor;

  m_FixedColor[0][0] = 0xffff0000;
  m_FixedColor[0][1] = 0xffffff00;
  m_FixedColor[0][2] = 0xff00ff00;
  m_FixedColor[0][3] = 0xff00ffff;
  m_FixedColor[0][4] = 0xff0000ff;
  m_FixedColor[0][5] = 0xffff00ff;

  m_FixedColor[1][0] = 0xff400000;
  m_FixedColor[1][1] = 0xff404000;
  m_FixedColor[1][2] = 0xff004000;
  m_FixedColor[1][3] = 0xff004040;
  m_FixedColor[1][4] = 0xff000040;
  m_FixedColor[1][5] = 0xff400040;

  m_FixedColor[2][0] = 0xff800000;
  m_FixedColor[2][1] = 0xff808000;
  m_FixedColor[2][2] = 0xff008000;
  m_FixedColor[2][3] = 0xff008080;
  m_FixedColor[2][4] = 0xff000080;
  m_FixedColor[2][5] = 0xff800080;

  m_FixedColor[3][0] = 0xffc00000;
  m_FixedColor[3][1] = 0xffc0c000;
  m_FixedColor[3][2] = 0xff00c000;
  m_FixedColor[3][3] = 0xff00c0c0;
  m_FixedColor[3][4] = 0xff0000c0;
  m_FixedColor[3][5] = 0xffc000c0;

  m_FixedColor[4][0] = 0xffff8080;
  m_FixedColor[4][1] = 0xffffff80;
  m_FixedColor[4][2] = 0xff80ff80;
  m_FixedColor[4][3] = 0xff80ffff;
  m_FixedColor[4][4] = 0xff8080ff;
  m_FixedColor[4][5] = 0xffff80ff;

  m_FixedColor[5][0] = 0xff000000;
  m_FixedColor[5][1] = 0xff333333;
  m_FixedColor[5][2] = 0xff666666;
  m_FixedColor[5][3] = 0xff999999;
  m_FixedColor[5][4] = 0xffcccccc;
  m_FixedColor[5][5] = 0xffffffff;

  rc.left = 120;
  rc.top = 10;
  rc.right = rc.left + 260;
  rc.bottom = rc.top + 260;
  wndColorWheel.CreateEx( WS_EX_STATICEDGE, _T( "STATIC" ), _T( "unknown" ), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, rc, this, 0, NULL );
  wndColorWheel.ShowWindow( SW_SHOW );

  wndColorWheel.ModifyStyle( 0, SS_OWNERDRAW );
	
  m_pPage = new GR::Graphic::GDIPage();
  m_pPage->Create( wndColorWheel.GetSafeHwnd(), 260, 260, 24 );

  {
    HBRUSH  hBrush = GetSysColorBrush( COLOR_BTNFACE );
    RECT    rc;
    wndColorWheel.GetClientRect( &rc );
    FillRect( m_pPage->GetDC(), &rc, hBrush );
  }

  float     fPos = 0.0f,
            fRadius = 100.0f;

  do
  {
    DWORD dwColor = GetWheelColor( (DWORD)fPos );
    for ( int i = 0; i < 10; i++ )
    {
      m_pPage->PutPixel( 130 + (int)( ( fRadius + i ) * cos( fPos * 3.1415926 / 180 ) ),
                         130 - (int)( ( fRadius + i ) * sin( fPos * 3.1415926 / 180 ) ),
                         dwColor );
    }
    fPos += ( 1 / fRadius );
  }
  while ( fPos < 360.0f );

  m_pImageWheel = new GR::Graphic::Image( 260, 260, m_pPage->GetDepth() );
  m_pImageWheel->GetImage( m_pPage, 0, 0 );

  m_RectPosX = 255;
  m_RectPosY = 128;

  m_DoNotUpdate = true;
  m_REditRot.SetRange( 0, 255 );
  m_REditRot.SetBarColor( RGB( 255, 0, 0 ) );
  m_REditGruen.SetRange( 0, 255 );
  m_REditGruen.SetBarColor( RGB( 0, 255, 0 ) );
  m_REditBlau.SetRange( 0, 255 );
  m_REditBlau.SetBarColor( RGB( 0, 0, 255 ) );
  m_REditHue.SetRange( 0, 359 );
  m_REditSat.SetRange( 0, 255 );
  m_REditLight.SetRange( 0, 255 );
  m_REditVerlauf.SetRange( 0, 255 );

  m_GradientColor1 = pSettings->GetSetting( "ColorPickerGradient1", 0xff000000 );
  m_GradientColor2 = pSettings->GetSetting( "ColorPickerGradient2", 0xffffffff );

  m_REditVerlauf.SetColors( m_GradientColor1, m_GradientColor2 );

  m_DoNotUpdate = false;

  ChangeColor( CC_BY_BUTTON );

	return TRUE;

}



void CColorPicker::OnDestroy() 
{
	CDialog::OnDestroy();
	
  if ( m_pImageWheel != NULL )
  {
    delete m_pImageWheel;
    m_pImageWheel = NULL;
  }
  if ( m_pPage != NULL )
  {
    delete m_pPage;
    m_pPage = NULL;
  }
}



DWORD GetWheelColor( DWORD dwPos )
{
  int   iR,iG,iB;

  HSVToRGB( dwPos,
            255, 
            255, 
            &iR,
            &iG,
            &iB );

  return ( ( iB & 0xff ) | ( ( iG << 8 ) & 0xff00 ) | ( ( iR << 16 ) & 0xff0000 ) );
}



DWORD GetBoxColor( int iHue, int j, int i )
{

  int     iR,
          iG,
          iB;


  HSVToRGB( iHue, j, i, &iR, &iG, &iB );

  return ( ( iR << 16 ) + ( iG << 8 ) + iB );

}



void CColorPicker::DrawCross( int iX, int iY )
{

  m_pPage->Line( iX - 4,
                 iY - 3,
                 iX + 3,
                 iY + 4,
                 0 );
  m_pPage->Line( iX - 3,
                 iY - 4,
                 iX + 4,
                 iY + 3,
                 0 );

  m_pPage->Line( iX + 3,
                 iY - 4,
                 iX - 4,
                 iY + 3,
                 0 );
  m_pPage->Line( iX + 4,
                 iY - 3,
                 iX - 3,
                 iY + 4,
                 0 );

  m_pPage->Line( iX - 3,
                 iY - 3,
                 iX + 3,
                 iY + 3,
                 0xffffff );
  m_pPage->Line( iX + 3,
                 iY - 3,
                 iX - 3,
                 iY + 3,
                 0xffffff );

}



void CColorPicker::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{

  for ( int i = 0; i < 6; i++ )
  {
    for ( int j = 0; j < 6; j++ )
    {
      if ( lpDrawItemStruct->hwndItem == buttonColor[i][j].GetSafeHwnd() )
      {
        if ( buttonColor[i][j].GetState() & 0x0004 )
        {
          DrawEdge( lpDrawItemStruct->hDC,
                    &lpDrawItemStruct->rcItem,
                    EDGE_SUNKEN,
                    BF_RECT );
        }
        else
        {
          DrawEdge( lpDrawItemStruct->hDC,
                    &lpDrawItemStruct->rcItem,
                    EDGE_RAISED,
                    BF_RECT );
        }

        HBRUSH brushDummy = CreateSolidBrush( ( ( m_FixedColor[i][j] & 0xff0000 ) >> 16 )
                                            | ( ( m_FixedColor[i][j] & 0x00ff00 ) )
                                            | ( ( m_FixedColor[i][j] & 0x0000ff ) << 16 ) );

        InflateRect( &lpDrawItemStruct->rcItem, -2, -2 );
        FillRect( lpDrawItemStruct->hDC,
                  &lpDrawItemStruct->rcItem,
                  brushDummy );

        DeleteObject( brushDummy );
        return;
      }
      else if ( lpDrawItemStruct->hwndItem == buttonCustomColor[i][j].GetSafeHwnd() )
      {
        if ( buttonCustomColor[i][j].GetState() & 0x0004 )
        {
          DrawEdge( lpDrawItemStruct->hDC,
                    &lpDrawItemStruct->rcItem,
                    EDGE_SUNKEN,
                    BF_RECT );
        }
        else
        {
          DrawEdge( lpDrawItemStruct->hDC,
                    &lpDrawItemStruct->rcItem,
                    EDGE_RAISED,
                    BF_RECT );
        }

        HBRUSH brushDummy = CreateSolidBrush( ( ( m_CustomColor[i][j] & 0xff0000 ) >> 16 )
                                            | ( ( m_CustomColor[i][j] & 0x00ff00 ) )
                                            | ( ( m_CustomColor[i][j] & 0x0000ff ) << 16 ) );

        InflateRect( &lpDrawItemStruct->rcItem, -2, -2 );
        FillRect( lpDrawItemStruct->hDC,
                  &lpDrawItemStruct->rcItem,
                  brushDummy );

        DeleteObject( brushDummy );
        return;
      }
    }
  }
  if ( lpDrawItemStruct->hwndItem == wndColorWheel.GetSafeHwnd() )
  {
    m_pImageWheel->PutImage( m_pPage, 0, 0, IMAGE_METHOD_PLAIN );

    // Kreuz auf ColorWheel
    float fRadius = 100.0f,
          fPos = (float)m_WheelPos;
    int   iX = 130 + (int)( ( fRadius + 5 ) * cos( fPos * 3.1415926 / 180 ) ),
          iY = 130 - (int)( ( fRadius + 5 ) * sin( fPos * 3.1415926 / 180 ) );

    DrawCross( iX, iY );

    int   iR = ( m_ChosenBoxColor >> 16 ) & 0xff,
          iG = ( m_ChosenBoxColor >>  8 ) & 0xff,
          iB = ( m_ChosenBoxColor       ) & 0xff;
    for ( int j = 0; j < 128; j++ )
    {
      for ( int i = 0; i <= 128; i++ )
      {
        m_pPage->PutPixel( 260 / 2 - 64 + j, 
                           260 / 2 - 64 + i, 
                           GetBoxColor( m_WheelPos, ( 255 * j ) / 127, ( 255 * i ) / 128 ) );
      }
    }
    // Kreuz in ColorBox
    DrawCross( 260 / 2 - 64 + m_RectPosX / 2,
               260 / 2 - 64 + m_RectPosY / 2 );

    BitBlt( lpDrawItemStruct->hDC, 0, 0, 260, 260, m_pPage->GetDC(), 0, 0, SRCCOPY );
  }
  else if ( nIDCtl == IDC_BUTTON_CP_VERLAUF_1 )
  {
    HBRUSH brushDummy = CreateSolidBrush( ( ( m_GradientColor1 & 0xff0000 ) >> 16 )
                                        | ( ( m_GradientColor1 & 0x00ff00 ) )
                                        | ( ( m_GradientColor1 & 0x0000ff ) << 16 ) );

    FillRect( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              brushDummy );

    DeleteObject( brushDummy );

    DrawEdge( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              EDGE_RAISED,
              BF_RECT );
  }
  else if ( nIDCtl == IDC_BUTTON_CP_VERLAUF_2 )
  {
    HBRUSH brushDummy = CreateSolidBrush( ( ( m_GradientColor2 & 0xff0000 ) >> 16 )
                                        | ( ( m_GradientColor2 & 0x00ff00 ) )
                                        | ( ( m_GradientColor2 & 0x0000ff ) << 16 ) );

    FillRect( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              brushDummy );

    DeleteObject( brushDummy );

    DrawEdge( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              EDGE_RAISED,
              BF_RECT );
  }
  else if ( lpDrawItemStruct->hwndItem == m_ButtonNewColor.GetSafeHwnd() )
  {
    HBRUSH brushDummy = CreateSolidBrush( ( ( m_WorkColor & 0xff0000 ) >> 16 )
                                        | ( ( m_WorkColor & 0x00ff00 ) )
                                        | ( ( m_WorkColor & 0x0000ff ) << 16 ) );

    FillRect( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              brushDummy );

    DeleteObject( brushDummy );

    DrawEdge( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              EDGE_RAISED,
              BF_RECT );
  }
  else if ( lpDrawItemStruct->hwndItem == m_ButtonOldColor.GetSafeHwnd() )
  {
    HBRUSH brushDummy = CreateSolidBrush( ( ( m_StartColor & 0xff0000 ) >> 16 )
                                        | ( ( m_StartColor & 0x00ff00 ) )
                                        | ( ( m_StartColor & 0x0000ff ) << 16 ) );

    FillRect( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              brushDummy );

    DeleteObject( brushDummy );

    DrawEdge( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              EDGE_RAISED,
              BF_RECT );
  }

}



void CColorPicker::ChangeColor( DWORD dwChangeType )
{

  if ( dwChangeType == CC_BY_HUE )
  {
    m_ChosenBoxColor = GetWheelColor( m_WheelPos );

    m_WorkColor = GetBoxColor( 
            m_WheelPos,
            m_RectPosX, 
            m_RectPosY );

    wndColorWheel.Invalidate();
  }
  else if ( ( dwChangeType == CC_BY_EDIT_RGB )
  ||        ( dwChangeType == CC_BY_EDIT_HTML ) )
  {
    RGBToHSV( ( m_WorkColor & 0xff0000 ) >> 16, 
              ( m_WorkColor & 0xff00 ) >> 8, 
              ( m_WorkColor & 0xff ), 
               &m_WheelPos, &m_RectPosX, &m_RectPosY );
    m_ChosenBoxColor = GetWheelColor( m_WheelPos );
    wndColorWheel.Invalidate();
  }
  else if ( dwChangeType == CC_BY_EDIT_HSL )
  {
    m_ChosenBoxColor = GetWheelColor( m_WheelPos );

    m_WorkColor = GetBoxColor( 
            m_WheelPos,
            m_RectPosX, 
            m_RectPosY );

    wndColorWheel.Invalidate();
  }
  else if ( dwChangeType == CC_BY_BUTTON )
  {
    RGBToHSV( ( m_WorkColor & 0xff0000 ) >> 16, 
              ( m_WorkColor & 0xff00 ) >> 8, 
              ( m_WorkColor & 0xff ), 
               &m_WheelPos, &m_RectPosX, &m_RectPosY );
    m_ChosenBoxColor = GetWheelColor( m_WheelPos );
    wndColorWheel.Invalidate();
  }
  else if ( dwChangeType == CC_BY_RGB )
  {
    m_WorkColor = GetBoxColor( 
            m_WheelPos,
            m_RectPosX, 
            m_RectPosY );
  }
  m_DoNotUpdate = true;
  if ( dwChangeType != CC_BY_EDIT_RGB )
  {
    m_REditRot.SetPosition( (int)( ( m_WorkColor >> 16 ) & 0xff ) );
    m_REditGruen.SetPosition( (int)( ( m_WorkColor >> 8 ) & 0xff ) );
    m_REditBlau.SetPosition( (int)( m_WorkColor & 0xff ) );
  }

  if ( dwChangeType != CC_BY_EDIT_HSL )
  {
    m_REditHue.SetPosition( m_WheelPos );
    m_REditSat.SetPosition( m_RectPosX );
    m_REditLight.SetPosition( m_RectPosY );
  }
  if ( dwChangeType != CC_BY_EDIT_HTML )
  {
    GR::tChar    szHex[10];

    wsprintf( szHex, _T( "#%06X" ), m_WorkColor );
    m_EditHTML.SetWindowText( szHex );
  }


  m_DoNotUpdate = false;

  m_ButtonNewColor.Invalidate();

}



void CColorPicker::OnLButtonDown( UINT nFlags, CPoint point ) 
{

  if ( ( point.x >= 120 + 260 / 2 - 64 )
  &&   ( point.y >= 10 + 260 / 2 - 64 )
  &&   ( point.x < 120 + 260 / 2 + 64 )
  &&   ( point.y <= 10 + 260 / 2 + 64 ) )
  {
    m_CaptureColorBox = TRUE;
    RECT    rc;

    rc.left = 120 + 260 / 2 - 64;
    rc.top = 10 + 260 / 2 - 64;
    rc.right = 120 + 260 / 2 + 64;
    rc.bottom = 10 + 260 / 2 + 64 + 1;

    m_RectPosX = ( ( point.x - rc.left ) * 255 ) / 127;
    m_RectPosY = ( ( point.y - rc.top ) * 255 ) / 128;

    m_WorkColor = GetBoxColor( 
            m_WheelPos,
            m_RectPosX, 
            m_RectPosY );

    ClientToScreen( &rc );
    ClipCursor( &rc );

    ChangeColor( CC_BY_HUE );
  }
  else if ( ( point.x >= 120 )
  &&        ( point.y >= 10 )
  &&        ( point.x < 120 + 260 )
  &&        ( point.y <= 10 + 260 ) )
  {
    m_CaptureColorWheel = TRUE;
    float   fWinkel;

    fWinkel = (float)atan2f( (float)-( point.y - 10 - 130 ), (float)( point.x - 120 - 130 ) ) * 180.0f / 3.1415926f;
    m_WheelPos = (int)fWinkel;
    while ( m_WheelPos < 0 )
    {
      m_WheelPos += 360;
    }

    RECT    rc;
    rc.left = 120;
    rc.top = 10;
    rc.right = 120 + 260;
    rc.bottom = 10 + 260;

    ClientToScreen( &rc );
    ClipCursor( &rc );

    ChangeColor( CC_BY_HUE );
  }

	CDialog::OnLButtonDown( nFlags, point );

}



void CColorPicker::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	ClipCursor( NULL );
	
  m_CaptureColorBox = FALSE;
  m_CaptureColorWheel = FALSE;

  if ( m_PickMode )
  {
    ReleaseCapture();
    SetCursor( LoadCursor( NULL, IDC_ARROW ) );
    m_PickMode = false;
    pSettings->SetSetting( "PickerActive", 0 );

    m_ButtonPicker.SendMessage( WM_LBUTTONUP, nFlags, MAKELPARAM( point.x, point.y ) );
  }
	CDialog::OnLButtonUp( nFlags, point );
}



void CColorPicker::OnMouseMove( UINT nFlags, CPoint point ) 
{
  if ( m_PickMode )
  {
		ClientToScreen( &point );

		HDC hScreenDC = ::GetDC( NULL );

    COLORREF    crMuell = ::GetPixel( hScreenDC, point.x, point.y );

		m_WorkColor = ( ( crMuell & 0xff0000 ) >> 16 ) + ( crMuell & 0xff00 ) + ( ( crMuell & 0xff ) << 16 );
    ChangeColor( CC_BY_BUTTON );
		::ReleaseDC(NULL,hScreenDC);

    CDialog::OnMouseMove( nFlags, point );
    return;
  }

  if ( nFlags & MK_LBUTTON )
  {
    if ( ( point.x >= 120 + 260 / 2 - 64 )
    &&   ( point.y >= 10 + 260 / 2 - 64 )
    &&   ( point.x < 120 + 260 / 2 + 64 )
    &&   ( point.y <= 10 + 260 / 2 + 64 )
    &&   ( !m_CaptureColorWheel ) )
    {
      m_CaptureColorBox = TRUE;
      m_RectPosX = ( ( point.x - ( 120 + 260 / 2 - 64 ) ) * 255 ) / 127;
      m_RectPosY = ( ( point.y - ( 10 + 260 / 2 - 64 ) ) * 255 ) / 128;

      m_WorkColor = GetBoxColor( 
              m_WheelPos,
              m_RectPosX, 
              m_RectPosY );

      RECT    rc;
      rc.left = 120 + 260 / 2 - 64;
      rc.top = 10 + 260 / 2 - 64;
      rc.right = 120 + 260 / 2 + 64;
      rc.bottom = 10 + 260 / 2 + 64 + 1;

      ClientToScreen( &rc );
      ClipCursor( &rc );

      ChangeColor( CC_BY_HUE );
    }
    else if ( ( point.x >= 120 )
    &&        ( point.y >= 10 )
    &&        ( point.x < 120 + 260 )
    &&        ( point.y <= 10 + 260 )
    &&        ( !m_CaptureColorBox ) )
    {
      m_CaptureColorWheel = TRUE;
      float   fWinkel;

      fWinkel = (float)atan2f( -(float)( point.y - 10 - 130 ), (float)( point.x - 120 - 130 ) ) * 180.0f / 3.1415926f;
      m_WheelPos = (int)fWinkel;
      while ( m_WheelPos < 0 )
      {
        m_WheelPos += 360;
      }

      RECT    rc;
      rc.left = 120;
      rc.top = 10;
      rc.right = 120 + 260;
      rc.bottom = 10 + 260;

      ClientToScreen( &rc );
      ClipCursor( &rc );

      ChangeColor( CC_BY_HUE );
    }
  }
	CDialog::OnMouseMove( nFlags, point );
}



void CColorPicker::OnActivateApp( BOOL bActive, DWORD hTask ) 
{
  Default();
	//CDialog::OnActivateApp( bActive, (DWORD)hTask );
	
  if ( !bActive )
  {
	  ClipCursor( NULL );
	  
    m_CaptureColorBox = FALSE;
    m_CaptureColorWheel = FALSE;
  }
}



BOOL CColorPicker::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
  if ( HIWORD( wParam ) == BN_CLICKED )
  {
    for ( int i = 0; i < 6; i++ )
    {
      for ( int j = 0; j < 6; j++ )
      {
        if ( (HWND)lParam == buttonColor[i][j].GetSafeHwnd() )
        {
          m_WorkColor = ( m_FixedColor[i][j] & 0xffffff );

          ChangeColor( CC_BY_BUTTON );
          return TRUE;
        }
        if ( (HWND)lParam == buttonCustomColor[i][j].GetSafeHwnd() )
        {
          m_WorkColor = ( m_CustomColor[i][j] & 0xffffff );

          ChangeColor( CC_BY_BUTTON );
          return TRUE;
        }
      }
    }
  }
	return CDialog::OnCommand( wParam, lParam );
}



void CColorPicker::OnChangeREditRot() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }
  m_DoNotUpdate = TRUE;
  CString     myString;

  m_REditRot.GetWindowText( myString );

  int iColor = GR::Convert::ToI32( (LPCTSTR)myString );

  if ( iColor < 0 )
  {
    iColor = 0;
  }
  if ( iColor > 255 )
  {
    iColor = 255;
  }

  if ( math::clamp( 0, iColor, 255 ) )
  {
    m_REditRot.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", iColor ) ).c_str() );
  }

  m_WorkColor &= 0x00ffff;
  m_WorkColor += ( ( iColor & 0xff ) << 16 );

  ChangeColor( CC_BY_EDIT_RGB );
  m_DoNotUpdate = FALSE;
}



void CColorPicker::OnChangeREditGruen() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  m_DoNotUpdate = TRUE;
  CString     myString;

  m_REditGruen.GetWindowText( myString );

  int iColor = GR::Convert::ToI32( (LPCTSTR)myString );

  if ( math::clamp( 0, iColor, 255 ) )
  {
    m_REditGruen.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", iColor ) ).c_str() );
  }


  m_WorkColor &= 0xff00ff;
  m_WorkColor += ( ( iColor & 0xff ) << 8 );

  ChangeColor( CC_BY_EDIT_RGB );
  m_DoNotUpdate = FALSE;
	
}



void CColorPicker::OnChangeREditBlau() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  m_DoNotUpdate = TRUE;
  CString     myString;

  m_REditBlau.GetWindowText( myString );

  int iColor = GR::Convert::ToI32( (LPCTSTR)myString );

  if ( math::clamp( 0, iColor, 255 ) )
  {
    m_REditBlau.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", iColor ) ).c_str() );
  }

  m_WorkColor &= 0xffff00;
  m_WorkColor += ( iColor & 0xff );

  ChangeColor( CC_BY_EDIT_RGB );
  m_DoNotUpdate = FALSE;
	
}



void CColorPicker::OnChangeREditHue() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  m_DoNotUpdate = TRUE;
  CString     myString;

  m_REditHue.GetWindowText( myString );

  int iColor = GR::Convert::ToI32( (LPCTSTR)myString );

  if ( math::clamp( 0, iColor, 360 ) )
  {
    m_REditHue.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", iColor ) ).c_str() );
  }

  m_WheelPos = iColor;

  ChangeColor( CC_BY_EDIT_HSL );
  m_DoNotUpdate = FALSE;
	
}



void CColorPicker::OnChangeREditSaturation() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  m_DoNotUpdate = TRUE;
  CString     myString;

  m_REditSat.GetWindowText( myString );

  int iColor = GR::Convert::ToI32( (LPCTSTR)myString );

  if ( math::clamp( 0, iColor, 255 ) )
  {
    m_REditSat.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", iColor ) ).c_str() );
  }

  m_RectPosX = iColor;

  ChangeColor( CC_BY_EDIT_HSL );
  m_DoNotUpdate = FALSE;
	
}



void CColorPicker::OnChangeREditLight() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  m_DoNotUpdate = TRUE;
  CString     myString;

  m_REditLight.GetWindowText( myString );

  int iColor = GR::Convert::ToU32( (LPCTSTR)myString );

  if ( math::clamp( 0, iColor, 255 ) )
  {
    m_REditLight.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", iColor ) ).c_str() );
  }

  m_RectPosY = iColor;

  ChangeColor( CC_BY_EDIT_HSL );
  m_DoNotUpdate = FALSE;
	
}



void CColorPicker::OnRButtonDown( UINT nFlags, CPoint point ) 
{

  
	CDialog::OnRButtonDown( nFlags, point );

}



BOOL CColorPicker::PreTranslateMessage( MSG* pMsg ) 
{
  if ( m_PickMode )
  {
    if ( pMsg->message == WM_LBUTTONDOWN )
    {
      POINT   point;
      GetCursorPos( &point );

		  HDC hScreenDC = ::GetDC( NULL );

      COLORREF    crMuell = ::GetPixel( hScreenDC, point.x, point.y );

		  m_WorkColor = ( ( crMuell & 0xff0000 ) >> 16 ) + ( crMuell & 0xff00 ) + ( ( crMuell & 0xff ) << 16 );
      ChangeColor( CC_BY_BUTTON );
		  ::ReleaseDC(NULL,hScreenDC);

      ReleaseCapture();
      SetCursor( LoadCursor( NULL, IDC_ARROW ) );
      m_PickMode = false;
      pSettings->SetSetting( "PickerActive", 0 );
      return TRUE;
    }
  }
  if ( pMsg->message == WM_RBUTTONDOWN )
  {
    for ( int i = 0; i < 6; i++ )
    {
      for ( int j = 0; j < 6; j++ )
      {
        if ( pMsg->hwnd == buttonCustomColor[i][j].GetSafeHwnd() )
        {
          m_CustomColor[i][j] = m_WorkColor;
          buttonCustomColor[i][j].Invalidate();
          pSettings->SetSetting( CMisc::printf( "CustomColor%d%d", i, j ), m_WorkColor );
          return TRUE;
        }
      }
    }
  }
	
	return CDialog::PreTranslateMessage( pMsg );
}



void CColorPicker::OnPickerPushed() 
{
  if ( m_PickMode )
  {
    return;
  }

  // Pick-Modus
  m_PickMode = true;
  pSettings->SetSetting( "PickerActive", 1 );
  SetCursor( LoadCursor( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDC_CURSOR_PICKER ) ) );
  SetCapture();
}



void CColorPicker::OnButtonCpOldColor() 
{
  m_WorkColor = m_StartColor;
  ChangeColor( CC_BY_BUTTON );
}



void CColorPicker::OnChangeEditCpHtml() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditHTML.GetWindowText( cstrGnu );

  GR::tString   strColor = LPCTSTR( cstrGnu );

  if ( strColor.length() >= 7 )
  {
    strColor = strColor.substr( 0, 7 );
  }
  if ( ( strColor.length() )
  &&   ( strColor[0] == '#' ) )
  {
    strColor = strColor.substr( 1 );
  }
  
  char    szHex[20] = "0123456789ABCDEF";

  DWORD   dwColor = 0;

  for ( size_t i = 0; i < 6; i++ )
  {
    if ( strColor.length() >= i + 1 )
    {
      char    cChar = toupper( strColor[i] );

      dwColor <<= 4;

      for ( int j = 0; j < 16; j++ )
      {
        if ( cChar == szHex[j] )
        {
          dwColor |= j;
        }
      }
    }
  }
  m_WorkColor = dwColor;
  ChangeColor( CC_BY_EDIT_HTML );
}



void CColorPicker::OnBnClickedButtonCpVerlauf2()
{
  m_GradientColor2 = m_WorkColor;
  m_buttonGradient2.Invalidate();

  pSettings->SetSetting( "ColorPickerGradient2", m_GradientColor2 );

  m_REditVerlauf.SetColors( m_GradientColor1, m_GradientColor2 );
}



void CColorPicker::OnBnClickedButtonCpVerlauf1()
{
  m_GradientColor1 = m_WorkColor;
  m_buttonGradient1.Invalidate();

  pSettings->SetSetting( "ColorPickerGradient1", m_GradientColor1 );

  m_REditVerlauf.SetColors( m_GradientColor1, m_GradientColor2 );
}



void CColorPicker::OnChangeREditVerlauf() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }
  m_DoNotUpdate = TRUE;

  CString     myString;

  m_REditVerlauf.GetWindowText( myString );

  int iValue = GR::Convert::ToI32( (LPCTSTR)myString );

  m_WorkColor = GFX::ColorGradient( m_GradientColor1,
                                           m_GradientColor2,
                                           iValue * 100 / 255 );

  ChangeColor( CC_BY_GRADIENT );
  m_DoNotUpdate = FALSE;
	
}



