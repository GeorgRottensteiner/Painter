#include "stdafx.h"
#include "DlgEffekte.h"
#include "EffektBrightness.h"
#include "PainterGFXTools.h"
#include "ActionBrightness.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektBrightness::CEffektBrightness(CWnd* pParent /*=NULL*/)
: EffektTemplate(CEffektBrightness::IDD,pParent),
    m_Value( 100 )
{
	//{{AFX_DATA_INIT(CEffektBrightness)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEffektBrightness::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektBrightness)
	DDX_Control(pDX, IDC_REDIT_BRIGHTNESS, m_REditBrightness);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektBrightness, CDialog)
	//{{AFX_MSG_MAP(CEffektBrightness)
	ON_EN_CHANGE(IDC_REDIT_BRIGHTNESS, OnChangeREditBrightness)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CEffektBrightness::OnChangeREditBrightness() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }
  CString cstrgnu;
  m_REditBrightness.GetWindowText( cstrgnu );
  m_Value = GR::Convert::ToI32( LPCTSTR( cstrgnu ) );

  RestartRendering();
}



void CEffektBrightness::DoEffekt()
{
  ActionBrightness   Brightness;

  Brightness.m_BrightnessFaktor = m_Value;
  GR::Graphic::ContextDescriptor* pCDDummy = Brightness.PerformAction( m_pCDSource );

  if ( pCDDummy )
  {
    pCDDummy->CopyArea( 0, 0, pCDDummy->Width(), pCDDummy->Height(), 0, 0, m_pCDTarget );
    delete pCDDummy;
  }
}



BOOL CEffektBrightness::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_DoNotUpdate = TRUE;
  m_REditBrightness.SetRange( 0, 400 );
  m_REditBrightness.SetPosition( 100 );

  m_DoNotUpdate = FALSE;
	
	return TRUE;
}
