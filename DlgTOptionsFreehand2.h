#pragma once
#include "afxwin.h"


class CDlgTOptionsFreehand2 : public CDialog
{
	DECLARE_DYNAMIC(CDlgTOptionsFreehand2)

public:
	CDlgTOptionsFreehand2(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CDlgTOptionsFreehand2();

// Dialogfelddaten
	enum { IDD = IDD_DIALOG_TO_FREEHAND2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
  CButton m_CheckAA;
  afx_msg void OnBnClickedCheckAa();
  virtual BOOL OnInitDialog();
};
