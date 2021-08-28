#include "stdafx.h"
#include "EffektColorize.h"
#include "DlgEffekte.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektColorize::CEffektColorize(CWnd* pParent /*=NULL*/)
	: EffektTemplate(IDD,pParent)
{
	//{{AFX_DATA_INIT(CEffektColorize)
  m_GrayMode = 0;
	//}}AFX_DATA_INIT
}


void CEffektColorize::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektColorize)
	DDX_Control(pDX, IDC_COMBO_GRAY_MODE, m_ComboGrayMode);
	DDX_Control(pDX, IDC_REDIT_SAETTIGUNG, m_REditSaettigung);
  DDX_Control(pDX, IDC_REDIT_FARBWERT,  m_REditFarbwert );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektColorize, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektColorize)
	ON_EN_CHANGE(IDC_REDIT_SAETTIGUNG, OnChangeREditSaettigung)
  ON_EN_CHANGE(IDC_REDIT_FARBWERT, OnChangeREditFarbwert)
	ON_CBN_SELCHANGE(IDC_COMBO_GRAY_MODE, OnSelchangeComboGrayMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CEffektColorize::DoEffekt()
{
  int         i,
              j,
              iFaktor,
              iFaktor2,
              iFarbWert,
              iFarbR,
              iFarbG,
              iFarbB,
              iR,
              iG,
              iB;


  iFarbWert = m_FarbWert;
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


  iFaktor2 = m_Saettigung;
  if ( iFaktor2 < 0 )
  {
    iFaktor2 = 0;
  }
  if ( iFaktor2 > 100 )
  {
    iFaktor2 = 100;
  }

  DWORD   dwPixel;

  for ( j = 0; j < m_pCDSource->Height(); j++ )
  {
    for ( i = 0; i < m_pCDSource->Width(); i++ )
    {
      dwPixel = m_pCDSource->GetPixel( i, j );
      iR = ( ( dwPixel & 0xff0000 ) >> 16 );
      iG = ( ( dwPixel & 0xff00 ) >> 8 );
      iB = ( dwPixel & 0xff );

      if ( m_GrayMode == 0 )
      {
        iFaktor = ( iR * 11 + iG * 80 + iB * 9 ) / 100;
        if ( iFaktor > 255 )
        {
          iFaktor = 255;
        }
      }
      else if ( m_GrayMode == 1 )
      {
        iFaktor = ( iR + iG + iB ) / 3;
      }
      iR = ( ( ( iFaktor * iFarbR / 255 ) * iFaktor2 ) + ( iR * ( 100 - iFaktor2 ) ) ) / 100;
      iG = ( ( ( iFaktor * iFarbG / 255 ) * iFaktor2 ) + ( iG * ( 100 - iFaktor2 ) ) ) / 100;
      iB = ( ( ( iFaktor * iFarbB / 255 ) * iFaktor2 ) + ( iB * ( 100 - iFaktor2 ) ) ) / 100;

      m_pCDTarget->PutPixel( i, j, ( iR << 16 ) + ( iG << 8 ) + iB );

      // gelegentliches Bild-Update
      if ( !m_pDlg->UpdateThread() )
      {
        return;
      }
    }
  }

}



void CEffektColorize::OnChangeREditFarbwert() 
{

	if ( m_DoNotUpdate )
  {
    return;
  }

  CString   cstrGnu;

  m_REditFarbwert.GetWindowText( cstrGnu );
  m_FarbWert = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );

  RestartRendering();
}



void CEffektColorize::OnChangeREditSaettigung() 
{
	if ( m_DoNotUpdate )
  {
    return;
  }

  CString   cstrGnu;

  m_REditSaettigung.GetWindowText( cstrGnu );
  m_Saettigung = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );

  RestartRendering();
}



BOOL CEffektColorize::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();
	
  m_DoNotUpdate = TRUE;

  m_FarbWert = 128;
  m_Saettigung = 100;

  m_REditFarbwert.SetPosition( 128 );
  m_REditSaettigung.SetPosition( 100 );

  m_ComboGrayMode.AddString( _T( "Kontrast" ) );
  m_ComboGrayMode.AddString( _T( "Mittelwert" ) );
  m_ComboGrayMode.SetCurSel( 0 );

  m_DoNotUpdate = FALSE;

	return TRUE;
}



void CEffektColorize::OnSelchangeComboGrayMode() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }
  m_GrayMode =   m_ComboGrayMode.GetCurSel();
  RestartRendering();
}
