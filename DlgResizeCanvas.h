#if !defined(AFX_DLGRESIZECANVAS_H__BAE82D28_C9E3_468F_8958_B58EF4F0B00D__INCLUDED_)
#define AFX_DLGRESIZECANVAS_H__BAE82D28_C9E3_468F_8958_B58EF4F0B00D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgResizeCanvas.h : header file
//

#include "resource.h"



class DocumentInfo;

class CDlgResizeCanvas : public CDialog
{

  // Construction
  public:

	  CDlgResizeCanvas(CWnd* pParent = NULL);   // standard constructor

    DocumentInfo*       m_pDocInfo;

    int                 m_NewWidth,
                        m_NewHeight;

    bool                m_CenterH,
                        m_CenterV;

  // Dialog Data
	  //{{AFX_DATA(CDlgResizeCanvas)
	enum { IDD = IDD_DIALOG_RESIZE_CANVAS };
	CEdit	m_EditNewHeight;
	CEdit	m_EditNewWidth;
	CEdit	m_EditHoehe;
	CEdit	m_EditBreite;
	CButton	m_CheckCenterV;
	CButton	m_CheckCenterH;
	//}}AFX_DATA


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CDlgResizeCanvas)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CDlgResizeCanvas)
	  virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnEnChangeEditNeueBreite();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRESIZECANVAS_H__BAE82D28_C9E3_468F_8958_B58EF4F0B00D__INCLUDED_)
