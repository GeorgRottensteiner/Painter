#if !defined(AFX_DLGFREEROTATE_H__470D1B3A_CB4A_439E_8F26_BCB07738F637__INCLUDED_)
#define AFX_DLGFREEROTATE_H__470D1B3A_CB4A_439E_8F26_BCB07738F637__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFreeRotate.h : header file
//

#include "resource.h"



/////////////////////////////////////////////////////////////////////////////
// CDlgFreeRotate dialog

class CDlgFreeRotate : public CDialog
{
// Construction
public:
	CDlgFreeRotate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFreeRotate)
	enum { IDD = IDD_DIALOG_ROTATE };
	CButton	m_CheckAllowResize;
  CButton	m_RadioRotateCW;
  CButton	m_RadioRotateCCW;
	CEdit	m_EditWinkel;
	//}}AFX_DATA

  float     m_Angle;

  bool      m_AllowResize,
            m_RotateCW;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFreeRotate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFreeRotate)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFREEROTATE_H__470D1B3A_CB4A_439E_8F26_BCB07738F637__INCLUDED_)
