#include "stdafx.h"
#include "painter.h"
#include "DlgRename.h"

#include <String/Convert.h>



IMPLEMENT_DYNAMIC(CDlgRename, CDialog)
CDlgRename::CDlgRename( const GR::String& Name, const GR::String& Caption, CWnd* pParent /*=NULL*/ )
	: CDialog(CDlgRename::IDD, pParent),
    m_Name( Name ),
    m_Caption( Caption )
{
}



CDlgRename::~CDlgRename()
{
}



void CDlgRename::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_STATIC_OLD, m_StaticOldDesc);
  DDX_Control(pDX, IDC_EDIT_OLD_NAME, m_EditOldName);
  DDX_Control(pDX, IDC_STATIC_NEW, m_StaticNewDesc);
  DDX_Control(pDX, IDC_EDIT_NEW_NAME, m_EditNewName);
}


BEGIN_MESSAGE_MAP(CDlgRename, CDialog)
ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()



void CDlgRename::OnBnClickedOk()
{
  CString   cstrGnu;

  m_EditNewName.GetWindowText( cstrGnu );

  m_Name = GR::Convert::ToUTF8( (LPCTSTR)cstrGnu );

  OnOK();
}



BOOL CDlgRename::OnInitDialog()
{
  CDialog::OnInitDialog();

  SetWindowText( GR::Convert::ToUTF16( m_Caption ).c_str() );

  m_EditOldName.SetWindowText( GR::Convert::ToUTF16( m_Name ).c_str() );
  m_EditNewName.SetWindowText( GR::Convert::ToUTF16( m_Name ).c_str() );

  m_EditNewName.SetFocus();
  m_EditNewName.SetSel( 0, -1 );

  return FALSE;
}
