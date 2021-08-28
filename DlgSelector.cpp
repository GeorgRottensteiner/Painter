#include "stdafx.h"
#include "painter.h"
#include "DlgSelector.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgSelector::CDlgSelector(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelector::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelector)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelector)
	DDX_Control(pDX, IDC_COMBO_RULE, m_ComboRule);
	DDX_Control(pDX, IDC_COMBO_ANDORXOR, m_ComboAndOrXOR);
	DDX_Control(pDX, IDC_LIST_SELECTION_RULES, m_ListRules);
  DDX_Control(pDX, IDC_REDIT_VALUE,  m_REditValue );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelector, CDialog)
	//{{AFX_MSG_MAP(CDlgSelector)
		ON_EN_CHANGE(IDC_REDIT_VALUE, OnChangeREditValue)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CDlgSelector::OnChangeREditValue() 
{
  CString   cstrGnu;

  m_REditValue.GetWindowText( cstrGnu );
  m_Param = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );
}



BOOL CDlgSelector::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_REditValue.SetRange( 0, 255 );
	
  m_ComboAndOrXOR.SetItemData( m_ComboAndOrXOR.AddString( _T( "AND" ) ), M_AND );
  m_ComboAndOrXOR.SetItemData( m_ComboAndOrXOR.AddString( _T( "OR" ) ), M_OR );
  m_ComboAndOrXOR.SetItemData( m_ComboAndOrXOR.AddString( _T( "XOR" ) ), M_XOR );
  m_ComboAndOrXOR.SetItemData( m_ComboAndOrXOR.AddString( _T( "NOT" ) ), M_NOT );

  m_ComboAndOrXOR.SetCurSel( 0 );

  m_ComboRule.SetItemData( m_ComboRule.AddString( _T( "R >= than..." ) ), R_R_BIGGER );
  m_ComboRule.SetItemData( m_ComboRule.AddString( _T( "R <= than..." ) ), R_R_LESSER );
  m_ComboRule.SetItemData( m_ComboRule.AddString( _T( "G >= than..." ) ), R_G_BIGGER );
  m_ComboRule.SetItemData( m_ComboRule.AddString( _T( "G <= than..." ) ), R_G_LESSER );
  m_ComboRule.SetItemData( m_ComboRule.AddString( _T( "B >= than..." ) ), R_B_BIGGER );
  m_ComboRule.SetItemData( m_ComboRule.AddString( _T( "B <= than..." ) ), R_B_LESSER );
  m_ComboRule.SetItemData( m_ComboRule.AddString( _T( "== Color 1" ) ), R_COLOR_1 );
  m_ComboRule.SetItemData( m_ComboRule.AddString( _T( "== Color 2" ) ), R_COLOR_2 );

  m_ComboRule.SetCurSel( 0 );

  m_Rules.clear();

  m_ListRules.InsertColumn( 0, _T( "Mode" ), LVCFMT_LEFT, 40 );
  m_ListRules.InsertColumn( 1, _T( "Rule" ), LVCFMT_LEFT, 130 );
  m_ListRules.InsertColumn( 2, _T( "Parameter" ), LVCFMT_LEFT, 100 );

	return TRUE;

}

void CDlgSelector::OnButtonAdd() 
{
  tRule   Rule;

  Rule.m_Mode = (eMode)m_ComboAndOrXOR.GetItemData( m_ComboAndOrXOR.GetCurSel() );
  Rule.m_Rule = (eRule)m_ComboRule.GetItemData( m_ComboRule.GetCurSel() );
  Rule.m_Param = m_Param;

  m_Rules.push_back( Rule );

  CString   cstrGnu;
  CString   cstrMode;

  m_ComboAndOrXOR.GetLBText( m_ComboAndOrXOR.GetCurSel(), cstrMode );
  m_ComboRule.GetLBText( m_ComboRule.GetCurSel(), cstrGnu );

  GR::tChar   szTemp[MAX_PATH];
  wsprintf( szTemp, _T( "%d" ), m_Param );
  int iItem = m_ListRules.InsertItem( m_ListRules.GetItemCount(), cstrMode );
  m_ListRules.SetItemData( iItem, (DWORD_PTR)&m_Rules.back() );
  m_ListRules.SetItemText( iItem, 1, cstrGnu );
  m_ListRules.SetItemText( iItem, 2, szTemp );
	
}



void CDlgSelector::OnOK() 
{
  if ( pSettings->m_pActiveViewInfo == NULL )
  {
    CDialog::OnOK();
    return;
  }

  pSettings->m_pActiveDocInfo->EmptySelection();

  GR::Graphic::ContextDescriptor*   pCD = pSettings->m_pActiveViewInfo->GetWorkCD();

  GR::Graphic::ContextDescriptor    cdSelection( pSettings->m_pActiveDocInfo->m_pImageSelection );

  GR::u32   col1 = pSettings->GetRGBColor( CSettings::CO_WORKCOLOR );
  GR::u32   col2 = pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 );

  for ( int i = 0; i < pCD->Width(); ++i )
  {
    for ( int j = 0; j < pCD->Height(); ++j )
    {
      GR::u32 color = pCD->GetPixel( i, j );

      bool    keepPixel = true;

      std::list<tRule>::iterator    it( m_Rules.begin() );
      while ( it != m_Rules.end() )
      {
        tRule&    Rule = *it;

        bool      ruleApplies = false;

        switch ( Rule.m_Rule )
        {
          case R_R_BIGGER:
            if ( ( ( color & 0xff0000 ) >> 16 ) >= (GR::u32)Rule.m_Param )
            {
              ruleApplies = true;
            }
            break;
          case R_R_LESSER:
            if ( ( ( color & 0xff0000 ) >> 16 ) <= ( GR::u32 )Rule.m_Param )
            {
              ruleApplies = true;
            }
            break;
          case R_G_BIGGER:
            if ( ( ( color & 0x00ff00 ) >> 8 ) >= ( GR::u32 )Rule.m_Param )
            {
              ruleApplies = true;
            }
            break;
          case R_G_LESSER:
            if ( ( ( color & 0x00ff00 ) >> 8 ) <= ( GR::u32 )Rule.m_Param )
            {
              ruleApplies = true;
            }
            break;
          case R_B_BIGGER:
            if ( ( color & 0x0000ff ) >= ( GR::u32 )Rule.m_Param )
            {
              ruleApplies = true;
            }
            break;
          case R_B_LESSER:
            if ( ( color & 0x0000ff ) <= ( GR::u32 )Rule.m_Param )
            {
              ruleApplies = true;
            }
            break;
          case R_COLOR_1:
            if ( color == col1 )
            {
              ruleApplies = true;
            }
            break;
          case R_COLOR_2:
            if ( color == col2 )
            {
              ruleApplies = true;
            }
            break;
        }

        if ( Rule.m_Mode == M_NOT )
        {
          ruleApplies = !ruleApplies;
        }


        if ( Rule.m_Mode == M_AND )
        {
          if ( !ruleApplies )
          {
            keepPixel = false;
            break;
          }
        }
        if ( Rule.m_Mode == M_OR )
        {
          if ( ( !ruleApplies )
          &&   ( m_Rules.size() == 1 ) )
          {
            keepPixel = false;
            break;
          }
        }
        ++it;
      }

      if ( keepPixel )
      {
        cdSelection.PutDirectPixel( i, j, 255 );
      }
    }
  }

  delete pCD;

  pSettings->m_pActiveDocInfo->UpdateMarchingAnts();
	
	CDialog::OnOK();
}
