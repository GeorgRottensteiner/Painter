#if !defined(AFX_PROPPROMPTS_H__8FA5F703_5CCC_11D5_9D7D_0001029EF128__INCLUDED_)
#define AFX_PROPPROMPTS_H__8FA5F703_5CCC_11D5_9D7D_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPrompts.h : Header-Datei
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPropPrompts 

class CPropPrompts : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPrompts)

// Konstruktion
public:
	CPropPrompts();
	~CPropPrompts();

// Dialogfelddaten
	//{{AFX_DATA(CPropPrompts)
	enum { IDD = IDD_PROP_PROMPTS };
	CButton	m_CheckPNT;
	CButton	m_CheckFNX;
	CButton	m_CheckFNH;
	CButton	m_CheckIGF;
	CButton	m_CheckBTN;
	CButton	m_CheckBTH;
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPropPrompts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPropPrompts)
	virtual void OnOK();
	afx_msg void OnCheckRegisterFileTypes();
	afx_msg void OnCheckRegisterBtn();
	afx_msg void OnCheckRegisterBth();
	afx_msg void OnCheckRegisterFnx();
	afx_msg void OnCheckRegisterFnh();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckRegisterPNT();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PROPPROMPTS_H__8FA5F703_5CCC_11D5_9D7D_0001029EF128__INCLUDED_
