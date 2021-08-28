#if !defined(AFX_EFFEKTCOLORIZE_H__5690C741_002F_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_EFFEKTCOLORIZE_H__5690C741_002F_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektColorize.h : Header-Datei
//

#include "resource.h"
#include "EffektTemplate.h"
#include <MFC/ColorRangeEdit.h>
#include <MFC/RangeEdit.h>



class CEffektColorize : public EffektTemplate
{
// Konstruktion
public:
	CEffektColorize(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEffektColorize)
	enum { IDD = IDD_EFFEKT_COLORIZE };
	CComboBox	m_ComboGrayMode;
	RangeEdit	m_REditSaettigung;
  ColorRangeEdit m_REditFarbwert;
	//}}AFX_DATA

  int     m_FarbWert,
          m_Saettigung,
          m_GrayMode;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEffektColorize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

  void DoEffekt();

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEffektColorize)
	afx_msg void OnChangeREditSaettigung();
  afx_msg void OnChangeREditFarbwert();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboGrayMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EFFEKTCOLORIZE_H__5690C741_002F_11D6_AC02_00104B6795C0__INCLUDED_
