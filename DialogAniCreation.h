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
		// HINWEIS: Der Klassen-Assistent f�gt hier Datenelemente ein
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(DialogAniCreation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(DialogAniCreation)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_DIALOGANICREATION_H__9CB09580_C097_11D4_AC01_00104B6795C0__INCLUDED_
