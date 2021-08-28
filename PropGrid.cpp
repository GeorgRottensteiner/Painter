#include "stdafx.h"
#include "painter.h"
#include "PropGrid.h"

#include <String/Convert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPropGrid, CPropertyPage)

CPropGrid::CPropGrid() : CPropertyPage(CPropGrid::IDD)
{
	//{{AFX_DATA_INIT(CPropGrid)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

CPropGrid::~CPropGrid()
{
}

void CPropGrid::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropGrid)
	DDX_Control(pDX, IDC_CHECK_GRID, m_CheckSnap);
	DDX_Control(pDX, IDC_EDIT_GRID_YOFFSET, m_EditGridYOffset);
	DDX_Control(pDX, IDC_EDIT_GRID_XOFFSET, m_EditGridXOffset);
	DDX_Control(pDX, IDC_EDIT_GRID_WIDTH, m_EditGridWidth);
	DDX_Control(pDX, IDC_EDIT_GRID_HEIGHT, m_EditGridHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropGrid, CPropertyPage)
	//{{AFX_MSG_MAP(CPropGrid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CPropGrid::OnInitDialog() 
{

	CPropertyPage::OnInitDialog();

  m_EditGridXOffset.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_iGridXOffset ) ).c_str() );
  m_EditGridYOffset.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_iGridYOffset ) ).c_str() );
  m_EditGridWidth.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_iGridWidth ) ).c_str() );
  m_EditGridHeight.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_iGridHeight ) ).c_str() );
  m_CheckSnap.SetCheck( m_bEnableSnap );
	
	return TRUE;

}

void CPropGrid::OnOK() 
{

  GR::tChar    szTemp[MAX_PATH];
  m_EditGridWidth.GetWindowText( szTemp, MAX_PATH );
  m_iGridWidth = GR::Convert::ToI32( szTemp );
  if ( m_iGridWidth <= 0 )
  {
    m_iGridWidth = 20;
  }

  m_EditGridHeight.GetWindowText( szTemp, MAX_PATH );
  m_iGridHeight = GR::Convert::ToI32( szTemp );
  if ( m_iGridHeight <= 0 )
  {
    m_iGridHeight = 20;
  }

  m_EditGridXOffset.GetWindowText( szTemp, MAX_PATH );
  m_iGridXOffset = GR::Convert::ToI32( szTemp );

  m_EditGridYOffset.GetWindowText( szTemp, MAX_PATH );
  m_iGridYOffset = GR::Convert::ToI32( szTemp );

  m_bEnableSnap = m_CheckSnap.GetCheck();
	
	
	CPropertyPage::OnOK();
}
