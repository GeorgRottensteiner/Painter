#include "stdafx.h"
#include "GRControlBar.h"
#include "GRMiniDockFrameWnd.h"
#include "GRDockingManager.h"

#include <debug/debugclient.h>



BOOL GRControlBar::hasclass = GRControlBar::RegisterMe();

BOOL GRControlBar::RegisterMe()
{
  WNDCLASS wc;   
  wc.style          = 0;                                                 
  wc.lpfnWndProc    = ::DefWindowProc; // must be this value
  wc.cbClsExtra     = 0;                         
  wc.cbWndExtra     = 0;                               
  wc.hInstance      = (HINSTANCE)::GetModuleHandle(NULL);
  wc.hIcon          = NULL;     // child window has no icon         
  wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
  wc.hbrBackground  = (HBRUSH)(COLOR_BTNFACE + 1);
  wc.lpszMenuName   = NULL;  // no menu                             
  wc.lpszClassName  = GRCONTROLBAR_CLASS_NAME;

  return AfxRegisterClass(&wc);
}




IMPLEMENT_DYNCREATE(GRControlBar, CMiniFrameWnd)

GRControlBar::GRControlBar() :
  m_Floating( true ),
  m_pFrameWnd( NULL ),
  m_CBStyle( GRCBS_FIXED_SIZE ),
  m_MRUDockPos( 0 )
{
  m_MRUDockedPos.x    = 0;
  m_MRUDockedPos.y    = 0;
  m_MRUFloatingPos.x  = 0;
  m_MRUFloatingPos.y  = 0;
}



GRControlBar::~GRControlBar()
{
  DestroyWindow();
}



BEGIN_MESSAGE_MAP(GRControlBar, CMiniFrameWnd)
  ON_WM_MOVE()
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
  ON_WM_CLOSE()
END_MESSAGE_MAP()



BOOL GRControlBar::Create( LPCTSTR lpWindowName, DWORD dwCBStyle, CWnd* pParentWnd, UINT nID )
{
  m_pFrameWnd = new GRMiniDockFrameWnd();

  if ( !m_pFrameWnd->Create( pParentWnd ) )
  {
    delete m_pFrameWnd;
    m_pFrameWnd = NULL;
    return FALSE;
  }

  if ( !CMiniFrameWnd::Create( GRCONTROLBAR_CLASS_NAME, lpWindowName, WS_CHILD, rectDefault, m_pFrameWnd, nID ) )
  {
    delete m_pFrameWnd;
    m_pFrameWnd = NULL;
    return FALSE;
  }

  SetOwner( pParentWnd );

  m_CBStyle = dwCBStyle;

  if ( dwCBStyle & GRCBS_FREE_SIZE )
  {
    m_pFrameWnd->ModifyStyle( 0, WS_THICKFRAME );
  }

  m_pFrameWnd->m_pControlBar = this;

  RECT    rc;

  CSize   szControlBar = CalcSize();
  SetRect( &rc, 0, 0, szControlBar.cx, szControlBar.cy );
  AdjustWindowRectEx( &rc, m_pFrameWnd->GetStyle(), FALSE, m_pFrameWnd->GetExStyle() );

  CString   cstrCaption;

  GetWindowText( cstrCaption );
  m_pFrameWnd->SetWindowText( cstrCaption );
  m_pFrameWnd->SetWindowPos( NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER );
  MoveWindow( 0, 0, szControlBar.cx, szControlBar.cy );
  //m_pFrameWnd->ShowWindow( SW_SHOW );
  ShowWindow( SW_SHOW );

  GRDockingManager::AddControlBar( this );

  EnableToolTips();

  return TRUE;
}



LRESULT GRControlBar::OnIdleUpdateCmdUI( WPARAM wParam, LPARAM )
{
	// handle delay hide/show
  /*
	BOOL bVis = GetStyle() & WS_VISIBLE;
	UINT swpFlags = 0;
	if ((m_nStateFlags & delayHide) && bVis)
		swpFlags = SWP_HIDEWINDOW;
	else if ((m_nStateFlags & delayShow) && !bVis)
		swpFlags = SWP_SHOWWINDOW;
	m_nStateFlags &= ~(delayShow|delayHide);
	if (swpFlags != 0)
	{
		SetWindowPos(NULL, 0, 0, 0, 0, swpFlags|
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}
  */

	// the style must be visible and if it is docked
	// the dockbar style must also be visible
	if ( ( GetStyle() & WS_VISIBLE ) 
  &&   ( ( IsFloating() )
  ||     ( ( m_pFrameWnd->GetStyle() & WS_VISIBLE ) ) ) )
	{
		CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
		if ( ( pTarget == NULL )
    ||   ( !pTarget->IsFrameWnd() ) )
    {
			pTarget = GetParentFrame();
    }
    if ( pTarget != NULL )
    {
      OnUpdateCmdUI( pTarget, (BOOL)wParam );
    }
	}
	return 0L;
}



void GRControlBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
}



void GRControlBar::PostNcDestroy()
{
  GRDockingManager::RemoveControlBar( this );
  m_pFrameWnd = NULL;

  CWnd::PostNcDestroy();
}



