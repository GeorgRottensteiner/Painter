#if !defined(AFX_DLGREPLACECOLOR_H__1BD8A18A_B13D_48FA_8351_1B20454F3FBE__INCLUDED_)
#define AFX_DLGREPLACECOLOR_H__1BD8A18A_B13D_48FA_8351_1B20454F3FBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CDlgReplaceColor : public CDialog
{
// Construction
public:
	CDlgReplaceColor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgReplaceColor)
	enum { IDD = IDD_DIALOG_REPLACE_COLOR };
	CButton	m_ButtonColor2;
	CButton	m_ButtonColor1;
	//}}AFX_DATA

    GR::u32         m_Color1,
                    m_Color2;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgReplaceColor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgReplaceColor)
	virtual void OnOK();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREPLACECOLOR_H__1BD8A18A_B13D_48FA_8351_1B20454F3FBE__INCLUDED_)
