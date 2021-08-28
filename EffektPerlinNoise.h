#if !defined(AFX_EFFEKTPERLINNOISE_H__E09E1852_4FA8_4DA6_9AAD_BCA3937E95F2__INCLUDED_)
#define AFX_EFFEKTPERLINNOISE_H__E09E1852_4FA8_4DA6_9AAD_BCA3937E95F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektPerlinNoise.h : header file
//

#include "resource.h"

#include "EffektTemplate.h"


/////////////////////////////////////////////////////////////////////////////
// CEffektPerlinNoise dialog

class CEffektPerlinNoise : public EffektTemplate
{
// Construction
public:
	CEffektPerlinNoise(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEffektPerlinNoise)
	enum { IDD = IDD_EFFEKT_PERLINNOISE };
	CEdit	m_EditScale;
	CEdit	m_EditSteps;
	CEdit	m_EditSeed;
	CButton	m_CheckRedInverse;
	CButton	m_CheckRed;
	CButton	m_CheckGreenInverse;
	CButton	m_CheckGreen;
	CButton	m_CheckBlueInverse;
	CButton	m_CheckBlue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffektPerlinNoise)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

  void DoEffekt();

  // Implementation
  protected:

    int   m_iSteps,
          m_iSeed;

    float m_fScale;

    bool  m_bGreen,
          m_bGreenInverse,
          m_bRed,
          m_bRedInverse,
          m_bBlue,
          m_bBlueInverse;

	// Generated message map functions
	//{{AFX_MSG(CEffektPerlinNoise)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditSeed();
	afx_msg void OnChangeEditSteps();
	afx_msg void OnCheckBlue();
	afx_msg void OnCheckBlueInverse();
	afx_msg void OnCheckGreen();
	afx_msg void OnCheckGreenInverse();
	afx_msg void OnCheckRed();
	afx_msg void OnCheckRedInverse();
	afx_msg void OnChangeEditScale();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFEKTPERLINNOISE_H__E09E1852_4FA8_4DA6_9AAD_BCA3937E95F2__INCLUDED_)
