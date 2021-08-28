#if !defined(AFX_EFFEKTTEMPLATE_H__169E7D67_FE97_11D5_B997_0001029EF128__INCLUDED_)
#define AFX_EFFEKTTEMPLATE_H__169E7D67_FE97_11D5_B997_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffektTemplate.h : header file
//

#include "resource.h"

#include "MaskedContextDescriptor.h"


class GR::Graphic::ContextDescriptor;
class CDlgEffekte;



class EffektTemplate : public CDialog
{
  public:


	  EffektTemplate( DWORD dwID = 0,CWnd* pParent = NULL);   // standard constructor
    virtual ~EffektTemplate();

	  bool                        m_ExitRequest,
                                m_DoNotUpdate;

    CMaskedContextDescriptor*   m_pCDSource;
    CMaskedContextDescriptor*   m_pCDLastFrame;
    CMaskedContextDescriptor*   m_pCDTarget;

    CDlgEffekte*                m_pDlg;

    DWORD                       m_FramePos;

	  virtual void CleanUp();
	  virtual void Init();
	  virtual void DoEffekt() = 0;
    virtual void RestartRendering();

	  virtual void OnOK();
	  virtual void OnCancel();

    void NextFrame();

    virtual DWORD GetID()
    {
      return m_dwRessourceID;
    }


  // Dialog Data
	  //{{AFX_DATA(EffektTemplate)
	  enum { IDD = IDD_EFFEKT_TEMPLATE };
		  // NOTE: the ClassWizard will add data members here
	  //}}AFX_DATA


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(EffektTemplate)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  private:

    DWORD     m_dwRessourceID;

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(EffektTemplate)
		  // NOTE: the ClassWizard will add member functions here
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFEKTTEMPLATE_H__169E7D67_FE97_11D5_B997_0001029EF128__INCLUDED_)
