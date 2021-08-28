#include "stdafx.h"
#include "Settings.h"
#include "PropDocInfo.h"

#include <Misc/Misc.h>
#include <String/Convert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE(CPropDocInfo, CPropertyPage)

CPropDocInfo::CPropDocInfo() : CPropertyPage(CPropDocInfo::IDD)
{
	//{{AFX_DATA_INIT(CPropDocInfo)
	//}}AFX_DATA_INIT

  m_pDocInfo = NULL;

}

CPropDocInfo::~CPropDocInfo()
{
}

void CPropDocInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropDocInfo)
	DDX_Control(pDX, IDC_PROP_DOC_USED_COLORS, m_StaticUsedColors);
	DDX_Control(pDX, IDC_PROP_DOC_WIDTH, m_StaticDocSize);
	DDX_Control(pDX, IDC_PROP_DOC_TYPE, m_StaticType);
	DDX_Control(pDX, IDC_PROP_DOC_PATH, m_StaticPath);
	DDX_Control(pDX, IDC_PROP_DOC_NAME, m_StaticName);
	DDX_Control(pDX, IDC_PROP_DOC_DEPTH, m_StaticDepth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropDocInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CPropDocInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPropDocInfo::OnInitDialog() 
{

	CPropertyPage::OnInitDialog();
	

	if ( m_pDocInfo == NULL )
  {
    return FALSE;
  }

  DWORD   dwColors = m_pDocInfo->CountUsedColors();

  m_StaticName.SetWindowText( GR::Convert::ToUTF16( m_pDocInfo->m_FileName ).c_str() );
  m_StaticDocSize.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%dx%d px", m_pDocInfo->Width(), m_pDocInfo->Height() ) ).c_str() );
  m_StaticUsedColors.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d unique colors", dwColors ) ).c_str() );
  m_StaticDepth.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d bpp", m_pDocInfo->m_BitDepth ) ).c_str() );
  m_StaticPath.SetWindowText( GR::Convert::ToUTF16( m_pDocInfo->m_FileName ).c_str() );

  switch ( m_pDocInfo->m_DocType )
  {
    case DT_INVALID:
      m_StaticType.SetWindowText( _T( "unbekannt" ) );
      break;
    case DT_IMAGE:
      m_StaticType.SetWindowText( _T( "Grafik" ) );
      break;
    case DT_FONT:
      m_StaticType.SetWindowText( _T( "Font" ) );
      break;
  }
	
	return TRUE;

}
