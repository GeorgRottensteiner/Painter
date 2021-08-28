// DlgDebug.cpp : implementation file
//

#include "stdafx.h"
#include "DlgDebug.h"

#include "Settings.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDebug dialog


CDlgDebug::CDlgDebug(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDebug::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDebug)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgDebug::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDebug)
	DDX_Control(pDX, IDC_LIST_DEBUG, m_ListDebug);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDebug, CDialog)
	//{{AFX_MSG_MAP(CDlgDebug)
	ON_BN_CLICKED(IDC_BUTTON_EMPTY, OnButtonEmpty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDebug message handlers


void CDlgDebug::Log( const char *szText )
{

  m_ListDebug.SetCurSel( m_ListDebug.AddString( szText ) );

}

BOOL CDlgDebug::OnInitDialog() 
{
	CDialog::OnInitDialog();

  SetWindowPos( &wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	
	return TRUE;

}

void CDlgDebug::OnButtonEmpty() 
{

  pSettings->Log( "------------" );
	
}
