// ChildFrm.cpp : Implementierung der Klasse CChildFrame
//

#include "stdafx.h"

#include "ChildFrm.h"

#include <MFC/ODMenu.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
  ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame Konstruktion/Destruktion

CChildFrame::CChildFrame()
{

}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame Diagnose

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG



int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	return 0;
}






void CChildFrame::ActivateFrame( int nCmdShow ) 
{
	
	CMDIChildWnd::ActivateFrame(nCmdShow);

}



void CChildFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{

  if ( nHitTest == HTSYSMENU )
  {
    ODMenu   odSysMenu( ODMenu::ODMS_XP );

    HMENU hSysMenu = GetSystemMenu( FALSE )->GetSafeHmenu();

    SendMessage( WM_INITMENU, (WPARAM)hSysMenu, 0 );
    SendMessage( WM_INITMENUPOPUP, (WPARAM)hSysMenu, MAKELPARAM( 0, TRUE ) );

    odSysMenu.Clone( hSysMenu, GetSafeHwnd() );

    RECT    rcClient;

    GetClientRect( &rcClient );
    ClientToScreen( &rcClient );

    int   iX = rcClient.left;
    int   iY = rcClient.top;

    DWORD   dwResult = odSysMenu.TrackPopupMenu( TPM_NONOTIFY | TPM_RETURNCMD, iX, iY, this );
    if ( dwResult )
    {
      PostMessage( WM_SYSCOMMAND, dwResult, 0 );
    }
    return;
  }

  CMDIChildWnd::OnNcLButtonDown(nHitTest, point);
}
