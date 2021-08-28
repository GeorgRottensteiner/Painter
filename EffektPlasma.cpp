#include "stdafx.h"
#include "DlgEffekte.h"
#include "EffektPlasma.h"
#include "ActionPlasma.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektPlasma::CEffektPlasma(CWnd* pParent /*=NULL*/)
	: EffektTemplate(CEffektPlasma::IDD,pParent)
{
	//{{AFX_DATA_INIT(CEffektPlasma)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}



void CEffektPlasma::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektPlasma)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CEffektPlasma, CDialog)
	//{{AFX_MSG_MAP(CEffektPlasma)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CEffektPlasma::DoEffekt()
{
  ActionPlasma    Plasma;


  GR::Graphic::ContextDescriptor* pCDSourceDummy = m_pCDSource->ConvertTo( GR::Graphic::IF_X8R8G8B8 );
  if ( pCDSourceDummy == NULL )
  {
    return;
  }


  GR::Graphic::ContextDescriptor* pCDDummy = Plasma.PerformAction( pCDSourceDummy );

  delete pCDSourceDummy;

  if ( pCDDummy )
  {
    if ( pCDDummy->ConvertSelfTo( m_pCDSource->ImageFormat() ) )
    {
      pCDDummy->CopyArea( 0, 0, pCDDummy->Width(), pCDDummy->Height(), 0, 0, m_pCDTarget );
    }
    delete pCDDummy;
  }
}


