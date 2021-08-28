#if !defined(AFX_PROPDOCINFO2_H__94822283_6D52_11D5_9D7D_0001029EF128__INCLUDED_)
#define AFX_PROPDOCINFO2_H__94822283_6D52_11D5_9D7D_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropDocInfo2.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPropDocInfo2 

class CPropDocInfo2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropDocInfo2)

// Konstruktion
public:

  DWORD         m_dwScrollWidth,
                m_dwScrollHeight;
	CPropDocInfo2();
	~CPropDocInfo2();

// Dialogfelddaten
	//{{AFX_DATA(CPropDocInfo2)
	enum { IDD = IDD_PROP_DOCUMENT_2 };
	CStatic	m_StaticScrollHeight;
	CStatic	m_StaticScrollWidth;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPropDocInfo2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPropDocInfo2)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PROPDOCINFO2_H__94822283_6D52_11D5_9D7D_0001029EF128__INCLUDED_
