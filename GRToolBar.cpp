// GRToolBar.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "GRToolBar.h"


// GRToolBar

//IMPLEMENT_DYNAMIC(GRToolBar, GRControlBar)


IMPLEMENT_DYNCREATE( GRToolBar, GRControlBar )

GRToolBar::GRToolBar()
{
  m_Horizontal        = true;
  m_HighlightedButton = -1;
  m_PushedButton      = -1;
  m_TimerID           = 0;

  m_hImageList            = ImageList_Create( 16, 16, ILC_COLOR24 | ILC_MASK, 0, 4 );
}



GRToolBar::~GRToolBar()
{
  if ( m_hImageList )
  {
    ImageList_Destroy( m_hImageList );
    m_hImageList = NULL;
  }
}



BEGIN_MESSAGE_MAP(GRToolBar, GRControlBar)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_DESTROY()
  ON_WM_TIMER()
//  ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



BOOL GRToolBar::CreateEx( CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, CRect rcBorders, UINT nID, const GR::tChar* szTitle )
{
  return GRControlBar::Create( szTitle, dwCtrlStyle, pParentWnd, nID );
}



class GRToolBarCmdUI : public CCmdUI        // class private to this file !
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
};
#include <debug/debugclient.h>
void GRToolBarCmdUI::Enable(BOOL bOn)
{
  GRToolBar*   pBar = (GRToolBar*)m_pOther;

  m_bEnableChanged = TRUE;

  pBar->EnableControl( m_nIndex, bOn );
}

void GRToolBarCmdUI::SetCheck(int nCheck)
{
  GRToolBar*   pBar = (GRToolBar*)m_pOther;

  pBar->SetCheck( m_nIndex, nCheck ? TRUE : FALSE );

}

void GRToolBarCmdUI::SetText(LPCTSTR)
{
	// ignore it
}


void GRToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{

	GRToolBarCmdUI state;


	state.m_pOther = this;

	state.m_nIndexMax = (UINT)m_Buttons.size();
	for ( state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++ )
	{
		// get buttons state
    state.m_nID = m_Buttons[state.m_nIndex].dwCommandID;

		// ignore separators
    if ( state.m_nID )
		{
			// allow reflections
			if (CWnd::OnCmdMsg(0, 
				MAKELONG(CN_UPDATE_COMMAND_UI&0xff, WM_COMMAND+WM_REFLECT_BASE), 
				&state, NULL))
				continue;

			// allow the toolbar itself to have update handlers
			if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
				continue;

			// allow the owner to process the update
			state.DoUpdate(pTarget, bDisableIfNoHndler);
		}
	}

  UpdateDialogControls( pTarget, bDisableIfNoHndler );
}



BOOL GRToolBar::LoadToolBar( DWORD dwResourceID )
{
  return LoadToolBar( MAKEINTRESOURCE( dwResourceID ) );
}



struct CToolBarData
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	//WORD aItems[wItemCount]

	WORD* items()
		{ return (WORD*)(this+1); }
};


BOOL GRToolBar::LoadToolBar(LPCTSTR lpszResourceName)
{
	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);

	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;
	ASSERT(pData->wVersion == 1);

	UINT* pItems = new UINT[pData->wItemCount];
	for (int i = 0; i < pData->wItemCount; i++)
  {
		pItems[i] = pData->items()[i];

    AddButton( pItems[i] );
  }
	BOOL bResult = TRUE;//SetButtons(pItems, pData->wItemCount);
	delete[] pItems;

	if (bResult)
	{
		// load bitmap now that sizes are known by the toolbar control
		bResult = LoadBitmap(lpszResourceName);
	}

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

  CalcSize();

	return bResult;
}



BOOL GRToolBar::LoadBitmap( LPCTSTR lpszResourceName )
{
	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);

	// determine location of the bitmap in resource fork
	HINSTANCE hInstImageWell = AfxFindResourceHandle(lpszResourceName, RT_BITMAP);
	HRSRC hRsrcImageWell = ::FindResource(hInstImageWell, lpszResourceName, RT_BITMAP);
	if ( hRsrcImageWell == NULL )
  {
		return FALSE;
  }

	// load the bitmap
  if ( m_hImageList )
  {
    ImageList_Destroy( m_hImageList );
    m_hImageList = NULL;
  }

  m_hImageList = ImageList_LoadImage( hInstImageWell, lpszResourceName, 16, 2, GetSysColor( COLOR_BTNFACE ), 
                                      IMAGE_BITMAP, 
                                      LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS );

	return TRUE;
}



