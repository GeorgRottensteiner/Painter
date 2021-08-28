#pragma once
#include "afxwin.h"


// CDlgRename-Dialogfeld

class CDlgRename : public CDialog
{
	DECLARE_DYNAMIC(CDlgRename)

public:

  GR::String        m_Name,
                    m_Caption;


  CDlgRename( const GR::String& Name, const GR::String& Caption, CWnd* pParent = NULL );   // Standardkonstruktor
	virtual ~CDlgRename();

// Dialogfelddaten
	enum { IDD = IDD_DIALOG_RENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
  CStatic m_StaticOldDesc;
  CEdit m_EditOldName;
  CStatic m_StaticNewDesc;
  CEdit m_EditNewName;
  afx_msg void OnBnClickedOk();
  virtual BOOL OnInitDialog();
};
