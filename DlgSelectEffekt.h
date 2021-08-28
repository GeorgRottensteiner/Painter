#if !defined(AFX_DLGSELECTEFFEKT_H__D5F50D40_C183_480F_9D30_63F8B00F2EAF__INCLUDED_)
#define AFX_DLGSELECTEFFEKT_H__D5F50D40_C183_480F_9D30_63F8B00F2EAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "resource.h"

#include <GR/GRTypes.h>



class CDlgSelectEffekt : public CDialog
{
  public:
	  CDlgSelectEffekt(CWnd* pParent = NULL);   // standard constructor

    GR::u32   m_Effekt;

  // Dialog Data
	  //{{AFX_DATA(CDlgSelectEffekt)
	  enum { IDD = IDD_DIALOG_SELECTEFFEKT };
	  CListBox	m_ListEffekte;
	  //}}AFX_DATA


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CDlgSelectEffekt)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CDlgSelectEffekt)
	  virtual void OnOK();
	  virtual BOOL OnInitDialog();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTEFFEKT_H__D5F50D40_C183_480F_9D30_63F8B00F2EAF__INCLUDED_)
