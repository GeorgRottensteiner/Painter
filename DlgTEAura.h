#if !defined(AFX_DLGTEAURA_H__3074F9A5_BBA1_43DD_AB7D_60D4C9C62E9D__INCLUDED_)
#define AFX_DLGTEAURA_H__3074F9A5_BBA1_43DD_AB7D_60D4C9C62E9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTEAura.h : header file
//

#include "resource.h"



class CTextEffekt;

class CDlgTEAura : public CDialog
{
// Construction
public:
	CDlgTEAura(CWnd* pParent = NULL);   // standard constructor

    CTextEffekt     *m_pEffekt;

// Dialog Data
	//{{AFX_DATA(CDlgTEAura)
	enum { IDD = IDD_DIALOG_TE_AURA };
	CButton	m_ButtonFarbe;
	CEdit	m_EditAuraOffset;
	CEdit	m_EditAuraSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTEAura)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTEAura)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnButtonTeFarbe();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTEAURA_H__3074F9A5_BBA1_43DD_AB7D_60D4C9C62E9D__INCLUDED_)