CSize GRControlBar::EstimateSize( GR::u32 SizingFlags )
{
  CSize     szCB( 120, 24 );

  return szCB;
}



CSize GRControlBar::CalcSize( GR::u32 SizingFlags )
{
  CSize     szCB( 20, 20 );

  if ( ( IsFloating() )
  &&   ( !m_pFrameWnd->IsWindowVisible() ) )
  {
    szCB.cx = 0;
    szCB.cy = 0;
    //return szCB;
  }

  if ( ( !IsFloating() )
  &&   ( m_CBStyle & GRCBS_HIDDEN ) )
  {
    szCB.cx = 0;
    szCB.cy = 0;
    return szCB;
  }

  /*
  if ( m_dwCBStyle & GRCB_FREE_SIZE )
  {
    RECT    rc;

    GetClientRect( &rc );

    if ( ( rc.right - rc.left )
    ||   ( rc.bottom - rc.top ) )
    {
      szCB.cx = rc.right - rc.left;
      szCB.cy = rc.bottom - rc.top;
    }
  }
  */

  szCB = EstimateSize( SizingFlags );

  if ( SizingFlags & GRCBS_APPLY_SIZE )
  {
    RECT    rc;

    rc.left = 0;
    rc.top = 0;
    rc.right = szCB.cx;
    rc.bottom = szCB.cy;

    AdjustWindowRectEx( &rc, m_pFrameWnd->GetStyle(), FALSE, m_pFrameWnd->GetExStyle() );

    m_pFrameWnd->SetWindowPos( NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER );
  }

  return szCB;
}



void GRControlBar::OnMove( int x, int y )
{
  CMiniFrameWnd::OnMove( x, y );
}



bool GRControlBar::IsFloating()
{
  return m_Floating;
}



void GRControlBar::Dock( GR::u32 DockPos, int X, int Y )
{
  POINT   pos;
  pos.x = X;
  pos.y = Y;

  GRDockingManager::DockControlBar( this, DockPos, pos );

  m_MRUDockedPos  = pos;
  m_MRUDockPos    = DockPos;

  GetParent()->ScreenToClient( &pos );

  if ( pos.y < 0 )
  {
    pos.y = 0;
  }
  if ( pos.x < 0 )
  {
    pos.x = 0;
  }
  SetWindowPos( NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

  /*
  if ( ( GetParent() )
  &&   ( !m_bFloating ) )
  {
    GRMiniDockFrameWnd* pFrameWnd = (GRMiniDockFrameWnd*)GetParent();

    GetParent()->ClientToScreen( &ptPos );
    pFrameWnd->RepositionControlBars( this, ptPos, dwDockPos );

    SetWindowPos( NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
  }
  */

  GetTopLevelFrame()->RecalcLayout();
}



void GRControlBar::Float( int X, int Y )
{
  m_MRUFloatingPos.x  = X;
  m_MRUFloatingPos.y  = Y;
  if ( IsFloating() )
  {
    if ( m_pFrameWnd )
    {
      m_pFrameWnd->SetWindowPos( NULL, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
    }
    return;
  }

  m_Floating = true;

  // DockBar-Größe ändern
  GRMiniDockFrameWnd*  pParent = (GRMiniDockFrameWnd*)GetParent();

  pParent->RemoveControlBar( this );

  m_CBStyle &= ~( GRCBS_DOCKED_LEFT | GRCBS_DOCKED_RIGHT | GRCBS_DOCKED_TOP | GRCBS_DOCKED_BOTTOM | GRCBS_DOCKED );

  CalcSize();

  SetParent( m_pFrameWnd );
  SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
  m_pFrameWnd->SetWindowPos( NULL, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
  m_pFrameWnd->ShowWindow( SW_SHOW );

  GetTopLevelFrame()->RecalcLayout();
}



void GRControlBar::ToggleState()
{
  if ( IsFloating() )
  {
    if ( m_MRUDockPos )
    {
      POINT   dockPos = m_MRUDockedPos;

      // Koordinaten anpassen lassen
      if ( GRDockingManager::CheckForDockingPosition( this, dockPos ) )
      {
        Dock( m_MRUDockPos, dockPos.x, dockPos.y );
      }
    }
  }
  else
  {
    Float( m_MRUFloatingPos.x, m_MRUFloatingPos.y );
  }
}



void GRControlBar::OnPaint()
{
  CPaintDC dc( this );

  RECT    rc;

  GetClientRect( &rc );

  dc.DrawText( _T( "Hurz" ), 4, &rc, DT_SINGLELINE );

  if ( !IsFloating() )
  {
    rc.bottom = rc.top + 10;

    dc.DrawFocusRect( &rc );
  }
}



void GRControlBar::OnLButtonDown( UINT nFlags, CPoint point )
{
  CMiniFrameWnd::OnLButtonDown( nFlags, point );
}



void GRControlBar::Show( bool Show )
{
  GRDockingManager::ShowControlBar( this, Show );
}



void GRControlBar::OnClose()
{
  Show( FALSE );

  CMiniFrameWnd::OnClose();
}

