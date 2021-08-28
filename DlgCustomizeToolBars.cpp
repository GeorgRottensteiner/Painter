#include "stdafx.h"
#include "painter.h"
#include "GRDockingManager.h"
#include "DlgCustomizeToolBars.h"
#include "MainFrm.h"


IMPLEMENT_DYNAMIC(CDlgCustomizeToolBars, CDialog)
CDlgCustomizeToolBars::CDlgCustomizeToolBars(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCustomizeToolBars::IDD, pParent)
{
  InitCommonControls();
}



CDlgCustomizeToolBars::~CDlgCustomizeToolBars()
{
}



void CDlgCustomizeToolBars::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_TOOLBARS, m_ListToolBars);
}


BEGIN_MESSAGE_MAP(CDlgCustomizeToolBars, CDialog)
  ON_CLBN_CHKCHANGE( IDC_LIST_TOOLBARS, OnListToolBarsCheckChange )
END_MESSAGE_MAP()



BOOL CDlgCustomizeToolBars::OnInitDialog()
{
  CDialog::OnInitDialog();

  CMainFrame::tMapControlBars::iterator   it( theApp.pMainFrame->m_mapControlBars.begin() );
  while ( it != theApp.pMainFrame->m_mapControlBars.end() )
  {
    GRControlBar*   pBar = it->second;

    int iItem = m_ListToolBars.AddString( it->first.c_str() );
    m_ListToolBars.SetItemData( iItem, (DWORD_PTR)pBar );

    if ( pBar->IsWindowVisible() )
    {
      m_ListToolBars.SetCheck( iItem, 1 );
    }

    ++it;
  }
  return TRUE;
}



void CDlgCustomizeToolBars::OnListToolBarsCheckChange()
{
  int   iItem = m_ListToolBars.GetCurSel();
  if ( iItem == LB_ERR )
  {
    return;
  }

  GRControlBar*   pBar = (GRControlBar*)m_ListToolBars.GetItemData( iItem );

  GRDockingManager::ShowControlBar( pBar, !pBar->IsWindowVisible() );
}