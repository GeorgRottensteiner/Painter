#if !defined(AFX_EFFEKTPLASMA_H__96F14058_306F_4B2C_BEFC_6CD52D2FD533__INCLUDED_)
#define AFX_EFFEKTPLASMA_H__96F14058_306F_4B2C_BEFC_6CD52D2FD533__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektPlasma.h : header file
//

#include "resource.h"

#include "EffektTemplate.h"

/////////////////////////////////////////////////////////////////////////////
// CEffektPlasma dialog

class CEffektPlasma : public EffektTemplate
{
// Construction
public:
	CEffektPlasma(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEffektPlasma)
	enum { IDD = IDD_EFFEKT_PLASMA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffektPlasma)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  virtual void DoEffekt();
	// Generated message map functions
	//{{AFX_MSG(CEffektPlasma)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFEKTPLASMA_H__96F14058_306F_4B2C_BEFC_6CD52D2FD533__INCLUDED_)
