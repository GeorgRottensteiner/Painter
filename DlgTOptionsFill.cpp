#include "stdafx.h"
#include "Settings.h"
#include "DlgTOptionsFill.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgTOptionsFill::CDlgTOptionsFill(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTOptionsFill::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTOptionsFill)
	//}}AFX_DATA_INIT
}


void CDlgTOptionsFill::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDlgTOptionsFill)
  DDX_Control(pDX, IDC_COMBO_FILLMODE, m_ComboFillMode);
  DDX_Control(pDX, IDC_REDIT_TOLERANZ, m_REToleranz);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_CHECK_GRADIENT_CLIP, m_CheckClipGradient);
}


BEGIN_MESSAGE_MAP(CDlgTOptionsFill, CDialog)
	//{{AFX_MSG_MAP(CDlgTOptionsFill)
	ON_CBN_SELCHANGE(IDC_COMBO_FILLMODE, OnSelchangeComboFillMode)
  ON_EN_CHANGE(IDC_REDIT_TOLERANZ, OnChangeREditToleranz)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_CHECK_GRADIENT_CLIP, OnBnClickedCheckGradientClip)
END_MESSAGE_MAP()



BOOL CDlgTOptionsFill::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_ComboFillMode.SetItemData( m_ComboFillMode.AddString( _T( "FloodFill" ) ), CSettings::FILL_FLOOD );
  m_ComboFillMode.SetItemData( m_ComboFillMode.AddString( _T( "RGB-Fill" ) ), CSettings::FILL_RGB );

  m_ComboFillMode.SetCurSel( pSettings->GetSetting( "FillMode" ) );

  m_REToleranz.SetRange( 0, 100 );
  m_REToleranz.SetPosition( pSettings->GetSetting( "FillToleranz", 0 ) );

  m_CheckClipGradient.SetCheck( pSettings->GetSetting( "FillClipGradient", 0 ) ? BST_CHECKED : 0 );
	
	return TRUE;
}



void CDlgTOptionsFill::OnSelchangeComboFillMode() 
{
	pSettings->SetSetting( "FillMode", m_ComboFillMode.GetCurSel() );
}



void CDlgTOptionsFill::OnChangeREditToleranz() 
{
	CString   cstrGnu;


  m_REToleranz.GetWindowText( cstrGnu );

  int   value = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );
  value = math::clamp( value, 0, 100 );
  pSettings->SetSetting( "FillToleranz", value );
  pSettings->SetSetting( "FillToleranzLog", (int)( 100 - 200 * ( 1 - 1 / ( ( 100 - pSettings->GetSetting( "FillToleranz", 0 ) ) / 100.0f + 1 ) ) ) );
}



void CDlgTOptionsFill::OnBnClickedCheckGradientClip()
{
  pSettings->SetSetting( "FillClipGradient", m_CheckClipGradient.GetCheck() ? 1 : 0 );
}
