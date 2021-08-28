#if !defined(AFX_DLGPSFILTER_H__40227713_E6AE_4D15_9462_14281A2966C0__INCLUDED_)
#define AFX_DLGPSFILTER_H__40227713_E6AE_4D15_9462_14281A2966C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CDlgPSFilter : public CDialog
{
// Construction
public:
	CDlgPSFilter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPSFilter)
	enum { IDD = IDD_DIALOG_PHOTOSHOP_FILTER };
	CTreeCtrl	m_TreeFilter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPSFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPSFilter)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkTreePsFilter(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPSFILTER_H__40227713_E6AE_4D15_9462_14281A2966C0__INCLUDED_)
