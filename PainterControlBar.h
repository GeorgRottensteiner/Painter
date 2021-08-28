#pragma once


//#include <MFC/SizeControlBar.h>

#include <vector>



class CPainterControlBar : public CControlBar
{

  public:

    enum eControlType
    {
      CT_NOTHING = 0,
      CT_BUTTON,
      CT_GRIPPER,
      CT_AUTOHIDING_GRIPPER,
      CT_BUTTON_DROPDOWN,
      CT_CHECKBUTTON,
      CT_RADIOBUTTON,
      CT_SEPARATOR,
      CT_TEXT,
      CT_HWND,
    };


  protected:
    
    struct tControlEntry
    {
      DWORD       m_dwType,
                  m_dwID;
      HICON       m_hIcon;
      HWND        m_hwnd;
      std::string m_strText;
      RECT        m_rcControl;
      bool        m_bMouseOver,
                  m_bPushed,
                  m_bChecked,
                  m_bHidden,
                  m_bDisabled;

      tControlEntry() :
        m_dwType( CT_NOTHING ),
        m_dwID( 0 ),
        m_hwnd( NULL ),
        m_hIcon( NULL ),
        m_strText( "" ),
        m_bDisabled( false ),
        m_bMouseOver( false ),
        m_bPushed( false ),
        m_bChecked( false ),
        m_bHidden( false )
      {
        SetRectEmpty( &m_rcControl );
      }

    };

    #define GRCONTROLBAR_CLASS_NAME _T( "GRControlBar" )

    static BOOL hasclass;
    static BOOL RegisterMe();

    typedef std::vector<tControlEntry>     tVectControls;

    tVectControls                   m_vectControls;

    size_t                          m_iMouseOverItem,
                                    m_iPushedItem;

    CSize                           m_szCommitted;

    DWORD                           m_dwTimerID,
                                    m_dwCommittedMode;

    bool                            m_bHorizontal,
                                    m_bDelayedLayout;

    virtual void                    DrawItem( tControlEntry& Entry, CDC& dc, RECT rc );

	DECLARE_DYNAMIC(CPainterControlBar)

public:
	CPainterControlBar();
	virtual ~CPainterControlBar();
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

    virtual CSize                   CalcDynamicLayout( int nLength, DWORD dwMode );
    virtual CSize                   CalcFixedLayout(BOOL bStretch, BOOL bHorz);

    virtual CSize                   CalcLayout( DWORD dwMode, int nLength = -1 );

    void                            AddGripper( bool bAutoHiding = true );
    void                            AddSeparator();
    

    int                             ItemFromPoint( POINT& pt );

    bool                            GetItemRect( size_t iItem, RECT& rc );

    void                            AddControl( eControlType, const char* szText, DWORD dwID, HICON hIcon = NULL, SIZE* pSize = NULL );

    void                            AddHWND( HWND hWnd );
    void                            AddText( const char* szText, DWORD dwID, HICON hIcon = NULL, SIZE* pSize = NULL );
    void                            AddButton( const char* szText, DWORD dwID, HICON hIcon = NULL, SIZE* pSize = NULL );
    void                            AddButtonDropDown( const char* szText, DWORD dwID, HICON hIcon = NULL, SIZE* pSize = NULL );
    void                            AddCheckButton( const char* szText, DWORD dwID, HICON hIcon = NULL, SIZE* pSize = NULL );
    void                            AddRadioButton( const char* szText, DWORD dwID, HICON hIcon = NULL, SIZE* pSize = NULL );

    void                            EnableControl( size_t iControlIndex, bool bEnable = true );
    void                            SetCheck( size_t iButtonIndex, bool bSetCheck = true );
    bool                            IsChecked( size_t iButtonIndex ) const;


    bool                            IsHorz() const;

    virtual void                    RecalcItemSize( tControlEntry& Entry );

  protected:
    

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnPaint();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnDestroy();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


