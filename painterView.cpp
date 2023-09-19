#include "stdafx.h"
#include "Painter.h"
#include "MainFrm.h"
#include "Settings.h"

#include "DlgLoadPalette.h"

#include "PaletteDialog.h"
#include "PainterDoc.h"
#include "PainterView.h"

#include "PainterGfxTools.h"

#include "PainterEnums.h"
#include ".\painterview.h"

#include <debug/debugclient.h>

#undef OS_WINDOWS
#include "shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPainterView, CScrollView)

BEGIN_MESSAGE_MAP(CPainterView, CScrollView)
	//{{AFX_MSG_MAP(CPainterView)
	ON_COMMAND(ID_ANIM_STOP, OnAnimStop)
	ON_COMMAND(ID_ANIM_PLAY, OnAnimPlay)
	ON_COMMAND(ID_ANIM_END, OnAnimEnd)
	ON_COMMAND(ID_ANIM_HOME, OnAnimHome)
	ON_COMMAND(ID_ANIM_STEP_BACK, OnAnimStepBack)
	ON_COMMAND(ID_ANIM_STEP_FORWARD, OnAnimStepForward)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_PALETTE_LADEN, OnPaletteLaden)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_COMMAND(ID_PALETTE_EDITIEREN, OnPaletteEditieren)
	ON_COMMAND(ID_PAINT_ZOOMIN, OnPaintZoomin)
	ON_COMMAND(ID_PAINT_ZOOMOUT, OnPaintZoomout)
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_ANIM_FRAME_DELETE, OnAnimFrameDelete)
	ON_COMMAND(ID_ANIM_FRAME_INSERT, OnAnimFrameInsert)
	ON_COMMAND(ID_BEARBEITEN_PASTEASSELECTION, OnBearbeitenPasteasselection)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_ANIM_FRAME_INSERTBEHIND, OnAnimFrameInsertbehind)
	ON_COMMAND(ID_PALETTE_SPEICHERN, OnPaletteSpeichern)
	ON_COMMAND(ID_MENU_SPEZIAL_SPLIT, OnMenuSpezialSplit)
	ON_COMMAND(ID_EDIT_ZOOM_IN, OnEditZoomIn)
	ON_COMMAND(ID_EDIT_ZOOM_OUT, OnEditZoomOut)
	ON_COMMAND(ID_MENU_SPEZIAL_SPLIT_ALPHA, OnMenuSpezialSplitAlpha)
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_PALETTE_EDITIEREN, OnUpdatePaletteEditieren)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




extern CPainterApp  theApp;


CPainterView::CPainterView()
{
  m_viewInfo.m_pView = (CScrollView*)this;
}



CPainterView::~CPainterView()
{
}



void CPainterView::OnDraw( CDC* pDC )
{
	CPainterDoc*              pDoc;

  RECT                      rc;

  DWORD                     dwWidth,
                            dwHeight;

  CPoint                    ptOffset;


  pDoc = GetDocument();
  if ( pDoc == NULL )
  {
    // schwerer Fehler! (oder Init?)
    return;
  }

  if ( m_viewInfo.m_pPage == NULL )
  {
    dh::Log( "view without page!" );
    return;
  }

  GR::Graphic::GDIPage      *pPageTarget = m_viewInfo.m_pPage;


  m_viewInfo.m_ptScrollOffset = GetScrollPosition();
  ptOffset = m_viewInfo.m_ptScrollOffset;

  GetClientRect( &rc );

  dwWidth = rc.right * 100 / m_viewInfo.m_ZoomFaktor;
  if ( pDoc->diInfo.m_DocType == DT_FONT )
  {
    if ( rc.right >= 200 )
    {
      dwWidth = ( rc.right - 200 ) * 100 / m_viewInfo.m_ZoomFaktor;
    }
    else
    {
      dwWidth = 0;
    }
    if ( dwWidth > (DWORD)pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 )->GetWidth() )
    {
      dwWidth = pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 )->GetWidth();
    }
  }
  if ( (int)dwWidth > pPageTarget->GetWidth() )
  {
    dwWidth = pPageTarget->GetWidth();
  }
  dwHeight = rc.bottom * 100 / m_viewInfo.m_ZoomFaktor;
  if ( pDoc->diInfo.m_DocType == DT_FONT )
  {
    if ( dwHeight > (DWORD)pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 )->GetHeight() )
    {
      dwHeight = pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 )->GetHeight();
    }
  }

  rc.left = ptOffset.x;
  rc.top = ptOffset.y;
  rc.right = rc.left + dwWidth;
  rc.bottom = rc.top + dwHeight;

  m_viewInfo.DisplayPage();

  if ( ( m_viewInfo.m_RedrawFlags == ViewInfo::REDRAW_ALL )
  ||   ( m_viewInfo.m_RedrawFlags == ViewInfo::REDRAW_NONE )
  ||   ( m_viewInfo.m_RedrawFlags == ViewInfo::REDRAW_RECT ) )
  {
    SetStretchBltMode( pDC->GetSafeHdc(), COLORONCOLOR );
    RECT rc2;
    GetClientRect( &rc2 );
    DWORD   dwWidth = rc2.right - rc2.left,
            dwHeight = rc2.bottom - rc2.top,
            dwImagePixelWidth,
            dwImagePixelHeight;

    int     iPixelFractX = 0,
            iPixelFractY = 0;

    if ( m_viewInfo.m_ZoomFaktor / 100 != 0 )
    {
      iPixelFractX = ptOffset.x % ( m_viewInfo.m_ZoomFaktor / 100 );
      iPixelFractY = ptOffset.y % ( m_viewInfo.m_ZoomFaktor / 100 );
    }

    ptOffset.x -= iPixelFractX;
    ptOffset.y -= iPixelFractY;


    dwImagePixelWidth = ( dwWidth * 100 ) / m_viewInfo.m_ZoomFaktor + 2;
    dwImagePixelHeight = ( dwHeight * 100 ) / m_viewInfo.m_ZoomFaktor + 2;

    if ( dwImagePixelWidth > pDoc->diInfo.Width() )
    {
      dwImagePixelWidth = pDoc->diInfo.Width();
    }
    if ( dwImagePixelHeight > pDoc->diInfo.Height() )
    {
      dwImagePixelHeight = pDoc->diInfo.Height();
    }

    StretchBlt( pDC->GetSafeHdc(),
                ptOffset.x,
                ptOffset.y,
                ( dwImagePixelWidth * m_viewInfo.m_ZoomFaktor ) / 100,
                ( dwImagePixelHeight * m_viewInfo.m_ZoomFaktor ) / 100,
                pPageTarget->GetDC(),
                ptOffset.x * 100 / m_viewInfo.m_ZoomFaktor,
                ptOffset.y * 100 / m_viewInfo.m_ZoomFaktor,
                dwImagePixelWidth,
                dwImagePixelHeight,
                SRCCOPY );
  }

  // Selektionsrahmen
  {
    m_viewInfo.DrawSelectionFrame( pDC->GetSafeHdc() );
  }

  // und wieder löschen
  m_viewInfo.m_RedrawFlags = 0;
}



