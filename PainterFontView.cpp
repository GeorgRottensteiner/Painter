#include "stdafx.h"
#include "Painter.h"
#include "MainFrm.h"
#include "Settings.h"
#include "PainterFontView.h"

#include <String/Convert.h>

#include <debug/debugclient.h>
#include ".\painterfontview.h"

#include ".\resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CPainterApp        theApp;



IMPLEMENT_DYNCREATE(CPainterFontView, CFormView)



CPainterFontView::CPainterFontView()
	: CFormView(CPainterFontView::IDD),
    m_DoNotUpdate( false )
{
	//{{AFX_DATA_INIT(CPainterFontView)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

  m_viewInfo.m_pView = (CScrollView*)this;
  hwndFontWorkArea = NULL;
}



CPainterFontView::~CPainterFontView()
{
}



CPainterFontDoc* CPainterFontView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPainterFontDoc)));
	return (CPainterFontDoc*)m_pDocument;
}



void CPainterFontView::DoDataExchange(CDataExchange* pDX)
{
  CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CPainterFontView)
  // HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_EDIT_FONT_ZEICHENNR, m_EditZeichen);
  DDX_Control(pDX, IDC_EDIT_FONT_BREITE, m_EditWidth);
  DDX_Control(pDX, IDC_EDIT_FONT_HOEHE, m_EditHeight);
  DDX_Control(pDX, IDC_EDIT_FONT_XOFFSET, m_EditXOffset);
  DDX_Control(pDX, IDC_EDIT_FONT_YOFFSET, m_EditYOffset);
  DDX_Control(pDX, IDC_SPIN_FONT_ZEICHENNR, m_SpinZeichen);
  DDX_Control(pDX, IDC_SPIN_FONT_BREITE, m_SpinWidth);
  DDX_Control(pDX, IDC_SPIN_FONT_HOEHE, m_SpinHeight);
  DDX_Control(pDX, IDC_SPIN_FONT_XOFFSET, m_SpinXOffset);
  DDX_Control(pDX, IDC_SPIN_FONT_YOFFSET, m_SpinYOffset);
  DDX_Control(pDX, IDC_STATIC_FONT_INHALT, m_StaticLetter);
  DDX_Control(pDX, IDC_STATIC_FONT_ZEICHENNR, m_StaticZeichen);
}


BEGIN_MESSAGE_MAP(CPainterFontView, CFormView)
	//{{AFX_MSG_MAP(CPainterFontView)
	ON_WM_CREATE()
	ON_WM_DRAWITEM()
	ON_EN_CHANGE(IDC_EDIT_FONT_ZEICHENNR, OnChangeEditFontZeichennr)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FONT_BREITE, OnDeltaposSpinFontBreite)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FONT_HOEHE, OnDeltaposSpinFontHoehe)
	ON_COMMAND(ID_BEARBEITEN_PASTEASSELECTION, OnBearbeitenPasteasselection)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
  ON_WM_DESTROY()
  ON_WM_RBUTTONDOWN()
  ON_COMMAND(32833, OnSplitRGB)
  ON_COMMAND(32863, OnSplitAlpha)
  ON_BN_CLICKED(IDC_BTN_KILL_LETTER, OnBnClickedBtnKillLetter)
  ON_WM_KEYUP()
END_MESSAGE_MAP()



#ifdef _DEBUG
void CPainterFontView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPainterFontView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG



int CPainterFontView::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( CFormView::OnCreate( lpCreateStruct ) == -1 )
  {
		return -1;
  }

	return 0;
}



