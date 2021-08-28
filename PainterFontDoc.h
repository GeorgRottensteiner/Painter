#if !defined(AFX_PAINTERFONTDOC_H__B5766BE3_D49C_11D3_924A_00104B6795C0__INCLUDED_)
#define AFX_PAINTERFONTDOC_H__B5766BE3_D49C_11D3_924A_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PainterFontDoc.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dokument CPainterFontDoc 

class CPainterFontView;

class CPainterFontDoc : public CDocument
{
protected:
	CPainterFontDoc();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CPainterFontDoc)

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPainterFontDoc)
	public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CPainterFontDoc();

  DocumentInfo     diInfo;


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CPainterFontDoc)
	afx_msg void OnConvertTo16bpp();
	afx_msg void OnConvertTo24bpp();
	afx_msg void OnConvertTo32bpp();
	afx_msg void OnConvertTo8bpp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnSpezialInvertieren();
  afx_msg void OnBearbeitenExportieren();
  afx_msg void OnBearbeitenImportieren();
  afx_msg void OnBearbeitenResize();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PAINTERFONTDOC_H__B5766BE3_D49C_11D3_924A_00104B6795C0__INCLUDED_
