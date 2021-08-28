#if !defined(AFX_DIALOGANICREATION_H__9CB09580_C097_11D4_AC01_00104B6795C0__INCLUDED_)
#define AFX_DIALOGANICREATION_H__9CB09580_C097_11D4_AC01_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogAniCreation.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld DialogAniCreation 

class DialogAniCreation : public CDialog
{
// Konstruktion
public:
	DialogAniCreation(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(DialogAniCreation)
	enum { IDD = IDD_DIALOG_ANI };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(DialogAniCreation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(DialogAniCreation)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DIALOGANICREATION_H__9CB09580_C097_11D4_AC01_00104B6795C0__INCLUDED_
