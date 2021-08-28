#include "stdafx.h"
#include "DlgEffekte.h"
#include "EffektColorDiffusor.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektColorDiffusor::CEffektColorDiffusor(CWnd* pParent /*=NULL*/)
	: EffektTemplate(CEffektColorDiffusor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffektColorDiffusor)
	//}}AFX_DATA_INIT
}


void CEffektColorDiffusor::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektColorDiffusor)
	DDX_Control(pDX, IDC_COMBO_AUSRICHTUNG, m_ComboAusrichtung);
	DDX_Control(pDX, IDC_EDIT_BREITE, m_EditBreite);
	DDX_Control(pDX, IDC_CHECK_GREEN, m_CheckGreen);
	DDX_Control(pDX, IDC_CHECK_BLUE, m_CheckBlue);
	DDX_Control(pDX, IDC_CHECK_RED, m_CheckRed);
	DDX_Control(pDX, IDC_RADIO_BOTH_DIRECTIONS, m_RadioBoth);
	DDX_Control(pDX, IDC_RADIO_UP, m_RadioUp);
	DDX_Control(pDX, IDC_RADIO_DOWN, m_RadioDown);
	DDX_Control(pDX, IDC_EDIT_ABWEICHUNG, m_EditAbweichung);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektColorDiffusor, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektColorDiffusor)
	ON_EN_CHANGE(IDC_EDIT_ABWEICHUNG, OnChangeEditAbweichung)
	ON_BN_CLICKED(IDC_CHECK_RED, OnCheckRed)
	ON_BN_CLICKED(IDC_CHECK_GREEN, OnCheckGreen)
	ON_BN_CLICKED(IDC_CHECK_BLUE, OnCheckBlue)
	ON_BN_CLICKED(IDC_RADIO_BOTH_DIRECTIONS, OnRadioBothDirections)
	ON_BN_CLICKED(IDC_RADIO_UP, OnRadioUp)
	ON_BN_CLICKED(IDC_RADIO_DOWN, OnRadioDown)
	ON_EN_CHANGE(IDC_EDIT_BREITE, OnChangeEditBreite)
	ON_CBN_SELCHANGE(IDC_COMBO_AUSRICHTUNG, OnSelchangeComboAusrichtung)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CEffektColorDiffusor::DoEffekt()
{
  int iWidth   = m_pCDSource->Width(),
      iHeight  = m_pCDSource->Height(),
      iR,
      iG,
      iB;

  DWORD   dwPixel;

  int   i = 0,
        j = 0,
        iZufallR = 0,
        iZufallG = 0,
        iZufallB = 0,
        iDX = 1,
        iDY = 1;

  if ( m_Ausrichtung == 0 )
  {
    iDX = m_Width;
    i = -rand() % m_Width;
  }
  else if ( m_Ausrichtung == 1 )
  {
    iDY = m_Width;
    j = -rand() % m_Width;
  }


  while ( ( j < iHeight )
  ||      ( i < iWidth ) )
  {
    if ( m_ChangeRed )
    {
      switch ( m_Direction )
      {
        case 0:
          iZufallR = ( rand() % ( 2 * m_Abweichung ) ) - m_Abweichung;
          break;
        case 1:
          iZufallR = ( rand() % m_Abweichung );
          break;
        case 2:
          iZufallR = -( rand() % m_Abweichung );
          break;
      }
    }
    if ( m_ChangeGreen )
    {
      switch ( m_Direction )
      {
        case 0:
          iZufallG = ( rand() % ( 2 * m_Abweichung ) ) - m_Abweichung;
          break;
        case 1:
          iZufallG = ( rand() % m_Abweichung );
          break;
        case 2:
          iZufallG = -( rand() % m_Abweichung );
          break;
      }
    }
    if ( m_ChangeBlue )
    {
      switch ( m_Direction )
      {
        case 0:
          iZufallB = ( rand() % ( 2 * m_Abweichung ) ) - m_Abweichung;
          break;
        case 1:
          iZufallB = ( rand() % m_Abweichung );
          break;
        case 2:
          iZufallB = -( rand() % m_Abweichung );
          break;
      }
    }

    for ( int iX = 0; iX < iDX; iX++ )
    {
      for ( int iY = 0; iY < iDY; iY++ )
      {
        dwPixel = m_pCDSource->GetPixel( i + iX, j + iY );

        iR = ( dwPixel & 0xff0000 ) >> 16;
        iG = ( dwPixel & 0x00ff00 ) >> 8;
        iB = ( dwPixel & 0x0000ff );

        iR += iZufallR;
        iG += iZufallG;
        iB += iZufallB;

        if ( iR < 0 )
        {
          iR = 0;
        }
        if ( iR > 255 )
        {
          iR = 255;
        }
        if ( iG < 0 )
        {
          iG = 0;
        }
        if ( iG > 255 )
        {
          iG = 255;
        }

        if ( iB < 0 )
        {
          iB = 0;
        }
        if ( iB > 255 )
        {
          iB = 255;
        }


        m_pCDTarget->PutPixel( i + iX, j + iY, ( iR << 16 ) + ( iG << 8 ) + iB );
      }
    }

    if ( !m_pDlg->UpdateThread() )
    {
      return;
    }

    if ( m_Ausrichtung == 0 )
    {
      i += iDX;
      if ( i > iWidth )
      {
        i = -rand() % m_Width;
        j += iDY;
      }
    }
    else if ( m_Ausrichtung == 1 )
    {
      j += iDY;
      if ( j > iHeight )
      {
        j = -rand() % m_Width;
        i += iDX;
      }
    }
  }
}



