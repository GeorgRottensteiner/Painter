#if !defined(AFX_PROPDOCINFO_H__2ABD3F61_5CD6_11D5_9D7D_0001029EF128__INCLUDED_)
#define AFX_PROPDOCINFO_H__2ABD3F61_5CD6_11D5_9D7D_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropDocInfo.h : Header-Datei
//

#include "DocumentInfo.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPropDocInfo 

class CPropDocInfo : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropDocInfo)

// Konstruktion
public:
	CPropDocInfo();
	~CPropDocInfo();

// Dialogfelddaten
	//{{AFX_DATA(CPropDocInfo)
	enum { IDD = IDD_PROP_DOCUMENT };
	CStatic	m_StaticUsedColors;
	CStatic	m_StaticDocSize;
	CStatic	m_StaticType;
	CStatic	m_StaticPath;
	CStatic	m_StaticName;
	CStatic	m_StaticDepth;
	//}}AFX_DATA

  DocumentInfo   *m_pDocInfo;


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPropDocInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPropDocInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PROPDOCINFO_H__2ABD3F61_5CD6_11D5_9D7D_0001029EF128__INCLUDED_
