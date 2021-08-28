// EffektGrayScale.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "EffektTileable.h"
#include "DlgEffekte.h"
#include "PainterGfxTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEffektGrayScale 


CEffektTileable::CEffektTileable(CWnd* pParent /*=NULL*/)
	: EffektTemplate(IDD,pParent)
{
	//{{AFX_DATA_INIT(CEffektGrayScale)
	//}}AFX_DATA_INIT

}


void CEffektTileable::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektTileable)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektTileable, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektTileable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEffektGrayScale 



void CEffektTileable::DoEffekt()
{


  int         iR,
              iG,
              iB,
              iR2,
              iG2,
              iB2,
              iR3,
              iG3,
              iB3,
              iFaktor,
              iFaktor2;

  DWORD       dwPixel;


  for ( int j = 0; j < m_pCDSource->Height(); j++ )
  {
    for ( int i = 0; i < m_pCDSource->Width(); i++ )
    {
      dwPixel = m_pCDSource->GetPixel( i, j );
      iR = ( ( dwPixel & 0xff0000 ) >> 16 );
      iG = ( ( dwPixel & 0xff00 ) >> 8 );
      iB = ( dwPixel & 0xff );

      dwPixel = m_pCDSource->GetPixel( m_pCDSource->Width() - 1 - i, j );
      iR2 = ( ( dwPixel & 0xff0000 ) >> 16 );
      iG2 = ( ( dwPixel & 0xff00 ) >> 8 );
      iB2 = ( dwPixel & 0xff );

      dwPixel = m_pCDSource->GetPixel( i, m_pCDSource->Height() - 1 - j );
      iR3 = ( ( dwPixel & 0xff0000 ) >> 16 );
      iG3 = ( ( dwPixel & 0xff00 ) >> 8 );
      iB3 = ( dwPixel & 0xff );

      iFaktor = abs( m_pCDSource->Width() / 2 - i ) * 100 / ( m_pCDSource->Width() / 2 );
      iFaktor2 = abs( m_pCDSource->Height() / 2 - j ) * 100 / ( m_pCDSource->Height() / 2 );

      if ( ( iFaktor != 0 )
      ||   ( iFaktor2 != 0 ) )
      {
        iR = ( iFaktor * iR + iFaktor2 * iR + iR2 * iFaktor + iR3 * iFaktor2 ) / ( iFaktor * 2 + iFaktor2 * 2 );
        iG = ( iFaktor * iG + iFaktor2 * iG + iG2 * iFaktor + iG3 * iFaktor2 ) / ( iFaktor * 2 + iFaktor2 * 2 );
        iB = ( iFaktor * iB + iFaktor2 * iB + iB2 * iFaktor + iB3 * iFaktor2 ) / ( iFaktor * 2 + iFaktor2 * 2 );
      }

      /*
      if ( i < m_pCDSource->Width() / 2 )
      {
        iR = ( ( iFaktor * iR ) + ( iR2 * ( 100 - iFaktor ) ) ) / 100;
        iG = ( ( iFaktor * iG ) + ( iG2 * ( 100 - iFaktor ) ) ) / 100;
        iB = ( ( iFaktor * iB ) + ( iB2 * ( 100 - iFaktor ) ) ) / 100;
      }
      else
      {
        iR = ( ( iFaktor * iR2 ) + ( iR * ( 100 - iFaktor ) ) ) / 100;
        iG = ( ( iFaktor * iG2 ) + ( iG * ( 100 - iFaktor ) ) ) / 100;
        iB = ( ( iFaktor * iB2 ) + ( iB * ( 100 - iFaktor ) ) ) / 100;
      }
      */

      m_pCDTarget->PutPixel( i, j, ( iR << 16 ) + ( iG << 8 ) + iB );
      // gelegentliches Bild-Update
      if ( !m_pDlg->UpdateThread() )
      {
        return;
      }
    }
  }

}

BOOL CEffektTileable::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();
	
  m_DoNotUpdate = TRUE;


  m_DoNotUpdate = FALSE;
	
	return TRUE;
}



