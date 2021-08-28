// Painter.h : Haupt-Header-Datei für die Anwendung PAINTER
//

#if !defined(AFX_PAINTER_H__6E63B462_8C86_11D2_9F65_004033315771__INCLUDED_)
#define AFX_PAINTER_H__6E63B462_8C86_11D2_9F65_004033315771__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#pragma warning ( disable:4786 )
#include "resource.h"       // Hauptsymbole

#include "Layer.h"
#include <Grafik/Image.h>
#include <Grafik/Animation.h>
#include <IO/FileStream.h>
#include <Grafik/Font.h>
#include <Grafik/GFXPage.h>
#include <Grafik/Palette.h>

#include "painterdoc.h"
#include "painterfontdoc.h"
#include "dialogresize.h"
#include "dialogbatchconversion.h"
#include "DialogAnimationErstellen.h"
#include <Misc/Misc.h>
#include "DocumentInfo.h"
#include "ViewInfo.h"
#include "DlgFileOpenPreview.h"
#include "PainterGFXTools.h"
#include "UndoManager.h"




/////////////////////////////////////////////////////////////////////////////
// CPainterApp:
// Siehe Painter.cpp für die Implementierung dieser Klasse
//

class CMainFrame;

class CPainterApp : public CWinApp
{
public:
	CPainterApp();
  ~CPainterApp();
  CMainFrame*     pMainFrame;

public:

  HCURSOR               hCursorFreehand1,
                        hCursorFreehand2,
                        hCursorSelection,
                        hCursorSelectionAdd,
                        hCursorSelectionRemove,
                        hCursorLine,
                        hCursorZoom,
                        hCursorRectangle,
                        hCursorBox,
                        hCursorCircle,
                        hCursorFilledCircle,
                        hCursorFill,
                        hCursorText,
                        hCursorDragHand;

  GR::Graphic::Palette          m_PainterPalette;

  CMultiDocTemplate     *pDocTemplate,        // für Grafiken
                        *pFontTemplate;       // für Fonts


  void                  OpenFile();
  void                  OnFileSaveAs( DocumentInfo& diInfo );

  void ChangeFunction( DWORD dwNewFunction );

  CDocument* CheckAndOpen( const GR::Char* File );

  void ShowStatusMessage( void );
  DocumentInfo* GetActiveDocumentInfo();
  CViewInfo* GetActiveViewInfo();

  DocumentInfo* GetDocumentInfoFromDocument( CDocument* pDoc );

  void SetAppCursor( DocumentInfo *pDocInfo );
  void ResizeWindow( CDocument *pDoc, DocumentInfo *pDocInfo, HWND hWnd, WORD ucOldZoom, BOOL bUpdateOffset );

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPainterApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementierung
	//{{AFX_MSG(CPainterApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnPaintArrow();
	afx_msg void OnPaintFreehand2();
	afx_msg void OnPaintBox();
  afx_msg void OnPaintRectangle();
	afx_msg void OnPaintCircle();
  afx_msg void OnPaintFilledCircle();
	afx_msg void OnPaintBrighten();
	afx_msg void OnPaintDarken();
	afx_msg void OnPaintFill();
	afx_msg void OnPaintFreehand1();
	afx_msg void OnPaintLine();
	afx_msg void OnPaintSelection();
  afx_msg void OnPaintSelectionColor();
	afx_msg void OnPaintZoom();
	afx_msg void OnFileNew();
	afx_msg void OnBatchConversion();
	afx_msg void OnPaintText();
	afx_msg void OnMenuAniCreator();
	afx_msg void OnMenuColorPicker();
	afx_msg void OnSpezialEffekte();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  protected:

    // Helper-Funktionen
    void CreateFilterList();
};

extern CPainterApp         theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PAINTER_H__6E63B462_8C86_11D2_9F65_004033315771__INCLUDED_)
