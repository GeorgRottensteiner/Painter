#include "StdAfx.h"
#include "grdockingmanager.h"

#include "GRMiniDockFrameWnd.h"
#include "GRControlBar.h"

#include <Misc/Misc.h>


std::list<GRControlBar*>       GRDockingManager::m_ControlBars;

GRMiniDockFrameWnd*            GRDockingManager::m_pDockBars[4] = { 0 };

bool                            GRDockingManager::m_LoadingState = false;


void GRDockingManager::EnableDocking( GR::u32 DockStyles )
{
  CFrameWnd*    pTopFrame = AfxGetMainWnd()->GetTopLevelFrame();

  RECT    rcMDIFrame;

  pTopFrame->GetClientRect( &rcMDIFrame );

  if ( DockStyles & CBRS_ALIGN_LEFT )
  {
    if ( m_pDockBars[2] == NULL )
    {
      m_pDockBars[2] = new GRMiniDockFrameWnd();
      m_pDockBars[2]->Create( pTopFrame );
      m_pDockBars[2]->m_dfPos = GRMiniDockFrameWnd::DFP_LEFT;
      m_pDockBars[2]->SetParent( pTopFrame );
      m_pDockBars[2]->SetWindowText( _T( "GRLeftDockBar" ) );
      m_pDockBars[2]->ModifyStyle( WS_SYSMENU | WS_POPUP | WS_CAPTION, WS_CHILD );
      m_pDockBars[2]->ModifyStyleEx( WS_EX_TOOLWINDOW, 0 );

      m_pDockBars[2]->SetWindowPos( NULL, rcMDIFrame.left, rcMDIFrame.top, 0, rcMDIFrame.bottom - rcMDIFrame.top, SWP_NOZORDER );
      m_pDockBars[2]->ShowWindow( SW_SHOW );
    }
  }
  else
  {
    if ( m_pDockBars[2] )
    {
      m_pDockBars[2]->DestroyWindow();
      m_pDockBars[2] = NULL;
    }
  }
  if ( DockStyles & CBRS_ALIGN_RIGHT )
  {
    if ( m_pDockBars[3] == NULL )
    {
      m_pDockBars[3] = new GRMiniDockFrameWnd();
      m_pDockBars[3]->Create( pTopFrame );
      m_pDockBars[3]->m_dfPos = GRMiniDockFrameWnd::DFP_RIGHT;
      m_pDockBars[3]->SetParent( pTopFrame );
      m_pDockBars[3]->SetWindowText( _T( "GRRightDockBar" ) );
      m_pDockBars[3]->ModifyStyle( WS_SYSMENU | WS_POPUP | WS_CAPTION, WS_CHILD );
      m_pDockBars[3]->ModifyStyleEx( WS_EX_TOOLWINDOW, 0 );

      m_pDockBars[3]->SetWindowPos( NULL, rcMDIFrame.right, rcMDIFrame.top, 0, rcMDIFrame.bottom - rcMDIFrame.top, SWP_NOZORDER );
      m_pDockBars[3]->ShowWindow( SW_SHOW );
    }
  }
  else
  {
    if ( m_pDockBars[3] )
    {
      m_pDockBars[3]->DestroyWindow();
      m_pDockBars[3] = NULL;
    }
  }
  if ( DockStyles & CBRS_ALIGN_TOP )
  {
    if ( m_pDockBars[0] == NULL )
    {
      m_pDockBars[0] = new GRMiniDockFrameWnd();
      m_pDockBars[0]->Create( pTopFrame );
      m_pDockBars[0]->m_dfPos = GRMiniDockFrameWnd::DFP_TOP;
      m_pDockBars[0]->SetParent( pTopFrame );
      m_pDockBars[0]->SetWindowText( _T( "GRUpperDockBar" ) );
      m_pDockBars[0]->ModifyStyle( WS_SYSMENU | WS_POPUP | WS_CAPTION, WS_CHILD );
      m_pDockBars[0]->ModifyStyleEx( WS_EX_TOOLWINDOW, 0 );

      m_pDockBars[0]->SetWindowPos( NULL, rcMDIFrame.left, rcMDIFrame.top, rcMDIFrame.right - rcMDIFrame.left, 0, SWP_NOZORDER );
      m_pDockBars[0]->ShowWindow( SW_SHOW );
    }
  }
  else
  {
    if ( m_pDockBars[0] )
    {
      m_pDockBars[0]->DestroyWindow();
      m_pDockBars[0] = NULL;
    }
  }
  if ( DockStyles & CBRS_ALIGN_BOTTOM )
  {
    if ( m_pDockBars[1] == NULL )
    {
      m_pDockBars[1] = new GRMiniDockFrameWnd();
      m_pDockBars[1]->Create( pTopFrame );
      m_pDockBars[1]->m_dfPos = GRMiniDockFrameWnd::DFP_BOTTOM;
      m_pDockBars[1]->SetParent( pTopFrame );
      m_pDockBars[1]->SetWindowText( _T( "GRLowerDockBar" ) );
      m_pDockBars[1]->ModifyStyle( WS_SYSMENU | WS_POPUP | WS_CAPTION, WS_CHILD );
      m_pDockBars[1]->ModifyStyleEx( WS_EX_TOOLWINDOW, 0 );

      m_pDockBars[1]->SetWindowPos( NULL, rcMDIFrame.left, rcMDIFrame.bottom, rcMDIFrame.right - rcMDIFrame.left, 0, SWP_NOZORDER );
      m_pDockBars[1]->ShowWindow( SW_SHOW );
    }
  }
  else
  {
    if ( m_pDockBars[1] )
    {
      m_pDockBars[1]->DestroyWindow();
      m_pDockBars[1] = NULL;
    }
  }
}



