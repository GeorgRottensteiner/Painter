#include "stdafx.h"
#include "DlgResizeCanvas.h"
#include "DocumentInfo.h"
#include "Settings.h"
#include <Misc/Misc.h>

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgResizeCanvas::CDlgResizeCanvas(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgResizeCanvas::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgResizeCanvas)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgResizeCanvas::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgResizeCanvas)
	DDX_Control(pDX, IDC_EDIT_NEUE_HOEHE, m_EditNewHeight);
	DDX_Control(pDX, IDC_EDIT_NEUE_BREITE, m_EditNewWidth);
	DDX_Control(pDX, IDC_EDIT_HOEHE, m_EditHoehe);
	DDX_Control(pDX, IDC_EDIT_BREITE, m_EditBreite);
	DDX_Control(pDX, IDC_CHECK_CENTER_V, m_CheckCenterV);
	DDX_Control(pDX, IDC_CHECK_CENTER_H, m_CheckCenterH);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgResizeCanvas, CDialog)
	//{{AFX_MSG_MAP(CDlgResizeCanvas)
	//}}AFX_MSG_MAP
  ON_EN_CHANGE(IDC_EDIT_NEUE_BREITE, OnEnChangeEditNeueBreite)
END_MESSAGE_MAP()



BOOL CDlgResizeCanvas::OnInitDialog() 
{
	CDialog::OnInitDialog();


  m_EditBreite.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_pDocInfo->Width() ) ).c_str() );
  m_EditHoehe.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_pDocInfo->Height() ) ).c_str() );
	
  m_EditNewWidth.SetFocus();

  m_EditNewWidth.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", pSettings->GetSetting( "NewCanvasWidth", m_pDocInfo->Width() ) ) ).c_str() );
  m_EditNewHeight.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", pSettings->GetSetting( "NewCanvasHeight", m_pDocInfo->Height() ) ) ).c_str() );

  m_CheckCenterH.SetCheck( pSettings->GetSettingString( "NewCanvasCenterH" ) != "No" );
  m_CheckCenterV.SetCheck( pSettings->GetSettingString( "NewCanvasCenterV" ) != "No" );
	
	return FALSE;
}



void CDlgResizeCanvas::OnOK() 
{
  GR::tChar    szTemp[MAX_PATH];
  m_EditNewWidth.GetWindowText( szTemp, MAX_PATH );
  m_NewWidth = GR::Convert::ToI32( szTemp );

  m_EditNewHeight.GetWindowText( szTemp, MAX_PATH );
  m_NewHeight = GR::Convert::ToI32( szTemp );

  pSettings->SetSetting( "NewCanvasWidth", m_NewWidth );
  pSettings->SetSetting( "NewCanvasHeight", m_NewHeight );

  m_CenterH = m_CheckCenterH.GetCheck() ? true : false;
  m_CenterV = m_CheckCenterV.GetCheck() ? true : false;

  pSettings->SetSetting( "NewCanvasCenterH", m_CenterH ? "Yes" : "No" );
  pSettings->SetSetting( "NewCanvasCenterV", m_CenterV ? "Yes" : "No" );

	CDialog::OnOK();
}



void CDlgResizeCanvas::OnEnChangeEditNeueBreite()
{
}
