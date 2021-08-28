#if !defined(AFX_EFFEKTAURA_H__809B9E82_FF70_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_EFFEKTAURA_H__809B9E82_FF70_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektAura.h : Header-Datei
//

#include "resource.h"

#include "EffektTemplate.h"
#include "afxwin.h"

class CEffektAura : public EffektTemplate
{
// Konstruktion
public:
	CEffektAura(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEffektAura)
	enum { IDD = IDD_EFFEKT_AURA };
	CComboBox	m_ComboType;
	CEdit	m_EditTiefe;
	int		m_AuraTiefe;
  int   m_AuraType;
  bool  m_AuraRounded;
	//}}AFX_DATA


  void DoEffekt();

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEffektAura)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEffektAura)
	afx_msg void OnChangeEditTiefe();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboAuraArt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  CButton m_CheckRounded;
  afx_msg void OnBnClickedCheckRounded();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EFFEKTAURA_H__809B9E82_FF70_11D5_AC02_00104B6795C0__INCLUDED_
