// GRMiniDockFrameWnd.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "GRMiniDockFrameWnd.h"
#include "GRDockingManager.h"

#include <debug/debugclient.h>

#include "GRControlBar.h"
#include ".\grminidockframewnd.h"

// GRMiniDockFrameWnd


BOOL GRMiniDockFrameWnd::hasclass = GRMiniDockFrameWnd::RegisterMe();

BOOL GRMiniDockFrameWnd::RegisterMe()
{
  WNDCLASS wc;   
  wc.style = 0;                                                 
  wc.lpfnWndProc = ::DefWindowProc; // must be this value
  wc.cbClsExtra = 0;                         
  wc.cbWndExtra = 0;                               
  wc.hInstance = (HINSTANCE)::GetModuleHandle(NULL);
  wc.hIcon = NULL;     // child window has no icon         
  wc.hCursor = LoadCursor( NULL, IDC_ARROW );
  wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  wc.lpszMenuName = NULL;  // no menu                             
  wc.lpszClassName = GRMINIDOCKFRAMEWND_CLASS_NAME;

  return AfxRegisterClass(&wc);
}




IMPLEMENT_DYNCREATE(GRMiniDockFrameWnd, CMiniDockFrameWnd)

GRMiniDockFrameWnd::GRMiniDockFrameWnd() :
  m_pControlBar( NULL ),
  m_dfPos( DFP_FREE )
{

  m_listControlBars.clear();

}

GRMiniDockFrameWnd::~GRMiniDockFrameWnd()
{
}


BEGIN_MESSAGE_MAP(GRMiniDockFrameWnd, CMiniDockFrameWnd)
  ON_WM_NCLBUTTONDOWN()
  ON_WM_SIZE()
  ON_WM_MOUSEACTIVATE()
  ON_WM_NCHITTEST()
  ON_WM_NCMOUSEMOVE()
  ON_WM_MOUSEMOVE()
  ON_WM_NCLBUTTONDBLCLK()
  ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
ON_WM_LBUTTONUP()
ON_WM_ERASEBKGND()
ON_WM_CLOSE()
ON_WM_CONTEXTMENU()
ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// GRMiniDockFrameWnd-Meldungshandler

void GRMiniDockFrameWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
{

	// special activation for floating toolbars
  if ( nHitTest == HTCAPTION )
  {
    StartDragging( point );
  }
	ActivateTopParent();
  CWnd::OnNcLButtonDown( nHitTest, point );

}



