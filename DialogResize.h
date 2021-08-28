#if !defined(AFX_DIALOGRESIZE_H__0CEB8201_BC1D_11D2_9237_00104B6795C0__INCLUDED_)
#define AFX_DIALOGRESIZE_H__0CEB8201_BC1D_11D2_9237_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogResize.h : Header-Datei
//

#include <MFC\RangeEdit.h>



class CDialogResize : public CDialog
{
// Konstruktion
public:
	CDialogResize(CWnd* pParent = NULL);   // Standardkonstruktor

  DocumentInfo*        m_pDocInfo;

  int                   Width,
                        Height,
                        ResizeType,
                        PercentageX,
                        PercentageY;

  bool                  m_DoNotUpdate;

  bool                  KeepAspectRatio;



  #define       RESIZE_PIXEL      0
  #define       RESIZE_BILINEAR   1
// Dialogfelddaten
	//{{AFX_DATA(CDialogResize)
	enum { IDD = IDD_BEARBEITEN_RESIZE };
	CComboBox	m_ComboResize;
	RangeEdit	m_REditNewWidth;
	RangeEdit	m_REditNewHeight;
	RangeEdit	m_REditNewWidthP;
	RangeEdit	m_REditNewHeightP;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDialogResize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDialogResize)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnResizeCheckKeepAspectratio();
	afx_msg void OnEditchangeComboResizeArt();
  afx_msg void OnChangeREditNewWidth();
  afx_msg void OnChangeREditNewHeight();
  afx_msg void OnChangeREditNewWidthP();
  afx_msg void OnChangeREditNewHeightP();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnCbnSelchangeComboResizeArt();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DIALOGRESIZE_H__0CEB8201_BC1D_11D2_9237_00104B6795C0__INCLUDED_
