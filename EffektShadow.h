#if !defined(AFX_EFFEKTSHADOW_H__ACB11854_5890_4394_BCC4_71787E513E25__INCLUDED_)
#define AFX_EFFEKTSHADOW_H__ACB11854_5890_4394_BCC4_71787E513E25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektShadow.h : header file
//
#include "resource.h"

#include "EffektTemplate.h"
#include <MFC\RangeEdit.h>
#include "p:\common\mfc\rangeedit.h"

/////////////////////////////////////////////////////////////////////////////
// CEffektShadow dialog

class CEffektShadow : public EffektTemplate
{
// Construction
public:
	CEffektShadow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEffektShadow)
	enum { IDD = IDD_EFFEKT_SHADOW };
		RangeEdit  m_REditShadowRange;
    RangeEdit  m_REditShadowOffsetX;
    RangeEdit  m_REditShadowOffsetY;
	//}}AFX_DATA

    int         m_iShadowRange,
                m_iShadowOffsetX,
                m_iShadowOffsetY;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffektShadow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

  virtual void DoEffekt();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEffektShadow)
	virtual BOOL OnInitDialog();
  virtual void OnChangeREditShadowRange();
  virtual void OnChangeREditShadowOffsetX();
  virtual void OnChangeREditShadowOffsetY();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFEKTSHADOW_H__ACB11854_5890_4394_BCC4_71787E513E25__INCLUDED_)