#ifdef _DEBUG
void CPainterView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPainterView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CPainterDoc* CPainterView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPainterDoc)));
	return (CPainterDoc*)m_pDocument;
}
#endif //_DEBUG



void CPainterView::OnAnimStop()
{
	CPainterDoc*              pDoc;


	pDoc = GetDocument();
  if ( pDoc->diInfo.m_DocType == DT_IMAGE )
  {
    pDoc->diInfo.m_AnimationRunning = FALSE;
  }
}



void CPainterView::OnAnimPlay()
{
	CPainterDoc*              pDoc = GetDocument();
  if ( pDoc->diInfo.m_DocType == DT_IMAGE )
  {
    pDoc->diInfo.m_AnimationRunning = TRUE;
  }
}



void CPainterView::OnAnimEnd()
{
	CPainterDoc*              pDoc = GetDocument();
  if ( pDoc->diInfo.m_DocType == DT_IMAGE )
  {
    pDoc->diInfo.m_AnimationRunning = FALSE;
    pDoc->diInfo.CurrentFrame( pDoc->diInfo.m_LayeredFrames.size() - 1 );

    m_viewInfo.RedrawView();
  }
  theApp.ShowStatusMessage();
}



void CPainterView::OnAnimHome()
{
	CPainterDoc*              pDoc = GetDocument();
  if ( pDoc->diInfo.m_DocType == DT_IMAGE )
  {
    pDoc->diInfo.m_AnimationRunning = FALSE;
    pDoc->diInfo.CurrentFrame( 0 );

    m_viewInfo.RedrawView();
  }
  theApp.ShowStatusMessage();
}



void CPainterView::OnAnimStepBack()
{
	CPainterDoc*              pDoc = GetDocument();
  if ( pDoc->diInfo.m_DocType == DT_IMAGE )
  {
    pDoc->diInfo.m_AnimationRunning = FALSE;
    pDoc->diInfo.CurrentFrame( ( pDoc->diInfo.CurrentFrame() + pDoc->diInfo.m_LayeredFrames.size() - 1 ) % pDoc->diInfo.m_LayeredFrames.size() );


    theApp.ShowStatusMessage();

    if ( m_viewInfo.m_pPalette != NULL )
    {
      delete m_viewInfo.m_pPalette;
      m_viewInfo.m_pPalette = new GR::Graphic::Palette( pDoc->diInfo.m_LayeredFrames[pDoc->diInfo.CurrentFrame()].Palette );
    }
    pSettings->Notify( NF_FRAME_CHANGED );
    m_viewInfo.RedrawView();
  }
}



void CPainterView::OnAnimStepForward()
{
	CPainterDoc*              pDoc;

  
  pDoc = GetDocument();
  if ( pDoc->diInfo.m_DocType == DT_IMAGE )
  {
    pDoc->diInfo.m_AnimationRunning = FALSE;
    pDoc->diInfo.CurrentFrame( ( pDoc->diInfo.CurrentFrame() + 1 ) % pDoc->diInfo.m_LayeredFrames.size() );
    theApp.ShowStatusMessage();

    if ( m_viewInfo.m_pPalette != NULL )
    {
      delete m_viewInfo.m_pPalette;
      m_viewInfo.m_pPalette = new GR::Graphic::Palette( pDoc->diInfo.m_LayeredFrames[pDoc->diInfo.CurrentFrame()].Palette );
    }

    pSettings->Notify( NF_FRAME_CHANGED );
    m_viewInfo.RedrawView();
  }
}



void CPainterView::OnLButtonDown( UINT nFlags, CPoint point )
{
  CPoint pt = m_viewInfo.m_ptScrollOffset;

  RECT    rc;


  GetClientRect( &rc );
  ClientToScreen( &rc );
  ClipCursor( &rc );
  if ( m_viewInfo.m_Dragging )
  {
    m_viewInfo.m_pDocInfo->m_DragStart = point;
    return;
  }


  point.Offset( pt );
  point.x = ( ( point.x * 100 ) / m_viewInfo.m_ZoomFaktor );
  point.y = ( ( point.y * 100 ) / m_viewInfo.m_ZoomFaktor );

  pSettings->SnapToGrid( &point );

  m_viewInfo.DoLeftButton( point, nFlags, TRUE );

	CScrollView::OnLButtonDown( nFlags, point );

  theApp.ShowStatusMessage();
}



