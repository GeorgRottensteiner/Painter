#if !defined(AFX_DLGSAVEIGF_H__C83DA221_293B_11D6_B997_0001029EF128__INCLUDED_)
#define AFX_DLGSAVEIGF_H__C83DA221_293B_11D6_B997_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSaveIGF.h : header file
//

#include "resource.h"




class CDlgSaveIGF : public CDialog
{
  // Construction
  public:

    bool      m_SaveMitPalette;

    bool      m_SaveMitCompressList;

    bool      m_HidePaletteCheckBox;


	  CDlgSaveIGF(CWnd* pParent = NULL);   // standard constructor

  // Dialog Data
	  //{{AFX_DATA(CDlgSaveIGF)
	  enum { IDD = IDD_DIALOG_IGF_OPTIONS };
	  CButton	m_checkSaveMitPalette;
    CButton	m_checkSaveMitCompList;
	  //}}AFX_DATA


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CDlgSaveIGF)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CDlgSaveIGF)
	  afx_msg void OnCheckIgfWithPal();
    afx_msg void OnCheckIgfWithCompList();
	  virtual BOOL OnInitDialog();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSAVEIGF_H__C83DA221_293B_11D6_B997_0001029EF128__INCLUDED_)
