// ConvertOptions.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ConvertOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CConvertOptions 


CConvertOptions::CConvertOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConvertOptions)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CConvertOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConvertOptions)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConvertOptions, CDialog)
	//{{AFX_MSG_MAP(CConvertOptions)
	ON_BN_CLICKED(IDC_RADIO_CONVERT_STRAIGHT, OnRadioConvertStraight)
	ON_BN_CLICKED(IDC_RADIO_CONVERT_DITHER, OnRadioConvertDither)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/*-OnRadioConvertStraight-----------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CConvertOptions::OnRadioConvertStraight() 
{

	m_dwConvertMode = CONVERT::TYPE::STRAIGHT;
	
}



/*-OnRadioConvertDither-------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CConvertOptions::OnRadioConvertDither() 
{

  m_dwConvertMode = CONVERT::TYPE::DITHER;
	
}



/*-OnInitDialog---------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

BOOL CConvertOptions::OnInitDialog() 
{

	CDialog::OnInitDialog();

  m_dwConvertMode = CONVERT::TYPE::STRAIGHT;
  ( (CButton*)GetDlgItem( IDC_RADIO_CONVERT_STRAIGHT ) )->SetCheck( 1 );

	
	return TRUE;

}