void CPainterView::OnLButtonUp( UINT nFlags, CPoint point )
{
	CPainterDoc*          pDoc;


  pDoc = GetDocument();
  pDoc->diInfo.m_MButton1Released = TRUE;
  ClipCursor( NULL );

  if ( m_viewInfo.m_Dragging )
  {
    pDoc->diInfo.m_MButton1Released = TRUE;
    m_viewInfo.m_Dragging = FALSE;
    return;
  }


  CPoint pt = m_viewInfo.m_ptScrollOffset;


  point.Offset( pt );
  point.x = ( ( point.x * 100 ) / m_viewInfo.m_ZoomFaktor );
  point.y = ( ( point.y * 100 ) / m_viewInfo.m_ZoomFaktor );

  pSettings->SnapToGrid( &point );

  if ( ( pSettings->m_dwFunction == CSettings::F_SELECTION )
  &&   ( pSettings->GetSetting( "SelectionSquare" ) == 1 ) )
  {
    int   iDiff = point.x - pDoc->diInfo.m_StartX;
    point.y = pDoc->diInfo.m_StartY + iDiff;
  }

  m_viewInfo.DoLeftButtonUp( point, nFlags );

	CScrollView::OnLButtonUp( nFlags, point );

  theApp.ShowStatusMessage();
}



void CPainterView::OnRButtonDown( UINT nFlags, CPoint point )
{
	CPainterDoc*          pDoc;

  GR::Graphic::Image             *pDocImage;


  //m_viewInfo.m_ptScrollOffset = GetScrollPosition();
  pDoc = GetDocument();

  pDocImage = pDoc->diInfo.GetActiveImage();

  //CPoint pt = GetScrollPosition();
  CPoint pt = m_viewInfo.m_ptScrollOffset;


  point.Offset( pt );
  point.x = ( ( point.x * 100 ) / m_viewInfo.m_ZoomFaktor );
  point.y = ( ( point.y * 100 ) / m_viewInfo.m_ZoomFaktor );

  if ( pDoc->diInfo.m_DocType == DT_FONT )
  {
    point.x -= 200;
  }

  pSettings->SnapToGrid( &point );

  if ( pSettings->m_dwFunction == CSettings::F_ZOOM )
  {
    // Zoom-In
    if ( m_viewInfo.m_ZoomFaktor > 25 )
    {
      CPoint  ptOffset = GetScrollPosition();

      RECT    rc;

      GetParentFrame()->GetWindowRect( &rc );

      int   iCenterX = point.x;
  
      int   iCenterY = point.y;

      m_viewInfo.m_ZoomFaktor /= 2;

      // Fenster anpassen
      m_viewInfo.AdjustWindowSize( iCenterX, iCenterY );
      m_viewInfo.RedrawView();
    }
    CScrollView::OnRButtonDown(nFlags, point);
    return;
  }

  m_viewInfo.GetWorkColorFromPixel( point.x, point.y );

	CScrollView::OnRButtonDown( nFlags, point );

  theApp.ShowStatusMessage();
}



void CPainterView::OnMouseMove( UINT nFlags, CPoint point )
{
  if ( pSettings->GetSetting( "PickerActive" ) )
  {
    TRACE( "View MouseMOve Picker!" );
    return;
  }

	CPainterDoc*          pDoc;
  
  
  //m_viewInfo.m_ptScrollOffset = GetScrollPosition();
  pDoc = GetDocument();

  CPoint pt = m_viewInfo.m_ptScrollOffset;
  //CPoint pt = GetScrollPosition();

  if ( ( m_viewInfo.m_Dragging )
  &&   ( nFlags & MK_LBUTTON ) )
  {
    RECT rc;
    GetClientRect( &rc );

    BOOL    bHScroll = FALSE,
            bVScroll = FALSE;

    if ( rc.right - rc.left < (long)pDoc->diInfo.Width() * m_viewInfo.m_ZoomFaktor / 100 )
    {
      bHScroll = TRUE;
    }
    if ( rc.bottom - rc.top < (long)pDoc->diInfo.Height() * m_viewInfo.m_ZoomFaktor / 100 )
    {
      bVScroll = TRUE;
    }
    if ( bHScroll )
    {
      pt.x -= ( point.x - pDoc->diInfo.m_DragStart.x );
    }
    if ( bVScroll )
    {
      pt.y -= ( point.y - pDoc->diInfo.m_DragStart.y );
    }

    ScrollToPosition( pt );

    pDoc->diInfo.m_DragStart = point;
    m_viewInfo.m_ptScrollOffset = pt;
    Invalidate();
    return;
  }



  point.Offset( pt );
  point.x = ( ( point.x * 100 ) / m_viewInfo.m_ZoomFaktor );
  point.y = ( ( point.y * 100 ) / m_viewInfo.m_ZoomFaktor );
  if ( pDoc->diInfo.m_DocType == DT_FONT )
  {
    point.x -= 200;
  }

  pSettings->SnapToGrid( &point );

  if ( ( pSettings->m_dwFunction == CSettings::F_SELECTION )
  &&   ( pSettings->GetSetting( "SelectionSquare" ) == 1 ) )
  {
    int   iDiff = point.x - pDoc->diInfo.m_StartX;
    point.y = pDoc->diInfo.m_StartY + iDiff;
  }

  if ( nFlags & MK_RBUTTON )
  {
    m_viewInfo.GetWorkColorFromPixel( point.x, point.y );
  }

  pDoc->diInfo.m_ActX = point.x;
  pDoc->diInfo.m_ActY = point.y;

  if ( m_viewInfo.m_FloatingSelection )
  {
    pDoc->diInfo.m_SelectionX = pDoc->diInfo.m_ActX;
    pDoc->diInfo.m_SelectionY = pDoc->diInfo.m_ActY;

    m_viewInfo.RedrawView();
  }

  if ( nFlags & MK_LBUTTON )
  {
    m_viewInfo.DoLeftButton( point, nFlags, FALSE );
  }
  
  theApp.ShowStatusMessage();

	CScrollView::OnMouseMove( nFlags, point );

}



void CPainterView::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
{

  pSettings->UpdateActiveViewInfo();

  CScrollView::OnActivateView( bActivate, pActivateView, pDeactiveView );

  theApp.ShowStatusMessage();

}