void CPainterFontView::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
  if ( m_viewInfo.m_pPage == NULL )
  {
    dh::Log( "view without page!" );
    return;
  }

  if ( nIDCtl == IDC_STATIC_FONT_INHALT )
  {
	  CPainterFontDoc*          pFontDoc = NULL;

    RECT                      rc;

    HDC                       hdc;

    HWND                      hwnd = NULL;

    int                       width = 0,
                              height = 0;

    GR::Graphic::GFXPage*     pTargetPage = NULL;
    

    pFontDoc = GetDocument();
    if ( pFontDoc == NULL )
    {
      // schwerer Fehler! (oder Init?)
      dh::Log( "no document!" );
      return;
    }

    hdc = lpDrawItemStruct->hDC;
    hwnd = lpDrawItemStruct->hwndItem;

    GetClientRect( &rc );

    GR::Graphic::Image*    pImageLetter = pFontDoc->diInfo.GetImage( pFontDoc->diInfo.CurrentFrame(), 0 );
    if ( pImageLetter == NULL )
    {
      return;
    }

    width = rc.right * 100 / m_viewInfo.m_ZoomFaktor;
    if ( width > (int)pImageLetter->GetWidth() )
    {
      width = pImageLetter->GetWidth();
    }
    if ( width > (int)m_viewInfo.m_pPage->GetWidth() )
    {
      width = m_viewInfo.m_pPage->GetWidth();
    }
    height = rc.bottom * 100 / m_viewInfo.m_ZoomFaktor;
    if ( height > (int)pImageLetter->GetHeight() )
    {
      height = pImageLetter->GetHeight();
    }
    if ( height > (int)m_viewInfo.m_pPage->GetHeight() )
    {
      height = m_viewInfo.m_pPage->GetHeight();
    }

    m_viewInfo.DisplayPage();

    if ( ( m_viewInfo.m_RedrawFlags == ViewInfo::REDRAW_ALL )
    ||   ( m_viewInfo.m_RedrawFlags == 0 )
    ||   ( m_viewInfo.m_RedrawFlags == ViewInfo::REDRAW_RECT ) )
    {
      MFCGFXStretchUpdate( hwnd, 
                           hdc, 
                           0,
                           0,
                           width * m_viewInfo.m_ZoomFaktor / 100,
                           height * m_viewInfo.m_ZoomFaktor / 100,
                           m_viewInfo.m_pPage,
                           0,
                           0,
                           width,
                           height );
    }

    GetClientRect( &rc );

    width = rc.right * 100 / m_viewInfo.m_ZoomFaktor;
    if ( width > (int)pImageLetter->GetWidth() )
    {
      width = pImageLetter->GetWidth();
    }
    height = rc.bottom * 100 / m_viewInfo.m_ZoomFaktor;
    if ( height > (int)pImageLetter->GetHeight() )
    {
      height = pImageLetter->GetHeight();
    }

    // Graue Box um Zeichenfeld
    HBRUSH hBrush = CreateSolidBrush( GetSysColor( COLOR_3DFACE ) );
    if ( rc.right >= (int)width * m_viewInfo.m_ZoomFaktor / 100 )
    {
      // rechts muß eine Box hin!
      rc.left += width * m_viewInfo.m_ZoomFaktor / 100;

      rc.bottom = height * m_viewInfo.m_ZoomFaktor / 100;
      FillRect( hdc, &rc, hBrush );

    }
    GetClientRect( &rc );
    if ( rc.bottom >= (int)height * m_viewInfo.m_ZoomFaktor / 100 )
    {
      // unten muß eine Box hin!
      rc.top += height * m_viewInfo.m_ZoomFaktor / 100;
      rc.right = width * m_viewInfo.m_ZoomFaktor / 100;

      FillRect( hdc, &rc, hBrush );
    }
    GetClientRect( &rc );
    if ( ( rc.right >= (int)width * m_viewInfo.m_ZoomFaktor / 100 )
    &&   ( rc.bottom >= (int)height * m_viewInfo.m_ZoomFaktor / 100 ) )
    {
      // rechts unten muß eine Box hin!
      rc.top += height * m_viewInfo.m_ZoomFaktor / 100;
      rc.left += width * m_viewInfo.m_ZoomFaktor / 100;

      FillRect( hdc, &rc, hBrush );
    }
    DeleteObject( hBrush );

    // Selektionsrahmen
    m_viewInfo.DrawSelectionFrame( hdc );

    // und wieder löschen
    m_viewInfo.m_RedrawFlags = 0;
  }
  else if ( nIDCtl == IDC_STATIC_FONT_PREVIEW )
  {

  }
}



