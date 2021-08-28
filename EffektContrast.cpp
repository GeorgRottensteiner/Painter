#include "stdafx.h"
#include "DlgEffekte.h"
#include "EffektContrast.h"
#include "PainterGFXTools.h"
#include "ActionContrast.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektContrast::CEffektContrast(CWnd* pParent /*=NULL*/)
: EffektTemplate(CEffektContrast::IDD,pParent),
  m_iLowerBound( 0 ),
  m_iUpperBound( 100 )
{
	//{{AFX_DATA_INIT(CEffektContrast)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEffektContrast::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektContrast)
  DDX_Control(pDX, IDC_REDIT_LOWER, m_REditLowerBound);
  DDX_Control(pDX, IDC_REDIT_UPPERBOUND, m_REditUpperBound);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektContrast, CDialog)
	//{{AFX_MSG_MAP(CEffektContrast)
  ON_EN_CHANGE(IDC_REDIT_LOWER, OnChangeREditLowerBound)
  ON_EN_CHANGE(IDC_REDIT_UPPERBOUND, OnChangeREditUpperBound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CEffektContrast::OnChangeREditLowerBound() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString cstrgnu;
  m_REditLowerBound.GetWindowText( cstrgnu );
  m_iLowerBound = GR::Convert::ToI32( LPCTSTR( cstrgnu ) );

  RestartRendering();

}



void CEffektContrast::OnChangeREditUpperBound() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString cstrgnu;
  m_REditUpperBound.GetWindowText( cstrgnu );
  m_iUpperBound = GR::Convert::ToI32( LPCTSTR( cstrgnu ) );

  RestartRendering();

}



void CEffektContrast::DoEffekt()
{
  ActionContrast Contrast;

  Contrast.m_LowerBound = m_iLowerBound;
  Contrast.m_UpperBound = m_iUpperBound;

  GR::Graphic::ContextDescriptor* pCDDummy = Contrast.PerformAction( m_pCDSource );

  if ( pCDDummy )
  {
    pCDDummy->CopyArea( 0, 0, pCDDummy->Width(), pCDDummy->Height(), 0, 0, m_pCDTarget );
    delete pCDDummy;
  }
}



BOOL CEffektContrast::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_DoNotUpdate = TRUE;

  m_REditLowerBound.SetRange( 0, 400 );
  m_REditLowerBound.SetPosition( 0 );

  m_REditUpperBound.SetRange( 0, 400 );
  m_REditUpperBound.SetPosition( 100 );

  m_DoNotUpdate = FALSE;
	
	return TRUE;
}
