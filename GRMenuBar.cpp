#include "stdafx.h"
#include "GRMenuBar.h"

#include <debug/debugclient.h>


// GRMenuBar
HHOOK         GRMenuBar::m_hookMouseProc = NULL;
GRMenuBar*   GRMenuBar::m_pCurrentBar = NULL;



IMPLEMENT_DYNAMIC(GRMenuBar, GRControlBar)
  GRMenuBar::GRMenuBar()

{
  m_Horizontal            = true;
  m_TrackingMenu          = false;

  m_HighlightedItem       = -1;
  m_PushedItem            = -1;

  m_TimerID               = 0;

  m_hmenuLoaded           = NULL;
  m_hMenuDefault          = NULL;

  m_BarHelper.m_pMenuBar  = this;

  m_hwndMaximizedMDIChild = NULL;
}



GRMenuBar::~GRMenuBar()
{
  if ( m_hmenuLoaded )
  {
    if ( !( m_MBStyles & GRMBS_OVERRIDE_MDI ) )
    {
      DestroyMenu( m_hmenuLoaded );
    }
    m_hmenuLoaded = NULL;
  }
  if ( m_MBStyles & GRMBS_OVERRIDE_MDI )
  {
    m_BarHelper.UnsubclassWindow();
  }
}



BEGIN_MESSAGE_MAP(GRMenuBar, GRControlBar)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_DESTROY()
  ON_WM_TIMER()
//  ON_WM_LBUTTONDBLCLK()
ON_WM_CREATE()
END_MESSAGE_MAP()



BOOL GRMenuBar::Create( CWnd* pParentWnd, DWORD MBStyles, UINT nID, const GR::tChar* szTitle )
{
  m_MBStyles = MBStyles;
  BOOL result = GRControlBar::Create( szTitle, CBRS_ALIGN_ANY, pParentWnd, nID );

  if ( !result )
  {
    return FALSE;
  }

  if ( MBStyles & GRMBS_OVERRIDE_MDI )
  {
    HWND hwnd = ((CMDIFrameWnd*)GetTopLevelFrame())->m_hWndMDIClient;

    m_hMenuDefault = GetTopLevelFrame()->GetMenu()->GetSafeHmenu();
    LoadHMenu( m_hMenuDefault );

    // Menü des MainFrames zappen, wir haben jetzt die Kontrolle
    GetTopLevelFrame()->SetMenu( NULL );

    m_BarHelper.SubclassWindow( hwnd );
  }
  return result;
}



class GRMenuBarCmdUI : public CCmdUI        // class private to this file !
{
  public: // re-implementations only

    HMENU         m_hSubMenu;

	  virtual void Enable(BOOL bOn);
	  virtual void SetCheck(int nCheck);
	  virtual void SetText(LPCTSTR lpszText);

    GRMenuBarCmdUI() :
      CCmdUI(),
      m_hSubMenu( NULL )
      {
      }
};



#include <debug/debugclient.h>
#include "resource.h"

void GRMenuBarCmdUI::Enable(BOOL bOn)
{
  GRMenuBar*   pBar = (GRMenuBar*)m_pOther;

  m_bEnableChanged = TRUE;

  if ( m_hSubMenu )
  {
    // ein echtes HMENU
    pBar->EnableSubMenuItem( m_nIndex, m_nID, m_hSubMenu, bOn );
    return;
  }

  if ( !bOn )
  {
    if ( pBar->m_Items[m_nIndex].hMenu )
    {
      return;
    }
  }

  pBar->EnableMenuItem( m_nIndex, bOn );
}



void GRMenuBarCmdUI::SetCheck(int nCheck)
{
  GRMenuBar*   pBar = (GRMenuBar*)m_pOther;

  if ( m_hSubMenu )
  {
    // ein echtes HMENU
    if ( nCheck )
    {
      CheckMenuItem( m_hSubMenu, m_nID, MF_CHECKED | MF_BYCOMMAND );
    }
    else
    {
      CheckMenuItem( m_hSubMenu, m_nID, MF_UNCHECKED | MF_BYCOMMAND );
    }    
    return;
  }

  pBar->CheckMenuItem( m_nIndex, nCheck ? TRUE : FALSE );
}



void GRMenuBarCmdUI::SetText(LPCTSTR)
{
	// ignore it
}



void GRMenuBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	GRMenuBarCmdUI state;


	state.m_pOther = this;

	state.m_nIndexMax = (UINT)m_Items.size();
	for ( state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++ )
	{
		// get buttons state
    state.m_nID = m_Items[state.m_nIndex].CommandID;

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



BOOL GRMenuBar::LoadMenu( UINT dwResourceID )
{
  return LoadMenu( MAKEINTRESOURCE( dwResourceID ) );
}



BOOL GRMenuBar::LoadMenu( LPCTSTR lpszResourceName )
{
	HINSTANCE hInst = AfxFindResourceHandle( lpszResourceName, RT_MENU );

  HMENU   hMenu = ::LoadMenu( hInst, lpszResourceName );
  if ( hMenu == NULL )
  {
    return FALSE;
  }

  return LoadHMenu( hMenu );
}



BOOL GRMenuBar::LoadHMenu( HMENU hMenu )
{
  if ( hMenu == NULL )
  {
    return FALSE;
  }

  m_Items.clear();

  int iItems = GetMenuItemCount( hMenu );

  for ( int i = 0; i < iItems; ++i )
  {
    MENUITEMINFO    mii;

    mii.cbSize = sizeof( mii );
    mii.fMask = MIIM_ID | MIIM_SUBMENU;
    if ( GetMenuItemInfo( hMenu, i, TRUE, &mii ) )
    {
      GR::tChar    szTemp[MAX_PATH];

      GetMenuString( hMenu, i, szTemp, MAX_PATH, MF_BYPOSITION );

      GR::String    menuText = GR::Convert::ToString( szTemp );

      AddItem( menuText, mii.wID );

      tItemInfo&    ItemInfo = m_Items.back();

      ItemInfo.hMenu = mii.hSubMenu;
    }
  }

  if ( !m_Horizontal )
  {
    CalcSize( GRCBS_APPLY_SIZE | GRCBS_FLOATING | GRCBS_DOCKED_LEFT );
  }
  else
  {
    CalcSize();
  }

  if ( m_hmenuLoaded )
  {
    if ( !( m_MBStyles & GRMBS_OVERRIDE_MDI ) )
    {
      DestroyMenu( m_hmenuLoaded );
    }
    m_hmenuLoaded = NULL;
  }

  m_hmenuLoaded = hMenu;

  return TRUE;
}



BOOL GRMenuBar::AddItem( DWORD CommandID, HICON hIcon )
{

  tItemInfo   ItemInfo;

  ItemInfo.CommandID  = CommandID;
  ItemInfo.hIcon      = hIcon;
  SetRect( &ItemInfo.RectButton, 0, 0, 23, 23 );

  m_Items.push_back( ItemInfo );

  CalcSize();

  return TRUE;
}



BOOL GRMenuBar::AddItem( const GR::String& Text, DWORD CommandID, HICON hIcon )
{
  tItemInfo   ItemInfo;

  ItemInfo.CommandID  = CommandID;
  ItemInfo.hIcon      = hIcon;
  ItemInfo.Text       = Text;
  SetRect( &ItemInfo.RectButton, 0, 0, 23, 23 );

  m_Items.push_back( ItemInfo );

  return TRUE;
}



CSize GRMenuBar::EstimateSize( GR::u32 SizingFlags )
{
  CDC*        pDC = GetDC();

  bool        estimateHorizontal = true;

  if ( ( SizingFlags & GRCBS_DOCKED_LEFT )
  ||   ( SizingFlags & GRCBS_DOCKED_RIGHT ) )
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

  HFONT   hOldFont = (HFONT)pDC->SelectObject( (HFONT)GetStockObject( DEFAULT_GUI_FONT ) );

  tVectItems  vectTempItems = m_Items;

  // falls noch MDI-Menu-Icons oder Caption-Buttons da sind, weg
  tVectItems::iterator    it( vectTempItems.begin() );
  while ( it != vectTempItems.end() )
  {
    tItemInfo&    ItemInfo = *it;

    if ( ( ItemInfo.ButtonStyle & tItemInfo::IF_SYSMENUBTN )
    ||   ( ItemInfo.ButtonStyle & tItemInfo::IF_SYSMENUICON ) )
    {
      if ( SizingFlags & GRCBS_APPLY_SIZE )
      {
        DeleteMenuItem( ItemInfo );
      }
      it = vectTempItems.erase( it );
      continue;
    }

    ++it;
  }

  if ( m_hwndMaximizedMDIChild )
  {
    // vorne ein Icon dazu
    tItemInfo   ItemInfo;

    ItemInfo.ButtonStyle = tItemInfo::IF_SYSMENUICON;
    SetRect( &ItemInfo.RectButton, 0, 0, 23, 23 );
    ItemInfo.Text = "@";

    ItemInfo.hMenu = ::GetSystemMenu( m_hwndMaximizedMDIChild, FALSE );

    vectTempItems.insert( vectTempItems.begin(), ItemInfo );


    // hinten drei Caption-Buttons
    RECT    rc = ItemInfo.RectButton;

    int   iWidth = GetSystemMetrics( SM_CXSMICON );
    int   iHeight = GetSystemMetrics( SM_CYSMICON ) - 2;

    // Icon zentrieren
    int   iDX = 0,
          iDY = 0;

    if ( estimateHorizontal )
    {
      iDY = ( 23 - iHeight ) / 2;
    }
    else
    {
      iDX = ( 23 - iWidth ) / 2;
    }


    {
      tItemInfo   ItemInfo;

      ItemInfo.ButtonStyle = tItemInfo::IF_SYSMENUBTN;
      ItemInfo.CommandID = SC_MINIMIZE;
      SetRect( &ItemInfo.RectButton, iDX, iDY, iDX + iWidth, iDY + iHeight );
      ItemInfo.Text = "_";

      vectTempItems.push_back( ItemInfo );
    }
    {
      tItemInfo   ItemInfo;

      ItemInfo.ButtonStyle = tItemInfo::IF_SYSMENUBTN;
      ItemInfo.CommandID = SC_RESTORE;
      SetRect( &ItemInfo.RectButton, iDX, iDY, iDX + iWidth, iDY + iHeight );
      ItemInfo.Text = "·";

      vectTempItems.push_back( ItemInfo );
    }
    {
      tItemInfo   ItemInfo;

      ItemInfo.ButtonStyle = tItemInfo::IF_SYSMENUBTN;
      ItemInfo.CommandID = SC_CLOSE;
      SetRect( &ItemInfo.RectButton, iDX, iDY, iDX + iWidth, iDY + iHeight );
      ItemInfo.Text = "X";

      vectTempItems.push_back( ItemInfo );
    }
  }

  for ( size_t i = 0; i < vectTempItems.size(); ++i )
  {
    tItemInfo&    ItemInfo = vectTempItems[i];

    if ( ( ItemInfo.ButtonStyle & tItemInfo::IF_SYSMENUICON )
    ||   ( ItemInfo.ButtonStyle & tItemInfo::IF_SYSMENUBTN ) )
    {
      int   iWidth  = 0;
      int   iHeight = 0;
      if ( !estimateHorizontal )
      {
        iHeight = ItemInfo.RectButton.bottom - ItemInfo.RectButton.top;
        if ( ItemInfo.CommandID == SC_RESTORE )
        {
          iHeight += 2;
        }
        szCB.cy += iHeight;
        iY += iHeight;
      }
      else
      {
        iWidth  = ItemInfo.RectButton.right - ItemInfo.RectButton.left;
        if ( ItemInfo.CommandID == SC_RESTORE )
        {
          iWidth += 2;
        }
        szCB.cx += iWidth;
        iX += iWidth;
      }
      OffsetRect( &ItemInfo.RectButton, iX - iWidth, iY - iHeight );
      continue;
    }

    ItemInfo.RectButton.left   = iX;
    ItemInfo.RectButton.top    = iY;


    if ( !estimateHorizontal )
    {
      GR::tString     strVertText;

      for ( size_t j = 0; j < ItemInfo.Text.length(); ++j )
      {
        if ( ItemInfo.Text[j] == '&' )
        {
          strVertText += ItemInfo.Text[j + 1];
          ++j;
          continue;
        }
        strVertText += ItemInfo.Text[j];
      }
      pDC->DrawText( strVertText.c_str(), (int)strVertText.length(), &ItemInfo.RectButton, DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX );

      // Text wird horizontal berechnet
      InflateRect( &ItemInfo.RectButton, 8, 4 );
      OffsetRect( &ItemInfo.RectButton, 8, 4 );
      szCB.cy += ItemInfo.RectButton.right - ItemInfo.RectButton.left;
      if ( ItemInfo.RectButton.bottom - ItemInfo.RectButton.top > szCB.cx )
      {
        szCB.cx = ItemInfo.RectButton.bottom - ItemInfo.RectButton.top;
      }
      

      int   iTemp = ItemInfo.RectButton.right - ItemInfo.RectButton.left;

      ItemInfo.RectButton.right = ItemInfo.RectButton.left + ItemInfo.RectButton.bottom - ItemInfo.RectButton.top;
      ItemInfo.RectButton.bottom = ItemInfo.RectButton.top + iTemp;

      iY += ItemInfo.RectButton.bottom - ItemInfo.RectButton.top;
    }
    else
    {
      GR::WString    menuText = GR::Convert::ToStringW( ItemInfo.Text );

      pDC->DrawText( menuText.c_str(), (int)menuText.length(), &ItemInfo.RectButton, DT_CALCRECT | DT_SINGLELINE );
      InflateRect( &ItemInfo.RectButton, 8, 4 );
      OffsetRect( &ItemInfo.RectButton, 8, 4 );

      szCB.cx += ItemInfo.RectButton.right - ItemInfo.RectButton.left;
      if ( ItemInfo.RectButton.bottom - ItemInfo.RectButton.top > szCB.cy )
      {
        szCB.cy = ItemInfo.RectButton.bottom - ItemInfo.RectButton.top;
      }
      iX += ItemInfo.RectButton.right - ItemInfo.RectButton.left;
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

  pDC->SelectObject( hOldFont );

  ReleaseDC( pDC );

  if ( SizingFlags & GRCBS_APPLY_SIZE )
  {
    m_Horizontal = estimateHorizontal;
    m_Items = vectTempItems;
  }

  return szCB;
}



void GRMenuBar::OnPaint()
{
  CPaintDC dc(this);

  RECT    rc;

  GetClientRect( &rc );

  int   iX = 0,
        iY = 0;


  if ( !IsFloating() )
  {
    // Gripper zeichnen
    if ( m_Horizontal )
    {
      dc.Draw3dRect( 0, 0, 2, 23, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
      dc.Draw3dRect( 2, 0, 2, 23, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );

      iX += 4;
    }
    else
    {
      dc.Draw3dRect( 0, 0, 23, 2, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
      dc.Draw3dRect( 0, 2, 23, 2, GetSysColor( COLOR_3DDKSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );

      iY += 4;
    }
  }

  dc.SetBkMode( TRANSPARENT );

  HFONT   hFontToUse = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
  if ( !m_Horizontal )
  {
    LOGFONT   lf;

    memset( &lf, 0, sizeof( lf ) );

    GetObject( (HFONT)GetStockObject( DEFAULT_GUI_FONT ), sizeof( lf ), &lf ); 

    lf.lfEscapement = 2700;//900;

    hFontToUse = CreateFontIndirect( &lf );
  }

  HFONT   hOldFont = (HFONT)dc.SelectObject( hFontToUse );

  for ( size_t i = 0; i < m_Items.size(); ++i )
  {
    tItemInfo&    ItemInfo = m_Items[i];

    int   iCX = ( ItemInfo.RectButton.right - ItemInfo.RectButton.left - 16 ) / 2;
    int   iCY = ( ItemInfo.RectButton.bottom - ItemInfo.RectButton.top - 15 ) / 2;

    if ( ( ItemInfo.ButtonStyle & tItemInfo::IF_PUSHED )
    ||   ( ItemInfo.ButtonStyle & tItemInfo::IF_CHECKED ) )
    {
      iCX++;
      iCY++;
    }

    if ( ItemInfo.ButtonStyle & tItemInfo::IF_SYSMENUBTN )
    {
      DWORD   dwExtraStyle = 0;
      if ( ItemInfo.ButtonStyle & tItemInfo::IF_PUSHED )
      {
        dwExtraStyle = DFCS_PUSHED;
      }
      RECT    rc = ItemInfo.RectButton;

      HFONT   hFontTemp = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
      HFONT   hOldFont = (HFONT)dc.SelectObject( hFontTemp );

      dc.SetTextAlign( TA_TOP );

      switch ( ItemInfo.CommandID )
      {
        case SC_MINIMIZE:
          DrawFrameControl( dc.GetSafeHdc(), &rc, DFC_CAPTION, dwExtraStyle | DFCS_CAPTIONMIN );
          break;
        case SC_RESTORE:
          DrawFrameControl( dc.GetSafeHdc(), &rc, DFC_CAPTION, dwExtraStyle | DFCS_CAPTIONRESTORE );
          break;
        case SC_CLOSE:
          DrawFrameControl( dc.GetSafeHdc(), &rc, DFC_CAPTION, dwExtraStyle | DFCS_CAPTIONCLOSE );
          break;
      }

      dc.SelectObject( hOldFont );

      if ( m_Horizontal )
      {
        iX += ItemInfo.RectButton.right - ItemInfo.RectButton.left;
      }
      else
      {
        iY += ItemInfo.RectButton.bottom - ItemInfo.RectButton.top;
      }
      continue;
    }
    else if ( ItemInfo.ButtonStyle & tItemInfo::IF_SYSMENUICON )
    {
      if ( m_hwndMaximizedMDIChild )
      {
        HICON hIcon = (HICON)GetClassLong( m_hwndMaximizedMDIChild, GCL_HICONSM );

        int   iWidth  = GetSystemMetrics( SM_CXSMICON );
        int   iHeight = GetSystemMetrics( SM_CYSMICON );

        if ( hIcon )
        {
          DrawIconEx( dc.GetSafeHdc(), ItemInfo.RectButton.left + ( 23 - iWidth ) / 2, ItemInfo.RectButton.top + ( 23 - iHeight ) / 2,
                      hIcon, 0, 0, 0, 0, DI_NORMAL );
        }
      }
      if ( m_Horizontal )
      {
        iX += ItemInfo.RectButton.right - ItemInfo.RectButton.left;
      }
      else
      {
        iY += ItemInfo.RectButton.bottom - ItemInfo.RectButton.top;
      }
      continue;
    }

    if ( m_Horizontal )
    {
      if ( ItemInfo.hIcon )
      {
        if ( ItemInfo.ButtonStyle & tItemInfo::IF_DISABLED )
        {
          DrawState( dc.GetSafeHdc(),
                      0, 0,
                      (LPARAM)ItemInfo.hIcon,
                      0,
                      iX + iCX, iCY,
                      16,
                      15,
                      DST_ICON | DSS_DISABLED );
        }
        else
        {
          DrawState( dc.GetSafeHdc(),
                      0, 0,
                      (LPARAM)ItemInfo.hIcon,
                      0,
                      iX + iCX, iCY,
                      16,
                      15,
                      DST_ICON );
        }
      }

      RECT    rcText = ItemInfo.RectButton;
      GR::WString    menuText = GR::Convert::ToStringW( ItemInfo.Text );

      OffsetRect( &rcText, 8, 4 );

      if ( ItemInfo.ButtonStyle & tItemInfo::IF_DISABLED )
      {
        dc.SetTextColor( GetSysColor( COLOR_WINDOW ) );
        OffsetRect( &rcText, 1, 1 );
        dc.DrawText( menuText.c_str(), (int)menuText.length(), &rcText, DT_SINGLELINE );
        dc.SetTextColor( GetSysColor( COLOR_3DSHADOW ) );
        OffsetRect( &rcText, -1, -1 );
        dc.DrawText( menuText.c_str(), (int)menuText.length(), &rcText, DT_SINGLELINE );
      }
      else
      {
        if ( ItemInfo.ButtonStyle & tItemInfo::IF_PUSHED )
        {
          OffsetRect( &rcText, 1, 1 );
        }
        dc.DrawText( menuText.c_str(), (int)menuText.length(), &rcText, DT_SINGLELINE );
      }

      iX += ItemInfo.RectButton.right - ItemInfo.RectButton.left;
    }
    else
    {
      if ( ItemInfo.hIcon )
      {
        if ( ItemInfo.ButtonStyle & tItemInfo::IF_DISABLED )
        {
          DrawState( dc.GetSafeHdc(),
                      0, 0,
                      (LPARAM)ItemInfo.hIcon,
                      0,
                      iCX, iY + iCY,
                      16,
                      15,
                      DST_ICON | DSS_DISABLED );
        }
        else
        {
          DrawState( dc.GetSafeHdc(),
                      0, 0,
                      (LPARAM)ItemInfo.hIcon,
                      0,
                      iCX, iY + iCY,
                      16,
                      15,
                      DST_ICON );
        }
      }
      RECT    rcText;

      rcText.left = 0;
      rcText.top = iY;
      rcText.right = rc.right;
      rcText.bottom = rc.bottom;

      dc.SetTextAlign( TA_BASELINE );

      OffsetRect( &rcText, 4, 8 );

      GR::tString     strVertText;

      for ( size_t j = 0; j < ItemInfo.Text.length(); ++j )
      {
        if ( ItemInfo.Text[j] == '&' )
        {
          strVertText += ItemInfo.Text[j + 1];
          ++j;
          continue;
        }
        strVertText += ItemInfo.Text[j];
      }
      if ( ItemInfo.ButtonStyle & tItemInfo::IF_DISABLED )
      {
        dc.SetTextColor( GetSysColor( COLOR_WINDOW ) );
        OffsetRect( &rcText, 1, 1 );
        dc.DrawText( strVertText.c_str(), (int)strVertText.length(), &rcText, DT_SINGLELINE );
        dc.SetTextColor( GetSysColor( COLOR_3DSHADOW ) );
        OffsetRect( &rcText, -1, -1 );
        dc.DrawText( strVertText.c_str(), (int)strVertText.length(), &rcText, DT_SINGLELINE );
      }
      else
      {
        if ( ItemInfo.ButtonStyle & tItemInfo::IF_PUSHED )
        {
          OffsetRect( &rcText, 1, 1 );
        }
        dc.DrawText( strVertText.c_str(), (int)strVertText.length(), &rcText, DT_SINGLELINE | DT_NOPREFIX );
      }

      iY += ItemInfo.RectButton.bottom - ItemInfo.RectButton.top;
    }

    if ( ( ItemInfo.ButtonStyle & tItemInfo::IF_PUSHED )
    ||   ( ItemInfo.ButtonStyle & tItemInfo::IF_CHECKED ) )
    {
      RECT    rc = ItemInfo.RectButton;

      dc.Draw3dRect( &rc, GetSysColor( COLOR_3DSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
    }
    else if ( ItemInfo.ButtonStyle & tItemInfo::IF_HILIGHTED )
    {
      RECT    rc = ItemInfo.RectButton;

      dc.Draw3dRect( &rc, GetSysColor( COLOR_3DHIGHLIGHT ), GetSysColor( COLOR_3DSHADOW ) );
    }
  }

  dc.SelectObject( hOldFont );

  if ( !m_Horizontal )
  {
    DeleteObject( hFontToUse );
  }
}



size_t GRMenuBar::MenuItemFromPoint( POINT point )
{
  size_t   iOverButton = -1;

  for ( size_t i = 0; i < m_Items.size(); ++i )
  {
    tItemInfo&    ItemInfo = m_Items[i];

    if ( !( ItemInfo.ButtonStyle & tItemInfo::IF_DISABLED ) )
    {
      if ( PtInRect( &ItemInfo.RectButton, point ) )
      {
        iOverButton = i;
        return iOverButton;
      }
    }
  }

  return iOverButton;

}



void GRMenuBar::OnLButtonDown(UINT nFlags, CPoint point)
{

  if ( !IsFloating() )
  {
    RECT    rcGripper;

    SetRectEmpty( &rcGripper );

    if ( m_Horizontal )
    {
      rcGripper.right   = 4;
      rcGripper.bottom  = 23;
    }
    else
    {
      rcGripper.right   = 23;
      rcGripper.bottom  = 4;
    }
    if ( PtInRect( &rcGripper, point ) )
    {
      ClientToScreen( &point );
      m_pFrameWnd->StartDragging( point );
      return;
    }
  }

  size_t iOverButton = MenuItemFromPoint( point );

  if ( iOverButton != m_HighlightedItem )
  {
    if ( m_HighlightedItem != -1 )
    {
      InvalidateRect( &m_Items[m_HighlightedItem].RectButton );
      m_Items[m_HighlightedItem].ButtonStyle &= ~tItemInfo::IF_HILIGHTED;
      m_HighlightedItem = -1;
    }
    m_HighlightedItem  = iOverButton;
  }
  if ( m_PushedItem != iOverButton )
  {
    m_PushedItem       = iOverButton;
    if ( iOverButton != -1 )
    {
      m_Items[iOverButton].ButtonStyle |= tItemInfo::IF_HILIGHTED | tItemInfo::IF_PUSHED;
      InvalidateRect( &m_Items[iOverButton].RectButton );

      tItemInfo&    ItemInfo = m_Items[m_PushedItem];
      if ( ItemInfo.ButtonStyle & tItemInfo::IF_SYSMENUBTN )
      {
        SetCapture();
      }
      else if ( ItemInfo.hMenu == NULL )
      {
        GetTopLevelFrame()->PostMessage( WM_COMMAND, MAKEWPARAM( m_Items[m_PushedItem].CommandID, 0 ), (LPARAM)GetSafeHwnd() );
      }
      else
      {
        if ( !m_TrackingMenu )
        {
          ShowPopupMenu( m_PushedItem, TRUE );
        }
        return;
      }
      //SetCapture();
    }
  }

  GRControlBar::OnLButtonDown(nFlags, point);
}



void GRMenuBar::OnLButtonUp(UINT nFlags, CPoint point)
{
  if ( m_TrackingMenu )
  {
    return;
  }

  if ( GetCapture() == this )
  {
    ReleaseCapture();
  }
  if ( ( m_HighlightedItem != -1 )
  &&   ( m_PushedItem != -1 )
  &&   ( m_HighlightedItem == m_PushedItem ) )
  {
    tItemInfo&    ItemInfo = m_Items[m_PushedItem];
    if ( ( ItemInfo.ButtonStyle & tItemInfo::IF_SYSMENUBTN )
    ||   ( ItemInfo.ButtonStyle & tItemInfo::IF_SYSMENUICON ) )
    {
      GetTopLevelFrame()->PostMessage( WM_COMMAND, MAKEWPARAM( ItemInfo.CommandID, 0 ), (LPARAM)GetSafeHwnd() );
    }
  }

  if ( m_HighlightedItem != -1 )
  {
    InvalidateRect( &m_Items[m_HighlightedItem].RectButton );
    m_Items[m_HighlightedItem].ButtonStyle &= ~tItemInfo::IF_HILIGHTED;
    m_HighlightedItem = -1;
  }
  if ( m_PushedItem != -1 )
  {
    InvalidateRect( &m_Items[m_PushedItem].RectButton );
    m_Items[m_PushedItem].ButtonStyle &= ~tItemInfo::IF_PUSHED;
    m_PushedItem = -1;
  }

  GRControlBar::OnLButtonUp(nFlags, point);
}



void GRMenuBar::OnMouseMove(UINT nFlags, CPoint point)
{
  size_t   iOverButton = -1;

  for ( size_t i = 0; i < m_Items.size(); ++i )
  {
    tItemInfo&    ItemInfo = m_Items[i];

    if ( !( ItemInfo.ButtonStyle & tItemInfo::IF_DISABLED ) )
    {
      if ( PtInRect( &ItemInfo.RectButton, point ) )
      {
        iOverButton = i;
        break;
      }
    }
  }

  if ( iOverButton != m_HighlightedItem )
  {
    if ( m_HighlightedItem != -1 )
    {
      InvalidateRect( &m_Items[m_HighlightedItem].RectButton );
      m_Items[m_HighlightedItem].ButtonStyle &= ~tItemInfo::IF_HILIGHTED;
      m_HighlightedItem = -1;
    }
    if ( m_PushedItem != -1 )
    {
      if ( m_Items[m_PushedItem].ButtonStyle & tItemInfo::IF_PUSHED )
      {
        m_Items[m_PushedItem].ButtonStyle &= ~tItemInfo::IF_PUSHED;
        InvalidateRect( &m_Items[m_PushedItem].RectButton );
      }
    }
    m_HighlightedItem  = iOverButton;
    /*
    if ( ( nFlags & MK_LBUTTON )
    &&   ( iOverButton != -1 ) )
    {
      m_PushedItem = iOverButton;
      m_Items[m_PushedItem].ButtonStyle |= tItemInfo::IF_PUSHED;
      InvalidateRect( &m_Items[m_PushedItem].RectButton );

      tItemInfo&    ItemInfo = m_Items[m_PushedItem];
      if ( ItemInfo.hMenu == NULL )
      {
        GetTopLevelFrame()->PostMessage( WM_COMMAND, MAKEWPARAM( m_Items[m_PushedItem].CommandID, 0 ), (LPARAM)GetSafeHwnd() );
      }
      else
      {
        ShowPopupMenu( TRUE );
        return;
      }
    }
    */

    if ( iOverButton != -1 )
    {
      if ( ( m_HighlightedItem == m_PushedItem )
      &&   ( m_HighlightedItem != -1 ) )
      {
        m_Items[m_HighlightedItem].ButtonStyle |= tItemInfo::IF_PUSHED;
      }
      if ( m_PushedItem == -1 )
      {
        m_Items[m_HighlightedItem].ButtonStyle |= tItemInfo::IF_HILIGHTED;
      }
      InvalidateRect( &m_Items[iOverButton].RectButton );
      if ( m_TimerID == 0 )
      {
        m_TimerID = (DWORD)SetTimer( 7, 50, NULL );
      }
    }
  }

  if ( ( m_TimerID != 0 )
  &&   ( m_HighlightedItem == -1 ) )
  {
    KillTimer( m_TimerID );
    m_TimerID = 0;
  }

  /*
  if ( m_HighlightedItem != -1 )
  {
    AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
    if ( ( pModuleThreadState->m_pLastStatus != (CControlBar*)this )
    ||   ( pModuleThreadState->m_nLastStatus != m_Items[m_HighlightedItem].CommandID ) )
    {
  	  pModuleThreadState->m_pLastStatus = (CControlBar*)this;
	    pModuleThreadState->m_nLastStatus = m_Items[m_HighlightedItem].CommandID;
      GetOwner()->SendMessage(WM_SETMESSAGESTRING, m_Items[m_HighlightedItem].CommandID);
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
  }
  */

  GRControlBar::OnMouseMove(nFlags, point);
}



/// Key hook to change menu selection on mouse move
LRESULT CALLBACK GRMenuBar::_MenuMouseHookProc( int code, WPARAM wParam, LPARAM lParam )
{
	if( HC_ACTION != code )
		return CallNextHookEx( m_hookMouseProc, code, wParam, lParam );

	switch( wParam )
	{
	  case WM_MOUSEMOVE: 
      {
		    LRESULT lResult = CallNextHookEx( m_hookMouseProc, code, wParam, lParam );

		    MOUSEHOOKSTRUCT* psMouseHook = (MOUSEHOOKSTRUCT*)lParam;
		    CPoint point = psMouseHook->pt;

        m_pCurrentBar->m_HookMousePos = psMouseHook->pt;
		    m_pCurrentBar->ScreenToClient( &point );
        /*
		    if( point == _gs_ptLastMove )
			    break;
          */

        size_t   iOverButton = -1;

        POINT     ptTemp = point;

        //m_pCurrentBar->ScreenToClient( &ptTemp );

        iOverButton = m_pCurrentBar->MenuItemFromPoint( ptTemp );

        for ( size_t i = 0; i < m_pCurrentBar->m_Items.size(); ++i )
        {
          tItemInfo&    ItemInfo = m_pCurrentBar->m_Items[i];

          if ( !( ItemInfo.ButtonStyle & tItemInfo::IF_DISABLED ) )
          {
            if ( PtInRect( &ItemInfo.RectButton, point ) )
            {
              iOverButton = i;
              break;
            }
          }
        }

        HWND hHitWnd = ::WindowFromPoint( psMouseHook->pt );

        if ( iOverButton != m_pCurrentBar->m_PushedItem )
		    {
			    /// Check mouse over the MenuBar
			    if ( hHitWnd == m_pCurrentBar->m_hWnd )
			    {
            if ( ( iOverButton != -1 )
            &&   ( !( m_pCurrentBar->m_Items[iOverButton].ButtonStyle & GRMenuBar::tItemInfo::IF_SYSMENUBTN ) )
            &&   ( !( m_pCurrentBar->m_Items[iOverButton].ButtonStyle & GRMenuBar::tItemInfo::IF_SYSMENUICON ) ) )
            {
              /*
				      keybd_event( VK_ESCAPE, 0, 0, 0 );
				      keybd_event( VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0 );
              */
              int iWidth = GetSystemMetrics( SM_CXSCREEN );
              int iHeight = GetSystemMetrics( SM_CYSCREEN );

              m_pCurrentBar->m_HookMousePos = point;

              mouse_event( MOUSEEVENTF_ABSOLUTE, point.x * 65535 / iWidth, point.y * 65535 / iHeight, 0, 0 );
              mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
              mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
              if ( m_pCurrentBar->m_PushedItem != -1 )
              {
              m_pCurrentBar->m_Items[m_pCurrentBar->m_PushedItem].ButtonStyle &= ~tItemInfo::IF_PUSHED;
              m_pCurrentBar->m_Items[m_pCurrentBar->m_PushedItem].ButtonStyle &= ~tItemInfo::IF_HILIGHTED;
              m_pCurrentBar->InvalidateRect( &m_pCurrentBar->m_Items[m_pCurrentBar->m_PushedItem].RectButton );
              }
              m_pCurrentBar->InvalidateRect( &m_pCurrentBar->m_Items[iOverButton].RectButton );
              m_pCurrentBar->m_PushedItem = iOverButton;
              m_pCurrentBar->m_ReOpenPopupMenu = true;
              m_pCurrentBar->UpdateWindow();
            }
            else if ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 )
            {
              int iWidth = GetSystemMetrics( SM_CXSCREEN );
              int iHeight = GetSystemMetrics( SM_CYSCREEN );
              mouse_event( MOUSEEVENTF_ABSOLUTE, point.x * 65535 / iWidth, point.y * 65535 / iHeight, 0, 0 );
              mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
              mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
              mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
              if ( m_pCurrentBar->m_PushedItem != -1 )
              {
              m_pCurrentBar->m_Items[m_pCurrentBar->m_PushedItem].ButtonStyle &= ~tItemInfo::IF_PUSHED;
              m_pCurrentBar->m_Items[m_pCurrentBar->m_PushedItem].ButtonStyle &= ~tItemInfo::IF_HILIGHTED;
              m_pCurrentBar->InvalidateRect( &m_pCurrentBar->m_Items[m_pCurrentBar->m_PushedItem].RectButton );
              }
              m_pCurrentBar->m_PushedItem = iOverButton;
              m_pCurrentBar->UpdateWindow();
            }
			    }
		    }
		    return lResult;
		  }
	}

	return CallNextHookEx( m_hookMouseProc, code, wParam, lParam );
}



UINT GRMenuBar::TrackMenu( HMENU hMenu, DWORD_PTR dwExtraData, UINT nAlignFlags, int iX, int iY, HWND hwndParent, TPMPARAMS* pParams )
{

  return TrackPopupMenuEx( hMenu, nAlignFlags, iX, iY, hwndParent, pParams );

}



void GRMenuBar::UpdatePopupMenu( size_t iPushedItem, HMENU hMenu )
{
}



void GRMenuBar::ShowPopupMenu( int iItem, BOOL bMouseActivated )
{

  //m_PushedItem = iItem;
  /*
	if ( FALSE == _Is_PopupActive() )
	{
		if( bMouseSelect )
			_Set_MouseActivated();
		else
			_Reset_MouseActivated();
      */

		/// Say entering menuloop, not using Trackpopmenu though using it 
    GetTopLevelFrame()->SendMessage( WM_ENTERMENULOOP, (WPARAM)FALSE );

		/// One time menu initialization
    GetTopLevelFrame()->SendMessage( WM_INITMENU, (WPARAM)m_Items[m_PushedItem].hMenu );
		//_Set_PopupActive();
	//}

  INT nID = 0;

  //while ( true )
  {
    POINT   point;

    if ( m_Horizontal )
    {
      point.x = m_Items[m_PushedItem].RectButton.left;
      point.y = m_Items[m_PushedItem].RectButton.bottom;
    }
    else
    {
      point.x = m_Items[m_PushedItem].RectButton.right;
      point.y = m_Items[m_PushedItem].RectButton.top;
    }

	  /// Select first popup item, if selected by keyboard
	  if ( FALSE == bMouseActivated )
	  {
		  keybd_event( VK_DOWN, 0, 0, 0 );
		  keybd_event( VK_DOWN, 0, KEYEVENTF_KEYUP, 0 );
	  }

    /*
	  _gs_nMenuNavKey = 0;
	  _gs_hmnuCur = _gs_hmnuLast = NULL;
	  _gs_nIDCur = _gs_nIDLast = 0;

	  /// Set Keyboard and mouse hook
	  _gs_bMenuKbHooked = TRUE;
    */
    m_hookMouseProc = SetWindowsHookEx( WH_MOUSE, _MenuMouseHookProc, NULL, ::GetCurrentThreadId() );

	  TPMPARAMS tpm;
	  tpm.cbSize = sizeof(TPMPARAMS);
	  GetClientRect( &tpm.rcExclude );
	  INT nAlignFlags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD;

    if ( !IsFloating() )
    {
      if ( m_MRUDockPos & CBRS_ALIGN_RIGHT )
      {
        nAlignFlags &= ~TPM_LEFTALIGN;
        nAlignFlags |= TPM_RIGHTALIGN;
        point.x = m_Items[m_PushedItem].RectButton.left;
      }
      else if ( m_MRUDockPos & CBRS_ALIGN_BOTTOM )
      {
        nAlignFlags &= ~TPM_TOPALIGN;
        nAlignFlags |= TPM_BOTTOMALIGN;
        point.y = m_Items[m_PushedItem].RectButton.top;
      }
    }

    RECT    m_rctCur;
    GetClientRect( &m_rctCur );
	  if ( !m_Horizontal )
	  {
		  tpm.rcExclude.left = m_rctCur.left;
		  tpm.rcExclude.right = m_rctCur.right;
		  nAlignFlags |= TPM_HORIZONTAL;
	  }
	  else
	  {
		  tpm.rcExclude.top = m_rctCur.top;
		  tpm.rcExclude.bottom = m_rctCur.bottom;
		  nAlignFlags |= TPM_VERTICAL;
	  }

    m_pCurrentBar = this;

	  ClientToScreen( &point );
	  ClientToScreen( &tpm.rcExclude );
    m_ReOpenPopupMenu = false;

    SetForegroundWindow();
    int   iOldPushed = m_PushedItem;

    // Menu-Items CMDuien
    GRMenuBarCmdUI   state;

    HMENU             hMenu = m_Items[m_PushedItem].hMenu;

    state.m_pOther = this;
    state.m_nIndexMax = ::GetMenuItemCount( hMenu );
	  for ( state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++ )
	  {
      state.m_nID = ::GetMenuItemID( hMenu, state.m_nIndex );
		  if ( state.m_nID == 0 )
			  continue; // menu separator or invalid cmd - ignore it

		  if ( state.m_nID == (UINT)-1 )
		  {
			  // possibly a popup menu, route to first item of that popup
			  state.m_hSubMenu = GetSubMenu( hMenu, state.m_nIndex );
			  if (state.m_hSubMenu == NULL ||
				  (state.m_nID = GetMenuItemID( state.m_hSubMenu, 0 )) == 0 ||
				  state.m_nID == (UINT)-1)
			  {
				  continue;       // first item of popup can't be routed to
			  }
			  state.DoUpdate( GetTopLevelFrame(), FALSE);    // popups are never auto disabled
		  }
		  else
		  {
			  // normal menu item
			  // Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			  //    set and command is _not_ a system command.
        state.m_hSubMenu = hMenu;
			  //state.DoUpdate(GetTopLevelFrame(), m_bAutoMenuEnable && state.m_nID < 0xF000);
        state.DoUpdate( GetTopLevelFrame(), true );// , m_bAutoMenuEnable && state.m_nID < 0xF000 );
		  }

		  // adjust for menu deletions and additions
		  UINT nCount = GetMenuItemCount( hMenu );
		  if (nCount < state.m_nIndexMax)
		  {
			  state.m_nIndex -= (state.m_nIndexMax - nCount);
			  while (state.m_nIndex < nCount &&
				  GetMenuItemID( hMenu, state.m_nIndex ) == state.m_nID)
			  {
				  state.m_nIndex++;
			  }
		  }
		  state.m_nIndexMax = nCount;
	  }

    GetTopLevelFrame()->SendMessage( WM_INITMENUPOPUP, (WPARAM)hMenu, MAKELPARAM( m_PushedItem, FALSE ) );

    UpdatePopupMenu( m_PushedItem, hMenu );

    m_TrackingMenu = true;
    nID = TrackMenu( m_Items[m_PushedItem].hMenu, 
                     m_Items[m_PushedItem].ExtraData,
                     nAlignFlags,
                     point.x, point.y, GetTopLevelFrame()->GetSafeHwnd(), &tpm );
    m_TrackingMenu = false;

	  /// Deactivate Keyboard and mouse hook
	  UnhookWindowsHookEx( m_hookMouseProc );

    UpdateWindow();

    /*
              int iWidth = GetSystemMetrics( SM_CXSCREEN );
              int iHeight = GetSystemMetrics( SM_CYSCREEN );
              mouse_event( MOUSEEVENTF_ABSOLUTE, point.x * 65535 / iWidth, point.y * 65535 / iHeight, 0, 0 );
              mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
              mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
              */

    POINT     ptMouse;

    ptMouse = m_HookMousePos;

    ScreenToClient( &ptMouse );

    size_t    iMenuItem = MenuItemFromPoint( ptMouse );


    if ( ( m_PushedItem != -1 )
    &&   ( m_PushedItem != iOldPushed )
    &&   ( nID == 0 ) )
    {
      // ein anderes Menü-Item ausgewählt...
      int iWidth = GetSystemMetrics( SM_CXSCREEN );
      int iHeight = GetSystemMetrics( SM_CYSCREEN );
      mouse_event( MOUSEEVENTF_ABSOLUTE, m_HookMousePos.x * 65535 / iWidth, m_HookMousePos.y * 65535 / iHeight, 0, 0 );
      mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
      if ( iMenuItem != -1 )
      {
        mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
        m_PushedItem = -1;
      }
      else
      {
        
        if ( m_PushedItem != -1 )
        {
          m_Items[m_PushedItem].ButtonStyle &= ~tItemInfo::IF_PUSHED;
          InvalidateRect( &m_Items[m_PushedItem].RectButton );
          m_PushedItem = -1;
        }
        if ( m_HighlightedItem != -1 )
        {
          m_Items[m_HighlightedItem].ButtonStyle &= ~tItemInfo::IF_HILIGHTED;
          InvalidateRect( &m_Items[m_HighlightedItem].RectButton );
          m_HighlightedItem = -1;
        }
      }
    }

    if ( ( nID == 0 )
    &&   ( iMenuItem == -1 ) )
    {
      mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
      if ( m_PushedItem != -1 )
      {
        m_Items[m_PushedItem].ButtonStyle &= ~tItemInfo::IF_PUSHED;
        InvalidateRect( &m_Items[m_PushedItem].RectButton );
        m_PushedItem = -1;
      }
      if ( m_HighlightedItem != -1 )
      {
        m_Items[m_HighlightedItem].ButtonStyle &= ~tItemInfo::IF_HILIGHTED;
        InvalidateRect( &m_Items[m_HighlightedItem].RectButton );
        m_HighlightedItem = -1;
      }
    }

    if ( nID )
    {
      // etwas ausgewählt
      if ( m_PushedItem != -1 )
      {
        m_Items[m_PushedItem].ButtonStyle &= ~tItemInfo::IF_PUSHED;
        InvalidateRect( &m_Items[m_PushedItem].RectButton );
        m_PushedItem = -1;
      }
      if ( m_HighlightedItem != -1 )
      {
        m_Items[m_HighlightedItem].ButtonStyle &= ~tItemInfo::IF_HILIGHTED;
        InvalidateRect( &m_Items[m_HighlightedItem].RectButton );
        m_HighlightedItem = -1;
      }
      //break;
    }
    
  }

	//_gs_bMenuKbHooked = FALSE;

	//g_hmnuMenuBarActive = NULL;

  /*
	if( hmnuHidden )
	{
		INT nCnt = GetMenuItemCount( hmnuHidden );
		for( INT i = 0; i < nCnt; i++ )
			RemoveMenu( hmnuHidden, 0, MF_BYPOSITION );

		DestroyMenu( hmnuHidden );
	}

	/// Menu item selected, post to active frame
	if( nID )
	{
		DeactivateMenu();
		if( bSysCommand )
			m_pFrameWnd->PostMessage( WM_SYSCOMMAND, nID );
		else
    */
  if ( nID )
  {
    GetTopLevelFrame()->SendMessage( WM_EXITMENULOOP, (WPARAM)FALSE );
  }

  if ( nID )
  {
    GetTopLevelFrame()->PostMessage( WM_COMMAND, nID );
  }
      /*
	}
	else if( 0 == _gs_nMenuNavKey )
		DeactivateMenu();
    */

	UpdateWindow();

  /*
	/// Simulate key to select next menu item, if menu exit by left or right key
	if( VK_LEFT == _gs_nMenuNavKey || VK_RIGHT == _gs_nMenuNavKey )
	{
		KeyDown( _gs_nMenuNavKey, 1, 0 );
		KeyUp( _gs_nMenuNavKey, 1, 0 );

		KeyDown( VK_DOWN, 1, 0 );
		KeyUp( VK_DOWN, 1, 0 );
	}
  */
}



void GRMenuBar::OnDestroy()
{
  if ( m_BarHelper.GetSafeHwnd() )
  {
    m_BarHelper.UnsubclassWindow();
  }

  if ( m_TimerID != 0 )
  {
    KillTimer( m_TimerID );
    m_TimerID = 0;
  }

  GRControlBar::OnDestroy();
}



void GRMenuBar::OnTimer(UINT nIDEvent)
{

  POINT   ptMouse;

  GetCursorPos( &ptMouse );

  ScreenToClient( &ptMouse );

  RECT    rc;

  GetClientRect( &rc );

  if ( !PtInRect( &rc, ptMouse ) )
  {
    if ( m_HighlightedItem != -1 )
    {
      InvalidateRect( &m_Items[m_HighlightedItem].RectButton );
      m_Items[m_HighlightedItem].ButtonStyle &= ~tItemInfo::IF_HILIGHTED;
      m_HighlightedItem = -1;

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
#include ".\grmenubar.h"

void GRMenuBar::EnableMenuItem( UINT nIndex, BOOL bOn )
{

  if ( nIndex >= m_Items.size() )
  {
    return;
  }

  if ( ( m_Items[nIndex].ButtonStyle & tItemInfo::IF_SYSMENUBTN )
  ||   ( m_Items[nIndex].ButtonStyle & tItemInfo::IF_SYSMENUICON ) )
  {
    return;
  }

  if ( ( ( bOn )
  &&     ( !( m_Items[nIndex].ButtonStyle & tItemInfo::IF_DISABLED ) ) )
  ||   ( ( !bOn )
  &&     ( m_Items[nIndex].ButtonStyle & tItemInfo::IF_DISABLED ) ) )
  {
    // nichts zu ändern
    return;
  }

  if ( bOn )
  {
    m_Items[nIndex].ButtonStyle &= ~tItemInfo::IF_DISABLED;
  }
  else
  {
    m_Items[nIndex].ButtonStyle |= tItemInfo::IF_DISABLED;
  }

  InvalidateRect( &m_Items[nIndex].RectButton );

}


void GRMenuBar::CheckMenuItem( UINT nIndex, BOOL bSet )
{

  if ( nIndex >= m_Items.size() )
  {
    return;
  }

  tItemInfo&    ItemInfo = m_Items[nIndex];

  if ( ( ( bSet )
  &&     ( ItemInfo.ButtonStyle & tItemInfo::IF_CHECKED ) )
  ||   ( ( !bSet )
  &&     ( !( ItemInfo.ButtonStyle & tItemInfo::IF_CHECKED ) ) ) )
  {
    // nichts zu ändern
    return;
  }

  if ( bSet )
  {
    ItemInfo.ButtonStyle |= tItemInfo::IF_CHECKED;
  }
  else
  {
    ItemInfo.ButtonStyle &= ~tItemInfo::IF_CHECKED;
  }

  InvalidateRect( &m_Items[nIndex].RectButton );

}



INT_PTR GRMenuBar::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{

	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// now hit test against CToolBar buttons
  for ( size_t i = 0; i < m_Items.size(); ++i )
  {
    const tItemInfo&    ItemInfo = m_Items[i];

    if ( ( ItemInfo.CommandID )
    &&   ( !( ItemInfo.ButtonStyle & tItemInfo::IF_DISABLED ) ) )
    {
      if ( PtInRect( &ItemInfo.RectButton, point ) )
      {
				UINT_PTR uHit = ItemInfo.CommandID;
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
					pTI->rect = ItemInfo.RectButton;
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



int GRMenuBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (GRControlBar::OnCreate(lpCreateStruct) == -1)
    return -1;

  SetFont( CFont::FromHandle( (HFONT)GetStockObject( DEFAULT_GUI_FONT ) ) );

  return 0;
}



LRESULT GRMenuBarHelper::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

  if ( message == WM_MDISETMENU )
  {
    if ( wParam == NULL )
    {
      wParam = (WPARAM)m_pMenuBar->m_hMenuDefault;
    }

    if ( wParam != NULL )
    {
      m_lastMDIMenu = (HMENU)wParam;
      m_pMenuBar->LoadHMenu( (HMENU)wParam );
      m_pMenuBar->Invalidate();
      wParam = NULL;
    }
  }
  else if ( message == WM_MDIGETACTIVE )
  {
    LRESULT   lRes = CWnd::WindowProc(message, wParam, lParam);

    HWND    hwndActive = (HWND)lRes;

    DWORD   dwCalcFlags = GRControlBar::GRCBS_APPLY_SIZE;
    if ( m_pMenuBar->IsFloating() )
    {
      dwCalcFlags |= GRControlBar::GRCBS_FLOATING;
    }
    else if ( m_pMenuBar->m_Horizontal )
    {
      dwCalcFlags |= GRControlBar::GRCBS_DOCKED_TOP;
    }
    else
    {
      dwCalcFlags |= GRControlBar::GRCBS_DOCKED_LEFT;
    }

    if ( hwndActive == NULL )
    {
      if ( m_pMenuBar->m_hwndMaximizedMDIChild != NULL )
      {
        m_pMenuBar->m_hwndMaximizedMDIChild = NULL;
        m_pMenuBar->CalcSize( dwCalcFlags );
        m_pMenuBar->Invalidate();
      }
    }
    else if ( ::IsZoomed( hwndActive ) )
    {
      if ( hwndActive != m_pMenuBar->m_hwndMaximizedMDIChild )
      {
        m_pMenuBar->m_hwndMaximizedMDIChild = hwndActive;
        m_pMenuBar->CalcSize( dwCalcFlags );
        m_pMenuBar->Invalidate();
      }
    }
    else
    {
      if ( m_pMenuBar->m_hwndMaximizedMDIChild != NULL )
      {
        m_pMenuBar->m_hwndMaximizedMDIChild = NULL;
        m_pMenuBar->CalcSize( dwCalcFlags );
        m_pMenuBar->Invalidate();
      }
    }
    return lRes;
  }
  else if ( message == WM_DESTROY )
  {
    UnsubclassWindow();
  }

  LRESULT   lRes = CWnd::WindowProc(message, wParam, lParam);

  return lRes;
}



void GRMenuBar::DeleteMenuItem( tItemInfo& ItemInfo )
{
}


void GRMenuBar::EnableSubMenuItem( int m_nIndex, UINT_PTR nID, HMENU hSubMenu, BOOL bOn )
{

  if ( bOn )
  {
    ::EnableMenuItem( hSubMenu, nID, MF_ENABLED | MF_BYCOMMAND );
  }
  else
  {
    ::EnableMenuItem( hSubMenu, nID, MF_GRAYED | MF_BYCOMMAND  );
  }    

}
