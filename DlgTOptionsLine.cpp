#include "stdafx.h"
#include "painter.h"
#include "DlgTOptionsLine.h"



IMPLEMENT_DYNAMIC(CDlgTOptionsLine, CDialog)
CDlgTOptionsLine::CDlgTOptionsLine(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTOptionsLine::IDD, pParent)
{
}



CDlgTOptionsLine::~CDlgTOptionsLine()
{
}



void CDlgTOptionsLine::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CHECK_AA, m_CheckAA);
  DDX_Control(pDX, IDC_CHECK_CONNECTED, m_CheckConnectedLines);
}


BEGIN_MESSAGE_MAP(CDlgTOptionsLine, CDialog)
  ON_BN_CLICKED(IDC_CHECK_AA, OnBnClickedCheckAa)
  ON_BN_CLICKED(IDC_CHECK_CONNECTED, OnBnClickedCheckConnected)
END_MESSAGE_MAP()



void CDlgTOptionsLine::OnBnClickedCheckAa()
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



BOOL CDlgTOptionsLine::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_CheckAA.SetCheck( pSettings->GetSetting( "LineAA", 0 ) ? BST_CHECKED : 0 );
  m_CheckConnectedLines.SetCheck( pSettings->GetSetting( "LineConnect", 0 ) ? BST_CHECKED : 0 );

  return TRUE;
}



void CDlgTOptionsLine::OnBnClickedCheckConnected()
{
  if ( m_CheckConnectedLines.GetCheck() )
  {
    pSettings->SetSetting( "LineConnect", "1" );
  }
  else
  {
    pSettings->SetSetting( "LineConnect", "0" );
  }
}
