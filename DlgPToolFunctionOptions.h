#if !defined(AFX_DLGPTOOLFUNCTIONS_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_DLGPTOOLFUNCTIONS_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolFarben.h : Header-Datei
//

#include "resource.h"

#include <Interface/INotifyMember.h>

#include <GR/GRTypes.h>



class CDlgPToolFunctionOptions : public CDialog, public INotifyMember<GR::u32>
{
// Konstruktion
public:
	CDlgPToolFunctionOptions(CWnd* pParent = NULL);   // Standardkonstruktor



    virtual void        OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember );

// Dialogfelddaten
	//{{AFX_DATA(CToolFarben)
	enum { IDD = IDD_PTOOL_FUNCTIONOPTIONS };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA



// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgPToolFunctionOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:


  CDialog     *m_pSubDialog;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgPToolFunctionOptions)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  virtual void OnOK();
  virtual void OnCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGPTOOLFUNCTIONS_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_