void CPainterView::OnPaletteLaden() 
{
  GR::Char                szTemp[MAX_PATH];

  CPainterDoc*            pDoc;

  GR::Graphic::Palette*   pPalette = NULL;

  int                     i;

  unsigned char           ucValue[3];


  pDoc = GetDocument();

  CFileDialog             pcfDlg( TRUE, NULL, NULL,
                                  OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
                                  _T( "Palette Files (*.PAL)|*.PAL|IGF Palette Files (*.IGF)|*.IGF|Photoshop Palette (*.ACT)|*.ACT|All Files (*.*)|*.*||\0\0" ),
                                  this );

  if ( pcfDlg.DoModal() == IDOK )
  {
    sprintf_s( szTemp, MAX_PATH, GR::Convert::ToUTF8( LPCTSTR( pcfDlg.GetPathName() ) ).c_str() );
    if ( pcfDlg.m_ofn.nFilterIndex == 1 )
    {
      // ein PAL-File
      pPalette = new GR::Graphic::Palette();
      pPalette->LoadPAL( szTemp );
    }
    else if ( pcfDlg.m_ofn.nFilterIndex == 2 )
    {
      // eine IGF-Palette
      pPalette = new GR::Graphic::Palette();
      pPalette->Load( szTemp );
    }
    else if ( pcfDlg.m_ofn.nFilterIndex == 3 )
    {
      // eine Photoshop-Palette
      GR::IO::FileStream    ioIn( szTemp );
      if ( ioIn.IsGood() )
      {
        pPalette = new GR::Graphic::Palette();
        for ( i = 0; i < 256; i++ )
        {
          ucValue[0] = ( ioIn.ReadU8() >> 2 );
          ucValue[1] = ( ioIn.ReadU8() >> 2 );
          ucValue[2] = ( ioIn.ReadU8() >> 2 );
          pPalette->SetColor( i, ucValue[0], ucValue[1], ucValue[2] );
        }
        ioIn.Close();
      }
    }
    if ( pPalette != NULL )
    {
      // diese Bilder können gar keine Palette haben!
      CDlgLoadPalette      dlgMap;

      dlgMap.m_pDocInfo = &pDoc->diInfo;
      if ( dlgMap.DoModal() == IDOK )
      {
        if ( dlgMap.m_Result == DLP_MAP_TO_PAL )
        {
          pDoc->diInfo.MapToPalette( pPalette );
        }
        else if ( dlgMap.m_Result == DLP_FORCE_SET )
        {
          *pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) = *pPalette;
          SafeDelete( pPalette );
          pDoc->diInfo.RecreateAllViews();
          pDoc->diInfo.RedrawAllViews();
        }
        Invalidate( FALSE );
        pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );
      }
    }
    else
    {
      MessageBox( _T( "Could not read palette data from file." ), _T( "Error!" ), 0 );
    }
  }
}



BOOL CPainterView::OnEraseBkgnd( CDC* pDC ) 
{

  // Hintergrund nicht löschen!
  // PFUI - PFUI - PFUI
  CBrush                    brushOutside;
  brushOutside.CreateHatchBrush( HS_DIAGCROSS, GetSysColor( COLOR_APPWORKSPACE ) );

  CPoint    ptOffs = GetScrollPosition();

  pDC->SetBkColor( GetSysColor( COLOR_BTNFACE ) );

	// fill rect outside the image
	CRect rect;
	GetClientRect(rect);
	ASSERT(rect.left == 0 && rect.top == 0);
	rect.left = m_totalDev.cx;
	if (!rect.IsRectEmpty())
  {
    pDC->SetBrushOrg( -ptOffs.x % 8, -ptOffs.y % 8 );
		pDC->FillRect(rect, &brushOutside);    // vertical strip along the side
  }
	rect.left = 0;
	rect.right = m_totalDev.cx;
	rect.top = m_totalDev.cy;
	if (!rect.IsRectEmpty())
  {
    pDC->SetBrushOrg( -ptOffs.x % 8, -ptOffs.y % 8 );
		pDC->FillRect(rect, &brushOutside);    // horizontal strip along the bottom
  }

  return TRUE;
}




void CPainterView::OnTimer( UINT nIDEvent ) 
{
  CPainterDoc*            pDoc = GetDocument();
  if ( ( pDoc->diInfo.m_DocType == DT_IMAGE )
  &&   ( pDoc->diInfo.m_AnimationRunning ) )
  {
    pDoc->diInfo.CurrentFrame( ( pDoc->diInfo.CurrentFrame() + 1 ) % pDoc->diInfo.m_LayeredFrames.size() );

    Invalidate( FALSE );
  }

  theApp.ShowStatusMessage();
}



int CPainterView::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( CScrollView::OnCreate( lpCreateStruct ) == -1 )
  {
		return -1;
  }
	return 0;
}



void CPainterView::OnPaletteEditieren() 
{
  CPaletteDialog        cpalDlg;

  CPainterDoc*          pDoc;


  pDoc = GetDocument();
  cpalDlg.pDoc = pDoc;
  cpalDlg.DoModal();
}



void CPainterView::OnPaintZoomin() 
{
  if ( m_viewInfo.m_ZoomFaktor < 1600 )
  {
    CPoint  ptOffset = GetScrollPosition();

    RECT    rc;

    GetParentFrame()->GetWindowRect( &rc );

    int   iCenterX = ( rc.right - rc.left - 2 * GetSystemMetrics( SM_CXSIZEFRAME ) - 2 * GetSystemMetrics( SM_CXEDGE ) ) / 2;
    
    iCenterX = ( ptOffset.x + iCenterX ) * 100 / m_viewInfo.m_ZoomFaktor;

    int   iCenterY = ( rc.bottom - rc.top - 2 * GetSystemMetrics( SM_CYSIZEFRAME ) - 2 * GetSystemMetrics( SM_CYEDGE ) - GetSystemMetrics( SM_CYCAPTION ) ) / 2;
    
    iCenterY = ( ptOffset.y + iCenterY ) * 100 / m_viewInfo.m_ZoomFaktor;

    m_viewInfo.m_ZoomFaktor *= 2;

    // Fenster anpassen
    m_viewInfo.AdjustWindowSize( iCenterX, iCenterY );
    Invalidate();
  }
}



