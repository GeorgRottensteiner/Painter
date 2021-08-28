#if !defined(AFX_EFFEKTSMOOTHEN_H__5690C742_002F_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_EFFEKTSMOOTHEN_H__5690C742_002F_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektSmoothen.h : Header-Datei
//

#include "resource.h"

#include "EffektTemplate.h"
#include "afxwin.h"


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEffektSmoothen 

class CEffektSmoothen : public EffektTemplate
{

  // Konstruktion

  public:

    int             m_iMatrixValue[3][3];


	  CEffektSmoothen(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEffektSmoothen)
	enum { IDD = IDD_EFFEKT_SMOOTHEN };
		// HINWEIS: Der Klassen-Assistent f�gt hier Datenelemente ein
	//}}AFX_DATA

  void DoEffekt();

// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CEffektSmoothen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEffektSmoothen)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  CEdit m_EditMatrix11;
  CEdit m_EditMatrix21;
  CEdit m_EditMatrix31;
  CEdit m_EditMatrix12;
  CEdit m_EditMatrix22;
  CEdit m_EditMatrix32;
  CEdit m_EditMatrix13;
  CEdit m_EditMatrix23;
  CEdit m_EditMatrix33;
  afx_msg void OnEnChangeEditMatrix11();
  afx_msg void OnEnChangeEditMatrix12();
  afx_msg void OnEnChangeEditMatrix13();
  afx_msg void OnEnChangeEditMatrix21();
  afx_msg void OnEnChangeEditMatrix22();
  afx_msg void OnEnChangeEditMatrix23();
  afx_msg void OnEnChangeEditMatrix31();
  afx_msg void OnEnChangeEditMatrix32();
  afx_msg void OnEnChangeEditMatrix33();
  virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_EFFEKTSMOOTHEN_H__5690C742_002F_11D6_AC02_00104B6795C0__INCLUDED_
