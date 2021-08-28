#include "stdafx.h"
#include "DlgEffekte.h"
#include "PainterGFXTools.h"

#include "EffektBrightness.h"
#include "EffektContrast.h"
#include "EffektAura.h"
#include "EffektGrayScale.h"
#include "EffektPixelize.h"
#include "EffektColorize.h"
#include "EffektDiffusor.h"
#include "EffektSmoothen.h"
#include "EffektJalousie.h"
#include "EffektReduce.h"
#include "EffektColorDiffusor.h"
#include "EffektPerlinNoise.h"
#include "EffektPlasma.h"
#include "EffektBevel.h"
#include "EffektShadow.h"
#include "EffektTileable.h"

#include "Settings.h"

#include <Misc/Misc.h>
#include <String/Convert.h>


#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


enum CDlgEffekte::eEffekte
{
  E_NONE,
  E_BRIGHTNESS,
  E_CONTRAST,
  E_AURA,
  E_GRAYSCALE,
  E_COLORIZE,
  E_DIFFUSOR,
  E_SMOOTHEN,
  E_JALOUSIE,
  E_REDUCE,
  E_PIXELIZE,
  E_COLOR_DIFFUSOR,
  E_PERLIN_NOISE,
  E_BEVEL,
  E_SHADOW,
  E_PLASMA,
  E_TILEABLE,
};

/////////////////////////////////////////////////////////////////////////////
// CDlgEffekte dialog


CDlgEffekte::CDlgEffekte(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEffekte::IDD, pParent),
    m_pCurrentEffekt( NULL ),
    m_eAppliedFilter( E_NONE ),
    m_pImageMask( NULL )
{

	//{{AFX_DATA_INIT(CDlgEffekte)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  pDocInfo = NULL;
  ThreadPleaseExit = FALSE;
  ThreadRunning = FALSE;
  hFilterThread = NULL;
  pPagePreFilter = NULL;
  pPagePostFilter = NULL;
  ZoomFaktor = 100;

  PreXOffset = 0;
  PreYOffset = 0;
  PostXOffset = 0;
  PostYOffset = 0;
}



CDlgEffekte::~CDlgEffekte()
{
  if ( m_pCurrentEffekt )
  {
    m_pCurrentEffekt->DestroyWindow();
    SafeDelete( m_pCurrentEffekt );
  }
}



void CDlgEffekte::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange( pDX );
  //{{AFX_DATA_MAP(CDlgEffekte)
  DDX_Control( pDX, IDC_TREE_EFFEKTE, m_TreeEffekte );
  //}}AFX_DATA_MAP
  DDX_Control( pDX, IDC_STATIC_FILTER_SETTINGS, m_StaticFilterSettings );
}



BEGIN_MESSAGE_MAP(CDlgEffekte, CDialog)
	//{{AFX_MSG_MAP(CDlgEffekte)
	  ON_BN_CLICKED(IDC_BUTTON_FILTER_ZOOMFAKTOR, OnButtonFilterZoomfaktor)
	  ON_WM_DRAWITEM()
	  ON_WM_LBUTTONDOWN()
	  ON_WM_LBUTTONUP()
	  ON_WM_MOUSEMOVE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_EFFEKTE, OnSelchangedTreeEffekte)
	ON_BN_CLICKED(IDC_BUTTON_RENDER, OnButtonRender)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CDlgEffekte::UpdateThread()
{
  if ( ThreadPleaseExit )
  {
    return FALSE;
  }
  if ( GetTickCount() - LastUpdate >= 1000 )
  {
    LastUpdate = GetTickCount();
    if ( GetSafeHwnd() )
    {
      GetDlgItem( IDC_STATIC_POST_VIEW )->Invalidate();
    }
  }
  return TRUE;
}