BOOL GRToolBar::AddButton( DWORD dwCommandID )
{
  tButtonInfo   BtnInfo;

  BtnInfo.dwCommandID = dwCommandID;
  SetRect( &BtnInfo.rcButton, 0, 0, 23, 22 );

  if ( dwCommandID == 0 )
  {
    // ein Separator
    BtnInfo.rcButton.right = 4;
  }

  m_Buttons.push_back( BtnInfo );

  return TRUE;
}



CSize GRToolBar::EstimateSize( GR::u32 dwSizingFlags )
{
  bool      estimateHorizontal = true;

  if ( ( dwSizingFlags & GRCBS_DOCKED_LEFT )
  ||   ( dwSizingFlags & GRCBS_DOCKED_RIGHT ) )
  {
    estimateHorizontal = false;
  }

  CSize     szCB( 0, 0 );

  int       iX = 0,
            iY = 0;

  if ( !IsFloating() )
  {
    // angedockt kommt ein Gripper dran
    if ( estimateHorizontal )
    {
      szCB.cx += 4;
      iX += 4;
    }
    else
    {
      szCB.cy += 4;
      iY += 4;
    }
  }

  tVectButtons    vectTemp = m_Buttons;


  for ( size_t i = 0; i < vectTemp.size(); ++i )
  {
    tButtonInfo&    BtnInfo = vectTemp[i];

    BtnInfo.rcButton.left   = iX;
    BtnInfo.rcButton.top    = iY;
    if ( !estimateHorizontal )
    {
      BtnInfo.rcButton.right  = BtnInfo.rcButton.left + 23;
      if ( BtnInfo.dwCommandID == 0 )
      {
        // ein Separator
        szCB.cy += 4;

        BtnInfo.rcButton.bottom = BtnInfo.rcButton.top + 4;
      }
      else
      {
        BtnInfo.rcButton.bottom = BtnInfo.rcButton.top + 22;

        szCB.cy += 22;
        if ( 22 > szCB.cx )
        {
          szCB.cx += 22;
        }
      }
      iY += BtnInfo.rcButton.bottom - BtnInfo.rcButton.top;
    }
    else
    {
      BtnInfo.rcButton.bottom = BtnInfo.rcButton.top + 22;
      if ( BtnInfo.dwCommandID == 0 )
      {
        // ein Separator
        szCB.cx += 4;

        BtnInfo.rcButton.right = BtnInfo.rcButton.left + 4;
      }
      else
      {
        BtnInfo.rcButton.right = BtnInfo.rcButton.left + 23;
        szCB.cx += 23;
        if ( 22 > szCB.cy )
        {
          szCB.cy += 22;
        }
      }
      iX += BtnInfo.rcButton.right - BtnInfo.rcButton.left;
    }
  }

  if ( !IsFloating() )
  {
    // angedockt kommt ein Gripper dran
    if ( !estimateHorizontal )
    {
      szCB.cx++;
    }
  }

  if ( dwSizingFlags & GRCBS_APPLY_SIZE )
  {
    m_Horizontal  = estimateHorizontal;
    m_Buttons     = vectTemp;
  }

  return szCB;
}