void CPainterFontView::OnInitialUpdate() 
{
  CPainterFontDoc     *pFontDoc;


	CFormView::OnInitialUpdate();

  if ( m_FontPreview.GetSafeHwnd() == NULL )
  {
    m_FontPreview.Attach( GetDlgItem( IDC_STATIC_FONT_PREVIEW )->GetSafeHwnd() );
  }

  hwndFontWorkArea = m_StaticLetter.GetSafeHwnd();
  m_StaticLetter.ModifyStyle( 0, SS_OWNERDRAW );

  m_SpinZeichen.SetRange( 0, 255 );
  m_SpinWidth.SetRange( 0, 255 );
  m_SpinHeight.SetRange( 0, 255 );
  m_SpinXOffset.SetRange( -255, 255 );
  m_SpinYOffset.SetRange( -255, 255 );

  pFontDoc = GetDocument();

  m_viewInfo.m_pDocInfo = &pFontDoc->diInfo;
  m_viewInfo.m_ZoomFaktor = 1600;
  m_viewInfo.InitializePage( ViewInfo::VI_RGB );

  if ( pFontDoc != NULL )
  {
    m_EditZeichen.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", pFontDoc->diInfo.CurrentFrame() ) ).c_str() );

    m_EditXOffset.SetWindowText( _T( "0" ) );
    m_EditYOffset.SetWindowText( _T( "0" ) );
  }

  if ( hwndFontWorkArea != NULL )
  {
    CPainterFontDoc*  pDoc = GetDocument();
    if ( pDoc != NULL )
    {
      RECT    rc;
      RECT    rc2;
      GetClientRect( &rc );
      ::GetClientRect( hwndFontWorkArea, &rc2 );
      if ( rc2.left < rc.right - rc.left )
      {
        ::SetWindowPos( hwndFontWorkArea, NULL, 0, 0, rc.right - rc2.left, rc.bottom - rc2.top, SWP_NOZORDER | SWP_NOMOVE );
      }
    }
  }
  RedrawFontPreview();
}



void CPainterFontView::OnChangeEditFontZeichennr() 
{
  GR::tChar                 szTemp[MAX_PATH];

  CPainterFontDoc*          pFontDoc;


  pFontDoc = GetDocument();
  if ( ( pFontDoc == NULL )
  ||   ( m_DoNotUpdate ) )
  {
    // schwerer Fehler! (oder Init?)
    return;
  }

  if ( m_EditZeichen.GetSafeHwnd() == NULL )
  {
    return;
  }
  m_EditZeichen.GetLine( 0, szTemp, MAX_PATH );
  int charValue = GR::Convert::ToI32( szTemp );
  if ( charValue < 0 )
  {
    charValue = 0;
    m_EditZeichen.SetWindowText( _T( "0" ) );
  }
  if ( charValue > 255 )
  {
    charValue = 255;
    m_EditZeichen.SetWindowText( _T( "255" ) );
  }
  if ( charValue >= 32 )
  {
    m_StaticZeichen.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ISO8895ToUTF8( CMisc::printf( "Zeichen (%c):", charValue ) ) ).c_str() );
  }
  else
  {
    m_StaticZeichen.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "Zeichen:", charValue ) ).c_str() );
  }

  pFontDoc->diInfo.CurrentFrame( (unsigned char)charValue );
  if ( pFontDoc->diInfo.m_LayeredFrames.size() > pFontDoc->diInfo.CurrentFrame() )
  {
    GR::Graphic::Image*   pImage = pFontDoc->diInfo.GetImage( pFontDoc->diInfo.CurrentFrame(), 0 );
    if ( pImage )
    {
      m_EditWidth.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", pFontDoc->diInfo.GetImage( pFontDoc->diInfo.CurrentFrame(), 0 )->GetWidth() ) ).c_str() );
      m_EditHeight.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", pFontDoc->diInfo.GetImage( pFontDoc->diInfo.CurrentFrame(), 0 )->GetHeight() ) ).c_str() );
    }
    else
    {
      m_EditWidth.SetWindowText( _T( "0" ) );
      m_EditHeight.SetWindowText( _T( "0" ) );
    }
  }
  LetterNumberChanged( charValue );
}



BOOL CPainterFontView::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message ) 
{
  CPainterFontDoc*          pDoc;


  pDoc = GetDocument();
  if ( pDoc == NULL )
  {
    return CView::OnSetCursor( pWnd, nHitTest, message );
  }
  if ( nHitTest != HTCLIENT )
  {
    SetCursor( LoadCursor( NULL, IDC_ARROW ) );
    return TRUE;
  }
  theApp.SetAppCursor( &pDoc->diInfo );
  
	return TRUE;
}



void CPainterFontView::OnLButtonDown( UINT nFlags, CPoint point ) 
{
  RECT                      rc,
                            rcParent;
  

  m_viewInfo.m_ptScrollOffset = GetScrollPosition();

  m_StaticLetter.GetParent()->GetWindowRect( &rcParent );
  m_StaticLetter.GetWindowRect( &rc );
  point.x -= ( rc.left - rcParent.left );

  point.x = ( ( point.x * 100 ) / m_viewInfo.m_ZoomFaktor );
  point.y = ( ( point.y * 100 ) / m_viewInfo.m_ZoomFaktor );

  pSettings->SnapToGrid( &point );

  m_viewInfo.DoLeftButton( point, nFlags, TRUE );

  CFormView::OnLButtonDown( nFlags, point );

  theApp.ShowStatusMessage();

  m_StaticLetter.SetFocus();
}



