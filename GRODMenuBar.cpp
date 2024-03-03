// GRODMenuBar.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "GRODMenuBar.h"

#include <MFC/ODMenu.h>

#include <WinSys/WinUtils.h>

#include <debug/debugclient.h>


// GRODMenuBar

IMPLEMENT_DYNAMIC(GRODMenuBar, GRMenuBar)



GRODMenuBar::GRODMenuBar()
{
}



GRODMenuBar::~GRODMenuBar()
{
  tVectItems::iterator    it( m_Items.begin() );
  while ( it != m_Items.end() )
  {
    tItemInfo&    ItemInfo = *it;

    DeleteMenuItem( ItemInfo );

    ++it;
  }
  m_Items.clear();
}


BEGIN_MESSAGE_MAP(GRODMenuBar, GRMenuBar)
//  ON_WM_DESTROY()
END_MESSAGE_MAP()



// GRODMenuBar-Meldungshandler


BOOL GRODMenuBar::LoadHMenu( HMENU hMenu )
{
  if ( hMenu == NULL )
  {
    return FALSE;
  }

  tVectItems::iterator    it( m_Items.begin() );
  while ( it != m_Items.end() )
  {
    tItemInfo&    ItemInfo = *it;

    if ( ItemInfo.ExtraData )
    {
      delete (ODMenu*)ItemInfo.ExtraData;
      ItemInfo.ExtraData = 0;
    }

    ++it;
  }
  m_Items.clear();

  int iItems = GetMenuItemCount( hMenu );

  for ( int i = 0; i < iItems; ++i )
  {
    MENUITEMINFO    mii;

    ZeroMemory( &mii, sizeof( mii ) );
    mii.cbSize = sizeof( mii );
    mii.fMask = MIIM_ID | MIIM_SUBMENU;
    if ( GetMenuItemInfo( hMenu, i, TRUE, &mii ) )
    {
      GR::tChar    szTemp[MAX_PATH];

      GetMenuString( hMenu, i, szTemp, MAX_PATH, MF_BYPOSITION );

      AddItem( GR::Convert::ToString( szTemp ), mii.wID );

      tItemInfo&    ItemInfo = m_Items.back();

      ItemInfo.hMenu = mii.hSubMenu;

      ItemInfo.ExtraData = (DWORD_PTR)ReplaceMenu( mii.hSubMenu );
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

  m_hmenuLoaded = hMenu;

  return TRUE;
}



ODMenu* GRODMenuBar::ReplaceMenu( HMENU hMenu )
{

  if ( hMenu == NULL )
  {
    return NULL;
  }

  ODMenu*    pODMenu = new ODMenu( ODMenu::ODMS_XP );

  for ( int i = 0; i < ::GetMenuItemCount( hMenu ); ++i )
  {
    GR::tChar    szTemp[MAX_PATH];

    GetMenuString( hMenu, i, szTemp, MAX_PATH, MF_BYPOSITION );

    MENUITEMINFO    ii;

    ii.cbSize = sizeof( ii );
    ii.fMask  = MIIM_ID | MIIM_SUBMENU | MIIM_STATE;
    GetMenuItemInfo( hMenu, i, TRUE, &ii );

    DWORD   dwExtraFlags = ii.fState;

    if ( ii.hSubMenu != NULL )
    {
      pODMenu->AppendMenu( dwExtraFlags | MF_POPUP, (UINT_PTR)ReplaceMenu( ii.hSubMenu ), szTemp );
    }
    else if ( ii.wID == 0 )
    {
      pODMenu->AppendMenu( dwExtraFlags | MF_SEPARATOR );
    }
    else
    {
      pODMenu->AppendMenu( dwExtraFlags | MF_STRING, ii.wID, szTemp );
    }
  }

  return pODMenu;

}



UINT GRODMenuBar::TrackMenu( HMENU hMenu, DWORD_PTR dwExtraData, UINT nAlignFlags, int iX, int iY, HWND hwndParent, TPMPARAMS* pParams )
{
  ODMenu*    pMenu = (ODMenu*)dwExtraData;

  if ( pMenu )
  {
    return pMenu->TrackPopupMenu( TPM_RETURNCMD | TPM_NONOTIFY, iX, iY, CWnd::FromHandle( hwndParent ) );
  }
  return 0;
}



void GRODMenuBar::UpdatePopupMenu( size_t iPushedItem, HMENU hMenu )
{
  if ( iPushedItem >= m_Items.size() )
  {
    return;
  }

  tItemInfo&    ItemInfo = m_Items[iPushedItem];

  if ( ItemInfo.ExtraData )
  {
    delete (ODMenu*)ItemInfo.ExtraData;
    ItemInfo.ExtraData = 0;
  }

  ItemInfo.ExtraData = (DWORD_PTR)ReplaceMenu( hMenu );
  ItemInfo.CommandID = (UINT_PTR)hMenu;
}



void GRODMenuBar::DeleteMenuItem( tItemInfo& ItemInfo )
{
  if ( ItemInfo.ExtraData )
  {
    delete (ODMenu*)ItemInfo.ExtraData;
    ItemInfo.ExtraData = 0;
  }
}



void GRODMenuBar::EnableSubMenuItem( int m_nIndex, UINT_PTR nID, HMENU m_hSubMenu, BOOL bOn )
{
  tVectItems::iterator    it( m_Items.begin() );
  while ( it != m_Items.end() )
  {
    tItemInfo&    ItemInfo = *it;

    if ( ItemInfo.ExtraData )
    {
      ODMenu*    pODMenu = (ODMenu*)ItemInfo.ExtraData;

      if ( pODMenu->EnableMenuItem( nID, bOn ) )
      {
        return;
      }
    }
    ++it;
  }
}



bool GRODMenuBar::SetMenuItemIcons( UINT_PTR nID,
                                     DWORD dwFlags,
                                     HICON hIconChecked,
                                     HICON hIconUnchecked )
{
  tVectItems::iterator    it( m_Items.begin() );
  while ( it != m_Items.end() )
  {
    tItemInfo&    ItemInfo = *it;

    if ( ItemInfo.ExtraData )
    {
      ODMenu*    pODMenu = (ODMenu*)ItemInfo.ExtraData;

      if ( pODMenu->SetMenuItemIcons( nID, dwFlags, hIconChecked, hIconUnchecked ) )
      {
        return true;
      }
    }
    ++it;
  }

  return false;
}
