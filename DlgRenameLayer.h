#if !defined(AFX_DLGRENAMELAYER_H__677F152E_6C2E_432E_981E_72A5662E6A95__INCLUDED_)
#define AFX_DLGRENAMELAYER_H__677F152E_6C2E_432E_981E_72A5662E6A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning( disable : 4786 )
#include <string>



class CDlgRenameLayer : public CDialog
{
  public:
	  CDlgRenameLayer(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
	  //{{AFX_DATA(CDlgRenameLayer)
	  enum { IDD = IDD_DIALOG_RENAME_LAYER };
	  CEdit	m_EditLayerName;
	  //}}AFX_DATA

    GR::String            m_LayerName;


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CDlgRenameLayer)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CDlgRenameLayer)
	  virtual BOOL OnInitDialog();
	  virtual void OnOK();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRENAMELAYER_H__677F152E_6C2E_432E_981E_72A5662E6A95__INCLUDED_)