void CPainterView::OnPaintZoomout() 
{
  if ( m_viewInfo.m_ZoomFaktor > 25 )
  {
    CPoint  ptOffset = GetScrollPosition();
    
    RECT    rc;

    GetParentFrame()->GetWindowRect( &rc );

    int   iCenterX = ( rc.right - rc.left - 2 * GetSystemMetrics( SM_CXSIZEFRAME ) - 2 * GetSystemMetrics( SM_CXEDGE ) ) / 2;
    
    iCenterX = ( ptOffset.x + iCenterX ) * 100 / m_viewInfo.m_ZoomFaktor;

    int   iCenterY = ( rc.bottom - rc.top - 2 * GetSystemMetrics( SM_CYSIZEFRAME ) - 2 * GetSystemMetrics( SM_CYEDGE ) - GetSystemMetrics( SM_CYCAPTION ) ) / 2;
    
    iCenterY = ( ptOffset.y + iCenterY ) * 100 / m_viewInfo.m_ZoomFaktor;

    m_viewInfo.m_ZoomFaktor /= 2;

    // Fenster anpassen
    m_viewInfo.AdjustWindowSize( iCenterX, iCenterY );
    Invalidate();
  }
}



void CPainterView::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
  CPainterDoc*          pDoc;

  BOOL                  bShift = FALSE,
                        bControl = FALSE,
                        bTab = FALSE;


  if ( ( nChar == VK_SHIFT )
  ||   ( nChar == VK_CONTROL ) )
  {
    theApp.SetAppCursor( m_viewInfo.m_pDocInfo );
  }

  if ( GetKeyState( VK_SHIFT ) & 0x8000 )
  {
    bShift = TRUE;
  }
  if ( GetKeyState( VK_CONTROL ) & 0x8000 )
  {
    bControl = TRUE;
  }
  if ( GetKeyState( VK_TAB ) & 0x8000 )
  {
    bTab = TRUE;
  }
  if ( nChar == ' ' )
  {
    m_viewInfo.m_Dragging = FALSE;
    ClipCursor( NULL );
    SetCursor( LoadCursor( NULL, IDC_ARROW ) );
    m_viewInfo.m_pDocInfo->m_MButton1Released = TRUE;
  }

  pDoc = GetDocument();

  if ( nChar == '1' )
  {
    if ( pDoc->diInfo.m_DocType == DT_IMAGE )
    {
      OnAnimStepForward();
    }
  }
  if ( nChar == '2' )
  {
    if ( pDoc->diInfo.m_DocType == DT_IMAGE )
    {
      OnAnimStepBack();
    }
  }

  if ( nChar == VK_RIGHT )
  {
    if ( bControl )
    {
      if ( bShift )
      {
        pDoc->diInfo.Shift( 10, 0 );
      }
      else
      {
        pDoc->diInfo.Shift( 1, 0 );
      }
    }
    else if ( bShift )
    {
      SendMessage( WM_HSCROLL, SB_PAGERIGHT, NULL );
    }
    else
    {
      SendMessage( WM_HSCROLL, SB_LINERIGHT, NULL );
    }
  }
  else if ( nChar == VK_LEFT )
  {
    if ( bControl )
    {
      if ( bShift )
      {
        pDoc->diInfo.Shift( -10, 0 );
      }
      else
      {
        pDoc->diInfo.Shift( -1, 0 );
      }
    }
    else if ( bShift )
    {
      SendMessage( WM_HSCROLL, SB_PAGELEFT, NULL );
    }
    else
    {
      SendMessage( WM_HSCROLL, SB_LINELEFT, NULL );
    }
  }
  else if ( nChar == VK_DOWN )
  {
    if ( bControl )
    {
      if ( bShift )
      {
        pDoc->diInfo.Shift( 0, 10 );
      }
      else
      {
        pDoc->diInfo.Shift( 0, 1 );
      }
    }
    else if ( bShift )
    {
      SendMessage( WM_VSCROLL, SB_PAGEDOWN, NULL );
    }
    else
    {
      SendMessage( WM_VSCROLL, SB_LINEDOWN, NULL );
    }
  }
  else if ( nChar == VK_UP )
  {
    if ( bControl )
    {
      if ( bShift )
      {
        pDoc->diInfo.Shift( 0, -10 );
      }
      else
      {
        pDoc->diInfo.Shift( 0, -1 );
      }
    }
    else if ( bShift )
    {
      SendMessage( WM_VSCROLL, SB_PAGEUP, NULL );
    }
    else
    {
      SendMessage( WM_VSCROLL, SB_LINEUP, NULL );
    }
  }
  else if ( nChar == 'S' )
  {
    // Stretch Selection auf Bildgröße
    if ( m_viewInfo.m_FloatingSelection )
    {
      int     i,
              j;
      int                   faktorx,
                            faktory,
                            curx,
                            cury;

      unsigned short int    dx,
                            ndx,
                            ndy;

      GR::Graphic::Image             *pNewImage;


      pNewImage = new GR::Graphic::Image( (WORD)pDoc->diInfo.Width(), (WORD)pDoc->diInfo.Height(), pDoc->diInfo.GetTrueBitDepth(), 0, 0 );
      dx = m_viewInfo.m_pImageFloatingSelection->GetWidth();
      ndx = pNewImage->GetWidth();
      ndy = pNewImage->GetHeight();
      faktorx = ( dx * 65535 / ndx );
      faktory = ( m_viewInfo.m_pImageFloatingSelection->GetHeight() * 65535 / ndy );

      BYTE                  *pointer = (BYTE*)pNewImage->GetData(),
                            *source = (BYTE*)m_viewInfo.m_pImageFloatingSelection->GetData();

      cury = 0;
      for ( j = 0; j < ndy; j++ )
      {
        curx = 0;
        for ( i = 0; i < ndx; i++ )
        {
          // TODO( "weniger als 8!" )
          if ( pNewImage->GetDepth() == 8 )
          {
            *pointer++ = source[curx >> 16];
            curx += faktorx;
          }
          else if ( pNewImage->GetDepth() == 16 )
          {
            *(WORD*)(pointer) = *(WORD*)( source + 2 * ( curx >> 16 ) );
            pointer += 2;
            curx += faktorx;
          }
          else if ( pNewImage->GetDepth() == 24 )
          {
            *(WORD*)(pointer) = *(WORD*)( source + 3 * ( curx >> 16 ) );
            *(pointer + 2) = *( source + 2 + 3 * ( curx >> 16 ) );
            pointer += 3;
            curx += faktorx;
          }
          else if ( pNewImage->GetDepth() == 32 )
          {
            *(DWORD*)(pointer) = *(DWORD*)( source + 4 * ( curx >> 16 ) );
            pointer += 4;
            curx += faktorx;
          }
        }
        // TODO( "weniger als 8!" )
        if ( pNewImage->GetDepth() == 8 )
        {
          source = (BYTE*)m_viewInfo.m_pImageFloatingSelection->GetData() + ( cury >> 16 ) * m_viewInfo.m_pImageFloatingSelection->GetWidth();
          cury += faktory;
        }
        else if ( pNewImage->GetDepth() == 16 )
        {
          source = (BYTE*)m_viewInfo.m_pImageFloatingSelection->GetData() + ( cury >> 16 ) * 2 * m_viewInfo.m_pImageFloatingSelection->GetWidth();
          cury += faktory;
        }
        else if ( pNewImage->GetDepth() == 24 )
        {
          source = (BYTE*)m_viewInfo.m_pImageFloatingSelection->GetData() + ( cury >> 16 ) * 3 * m_viewInfo.m_pImageFloatingSelection->GetWidth();
          cury += faktory;
        }
        else if ( pNewImage->GetDepth() == 32 )
        {
          source = (BYTE*)m_viewInfo.m_pImageFloatingSelection->GetData() + ( cury >> 16 ) * 4 * m_viewInfo.m_pImageFloatingSelection->GetWidth();
          cury += faktory;
        }
      }
      delete m_viewInfo.m_pImageFloatingSelection;
      m_viewInfo.m_pImageFloatingSelection = pNewImage;

    }
  }
  else if ( nChar == 'T' )
  {
    if ( ( !bShift )
    &&   ( !bControl ) )
    {
      pDoc->diInfo.m_SelectionFlags ^= CSettings::FLOAT_TRANSPARENT;

      m_viewInfo.RedrawView();
      return;
    }
  }
  else if ( nChar == 'H' )
  {
    if ( ( !bShift )
    &&   ( !bControl ) )
    {
      // Bild horizontal spiegeln!
      if ( m_viewInfo.m_pImageFloatingSelection != NULL )
      {
        HMirrorImage( m_viewInfo.m_pImageFloatingSelection );
        if ( m_viewInfo.m_pImageFloatingSelectionMask )
        {
          HMirrorImage( m_viewInfo.m_pImageFloatingSelectionMask );
        }

        m_viewInfo.RedrawView();
        return;
      }
      for ( size_t i = 0; i < pDoc->diInfo.m_LayeredFrames[0].LayerCount(); i++ )
      {
        CLayer* pLayer = pDoc->diInfo.GetLayer( 0, i );

        if ( m_viewInfo.m_Type == ViewInfo::VI_ALPHA )
        {
          HMirrorImage( pLayer->GetMask() );
        }
        else
        {
          HMirrorImage( pLayer->GetImage() );
        }
      }
      pDoc->diInfo.RedrawAllViews();
    }
  }
  else if ( nChar == 'V' )
  {
    if ( ( !bShift )
    &&   ( !bControl ) )
    {
      // Bild vertikal spiegeln
      if ( m_viewInfo.m_pImageFloatingSelection != NULL )
      {
        VMirrorImage( m_viewInfo.m_pImageFloatingSelection );
        if ( m_viewInfo.m_pImageFloatingSelectionMask )
        {
          VMirrorImage( m_viewInfo.m_pImageFloatingSelectionMask );
        }

        m_viewInfo.RedrawView();
        return;
      }
      for ( size_t i = 0; i < pDoc->diInfo.m_LayeredFrames[0].LayerCount(); i++ )
      {
        CLayer* pLayer = pDoc->diInfo.GetLayer( 0, i );

        if ( m_viewInfo.m_Type == ViewInfo::VI_ALPHA )
        {
          VMirrorImage( pLayer->GetMask() );
        }
        else
        {
          VMirrorImage( pLayer->GetImage() );
        }
      }
      pDoc->diInfo.RedrawAllViews();
    }
  }
  else if ( nChar == 'R' )
  {
    // Floating Selection rotieren
    if ( m_viewInfo.m_FloatingSelection )
    {
      GR::Graphic::Image*     pNewImage = NULL;
      GR::Graphic::Image*     pNewImageMask = NULL;

      pNewImage = RotateImage( m_viewInfo.m_pImageFloatingSelection );
      if ( m_viewInfo.m_pImageFloatingSelectionMask )
      {
        pNewImageMask = RotateImage( m_viewInfo.m_pImageFloatingSelectionMask );

        delete m_viewInfo.m_pImageFloatingSelectionMask;
        m_viewInfo.m_pImageFloatingSelectionMask = pNewImageMask;
      }

      delete m_viewInfo.m_pImageFloatingSelection;
      m_viewInfo.m_pImageFloatingSelection = pNewImage;

      m_viewInfo.RedrawView();
      return;
    }
    pDoc->diInfo.Rotate();
    pDoc->diInfo.RedrawAllViews();
  }
  else if ( nChar == VK_ESCAPE )
  {
    if ( m_viewInfo.m_FloatingSelection )
    {
      delete m_viewInfo.m_pImageFloatingSelection;
      m_viewInfo.m_pImageFloatingSelection = NULL;
      if ( m_viewInfo.m_pImageFloatingSelectionMask )
      {
        delete m_viewInfo.m_pImageFloatingSelectionMask;
        m_viewInfo.m_pImageFloatingSelectionMask = NULL;
      }
      m_viewInfo.m_FloatingSelection = FALSE;

      m_viewInfo.RedrawView();
      return;
    }
  }

	CScrollView::OnKeyUp( nChar, nRepCnt, nFlags );

  theApp.ShowStatusMessage();
}



