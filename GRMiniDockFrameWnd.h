#pragma once

#include "afxpriv.h"

#include <list>


// GRMiniDockFrameWnd-Rahmen

class GRControlBar;

class GRMiniDockFrameWnd : public CMiniDockFrameWnd
{

	  DECLARE_DYNCREATE(GRMiniDockFrameWnd)

  protected:

  public:

    enum eDockFramePos
    {
      DFP_FREE,
      DFP_TOP,
      DFP_BOTTOM,
      DFP_LEFT,
      DFP_RIGHT,
    };

    #define GRMINIDOCKFRAMEWND_CLASS_NAME _T( "GRMiniDockFrameWnd" )

    static BOOL hasclass;
    static BOOL RegisterMe();

	  GRMiniDockFrameWnd();           // Dynamische Erstellung verwendet geschützten Konstruktor
	  virtual ~GRMiniDockFrameWnd();

    void              StartDragging( CPoint point );

    eDockFramePos     DockPos() const;

  protected:

    std::list<GRControlBar*>   m_listControlBars;

    eDockFramePos     m_dfPos;

    GRControlBar*     m_pControlBar;

    bool              m_Dragging;

    POINT             m_ptDragOffset;

    void              InsertControlBar( GRControlBar* pBar );
    void              RemoveControlBar( GRControlBar* pBar );

    void              RepositionControlBars( GRControlBar* pBar, POINT& ptBarPos, DWORD dwDockPos );

    virtual void      CalculateNeededSize( RECT& rcRemainingSize );

	  DECLARE_MESSAGE_MAP()

    friend class GRControlBar;
    friend class GRDockingManager;

public:
  afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
  afx_msg LRESULT OnNcHitTest(CPoint point);
  afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  virtual BOOL Create( CWnd* pParent );
  virtual void RecalcLayout(BOOL bNotify = TRUE);
  afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg LRESULT OnSizeParent( WPARAM wParam, LPARAM lParam );
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnClose();
  afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
  afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};


