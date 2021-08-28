#include "stdafx.h"
#include "EffektDiffusor.h"
#include "DlgEffekte.h"

#include <pjlib/math/pj_math_random.h>

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CEffektDiffusor::CEffektDiffusor(CWnd* pParent /*=NULL*/)
	: EffektTemplate(IDD,pParent)
{
	//{{AFX_DATA_INIT(CEffektDiffusor)
	m_dwIterations = 0;
	//}}AFX_DATA_INIT
}


void CEffektDiffusor::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektDiffusor)
	DDX_Control(pDX, IDC_EDIT_ITERATIONS, m_EditIterations);
	DDX_Text(pDX, IDC_EDIT_ITERATIONS, m_dwIterations);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektDiffusor, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektDiffusor)
	ON_EN_CHANGE(IDC_EDIT_ITERATIONS, OnChangeEditIterations)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEffektDiffusor 



/*-DoEffekt-------------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CEffektDiffusor::DoEffekt()
{

  int y, x;
  int width   = m_pCDSource->Width(),
      height  = m_pCDSource->Height();

  int cycle = m_dwIterations;
  
  // //- Ränder ausnullen
  // unsigned char* pCleaner; 
  // for ( pCleaner = pTarget,           y = 0; y < height; ++y, pCleaner+=lineoffset ) *pCleaner = 0;
  // for ( pCleaner = pTarget + width-1, y = 0; y < height; ++y, pCleaner+=lineoffset ) *pCleaner = 0;
  // for ( pCleaner = pTarget,                               x = 0; x < width; ++x ) *pCleaner++ = 0;
  // for ( pCleaner = pTarget + ( height - 1 ) * lineoffset, x = 0; x < width; ++x ) *pCleaner++ = 1;

  static int blockoffA[ 4 ], blockoffAs[ 4 ][ 2 ],
             blockoffAy[ 4 ], blockoffAsy[ 4 ][ 2 ];
  static int blockoffB[ 4 ], blockoffBs[ 4 ][ 2 ],
             blockoffBy[ 4 ], blockoffBsy[ 4 ][ 2 ];


  //- Destination-Offsets für A-Blocks
  blockoffA[  0 ] =   0;                    //- ich
  blockoffA[  1 ] = + 1;                    //- rechts von mir
  blockoffA[  2 ] = + 1 ;       //- rechts unten von mir
  blockoffA[  3 ] =     0;       //- unter mir
  blockoffAy[  0 ] =   0;                    //- ich
  blockoffAy[  1 ] = 0;                    //- rechts von mir
  blockoffAy[  2 ] = + 1;       //- rechts unten von mir
  blockoffAy[  3 ] = + 1;       //- unter mir
  
  //- Source-Offsets für A-Blocks
  //- Uhrzeiger
  blockoffAs[ 0 ][ 0 ] =    0 ;  //- unter mir 
  blockoffAs[ 1 ][ 0 ] =   0;               //- ich
  blockoffAs[ 2 ][ 0 ] = + 1;               //- rechts von mir
  blockoffAs[ 3 ][ 0 ] = + 1 ;  //- rechts unten von mir  
  blockoffAsy[ 0 ][ 0 ] =     + 1;  //- unter mir 
  blockoffAsy[ 1 ][ 0 ] =   0;               //- ich
  blockoffAsy[ 2 ][ 0 ] = 0;               //- rechts von mir
  blockoffAsy[ 3 ][ 0 ] = + 1;  //- rechts unten von mir  
  //- gegen Uhrzeiger
  blockoffAs[ 0 ][ 1 ] = + 1;               //- rechts von mir  
  blockoffAs[ 1 ][ 1 ] = + 1 ;  //- rechts unten von mir
  blockoffAs[ 2 ][ 1 ] =     0;  //- unter mir 
  blockoffAs[ 3 ][ 1 ] =   0;               //- ich
  blockoffAsy[ 0 ][ 1 ] = 0;               //- rechts von mir  
  blockoffAsy[ 1 ][ 1 ] = + 1;  //- rechts unten von mir
  blockoffAsy[ 2 ][ 1 ] =     + 1;  //- unter mir 
  blockoffAsy[ 3 ][ 1 ] =   0;               //- ich
  

  //- Destination-Offsets für B-Blocks
  blockoffB[ 0 ] = - 1 ;        //- links über von mir 
  blockoffB[ 1 ] =     0;        //- über mir 
  blockoffB[ 2 ] =   0;                     //- ich
  blockoffB[ 3 ] = - 1;                     //- links von mir
  blockoffBy[ 0 ] = - 1;        //- links über von mir 
  blockoffBy[ 1 ] =     - 1;        //- über mir 
  blockoffBy[ 2 ] =   0;                     //- ich
  blockoffBy[ 3 ] = 0;                     //- links von mir

  //- Source-Offsets für B-Blocks
  //- Uhrzeiger
  blockoffBs[ 0 ][ 0 ] = - 1;               //- links von mir
  blockoffBs[ 1 ][ 0 ] = - 1 ;  //- links über von mir 
  blockoffBs[ 2 ][ 0 ] =     0;  //- über mir 
  blockoffBs[ 3 ][ 0 ] =   0;               //- ich  
  blockoffBsy[ 0 ][ 0 ] = 0;               //- links von mir
  blockoffBsy[ 1 ][ 0 ] = - 1;  //- links über von mir 
  blockoffBsy[ 2 ][ 0 ] =     - 1;  //- über mir 
  blockoffBsy[ 3 ][ 0 ] =   0;               //- ich  
  //- gegen Uhrzeiger
  blockoffBs[ 0 ][ 1 ] =     0;  //- über mir 
  blockoffBs[ 1 ][ 1 ] =   0;               //- ich
  blockoffBs[ 2 ][ 1 ] = - 1;               //- links von mir
  blockoffBs[ 3 ][ 1 ] = - 1 ;  //- links über von mir 
  blockoffBsy[ 0 ][ 1 ] =     - 1;  //- über mir 
  blockoffBsy[ 1 ][ 1 ] =   0;               //- ich
  blockoffBsy[ 2 ][ 1 ] = 0;               //- links von mir
  blockoffBsy[ 3 ][ 1 ] = - 1;  //- links über von mir 

  static math::CRand random;
  int i, j;

  do
  {
    if ( ( cycle % 2 ) == 1 )
    {
      for ( y = 1; y < height - 1; y += 2 )
      {
        for ( x = 1; x < width - 1; x += 2 )
        {
          i = random( 2 ); 
          for ( j = 0; j < 4; ++j )
          {
            m_pCDTarget->PutPixel( x + blockoffA[j], 
                                   y + blockoffAy[j],
                                   m_pCDLastFrame->GetPixel( x + blockoffAs[j][i],
                                                             y + blockoffAsy[j][i] ) );
          }
        }
      }
    }
    else
    {
      for ( y = 1; y < height - 1; y += 2 )
      {
        for ( x = 1; x < width - 1; x += 2 )
        {
          i = random( 2 ); 
          for ( j = 0; j < 4; ++j )
          {
            m_pCDTarget->PutPixel( x + blockoffB[j], 
                                   y + blockoffBy[j],
                                   m_pCDLastFrame->GetPixel( x + blockoffBs[j][i],
                                                             y + blockoffBsy[j][i] ) );
          }
        }
      }
    }
    --cycle;

    NextFrame();

    // gelegentliches Bild-Update
    if ( !m_pDlg->UpdateThread() )
    {
      return;
    }
  }
  while ( cycle >= 0 );

}



void CEffektDiffusor::OnChangeEditIterations() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }

  CString   cstrGnu;
  m_EditIterations.GetWindowText( cstrGnu );
  m_dwIterations = GR::Convert::ToU32( LPCTSTR( cstrGnu ) );

  RestartRendering();
	
}



BOOL CEffektDiffusor::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();
	
  m_DoNotUpdate = TRUE;

  m_EditIterations.SetWindowText( _T( "5" ) );
  m_dwIterations = 5;

  m_DoNotUpdate = FALSE;
	
	return TRUE;
}