void CPainterView::OnFileSaveAs() 
{
  theApp.OnFileSaveAs( GetDocument()->diInfo );
}



void CPainterView::OnFileSave() 
{
  CPainterDoc*            pDoc;


  pDoc = GetDocument();
  if ( ( CPainterView::GetDocument()->GetTitle() == "New Document" )
  ||   ( CPainterView::GetDocument()->GetTitle() == "New Document *" ) )
  {
    OnFileSaveAs();
  }
  else
  {
    CPainterView::GetDocument()->OnSaveDocument( GR::Convert::ToUTF16( CPainterView::GetDocument()->diInfo.m_FileName ).c_str() );
  }
}



void CPainterView::OnEditCopy() 
{
  m_viewInfo.CopyToClipboard();
}



void CPainterView::OnAnimFrameDelete() 
{
  CPainterDoc*          pDoc;


  pDoc = GetDocument();
  if ( pDoc->diInfo.m_DocType == DT_IMAGE )
  {
    if ( !pDoc->diInfo.m_LayeredFrames.empty() )
    {
      if ( pDoc->diInfo.m_LayeredFrames.size() == 1 )
      {
        AfxMessageBox( _T( "Es muß immer mindestens ein Frame vorhanden sein!" ) );
        return;
      }
      pDoc->diInfo.AddUndoFrameChange( pDoc->diInfo.CurrentFrame(),
                                       CUndoFrameChange::UFC_REMOVE_FRAME );

      pDoc->diInfo.RemoveFrame( pDoc->diInfo.CurrentFrame() );
      Invalidate( FALSE );
      pDoc->diInfo.SetModify( TRUE );
    }
    else
    {
      MessageBox( _T( "Die Animation hat nur noch einen Frame!" ), _T( "Fehler" ), 0 );
    }
  }
  theApp.ShowStatusMessage();
}