void CPainterFontView::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	CPainterFontDoc*          pDoc;

  RECT                      rc,
                            rcParent;


  pDoc = GetDocument();
  pDoc->diInfo.m_MButton1Released = TRUE;
  ClipCursor( NULL );

  m_StaticLetter.GetParent()->GetWindowRect( &rcParent );
  m_StaticLetter.GetWindowRect( &rc );
  point.x -= ( rc.left - rcParent.left );

  m_viewInfo.m_ptScrollOffset = GetScrollPosition();

  GR::Graphic::Image *pImage = pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 );

  point.x = ( ( point.x * 100 ) / m_viewInfo.m_ZoomFaktor );
  point.y = ( ( point.y * 100 ) / m_viewInfo.m_ZoomFaktor );

  pDoc->diInfo.m_ActX = point.x;
  pDoc->diInfo.m_ActY = point.y;
  if ( pDoc->diInfo.m_ActX < 0 )
  {
    pDoc->diInfo.m_ActX = 0;
  }
  if ( pDoc->diInfo.m_ActY < 0 )
  {
    pDoc->diInfo.m_ActY = 0;
  }
  if ( pImage )
  {
    if ( pDoc->diInfo.m_ActX >= pImage->GetWidth() )
    {
      pDoc->diInfo.m_ActX = pImage->GetWidth() - 1;
    }
    if ( pDoc->diInfo.m_ActY >= pImage->GetHeight() )
    {
      pDoc->diInfo.m_ActY = pImage->GetHeight() - 1;
    }
  }
  point.x = pDoc->diInfo.m_ActX;
  point.y = pDoc->diInfo.m_ActY;


  pSettings->SnapToGrid( &point );

  m_viewInfo.DoLeftButtonUp( point, nFlags );

	CFormView::OnLButtonUp(nFlags, point);

  theApp.ShowStatusMessage();
}



void CPainterFontView::OnMouseMove( UINT nFlags, CPoint point )
{
	CPainterFontDoc*          pDoc;

  RECT                      rcParent,
                            rc;
  
  
  pDoc = GetDocument();

  m_StaticLetter.GetParent()->GetWindowRect( &rcParent );
  m_StaticLetter.GetWindowRect( &rc );
  point.x -= ( rc.left - rcParent.left );

  m_viewInfo.m_ptScrollOffset = GetScrollPosition();

  pDoc->diInfo.m_ActX = m_viewInfo.m_ptScrollOffset.x + ( point.x * 100 / m_viewInfo.m_ZoomFaktor );
  pDoc->diInfo.m_ActY = m_viewInfo.m_ptScrollOffset.y + ( point.y * 100 / m_viewInfo.m_ZoomFaktor );

  GR::Graphic::Image* pImage = pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 );

  if ( pDoc->diInfo.m_ActX < 0 )
  {
    pDoc->diInfo.m_ActX = 0;
  }
  if ( pDoc->diInfo.m_ActY < 0 )
  {
    pDoc->diInfo.m_ActY = 0;
  }
  if ( pImage )
  {
    if ( pDoc->diInfo.m_ActX >= pImage->GetWidth() )
    {
      pDoc->diInfo.m_ActX = pImage->GetWidth() - 1;
    }
    if ( pDoc->diInfo.m_ActY >= pImage->GetHeight() )
    {
      pDoc->diInfo.m_ActY = pImage->GetHeight() - 1;
    }
  }

  point.x = ( ( point.x * 100 ) / m_viewInfo.m_ZoomFaktor );
  point.y = ( ( point.y * 100 ) / m_viewInfo.m_ZoomFaktor );

  if ( nFlags & MK_RBUTTON )
  {
    m_viewInfo.GetWorkColorFromPixel( point.x, point.y );
  }


  pSettings->SnapToGrid( &point );

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

	CFormView::OnMouseMove(nFlags, point);

}



void CPainterFontView::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView ) 
{
  pSettings->UpdateActiveViewInfo();
  
  CFormView::OnActivateView( bActivate, pActivateView, pDeactiveView );

  theApp.ShowStatusMessage();
}



void CPainterFontView::OnFileSaveAs() 
{
  theApp.OnFileSaveAs( GetDocument()->diInfo );
}



