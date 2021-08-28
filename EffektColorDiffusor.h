#if !defined(AFX_EFFEKTCOLORDIFFUSOR_H__CBF14ADB_377B_47ED_8D5A_83B52D6A3D83__INCLUDED_)
#define AFX_EFFEKTCOLORDIFFUSOR_H__CBF14ADB_377B_47ED_8D5A_83B52D6A3D83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektColorDiffusor.h : header file
//


#include "resource.h"

#include "EffektTemplate.h"



class CEffektColorDiffusor : public EffektTemplate
{
  public:
	  CEffektColorDiffusor(CWnd* pParent = NULL);   // standard constructor

  // Dialog Data
	  //{{AFX_DATA(CEffektColorDiffusor)
	  enum { IDD = IDD_EFFEKT_COLORDIFFUSOR };
	  CComboBox	m_ComboAusrichtung;
	  CEdit	m_EditBreite;
	  CButton	m_CheckGreen;
	  CButton	m_CheckBlue;
	  CButton	m_CheckRed;
    CButton m_RadioBoth;
    CButton m_RadioUp;
    CButton m_RadioDown;
	  CEdit	m_EditAbweichung;
	  //}}AFX_DATA

    int     m_Abweichung,
            m_Width,
            m_Ausrichtung;

    bool    m_ChangeRed,
            m_ChangeGreen,
            m_ChangeBlue;

    GR::u32 m_Direction;


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CEffektColorDiffusor)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL


    void DoEffekt();

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CEffektColorDiffusor)
	  virtual BOOL OnInitDialog();
	  afx_msg void OnChangeEditAbweichung();
	  afx_msg void OnCheckRed();
	  afx_msg void OnCheckGreen();
	  afx_msg void OnCheckBlue();
	  afx_msg void OnRadioBothDirections();
	  afx_msg void OnRadioUp();
	  afx_msg void OnRadioDown();
	  afx_msg void OnChangeEditBreite();
	  afx_msg void OnSelchangeComboAusrichtung();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFEKTCOLORDIFFUSOR_H__CBF14ADB_377B_47ED_8D5A_83B52D6A3D83__INCLUDED_)
