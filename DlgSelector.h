#if !defined(AFX_DLGSELECTOR_H__C32D4766_5112_46E9_8A6D_202E45A24E3C__INCLUDED_)
#define AFX_DLGSELECTOR_H__C32D4766_5112_46E9_8A6D_202E45A24E3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <MFC/RangeEdit.h>



class CDlgSelector : public CDialog
{
  public:

    enum eRule
    {
      R_R_BIGGER,
      R_R_LESSER,
      R_G_BIGGER,
      R_G_LESSER,
      R_B_BIGGER,
      R_B_LESSER,
      R_COLOR_1,
      R_COLOR_2,
    };

    enum eMode
    {
      M_AND,
      M_OR,
      M_XOR,
      M_NOT,
    };

    struct tRule
    {
      eRule     m_Rule;
      eMode     m_Mode;
      int       m_Param;
    };

    std::list<tRule>      m_Rules;

    int                   m_Param;

	  CDlgSelector(CWnd* pParent = NULL);   // standard constructor

  // Dialog Data
	  //{{AFX_DATA(CDlgSelector)
	  enum { IDD = IDD_DIALOG_SELECT };
	  CComboBox	m_ComboRule;
	  CComboBox	m_ComboAndOrXOR;
	  CListCtrl	m_ListRules;
    RangeEdit m_REditValue;
	  //}}AFX_DATA


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CDlgSelector)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CDlgSelector)
		  afx_msg void OnChangeREditValue();
	  virtual BOOL OnInitDialog();
	  afx_msg void OnButtonAdd();
	  virtual void OnOK();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTOR_H__C32D4766_5112_46E9_8A6D_202E45A24E3C__INCLUDED_)
