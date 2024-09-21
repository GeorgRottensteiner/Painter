#pragma once
#include "afxdialogex.h"


// DlgWebPOptions dialog

class DlgWebPOptions : public CDialogEx
{
	DECLARE_DYNAMIC(DlgWebPOptions)

	public:

			GR::u32   m_SaveOption;


		DlgWebPOptions(CWnd* pParent = nullptr);   // standard constructor
		virtual ~DlgWebPOptions();

	// Dialog Data
	#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_DIALOG_WEBP_OPTIONS };
	#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
	CButton m_RadioLosless;
	CButton m_RadioDefault;
	afx_msg void OnBnClickedRadioWebpLosless();
	afx_msg void OnBnClickedRadioWebpDefault75();

	virtual BOOL OnInitDialog();
};
