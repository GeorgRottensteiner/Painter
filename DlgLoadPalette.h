#if !defined(AFX_DLGLOADPALETTE_H__4A51D305_E59B_4FA9_96D2_48BAB15E42CA__INCLUDED_)
#define AFX_DLGLOADPALETTE_H__4A51D305_E59B_4FA9_96D2_48BAB15E42CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



enum DlgLoadPaletteResult
{
  DLP_MAP_TO_PAL    = 0,
  DLP_FORCE_SET,
};



class CDlgLoadPalette : public CDialog
{
  // Construction
  public:
	  CDlgLoadPalette(CWnd* pParent = NULL);   // standard constructor

  // Dialog Data
	  //{{AFX_DATA(CDlgLoadPalette)
	  enum { IDD = IDD_DIALOG_LOAD_PALETTE };
    CButton  m_RadioMapToPal;
    CButton  m_RadioForcePal;
	  //}}AFX_DATA

    DlgLoadPaletteResult    m_Result;

    DocumentInfo*           m_pDocInfo;


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CDlgLoadPalette)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CDlgLoadPalette)
	  virtual BOOL OnInitDialog();
	  afx_msg void OnRadioMapToPal();
	  afx_msg void OnRadioForcePal();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOADPALETTE_H__4A51D305_E59B_4FA9_96D2_48BAB15E42CA__INCLUDED_)