void GRMiniDockFrameWnd::StartDragging( CPoint point )
{

	RECT rRect = {NULL};
	GetWindowRect( &rRect );

  if ( rRect.left < -30000 )
  {
    SetWindowPos( NULL, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
    GetWindowRect( &rRect );
  }

	m_ptDragOffset.x = point.x - rRect.left;
	m_ptDragOffset.y = point.y - rRect.top;
  m_Dragging = true;
  SetCapture();

}



void GRMiniDockFrameWnd::OnSize(UINT nType, int cx, int cy)
{
  if ( m_pControlBar )
  {
    RECT    rc;
    GetClientRect( &rc );
    if ( m_pControlBar->IsFloating() )
    {
      m_pControlBar->MoveWindow( 0, 0, rc.right - rc.left, rc.bottom - rc.top );
    }
    else
    {
      m_pControlBar->SetWindowPos( NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE );
    }
  }
  CWnd::OnSize( nType, cx, cy );
}

int GRMiniDockFrameWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
  return CMiniDockFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

LRESULT GRMiniDockFrameWnd::OnNcHitTest(CPoint point)
{
  return CWnd::OnNcHitTest(point);
}

void GRMiniDockFrameWnd::OnNcMouseMove(UINT nHitTest, CPoint point)
{
  CMiniDockFrameWnd::OnNcMouseMove(nHitTest, point);
}

void GRMiniDockFrameWnd::OnMouseMove(UINT nFlags, CPoint point)
{
  if ( ( m_Dragging )
  &&   ( m_pControlBar ) )
  {
    RECT      rc;
    GetWindowRect( &rc );

    SIZE szSize = { rc.right - rc.left, rc.bottom - rc.top };

    POINT   ptMouse;
    GetCursorPos( &ptMouse );

    POINT   ptPos;

    /*
    ptPos.x = ptMouse.x - m_ptDragOffset.x;
    ptPos.y = ptMouse.y - m_ptDragOffset.y;
    */
    ptPos = ptMouse;

    DWORD   dwDockFlags = GRDockingManager::CheckForDockingPosition( m_pControlBar, ptPos );
    if ( dwDockFlags )
    {
      // irgendwo angedockt?
      m_pControlBar->Dock( dwDockFlags, ptPos.x, ptPos.y );
    }
    else
    {
      m_pControlBar->Float( ptPos.x - m_ptDragOffset.x, ptPos.y - m_ptDragOffset.y );
    }
    if ( m_pControlBar->IsFloating() )
    {
      SetWindowPos( NULL, ptPos.x - m_ptDragOffset.x, ptPos.y - m_ptDragOffset.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
    }
  }
  CWnd::OnMouseMove(nFlags, point);
}

BOOL GRMiniDockFrameWnd::Create( CWnd* pParent )
{

  return CMiniFrameWnd::CreateEx( WS_EX_TOOLWINDOW, NULL, _T( "" ), WS_SYSMENU | MFS_SYNCACTIVE | WS_CAPTION | WS_POPUP, rectDefault, pParent );

}




void GRMiniDockFrameWnd::RecalcLayout(BOOL bNotify)
{
  CMiniFrameWnd::RecalcLayout(bNotify);
}

void GRMiniDockFrameWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
  if ( ( m_dfPos == DFP_FREE )
  &&   ( nHitTest == HTCAPTION ) )
  {
    if ( m_pControlBar )
    {
      m_pControlBar->ToggleState();
    }
  }
  CMiniFrameWnd::OnNcLButtonDblClk(nHitTest, point);
}


void GRMiniDockFrameWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
  if ( m_Dragging )
  {
    m_Dragging = false;
    if ( GetCapture() == this )
    {
      ReleaseCapture();
    }
  }

  CMiniDockFrameWnd::OnLButtonUp(nFlags, point);
}



void GRMiniDockFrameWnd::InsertControlBar( GRControlBar* pBar )
{

  m_listControlBars.push_back( pBar );

}



void GRMiniDockFrameWnd::RemoveControlBar( GRControlBar* pBar )
{

  CFrameWnd*    pTopFrame = AfxGetMainWnd()->GetTopLevelFrame();

  RECT    rc;

  GetWindowRect( &rc );
  pTopFrame->ScreenToClient( &rc );

  std::list<GRControlBar*>::iterator   it( m_listControlBars.begin() );
  while ( it != m_listControlBars.end() )
  {
    GRControlBar*  pThatBar = *it;

    if ( pBar == pThatBar )
    {
      it = m_listControlBars.erase( it );
      break;
    }
    ++it;
  }

}



