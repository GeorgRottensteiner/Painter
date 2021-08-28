#if !defined(AFX_EffektContrast_H__169E7D66_FE97_11D5_B997_0001029EF128__INCLUDED_)
#define AFX_EffektContrast_H__169E7D66_FE97_11D5_B997_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektContrast.h : header file
//

#include "resource.h"

#include "EffektTemplate.h"
#include <MFC/RangeEdit.h>

/////////////////////////////////////////////////////////////////////////////
// CEffektContrast dialog

class CEffektContrast : public EffektTemplate
{
// Construction
public:
	CEffektContrast(CWnd* pParent = NULL);   // standard constructor

  int   m_iLowerBound;
  int   m_iUpperBound;

// Dialog Data
	//{{AFX_DATA(CEffektContrast)
  enum { IDD = IDD_EFFEKT_CONTRAST };
	RangeEdit	m_REditLowerBound;
  RangeEdit	m_REditUpperBound;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffektContrast)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  virtual void DoEffekt();

	// Generated message map functions
	//{{AFX_MSG(CEffektContrast)
	afx_msg void OnChangeREditLowerBound();
  afx_msg void OnChangeREditUpperBound();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EffektContrast_H__169E7D66_FE97_11D5_B997_0001029EF128__INCLUDED_)
