#if !defined(AFX_PAINTERDOC_H__6E63B46A_8C86_11D2_9F65_004033315771__INCLUDED_)
#define AFX_PAINTERDOC_H__6E63B46A_8C86_11D2_9F65_004033315771__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DocumentInfo.h"
#include "FreeImage.h"



class CPainterDoc : public CDocument
{
  public:
    CPainterDoc();
    BOOL CPainterDoc::OnCreateDocumentFromClipboard( void );
  protected: // Nur aus Serialisierung erzeugen
	  //CPainterDoc();
	  DECLARE_DYNCREATE(CPainterDoc)

  private:

  
  // Attribute
  public:

    DocumentInfo         diInfo;


  // Operationen
  public:

  // Überladungen
	  // Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	  //{{AFX_VIRTUAL(CPainterDoc)
	  public:
	  virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	  virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	  virtual void OnCloseDocument();
	  //}}AFX_VIRTUAL

  // Implementierung
  public:
	  virtual ~CPainterDoc();

  #ifdef _DEBUG
	  virtual void AssertValid() const;
	  virtual void Dump(CDumpContext& dc) const;
  #endif


  protected:


    void AddNewImage( GR::Graphic::Image *pImage, GR::Graphic::Palette *pPalette, GR::Graphic::Image* pImageMask = NULL );
    void AddAlphaMask( GR::Graphic::Image *pImage );

  // Generierte Message-Map-Funktionen
  protected:
	  //{{AFX_MSG(CPainterDoc)
	  afx_msg void OnPaintZoomin();
	  afx_msg void OnConvertTo16bpp();
	  afx_msg void OnConvertTo24bpp();
	  afx_msg void OnConvertTo32bpp();
	  afx_msg void OnBearbeitenMirrorH();
	  afx_msg void OnBearbeitenMirrorV();
	  afx_msg void OnBearbeitenRotieren();
	  afx_msg void OnBearbeitenLoeschen();
	  afx_msg void OnConvertTo8bpp();
	  afx_msg void OnBearbeitenResizeCanvas();
	  afx_msg void OnBearbeitenResize();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnSpezialAuf4bitkonvertieren();
    afx_msg void OnSpezialAuf1bitkonvertieren();
    afx_msg void OnExtras2xsai();
    afx_msg void OnBearbeitenExportieren32909();
    afx_msg void OnBearbeitenImportieren32910();
    afx_msg void OnBearbeitenRunscript();
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PAINTERDOC_H__6E63B46A_8C86_11D2_9F65_004033315771__INCLUDED_)