LRESULT GRMiniDockFrameWnd::OnSizeParent( WPARAM wParam, LPARAM lParam )
{

  if ( m_dfPos == DFP_FREE )
  {
    return 0;
  }

  AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;

  RECT    rcRemainingSize;
  RECT    rc;
  RECT    rcClientInMDI;
  RECT    rcMDI;

  GetClientRect( &rc );

  GetTopLevelFrame()->GetClientRect( &rcMDI );

  rcMDI = lpLayout->rect;

  rcClientInMDI = rc;
  ClientToScreen( &rcClientInMDI );
  GetTopLevelFrame()->ScreenToClient( &rcClientInMDI );

  CalculateNeededSize( rcRemainingSize );

  int   iRemainingWidth = rcRemainingSize.right - rcRemainingSize.left;
  int   iRemainingHeight = rcRemainingSize.bottom - rcRemainingSize.top;


  // TODO - mehr als ein ControlBar in einem Framewnd!!
  switch ( m_dfPos )
  {
    case DFP_TOP:
      SetWindowPos( NULL, rcMDI.left, rcMDI.top, rcMDI.right - rcMDI.left, iRemainingHeight, SWP_NOZORDER );
      break;
    case DFP_BOTTOM:
      SetWindowPos( NULL, rcMDI.left, rcMDI.bottom - iRemainingHeight, rcMDI.right - rcMDI.left, iRemainingHeight, SWP_NOZORDER );
      break;
    case DFP_LEFT:
      //SetWindowPos( NULL, 0, 0, iRemainingWidth, rcMDI.bottom - rcMDI.top, SWP_NOZORDER );
      SetWindowPos( NULL, rcMDI.left, rcMDI.top, iRemainingWidth, rcMDI.bottom - rcMDI.top, SWP_NOZORDER );
      break;
    case DFP_RIGHT:
      SetWindowPos( NULL, rcMDI.right - iRemainingWidth, rcMDI.top, iRemainingWidth, rcMDI.bottom - rcMDI.top, SWP_NOZORDER );
      break;
  }

  GetClientRect( &rc );

  RECT    rcAvail = lpLayout->rect;

  SIZE    szDockBar;
  
  szDockBar.cx = rc.right - rc.left;
  szDockBar.cy = rc.bottom - rc.top;

	szDockBar.cx = min( szDockBar.cx, lpLayout->rect.right - lpLayout->rect.left );
	szDockBar.cy = min( szDockBar.cy, lpLayout->rect.bottom - lpLayout->rect.top );

  switch ( m_dfPos )
  {
    case DFP_TOP:
			lpLayout->sizeTotal.cy += szDockBar.cy;
			lpLayout->sizeTotal.cx = max(lpLayout->sizeTotal.cx, szDockBar.cx);
      lpLayout->rect.top += szDockBar.cy;
      break;
    case DFP_BOTTOM:
			lpLayout->sizeTotal.cy += szDockBar.cy;
			lpLayout->sizeTotal.cx = max(lpLayout->sizeTotal.cx, szDockBar.cx);
			rcAvail.top = rcAvail.bottom - szDockBar.cy;
			lpLayout->rect.bottom -= szDockBar.cy;
      break;
    case DFP_LEFT:
			lpLayout->sizeTotal.cx += szDockBar.cx;
			lpLayout->sizeTotal.cy = max(lpLayout->sizeTotal.cy, szDockBar.cy);
      lpLayout->rect.left += szDockBar.cx;
      break;
    case DFP_RIGHT:
			lpLayout->sizeTotal.cx += szDockBar.cx;
			lpLayout->sizeTotal.cy = max(lpLayout->sizeTotal.cy, szDockBar.cy);
			rcAvail.left = rcAvail.right - szDockBar.cx;
			lpLayout->rect.right -= szDockBar.cx;
      break;
  }

  return 0;

}
BOOL GRMiniDockFrameWnd::OnEraseBkgnd(CDC* pDC)
{

  RECT    rc;
  GetClientRect( &rc );
  pDC->FillSolidRect( &rc, GetSysColor( COLOR_BTNFACE ) );

  return TRUE;
}



