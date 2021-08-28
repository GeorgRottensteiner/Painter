#pragma once
#include "afxwin.h"


// CDlgCustomizeToolBars-Dialogfeld

class CDlgCustomizeToolBars : public CDialog
{
	DECLARE_DYNAMIC(CDlgCustomizeToolBars)

public:
	CDlgCustomizeToolBars(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CDlgCustomizeToolBars();

// Dialogfelddaten
	enum { IDD = IDD_DLG_TOOLBARS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
  CCheckListBox m_ListToolBars;
  virtual BOOL OnInitDialog();
  afx_msg void OnListToolBarsCheckChange();
};
