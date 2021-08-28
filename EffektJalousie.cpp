#include "stdafx.h"
#include "EffektJalousie.h"
#include "DlgEffekte.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektJalousie::CEffektJalousie(CWnd* pParent /*=NULL*/)
	: EffektTemplate(IDD,pParent)
{
	//{{AFX_DATA_INIT(CEffektJalousie)
	//}}AFX_DATA_INIT
}


void CEffektJalousie::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektJalousie)
	DDX_Control(pDX, IDC_COMBO_AUSRICHTUNG, m_ComboAusrichtung);
	DDX_Control(pDX, IDC_COMBO_RICHTUNG, m_ComboRichtung);
	DDX_Control(pDX, IDC_EDIT_BREITE, m_EditBreite);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektJalousie, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektJalousie)
	ON_EN_CHANGE(IDC_EDIT_BREITE, OnChangeEditBreite)
	ON_CBN_SELCHANGE(IDC_COMBO_RICHTUNG, OnSelchangeComboRichtung)
	ON_CBN_SELCHANGE(IDC_COMBO_AUSRICHTUNG, OnSelchangeComboAusrichtung)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEffektJalousie 

void CEffektJalousie::OnChangeEditBreite() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }

  CString   cstrGnu;

  m_EditBreite.GetWindowText( cstrGnu );
  m_iBreite = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();
	
}

void CEffektJalousie::OnSelchangeComboRichtung() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }

  if ( m_ComboRichtung.GetCurSel() )
  {
    m_bVorwaerts = FALSE;    
  }
  else
  {
    m_bVorwaerts = TRUE;
  }

  RestartRendering();
	
}

void CEffektJalousie::OnSelchangeComboAusrichtung() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }

  if ( m_ComboAusrichtung.GetCurSel() )
  {
    m_bVertikal = FALSE;
  }
  else
  {
    m_bVertikal = TRUE;
  }

  RestartRendering();
	
}

BOOL CEffektJalousie::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();

  m_DoNotUpdate = TRUE;

  m_bVertikal   = TRUE;
  m_bVorwaerts  = TRUE;
  m_iBreite     = 10;

  m_EditBreite.SetWindowText( _T( "10" ) );

  m_ComboRichtung.AddString( _T( "Vorwärts" ) );
  m_ComboRichtung.AddString( _T( "Rückwärts" ) );
  m_ComboRichtung.SetCurSel( 0 );

  m_ComboAusrichtung.AddString( _T( "vertikal" ) );
  m_ComboAusrichtung.AddString( _T( "horizontal" ) );
  m_ComboAusrichtung.SetCurSel( 0 );

  m_DoNotUpdate = FALSE;
	
	return TRUE;

}



void CEffektJalousie::DoEffekt()
{

  int         i,
              j,
              iR,
              iG,
              iB,
              iROrig,
              iGOrig,
              iBOrig,
              iFaktor;

  DWORD       dwPixel;


  DWORD       dwNewColor = pSettings->GetRGBColor( CSettings::CO_WORKCOLOR );

  iROrig = ( dwNewColor & 0xff0000 ) >> 16;
  iGOrig = ( dwNewColor & 0x00ff00 ) >> 8;
  iBOrig = ( dwNewColor & 0x0000ff );


  if ( m_bVertikal )
  {
    // horizontal
    for ( j = 0; j < m_pCDSource->Height(); j++ )
    {
      if ( m_iBreite == 0 )
      {
        iFaktor = 0;
      }
      else
      {
        if ( m_bVorwaerts )
        {
          // vorwärts
          iFaktor = ( ( j % m_iBreite ) * 256 ) / ( m_iBreite );
        }
        else
        {
          // rückwärts
          iFaktor = ( ( m_iBreite - 1 - j % m_iBreite ) * 256 ) / ( m_iBreite );
        }
      }
      for ( i = 0; i < m_pCDSource->Width(); i++ )
      {
        dwPixel = m_pCDSource->GetPixel( i, j );
        iR = ( ( dwPixel & 0xff0000 ) >> 16 );
        iG = ( ( dwPixel & 0xff00 ) >> 8 );
        iB = ( dwPixel & 0xff );

        iR = ( ( iR * iFaktor ) + ( iROrig * ( 256 - iFaktor ) ) ) / 256;
        iG = ( ( iG * iFaktor ) + ( iGOrig * ( 256 - iFaktor ) ) ) / 256;
        iB = ( ( iB * iFaktor ) + ( iBOrig * ( 256 - iFaktor ) ) ) / 256;

        m_pCDTarget->PutPixel( i, j, ( iR << 16 ) + ( iG << 8 ) + iB );
      }
      // gelegentliches Bild-Update
      if ( !m_pDlg->UpdateThread() )
      {
        return;
      }
    }
  }
  else
  {
    for ( i = 0; i < m_pCDSource->Width(); i++ )
    {
      if ( m_iBreite == 0 )
      {
        iFaktor = 0;
      }
      else
      {
        if ( m_iBreite == 0 )
        {
          iFaktor = 0;
        }
        else
        {
          if ( m_bVorwaerts == 0 )
          {
            // vorwärts
            iFaktor = ( ( i % m_iBreite ) * 256 ) / ( m_iBreite );
          }
          else
          {
            // rückwärts
            iFaktor = ( ( m_iBreite - 1 - i % m_iBreite ) * 256 ) / ( m_iBreite );
          }
        }
      }
      for ( j = 0; j < m_pCDSource->Height(); j++ )
      {
        dwPixel = m_pCDSource->GetPixel( i, j );
        iR = ( ( dwPixel & 0xff0000 ) >> 16 );
        iG = ( ( dwPixel & 0xff00 ) >> 8 );
        iB = ( dwPixel & 0xff );
        iR = ( ( iR * iFaktor ) + ( iROrig * ( 256 - iFaktor ) ) ) / 256;
        iG = ( ( iG * iFaktor ) + ( iGOrig * ( 256 - iFaktor ) ) ) / 256;
        iB = ( ( iB * iFaktor ) + ( iBOrig * ( 256 - iFaktor ) ) ) / 256;

        m_pCDTarget->PutPixel( i, j, ( iR << 16 ) + ( iG << 8 ) + iB );
        // gelegentliches Bild-Update
        if ( !m_pDlg->UpdateThread() )
        {
          return;
        }
      }
    }
  }

}