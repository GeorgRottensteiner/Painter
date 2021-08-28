#if !defined(AFX_EFFEKTDIFFUSOR_H__5690C740_002F_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_EFFEKTDIFFUSOR_H__5690C740_002F_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektDiffusor.h : Header-Datei
//

#include "resource.h"

#include "EffektTemplate.h"


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEffektDiffusor 

class CEffektDiffusor : public EffektTemplate
{
// Konstruktion
public:
	CEffektDiffusor(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEffektDiffusor)
	enum { IDD = IDD_EFFEKT_DIFFUSOR };
	CEdit	m_EditIterations;
	DWORD	m_dwIterations;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEffektDiffusor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL


  void DoEffekt();

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEffektDiffusor)
	afx_msg void OnChangeEditIterations();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EFFEKTDIFFUSOR_H__5690C740_002F_11D6_AC02_00104B6795C0__INCLUDED_
