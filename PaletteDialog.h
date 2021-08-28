#if !defined(AFX_PALETTEDIALOG_H__E07CB5C1_AC56_11D2_9237_00104B6795C0__INCLUDED_)
#define AFX_PALETTEDIALOG_H__E07CB5C1_AC56_11D2_9237_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaletteDialog.h : Header-Datei
//

#include "painterDoc.h"



class CPaletteDialog : public CDialog
{
  public:
	  CPaletteDialog(CWnd* pParent = NULL);   // Standardkonstruktor
    CPainterDoc*              pDoc;

    GR::Graphic::Palette*     m_pCurrentPal;

  // Dialogfelddaten
	  //{{AFX_DATA(CPaletteDialog)
	  enum { IDD = IDD_PALETTEEDIT };
		  // HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	  //}}AFX_DATA


  // Überschreibungen
	  // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	  //{{AFX_VIRTUAL(CPaletteDialog)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	  //}}AFX_VIRTUAL

  // Implementierung
  protected:

	  // Generierte Nachrichtenzuordnungsfunktionen
	  //{{AFX_MSG(CPaletteDialog)
	  afx_msg void OnPaint();
	  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	  afx_msg void OnButtonVerlauf();
	  afx_msg void OnButtonWin20();
	  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	  virtual BOOL OnInitDialog();
	  afx_msg void OnButtonShiftPal();
	  afx_msg void OnButtonGrayscale();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()

      GR::u32   m_Color1,
                m_Color2;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PALETTEDIALOG_H__E07CB5C1_AC56_11D2_9237_00104B6795C0__INCLUDED_