void GRMiniDockFrameWnd::CalculateNeededSize( RECT& rcRemainingSize )
{

  RECT    rcNow;

  GetClientRect( &rcNow );

  SetRectEmpty( &rcRemainingSize );

  std::list<GRControlBar*>::iterator   it( m_listControlBars.begin() );
  while ( it != m_listControlBars.end() )
  {
    GRControlBar*  pThatBar = *it;

    if ( !( pThatBar->m_CBStyle & GRControlBar::GRCBS_HIDDEN ) )
    {
      RECT    rcBar;

      pThatBar->GetClientRect( &rcBar );

      // neue, tatsächliche Position der Bars benutzen
      pThatBar->ClientToScreen( &rcBar );
      ScreenToClient( &rcBar );

      if ( rcBar.top < 0 )
      {
        OffsetRect( &rcBar, 0, -rcBar.top );
        pThatBar->SetWindowPos( NULL, rcBar.left, rcBar.top, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top, SWP_NOZORDER );
      }
      
      UnionRect( &rcRemainingSize, &rcRemainingSize, &rcBar );
    }

    ++it;
  }
  
  if ( GRDockingManager::m_LoadingState )
  {
    return;
  }

  int   iDeltaX = 0,
        iDeltaY = 0;


  if ( m_dfPos == DFP_TOP )
  {
    if ( rcRemainingSize.top > 0 )
    {
      // oben ist eine Lücke, alle ChildBars nachrutschen lassen
      iDeltaY = -rcRemainingSize.top;
    }
  }
  else if ( m_dfPos == DFP_LEFT )
  {
    if ( rcRemainingSize.left > 0 )
    {
      // links ist eine Lücke, alle ChildBars nachrutschen lassen
      iDeltaX = -rcRemainingSize.left;
    }
  }
  else if ( m_dfPos == DFP_BOTTOM )
  {
    if ( rcRemainingSize.top < 0 )
    {
      iDeltaY = -rcRemainingSize.top;
    }
    else if ( rcRemainingSize.top > 0 )
    {
      // oben ist eine Lücke, alle ChildBars nachrutschen lassen
      iDeltaY = -rcRemainingSize.top;
    }
  }
  else if ( m_dfPos == DFP_RIGHT )
  {
    if ( rcRemainingSize.left < 0 )
    {
      iDeltaX = -rcRemainingSize.left;
    }
    else if ( rcRemainingSize.left > 0 )
    {
      // oben ist eine Lücke, alle ChildBars nachrutschen lassen
      iDeltaX = -rcRemainingSize.left;
    }
  }

  if ( ( iDeltaX != 0 )
  ||   ( iDeltaY != 0 ) )
  {
    std::list<GRControlBar*>::iterator   it( m_listControlBars.begin() );
    while ( it != m_listControlBars.end() )
    {
      GRControlBar*  pThatBar = *it;

      RECT    rc;
      pThatBar->GetClientRect( &rc );
      pThatBar->ClientToScreen( &rc );
      ScreenToClient( &rc );

      OffsetRect( &rc, iDeltaX, iDeltaY );
      pThatBar->MoveWindow( &rc );

      pThatBar->m_MRUDockedPos.x += iDeltaX;
      pThatBar->m_MRUDockedPos.y += iDeltaY;

      ++it;
    }
    OffsetRect( &rcRemainingSize, iDeltaX, iDeltaY );
  }
}