void CPainterView::OnAnimFrameInsert() 
{
  CPainterDoc*          pDoc;

  pDoc = GetDocument();
  if ( pDoc->diInfo.m_DocType == DT_IMAGE )
  {
    pDoc->diInfo.AddUndoFrameChange( pDoc->diInfo.CurrentFrame(),
                                     CUndoFrameChange::UFC_INSERT_FRAME );

    int   iNewFrame = pDoc->diInfo.AddFrame( pDoc->diInfo.CurrentFrame() );
    pDoc->diInfo.AddLayer( new GR::Graphic::Image( pDoc->diInfo.Width(),
                                          pDoc->diInfo.Height(),
                                          pDoc->diInfo.m_BitDepth ),
                           iNewFrame );

    *pDoc->diInfo.GetPalette( iNewFrame ) = *pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() );

    m_viewInfo.RedrawView();
    pDoc->diInfo.SetModify( TRUE );
  }
  theApp.ShowStatusMessage();
}



void CPainterView::OnAnimFrameInsertbehind() 
{
  CPainterDoc*          pDoc;

  pDoc = GetDocument();
  if ( pDoc->diInfo.m_DocType == DT_IMAGE )
  {
    pDoc->diInfo.AddUndoFrameChange( pDoc->diInfo.CurrentFrame() + 1,
                                     CUndoFrameChange::UFC_INSERT_FRAME );

    int   iNewFrame = pDoc->diInfo.AddFrame( pDoc->diInfo.CurrentFrame() + 1 );
    pDoc->diInfo.AddLayer( new GR::Graphic::Image( pDoc->diInfo.Width(),
                                          pDoc->diInfo.Height(),
                                          pDoc->diInfo.m_BitDepth ),
                           iNewFrame );

    *pDoc->diInfo.GetPalette( iNewFrame ) = *pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() );

    m_viewInfo.RedrawView();
    pDoc->diInfo.SetModify( TRUE );
  }
  theApp.ShowStatusMessage();
}



void CPainterView::OnBearbeitenPasteasselection() 
{
  CPainterDoc*          pDoc;


  pDoc = GetDocument();
  if ( pDoc == NULL )
  {
    return;
  }
  if ( OpenClipboard() )
  {
    pDoc->diInfo.PasteFromClipBoard( 0 );
    CloseClipboard();
  }
  else
  {
    MessageBox( _T( "failed to open clipboard" ), _T( "mist" ), MB_OK | MB_APPLMODAL );
  }
  theApp.ShowStatusMessage();
}



BOOL CPainterView::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message ) 
{
  CPainterDoc*          pDoc;


  pDoc = GetDocument();
  if ( pDoc == NULL )
  {
    return CScrollView::OnSetCursor( pWnd, nHitTest, message );
  }
  if ( nHitTest != HTCLIENT )
  {
    SetCursor( LoadCursor( NULL, IDC_ARROW ) );
    return TRUE;
  }
  if ( m_viewInfo.m_Dragging )
  {
    SetCursor( LoadCursor( theApp.m_hInstance, MAKEINTRESOURCE( IDC_CURSOR_HAND ) ) );
    return TRUE;
  }
  theApp.SetAppCursor( &pDoc->diInfo );
  
	return TRUE;
}



BOOL CPainterView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  CPainterDoc*          pDoc = GetDocument();

	return CScrollView::OnCommand( wParam, lParam );
}



void CPainterView::OnPaletteSpeichern() 
{
  CFileDialog*            pcfDlg;

	CPainterDoc*          pDoc;
  
  pDoc = GetDocument();
  if ( pDoc->diInfo.GetTrueBitDepth() != 8 )
  {
    MessageBox( _T( "Paletten können nur von 8-bit-Grafiken gespeichert werden!" ), _T( "Hinweis:" ), 0 );
    return;
  }
  pcfDlg = new CFileDialog( FALSE, _T( "igf" ), NULL, 
                            OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
                            _T( "Palette Files (*.IGF)|*.IGF|All Files (*.*)|*.*||\0\0" ),
                            this );
  if ( pcfDlg->DoModal() == IDOK )
  {
    GR::String      targetFile = GR::Convert::ToUTF8( LPCTSTR( pcfDlg->GetPathName() ) );
    m_viewInfo.m_pPalette->Save( targetFile.c_str() );
  }
  delete pcfDlg;
}



