#if !defined(AFX_DLGTOPTIONSFILL_H__E18B58A2_036E_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_DLGTOPTIONSFILL_H__E18B58A2_036E_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTOptionsFill.h : Header-Datei
//

#include "resource.h"

#include <MFC\RangeEdit.h>
#include "afxwin.h"


class CDlgTOptionsFill : public CDialog
{
  // Konstruktion
  public:

	  CDlgTOptionsFill(CWnd* pParent = NULL);   // Standardkonstruktor

  // Dialogfelddaten
	  //{{AFX_DATA(CDlgTOptionsFill)
	  enum { IDD = IDD_DIALOG_TO_FILL };
	  CComboBox	m_ComboFillMode;
    RangeEdit  m_REToleranz;
	  //}}AFX_DATA


  // Überschreibungen
	  // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	  //{{AFX_VIRTUAL(CDlgTOptionsFill)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	  //}}AFX_VIRTUAL

  // Implementierung
  protected:

	  // Generierte Nachrichtenzuordnungsfunktionen
	  //{{AFX_MSG(CDlgTOptionsFill)
	  virtual BOOL OnInitDialog();
	  afx_msg void OnSelchangeComboFillMode();
    afx_msg void OnChangeREditToleranz();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnBnClickedCheckGradientClip();
    CButton m_CheckClipGradient;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGTOPTIONSFILL_H__E18B58A2_036E_11D6_AC02_00104B6795C0__INCLUDED_
