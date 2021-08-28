#include "stdafx.h"
#include "DlgSelectEffekt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgSelectEffekt::CDlgSelectEffekt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectEffekt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectEffekt)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSelectEffekt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectEffekt)
	DDX_Control(pDX, IDC_LIST_EFFEKTE, m_ListEffekte);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectEffekt, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectEffekt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CDlgSelectEffekt::OnOK() 
{
  int item = m_ListEffekte.GetCurSel();
  if ( item != LB_ERR )
  {
    m_Effekt = m_ListEffekte.GetItemData( item );
  }
	
	CDialog::OnOK();
}



BOOL CDlgSelectEffekt::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_ListEffekte.SetItemData( m_ListEffekte.AddString( _T( "Aura" ) ), 1 );
	
	return TRUE;
}