void CPainterFontView::OnFileSave() 
{
  CPainterFontDoc*            pDoc;

  pDoc = GetDocument();
  if ( ( CPainterFontView::GetDocument()->GetTitle() == "New Document" )
  ||   ( CPainterFontView::GetDocument()->GetTitle() == "New Document *" ) )
  {
    OnFileSaveAs();
  }
  else
  {
    pDoc->OnSaveDocument( GR::Convert::ToUTF16( pDoc->diInfo.m_FileName ).c_str() );
  }
}



void CPainterFontView::OnDeltaposSpinFontBreite( NMHDR* pNMHDR, LRESULT* pResult ) 
{

	NMUPDOWN                    *pNMUpDown;

  CPainterFontDoc             *pDoc;

  GR::Graphic::Image                   *pDummy;
  

  pNMUpDown = (NM_UPDOWN*)pNMHDR;
  pDoc = GetDocument();

  CLayer*   pActiveLayer = pDoc->diInfo.ActiveLayer();

  if ( pNMUpDown->iDelta == -1 )
  {
    // Buchstaben-Breite verkleinern
    GR::Graphic::Image* pImage = pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 );

    GR::Graphic::Image* pMask = pDoc->diInfo.GetMask( pDoc->diInfo.CurrentFrame(), 0 );

    if ( pImage->GetWidth() > 2 )
    {
      pDummy = new GR::Graphic::Image( pImage->GetWidth() - 1,
                              pImage->GetHeight(),
                              pImage->GetDepth(), 0, 0 );

      GR::Graphic::ContextDescriptor      cdSource( pImage, pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) );
      GR::Graphic::ContextDescriptor      cdTarget( pDummy, pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) );

      cdSource.CopyArea( 0, 0, pImage->GetWidth() - 1, pImage->GetHeight(), 0, 0, &cdTarget );

      pActiveLayer->SetLayerImage( pDummy );

      // Alpha-Maske anpassen!
      if ( pMask )
      {
        GR::Graphic::Image* pMaskDummy = new GR::Graphic::Image( pMask->GetWidth() - 1,
                                                 pMask->GetHeight(),
                                                 pMask->GetDepth() );

        GR::Graphic::ContextDescriptor      cdSource( pMask );
        GR::Graphic::ContextDescriptor      cdTarget( pMaskDummy );

        cdSource.CopyArea( 0, 0, pMask->GetWidth() - 1, pMask->GetHeight(), 0, 0, &cdTarget );

        pActiveLayer->SetLayerMask( pMaskDummy );
      }

      LetterSizeModified( pDummy->GetWidth(), pDummy->GetHeight() );
    }
    else
    {
      *pResult = 1;
      return;
    }
  }
  else if ( pNMUpDown->iDelta == 1 )
  {
    GR::Graphic::Image* pImage = pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 );

    GR::Graphic::Image* pMask = pDoc->diInfo.GetMask( pDoc->diInfo.CurrentFrame(), 0 );

    if ( pImage == NULL )
    {
      pImage = new GR::Graphic::Image( 1, 1, pDoc->diInfo.m_BitDepth );

      pDoc->diInfo.AddLayer( pImage, pDoc->diInfo.CurrentFrame() );
      pActiveLayer = pDoc->diInfo.ActiveLayer();
      *pResult = 0;
      return;
    }
    if ( pImage->GetWidth() < 200 )
    {
      pDummy = new GR::Graphic::Image( pImage->GetWidth() + 1,
                              pImage->GetHeight(),
                              pImage->GetDepth(), 0, 0 );

      GR::Graphic::ContextDescriptor      cdSource( pImage, pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) );
      GR::Graphic::ContextDescriptor      cdTarget( pDummy, pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) );

      cdSource.CopyArea( 0, 0, pImage->GetWidth(), pImage->GetHeight(), 0, 0, &cdTarget );

      pActiveLayer->SetLayerImage( pDummy );

      // Alpha-Maske anpassen!
      if ( pMask )
      {
        GR::Graphic::Image* pMaskDummy = new GR::Graphic::Image( pMask->GetWidth() + 1,
                                                 pMask->GetHeight(),
                                                 pMask->GetDepth() );

        GR::Graphic::ContextDescriptor      cdSource( pMask );
        GR::Graphic::ContextDescriptor      cdTarget( pMaskDummy );

        cdSource.CopyArea( 0, 0, pMask->GetWidth(), pMask->GetHeight(), 0, 0, &cdTarget );

        pActiveLayer->SetLayerMask( pMaskDummy );
      }

      LetterSizeModified( pDummy->GetWidth(), pDummy->GetHeight() );
    }
    else
    {
      *pResult = 1;
      return;
    }
  }
	*pResult = 0;
}



