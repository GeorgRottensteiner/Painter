#pragma once

#include <GR/GRTypes.h>

#include "GRControlBar.h"
#include "GRMiniDockFrameWnd.h"

#include <vector>




class GRToolBar : public GRControlBar
{
#define GRTOOLBAR_CLASS_NAME _T( "GRToolBar" )

	//DECLARE_DYNAMIC(GRToolBar)
  DECLARE_DYNCREATE( GRToolBar )

public:




	GRToolBar();
	virtual ~GRToolBar();

  virtual BOOL      CreateEx( CWnd* pParentWnd, 
                              DWORD dwCtrlStyle = TBSTYLE_FLAT, 
                              DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, 
                              CRect rcBorders = CRect(0, 0, 0, 0), 
                              UINT nID = AFX_IDW_TOOLBAR,
                              const GR::tChar* szTitle = NULL );


  BOOL                        LoadToolBar( LPCTSTR lpszResourceName );
  BOOL                        LoadToolBar( DWORD dwResourceID );

  BOOL                        LoadBitmap( LPCTSTR lpszResourceName );

  BOOL                        AddButton( DWORD dwCommandID, HICON hIcon );
  BOOL                        AddCheckButton( DWORD dwCommandID, HICON hIcon );
  BOOL                        AddRadioButton( DWORD dwCommandID, HICON hIcon );
  BOOL                        AddSeparator();


protected:

  struct tButtonInfo
  {
    enum eButtonFlags
    {
      BTN_DEFAULT     = 0,
      BTN_HILIGHTED   = 0x00000001,
      BTN_PUSHED      = 0x00000002,
      BTN_DISABLED    = 0x00000004,
      BTN_CHECKABLE   = 0x00000008,
      BTN_CHECKED     = 0x00000010,
      BTN_RADIO       = 0x00000020,
    };

    DWORD     dwCommandID;
    DWORD     dwButtonStyle;
    RECT      rcButton;
    
    tButtonInfo() :
      dwCommandID( 0 ),
      dwButtonStyle( 0 )
    {
      SetRectEmpty( &rcButton );
    }
  };

  typedef std::vector<tButtonInfo>    tVectButtons;

  bool                        m_Horizontal;

  size_t                      m_HighlightedButton,
                              m_PushedButton;

  tVectButtons                m_Buttons;

  HIMAGELIST                  m_hImageList;

  DWORD                       m_TimerID;


  BOOL                        AddButton( DWORD dwCommandID );

  virtual CSize               EstimateSize( GR::u32 dwSizingFlags );

  virtual INT_PTR             OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnDestroy();
  afx_msg void OnTimer(UINT nIDEvent);

  virtual void                OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler );

  void                        EnableControl( UINT nIndex, BOOL bOn );
  void                        SetCheck( UINT nIndex, BOOL bSet );
};


