#if !defined(AFX_VIEWINFO_H__45ED4B42_6C8F_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_VIEWINFO_H__45ED4B42_6C8F_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Grafik/GDIPage.h>
#include <Grafik/Palette.h>
#include <Grafik/Image.h>

#include "MaskedContextDescriptor.h"


class DocumentInfo;

class CLayer;

class CScrollView;

class CViewInfo  
{
  public:

    enum eRedraw
    {
      REDRAW_NONE       = 0,
      REDRAW_IMAGE,
      REDRAW_SELECTION,
      REDRAW_RECT,
      REDRAW_UPDATE_VIEW_INFO,    // für Fontviews, um die Breite/Höhe/Nr. an anderen View anzupassen
      REDRAW_ALL        = 0xffffffff,
    };

    enum eViewInfoType
    {
      VI_INVALID,
      VI_DEFAULT,
      VI_RGB,
      VI_ALPHA,
    };

    CScrollView*          m_pView;

    DocumentInfo*        m_pDocInfo;

    GR::Graphic::Image*   m_pImageFloatingSelection;
    GR::Graphic::Image*   m_pImageFloatingSelectionMask;

    GR::Graphic::GDIPage* m_pPage;

    GR::Graphic::Palette* m_pPalette;
    GR::Graphic::Palette* m_pAlphaPalette;
    GR::Graphic::Palette* m_pDefaultPalette;

    POINT                 m_ptScrollOffset;

    bool                  m_FloatingSelection,
                          m_NeedToReleaseMouseButton;

    BOOL                  m_Dragging,
                          m_Selecting;       // zieht der User gerade eine SizeBox?

    bool                  m_DrawFirstSelectingFrame;

    GR::tRect             m_LastSelectingFrame,
                          m_rectRedraw;

    WORD                  m_ZoomFaktor;

    eViewInfoType         m_Type;

    DWORD                 m_RedrawFlags;


    CViewInfo();
	  virtual ~CViewInfo();

    void                  InitializePage( eViewInfoType eType = VI_DEFAULT );

    void                  AdjustWindowSize( int iCenteredX = -1, int iCenteredY = -1 );

    void                  DrawFloatingSelection( CLayer* pLayer, GR::Graphic::ContextDescriptor& cdTarget, int iX, int iY, bool bInsert = false );
    void                  DrawSelectionFrame( HDC hdc );

    CMaskedContextDescriptor *GetCD();

    void                  DoLeftButton( POINT pt, int iFlags, BOOL bFirstDown );
    void                  DoLeftButtonUp( POINT point, int iFlags );

    void DoFunction( CMaskedContextDescriptor *pCD, DWORD dwFunction, int iX, int iY, int iX2 = 0, int iY2 = 0 );

    DWORD GetRGBColor( DWORD dwOrigIndex );
    void GetWorkColorFromPixel( int iX, int iY );

    GR::Graphic::Palette*         GetActivePalette();
    GR::Graphic::Image *GetActiveImage();
    GR::Graphic::Image *GetWorkImage();

    // gibt eine Kopie des aktiven Images zurück (als CD-Owner)
    GR::Graphic::ContextDescriptor*           GetWorkCD();

    DWORD GetViewBitDepth();

    void                  Invert();

    void RedrawView();

    void DisplayPage();        // Helper-Funktion zum Anzeigen

    // Clipboard
    void                  CopyToClipboard();

    DWORD                 ToLocalColor( DWORD dwColor );

    void                  AddUndoRect( int iX1, int iY1, int iX2, int iY2, int iFrame, int iLayer, BOOL bStartNewUndoGroup = TRUE );

};

#endif // !defined(AFX_VIEWINFO_H__45ED4B42_6C8F_11D5_AC02_00104B6795C0__INCLUDED_)
