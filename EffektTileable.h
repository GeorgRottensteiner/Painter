#if !defined(AFX_EFFEKTTILEABLE_H__809B9E83_FF70_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_EFFEKTTILEABLE_H__809B9E83_FF70_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektGrayScale.h : Header-Datei
//

#include "resource.h"
#include "EffektTemplate.h"




/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEffektGrayScale 

class CEffektTileable : public EffektTemplate
{
// Konstruktion
public:
	CEffektTileable(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEffektTileable)
	enum { IDD = IDD_EFFEKT_TILEABLE };
	//}}AFX_DATA

  void DoEffekt();


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEffektTileable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEffektTileable)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EFFEKTTILEABLE_H__809B9E83_FF70_11D5_AC02_00104B6795C0__INCLUDED_
