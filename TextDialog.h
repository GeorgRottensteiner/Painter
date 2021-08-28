#if !defined(AFX_TEXTDIALOG_H__E18AAAC0_4A99_11D4_924A_00104B6795C0__INCLUDED_)
#define AFX_TEXTDIALOG_H__E18AAAC0_4A99_11D4_924A_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextDialog.h : Header-Datei
//

#pragma warning ( disable:4786 )
#include <string>
#include <list>
#include <vector>
#include <map>

#include <GR/GRTypes.h>

#include "resource.h"
#include "afxwin.h"




class CViewInfo;
class CMaskedContextDescriptor;

namespace GR
{
  namespace Graphic
  {
    class ContextDescriptor;
    class GDIPage;
  };
};

typedef std::map<DWORD,int> tmapKP;

class CTextEffekt
{
  public:

    GR::u32   m_Type,
              m_Color,
              m_Param1,
              m_Param2,
              m_Param3,
              m_Param4;

    CTextEffekt() :
      m_Type( 0 ),
      m_Color( 0 ),
      m_Param1( 0 ),
      m_Param2( 0 ),
      m_Param3( 0 ),
      m_Param4( 0 )
    {
    }
};

typedef std::list<CTextEffekt*>   tListEffekte;

typedef std::pair<GR::tFPoint,GR::tFPoint>    tEdge;

class CTextDialog : public CDialog
{
// Konstruktion
public:

	CTextDialog(CWnd* pParent = NULL);   // Standardkonstruktor


// Dialogfelddaten
	//{{AFX_DATA(CTextDialog)
	enum { IDD = IDD_DIALOG_TEXT };
	CButton	m_CheckEffectsSingle;
	CListBox	m_ListEffekte;
	CButton	m_ButtonToggleEffektListe;
	CSpinButtonCtrl	m_SpinTilt;
	CEdit	m_EditTilt;
	CSpinButtonCtrl	m_SpinKerning;
	CEdit	m_EditKerning;
	CEdit	m_EditText;
	CStatic	m_StaticPreview;
	//}}AFX_DATA

  GR::Graphic::GDIPage*     pPagePreview;

  LOGFONT           m_logFont;

  HFONT             m_hFont;

  CViewInfo         *pViewInfo;

  tListEffekte      m_Effects;

  GR::String        m_Text,
                    m_FontName;

  bool              m_Expanded,
                    m_Initialized,
                    m_Filled,
                    m_ApplyEffectsSingle;

  int               m_TextWidth,
                    m_TextHeight,
                    m_Kerning,
                    m_Tilt,
                    m_CharacterExtra;

  tmapKP            m_KerningPairs;
                      


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTextDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:


    void ApplyEffects( CMaskedContextDescriptor *pCDSource, CMaskedContextDescriptor *pCDTargte,
                       int iX1, int iY1, int iX2, int iY2 );
    void RebuildEffektList();
    void RebuildPreview();
    void UpdateFont();


	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTextDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnChangeEditTextText();
	virtual void OnOK();
	afx_msg void OnSelchangeComboFont();
	afx_msg void OnChangeEditTextKerning();
	afx_msg void OnChangeEditTextTilt();
	afx_msg void OnButtonEffektListe();
	afx_msg void OnButtonNewEffekt();
	afx_msg void OnDblclkListEffekte();
	afx_msg void OnCheckEffektSingle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButtonFont();
  CButton m_CheckFilled;
  afx_msg void OnBnClickedCheckFilled();

  void FillAAPoly( GR::Graphic::ContextDescriptor& cdPage, std::vector<tEdge>& listLines );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEXTDIALOG_H__E18AAAC0_4A99_11D4_924A_00104B6795C0__INCLUDED_
