#if !defined(AFX_CONVERTOPTIONS_H__A9BA5D80_D747_11D4_AC01_00104B6795C0__INCLUDED_)
#define AFX_CONVERTOPTIONS_H__A9BA5D80_D747_11D4_AC01_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
// ConvertOptions.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CConvertOptions 

namespace CONVERT
{
  namespace TYPE
  {
    const DWORD STRAIGHT        = 0;
    const DWORD DITHER          = 1;
  };
};

class CConvertOptions : public CDialog
{
// Konstruktion
public:
	CConvertOptions(CWnd* pParent = NULL);   // Standardkonstruktor


  DWORD       m_dwConvertMode;

// Dialogfelddaten
	//{{AFX_DATA(CConvertOptions)
	enum { IDD = IDD_DIALOG_CONVERT_OPTIONS };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CConvertOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CConvertOptions)
	afx_msg void OnRadioConvertStraight();
	afx_msg void OnRadioConvertDither();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CONVERTOPTIONS_H__A9BA5D80_D747_11D4_AC01_00104B6795C0__INCLUDED_
