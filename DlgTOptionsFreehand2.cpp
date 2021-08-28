#include "stdafx.h"
#include "painter.h"
#include "DlgTOptionsFreehand2.h"



IMPLEMENT_DYNAMIC(CDlgTOptionsFreehand2, CDialog)
CDlgTOptionsFreehand2::CDlgTOptionsFreehand2(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTOptionsFreehand2::IDD, pParent)
{
}



CDlgTOptionsFreehand2::~CDlgTOptionsFreehand2()
{
}



void CDlgTOptionsFreehand2::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CHECK_AA, m_CheckAA);
}



BEGIN_MESSAGE_MAP(CDlgTOptionsFreehand2, CDialog)
  ON_BN_CLICKED(IDC_CHECK_AA, OnBnClickedCheckAa)
END_MESSAGE_MAP()



void CDlgTOptionsFreehand2::OnBnClickedCheckAa()
{
  if ( m_CheckAA.GetCheck() )
  {
    pSettings->SetSetting( "LineAA", "1" );
  }
  else
  {
    pSettings->SetSetting( "LineAA", "0" );
  }
}



BOOL CDlgTOptionsFreehand2::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_CheckAA.SetCheck( pSettings->GetSetting( "LineAA", 0 ) ? BST_CHECKED : 0 );

  return TRUE;
}