void CPainterFontView::OnDeltaposSpinFontHoehe(NMHDR* pNMHDR, LRESULT* pResult) 
{

	NM_UPDOWN         *pNMUpDown;
  
  CPainterFontDoc             *pDoc;

  GR::Graphic::Image                   *pDummy;

  
  pNMUpDown = (NM_UPDOWN*)pNMHDR;
  pDoc = GetDocument();

  CLayer*   pActiveLayer = pDoc->diInfo.ActiveLayer();

  if ( pNMUpDown->iDelta == -1 )
  {
    // Buchstaben-Höhe verkleinern
    GR::Graphic::Image* pImage = pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 );

    GR::Graphic::Image* pMask = pDoc->diInfo.GetMask( pDoc->diInfo.CurrentFrame(), 0 );

    if ( pImage->GetHeight() > 2 )
    {
      pDummy = new GR::Graphic::Image( pImage->GetWidth(),
                              pImage->GetHeight() - 1,
                              pImage->GetDepth(), 0, 0 );

      GR::Graphic::ContextDescriptor      cdSource( pImage, pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) );
      GR::Graphic::ContextDescriptor      cdTarget( pDummy, pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) );

      cdSource.CopyArea( 0, 0, pImage->GetWidth(), pImage->GetHeight() - 1, 0, 0, &cdTarget );

      pActiveLayer->SetLayerImage( pDummy );

      // Alpha-Maske anpassen!
      if ( pMask )
      {
        GR::Graphic::Image* pMaskDummy = new GR::Graphic::Image( pMask->GetWidth(),
                                                 pMask->GetHeight() - 1,
                                                 pMask->GetDepth() );

        GR::Graphic::ContextDescriptor      cdSource( pMask );
        GR::Graphic::ContextDescriptor      cdTarget( pMaskDummy );

        cdSource.CopyArea( 0, 0, pMask->GetWidth(), pMask->GetHeight() - 1, 0, 0, &cdTarget );

        pActiveLayer->SetLayerMask( pMaskDummy );
      }

      LetterSizeModified( pDummy->GetWidth(), pDummy->GetHeight() );
    }
    else
    {
      *pResult = 1;
      return;
    }
  }
  else if ( pNMUpDown->iDelta == 1 )
  {
    GR::Graphic::Image* pImage = pDoc->diInfo.GetImage( pDoc->diInfo.CurrentFrame(), 0 );

    GR::Graphic::Image* pMask = pDoc->diInfo.GetMask( pDoc->diInfo.CurrentFrame(), 0 );

    if ( pImage == NULL )
    {
      pImage = new GR::Graphic::Image( 1, 1, pDoc->diInfo.m_BitDepth );

      pDoc->diInfo.AddLayer( pImage, pDoc->diInfo.CurrentFrame() );
      pActiveLayer = pDoc->diInfo.ActiveLayer();
      *pResult = 0;
      return;
    }

    if ( pImage->GetHeight() < 200 )
    {
      pDummy = new GR::Graphic::Image( pImage->GetWidth(),
                              pImage->GetHeight() + 1,
                              pImage->GetDepth(), 0, 0 );

      GR::Graphic::ContextDescriptor      cdSource( pImage, pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) );
      GR::Graphic::ContextDescriptor      cdTarget( pDummy, pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) );

      cdSource.CopyArea( 0, 0, pImage->GetWidth(), pImage->GetHeight(), 0, 0, &cdTarget );

      pActiveLayer->SetLayerImage( pDummy );

      // Alpha-Maske anpassen!
      if ( pMask )
      {
        GR::Graphic::Image* pMaskDummy = new GR::Graphic::Image( pMask->GetWidth(),
                                                 pMask->GetHeight() + 1,
                                                 pMask->GetDepth() );

        GR::Graphic::ContextDescriptor      cdSource( pMask );
        GR::Graphic::ContextDescriptor      cdTarget( pMaskDummy );

        cdSource.CopyArea( 0, 0, pMask->GetWidth(), pMask->GetHeight(), 0, 0, &cdTarget );

        pActiveLayer->SetLayerMask( pMaskDummy );
      }

      LetterSizeModified( pDummy->GetWidth(), pDummy->GetHeight() );
    }
    else
    {
      *pResult = 1;
      return;
    }
  }
	*pResult = 0;
}



