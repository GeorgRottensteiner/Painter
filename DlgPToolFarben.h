#if !defined(AFX_DLGPTOOLFARBEN_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_DLGPTOOLFARBEN_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolFarben.h : Header-Datei
//

#include "stdafx.h"

#include "resource.h"

#include <Interface/INotifyMember.h>

#include <GR/GRTypes.h>


namespace GR
{
  namespace Graphic
  {
    class GDIPage;
  }
}



class CViewInfo;



class CDlgPToolFarben : public CDialog, public INotifyMember<GR::u32>
{
// Konstruktion
public:
	CDlgPToolFarben(CWnd* pParent = NULL);   // Standardkonstruktor

  GR::Graphic::GDIPage                *pPage;
  CViewInfo                   *m_pViewInfo;
  CWnd                        wndPalette;


  // BAUSTELLE, wir sind ja jetzt Observer
  void UpdateControls();

  virtual void            OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember );

// Dialogfelddaten
	//{{AFX_DATA(CToolFarben)
	enum { IDD = IDD_PTOOL_FARBEN };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CToolFarben)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

  CRect         m_ColorSwitchRect;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CToolFarben)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove( UINT nFlags, CPoint point );
  afx_msg void OnDestroy();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


    COLORREF GetQuadrantColor( int iQuadrant, unsigned char ucHCPalette, int iX, int iY );
    DWORD GetDisplayBitDepth();
    DWORD MakeColor( DWORD dwRed, DWORD dwGreen, DWORD dwBlue, WORD wIntensity );

    void DoLButtonDown( UINT nFlags, CPoint point );
    void DoRButtonDown( UINT nFlags, CPoint point );

    virtual void OnOK();
    virtual void OnCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGPTOOLFARBEN_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_
