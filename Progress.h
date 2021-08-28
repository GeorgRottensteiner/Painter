#if !defined(AFX_PROGRESS_H__BA4D3700_1152_11D4_924A_00104B6795C0__INCLUDED_)
#define AFX_PROGRESS_H__BA4D3700_1152_11D4_924A_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Progress.h : Header-Datei
//

#include "stdafx.h"

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProgress 

class CProgress : public CDialog
{
// Konstruktion
public:
	CProgress(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CProgress)
	enum { IDD = IDD_DIALOG_PROGRESS };
	CStatic	m_Prozent;
	CStatic	m_Info;
	CProgressCtrl	m_ProgressBar;
	//}}AFX_DATA

  CProgress( GR::Char* Text, int iPos, int iMax );

  void SetText( GR::Char* Text );
  void SetPosition( int iPosition, int iFull );


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CProgress)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PROGRESS_H__BA4D3700_1152_11D4_924A_00104B6795C0__INCLUDED_
