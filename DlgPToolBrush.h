#if !defined(AFX_DLGPTOOLBRUSH_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_DLGPTOOLBRUSH_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolFarben.h : Header-Datei
//

#include "resource.h"

#include <Interface/INotifyMember.h>

#include <MFC/odlistbox.h>

#include <Grafik/Image.h>
#include <Grafik/GDIPage.h>


class GR::Graphic::GDIPage;



struct tBrushTip;

class CDlgPToolBrush : public CDialog, public INotifyMember<GR::u32>
{
  // Konstruktion
  public:

	  CDlgPToolBrush(CWnd* pParent = NULL);   // Standardkonstruktor

    tBrushTip*            AddBrushTip( GR::Graphic::Image* pImage );


// Dialogfelddaten
	//{{AFX_DATA(CDlgPToolBrush)
	enum { IDD = IDD_PTOOL_BRUSH };
	CButton	m_ButtonBrushForegroundType;
	CButton	m_ButtonBrushForeground;
	CButton	m_ButtonBrushBackgroundType;
	CButton	m_ButtonBrushBackground;
	//}}AFX_DATA



// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgPToolBrush)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

  // Implementierung
  protected:

    GR::Graphic::GDIPage        *m_pPageForeground,
                        *m_pPageBackground;


    void                OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember );

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgPToolBrush)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBrushForeground();
	afx_msg void OnButtonBrushBackground();
	afx_msg void OnButtonBrushForegroundtype();
	afx_msg void OnButtonBrushBackgroundtype();
	afx_msg void OnButtonSwapBrush();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDestroy();
  afx_msg void OnListBrushTipSelChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  CODListBox m_ListBrushTips;
protected:
  void RedrawPreviews(void);
  virtual void OnOK();
  virtual void OnCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGPTOOLBRUSH_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_
