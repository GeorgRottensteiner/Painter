#if !defined(AFX_JPEGSAVEOPTIONS_H__45ED4B40_6C8F_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_JPEGSAVEOPTIONS_H__45ED4B40_6C8F_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JpegSaveOptions.h : Header-Datei
//

#include "resource.h"

#include <GR/GRTypes.h>



class CJpegSaveOptions : public CDialog
{
  public:
    GR::u32   m_SaveOption;

	  CJpegSaveOptions(CWnd* pParent = NULL);   // Standardkonstruktor

  // Dialogfelddaten
	  //{{AFX_DATA(CJpegSaveOptions)
	  enum { IDD = IDD_DIALOG_JPEG_OPTIONS };
    CButton  m_RadioJPGFast;
    CButton  m_RadioJPGAccurate;
    CButton  m_RadioJPGPerfect;
    CButton  m_RadioJPGGood;
    CButton  m_RadioJPGNormal;
    CButton  m_RadioJPGAverage;
    CButton  m_RadioJPGBad;
		  // HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	  //}}AFX_DATA


  // Überschreibungen
	  // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	  //{{AFX_VIRTUAL(CJpegSaveOptions)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	  //}}AFX_VIRTUAL

  // Implementierung
  protected:

	  // Generierte Nachrichtenzuordnungsfunktionen
	  //{{AFX_MSG(CJpegSaveOptions)
	  virtual BOOL OnInitDialog();
	  afx_msg void OnRadioJpegFast();
	  afx_msg void OnRadioJpegAccurate();
	  afx_msg void OnRadioJpegDurchschnitt();
	  afx_msg void OnRadioJpegGut();
	  afx_msg void OnRadioJpegNormal();
	  afx_msg void OnRadioJpegPerfekt();
	  afx_msg void OnRadioJpegSchlecht();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_JPEGSAVEOPTIONS_H__45ED4B40_6C8F_11D5_AC02_00104B6795C0__INCLUDED_
