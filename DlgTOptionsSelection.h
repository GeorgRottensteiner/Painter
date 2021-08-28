#pragma once
#include "afxwin.h"



class CDlgTOptionsSelection : public CDialog
{
	DECLARE_DYNAMIC(CDlgTOptionsSelection)

public:
	CDlgTOptionsSelection(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTOptionsSelection();

// Dialog Data
	enum { IDD = IDD_DIALOG_TO_SELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CButton m_CheckSquare;
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedCheckSquare();
};
