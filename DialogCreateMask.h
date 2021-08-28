#if !defined(AFX_DIALOGCREATEMASK_H__5938D9A3_79E4_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_DIALOGCREATEMASK_H__5938D9A3_79E4_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogCreateMask.h : Header-Datei
//
#include "resource.h"



class CDialogCreateMask : public CDialog
{
// Konstruktion
public:
	CDialogCreateMask(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDialogCreateMask)
	enum { IDD = IDD_DIALOG_CREATE_MASK };
	CButton	m_CheckInvertMask;
  CButton  m_RadioCMVoll;
  CButton  m_RadioCMWorkColor;
	//}}AFX_DATA


  bool        m_InvertMask;

  int         m_CreateMaskType;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDialogCreateMask)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDialogCreateMask)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioCmVoll();
	afx_msg void OnRadioCmWorkcolor();
	afx_msg void OnCheckCmInvertMask();
	afx_msg void OnRadioCmSelection();
	afx_msg void OnRadioCmGrayscale();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DIALOGCREATEMASK_H__5938D9A3_79E4_11D5_AC02_00104B6795C0__INCLUDED_
