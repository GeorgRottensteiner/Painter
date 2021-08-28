#if !defined(AFX_PROPGRID_H__2615DE01_7238_11D5_9D7D_0001029EF128__INCLUDED_)
#define AFX_PROPGRID_H__2615DE01_7238_11D5_9D7D_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropGrid.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPropGrid 

class CPropGrid : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropGrid)

// Konstruktion
public:
	CPropGrid();
	~CPropGrid();

// Dialogfelddaten
	//{{AFX_DATA(CPropGrid)
	enum { IDD = IDD_PROP_GRID };
	CButton	m_CheckSnap;
	CEdit	m_EditGridYOffset;
	CEdit	m_EditGridXOffset;
	CEdit	m_EditGridWidth;
	CEdit	m_EditGridHeight;
	//}}AFX_DATA

  int       m_iGridWidth,
            m_iGridHeight,
            m_iGridXOffset,
            m_iGridYOffset;

  BOOL      m_bEnableSnap;

// �berschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CPropGrid)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPropGrid)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_PROPGRID_H__2615DE01_7238_11D5_9D7D_0001029EF128__INCLUDED_
