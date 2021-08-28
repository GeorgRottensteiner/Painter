#include "stdafx.h"
#include "painter.h"
#include "DlgTOptionsSelection.h"



IMPLEMENT_DYNAMIC(CDlgTOptionsSelection, CDialog)
CDlgTOptionsSelection::CDlgTOptionsSelection(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTOptionsSelection::IDD, pParent)
{
}



CDlgTOptionsSelection::~CDlgTOptionsSelection()
{
}



void CDlgTOptionsSelection::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CHECK_SQUARE, m_CheckSquare);
}



BEGIN_MESSAGE_MAP(CDlgTOptionsSelection, CDialog)
  ON_BN_CLICKED(IDC_CHECK_SQUARE, OnBnClickedCheckSquare)
END_MESSAGE_MAP()



BOOL CDlgTOptionsSelection::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_CheckSquare.SetCheck( pSettings->GetSetting( "SelectionSquare", 0 ) ? BST_CHECKED : 0 );

  return TRUE;
}



void CDlgTOptionsSelection::OnBnClickedCheckSquare()
{
  pSettings->SetSetting( "SelectionSquare", m_CheckSquare.GetCheck() ? 1 : 0 );
}