void CPainterFontView::OnBearbeitenPasteasselection() 
{
  CPainterFontDoc*          pDoc;


  pDoc = GetDocument();
  if ( pDoc == NULL )
  {
    return;
  }
  if ( OpenClipboard() )
  {
    pDoc->diInfo.PasteFromClipBoard( 0 );
    CloseClipboard();
    Invalidate( FALSE );
  }
  else
  {
    MessageBox( _T( "failed to open clipboard" ), _T( "mist" ), MB_OK | MB_APPLMODAL );
  }
  theApp.ShowStatusMessage();
}



void CPainterFontView::OnEditCopy() 
{
  m_viewInfo.CopyToClipboard();
}



void CPainterFontView::OnSize( UINT nType, int cx, int cy )
{
  RECT              rc,
                    rc2;


	CFormView::OnSize( nType, cx, cy );

  if ( hwndFontWorkArea == NULL )
  {
    return;
  }

  CPainterFontDoc*  pDoc = GetDocument();
  if ( pDoc != NULL )
  {
    GetClientRect( &rc );
    ::GetClientRect( hwndFontWorkArea, &rc2 );
    if ( rc2.left < cx )
    {
      ::SetWindowPos( hwndFontWorkArea, NULL, 0, 0, rc.right - rc2.left, rc.bottom - rc2.top, SWP_NOZORDER | SWP_NOMOVE );
    }
  }
}



void CPainterFontView::DoUpdate( LPARAM lHint, GR::tRect* pRect )
{
  if ( m_viewInfo.m_RedrawFlags != ViewInfo::REDRAW_ALL )
  {
    m_viewInfo.m_RedrawFlags = (DWORD)lHint;
  }

  if ( m_viewInfo.m_RedrawFlags == ViewInfo::REDRAW_RECT )
  {
    if ( pRect != NULL )
    {
      m_viewInfo.m_rectRedraw.Combine( *pRect );
    }
  }
  if ( m_viewInfo.m_RedrawFlags == ViewInfo::REDRAW_ALL )
  {
    RECT     rc;

    m_StaticLetter.GetClientRect( &rc );
    
    m_viewInfo.m_rectRedraw.Set( rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top );
  }
  m_viewInfo.DisplayPage();
  if ( hwndFontWorkArea )
  {
    ::InvalidateRect( hwndFontWorkArea, NULL, FALSE );
    ::UpdateWindow( hwndFontWorkArea );
  }
}



void CPainterFontView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
  if ( lHint == ViewInfo::REDRAW_UPDATE_VIEW_INFO )
  {
    if ( pSender != this )
    {
      m_DoNotUpdate = true;
      m_EditZeichen.SetWindowText( GR::Convert::ToUTF16( ( Misc::Format() << m_viewInfo.m_pDocInfo->CurrentFrame() ).Result() ).c_str() );
      m_DoNotUpdate = false;

      m_EditWidth.SetWindowText( GR::Convert::ToUTF16( ( Misc::Format() << m_viewInfo.m_pDocInfo->GetImage( m_viewInfo.m_pDocInfo->CurrentFrame(), 0 )->GetWidth() ).Result() ).c_str() );
      m_EditHeight.SetWindowText( GR::Convert::ToUTF16( ( Misc::Format() << m_viewInfo.m_pDocInfo->GetImage( m_viewInfo.m_pDocInfo->CurrentFrame(), 0 )->GetHeight() ).Result() ).c_str() );
    }
    lHint = ViewInfo::REDRAW_ALL;
  }
  DoUpdate( lHint, (GR::tRect*)pHint );
}



void CPainterFontView::RedrawFontPreview()
{
  if ( m_FontPreview.m_pPage )
  {
    m_FontPreview.m_pPage->Box( 0, 0, m_FontPreview.m_pPage->GetWidth() - 1, m_FontPreview.m_pPage->GetHeight() - 1, 0 );

    GR::Graphic::Image*   pLetter = m_viewInfo.m_pDocInfo->GetImage( m_viewInfo.m_pDocInfo->CurrentFrame(), 0 );
    if ( pLetter )
    {
      pLetter->PutImage( m_FontPreview.m_pPage, 10, 10 );
    }
  }
}



void CPainterFontView::OnDestroy()
{
  if ( m_FontPreview.GetSafeHwnd() )
  {
    m_FontPreview.Detach();
  }

  CFormView::OnDestroy();
}



