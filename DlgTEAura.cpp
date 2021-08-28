#include "stdafx.h"
#include "DlgTEAura.h"
#include "TextDialog.h"
#include "ColorPicker.h"

#include <Misc/Misc.h>

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDlgTEAura::CDlgTEAura(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTEAura::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTEAura)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgTEAura::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTEAura)
	DDX_Control(pDX, IDC_BUTTON_TE_FARBE, m_ButtonFarbe);
	DDX_Control(pDX, IDC_EDIT_AURA_OFFSET, m_EditAuraOffset);
	DDX_Control(pDX, IDC_EDIT_AURA_SIZE, m_EditAuraSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTEAura, CDialog)
	//{{AFX_MSG_MAP(CDlgTEAura)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON_TE_FARBE, OnButtonTeFarbe)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CDlgTEAura::OnOK() 
{
  CString   cstrGnu;

  m_EditAuraSize.GetWindowText( cstrGnu );
  m_pEffekt->m_Param1 = GR::Convert::ToU32( LPCTSTR( cstrGnu ) );

  m_EditAuraOffset.GetWindowText( cstrGnu );
  m_pEffekt->m_Param2 = GR::Convert::ToU32( LPCTSTR( cstrGnu ) );
	
	CDialog::OnOK();
}



BOOL CDlgTEAura::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_EditAuraSize.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_pEffekt->m_Param1 ) ).c_str() );
  m_EditAuraOffset.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_pEffekt->m_Param2 ) ).c_str() );
	
	return TRUE;
}



void CDlgTEAura::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if ( lpDrawItemStruct->hwndItem == m_ButtonFarbe.GetSafeHwnd() )
  {
    HBRUSH hbrush = CreateSolidBrush( RGB( ( ( m_pEffekt->m_Color & 0xff0000 ) >> 16 ),
                                           ( ( m_pEffekt->m_Color & 0xff00 ) >> 8 ),
                                           ( ( m_pEffekt->m_Color & 0xff ) ) ) );
    FillRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, hbrush );
    DeleteObject( hbrush );
    return;
  }
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}



void CDlgTEAura::OnButtonTeFarbe() 
{
  CColorPicker    dlgCP;


  dlgCP.m_WorkColor = m_pEffekt->m_Color;
  if ( dlgCP.DoModal() == IDOK )
  {
    m_pEffekt->m_Color = dlgCP.m_WorkColor;
  }
}
