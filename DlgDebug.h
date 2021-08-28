#if !defined(AFX_DLGDEBUG_H__057C2E67_1E36_11D6_B997_0001029EF128__INCLUDED_)
#define AFX_DLGDEBUG_H__057C2E67_1E36_11D6_B997_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDebug.h : header file
//

#include "resource.h"

#pragma warning ( disable:4786 )
#include <string>
/////////////////////////////////////////////////////////////////////////////
// CDlgDebug dialog

class CDlgDebug : public CDialog
{
// Construction
public:
	CDlgDebug(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDebug)
	enum { IDD = IDD_DIALOG_DEBUG };
	CListBox	m_ListDebug;
	//}}AFX_DATA


  void Log( const char *szText );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDebug)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDebug)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonEmpty();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDEBUG_H__057C2E67_1E36_11D6_B997_0001029EF128__INCLUDED_)
