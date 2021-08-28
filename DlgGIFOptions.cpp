#include "stdafx.h"
#include "painter.h"
#include "DlgGIFOptions.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(DlgGIFOptions, CDialogEx)

DlgGIFOptions::DlgGIFOptions(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_GIF_OPTIONS, pParent)
{

}

DlgGIFOptions::~DlgGIFOptions()
{
}

void DlgGIFOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_RADIO_GIF_87, m_RadioGIF87 );
	DDX_Control( pDX, IDC_RADIO_GIF_89, m_RadioGIF89 );
}


BEGIN_MESSAGE_MAP(DlgGIFOptions, CDialog)
	ON_BN_CLICKED( IDOK, &DlgGIFOptions::OnBnClickedOk )
END_MESSAGE_MAP()


// DlgGIFOptions message handlers



BOOL DlgGIFOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_RadioGIF89.SetCheck( m_SaveASGIF89 ? BST_CHECKED : 0 );
	m_RadioGIF87.SetCheck( !m_SaveASGIF89 ? BST_CHECKED : 0 );

	return TRUE;
}



void DlgGIFOptions::OnBnClickedOk()
{
	m_SaveASGIF89 = ( m_RadioGIF89.GetCheck() == BST_CHECKED );

	CDialog::OnOK();
}
