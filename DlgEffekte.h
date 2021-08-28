#if !defined(AFX_DLGEFFEKTE_H__169E7D64_FE97_11D5_B997_0001029EF128__INCLUDED_)
#define AFX_DLGEFFEKTE_H__169E7D64_FE97_11D5_B997_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEffekte.h : header file
//

#include "resource.h"

#include "DocumentInfo.h"
#include "EffektTemplate.h"

#include <Grafik/Image.h>
#include <Grafik/Palette.h>
#include <Grafik/GfxPage.h>
#include "afxwin.h"



class CDlgEffekte : public CDialog
{
  // Construction
  public:

    enum                eEffekte;

    HANDLE              hFilterThread;

    GR::u32             LastUpdate;

    eEffekte            m_eUsedFilter,
                        m_eAppliedFilter;

    GR::u16             ZoomFaktor,
                        Inside;

    int                 PreXOffset,
                        PreYOffset,
                        PostXOffset,
                        PostYOffset,
                        MouseX,
                        MouseY;

    volatile bool       ThreadPleaseExit;
    volatile bool       ThreadRunning;

    bool                AsNewLayer;

    char                szFilter[5][6];

    DocumentInfo*       pDocInfo;

    EffektTemplate*    m_pCurrentEffekt;

    GR::Graphic::Image* pImageSource;
    GR::Graphic::Image* m_pImageMask;

    GR::Graphic::Palette* pPalette;

    GR::Graphic::GDIPage*   pPagePreFilter;
    GR::Graphic::GDIPage*   pPagePostFilter;



	  CDlgEffekte(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDlgEffekte();
    BOOL UpdateThread();

    //void DisableFilterControls();
    void ApplyFilter( eEffekte dwFilter );
    void SelectFilter( eEffekte dwFilter );


    void StopThread();

  // Dialog Data
	  //{{AFX_DATA(CDlgEffekte)
	  enum { IDD = IDD_DIALOG_EFFEKTE };
	  CTreeCtrl	m_TreeEffekte;
	  //}}AFX_DATA


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CDlgEffekte)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:


      void          AddEffekt( const GR::tChar* szDesc, eEffekte eEnum, HTREEITEM hItemParent );
      void          WrapCoord( int& X, int& Y, DWORD dwResID );

	  // Generated message map functions
	  //{{AFX_MSG(CDlgEffekte)
      afx_msg void OnButtonFilterZoomfaktor();
      afx_msg BOOL OnInitDialog();
      afx_msg void OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct );
      afx_msg void OnOK();
      afx_msg void OnCancel();
      afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
      afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
      afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	  afx_msg void OnSelchangedTreeEffekte(NMHDR* pNMHDR, LRESULT* pResult);
	  afx_msg void OnButtonRender();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  public:
  CStatic m_StaticFilterSettings;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEFFEKTE_H__169E7D64_FE97_11D5_B997_0001029EF128__INCLUDED_)
