#pragma once

#include "GRMenuBar.h"

#include <MFC/ODMenu.h>

// GRODMenuBar

class GRODMenuBar : public GRMenuBar
{
	DECLARE_DYNAMIC(GRODMenuBar)

public:
	GRODMenuBar();
	virtual ~GRODMenuBar();

  virtual BOOL                LoadHMenu( HMENU hMenu );
  ODMenu*                    ReplaceMenu( HMENU hMenu );

  virtual UINT                TrackMenu( HMENU hMenu, DWORD_PTR dwExtraData, UINT nAlignFlags, int iX, int iY, 
                                         HWND hwndParent, TPMPARAMS* pParams = NULL );

  virtual void                UpdatePopupMenu( size_t iPushedItem, HMENU hMenu );

  virtual void                DeleteMenuItem( tItemInfo& ItemInfo );

  virtual void                EnableSubMenuItem( int m_nIndex, UINT_PTR nID, HMENU m_hSubMenu, BOOL bOn );

  bool                        SetMenuItemIcons( UINT_PTR nID,
                                                DWORD dwFlags,
                                                HICON hIconChecked,
                                                HICON hIconUnchecked );


protected:
	DECLARE_MESSAGE_MAP()
};