void GRDockingManager::AddControlBar( GRControlBar* pBar )
{
  m_ControlBars.push_back( pBar );
}



void GRDockingManager::RemoveControlBar( GRControlBar* pBar )
{
  m_ControlBars.remove( pBar );
}



DWORD GRDockingManager::CheckForDockingPosition( GRControlBar* pBar, POINT& Pos )
{
  GR::u32   dockFlags = 0;

  CSize     szBarH = pBar->CalcSize( GRControlBar::GRCBS_DOCKED | GRControlBar::GRCBS_DOCKED_TOP );
  CSize     szBarV = pBar->CalcSize( GRControlBar::GRCBS_DOCKED | GRControlBar::GRCBS_DOCKED_LEFT );

  POINT     ptOrigPos = Pos;

  for ( int i = 0; i < 4; ++i )
  {
    if ( m_pDockBars[i] )
    {
      RECT    rc;

      m_pDockBars[i]->GetClientRect( &rc );

      m_pDockBars[i]->ClientToScreen( &rc );
      //AfxGetMainWnd()->GetTopLevelFrame()->ScreenToClient( &rc );

      // an den MDIClient-Rahmen andocken?
      if ( ( Pos.y >= rc.top )
      &&   ( Pos.y < rc.bottom ) )
      {
        if ( i == 2 )
        {
          if ( abs( (int)Pos.x - (int)rc.left ) < 20 )
          {
            Pos.x = rc.left;
            dockFlags |= CBRS_ALIGN_LEFT;
          }
          // prüfen an Unterkante von vorhandenen Bars
          std::list<GRControlBar*>::iterator   it( m_pDockBars[i]->m_listControlBars.begin() );
          while ( it != m_pDockBars[i]->m_listControlBars.end() )
          {
            GRControlBar*    pChildBar = *it;

            if ( pChildBar != pBar )
            {
              RECT    rcChildBar;
              pChildBar->GetClientRect( &rcChildBar );
              pChildBar->ClientToScreen( &rcChildBar );
              if ( abs( (int)Pos.x - (int)rcChildBar.right ) < 20 )
              {
                Pos.x = rcChildBar.right;
                dockFlags |= CBRS_ALIGN_LEFT;
              }
            }

            ++it;
          }
        }
        else if ( i == 3 )
        {
          if ( abs( (int)Pos.x - (int)rc.right ) < 20 )
          {
            Pos.x = rc.right - szBarV.cx;
            dockFlags |= CBRS_ALIGN_RIGHT;
          }
          else
          {
            // prüfen an Oberkante von vorhandenen Bars
            std::list<GRControlBar*>::iterator   it( m_pDockBars[i]->m_listControlBars.begin() );
            while ( it != m_pDockBars[i]->m_listControlBars.end() )
            {
              GRControlBar*    pChildBar = *it;

              if ( pChildBar != pBar )
              {
                RECT    rcChildBar;
                pChildBar->GetClientRect( &rcChildBar );
                pChildBar->ClientToScreen( &rcChildBar );
                if ( abs( (int)Pos.x - (int)rcChildBar.left ) < 20 )
                {
                  Pos.x = rcChildBar.left - szBarV.cx;
                  dockFlags |= CBRS_ALIGN_RIGHT;
                }
              }
              ++it;
            }
          }
        }
        /*
        else if ( ( abs( (int)ptPos.x - (int)rc.right ) < 40 )
        &&        ( i == 3 ) )
        {
          ptPos.x = rc.left;
          dwDockFlags |= CBRS_ALIGN_RIGHT;
        }
        */
      }
      if ( ( Pos.x >= rc.left )
      &&   ( Pos.x < rc.right ) )
      {
        if ( i == 0 )
        {
          if ( abs( (int)Pos.y - (int)rc.top ) < 20 )
          {
            Pos.y = rc.top;
            dockFlags |= CBRS_ALIGN_TOP;
          }
          // prüfen an Unterkante von vorhandenen Bars
          std::list<GRControlBar*>::iterator   it( m_pDockBars[i]->m_listControlBars.begin() );
          while ( it != m_pDockBars[i]->m_listControlBars.end() )
          {
            GRControlBar*    pChildBar = *it;

            if ( pChildBar != pBar )
            {
              RECT    rcChildBar;
              pChildBar->GetClientRect( &rcChildBar );
              pChildBar->ClientToScreen( &rcChildBar );
              if ( abs( (int)Pos.y - (int)rcChildBar.bottom ) < 20 )
              {
                Pos.y = rcChildBar.bottom;
                dockFlags |= CBRS_ALIGN_TOP;
              }
            }

            ++it;
          }
        }
        else if ( i == 1 )
        {
          if ( abs( (int)Pos.y - (int)rc.bottom ) < 20 )
          {
            Pos.y = rc.bottom - szBarH.cy;
            dockFlags |= CBRS_ALIGN_BOTTOM;
          }
          else
          {
            // prüfen an Oberkante von vorhandenen Bars
            std::list<GRControlBar*>::iterator   it( m_pDockBars[i]->m_listControlBars.begin() );
            while ( it != m_pDockBars[i]->m_listControlBars.end() )
            {
              GRControlBar*    pChildBar = *it;

              if ( pChildBar != pBar )
              {
                RECT    rcChildBar;
                pChildBar->GetClientRect( &rcChildBar );
                pChildBar->ClientToScreen( &rcChildBar );
                if ( abs( (int)Pos.y - (int)rcChildBar.top ) < 20 )
                {
                  Pos.y = rcChildBar.top - szBarH.cy;
                  dockFlags |= CBRS_ALIGN_BOTTOM;
                }
              }
              ++it;
            }
          }
        }
      }

    }
  }

  if ( ( pBar->m_CBStyle & GRControlBar::GRCBS_DOCKED_BOTTOM )
  &&   ( dockFlags & CBRS_ALIGN_BOTTOM ) )
  {
    // falls der Bar schon angedockt ist, wird dieser Dockframe bevorzugt
    dockFlags &= CBRS_ALIGN_BOTTOM;
    Pos.x = ptOrigPos.x;
  }
  if ( ( pBar->m_CBStyle & GRControlBar::GRCBS_DOCKED_TOP )
  &&   ( dockFlags & CBRS_ALIGN_TOP ) )
  {
    // falls der Bar schon angedockt ist, wird dieser Dockframe bevorzugt
    dockFlags &= CBRS_ALIGN_TOP;
    Pos.x = ptOrigPos.x;
  }
  if ( ( pBar->m_CBStyle & GRControlBar::GRCBS_DOCKED_LEFT )
  &&   ( dockFlags & CBRS_ALIGN_LEFT ) )
  {
    // falls der Bar schon angedockt ist, wird dieser Dockframe bevorzugt
    dockFlags &= CBRS_ALIGN_LEFT;
    Pos.y = ptOrigPos.y;
  }
  if ( ( pBar->m_CBStyle & GRControlBar::GRCBS_DOCKED_RIGHT )
  &&   ( dockFlags & CBRS_ALIGN_RIGHT ) )
  {
    // falls der Bar schon angedockt ist, wird dieser Dockframe bevorzugt
    dockFlags &= CBRS_ALIGN_RIGHT;
    Pos.y = ptOrigPos.y;
  }

  return dockFlags;
}



