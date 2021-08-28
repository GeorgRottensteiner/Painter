#if !defined(AFX_CPAINTERFONTVIEW_H__B5766BE2_D49C_11D3_924A_00104B6795C0__INCLUDED_)
#define AFX_CPAINTERFONTVIEW_H__B5766BE2_D49C_11D3_924A_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/*
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
*/

#include "ViewInfo.h"
#include "afxwin.h"
#include "afxcmn.h"

#include <MFC/ImageWnd.h>



class CPainterFontDoc;

class CPainterFontView : public CFormView
{
protected:
	CPainterFontView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CPainterFontView)

  HWND                  hwndFontWorkArea;

// Formulardaten
public:
	//{{AFX_DATA(PainterFontView)
	enum { IDD = IDD_PAINTERFONTVIEW_FORM };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA

// Attribute
public:

  CViewInfo             m_viewInfo;

  CPainterFontDoc*      GetDocument();

  void DoUpdate( LPARAM lHint, GR::tRect* pRect = NULL );

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPainterFontView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementierung
protected:

  bool                m_DoNotUpdate;


	virtual ~CPainterFontView();

  void                RedrawFontPreview();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPainterFontView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnChangeEditFontZeichennr();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	afx_msg void OnDeltaposSpinFontBreite(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinFontHoehe(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBearbeitenPasteasselection();
	afx_msg void OnEditCopy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  CEdit m_EditZeichen;
  CEdit m_EditWidth;
  CEdit m_EditHeight;
  CEdit m_EditXOffset;
  CEdit m_EditYOffset;
  CSpinButtonCtrl m_SpinZeichen;
  CSpinButtonCtrl m_SpinWidth;
  CSpinButtonCtrl m_SpinHeight;
  CSpinButtonCtrl m_SpinXOffset;
  CSpinButtonCtrl m_SpinYOffset;
  CStatic m_StaticLetter;
  CStatic m_StaticZeichen;

  CImageWnd             m_FontPreview;



  afx_msg void OnDestroy();
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnSplitRGB();
  afx_msg void OnSplitAlpha();

  void                    LetterSizeModified( int iWidth, int iHeight );
  void                    LetterNumberChanged( int iChar );

  afx_msg void OnBnClickedBtnKillLetter();
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

  BOOL PreTranslateMessage( MSG* pMsg );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CPAINTERFONTVIEW_H__B5766BE2_D49C_11D3_924A_00104B6795C0__INCLUDED_
