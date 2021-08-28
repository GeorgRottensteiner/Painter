#if !defined(AFX_DIALOGVERLAUF_H__6A8D49E2_68A3_11D5_9D7D_0001029EF128__INCLUDED_)
#define AFX_DIALOGVERLAUF_H__6A8D49E2_68A3_11D5_9D7D_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogVerlauf.h : Header-Datei
//

#include "resource.h"

#include "Settings.h"

#include <Grafik/Image.h>

#include <MFC\AngleWnd.h>
#include <mfc\gradientctrl.h>
#include <mfc\coloredbutton.h>
#include <mfc\rangeedit.h>
#include "afxwin.h"



class CDialogVerlauf : public CDialog
{
// Konstruktion
public:
	CDialogVerlauf(CWnd* pParent = NULL);   // Standardkonstruktor

  GR::Graphic::GDIPage*   m_pPagePreview;

  GR::Graphic::Image*     m_pImageTarget;

  PainterBrush      m_Brush;

  bool              m_DoNotUpdate;

  bool              m_RedrawPage;

  GR::u32           m_PegColor;


  void              RefillCombo();


// Dialogfelddaten
	//{{AFX_DATA(CDialogVerlauf)
	enum { IDD = IDD_DIALOG_GRADIENT };
	CStatic	m_StaticPreview;
	CSpinButtonCtrl	m_SpinAngle;
	CEdit	m_EditAngle;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDialogVerlauf)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDialogVerlauf)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnChangeEditGradientAngle();
  afx_msg void OnChangeAngleWnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  CAngleWnd m_AngleWnd;
protected:
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
  CGradientCtrl m_GradientCtrl;
  afx_msg void OnBnClickedButtonAddPeg();
  CColoredButton m_ButtonPegColor;
  afx_msg void OnBnClickedButtonColor();
  RangeEdit m_REditPegPos;
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  CComboBox m_ComboPresetGradients;
  CEdit m_EditPresetName;
  afx_msg void OnBnClickedButtonSaveGradient();
  afx_msg void OnCbnSelchangeComboPresetGradients();
protected:
//  virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
  CComboBox m_ComboGradientType;
  afx_msg void OnCbnSelchangeComboGradientType();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DIALOGVERLAUF_H__6A8D49E2_68A3_11D5_9D7D_0001029EF128__INCLUDED_
