#include "stdafx.h"
#include "painter.h"
#include "DlgRenameLayer.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgRenameLayer::CDlgRenameLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRenameLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRenameLayer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgRenameLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRenameLayer)
	DDX_Control(pDX, IDC_EDIT_LAYER_NAME, m_EditLayerName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRenameLayer, CDialog)
	//{{AFX_MSG_MAP(CDlgRenameLayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CDlgRenameLayer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_EditLayerName.SetWindowText( GR::Convert::ToUTF16( m_LayerName ).c_str() );
  m_EditLayerName.SetSel( 0, -1 );
  m_EditLayerName.SetFocus();
	
	return FALSE;
}



void CDlgRenameLayer::OnOK() 
{
  CString   cstrGnu;

  m_EditLayerName.GetWindowText( cstrGnu );
  m_LayerName = GR::Convert::ToUTF8( LPCTSTR( cstrGnu ) );
	
	CDialog::OnOK();
}