BOOL CEffektColorDiffusor::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();
	
  m_DoNotUpdate = TRUE;

  m_EditAbweichung.SetWindowText( _T( "5" ) );
  m_Abweichung = 5;

  m_Direction = 0;
  m_RadioBoth.SetCheck( 1 );

  m_CheckRed.SetCheck( 1 );
  m_ChangeRed = true;
  m_CheckGreen.SetCheck( 1 );
  m_ChangeGreen = true;
  m_CheckBlue.SetCheck( 1 );
  m_ChangeBlue = true;

  m_EditBreite.SetWindowText( _T( "1" ) );
  m_Width = 1;

  m_ComboAusrichtung.AddString( _T( "Horizontal" ) );
  m_ComboAusrichtung.AddString( _T( "Vertikal" ) );
  m_Ausrichtung = 0;
  m_ComboAusrichtung.SetCurSel( 0 );

  m_DoNotUpdate = FALSE;
	
	return TRUE;
}



void CEffektColorDiffusor::OnChangeEditAbweichung() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }

  CString   cstrGnu;
  m_EditAbweichung.GetWindowText( cstrGnu );
  m_Abweichung = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );
  if ( m_Abweichung <= 0 )
  {
    m_Abweichung = 1;
  }

  RestartRendering();
}



void CEffektColorDiffusor::OnCheckRed() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }

  if ( m_CheckRed.GetCheck() )
  {
    m_ChangeRed = true;
  }
  else
  {
    m_ChangeRed = false;
  }

  RestartRendering();
}



void CEffektColorDiffusor::OnCheckGreen() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }

  if ( m_CheckGreen.GetCheck() )
  {
    m_ChangeGreen = true;
  }
  else
  {
    m_ChangeGreen = false;
  }

  RestartRendering();
}



void CEffektColorDiffusor::OnCheckBlue() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }

  if ( m_CheckBlue.GetCheck() )
  {
    m_ChangeBlue = true;
  }
  else
  {
    m_ChangeBlue = false;
  }

  RestartRendering();
}



void CEffektColorDiffusor::OnRadioBothDirections() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }

  if ( m_RadioBoth.GetCheck() )
  {
    m_Direction = 0;
    RestartRendering();
  }
}



void CEffektColorDiffusor::OnRadioUp() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }

  if ( m_RadioUp.GetCheck() )
  {
    m_Direction = 1;
    RestartRendering();
  }
}



void CEffektColorDiffusor::OnRadioDown() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }

  if ( m_RadioDown.GetCheck() )
  {
    m_Direction = 2;
    RestartRendering();
  }
}



void CEffektColorDiffusor::OnChangeEditBreite() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }

  CString   cstrGnu;
  m_EditBreite.GetWindowText( cstrGnu );
  m_Width = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );
  if ( m_Width <= 0 )
  {
    m_Width = 1;
  }

  RestartRendering();
}



void CEffektColorDiffusor::OnSelchangeComboAusrichtung() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }

  m_Ausrichtung = m_ComboAusrichtung.GetCurSel();

  RestartRendering();
}
