// PainterView.h : Schnittstelle der Klasse CPainterView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAINTERVIEW_H__6E63B46C_8C86_11D2_9F65_004033315771__INCLUDED_)
#define AFX_PAINTERVIEW_H__6E63B46C_8C86_11D2_9F65_004033315771__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ViewInfo.h"



class CPainterView : public CScrollView
{
protected: // Nur aus Serialisierung erzeugen
	CPainterView();
	DECLARE_DYNCREATE(CPainterView)

// Attribute
public:

  ViewInfo         m_viewInfo;

	CPainterDoc*      GetDocument();
  int               iXorX,
                    iXorY;

  void XorRectangleOn( signed long slXStart, signed long slYStart, signed long slXEnd,
                                     signed long slYEnd );
  void GetXorImage( signed long slXStart, signed long slYStart, signed long slXEnd,
                                  signed long slYEnd );


// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPainterView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual void OnInitialUpdate();
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CPainterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CPainterView)
	afx_msg void OnAnimStop();
	afx_msg void OnAnimPlay();
	afx_msg void OnAnimEnd();
	afx_msg void OnAnimHome();
	afx_msg void OnAnimStepBack();
	afx_msg void OnAnimStepForward();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaletteLaden();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaletteEditieren();
	afx_msg void OnPaintZoomin();
	afx_msg void OnPaintZoomout();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditCopy();
	afx_msg void OnAnimFrameDelete();
	afx_msg void OnAnimFrameInsert();
	afx_msg void OnBearbeitenPasteasselection();
	afx_msg void OnFileSave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnAnimFrameInsertbehind();
	afx_msg void OnPaletteSpeichern();
	afx_msg void OnMenuSpezialSplit();
	afx_msg void OnEditZoomIn();
	afx_msg void OnEditZoomOut();
	afx_msg void OnMenuSpezialSplitAlpha();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnUpdatePaletteEditieren(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#ifndef _DEBUG  // Testversion in PainterView.cpp
inline CPainterDoc* CPainterView::GetDocument()
   { return (CPainterDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PAINTERVIEW_H__6E63B46C_8C86_11D2_9F65_004033315771__INCLUDED_)