void CPainterView::OnMenuSpezialSplit() 
{
  CFrameWnd * pFrameWnd = theApp.pDocTemplate->CreateNewFrame( m_pDocument, NULL );

  // error checking in case creation of frame or view fails
  if ( pFrameWnd == NULL )
  {
    return;
  }

  // let the frame do any initialization the framework needs,
  // call the view's OnInitialUpdate()
  theApp.pDocTemplate->InitialUpdateFrame( pFrameWnd, m_pDocument );
  CPainterView * pView = (CPainterView*)pFrameWnd->GetActiveView();
  pView->m_viewInfo.InitializePage( ViewInfo::VI_ALPHA );
}



void CPainterView::OnMenuSpezialSplitAlpha() 
{
  CFrameWnd * pFrameWnd = theApp.pDocTemplate->CreateNewFrame( m_pDocument, NULL );

  // error checking in case creation of frame or view fails
  if ( pFrameWnd == NULL )
  {
    return;
  }

  // let the frame do any initialization the framework needs,
  // call the view's OnInitialUpdate()
  theApp.pDocTemplate->InitialUpdateFrame( pFrameWnd, m_pDocument );
  CPainterView * pView = (CPainterView*)pFrameWnd->GetActiveView();
  pView->m_viewInfo.InitializePage( ViewInfo::VI_ALPHA );
}



void CPainterView::OnInitialUpdate() 
{
  CPainterDoc*            pDoc;


	CScrollView::OnInitialUpdate();

  pDoc = GetDocument();

  if ( pDoc == NULL )
  {
    MessageBox( _T( "ONINITIALUPDATE pDoc gleich NULL!" ), _T( "Fehler!" ), 0 );
    return;
  }
  m_viewInfo.m_pDocInfo = &( (CPainterDoc*)GetDocument() )->diInfo;

  m_viewInfo.AdjustWindowSize();

  m_viewInfo.InitializePage( ViewInfo::VI_RGB );

  m_viewInfo.m_RedrawFlags = ViewInfo::REDRAW_ALL;
  m_viewInfo.m_rectRedraw.set( 0, 0, m_viewInfo.m_pDocInfo->Width(), m_viewInfo.m_pDocInfo->Height() );
  Invalidate();

  SetScrollSizes( MM_TEXT, CSize( ( pDoc->diInfo.Width() * m_viewInfo.m_ZoomFaktor ) / 100, ( pDoc->diInfo.Height() * m_viewInfo.m_ZoomFaktor ) / 100 ) );
}



void CPainterView::OnEditZoomIn() 
{
  CPainterDoc     *pDoc;

	
  pDoc = GetDocument();
  if ( pDoc != NULL )
  {
    pDoc->diInfo.ZoomIn();
  }
}



void CPainterView::OnEditZoomOut() 
{
  CPainterDoc*  pDoc = GetDocument();
  if ( pDoc != NULL )
  {
    pDoc->diInfo.ZoomOut();
  }
}



void CPainterView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
  if ( ( nChar == VK_SHIFT )
  ||   ( nChar == VK_CONTROL ) )
  {
    theApp.SetAppCursor( m_viewInfo.m_pDocInfo );
  }

  if ( nChar == VK_SPACE )
  {
    m_viewInfo.m_Dragging = TRUE;
    SetCursor( LoadCursor( theApp.m_hInstance, MAKEINTRESOURCE( IDC_CURSOR_HAND ) ) );
  }
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CPainterView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
  if ( m_viewInfo.m_RedrawFlags != ViewInfo::REDRAW_ALL )
  {
    m_viewInfo.m_RedrawFlags = (DWORD)lHint;
  }
  if ( m_viewInfo.m_RedrawFlags == ViewInfo::REDRAW_RECT )
  {
    m_viewInfo.m_rectRedraw.combine( *( GR::tRect* )pHint );

    if ( m_viewInfo.m_rectRedraw.Left < 0 )
    {
      m_viewInfo.m_rectRedraw.Left = 0;
    }
    if ( m_viewInfo.m_rectRedraw.Right > (int)m_viewInfo.m_pDocInfo->Width() )
    {
      m_viewInfo.m_rectRedraw.Right = m_viewInfo.m_pDocInfo->Width();
    }
    if ( m_viewInfo.m_rectRedraw.Top < 0 )
    {
      m_viewInfo.m_rectRedraw.Top = 0;
    }
    if ( m_viewInfo.m_rectRedraw.Bottom > (int)m_viewInfo.m_pDocInfo->Height() )
    {
      m_viewInfo.m_rectRedraw.Bottom = m_viewInfo.m_pDocInfo->Height();
    }
  }
  Invalidate();
}



BOOL CPainterView::OnScroll( UINT nScrollCode, UINT nPos, BOOL bDoScroll ) 
{
  Invalidate();

  BOOL    bResult = CScrollView::OnScroll( nScrollCode, nPos, bDoScroll );

  POINT   ptScrollOffset = GetScrollPosition();

  if ( ( ptScrollOffset.x != m_viewInfo.m_ptScrollOffset.x )
  ||   ( ptScrollOffset.y != m_viewInfo.m_ptScrollOffset.y ) )
  {
    m_viewInfo.m_ptScrollOffset = ptScrollOffset;
  }
		
	return bResult;
}



void CPainterView::OnDestroy() 
{
  CScrollView::OnDestroy();

  pSettings->UpdateActiveViewInfo();
}



void CPainterView::OnUpdatePaletteEditieren( CCmdUI* pCmdUI ) 
{
  if ( ( pSettings->m_pActiveViewInfo == NULL )
  ||   ( pSettings->m_pActiveViewInfo->m_pPalette == NULL ) )
  {
    pCmdUI->Enable( FALSE );
    return;
  }
  pCmdUI->Enable();
}



