#include "stdafx.h"
#include "EffektGrayScale.h"
#include "DlgEffekte.h"
#include "PainterGfxTools.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektGrayScale::CEffektGrayScale(CWnd* pParent /*=NULL*/)
	: EffektTemplate(IDD,pParent)
{
	//{{AFX_DATA_INIT(CEffektGrayScale)
	m_iGrayValue = 0;
  m_iGrayMode = 0;
	//}}AFX_DATA_INIT

}


void CEffektGrayScale::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektGrayScale)
	DDX_Control(pDX, IDC_COMBO_GRAY_MODE, m_ComboGrayMode);
	DDX_Control(pDX, IDC_REDIT_GRAYSCALE, m_REditGrayScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektGrayScale, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektGrayScale)
	ON_EN_CHANGE(IDC_REDIT_GRAYSCALE, OnChangeEditGrayscale)
	ON_CBN_SELCHANGE(IDC_COMBO_GRAY_MODE, OnSelchangeComboGrayMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEffektGrayScale 

void CEffektGrayScale::OnChangeEditGrayscale() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }

  CString   cstrGnu;

  m_REditGrayScale.GetWindowText( cstrGnu );
  m_iGrayValue = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();
	
}



void CEffektGrayScale::DoEffekt()
{

  int         i,
              j,
              iFaktor,
              iFaktor2,
              iR,
              iG,
              iB;

  DWORD       dwPixel;


  iFaktor2 = m_iGrayValue;
  if ( iFaktor2 < 0 )
  {
    iFaktor2 = 0;
  }
  if ( iFaktor2 > 100 )
  {
    iFaktor2 = 100;
  }

  for ( j = 0; j < m_pCDSource->Height(); j++ )
  {
    for ( i = 0; i < m_pCDSource->Width(); i++ )
    {
      dwPixel = m_pCDSource->GetPixel( i, j );
      iR = ( ( dwPixel & 0xff0000 ) >> 16 );
      iG = ( ( dwPixel & 0xff00 ) >> 8 );
      iB = ( dwPixel & 0xff );

      if ( m_iGrayMode == 0 )
      {
        iFaktor = ( iR * 11 + iG * 80 + iB * 9 ) / 100;
        if ( iFaktor > 255 )
        {
          iFaktor = 255;
        }
      }
      else if ( m_iGrayMode == 1 )
      {
        iFaktor = ( iR + iG + iB ) / 3;
      }
      iR = ( ( iFaktor * iFaktor2 ) + ( iR * ( 100 - iFaktor2 ) ) ) / 100;
      iG = ( ( iFaktor * iFaktor2 ) + ( iG * ( 100 - iFaktor2 ) ) ) / 100;
      iB = ( ( iFaktor * iFaktor2 ) + ( iB * ( 100 - iFaktor2 ) ) ) / 100;

      m_pCDTarget->PutPixel( i, j, ( iR << 16 ) + ( iG << 8 ) + iB );
      // gelegentliches Bild-Update
      if ( !m_pDlg->UpdateThread() )
      {
        return;
      }
    }
  }

}

BOOL CEffektGrayScale::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();
	
  m_DoNotUpdate = TRUE;

  m_iGrayValue = 100;
  m_REditGrayScale.SetPosition( 100 );

  m_ComboGrayMode.AddString( _T( "Kontrast" ) );
  m_ComboGrayMode.AddString( _T( "Mittelwert" ) );
  m_ComboGrayMode.SetCurSel( 0 );

  m_DoNotUpdate = FALSE;
	
	return TRUE;
}



void CEffektGrayScale::OnSelchangeComboGrayMode() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  m_iGrayMode =   m_ComboGrayMode.GetCurSel();
  RestartRendering();
	
}
