#include "stdafx.h"
#include "painter.h"
#include "PropDocInfo2.h"

#include <String/Convert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CPropDocInfo2 

IMPLEMENT_DYNCREATE(CPropDocInfo2, CPropertyPage)

CPropDocInfo2::CPropDocInfo2() : CPropertyPage(CPropDocInfo2::IDD)
{
	//{{AFX_DATA_INIT(CPropDocInfo2)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

CPropDocInfo2::~CPropDocInfo2()
{
}

void CPropDocInfo2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropDocInfo2)
	DDX_Control(pDX, IDC_PROP_DOC_SCROLLHEIGHT, m_StaticScrollHeight);
	DDX_Control(pDX, IDC_PROP_DOC_SCROLLWIDTH, m_StaticScrollWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropDocInfo2, CPropertyPage)
	//{{AFX_MSG_MAP(CPropDocInfo2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPropDocInfo2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	

  m_StaticScrollWidth.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_dwScrollWidth ) ).c_str() );
  m_StaticScrollHeight.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_dwScrollHeight ) ).c_str() );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
