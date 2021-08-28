#if !defined(AFX_EFFEKTPIXELIZE_H__5A46E4A3_05C5_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_EFFEKTPIXELIZE_H__5A46E4A3_05C5_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektPixelize.h : Header-Datei
//

#include "resource.h"

#include "EffektTemplate.h"


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEffektPixelize 

class CEffektPixelize : public EffektTemplate
{
// Konstruktion
public:
	CEffektPixelize(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEffektPixelize)
	enum { IDD = IDD_EFFEKT_PIXELIZE };
	CEdit	m_EditTiefe;
	//}}AFX_DATA

  int   m_iTiefe;

  void DoEffekt();


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEffektPixelize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEffektPixelize)
	afx_msg void OnChangeEditTiefe();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EFFEKTPIXELIZE_H__5A46E4A3_05C5_11D6_AC02_00104B6795C0__INCLUDED_