DWORD WINAPI CallEffekt( LPVOID lParam )
{
  CDlgEffekte* pDlg = (CDlgEffekte*)lParam;

  if ( ( !pDlg )
  ||   ( !pDlg->m_pCurrentEffekt ) )
  {
    pDlg->ThreadRunning = FALSE;
    return 0;
  }

  EffektTemplate   *pEffekt = pDlg->m_pCurrentEffekt;

  pEffekt->m_pDlg = pDlg;
  pEffekt->m_pCDSource  = new CMaskedContextDescriptor( pDlg->pImageSource,    pDlg->pPalette );
  pEffekt->m_pCDTarget  = new CMaskedContextDescriptor( pDlg->pPagePostFilter,  pDlg->pPalette );

  if ( pDlg->m_pImageMask )
  {
    pEffekt->m_pCDSource->m_pMaskData = pDlg->m_pImageMask->GetData();
  }

  GR::Graphic::Image*   pImageTarget2 = new GR::Graphic::Image( pDlg->pImageSource );

  pEffekt->m_pCDLastFrame = new CMaskedContextDescriptor( pImageTarget2,    pDlg->pPalette );

  pEffekt->m_FramePos = 0;
  pEffekt->Init();
  pEffekt->DoEffekt();
  pEffekt->CleanUp();

  if ( pEffekt->m_FramePos )
  {
    pImageTarget2->PutImage( pDlg->pPagePostFilter, 0, 0 );
  }

  SafeDelete( pEffekt->m_pCDLastFrame );
  SafeDelete( pImageTarget2 );
  SafeDelete( pEffekt->m_pCDSource );
  SafeDelete( pEffekt->m_pCDTarget );

  pDlg->GetDlgItem( IDC_STATIC_POST_VIEW )->Invalidate();
  pDlg->ThreadRunning = false;

  ExitThread( 0 );

  return 0;

}



void CDlgEffekte::StopThread()
{
  if ( hFilterThread != NULL )
  {
    // der Thread läuft noch, beenden
    ThreadPleaseExit = true;
    while ( ThreadRunning )
    {
      Sleep( 10 );
    }
    Sleep( 10 );
    CloseHandle( hFilterThread );
    hFilterThread = NULL;
  }
}



void CDlgEffekte::ApplyFilter( eEffekte dwFilter )
{
  DWORD       dwThreadID;


  StopThread();

  if ( m_pCurrentEffekt )
  {
    m_pCurrentEffekt->DestroyWindow();
    SafeDelete( m_pCurrentEffekt );
  }

  m_eAppliedFilter = dwFilter;
  switch ( m_eAppliedFilter )
  {
    case E_BRIGHTNESS:
      m_pCurrentEffekt = new CEffektBrightness();
      break;
    case E_CONTRAST:
      m_pCurrentEffekt = new CEffektContrast();
      break;
    case E_AURA:
      m_pCurrentEffekt = new CEffektAura();
      break;
    case E_GRAYSCALE:
      m_pCurrentEffekt = new CEffektGrayScale();
      break;
    case E_DIFFUSOR:
      m_pCurrentEffekt = new CEffektDiffusor();
      break;
    case E_SMOOTHEN:
      m_pCurrentEffekt = new CEffektSmoothen();
      break;
    case E_REDUCE:
      m_pCurrentEffekt = new CEffektReduce();
      break;
    case E_JALOUSIE:
      m_pCurrentEffekt = new CEffektJalousie();
      break;
    case E_COLORIZE:
      m_pCurrentEffekt = new CEffektColorize();
      break;
    case E_PIXELIZE:
      m_pCurrentEffekt = new CEffektPixelize();
      break;
    case E_COLOR_DIFFUSOR:
      m_pCurrentEffekt = new CEffektColorDiffusor();
      break;
    case E_PERLIN_NOISE:
      m_pCurrentEffekt = new CEffektPerlinNoise();
      break;
    case E_PLASMA:
      m_pCurrentEffekt = new CEffektPlasma();
      break;
    case E_BEVEL:
      m_pCurrentEffekt = new CEffektBevel();
      break;
    case E_SHADOW:
      m_pCurrentEffekt = new CEffektShadow();
      break;
    case E_TILEABLE:
      m_pCurrentEffekt = new CEffektTileable();
      break;
    default:
      m_pCurrentEffekt = NULL;
      break;
  }

  pImageSource->PutImage( pPagePostFilter, 0, 0, IMAGE_METHOD_PLAIN );
  LastUpdate = GetTickCount();

  if ( m_pCurrentEffekt )
  {
    ThreadPleaseExit = FALSE;

    // Dialog einsetzen
    if ( !m_pCurrentEffekt->Create( MAKEINTRESOURCE( m_pCurrentEffekt->GetID() ), this ) )
    {
      MessageBox( _T( "Create failed" ) );
    }
    RECT    rc;
    m_StaticFilterSettings.GetClientRect( &rc );

    m_StaticFilterSettings.ClientToScreen( &rc );
    ScreenToClient( &rc );

    InflateRect( &rc, -15, -15 );

    //m_pCurrentEffekt->SetWindowPos( NULL, 240, 355, 380, 135, SWP_NOZORDER | SWP_DRAWFRAME );
    m_pCurrentEffekt->SetWindowPos( NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DRAWFRAME );

    m_pCurrentEffekt->ShowWindow( SW_SHOW );

    ThreadRunning = TRUE;
    hFilterThread = CreateThread( NULL, 0, CallEffekt, this, 0, &dwThreadID );
  }
}



