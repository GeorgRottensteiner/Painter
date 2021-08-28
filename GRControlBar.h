#pragma once

#include <GR/GRTypes.h>


// GRControlBar-Rahmen
class GRMiniDockFrameWnd;

class GRControlBar : public CMiniFrameWnd
{

	  DECLARE_DYNCREATE(GRControlBar)
  //DECLARE_DYNAMIC( GRControlBar )

  public:

    enum eGRControlBarStyles
    {
      GRCBS_FIXED_SIZE        = 0x00000001,   // the size cannot be changed
      GRCBS_CONTENT_SIZE      = 0x00000002,   // the size depends on content, but can be wrapping (toolbar behaviour)
      GRCBS_FREE_SIZE         = 0x00000004,   // free sizing (dlg-like)
      GRCBS_DOCK_LEFT         = 0x00000010,
      GRCBS_DOCK_RIGHT        = 0x00000020,
      GRCBS_DOCK_TOP          = 0x00000040,
      GRCBS_DOCK_BOTTOM       = 0x00000080,
      GRCBS_DOCK_ANY          = GRCBS_DOCK_LEFT | GRCBS_DOCK_RIGHT | GRCBS_DOCK_TOP | GRCBS_DOCK_BOTTOM,
      GRCBS_APPLY_SIZE        = 0x00010000,
      GRCBS_DOCKED            = 0x00020000,
      GRCBS_DOCKED_LEFT       = 0x00040000,
      GRCBS_DOCKED_RIGHT      = 0x00080000,
      GRCBS_DOCKED_TOP        = 0x00100000,
      GRCBS_DOCKED_BOTTOM     = 0x00200000,
      GRCBS_FLOATING          = 0x00400000,
      GRCBS_HIDDEN            = 0x00800000,
    };

	  GRControlBar();           // Dynamische Erstellung verwendet geschützten Konstruktor
	  virtual ~GRControlBar();

    #define GRCONTROLBAR_CLASS_NAME _T( "GRControlBar" )

    static BOOL hasclass;
    static BOOL RegisterMe();

    BOOL                      Create( LPCTSTR lpWindowName, DWORD dwCBStyle, CWnd* pParentWnd = 0, UINT nID = 0 );

    CSize                     CalcSize( GR::u32 SizingFlags = GRCBS_APPLY_SIZE | GRCBS_FLOATING );

    virtual CSize             EstimateSize( GR::u32 SizingFlags = GRCBS_APPLY_SIZE | GRCBS_FLOATING );

    bool                      IsFloating();

    void                      Dock( GR::u32 DockPos, int iX, int iY );
    void                      Float( int iX, int iY );
    void                      ToggleState();

    void                      Show( bool Show );

    virtual void              OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler );


  protected:

    bool                      m_Floating;

    DWORD                     m_CBStyle,
                              m_MRUDockPos;

    POINT                     m_MRUDockedPos,
                              m_MRUFloatingPos;

    GRMiniDockFrameWnd*       m_pFrameWnd;



  	DECLARE_MESSAGE_MAP()
    virtual void PostNcDestroy();

    friend class GRDockingManager;
    friend class GRMiniDockFrameWnd;



  public:

    afx_msg void OnMove(int x, int y);
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
    afx_msg void OnClose();

};


