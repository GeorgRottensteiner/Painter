#include "stdafx.h"
#include "DlgFreeRotate.h"
#include "Settings.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgFreeRotate::CDlgFreeRotate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFreeRotate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFreeRotate)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}



void CDlgFreeRotate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFreeRotate)
	DDX_Control(pDX, IDC_CHECK_ALLOW_RESIZE, m_CheckAllowResize);
	DDX_Control(pDX, IDC_EDIT_WINKEL, m_EditWinkel);
  DDX_Control(pDX, IDC_RADIO_CW, m_RadioRotateCW);
  DDX_Control(pDX, IDC_RADIO_CCW, m_RadioRotateCCW);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CDlgFreeRotate, CDialog)
	//{{AFX_MSG_MAP(CDlgFreeRotate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CDlgFreeRotate::OnOK() 
{
  CString   cstrGnu;

  m_EditWinkel.GetWindowText( cstrGnu );
  m_Angle = GR::Convert::ToF32( LPCTSTR( cstrGnu ) );

  m_AllowResize = !!m_CheckAllowResize.GetCheck();
  m_RotateCW    = !!m_RadioRotateCW.GetCheck();
	
	CDialog::OnOK();
}



BOOL CDlgFreeRotate::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_EditWinkel.SetWindowText( GR::Convert::ToUTF16( pSettings->GetSettingString( "FreeRotateAngle", "45" ) ).c_str() );
  m_CheckAllowResize.SetCheck( 1 );
  m_RadioRotateCW.SetCheck( 1 );
	
	return TRUE;
}
