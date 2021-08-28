#pragma once
#include "afxwin.h"



class CDlgTOptionsLine : public CDialog
{
	DECLARE_DYNAMIC(CDlgTOptionsLine)

public:
	CDlgTOptionsLine(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CDlgTOptionsLine();

// Dialogfelddaten
	enum { IDD = IDD_DIALOG_TO_LINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
  CButton m_CheckAA;
  afx_msg void OnBnClickedCheckAa();
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedCheckConnected();
  CButton m_CheckConnectedLines;
};
