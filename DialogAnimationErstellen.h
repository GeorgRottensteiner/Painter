#if !defined(AFX_DIALOGANIMATIONERSTELLEN_H__E249AFC0_C09F_11D4_9D7D_0001029EF128__INCLUDED_)
#define AFX_DIALOGANIMATIONERSTELLEN_H__E249AFC0_C09F_11D4_9D7D_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogAnimationErstellen.h : Header-Datei
//

#include "resource.h"



class CDialogAnimationErstellen : public CDialog
{
// Konstruktion
public:
	CDialogAnimationErstellen(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDialogAnimationErstellen)
	enum { IDD = IDD_DIALOG_ANI };
	CComboBox	m_ComboType;
	CComboBox	m_ComboDepth;
	CEdit	m_EditTarget;
	CEdit	m_EditPalette;
	CListBox	m_ListLog;
	CListBox	m_ListFiles;
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CDialogAnimationErstellen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDialogAnimationErstellen)
	afx_msg void OnButtonCAAddFile();
	afx_msg void OnButtonCABrowse();
	afx_msg void OnButtonStartAC();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_DIALOGANIMATIONERSTELLEN_H__E249AFC0_C09F_11D4_9D7D_0001029EF128__INCLUDED_
