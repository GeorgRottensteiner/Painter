#include "stdafx.h"
#include "EffektPixelize.h"
#include "DlgEffekte.h"

#include <String/Convert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektPixelize::CEffektPixelize(CWnd* pParent /*=NULL*/)
	: EffektTemplate(CEffektPixelize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffektPixelize)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CEffektPixelize::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektPixelize)
	DDX_Control(pDX, IDC_EDIT_TIEFE, m_EditTiefe);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektPixelize, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektPixelize)
	ON_EN_CHANGE(IDC_EDIT_TIEFE, OnChangeEditTiefe)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEffektPixelize 



void CEffektPixelize::DoEffekt()
{

  int         i,
              j,
              iFaktor,
              iSize,
              iR,
              iG,
              iB;


  iFaktor = m_iTiefe;
  if ( iFaktor < 1 )
  {
    iFaktor = 1;
  }
  if ( iFaktor >= 16 )
  {
    iFaktor = 16;
  }

  DWORD   dwPixel;

  iSize = 2;
  while ( iFaktor )
  {
    for ( j = 0; j < m_pCDSource->Height() / iSize; j++ )
    {
      for ( i = 0; i < m_pCDSource->Width() / iSize; i++ )
      {
        dwPixel = m_pCDLastFrame->GetPixel( i * iSize, j * iSize );
        iR = ( ( dwPixel & 0xff0000 ) >> 16 );
        iG = ( ( dwPixel & 0xff00 ) >> 8 );
        iB = ( dwPixel & 0xff );

        dwPixel = m_pCDLastFrame->GetPixel( ( i + 1 ) * iSize - 1, j * iSize );
        iR += ( ( dwPixel & 0xff0000 ) >> 16 );
        iG += ( ( dwPixel & 0xff00 ) >> 8 );
        iB += ( dwPixel & 0xff );

        dwPixel = m_pCDLastFrame->GetPixel( i * iSize, ( j + 1 ) * iSize - 1 );
        iR += ( ( dwPixel & 0xff0000 ) >> 16 );
        iG += ( ( dwPixel & 0xff00 ) >> 8 );
        iB += ( dwPixel & 0xff );

        dwPixel = m_pCDLastFrame->GetPixel( ( i + 1 ) * iSize - 1, ( j + 1 ) * iSize - 1 );
        iR += ( ( dwPixel & 0xff0000 ) >> 16 );
        iG += ( ( dwPixel & 0xff00 ) >> 8 );
        iB += ( dwPixel & 0xff );

        iR >>= 2;
        iG >>= 2;
        iB >>= 2;

        m_pCDTarget->Box( i * iSize, j * iSize, iSize, iSize, ( iR << 16 ) + ( iG << 8 ) + iB );

        // gelegentliches Bild-Update
        if ( !m_pDlg->UpdateThread() )
        {
          return;
        }
      }
    }

    NextFrame();

    iFaktor--;
    iSize <<= 1;
  }

}



void CEffektPixelize::OnChangeEditTiefe() 
{

	if ( m_DoNotUpdate )
  {
    return;
  }

  CString   cstrGnu;

  m_EditTiefe.GetWindowText( cstrGnu );
  m_iTiefe = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();
	
}



BOOL CEffektPixelize::OnInitDialog() 
{

	EffektTemplate::OnInitDialog();
	
  m_DoNotUpdate = TRUE;

  m_iTiefe = 1;
  
  m_EditTiefe.SetWindowText( _T( "1" ) );

  m_DoNotUpdate = FALSE;

	return TRUE;

}

