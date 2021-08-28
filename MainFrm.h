#if !defined(AFX_MAINFRM_H__6E63B466_8C86_11D2_9F65_004033315771__INCLUDED_)
#define AFX_MAINFRM_H__6E63B466_8C86_11D2_9F65_004033315771__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable:4786 )
#include <Interface/INotifyMember.h>

#include "GRControlBar.h"
#include "PainterEnums.h"

#include "DlgPToolFarben.h"
#include "DlgPToolBrush.h"
#include "DlgPToolLayer.h"
#include "DlgPToolFunctionOptions.h"
#include "DlgPToolBrowser.h"
#include "DlgPToolTileTester.h"
#include "DlgPToolAnimation.h"

#include <string>
#include <map>


class CBasicToolDlg;

class CMainFrame : public CMDIFrameWnd, public INotifyMember<GR::u32>
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attribute
public:

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMainFrame)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

  typedef std::map<GR::tString,GRControlBar*>   tMapControlBars;

  CStatusBar                m_wndStatusBar;

  CComboBox                 m_ComboZoom;

  CDlgPToolFarben           m_dlgFarben;

  CDlgPToolBrush            m_dlgBrush;
  
  CDlgPToolLayer            m_dlgLayer;

  CDlgPToolBrowser          m_dlgBrowser;

  CDlgPToolFunctionOptions  m_dlgFunctionOptions;

  CDlgPToolTileTester       m_dlgTileTester;

  CDlgPToolAnimation        m_dlgAnimation;

  std::list<tPainterFunction>   m_listFunctions;

  tMapControlBars           m_mapControlBars;

  bool                      m_SessionEnding;



  void                      OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember );

  void DockControlBarLeftOf( CControlBar* Bar, CControlBar* LeftOf );


  protected:  // Eingebundene Elemente der Steuerleiste


    void                    AddFunction( const GR::tString& strGroup, const GR::tString& strName, DWORD dwIconID, DWORD_PTR dwCommandID,
                                         eControlBarControlType eType = CBCT_BUTTON ); 

    void                    LoadBarSetup( const char* szProfileName );
    void                    SaveBarSetup( const char* szProfileName );

// Generierte Message-Map-Funktionen
protected:
	
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBearbeitenPasteasnewimage();
	afx_msg void OnBearbeitenEinstellungen();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnUpdateMenuUndo(CCmdUI* pCmdUI);
	afx_msg void OnMenuUndo();
	afx_msg void OnBearbeitenDokumenteinstellungen();
	afx_msg void OnUpdateBearbeitenDokumentEinstellungen(CCmdUI* pCmdUI);
	afx_msg void OnIdViewToggleToolWindows();
	afx_msg void OnMenuToggleSnap();
	afx_msg void OnUpdateMenuToggleSnap(CCmdUI* pCmdUI);
	afx_msg void OnSpecialSelectionVonMaske();
	afx_msg void OnSpecialSelectionInvert();
	afx_msg void OnMenuRedo();
	afx_msg void OnUpdateMenuRedo(CCmdUI* pCmdUI);
  afx_msg LRESULT OnUser( WPARAM wParam, LPARAM lParam );
	afx_msg void OnSpecialRemoveSelection();
	afx_msg void OnMenuRotateFree();
	afx_msg void OnMenuCreateFont();
	afx_msg void OnMenuColorInvert();
	afx_msg void OnSpecialReplace();
	afx_msg void OnMenuScriptTest();
	afx_msg void OnMenuPhotoshopFilter();
	afx_msg void OnSpecialSelectionFromDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnDestroy();
  virtual BOOL DestroyWindow();
  afx_msg void OnMenuToolbars();
  afx_msg void OnBearbeitenIsometrisieren();
  afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
  afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
protected:
  virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

  afx_msg void OnEndSession( BOOL bEnding );
  afx_msg BOOL OnQueryEndSession();
public:
  afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
  afx_msg void OnSpecialExchangecolor();
  afx_msg void OnFileCloseAll();
  afx_msg void OnSpecialSafetySave();
  afx_msg void OnRestoreSafetySave();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MAINFRM_H__6E63B466_8C86_11D2_9F65_004033315771__INCLUDED_)
