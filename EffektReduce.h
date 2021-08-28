#if !defined(AFX_EFFEKTREDUCE_H__AF661A22_00F5_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_EFFEKTREDUCE_H__AF661A22_00F5_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektReduce.h : Header-Datei
//

#include "resource.h"
#include "EffektTemplate.h"


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEffektReduce 

class CEffektReduce : public EffektTemplate
{
// Konstruktion
public:
	CEffektReduce(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEffektReduce)
	enum { IDD = IDD_EFFEKT_REDUCE };
	CEdit	m_EditColors;
	//}}AFX_DATA

  DWORD     m_dwColors;

  void DoEffekt();


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEffektReduce)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEffektReduce)
	afx_msg void OnChangeEditColors();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EFFEKTREDUCE_H__AF661A22_00F5_11D6_AC02_00104B6795C0__INCLUDED_
