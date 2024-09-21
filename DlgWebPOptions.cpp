#include "stdafx.h"
#include "painter.h"
#include "afxdialogex.h"
#include "DlgWebPOptions.h"


IMPLEMENT_DYNAMIC(DlgWebPOptions, CDialogEx)



DlgWebPOptions::DlgWebPOptions(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WEBP_OPTIONS, pParent),
	m_SaveOption( WEBP_LOSSLESS )
{

}



DlgWebPOptions::~DlgWebPOptions()
{
}



void DlgWebPOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_RADIO_WEBP_LOSLESS, m_RadioLosless );
	DDX_Control( pDX, IDC_RADIO_WEBP_DEFAULT_75, m_RadioDefault );
}


BEGIN_MESSAGE_MAP(DlgWebPOptions, CDialogEx)
	ON_BN_CLICKED( IDC_RADIO_WEBP_LOSLESS, &DlgWebPOptions::OnBnClickedRadioWebpLosless )
	ON_BN_CLICKED( IDC_RADIO_WEBP_DEFAULT_75, &DlgWebPOptions::OnBnClickedRadioWebpDefault75 )
END_MESSAGE_MAP()



BOOL DlgWebPOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_RadioLosless.SetCheck( 1 );
	return TRUE;
}



void DlgWebPOptions::OnBnClickedRadioWebpLosless()
{
	m_SaveOption = WEBP_LOSSLESS;
}



void DlgWebPOptions::OnBnClickedRadioWebpDefault75()
{
	m_SaveOption = WEBP_DEFAULT;
}
