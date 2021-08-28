// DlgSaveIGF.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSaveIGF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgSaveIGF::CDlgSaveIGF(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSaveIGF::IDD, pParent)
{
	m_SaveMitPalette       = false;
  m_SaveMitCompressList  = false;
  m_HidePaletteCheckBox  = false;
	
	//{{AFX_DATA_INIT(CDlgSaveIGF)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSaveIGF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSaveIGF)
	DDX_Control(pDX, IDC_CHECK_IGF_WITHPAL, m_checkSaveMitPalette);
  DDX_Control(pDX, IDC_CHECK_IGF_WITHCOMPRESSLIST, m_checkSaveMitCompList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSaveIGF, CDialog)
	//{{AFX_MSG_MAP(CDlgSaveIGF)
	ON_BN_CLICKED(IDC_CHECK_IGF_WITHPAL, OnCheckIgfWithPal)
  ON_BN_CLICKED(IDC_CHECK_IGF_WITHCOMPRESSLIST, OnCheckIgfWithCompList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CDlgSaveIGF::OnCheckIgfWithPal() 
{
  if ( m_checkSaveMitPalette.GetCheck() )
  {
    m_SaveMitPalette = true;
  }
  else
  {
    m_SaveMitPalette = false;
  }
}



void CDlgSaveIGF::OnCheckIgfWithCompList() 
{
  if ( m_checkSaveMitCompList.GetCheck() )
  {
    m_SaveMitCompressList = true;
  }
  else
  {
    m_SaveMitCompressList = false;
  }
}



BOOL CDlgSaveIGF::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_checkSaveMitCompList.SetCheck( m_SaveMitCompressList ? 1 : 0 );
  m_checkSaveMitPalette.SetCheck( m_SaveMitPalette ? 1 : 0 );

  if ( m_HidePaletteCheckBox )
  {
    m_checkSaveMitPalette.ShowWindow( SW_HIDE );
  }
	
	return TRUE;
}