void GRToolBar::OnPaint()
{
  CPaintDC dc(this);

  RECT    rc;

  GetClientRect( &rc );

  int   iButtonIndex = 0,
        iX = 0,
        iY = 0;

  if ( !IsFloating() )
  {
    // Gripper zeichnen
    if ( m_Horizontal )
    {
      dc.Draw3dRect( 0, 0, 2, 22, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
      dc.Draw3dRect( 2, 0, 2, 22, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );

      iX += 4;
    }
    else
    {
      dc.Draw3dRect( 0, 0, 23, 2, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
      dc.Draw3dRect( 0, 2, 23, 2, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );

      iY += 4;
    }
  }

  for ( size_t i = 0; i < m_Buttons.size(); ++i )
  {
    tButtonInfo&    BtnInfo = m_Buttons[i];

    int   iCX = ( BtnInfo.rcButton.right - BtnInfo.rcButton.left - 16 ) / 2;
    int   iCY = ( BtnInfo.rcButton.bottom - BtnInfo.rcButton.top - 15 ) / 2;

    if ( ( BtnInfo.dwButtonStyle & tButtonInfo::BTN_PUSHED )
    ||   ( BtnInfo.dwButtonStyle & tButtonInfo::BTN_CHECKED ) )
    {
      iCX++;
      iCY++;
    }

    if ( m_Horizontal )
    {
      if ( BtnInfo.dwCommandID )
      {
        if ( BtnInfo.dwButtonStyle & tButtonInfo::BTN_DISABLED )
        {
          HICON hIcon = ImageList_ExtractIcon( AfxGetInstanceHandle(), m_hImageList, iButtonIndex );
          DrawState( dc.GetSafeHdc(),
                      0, 0,
                      (LPARAM)hIcon,
                      0,
                      iX + iCX, iCY,
                      16,
                      15,
                      DST_ICON | DSS_DISABLED );
          DestroyIcon( hIcon );
        }
        else
        {
          ImageList_DrawEx( m_hImageList, iButtonIndex, dc.GetSafeHdc(), iX + iCX, iCY, 0, 0, CLR_NONE, CLR_NONE, ILD_NORMAL );
        }
        iButtonIndex++;
      }
      else
      {
        // Separator
        dc.Draw3dRect( iX + 1, 0, 2, BtnInfo.rcButton.bottom - BtnInfo.rcButton.top, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
      }

      iX += BtnInfo.rcButton.right - BtnInfo.rcButton.left;
    }
    else
    {
      if ( BtnInfo.dwCommandID )
      {
        if ( BtnInfo.dwButtonStyle & tButtonInfo::BTN_DISABLED )
        {
          HICON hIcon = ImageList_ExtractIcon( AfxGetInstanceHandle(), m_hImageList, iButtonIndex );
          DrawState( dc.GetSafeHdc(),
                      0, 0,
                      (LPARAM)hIcon,
                      0,
                      iCX, iY + iCY,
                      16,
                      15,
                      DST_ICON | DSS_DISABLED );
          DestroyIcon( hIcon );
        }
        else
        {
          ImageList_DrawEx( m_hImageList, iButtonIndex, dc.GetSafeHdc(), iCX, iY + iCY, 0, 0, CLR_NONE, CLR_NONE, ILD_NORMAL );
        }
        iButtonIndex++;
      }
      else
      {
        // Separator
        dc.Draw3dRect( 0, iY + 1, BtnInfo.rcButton.right - BtnInfo.rcButton.left, 2, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
      }

      iY += BtnInfo.rcButton.bottom - BtnInfo.rcButton.top;
    }

    if ( BtnInfo.dwCommandID )
    {
      if ( ( BtnInfo.dwButtonStyle & tButtonInfo::BTN_PUSHED )
      ||   ( BtnInfo.dwButtonStyle & tButtonInfo::BTN_CHECKED ) )
      {
        RECT    rc = BtnInfo.rcButton;

        dc.Draw3dRect( &rc, GetSysColor( COLOR_3DSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
      }
      else if ( BtnInfo.dwButtonStyle & tButtonInfo::BTN_HILIGHTED )
      {
        RECT    rc = BtnInfo.rcButton;

        dc.Draw3dRect( &rc, GetSysColor( COLOR_3DHIGHLIGHT ), GetSysColor( COLOR_3DSHADOW ) );
      }
    }
  }

}



void GRToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
  if ( !IsFloating() )
  {
    RECT    rcGripper;

    SetRectEmpty( &rcGripper );

    if ( m_Horizontal )
    {
      rcGripper.right   = 4;
      rcGripper.bottom  = 22;
    }
    else
    {
      rcGripper.right   = 23;
      rcGripper.bottom  = 4;
    }
    if ( PtInRect( &rcGripper, point ) )
    {
      m_pFrameWnd->StartDragging( point );
      return;
    }
  }

  size_t   iOverButton = -1;

  for ( size_t i = 0; i < m_Buttons.size(); ++i )
  {
    tButtonInfo&    BtnInfo = m_Buttons[i];

    if ( ( BtnInfo.dwCommandID )
    &&   ( !( BtnInfo.dwButtonStyle & tButtonInfo::BTN_DISABLED ) ) )
    {
      if ( PtInRect( &BtnInfo.rcButton, point ) )
      {
        iOverButton = i;
        break;
      }
    }
  }

  if ( iOverButton != m_HighlightedButton )
  {
    if ( m_HighlightedButton != -1 )
    {
      InvalidateRect( &m_Buttons[m_HighlightedButton].rcButton );
      m_Buttons[m_HighlightedButton].dwButtonStyle &= ~tButtonInfo::BTN_HILIGHTED;
      m_HighlightedButton = -1;
    }
    m_HighlightedButton  = iOverButton;
  }
  if ( m_PushedButton != iOverButton )
  {
    m_PushedButton       = iOverButton;
    if ( iOverButton != -1 )
    {
      m_Buttons[iOverButton].dwButtonStyle |= tButtonInfo::BTN_HILIGHTED | tButtonInfo::BTN_PUSHED;
      InvalidateRect( &m_Buttons[iOverButton].rcButton );
      SetCapture();
    }
  }
  GRControlBar::OnLButtonDown(nFlags, point);
}



void GRToolBar::OnLButtonUp( UINT nFlags, CPoint point )
{
  if ( GetCapture() == this )
  {
    if ( ( m_HighlightedButton == m_PushedButton )
    &&   ( m_PushedButton != -1 ) )
    {
      GetTopLevelFrame()->PostMessage( WM_COMMAND, MAKEWPARAM( m_Buttons[m_PushedButton].dwCommandID, 0 ), (LPARAM)GetSafeHwnd() );
    }
    ReleaseCapture();
  }
  if ( m_HighlightedButton != -1 )
  {
    InvalidateRect( &m_Buttons[m_HighlightedButton].rcButton );
    m_Buttons[m_HighlightedButton].dwButtonStyle &= ~tButtonInfo::BTN_HILIGHTED;
    m_HighlightedButton = -1;
  }
  if ( m_PushedButton != -1 )
  {
    InvalidateRect( &m_Buttons[m_PushedButton].rcButton );
    m_Buttons[m_PushedButton].dwButtonStyle &= ~tButtonInfo::BTN_PUSHED;
    m_PushedButton = -1;
  }

  GRControlBar::OnLButtonUp(nFlags, point);
}



void GRToolBar::OnMouseMove(UINT nFlags, CPoint point)
{

  size_t   iOverButton = -1;

  for ( size_t i = 0; i < m_Buttons.size(); ++i )
  {
    tButtonInfo&    BtnInfo = m_Buttons[i];

    if ( ( BtnInfo.dwCommandID )
    &&   ( !( BtnInfo.dwButtonStyle & tButtonInfo::BTN_DISABLED ) ) )
    {
      if ( PtInRect( &BtnInfo.rcButton, point ) )
      {
        iOverButton = i;
        break;
      }
    }
  }

  if ( iOverButton != m_HighlightedButton )
  {
    if ( m_HighlightedButton != -1 )
    {
      InvalidateRect( &m_Buttons[m_HighlightedButton].rcButton );
      m_Buttons[m_HighlightedButton].dwButtonStyle &= ~tButtonInfo::BTN_HILIGHTED;
      m_HighlightedButton = -1;
    }
    if ( m_PushedButton != -1 )
    {
      if ( m_Buttons[m_PushedButton].dwButtonStyle & tButtonInfo::BTN_PUSHED )
      {
        m_Buttons[m_PushedButton].dwButtonStyle &= ~tButtonInfo::BTN_PUSHED;
        InvalidateRect( &m_Buttons[m_PushedButton].rcButton );
      }
    }
    m_HighlightedButton  = iOverButton;
    if ( iOverButton != -1 )
    {
      if ( m_HighlightedButton == m_PushedButton )
      {
        m_Buttons[m_HighlightedButton].dwButtonStyle |= tButtonInfo::BTN_PUSHED;
      }
      if ( m_PushedButton == -1 )
      {
        m_Buttons[m_HighlightedButton].dwButtonStyle |= tButtonInfo::BTN_HILIGHTED;
      }
      InvalidateRect( &m_Buttons[iOverButton].rcButton );
      if ( m_TimerID == 0 )
      {
        m_TimerID = (DWORD)SetTimer( 7, 50, NULL );
      }
    }
  }

  if ( ( m_TimerID != 0 )
  &&   ( m_HighlightedButton == -1 ) )
  {
    KillTimer( m_TimerID );
    m_TimerID = 0;
  }

  /*
  if ( m_HighlightedButton != -1 )
  {
    AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
    if ( ( pModuleThreadState->m_pLastStatus != (CControlBar*)this )
    ||   ( pModuleThreadState->m_nLastStatus != m_Buttons[m_HighlightedButton].dwCommandID ) )
    {
  	  pModuleThreadState->m_pLastStatus = (CControlBar*)this;
	    pModuleThreadState->m_nLastStatus = m_Buttons[m_HighlightedButton].dwCommandID;
      GetOwner()->SendMessage(WM_SETMESSAGESTRING, m_Buttons[m_HighlightedButton].dwCommandID);
    }
  }
  else
  {
    AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
    if ( pModuleThreadState->m_pLastStatus == (CControlBar*)this )
    {
  	  pModuleThreadState->m_pLastStatus = NULL;
	    pModuleThreadState->m_nLastStatus = -1;
      GetOwner()->SendMessage( WM_POPMESSAGESTRING, AFX_IDS_IDLEMESSAGE );
    }
  }*/

  GRControlBar::OnMouseMove(nFlags, point);
}



void GRToolBar::OnDestroy()
{

  if ( m_TimerID != 0 )
  {
    KillTimer( m_TimerID );
    m_TimerID = 0;
  }

  GRControlBar::OnDestroy();

}



void GRToolBar::OnTimer(UINT nIDEvent)
{

  POINT   ptMouse;

  GetCursorPos( &ptMouse );

  ScreenToClient( &ptMouse );

  RECT    rc;

  GetClientRect( &rc );

  if ( !PtInRect( &rc, ptMouse ) )
  {
    if ( m_HighlightedButton != -1 )
    {
      InvalidateRect( &m_Buttons[m_HighlightedButton].rcButton );
      m_Buttons[m_HighlightedButton].dwButtonStyle &= ~tButtonInfo::BTN_HILIGHTED;
      m_HighlightedButton = -1;

      AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
      if ( pModuleThreadState->m_pLastStatus == (CControlBar*)this )
      {
  	    pModuleThreadState->m_pLastStatus = NULL;
	      pModuleThreadState->m_nLastStatus = -1;
        GetOwner()->SendMessage( WM_POPMESSAGESTRING, AFX_IDS_IDLEMESSAGE );
      }
    }
    KillTimer( m_TimerID );
    m_TimerID = 0;
  }

  GRControlBar::OnTimer(nIDEvent);
}

#include <debug/debugclient.h>
#include ".\grtoolbar.h"

void GRToolBar::EnableControl( UINT nIndex, BOOL bOn )
{

  if ( nIndex >= m_Buttons.size() )
  {
    return;
  }

  if ( ( ( bOn )
  &&     ( !( m_Buttons[nIndex].dwButtonStyle & tButtonInfo::BTN_DISABLED ) ) )
  ||   ( ( !bOn )
  &&     ( m_Buttons[nIndex].dwButtonStyle & tButtonInfo::BTN_DISABLED ) ) )
  {
    // nichts zu ändern
    return;
  }

  if ( bOn )
  {
    m_Buttons[nIndex].dwButtonStyle &= ~tButtonInfo::BTN_DISABLED;
  }
  else
  {
    m_Buttons[nIndex].dwButtonStyle |= tButtonInfo::BTN_DISABLED;
  }

  InvalidateRect( &m_Buttons[nIndex].rcButton );

}


void GRToolBar::SetCheck( UINT nIndex, BOOL bSet )
{

  if ( nIndex >= m_Buttons.size() )
  {
    return;
  }

  tButtonInfo&    BtnInfo = m_Buttons[nIndex];

  if ( ( !( BtnInfo.dwButtonStyle & tButtonInfo::BTN_CHECKABLE ) )
  &&   ( !( BtnInfo.dwButtonStyle & tButtonInfo::BTN_RADIO ) ) )
  {
    return;
  }

  if ( ( ( bSet )
  &&     ( BtnInfo.dwButtonStyle & tButtonInfo::BTN_CHECKED ) )
  ||   ( ( !bSet )
  &&     ( !( BtnInfo.dwButtonStyle & tButtonInfo::BTN_CHECKED ) ) ) )
  {
    // nichts zu ändern
    return;
  }

  if ( bSet )
  {
    BtnInfo.dwButtonStyle |= tButtonInfo::BTN_CHECKED;
  }
  else
  {
    BtnInfo.dwButtonStyle &= ~tButtonInfo::BTN_CHECKED;
  }

  InvalidateRect( &m_Buttons[nIndex].rcButton );

  if ( ( BtnInfo.dwButtonStyle & tButtonInfo::BTN_RADIO )
  &&   ( bSet ) )
  {
    // alle Radios links und rechts (in meiner Gruppe also) entradioen
    UINT    nTempIndex = nIndex;
    while ( nTempIndex )
    {
      nTempIndex--;
      if ( ( m_Buttons[nTempIndex].dwCommandID )
      &&   ( m_Buttons[nTempIndex].dwButtonStyle & tButtonInfo::BTN_RADIO ) )
      {
        SetCheck( nTempIndex, FALSE );
      }
    }
    nTempIndex = nIndex;
    while ( nTempIndex + 1 < m_Buttons.size() )
    {
      nTempIndex++;
      if ( ( m_Buttons[nTempIndex].dwCommandID )
      &&   ( m_Buttons[nTempIndex].dwButtonStyle & tButtonInfo::BTN_RADIO ) )
      {
        SetCheck( nTempIndex, FALSE );
      }
    }
  }

}



BOOL GRToolBar::AddButton( DWORD dwCommandID, HICON hIcon )
{

  if ( hIcon == NULL )
  {
    return FALSE;
  }

  if ( !AddButton( dwCommandID ) )
  {
    return FALSE;
  }

  ImageList_AddIcon( m_hImageList, hIcon );

  CalcSize();

  return TRUE;

}



BOOL GRToolBar::AddCheckButton( DWORD dwCommandID, HICON hIcon )
{

  if ( !AddButton( dwCommandID, hIcon ) )
  {
    return FALSE;
  }

  tButtonInfo&    BtnInfo = m_Buttons.back();

  BtnInfo.dwButtonStyle |= tButtonInfo::BTN_CHECKABLE;

  return TRUE;

}



BOOL GRToolBar::AddRadioButton( DWORD dwCommandID, HICON hIcon )
{

  if ( !AddButton( dwCommandID, hIcon ) )
  {
    return FALSE;
  }

  tButtonInfo&    BtnInfo = m_Buttons.back();

  BtnInfo.dwButtonStyle |= tButtonInfo::BTN_RADIO;

  return TRUE;

}



BOOL GRToolBar::AddSeparator()
{

  return AddButton( 0 );

}



INT_PTR GRToolBar::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{

	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// now hit test against CToolBar buttons
  for ( size_t i = 0; i < m_Buttons.size(); ++i )
  {
    const tButtonInfo&    BtnInfo = m_Buttons[i];

    if ( ( BtnInfo.dwCommandID )
    &&   ( !( BtnInfo.dwButtonStyle & tButtonInfo::BTN_DISABLED ) ) )
    {
      if ( PtInRect( &BtnInfo.rcButton, point ) )
      {
				UINT_PTR uHit = BtnInfo.dwCommandID;
        typedef struct tagAFX_OLDTOOLINFO {
	        UINT cbSize;
	        UINT uFlags;
	        HWND hwnd;
	        UINT uId;
	        RECT rect;
	        HINSTANCE hinst;
	        LPTSTR lpszText;
        } AFX_OLDTOOLINFO;

				if (pTI != NULL && pTI->cbSize >= sizeof(AFX_OLDTOOLINFO))
				{
					pTI->hwnd = m_hWnd;
					pTI->rect = BtnInfo.rcButton;
					pTI->uId = uHit;
					pTI->lpszText = LPSTR_TEXTCALLBACK;
				}
				// found matching rect, return the ID of the button
				return uHit != 0 ? static_cast<INT_PTR>(uHit) : static_cast<INT_PTR>(-1);
      }
    }
  }

  return -1;

}