void GRDockingManager::DockControlBar( GRControlBar* pBar, GR::u32 DockPos, POINT& BarPos )
{
  CFrameWnd*    pTopFrame = AfxGetMainWnd()->GetTopLevelFrame();

  GRMiniDockFrameWnd*    pDockFrame = NULL;
  if ( DockPos & CBRS_ALIGN_LEFT )
  {
    pDockFrame = m_pDockBars[2];
  }
  else if ( DockPos & CBRS_ALIGN_RIGHT )
  {
    pDockFrame = m_pDockBars[3];
  }
  else if ( DockPos & CBRS_ALIGN_TOP )
  {
    pDockFrame = m_pDockBars[0];
  }
  else if ( DockPos & CBRS_ALIGN_BOTTOM )
  {
    pDockFrame = m_pDockBars[1];
  }
  if ( pDockFrame == NULL )
  {
    TRACE( "KREISCH _ FEHLER!!!\n" );
    return;
  }

  if ( pBar->GetParent() != pDockFrame )
  {
    if ( !pBar->IsFloating() )
    {
      // der Bar steckt in einem anderen DockFrame
      GRMiniDockFrameWnd*    pOldDockFrame = (GRMiniDockFrameWnd*)pBar->GetParent();

      pOldDockFrame->RemoveControlBar( pBar );
    }
    pBar->SetParent( pDockFrame );
    
    DWORD   dwSizeFlags = GRControlBar::GRCBS_APPLY_SIZE | GRControlBar::GRCBS_DOCKED;

    if ( DockPos & CBRS_ALIGN_LEFT )
    {
      dwSizeFlags |= GRControlBar::GRCBS_DOCKED_LEFT;
      pBar->m_CBStyle |= GRControlBar::GRCBS_DOCKED_LEFT;
    }
    if ( DockPos & CBRS_ALIGN_RIGHT )
    {
      dwSizeFlags |= GRControlBar::GRCBS_DOCKED_RIGHT;
      pBar->m_CBStyle |= GRControlBar::GRCBS_DOCKED_RIGHT;
    }
    if ( DockPos & CBRS_ALIGN_TOP )
    {
      dwSizeFlags |= GRControlBar::GRCBS_DOCKED_TOP;
      pBar->m_CBStyle |= GRControlBar::GRCBS_DOCKED_TOP;
    }
    if ( DockPos & CBRS_ALIGN_BOTTOM )
    {
      dwSizeFlags |= GRControlBar::GRCBS_DOCKED_BOTTOM;
      pBar->m_CBStyle |= GRControlBar::GRCBS_DOCKED_BOTTOM;
    }
    if ( dwSizeFlags )
    {
      dwSizeFlags |= GRControlBar::GRCBS_DOCKED;
      pBar->m_CBStyle |= GRControlBar::GRCBS_DOCKED;
    }
    pBar->m_Floating = false;

    pBar->CalcSize( dwSizeFlags );

    pDockFrame->InsertControlBar( pBar );
  }

  if ( !m_LoadingState )
  {
    pDockFrame->RepositionControlBars( pBar, BarPos, DockPos );
  }

  pBar->m_Floating = false;

  // DockBar-Größe ändern
  pBar->m_pFrameWnd->SetWindowPos( NULL, -32000, -32000, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
}



void GRDockingManager::ShowControlBar( GRControlBar* pBar, bool Show )
{
  if ( pBar == NULL )
  {
    return;
  }
  if ( Show == !!pBar->IsWindowVisible() )
  {
    return;
  }

  if ( !Show )
  {
    // einfach
    pBar->m_CBStyle |= GRControlBar::GRCBS_HIDDEN;
    if ( pBar->IsFloating() )
    {
      pBar->m_pFrameWnd->ShowWindow( SW_HIDE );
      return;
    }
    // der Bar muß "entdockt" werden
    pBar->ShowWindow( SW_HIDE );
  }
  else
  {
    // einfach
    pBar->m_CBStyle &= ~GRControlBar::GRCBS_HIDDEN;
    if ( pBar->IsFloating() )
    {
      pBar->m_pFrameWnd->ShowWindow( SW_SHOW );
      return;
    }
    // der Bar muß "entdockt" werden
    pBar->ShowWindow( SW_SHOW );
  }

  RECT    rcBar;

  pBar->GetClientRect( &rcBar );
  pBar->ClientToScreen( &rcBar );
  POINT   ptBar;

  ptBar.x = rcBar.left;
  ptBar.y = rcBar.top;
  ((GRMiniDockFrameWnd*)pBar->GetParentFrame())->RepositionControlBars( pBar, ptBar, pBar->m_MRUDockPos );
  ((GRMiniDockFrameWnd*)pBar->GetParentFrame())->ScreenToClient( &ptBar );
  pBar->SetWindowPos( NULL, ptBar.x, ptBar.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

  AfxGetMainWnd()->GetTopLevelFrame()->RecalcLayout();
}



void GRDockingManager::SaveBarState()
{
  CWinApp* pApp = AfxGetApp();

  int   iCount = (int)m_ControlBars.size();

  pApp->WriteProfileInt( _T( "BarStates" ), _T( "BarCount" ), iCount );

  int   iBarIndex = 0;

  GR::tChar szTemp[MAX_PATH];

  std::list<GRControlBar*>::iterator   it( m_ControlBars.begin() );
  while ( it != m_ControlBars.end() )
  {
    GRControlBar*   pBar = *it;

    wsprintf( szTemp, _T( "Bar%dID" ), iBarIndex );
    pApp->WriteProfileInt( _T( "BarStates" ), szTemp, pBar->GetDlgCtrlID() );
    wsprintf( szTemp, _T( "Bar%dStyle" ), iBarIndex );
    pApp->WriteProfileInt( _T( "BarStates" ), szTemp, pBar->m_CBStyle );
    wsprintf( szTemp, _T( "Bar%dDocked" ), iBarIndex );
    pApp->WriteProfileInt( _T( "BarStates" ), szTemp, pBar->IsFloating() ? 0 : 1 );
    wsprintf( szTemp, _T( "Bar%dMRUDockPosX" ), iBarIndex );
    pApp->WriteProfileInt( _T( "BarStates" ), szTemp, pBar->m_MRUDockedPos.x );
    wsprintf( szTemp, _T( "Bar%dMRUDockPosY" ), iBarIndex );
    pApp->WriteProfileInt( _T( "BarStates" ), szTemp, pBar->m_MRUDockedPos.y );
    wsprintf( szTemp, _T( "Bar%dMRUFloatPosX" ), iBarIndex );
    pApp->WriteProfileInt( _T( "BarStates" ), szTemp, pBar->m_MRUFloatingPos.x );
    wsprintf( szTemp, _T( "Bar%dMRUFloatPosY" ), iBarIndex );
    pApp->WriteProfileInt( _T( "BarStates" ), szTemp, pBar->m_MRUFloatingPos.y );
    wsprintf( szTemp, _T( "Bar%dMRUDockPos" ), iBarIndex );
    pApp->WriteProfileInt( _T( "BarStates" ), szTemp, pBar->m_MRUDockPos );

    WINDOWPLACEMENT   wpl;

    wpl.length = sizeof( wpl );

    if ( pBar->IsFloating() )
    {
      pBar->m_pFrameWnd->GetWindowPlacement( &wpl );
    }
    else
    {
      pBar->GetWindowPlacement( &wpl );
    }

    wsprintf( szTemp, _T( "Bar%dPlacement" ), iBarIndex );
    pApp->WriteProfileBinary( _T( "BarStates" ), szTemp, (BYTE*)&wpl, sizeof( wpl ) );

    ++iBarIndex;
    ++it;
  }
}



void GRDockingManager::LoadBarState()
{
  CWinApp* pApp = AfxGetApp();

  int   iCount = pApp->GetProfileInt( _T( "BarStates" ), _T( "BarCount" ), 0 );

  GR::tChar  szTemp[MAX_PATH];

  m_LoadingState = true;
  for ( int iBarIndex = 0; iBarIndex < iCount; ++iBarIndex )
  {
    wsprintf( szTemp, _T( "Bar%dID" ), iBarIndex );
    int   iBarID = pApp->GetProfileInt( _T( "BarStates" ), szTemp, 0 );

    if ( iBarID == 0 )
    {
      continue;
    }

    GRControlBar*    pThisBar = NULL;

    std::list<GRControlBar*>::iterator   it( m_ControlBars.begin() );
    while ( it != m_ControlBars.end() )
    {
      GRControlBar*    pBar = *it;

      if ( pBar->GetDlgCtrlID() == iBarID )
      {
        pThisBar = pBar;
        break;
      }
      ++it;
    }
    if ( pThisBar == NULL )
    {
      continue;
    }

    pThisBar->SetWindowPos( NULL, -32000, -32000, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW );

    WINDOWPLACEMENT* pWPL = NULL;

    UINT   dwDataSize = 0;

    wsprintf( szTemp, _T( "Bar%dPlacement" ), iBarIndex );
    pApp->GetProfileBinary( _T( "BarStates" ), szTemp, (LPBYTE*)&pWPL, &dwDataSize );
    if ( dwDataSize != sizeof( WINDOWPLACEMENT ) )
    {
      delete[] pWPL;
      pWPL = NULL;
      continue;
    }

    wsprintf( szTemp, _T( "Bar%dStyle" ), iBarIndex );
    DWORD   dwBarStyles = pApp->GetProfileInt( _T( "BarStates" ), szTemp, GRControlBar::GRCBS_FIXED_SIZE );

    
    wsprintf( szTemp, _T( "Bar%dMRUDockPosX" ), iBarIndex );
    pThisBar->m_MRUDockedPos.x = pApp->GetProfileInt( _T( "BarStates" ), szTemp, 0 );
    wsprintf( szTemp, _T( "Bar%dMRUDockPosY" ), iBarIndex );
    pThisBar->m_MRUDockedPos.y = pApp->GetProfileInt( _T( "BarStates" ), szTemp, 0 );
    wsprintf( szTemp, _T( "Bar%dMRUFloatPosX" ), iBarIndex );
    pThisBar->m_MRUFloatingPos.x = pApp->GetProfileInt( _T( "BarStates" ), szTemp, 0 );
    wsprintf( szTemp, _T( "Bar%dMRUFloatPosY" ), iBarIndex );
    pThisBar->m_MRUFloatingPos.y = pApp->GetProfileInt( _T( "BarStates" ), szTemp, 0 );
    wsprintf( szTemp, _T( "Bar%dMRUDockPos" ), iBarIndex );
    pThisBar->m_MRUDockPos = pApp->GetProfileInt( _T( "BarStates" ), szTemp, 0 );

    if ( dwBarStyles & GRControlBar::GRCBS_DOCKED )
    {
      DWORD   dwDockPos = 0;

      GRMiniDockFrameWnd*    pDockFrame = NULL;

      if ( dwBarStyles & GRControlBar::GRCBS_DOCKED_LEFT )
      {
        dwDockPos |= CBRS_ALIGN_LEFT;
        pDockFrame = m_pDockBars[2];
      }
      if ( dwBarStyles & GRControlBar::GRCBS_DOCKED_RIGHT )
      {
        dwDockPos |= CBRS_ALIGN_RIGHT;
        pDockFrame = m_pDockBars[3];
      }
      if ( dwBarStyles & GRControlBar::GRCBS_DOCKED_TOP )
      {
        dwDockPos |= CBRS_ALIGN_TOP;
        pDockFrame = m_pDockBars[0];
      }
      if ( dwBarStyles & GRControlBar::GRCBS_DOCKED_BOTTOM )
      {
        dwDockPos |= CBRS_ALIGN_BOTTOM;
        pDockFrame = m_pDockBars[1];
      }

      if ( pDockFrame )
      {
        pDockFrame->ClientToScreen( &pWPL->rcNormalPosition );
        pThisBar->Dock( dwDockPos, pWPL->rcNormalPosition.left, pWPL->rcNormalPosition.top );
      }
    }
    else
    {
      pThisBar->m_Floating = false;

	  if ( ( pWPL->rcNormalPosition.left == 32767 )
	  &&   ( pWPL->rcNormalPosition.top == 32767 ) )
	  {
      // sanity check
      pWPL->rcNormalPosition.left = 0;
      pWPL->rcNormalPosition.top = 0;
	  }
      pThisBar->Float( pWPL->rcNormalPosition.left, pWPL->rcNormalPosition.top );
    }

    if ( dwBarStyles & GRControlBar::GRCBS_HIDDEN )
    {
      ShowControlBar( pThisBar, FALSE );
    }

    pThisBar->m_CBStyle = dwBarStyles;

    delete[] pWPL;
  }
  m_LoadingState = false;
}