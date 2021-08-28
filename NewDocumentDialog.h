#if !defined(AFX_NEWDOCUMENTDIALOG_H__F61B1F20_B1E7_11D2_9237_00104B6795C0__INCLUDED_)
#define AFX_NEWDOCUMENTDIALOG_H__F61B1F20_B1E7_11D2_9237_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewDocumentDialog.h : Header-Datei
//

#include "resource.h"



class CNewDocumentDialog : public CDialog
{
  public:

	  CNewDocumentDialog(CWnd* pParent = NULL);   // Standardkonstruktor

    int         m_Width,
                m_Height,
                m_Frames,
                m_Type;

    GR::u8      m_Depth;


  // Dialogfelddaten
	  //{{AFX_DATA(CNewDocumentDialog)
	  enum { IDD = IDD_NEWDOCUMENT };
	  CEdit	m_EditHeight;
	  CEdit	m_EditWidth;
	  CEdit	m_EditFrames;
	  CComboBox	m_ComboType;
	  CComboBox	m_ComboDepth;
	  //}}AFX_DATA


  // Überschreibungen
	  // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	  //{{AFX_VIRTUAL(CNewDocumentDialog)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	  //}}AFX_VIRTUAL

  // Implementierung
  protected:


	  // Generierte Nachrichtenzuordnungsfunktionen
	  //{{AFX_MSG(CNewDocumentDialog)
	  virtual BOOL OnInitDialog();
	  virtual void OnOK();
	  afx_msg void OnSelchangeNewdocumentType();
	  afx_msg void OnSelchangeNewdocumentBitdepth();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_NEWDOCUMENTDIALOG_H__F61B1F20_B1E7_11D2_9237_00104B6795C0__INCLUDED_