void CPainterFontView::OnRButtonDown(UINT nFlags, CPoint point)
{
  RECT                      rc,
                            rcParent;
  

  m_viewInfo.m_ptScrollOffset = GetScrollPosition();

  m_StaticLetter.GetParent()->GetWindowRect( &rcParent );
  m_StaticLetter.GetWindowRect( &rc );
  point.x -= ( rc.left - rcParent.left );

  point.x = ( ( point.x * 100 ) / m_viewInfo.m_ZoomFaktor );
  point.y = ( ( point.y * 100 ) / m_viewInfo.m_ZoomFaktor );

  pSettings->SnapToGrid( &point );

  m_viewInfo.GetWorkColorFromPixel( point.x, point.y );

  CFormView::OnRButtonDown( nFlags, point );

  theApp.ShowStatusMessage();

  m_StaticLetter.SetFocus();
}



void CPainterFontView::OnSplitRGB() 
{

  CFrameWnd * pFrameWnd = theApp.pFontTemplate->CreateNewFrame( GetDocument(), NULL );

  // error checking in case creation of frame or view fails
  if ( pFrameWnd == NULL )
  {
    return;
  }

  // let the frame do any initialization the framework needs,
  // call the view's OnInitialUpdate()
  theApp.pDocTemplate->InitialUpdateFrame( pFrameWnd, m_pDocument );
  CPainterFontView * pView = (CPainterFontView*)pFrameWnd->GetActiveView();
  pView->m_viewInfo.InitializePage( ViewInfo::VI_ALPHA );

}



void CPainterFontView::OnSplitAlpha() 
{

  CFrameWnd * pFrameWnd = theApp.pFontTemplate->CreateNewFrame( GetDocument(), NULL );

  // error checking in case creation of frame or view fails
  if ( pFrameWnd == NULL )
  {
    return;
  }

  // let the frame do any initialization the framework needs,
  // call the view's OnInitialUpdate()
  theApp.pDocTemplate->InitialUpdateFrame( pFrameWnd, m_pDocument );
  CPainterFontView * pView = (CPainterFontView*)pFrameWnd->GetActiveView();
  pView->m_viewInfo.InitializePage( ViewInfo::VI_ALPHA );
	
}



void CPainterFontView::LetterSizeModified( int iWidth, int iHeight )
{

  CPainterFontDoc*  pDoc = GetDocument();

  pDoc->UpdateAllViews( this, ViewInfo::REDRAW_UPDATE_VIEW_INFO );

  DoUpdate( ViewInfo::REDRAW_ALL );

}



void CPainterFontView::LetterNumberChanged( int iChar )
{
  CPainterFontDoc*  pDoc = GetDocument();

  pDoc->UpdateAllViews( this, ViewInfo::REDRAW_UPDATE_VIEW_INFO );
  DoUpdate( ViewInfo::REDRAW_ALL );
  //m_StaticLetter.Invalidate();
}



void CPainterFontView::OnBnClickedBtnKillLetter()
{
  // TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
}



BOOL CPainterFontView::PreTranslateMessage( MSG* pMsg )
{
  if ( pMsg->hwnd == m_StaticLetter.GetSafeHwnd() )
  {
    if ( pMsg->message == WM_KEYUP )
    {
      OnKeyUp( pMsg->wParam, 0, 0 );
      return TRUE;
    }
  }

  return CFormView::PreTranslateMessage( pMsg );
}



void CPainterFontView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  BOOL                  bShift = FALSE,
                        bControl = FALSE;

  CPainterFontDoc*      pDoc = GetDocument();


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

  if ( nChar == VK_RIGHT )
  {
    if ( bControl )
    {
      pDoc->diInfo.Shift( 1, 0, pDoc->diInfo.CurrentFrame() );
    }
  }
  else if ( nChar == VK_LEFT )
  {
    if ( bControl )
    {
      pDoc->diInfo.Shift( -1, 0, pDoc->diInfo.CurrentFrame() );
    }
  }
  else if ( nChar == VK_UP )
  {
    if ( bControl )
    {
      pDoc->diInfo.Shift( 0, -1, pDoc->diInfo.CurrentFrame() );
    }
  }
  else if ( nChar == VK_DOWN )
  {
    if ( bControl )
    {
      pDoc->diInfo.Shift( 0, 1, pDoc->diInfo.CurrentFrame() );
    }
  }

  CFormView::OnKeyUp( nChar, nRepCnt, nFlags );
}



