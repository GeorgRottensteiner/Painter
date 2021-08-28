#include "stdafx.h"
#include "GRDialogBar.h"
#include ".\grdialogbar.h"



IMPLEMENT_DYNAMIC(GRDialogBar, GRControlBar)
  GRDialogBar::GRDialogBar()

{
  m_hwndDialog      = NULL;
  m_Horizontal     = true;
}



GRDialogBar::~GRDialogBar()
{
}


BEGIN_MESSAGE_MAP(GRDialogBar, GRControlBar)
  ON_WM_LBUTTONDOWN()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()



BOOL GRDialogBar::Create( CWnd* pParentWnd, HWND hwndDialog, DWORD dwControlStyle, UINT nID, const GR::tChar* szTitle )
{
  if ( hwndDialog == NULL )
  {
    return FALSE;
  }

  BOOL    bResult = GRControlBar::Create( szTitle, dwControlStyle, pParentWnd, nID );

  if ( !bResult )
  {
    return FALSE;
  }

  m_hwndDialog = hwndDialog;

  CWnd*   pWnd = CWnd::FromHandle( hwndDialog );

  pWnd->SetParent( this );
  pWnd->ModifyStyle( WS_POPUP, WS_CHILD );
  pWnd->SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
  pWnd->ShowWindow( SW_SHOW );

  CalcSize();

  return bResult;
}



CSize GRDialogBar::EstimateSize( GR::u32 SizingFlags )
{
  bool      estimateHorizontal = true;

  if ( ( SizingFlags & GRCBS_DOCKED_LEFT )
  ||   ( SizingFlags & GRCBS_DOCKED_RIGHT ) )
  {
    estimateHorizontal = false;
  }

  CSize     szCB( 0, 0 );

  RECT    rc;

  ::GetWindowRect( m_hwndDialog, &rc );

  ScreenToClient( &rc );

  szCB.cx = rc.right - rc.left;
  szCB.cy = rc.bottom - rc.top;

  if ( !IsFloating() )
  {
    // angedockt - Gripper oben drauf
    if ( estimateHorizontal )
    {
      if ( rc.left == 0 )
      {
        if ( SizingFlags & GRCBS_APPLY_SIZE )
        {
        ::SetWindowPos( m_hwndDialog, NULL, 4, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
      }
      }
      szCB.cx += 4;
    }
    else
    {
      if ( rc.left == 0 )
      {
        if ( SizingFlags & GRCBS_APPLY_SIZE )
        {
        ::SetWindowPos( m_hwndDialog, NULL, 0, 4, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
      }
      }
      szCB.cy += 4;
    }
  }
  else
  {
    estimateHorizontal = true;
    if ( ( rc.left )
    ||   ( rc.top ) )
    {
      ::SetWindowPos( m_hwndDialog, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
    }
  }

  if ( SizingFlags & GRCBS_APPLY_SIZE )
  {
    m_Horizontal = estimateHorizontal;
  }

  return szCB;
}



void GRDialogBar::OnLButtonDown( UINT nFlags, CPoint point )
{
  if ( !IsFloating() )
  {
    RECT    rcGripper;

    GetClientRect( &rcGripper );

    if ( m_Horizontal )
    {
      rcGripper.right   = 4;
    }
    else
    {
      rcGripper.bottom  = 4;
    }
    if ( PtInRect( &rcGripper, point ) )
    {
      m_pFrameWnd->StartDragging( point );
      return;
    }
  }
  GRControlBar::OnLButtonDown( nFlags, point );
}



void GRDialogBar::OnPaint()
{
  CPaintDC dc( this );

  RECT    rc;

  GetClientRect( &rc );

  if ( !IsFloating() )
  {
    // Gripper zeichnen
    if ( m_Horizontal )
    {
      dc.Draw3dRect( 0, 0, 2, rc.bottom - rc.top, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
      dc.Draw3dRect( 2, 0, 2, rc.bottom - rc.top, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
    }
    else
    {
      dc.Draw3dRect( 0, 0, rc.right - rc.left, 2, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
      dc.Draw3dRect( 0, 2, rc.right - rc.left, 2, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
    }
  }
}



void GRDialogBar::OnSize( UINT nType, int cx, int cy )
{
  GRControlBar::OnSize( nType, cx, cy );

  if ( m_hwndDialog )
  {
    RECT    rc;

    GetClientRect( &rc );

    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;

    if ( !IsFloating() )
    {
      if ( m_Horizontal )
      {
        cx -= 4;
      }
      else
      {
        cy -= 4;
      }
    }
    ::SetWindowPos( m_hwndDialog, NULL, 0, 0, cx, cy,
          SWP_NOMOVE | SWP_NOZORDER );
  }
}



void GRDialogBar::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
  if ( !::SendMessage( m_hwndDialog, WM_GETMINMAXINFO, 0, (LPARAM)lpMMI ) )
  {
    return;
  }

  GRControlBar::OnGetMinMaxInfo( lpMMI );
}
