#pragma once
#include "afxwin.h"


// CDlgPToolAnimation-Dialogfeld

class CViewInfo;

class CDlgPToolAnimation : public CDialog, public INotifyMember<GR::u32>
{
	DECLARE_DYNAMIC(CDlgPToolAnimation)

public:

  CViewInfo*        m_pViewInfo;

	CDlgPToolAnimation(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CDlgPToolAnimation();

// Dialogfelddaten
	enum { IDD = IDD_PTOOL_ANIMATIONFRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
  CEdit m_EditDelay;
  CButton m_BtnPlay;
  CButton m_BtnStop;
  afx_msg void OnBnClickedButtonPlayAnim();
  afx_msg void OnBnClickedButtonStopAnim();
  virtual BOOL OnInitDialog();
  afx_msg void OnEnChangeEditDelay();

  virtual void            OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember );
  CStatic m_StaticDelay;
};
