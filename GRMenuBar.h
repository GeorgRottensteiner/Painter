#pragma once

#include <GR/GRTypes.h>

#include "GRControlBar.h"
#include "GRMiniDockFrameWnd.h"

#include <vector>



class GRMenuBar;

class GRMenuBarHelper : public CWnd
{
  public:

    GRMenuBar*        m_pMenuBar;

    HMENU             m_lastMDIMenu;

  protected:

    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

};



class GRMenuBar : public GRControlBar
{
	DECLARE_DYNAMIC(GRMenuBar)

  protected:

    struct tItemInfo;


  public:

    enum eMenuBarStyles
    {
      GRMBS_DEFAULT = 0,
      GRMBS_OVERRIDE_MDI,
    };

    GR::u32                     m_MBStyles;

    GRMenuBarHelper             m_BarHelper;

    HWND                        m_hwndMaximizedMDIChild;


	  GRMenuBar();
	  virtual ~GRMenuBar();

    virtual BOOL                Create( CWnd* pParentWnd, 
                                        DWORD dwMBStyle = GRMBS_OVERRIDE_MDI,
                                        UINT nID = AFX_IDW_TOOLBAR,
                                        const GR::tChar* szTitle = NULL );


    BOOL                        LoadMenu( LPCTSTR lpszResourceName );
    BOOL                        LoadMenu( UINT dwResourceID );
    virtual BOOL                LoadHMenu( HMENU hMenu );

    BOOL                        AddItem( DWORD dwCommandID, HICON hIcon = NULL );
    BOOL                        AddItem( const GR::String& Name, DWORD dwCommandID = 0, HICON hIcon = NULL );

    // called when opening a menu
    virtual UINT                TrackMenu( HMENU hMenu, DWORD_PTR dwExtraData, UINT nAlignFlags, 
                                          int iX, int iY, HWND hwndParent, TPMPARAMS* pParams );

    // called before opening a menu after mainframe updated it
    virtual void                UpdatePopupMenu( size_t iPushedItem, HMENU hMenu );

    // called when a menu item gets deleted (for cleanup reasons)
    virtual void                DeleteMenuItem( tItemInfo& ItemInfo );

    // overridable CMDUI-Handlers
    virtual void                EnableSubMenuItem( int m_nIndex, UINT_PTR nID, HMENU m_hSubMenu, BOOL bOn );



  protected:



    struct tItemInfo
    {
      enum eItemFlags
      {
        IF_DEFAULT      = 0,
        IF_HILIGHTED    = 0x00000001,
        IF_PUSHED       = 0x00000002,
        IF_DISABLED     = 0x00000004,
        IF_CHECKED      = 0x00000008,
        IF_GRAYED       = 0x00000010,
        IF_SYSMENUICON  = 0x40000000,   // das System-Menü-Icon ganz links/oben
        IF_SYSMENUBTN   = 0x80000000,   // Caption-Buttons Minimize/Maximize/Close für maximierte MDI-Childs
      };

      GR::u32       CommandID;
      GR::u32       ButtonStyle;
      GR::up        ExtraData;
      RECT          RectButton;
      GR::String    Text;
      HICON         hIcon;
      HMENU         hMenu;
      


      tItemInfo() :
        CommandID( 0 ),
        ButtonStyle( 0 ),
        ExtraData( 0 ),
        Text( "" ),
        hIcon( NULL ),
        hMenu( NULL )
      {
        SetRectEmpty( &RectButton );
      }
    };

    typedef std::vector<tItemInfo>    tVectItems;

    bool                        m_Horizontal,
                                m_TrackingMenu;

    size_t                      m_HighlightedItem,
                                m_PushedItem;

    tVectItems                  m_Items;

    DWORD                       m_TimerID;

    HMENU                       m_hmenuLoaded;

    HMENU                       m_hMenuDefault;

    POINT                       m_HookMousePos;

    bool                        m_ReOpenPopupMenu;

    static HHOOK                m_hookMouseProc;

    static GRMenuBar*           m_pCurrentBar;


    virtual CSize               EstimateSize( GR::u32 SizingFlags );

    virtual INT_PTR             OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;

    size_t                      MenuItemFromPoint( POINT point );

    void                        ShowPopupMenu( int Item, BOOL MouseActivated = FALSE );

    static LRESULT CALLBACK     _MenuMouseHookProc( int code, WPARAM wParam, LPARAM lParam );

	  DECLARE_MESSAGE_MAP()


  public:

    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT nIDEvent);

    virtual void                OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler );

    void                        EnableMenuItem( UINT nIndex, BOOL bOn );
    void                        CheckMenuItem( UINT nIndex, BOOL bSet );
    
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    friend class GRMenuBarCmdUI;
    friend class GRMenuBarHelper;
};


