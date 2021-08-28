#include "stdafx.h"
#include "painter.h"
#include "MainFrm.h"
#include "Progress.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CProgress::CProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgress)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgress)
	DDX_Control(pDX, IDC_STATIC_PROGRESS_PROZENT, m_Prozent);
	DDX_Control(pDX, IDC_STATIC_PROGRESS_INFO, m_Info);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_ProgressBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgress, CDialog)
	//{{AFX_MSG_MAP(CProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



extern CPainterApp theApp;



CProgress::CProgress( GR::Char* Text, int iPos, int iMax )
{
  Create( MAKEINTRESOURCE( IDD_DIALOG_PROGRESS ), theApp.pMainFrame );
  SetText( Text );
  SetPosition( iPos, iMax );
}



void CProgress::SetPosition( int iPosition, int iFull )
{
  int     iProzent;


  if ( iFull == 0 )
  {
    iProzent = 100;
  }
  else
  {
    iProzent = ( iPosition * 100 ) / iFull;
  }
  if ( iProzent < 0 )
  {
    iProzent = 0;
  }
  if ( iProzent > 100 )
  {
    iProzent = 100;
  }
  m_Prozent.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%02d%%", iProzent ) ).c_str() );
  m_ProgressBar.SetPos( iProzent );
}



void CProgress::SetText( GR::Char *szText )
{
  m_Info.SetWindowText( GR::Convert::ToUTF16( szText ).c_str() );
}



BOOL CProgress::OnInitDialog() 
{

	CDialog::OnInitDialog();


  CenterWindow();
	
	return TRUE;

}