void GRMiniDockFrameWnd::RepositionControlBars( GRControlBar* pBar, POINT& ptBarPos, DWORD dwDockPos )
{
  POINT   ptNewBarPos = ptBarPos;

  RECT    rcDockBar;

  GetClientRect( &rcDockBar );
  ClientToScreen( &rcDockBar );

  if ( ( ( m_dfPos == DFP_TOP )
  ||     ( m_dfPos == DFP_BOTTOM ) )
  &&   ( ptNewBarPos.x < rcDockBar.left ) )
  {
    ptNewBarPos.x = rcDockBar.left;
  }
  if ( ( ( m_dfPos == DFP_LEFT )
  ||     ( m_dfPos == DFP_RIGHT ) )
  &&   ( ptNewBarPos.y < rcDockBar.top ) )
  {
    ptNewBarPos.y = rcDockBar.top;
  }
  if ( m_dfPos == DFP_TOP )
  {
    if ( ptNewBarPos.y < rcDockBar.top )
    {
      ptNewBarPos.y = rcDockBar.top;
    }
  }

  // Bars anordnen
  std::list<GRControlBar*>::iterator   it( m_listControlBars.begin() );
  while ( it != m_listControlBars.end() )
  {
    GRControlBar*  pThatBar = *it;

    if ( pBar != pThatBar )
    {
      RECT    rcNewBar;

      pBar->GetClientRect( &rcNewBar );
      OffsetRect( &rcNewBar, ptNewBarPos.x, ptNewBarPos.y );
      ScreenToClient( &rcNewBar );

      RECT    rcOldBar;

      pThatBar->GetClientRect( &rcOldBar );
      pThatBar->ClientToScreen( &rcOldBar );
      ScreenToClient( &rcOldBar );

      RECT    rcTemp;

      if ( IntersectRect( &rcTemp, &rcNewBar, &rcOldBar ) )
      {
        // die überlappen sich!
        if ( ( m_dfPos == DFP_LEFT )
        ||   ( m_dfPos == DFP_RIGHT ) )
        {
          ClientToScreen( &rcOldBar );
          ptNewBarPos.y = rcOldBar.bottom;

          if ( ptNewBarPos.y + rcNewBar.bottom - rcNewBar.top > rcDockBar.bottom )
          {
            // außerhalb des Rects
            if ( m_dfPos == DFP_LEFT )
            {
              ptNewBarPos.x += rcNewBar.right - rcNewBar.left;
            }
            else if ( m_dfPos == DFP_RIGHT )
            {
              ptNewBarPos.x -= rcNewBar.right - rcNewBar.left;
            }
            ptNewBarPos.y = rcDockBar.top;
          }
        }
        else
        {
          ClientToScreen( &rcOldBar );
          ptNewBarPos.x = rcOldBar.right;

          if ( ptNewBarPos.x + rcNewBar.right - rcNewBar.left > rcDockBar.right )
          {
            // außerhalb des Rects
            if ( m_dfPos == DFP_TOP )
            {
              ptNewBarPos.y += rcNewBar.bottom - rcNewBar.top;
            }
            else if ( m_dfPos == DFP_BOTTOM )
            {
              ptNewBarPos.y -= rcNewBar.bottom - rcNewBar.top;
            }
            ptNewBarPos.x = rcDockBar.left;
          }
        }

        it = m_listControlBars.begin();
        continue;
      }
    }
    ++it;
  }

  ptBarPos = ptNewBarPos;

}
void GRMiniDockFrameWnd::OnClose()
{

  if ( m_dfPos == DFP_FREE )
  {
    m_pControlBar->Show( FALSE );
  }

  CMiniDockFrameWnd::OnClose();
}



GRMiniDockFrameWnd::eDockFramePos GRMiniDockFrameWnd::DockPos() const
{
  return m_dfPos;
}



void GRMiniDockFrameWnd::OnContextMenu( CWnd* pWnd, CPoint point )
{
  if ( pWnd != this )
  {
    return;
  }
  
  HMENU   hPopupMenu = CreatePopupMenu();

  int   iIndex = 1;
  std::list<GRControlBar*>::iterator   it( GRDockingManager::m_ControlBars.begin() );
  while ( it != GRDockingManager::m_ControlBars.end() )
  {
    GRControlBar*    pBar = *it;

    CString   cstrGnu;

    pBar->GetWindowText( cstrGnu );

    DWORD   dwFlags = MF_STRING | MF_ENABLED;

    if ( pBar->IsWindowVisible() )
    {
      dwFlags |= MF_CHECKED;
    }

    AppendMenu( hPopupMenu, dwFlags, iIndex++, LPCTSTR( cstrGnu ) );

    ++it;
  }

  DWORD dwResult = TrackPopupMenu( hPopupMenu, TPM_NONOTIFY | TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, 0, AfxGetMainWnd()->GetSafeHwnd(), 0 );

  if ( dwResult )
  {
    int   iIndex = 1;
    std::list<GRControlBar*>::iterator   it( GRDockingManager::m_ControlBars.begin() );
    while ( it != GRDockingManager::m_ControlBars.end() )
    {
      GRControlBar*    pBar = *it;

      if ( iIndex == dwResult )
      {
        pBar->Show( !pBar->IsWindowVisible() );
        break;
      }

      ++iIndex;
      ++it;
    }
  }

  DestroyMenu( hPopupMenu );
}



void GRMiniDockFrameWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
  if ( m_dfPos == DFP_FREE )
  {
    if ( !m_pControlBar->SendMessage( WM_GETMINMAXINFO, 0, (LPARAM)lpMMI ) )
    {
      return;
    }
  }

  CMiniDockFrameWnd::OnGetMinMaxInfo(lpMMI);
}
