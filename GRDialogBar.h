#pragma once

#include <GR/GRTypes.h>

#include "GRControlBar.h"
#include "GRMiniDockFrameWnd.h"

#include <vector>


class GRDialogBar : public GRControlBar
{
	  DECLARE_DYNAMIC(GRDialogBar)

  public:


	  GRDialogBar();
	  virtual ~GRDialogBar();

    virtual BOOL                Create( CWnd* pParentWnd,
                                        HWND hwndDialog,
                                        DWORD dwControlStyles = GRCBS_FIXED_SIZE, 
                                        UINT nID = 0,
                                        const GR::tChar* szTitle = NULL );


    BOOL                        LoadToolBar( LPCTSTR lpszResourceName );
    BOOL                        LoadToolBar( DWORD dwResourceID );

    BOOL                        LoadBitmap( LPCTSTR lpszResourceName );
    BOOL                        AddReplaceBitmap( HBITMAP hbmImageWell );


  protected:

    HWND                        m_hwndDialog;

    bool                        m_Horizontal;

    virtual CSize               EstimateSize( GR::u32 dwSizingFlags );

	  DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};


