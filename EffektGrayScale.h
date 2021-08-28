#if !defined(AFX_EFFEKTGRAYSCALE_H__809B9E83_FF70_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_EFFEKTGRAYSCALE_H__809B9E83_FF70_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektGrayScale.h : Header-Datei
//

#include "resource.h"
#include "EffektTemplate.h"
#include <MFC/RangeEdit.h>




/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEffektGrayScale 

class CEffektGrayScale : public EffektTemplate
{
// Konstruktion
public:
	CEffektGrayScale(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEffektGrayScale)
	enum { IDD = IDD_EFFEKT_GRAYSCALE };
	CComboBox	m_ComboGrayMode;
	RangeEdit	m_REditGrayScale;
	int		m_iGrayValue;
  int   m_iGrayMode;
	//}}AFX_DATA

  void DoEffekt();


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEffektGrayScale)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEffektGrayScale)
	afx_msg void OnChangeEditGrayscale();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboGrayMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EFFEKTGRAYSCALE_H__809B9E83_FF70_11D5_AC02_00104B6795C0__INCLUDED_
