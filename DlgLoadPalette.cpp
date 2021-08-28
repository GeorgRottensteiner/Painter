#include "stdafx.h"
#include "painter.h"
#include "DlgLoadPalette.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgLoadPalette::CDlgLoadPalette(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLoadPalette::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLoadPalette)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgLoadPalette::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLoadPalette)
	DDX_Control(pDX, IDC_RADIO_MAP_TO_PAL, m_RadioMapToPal );
	DDX_Control(pDX, IDC_RADIO_FORCE_PAL,  m_RadioForcePal );
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CDlgLoadPalette, CDialog)
	//{{AFX_MSG_MAP(CDlgLoadPalette)
	ON_BN_CLICKED(IDC_RADIO_MAP_TO_PAL, OnRadioMapToPal)
	ON_BN_CLICKED(IDC_RADIO_FORCE_PAL, OnRadioForcePal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CDlgLoadPalette::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_Result = DLP_MAP_TO_PAL;
  m_RadioMapToPal.SetCheck( 1 );
  if ( m_pDocInfo->m_BitDepth != 8 )
  {
    m_RadioForcePal.EnableWindow( FALSE );
  }
	
	return TRUE;
}



void CDlgLoadPalette::OnRadioMapToPal() 
{
	m_Result = DLP_MAP_TO_PAL;
}



void CDlgLoadPalette::OnRadioForcePal() 
{
	m_Result = DLP_FORCE_SET;
}
