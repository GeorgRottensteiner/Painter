#if !defined(AFX_COLORPICKER_H__B1D07D00_179E_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_COLORPICKER_H__B1D07D00_179E_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorPicker.h : Header-Datei
//

#include "resource.h"

#include <MFC/GradientRangeEdit.h>

#include <MFC\IconButton.h>
#include <MFC\RangeEdit.h>


namespace GR
{
  namespace Graphic
  {
    class Image;
    class GDIPage;
  };
};

class CColorPicker : public CDialog
{
// Konstruktion
public:

  enum eColorChangedBy
  {
    CC_BY_RGB       = 1,
    CC_BY_HUE,
    CC_BY_BUTTON,
    CC_BY_EDIT_RGB,
    CC_BY_EDIT_HSL,
    CC_BY_EDIT_HTML,
    CC_BY_GRADIENT,
  };

	CColorPicker(CWnd* pParent = NULL);   // Standardkonstruktor

  CButton         buttonColor[6][6],
                  buttonCustomColor[6][6],
                  m_buttonGradient1,
                  m_buttonGradient2;

  GR::u32         m_FixedColor[6][6],
                  m_CustomColor[6][6];

  CWnd            wndColorWheel;

  GR::Graphic::GDIPage*   m_pPage;

  GR::Graphic::Image*     m_pImageWheel;

  GR::u32         m_WorkColor,
                  m_StartColor,
                  m_ChosenBoxColor,

                  m_GradientColor1,
                  m_GradientColor2;

  bool            m_CaptureColorBox,
                  m_CaptureColorWheel,
                  m_DoNotUpdate;

  bool            m_PickMode;

  int             m_RectPosX,
                  m_RectPosY,
                  m_WheelPos;

// Dialogfelddaten
	//{{AFX_DATA(CColorPicker)
	enum { IDD = IDD_DIALOG_COLOR_PICKER };
	CButton	m_ButtonOldColor;
	CIconButton	m_ButtonPicker;
	CEdit	m_EditHTML;
	RangeEdit	m_REditSat;
	RangeEdit	m_REditLight;
	RangeEdit	m_REditHue;
	CButton	m_ButtonNewColor;
	RangeEdit	m_REditRot;
	RangeEdit	m_REditGruen;
	RangeEdit	m_REditBlau;
  CGradientRangeEdit	m_REditVerlauf;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CColorPicker)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
protected:


  void ChangeColor( DWORD dwChangeType );
  void DrawCross( int iX, int iY );


	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CColorPicker)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	afx_msg void OnChangeREditRot();
	afx_msg void OnChangeREditGruen();
	afx_msg void OnChangeREditBlau();
	afx_msg void OnChangeREditHue();
	afx_msg void OnChangeREditSaturation();
	afx_msg void OnChangeREditLight();
  afx_msg void OnChangeREditVerlauf();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnPickerPushed();
	afx_msg void OnButtonCpOldColor();
	afx_msg void OnChangeEditCpHtml();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButtonCpVerlauf2();
  afx_msg void OnBnClickedButtonCpVerlauf1();
};

DWORD GetWheelColor( DWORD dwPos );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_COLORPICKER_H__B1D07D00_179E_11D5_AC02_00104B6795C0__INCLUDED_
