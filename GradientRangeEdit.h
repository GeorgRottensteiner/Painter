#if !defined(AFX_GRADIENTRANGEEDIT_H__E7AA1AE5_845C_4A53_8EF4_92F8C235D722__INCLUDED_)
#define AFX_GRADIENTRANGEEDIT_H__E7AA1AE5_845C_4A53_8EF4_92F8C235D722__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GradientRangeEdit.h : header file
//

#include <MFC/RangeEdit.h>

/////////////////////////////////////////////////////////////////////////////
// CGradientRangeEdit window

class CGradientRangeEdit : public CRangeEdit
{
// Construction
public:
	CGradientRangeEdit();

protected:

    HBITMAP         m_hbmColorBar;

    DWORD           m_dwColorGradient1,
                    m_dwColorGradient2;

    COLORREF        GetColor( int iPos );

// Operations
public:

    virtual void    SetRange( int iMin, int iMax, int iStep = 1 );
    virtual void    SetRange( float fMin, float fMax, float fStep = 0.02f, int iVorKomma = -1, int iNachKomma = 2 );

    virtual void    SetPosition( float fPos );

    virtual void    DrawPopup( HDC hdc, const RECT& rcTarget );

    void            SetColors( DWORD dwColor1, DWORD dwColor2 );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGradientRangeEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGradientRangeEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGradientRangeEdit)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORRANGEEDIT_H__E7AA1AE5_845C_4A53_8EF4_92F8C235D722__INCLUDED_)
