#if !defined(AFX_DLGSELECTPATTERN_H__844B5A82_041A_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_DLGSELECTPATTERN_H__844B5A82_041A_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectPattern.h : Header-Datei
//

#include "resource.h"

#include <MFC\ODListBox.h>


class CPattern;



class CDlgSelectPattern : public CDialog
{
// Konstruktion
public:
	CDlgSelectPattern(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDlgSelectPattern)
	enum { IDD = IDD_DIALOG_SELECTPATTERN };
	CListBox	m_DummyList;
	//}}AFX_DATA


  CODListBox        m_ODListPatterns;

  CPattern*         m_pSelectedPattern;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgSelectPattern)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgSelectPattern)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnCancel();
	afx_msg void OnButtonAddPattern();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGSELECTPATTERN_H__844B5A82_041A_11D6_AC02_00104B6795C0__INCLUDED_
