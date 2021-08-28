#if !defined(AFX_EFFEKTBRIGHTNESS_H__169E7D66_FE97_11D5_B997_0001029EF128__INCLUDED_)
#define AFX_EFFEKTBRIGHTNESS_H__169E7D66_FE97_11D5_B997_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektBrightness.h : header file
//

#include "resource.h"

#include "EffektTemplate.h"
#include <MFC/RangeEdit.h>



class CEffektBrightness : public EffektTemplate
{
  public:
	  CEffektBrightness(CWnd* pParent = NULL);   // standard constructor

    int   m_Value;

  // Dialog Data
	  //{{AFX_DATA(CEffektBrightness)
	  enum { IDD = IDD_EFFEKT_BRIGHTNESS };
	  RangeEdit	m_REditBrightness;
	  //}}AFX_DATA

  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CEffektBrightness)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:

    virtual void DoEffekt();

	  // Generated message map functions
	  //{{AFX_MSG(CEffektBrightness)
	  afx_msg void OnChangeREditBrightness();
	  virtual BOOL OnInitDialog();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFEKTBRIGHTNESS_H__169E7D66_FE97_11D5_B997_0001029EF128__INCLUDED_)
