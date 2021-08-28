#if !defined(AFX_EFFEKTBEVEL_H__39872F9F_4159_4346_9ADD_D0393A3E3B66__INCLUDED_)
#define AFX_EFFEKTBEVEL_H__39872F9F_4159_4346_9ADD_D0393A3E3B66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektBevel.h : header file
//

#include "EffektTemplate.h"

#include "resource.h"



class CEffektBevel : public EffektTemplate
{
  public:
	  CEffektBevel(CWnd* pParent = NULL);   // standard constructor

  // Dialog Data
	  //{{AFX_DATA(CEffektBevel)
	  enum { IDD = IDD_EFFEKT_BEVEL };
		  // NOTE: the ClassWizard will add data members here
	  //}}AFX_DATA

    void DoEffekt();


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CEffektBevel)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CEffektBevel)
	  virtual BOOL OnInitDialog();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFEKTBEVEL_H__39872F9F_4159_4346_9ADD_D0393A3E3B66__INCLUDED_)
