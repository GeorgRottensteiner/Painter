#if !defined(AFX_DIALOGBATCHCONVERSION_H__C843CBA0_0345_11D4_924A_00104B6795C0__INCLUDED_)
#define AFX_DIALOGBATCHCONVERSION_H__C843CBA0_0345_11D4_924A_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogBatchConversion.h : Header-Datei
//

#include "resource.h"



class CDialogBatchConversion : public CDialog
{
// Konstruktion
public:
	CDialogBatchConversion(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDialogBatchConversion)
	enum { IDD = IDD_DIALOG_BATCH };
	CEdit	m_EditPalette;
	CEdit	m_EditTargetPath;
	CComboBox	m_ComboDepth;
	CComboBox	m_ComboType;
	CListBox	m_ListLog;
	CListBox	m_BatchFileList;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDialogBatchConversion)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDialogBatchConversion)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBatchAddFile();
	afx_msg void OnButtonBatchStart();
	afx_msg void OnButtonBatchRemovefile();
	afx_msg void OnButtonBatchRemoveAllFiles();
	afx_msg void OnButtonBatchBrowsePalette();
	afx_msg void OnButtonBatchBrowseTargetPath();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DIALOGBATCHCONVERSION_H__C843CBA0_0345_11D4_924A_00104B6795C0__INCLUDED_