void CDlgEffekte::AddEffekt( const GR::tChar* szDesc, eEffekte eEnum, HTREEITEM hItemParent )
{
  HTREEITEM   hItemEffekt = m_TreeEffekte.InsertItem( szDesc, hItemParent );

  m_TreeEffekte.SetItemData( hItemEffekt, eEnum );
}



BOOL CDlgEffekte::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  if ( pDocInfo == NULL )
  {
    return FALSE;
  }

  // Effekte in Tree
  HTREEITEM   hItemGroup;

  hItemGroup  = m_TreeEffekte.InsertItem( _T( "Farbwert-Effekte" ) );

  AddEffekt( _T( "Brightness" ),      E_BRIGHTNESS,       hItemGroup );
  AddEffekt( _T( "Contrast" ),        E_CONTRAST,         hItemGroup );
  AddEffekt( _T( "Aura" ),            E_AURA,             hItemGroup );
  AddEffekt( _T( "GrayScale" ),       E_GRAYSCALE,        hItemGroup );
  AddEffekt( _T( "Colorize" ),        E_COLORIZE,         hItemGroup );
  AddEffekt( _T( "Diffusor" ),        E_DIFFUSOR,         hItemGroup );
  AddEffekt( _T( "Smoothen" ),        E_SMOOTHEN,         hItemGroup );
  AddEffekt( _T( "Jalousie" ),        E_JALOUSIE,         hItemGroup );
  AddEffekt( _T( "Reduce" ),          E_REDUCE,           hItemGroup );
  AddEffekt( _T( "Pixelize" ),        E_PIXELIZE,         hItemGroup );
  AddEffekt( _T( "Color-Diffusor" ),  E_COLOR_DIFFUSOR,   hItemGroup );
  AddEffekt( _T( "Bevel" ),           E_BEVEL,            hItemGroup );  
  AddEffekt( _T( "Schattenwurf" ),    E_SHADOW,           hItemGroup );  
  AddEffekt( _T( "Tileable" ),        E_TILEABLE,         hItemGroup );  

  hItemGroup  = m_TreeEffekte.InsertItem( _T( "Generatoren" ) );

  AddEffekt( _T( "Plasma" ),          E_PLASMA,   hItemGroup );
  AddEffekt( _T( "Perlin Noise" ),    E_PERLIN_NOISE,     hItemGroup );

  m_eUsedFilter = E_NONE;

  GetDlgItem( IDC_STATIC_FILTER_INFO )->SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%dx%dx%d", pImageSource->GetWidth(), pImageSource->GetHeight(), pImageSource->GetDepth() ) ).c_str() );

  GetDlgItem( IDC_STATIC_PRE_VIEW )->ModifyStyle( 0, SS_OWNERDRAW );
  pPagePreFilter = new GR::Graphic::GDIPage();
  pPagePreFilter->Create( GetDlgItem( IDC_STATIC_PRE_VIEW )->GetSafeHwnd(), pImageSource->GetWidth(), pImageSource->GetHeight(), pImageSource->GetDepth() );

  if ( pImageSource->GetDepth() <= 8 )
  {
    pPagePreFilter->SetPalette( pPalette );
  }

  pImageSource->PutImage( pPagePreFilter, 0, 0, IMAGE_METHOD_PLAIN );

  GetDlgItem( IDC_STATIC_POST_VIEW )->ModifyStyle( 0, SS_OWNERDRAW );
  pPagePostFilter = new GR::Graphic::GDIPage();
  pPagePostFilter->Create( GetDlgItem( IDC_STATIC_PRE_VIEW )->GetSafeHwnd(), pImageSource->GetWidth(), pImageSource->GetHeight(), pImageSource->GetDepth() );

  if ( pImageSource->GetDepth() <= 8 )
  {
    pPagePostFilter->SetPalette( pPalette );
  }

  pImageSource->PutImage( pPagePostFilter, 0, 0, IMAGE_METHOD_PLAIN );

  hFilterThread = NULL;
  ThreadPleaseExit = FALSE;
  ApplyFilter( m_eUsedFilter );
	
	return TRUE;
}



