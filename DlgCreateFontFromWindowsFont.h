#if !defined(AFX_DLGCREATEFONTFROMWINDOWSFONT_H__60A81D9A_0E9F_4D42_872E_30D60EA6D743__INCLUDED_)
#define AFX_DLGCREATEFONTFROMWINDOWSFONT_H__60A81D9A_0E9F_4D42_872E_30D60EA6D743__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCreateFontFromWindowsFont.h : header file
//

#include <Grafik/Font.h>



class CDlgCreateFontFromWindowsFont : public CDialog
{
// Construction
public:
	CDlgCreateFontFromWindowsFont(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCreateFontFromWindowsFont)
	enum { IDD = IDD_DIALOG_CREATE_FONT_FROM_WIN };
	CComboBox	m_ComboDepth;
	CEdit	m_EditFontWidth;
	CEdit	m_EditTextSize;
	CComboBox	m_ComboFont;
	//}}AFX_DATA


  GR::Font*         m_pFont;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCreateFontFromWindowsFont)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCreateFontFromWindowsFont)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCREATEFONTFROMWINDOWSFONT_H__60A81D9A_0E9F_4D42_872E_30D60EA6D743__INCLUDED_)
