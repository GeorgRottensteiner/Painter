#if !defined(AFX_EFFEKTJALOUSIE_H__5690C743_002F_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_EFFEKTJALOUSIE_H__5690C743_002F_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektJalousie.h : Header-Datei
//

#include "resource.h"

#include "EffektTemplate.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEffektJalousie 

class CEffektJalousie : public EffektTemplate
{
// Konstruktion
public:
	CEffektJalousie(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEffektJalousie)
	enum { IDD = IDD_EFFEKT_JALOUSIE };
	CComboBox	m_ComboAusrichtung;
	CComboBox	m_ComboRichtung;
	CEdit	m_EditBreite;
	//}}AFX_DATA

  BOOL        m_bVertikal,
              m_bVorwaerts;

  int         m_iBreite;


  void DoEffekt();

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEffektJalousie)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEffektJalousie)
	afx_msg void OnChangeEditBreite();
	afx_msg void OnSelchangeComboRichtung();
	afx_msg void OnSelchangeComboAusrichtung();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EFFEKTJALOUSIE_H__5690C743_002F_11D6_AC02_00104B6795C0__INCLUDED_
