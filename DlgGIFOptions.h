#pragma once


// DlgGIFOptions dialog

class DlgGIFOptions : public CDialog
{
	DECLARE_DYNAMIC(DlgGIFOptions)

public:

	bool							m_SaveASGIF89;

	DlgGIFOptions(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DlgGIFOptions();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GIF_OPTIONS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	public:
	CButton m_RadioGIF87;
	CButton m_RadioGIF89;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