void CDlgEffekte::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct )
{
  RECT              rc;

  int               iXOffset,
                    iYOffset,
                    iWidth,
                    iHeight;


  iWidth = pImageSource->GetWidth() * ZoomFaktor / 100;
  iHeight = pImageSource->GetHeight() * ZoomFaktor / 100;

  iXOffset = 0;//( ( lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left ) - iWidth ) / 2;
  iYOffset = 0;//( ( lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top ) - iHeight ) / 2;

  if ( lpDrawItemStruct->hwndItem == GetDlgItem( IDC_STATIC_PRE_VIEW )->GetSafeHwnd() )
  {
    iXOffset += PreXOffset;
    iYOffset += PreYOffset;
    if ( iXOffset > 0 )
    {
      // links leer!
      rc.left = lpDrawItemStruct->rcItem.left;
      rc.top = lpDrawItemStruct->rcItem.top;
      rc.right = lpDrawItemStruct->rcItem.left + iXOffset;
      rc.bottom = lpDrawItemStruct->rcItem.bottom;
      FillRect( lpDrawItemStruct->hDC, &rc, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
    }
    if ( lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left > iXOffset + iWidth )
    {
      // rechts leer!
      rc.left = lpDrawItemStruct->rcItem.left + iXOffset + iWidth;
      rc.top = lpDrawItemStruct->rcItem.top;
      rc.right = lpDrawItemStruct->rcItem.right;
      rc.bottom = lpDrawItemStruct->rcItem.bottom;
      FillRect( lpDrawItemStruct->hDC, &rc, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
    }
    if ( iYOffset > 0 )
    {
      // oben leer!
      rc.left = lpDrawItemStruct->rcItem.left + iXOffset;
      rc.top = lpDrawItemStruct->rcItem.top;
      rc.right = lpDrawItemStruct->rcItem.left + iXOffset + iWidth;
      rc.bottom = lpDrawItemStruct->rcItem.top + iYOffset;
      FillRect( lpDrawItemStruct->hDC, &rc, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
    }
    if ( lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top > iYOffset + iHeight )
    {
      // unten leer!
      rc.left = lpDrawItemStruct->rcItem.left + iXOffset;
      rc.top = lpDrawItemStruct->rcItem.top + iYOffset + iHeight;
      rc.right = lpDrawItemStruct->rcItem.left + iXOffset + iWidth;
      rc.bottom = lpDrawItemStruct->rcItem.bottom;
      FillRect( lpDrawItemStruct->hDC, &rc, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
    }
    SetStretchBltMode( lpDrawItemStruct->hDC, COLORONCOLOR );

    StretchBlt( lpDrawItemStruct->hDC, 
                iXOffset, iYOffset, 
                iWidth,
                iHeight,
                pPagePreFilter->GetDC(), 0, 0, 
                pPagePreFilter->GetWidth(), pPagePreFilter->GetHeight(),
                SRCCOPY );
  }
  else if ( lpDrawItemStruct->hwndItem == GetDlgItem( IDC_STATIC_POST_VIEW )->GetSafeHwnd() )
  {
    iXOffset += PostXOffset;
    iYOffset += PostYOffset;
    if ( iXOffset > 0 )
    {
      // links leer!
      rc.left = lpDrawItemStruct->rcItem.left;
      rc.top = lpDrawItemStruct->rcItem.top;
      rc.right = lpDrawItemStruct->rcItem.left + iXOffset;
      rc.bottom = lpDrawItemStruct->rcItem.bottom;
      FillRect( lpDrawItemStruct->hDC, &rc, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
    }
    if ( lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left > iXOffset + iWidth )
    {
      // rechts leer!
      rc.left = lpDrawItemStruct->rcItem.left + iXOffset + iWidth;
      rc.top = lpDrawItemStruct->rcItem.top;
      rc.right = lpDrawItemStruct->rcItem.right;
      rc.bottom = lpDrawItemStruct->rcItem.bottom;
      FillRect( lpDrawItemStruct->hDC, &rc, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
    }
    if ( iYOffset > 0 )
    {
      // oben leer!
      rc.left = lpDrawItemStruct->rcItem.left + iXOffset;
      rc.top = lpDrawItemStruct->rcItem.top;
      rc.right = lpDrawItemStruct->rcItem.left + iXOffset + iWidth;
      rc.bottom = lpDrawItemStruct->rcItem.top + iYOffset;
      FillRect( lpDrawItemStruct->hDC, &rc, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
    }
    if ( lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top > iYOffset + iHeight )
    {
      // unten leer!
      rc.left = lpDrawItemStruct->rcItem.left + iXOffset;
      rc.top = lpDrawItemStruct->rcItem.top + iYOffset + iHeight;
      rc.right = lpDrawItemStruct->rcItem.left + iXOffset + iWidth;
      rc.bottom = lpDrawItemStruct->rcItem.bottom;
      FillRect( lpDrawItemStruct->hDC, &rc, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
    }
    SetStretchBltMode( lpDrawItemStruct->hDC, COLORONCOLOR );
    StretchBlt( lpDrawItemStruct->hDC, iXOffset, iYOffset, 
                iWidth,
                iHeight,
                pPagePostFilter->GetDC(), 0, 0, 
                pPagePostFilter->GetWidth(), pPagePostFilter->GetHeight(),
                SRCCOPY );
  }

	
	CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
}



void CDlgEffekte::OnOK() 
{
  // warten, bis der Filter fertig ist
  GetDlgItem( IDOK )->EnableWindow( FALSE );
  if ( hFilterThread != NULL )
  {
    while ( ThreadRunning )
    {
      Sleep( 10 );
    }
    CloseHandle( hFilterThread );
    hFilterThread = NULL;
  }
  AsNewLayer = !!( (CButton*)GetDlgItem( IDC_CHECK_FILTER_NEWLAYER ) )->GetCheck();
  
  pImageSource->GetImage( pPagePostFilter, 0, 0 );
  if ( pPagePreFilter != NULL )
  {
    delete pPagePreFilter;
    pPagePreFilter = NULL;
  }
  if ( pPagePostFilter != NULL )
  {
    delete pPagePostFilter;
    pPagePostFilter = NULL;
  }

	CDialog::OnOK();
}



void CDlgEffekte::OnCancel() 
{
  StopThread();

  if ( pPagePreFilter != NULL )
  {
    delete pPagePreFilter;
    pPagePreFilter = NULL;
  }
  if ( pPagePostFilter != NULL )
  {
    delete pPagePostFilter;
    pPagePostFilter = NULL;
  }

	CDialog::OnCancel();
}



void CDlgEffekte::OnButtonFilterZoomfaktor() 
{
  ZoomFaktor <<= 1;
  if ( ZoomFaktor >= 3200 )
  {
    ZoomFaktor = 25;
  }
  GetDlgItem( IDC_STATIC_FILTER_INFO )->SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%dx%dx%d (%d%%)", pImageSource->GetWidth(), pImageSource->GetHeight(), pImageSource->GetDepth(), ZoomFaktor ) ).c_str() );

  GetDlgItem( IDC_STATIC_POST_VIEW )->Invalidate();
  GetDlgItem( IDC_STATIC_PRE_VIEW )->Invalidate();

  WrapCoord( PreXOffset, PreYOffset, IDC_STATIC_PRE_VIEW );
  WrapCoord( PostXOffset, PostYOffset, IDC_STATIC_POST_VIEW );
}



void CDlgEffekte::OnLButtonDown( UINT nFlags, CPoint point )
{
  RECT          rc;

  ClientToScreen( &point );
  MouseX = point.x;
  MouseY = point.y;
  GetDlgItem( IDC_STATIC_PRE_VIEW )->GetWindowRect( &rc );
  Inside = 0;
  if ( ( MouseX >= rc.left )
  &&   ( MouseY >= rc.top )
  &&   ( MouseX <= rc.right )
  &&   ( MouseY <= rc.bottom ) )
  {
    Inside = 1;
    ClipCursor( &rc );
  }
  GetDlgItem( IDC_STATIC_POST_VIEW )->GetWindowRect( &rc );
  if ( ( MouseX >= rc.left )
  &&   ( MouseY >= rc.top )
  &&   ( MouseX <= rc.right )
  &&   ( MouseY <= rc.bottom ) )
  {
    Inside = 2;
    ClipCursor( &rc );
  }
	
	CDialog::OnLButtonDown(nFlags, point);
}



void CDlgEffekte::OnLButtonUp( UINT nFlags, CPoint point )
{
  ClientToScreen( &point );
  if ( Inside == 1 )
  {
    PreXOffset += ( point.x - MouseX );
    PreYOffset += ( point.y - MouseY );
    MouseX = point.x;
    MouseY = point.y;
    GetDlgItem( IDC_STATIC_PRE_VIEW )->Invalidate();

    WrapCoord( PreXOffset, PreYOffset, IDC_STATIC_PRE_VIEW );
  }
  else if ( Inside == 2 )
  {
    PostXOffset += ( point.x - MouseX );
    PostYOffset += ( point.y - MouseY );
    MouseX = point.x;
    MouseY = point.y;
    GetDlgItem( IDC_STATIC_POST_VIEW )->Invalidate();

    WrapCoord( PostXOffset, PostYOffset, IDC_STATIC_POST_VIEW );
  }
  Inside = 0;
  ClipCursor( NULL );
	CDialog::OnLButtonUp( nFlags, point );
}



void CDlgEffekte::OnMouseMove( UINT nFlags, CPoint point )
{
  ClientToScreen( &point );	
  if ( Inside == 1 )
  {
    PreXOffset += ( point.x - MouseX );
    PreYOffset += ( point.y - MouseY );
    MouseX = point.x;
    MouseY = point.y;
    GetDlgItem( IDC_STATIC_PRE_VIEW )->Invalidate();

    WrapCoord( PreXOffset, PreYOffset, IDC_STATIC_PRE_VIEW );

  }
  else if ( Inside == 2 )
  {
    PostXOffset += ( point.x - MouseX );
    PostYOffset += ( point.y - MouseY );
    MouseX = point.x;
    MouseY = point.y;
    GetDlgItem( IDC_STATIC_POST_VIEW )->Invalidate();

    WrapCoord( PostXOffset, PostYOffset, IDC_STATIC_POST_VIEW );
  }
	
	CDialog::OnMouseMove( nFlags, point );
}




void CDlgEffekte::OnSelchangedTreeEffekte(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

  m_eUsedFilter = (eEffekte)m_TreeEffekte.GetItemData( pNMTreeView->itemNew.hItem );

  ApplyFilter( m_eUsedFilter );
	
	*pResult = 0;
}



void CDlgEffekte::OnButtonRender() 
{
  if ( m_pCurrentEffekt )
  {
    DWORD   dwThreadID;
    StopThread();

    ThreadPleaseExit  = false;
    ThreadRunning     = true;
    hFilterThread     = CreateThread( NULL, 0, CallEffekt, this, 0, &dwThreadID );
  }
}



void CDlgEffekte::WrapCoord( int& iX, int& iY, DWORD dwResID )
{
  int iWidth = pImageSource->GetWidth() * ZoomFaktor / 100;
  int iHeight = pImageSource->GetHeight() * ZoomFaktor / 100;

  RECT    rc;

  GetDlgItem( dwResID )->GetClientRect( &rc );

  if ( ( rc.right - rc.left ) > iWidth )
  {
    if ( iX < 0 )
    {
      iX = 0;
    }
    if ( iX >= rc.right - iWidth )
    {
      iX = rc.right - iWidth;
    }
  }
  else
  {
    if ( iX > 0 )
    {
      iX = 0;
    }
    if ( iX < -( iWidth - rc.right ) )
    {
      iX = -( iWidth - rc.right );
    }
  }
  if ( ( rc.bottom - rc.top ) > iHeight )
  {
    if ( iY < 0 )
    {
      iY = 0;
    }
    if ( iY >= rc.bottom - iHeight )
    {
      iY = rc.bottom - iHeight;
    }
  }
  else
  {
    if ( iY > 0 )
    {
      iY = 0;
    }
    if ( iY < -( iHeight - rc.bottom ) )
    {
      iY = -( iHeight - rc.bottom );
    }
  }
}