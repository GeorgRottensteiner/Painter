#include "stdafx.h"
#include "EffektTemplate.h"
#include "DlgEffekte.h"

#pragma warning ( disable : 4786 )
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



EffektTemplate::EffektTemplate(DWORD dwID, CWnd* pParent /*=NULL*/)
	: CDialog(dwID, pParent),
    m_DoNotUpdate( FALSE ),
    m_pDlg( NULL )
{
	//{{AFX_DATA_INIT(EffektTemplate)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_FramePos = 0;
  m_dwRessourceID = dwID;
}



void EffektTemplate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EffektTemplate)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EffektTemplate, CDialog)
	//{{AFX_MSG_MAP(EffektTemplate)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



EffektTemplate::~EffektTemplate()
{
}


void EffektTemplate::RestartRendering()
{
  if ( m_pDlg )
  {
    m_pDlg->PostMessage( WM_COMMAND, 
                            MAKEWPARAM( IDC_BUTTON_RENDER, BN_CLICKED ),
                            (LPARAM)m_pDlg->GetDlgItem( IDC_BUTTON_RENDER )->GetSafeHwnd() );
  }
}


void EffektTemplate::Init()
{
}



void EffektTemplate::CleanUp()
{
}



void EffektTemplate::NextFrame()
{
  std::swap( m_pCDTarget, m_pCDLastFrame );
  m_FramePos = 1 - m_FramePos;
}



void EffektTemplate::OnOK()
{
}



void EffektTemplate::OnCancel()
{
}



