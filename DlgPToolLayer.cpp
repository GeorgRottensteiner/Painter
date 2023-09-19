#include "stdafx.h"
#include "DlgPToolLayer.h"

#include "DocumentInfo.h"
#include "Layer.h"
#include "PainterGfxTools.h"
#include "Settings.h"

#include "DialogCreateMask.h"
#include "DlgRename.h"

#include "UndoImageChange.h"
#include "UndoLayerChange.h"

#include "PainterEnums.h"

#include <Misc/Misc.h>

#include <MFC/ODMenu.h>
#include <String/Convert.h>

#include <set>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



DROPEFFECT CDragMaskTarget::OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{

  for ( int i = 0; i < m_pToolLayer->m_Listm_vectLayers.GetCount(); i++ )
  {
    RECT    rc;
    m_pToolLayer->m_Listm_vectLayers.GetItemRect( i, &rc );

    if ( PtInRect( &rc, point ) )
    {
      return DROPEFFECT_COPY;
    }
  }
  return DROPEFFECT_NONE;

}



DROPEFFECT CDragMaskTarget::OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
  for ( int i = 0; i < m_pToolLayer->m_Listm_vectLayers.GetCount(); i++ )
  {
    RECT    rc;
    m_pToolLayer->m_Listm_vectLayers.GetItemRect( i, &rc );

    if ( PtInRect( &rc, point ) )
    {
      return DROPEFFECT_COPY;
    }
  }
  return DROPEFFECT_NONE;
}



BOOL CDragMaskTarget::OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{

  for ( int i = 0; i < m_pToolLayer->m_Listm_vectLayers.GetCount(); i++ )
  {
    RECT    rc;
    m_pToolLayer->m_Listm_vectLayers.GetItemRect( i, &rc );

    if ( PtInRect( &rc, point ) )
    {
      if ( dropEffect == DROPEFFECT_COPY )
      {
        // wir können nur Bitmap
        if ( !pDataObject->IsDataAvailable( CF_DIB ) )
        {
	        return FALSE;
        }

        ViewInfo   *pViewInfo = pSettings->m_pActiveViewInfo;

        if ( !pViewInfo )
        {
          pSettings->Log( "no viewinfo" );
          return FALSE;
        }


        HGLOBAL hmem = (HGLOBAL)pDataObject->GetGlobalData( CF_DIB );
        if ( hmem == NULL )
        {
	        return FALSE;
        }

        CPainterImagePackage *pPack = CreateImagePackFromHDIB( hmem );

        CLayer*   pLayer = pViewInfo->m_pDocInfo->GetLayer( pViewInfo->m_pDocInfo->CurrentFrame(), m_pToolLayer->m_Listm_vectLayers.GetItemData( i ) );

        pLayer->SetLayerMask( pPack->m_pImage );
        pLayer->m_HasMask = true;

        SafeDelete( pPack );

        m_pToolLayer->m_Listm_vectLayers.Invalidate();

        pViewInfo->m_pDocInfo->RedrawAllViews();

        GlobalUnlock( hmem );
        GlobalFree( hmem );
        pDataObject->Release();
        return TRUE;
      }

    }
  }
  return FALSE;

}


CDlgPToolLayer::CDlgPToolLayer(CWnd* pParent /*=NULL*/)
  : CDialog(CDlgPToolLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPToolLayer)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
  m_pDocInfo = NULL;

  iconActiveLayer = ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_ICON_ACTIVE_LAYER ) );
  iconVisibleLayer = ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_ICON_VISIBLE_LAYER ) );
  iconInactiveLayer = ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_ICON_INACTIVE_LAYER ) );
  iconActiveMask = ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_ICON_ACTIVE_MASK ) );
  iconInactiveMask = ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_ICON_INACTIVE_MASK ) );
  iconColorKeyLayer = ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_ICON_COLORKEY_LAYER ) );

}


void CDlgPToolLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPToolLayer)
	DDX_Control(pDX, IDC_BUTTON_TOOL_LAYER_MERGE, m_ButtonMergeVisible);
	DDX_Control(pDX, IDC_BUTTON_TOOL_LAYER_CREATE_MASK, m_ButtonCreateMask);
	DDX_Control(pDX, IDC_BUTTON_TOOL_LAYER_MERGE_ALL, m_ButtonMergeAll);
	DDX_Control(pDX, IDC_BUTTON_TOOL_LAYER_NEWLAYER, m_ButtonNewLayer);
	DDX_Control(pDX, IDC_LIST_m_vectLayers, m_Listm_vectLayers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPToolLayer, CDialog)
	//{{AFX_MSG_MAP(CDlgPToolLayer)
	ON_BN_CLICKED(IDC_BUTTON_TOOL_LAYER_NEWLAYER, OnButtonToolLayerNewLayer)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON_TOOL_LAYER_MERGE_ALL, OnButtonToolLayerMergeAll)
	ON_BN_CLICKED(IDC_BUTTON_TOOL_LAYER_MERGE, OnButtonToolLayerMerge)
	ON_BN_CLICKED(IDC_BUTTON_TOOL_LAYER_CREATE_MASK, OnButtonToolLayerCreateMask)
	ON_WM_MEASUREITEM()
	//}}AFX_MSG_MAP
  ON_WM_DESTROY()
END_MESSAGE_MAP()




static unsigned int const g_DragListMsg = RegisterWindowMessage( DRAGLISTMSGSTRING );



/*-LBAlphaRegler--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CDlgPToolLayer::LBAlphaRegler( int iX, RECT& rc )
{

  int   iNewAlpha = ( 256 * ( iX - rc.left - LAYER_ALPHA_X ) ) / LAYER_ALPHA_W;

  if ( iNewAlpha < 0 )
  {
    iNewAlpha = 0;
  }
  if ( iNewAlpha >= 256 )
  {
    iNewAlpha = 255;
  }

  if ( iNewAlpha != pClipLayer->m_Alpha )
  {
    pClipLayer->m_Alpha = iNewAlpha;

    rc.left += LAYER_ALPHA_X;
    rc.top += LAYER_ALPHA_Y;
    rc.right = rc.left + LAYER_ALPHA_W + 1;
    rc.bottom = rc.top + LAYER_ALPHA_H;
    

    // neu zeichnen
    m_pDocInfo->RedrawAllViews();
    m_Listm_vectLayers.InvalidateRect( &rc, TRUE );

    m_Listm_vectLayers.ClientToScreen( &rc );
    //ClipCursor( &rc );
  }

}

#include <afxadv.h>
#include ".\dlgptoollayer.h"

/*-StartDrag------------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void StartDrag( GR::Graphic::Image *pImage )
{

  HBITMAP                 hTempBitmap   = NULL,
                          hBitmap;

  BITMAPINFO              bminfo;

  RGBQUAD                 bmiColor[256];

  HDC                     hDC;

  int                     i,
                          j,
                          iLO;

  void                    *pData;



  ViewInfo *pViewInfo = pSettings->m_pActiveViewInfo;

  if ( pViewInfo == NULL )
  {
    return;
  }

  bminfo.bmiHeader.biSize             = sizeof( BITMAPINFOHEADER );
  bminfo.bmiHeader.biPlanes           = 1;
  bminfo.bmiHeader.biBitCount         = 8;
  bminfo.bmiHeader.biCompression      = BI_RGB;
  bminfo.bmiHeader.biSizeImage        = 0;
  bminfo.bmiHeader.biClrUsed          = 0;
  bminfo.bmiHeader.biClrImportant     = 0;
  bminfo.bmiHeader.biWidth            = pImage->GetWidth();
  bminfo.bmiHeader.biHeight           = - (signed long)pImage->GetHeight();  // minus, damit es TOP-DOWN ist

  hDC                                 = CreateCompatibleDC( pViewInfo->m_pPage->GetDC() );
  hBitmap                             = CreateDIBSection( hDC, (BITMAPINFO*)&bminfo, DIB_PAL_COLORS, &pData, NULL, 0 );
  hTempBitmap                         = (HBITMAP)SelectObject( hDC, hBitmap );
  for ( i = 0; i < 256; i++ )
  {
    bmiColor[i].rgbRed = i;
    bmiColor[i].rgbGreen = i;
    bmiColor[i].rgbBlue = i;
    bmiColor[i].rgbReserved = 0;
  }
  SetDIBColorTable( hDC, 0, 256, bmiColor );

  iLO = pImage->GetWidth();
  if ( iLO % 4 )
  {
    iLO += 4 - ( iLO % 4 );
  }
  for ( j = 0; j < pImage->GetHeight(); j++ )
  {
    for ( i = 0; i < pImage->GetWidth(); i++ )
    {
      ((unsigned char*)pData)[i + j * iLO] = ( (BYTE*)pImage->GetData() )[i + j * pImage->GetWidth()];
    }
  }
  SelectObject( hDC, hTempBitmap );
  if ( hDC )
  {
    DeleteDC( hDC );
  }

  OleInitialize(NULL);
  

  COleDataSource  *pods = new COleDataSource();

  /*
  STGMEDIUM   stgData;

  memset( &stgData, 0, sizeof( STGMEDIUM ) );
  stgData.tymed = TYMED_GDI;
  stgData.hBitmap = (HBITMAP)::BitmapToHDIB( hBitmap, NULL );
  stgData.pUnkForRelease = NULL;
  */


  FORMATETC fe;

  fe.cfFormat = CF_BITMAP; 
  fe.ptd = NULL; 
  fe.dwAspect = DVASPECT_CONTENT; 
  fe.lindex = -1; 
  fe.tymed = TYMED_GDI; 

  pods->CacheGlobalData( CF_DIB, (HGLOBAL)::BitmapToHDIB( hBitmap, NULL ) );//, &stgData, &fe ); 
  DROPEFFECT deResult = pods->DoDragDrop( DROPEFFECT_COPY );

  if ( deResult == DROPEFFECT_MOVE )
  {
    // löschen!
    pSettings->Log( "move!!" );
  }

  delete pods;

  DeleteObject( hBitmap );

  pSettings->Log( "DoDragDrop finished" );
}



void CDlgPToolLayer::DoPopup( CLayer* pLayer, int iLayerIndex )
{

  HMENU     menuPopup;

  DWORD     dwResult;

  POINT     pt;


  GetCursorPos( &pt );

  menuPopup = CreatePopupMenu();

  if ( iLayerIndex != m_pDocInfo->CurrentLayer() )
  {
    AppendMenuA( menuPopup, MF_STRING, 1, "Aktivieren" );
  }
  if ( !pLayer->m_Visible )
  {
    AppendMenuA( menuPopup, MF_STRING, 2, "Sichtbar machen" );
  }
  else
  {
    AppendMenuA( menuPopup, MF_STRING, 2, "Unsichtbar machen" );
  }
  if ( pLayer->m_HasMask )
  {
    if ( pLayer->m_MaskEnabled )
    {
      AppendMenuA( menuPopup, MF_STRING, 4, "Maske deaktivieren" );
    }
    else
    {
      AppendMenuA( menuPopup, MF_STRING, 4, "Maske aktivieren" );
    }
    AppendMenuA( menuPopup, MF_STRING, 5, "Maske löschen" );
  }
  if ( pLayer->m_Transparent )
  {
    AppendMenuA( menuPopup, MF_STRING, 6, "aktuelle Farbe als transparent" );
  }

  if ( pLayer != m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), 0 ) )
  {
    AppendMenuA( menuPopup, MF_STRING, 3, "Löschen" );
  }
  AppendMenuA( menuPopup, MF_STRING, 7, "Umbenennen" );

  ODMenu   odMenu( ODMenu::ODMS_XP );

  odMenu.Clone( menuPopup );

  dwResult = odMenu.TrackPopupMenu( TPM_RETURNCMD, pt.x, pt.y, this, NULL );
  //dwResult = TrackPopupMenu( menuPopup, TPM_RETURNCMD, pt.x, pt.y, 0, GetSafeHwnd(), NULL );

  DestroyMenu( menuPopup );

  if ( dwResult == 1 )
  {
    SetActiveLayer( iLayerIndex );
  }
  else if ( dwResult == 2 )
  {
    pLayer->m_Visible = !pLayer->m_Visible;
    m_pDocInfo->RedrawAllViews();
    m_Listm_vectLayers.Invalidate( FALSE );
  }
  else if ( dwResult == 3 )
  {
    if ( AfxMessageBox( GR::Convert::ToUTF16( CMisc::printf( "Der Layer %s soll gelöscht werden.\nSind Sie sicher?", pLayer->m_Name.c_str() ) ).c_str(), MB_YESNO ) == IDYES )
    {
      m_pDocInfo->AddUndoLayerChange( iLayerIndex, 0, CUndoLayerChange::ULC_REMOVE_LAYER );

      std::vector<CLayer*>::iterator    it( m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].Layers.begin() );
      std::advance( it, iLayerIndex );

      m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].Layers.erase( it );
      UpdateControls();
      m_pDocInfo->RedrawAllViews();
    }
  }
  else if ( dwResult == 4 )
  {
    pLayer->m_MaskEnabled = !pLayer->m_MaskEnabled;
    m_pDocInfo->RedrawAllViews();
  }
  else if ( dwResult == 5 )
  {
    pLayer->SetLayerMask( NULL );

    m_pDocInfo->RedrawAllViews();
    m_Listm_vectLayers.Invalidate( FALSE );
  }
  else if ( dwResult == 6 )
  {
    if ( m_pDocInfo->m_BitDepth <= 8 )
    {
      pLayer->m_Transparenz = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_8BIT );
    }
    else
    {
      pLayer->m_Transparenz = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR );
    }
    m_pDocInfo->RedrawAllViews();
  }
  else if ( dwResult == 7 )
  {
    CDlgRename    dlgRename( pLayer->m_Name, "Rename Layer" );

    if ( dlgRename.DoModal() == IDOK )
    {
      pLayer->m_Name = dlgRename.m_Name;
      UpdateControls();
    }
  }
}


BOOL CDlgPToolLayer::OnInitDialog() 
{
  CDialog::OnInitDialog();

  UpdateData( FALSE );

  ddMaskTarget.Register( &m_Listm_vectLayers );
  ddMaskTarget.m_pToolLayer = this;

  pClipLayer = NULL;
  bClipping = FALSE;

  m_hIconNewLayer = (HICON)LoadImage( AfxGetInstanceHandle(),
                                       MAKEINTRESOURCE( IDI_ICON_LAYER_NEW ),
                                       IMAGE_ICON,
                                       0,
                                       0,
                                       0 );
  m_hIconMergeVisiblem_vectLayers = (HICON)LoadImage( AfxGetInstanceHandle(),
                                       MAKEINTRESOURCE( IDI_ICON_LAYER_MERGEVISIBLE ),
                                       IMAGE_ICON,
                                       0,
                                       0,
                                       0 );
  m_hIconMergeAllm_vectLayers = (HICON)LoadImage( AfxGetInstanceHandle(),
                                       MAKEINTRESOURCE( IDI_ICON_LAYER_MERGEALL ),
                                       IMAGE_ICON,
                                       0,
                                       0,
                                       0 );
  m_hIconCreateLayerMask = (HICON)LoadImage( AfxGetInstanceHandle(),
                                       MAKEINTRESOURCE( IDI_ICON_LAYER_CREATEMASK ),
                                       IMAGE_ICON,
                                       0,
                                       0,
                                       0 );

  m_ButtonNewLayer.SetIcon( m_hIconNewLayer );
  m_ButtonMergeVisible.SetIcon( m_hIconMergeVisiblem_vectLayers );
  m_ButtonMergeAll.SetIcon( m_hIconMergeAllm_vectLayers );
  m_ButtonCreateMask.SetIcon( m_hIconCreateLayerMask );

  if ( !MakeDragList( m_Listm_vectLayers.GetSafeHwnd() ) )
  {
    MessageBox( _T( "MakeDragList failed" ) );
  }

	return TRUE;

}



void CDlgPToolLayer::UpdateControls()
{

  m_Listm_vectLayers.ResetContent();

  if ( m_pDocInfo == NULL )
  {
    // kein Document, keine Buttons!
    m_ButtonCreateMask.EnableWindow( FALSE );
    m_ButtonMergeAll.EnableWindow( FALSE );
    m_ButtonMergeVisible.EnableWindow( FALSE );
    m_ButtonNewLayer.EnableWindow( FALSE );
    return;
  }
  m_ButtonCreateMask.EnableWindow();
  m_ButtonMergeAll.EnableWindow();
  m_ButtonMergeVisible.EnableWindow();
  m_ButtonNewLayer.EnableWindow();


  // Layer eintragen
  if ( !m_pDocInfo->m_LayeredFrames.empty() )
  {
    CLayer       *pLayer;

    DWORD           dwLayerCount;

    dwLayerCount = 0;
    for ( size_t i = 0; i < m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount(); i++ )
    {
      pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), i );

      dwLayerCount++;
      m_Listm_vectLayers.SetItemData( m_Listm_vectLayers.AddString( GR::Convert::ToUTF16( pLayer->m_Name ).c_str() ), i );
    }

    m_Listm_vectLayers.Invalidate( FALSE );
  }
}




void CDlgPToolLayer::OnButtonToolLayerNewLayer() 
{
  if ( m_pDocInfo == NULL )
  {
    MessageBox( _T( "no active docinfo!" ) );
    return;
  }

  m_pDocInfo->AddUndoLayerChange( (DWORD)m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount(),
                                  0,
                                  CUndoLayerChange::ULC_ADD_LAYER );

  GR::Graphic::Image*  pFirstLayerImage = m_pDocInfo->GetImage( m_pDocInfo->CurrentFrame(), 0 );

  GR::Graphic::Image*  pImage = new GR::Graphic::Image( pFirstLayerImage->GetWidth(), pFirstLayerImage->GetHeight(), pFirstLayerImage->GetDepth() );
  m_pDocInfo->AddLayer( pImage );
  UpdateControls();
  m_pDocInfo->RedrawAllViews();
}



void CDlgPToolLayer::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  CLayer*       pLayer;

  GR::tChar     szTemp[20];

  RECT          rc;


  if ( lpDrawItemStruct->CtlType == ODT_LISTBOX )
  {
    if ( m_pDocInfo == NULL )
    {
      FillRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, GetSysColorBrush( COLOR_WINDOW ) );
      return;
    }

    GR::tChar      szBuffer[MAX_PATH];
    if ( lpDrawItemStruct->itemID != -1 )
    {
      pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), lpDrawItemStruct->itemData );
      if ( lpDrawItemStruct->itemState & ODS_SELECTED )
      {
        FillRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, GetSysColorBrush( COLOR_HIGHLIGHT ) );
        SetTextColor( lpDrawItemStruct->hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ) );
      }
      else
      {
        FillRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, GetSysColorBrush( COLOR_WINDOW ) );
        SetTextColor( lpDrawItemStruct->hDC, GetSysColor( COLOR_WINDOWTEXT ) );
      }
      if ( lpDrawItemStruct->itemID == m_pDocInfo->CurrentLayer() )
      {
        DrawIconEx( lpDrawItemStruct->hDC, LAYER_ACT_ICON_X, lpDrawItemStruct->rcItem.top + LAYER_ACT_ICON_Y, iconActiveLayer, 16, 16, 0, GetSysColorBrush( COLOR_WINDOW ), DI_NORMAL );
      }
      else
      {
        DrawIconEx( lpDrawItemStruct->hDC, LAYER_ACT_ICON_X, lpDrawItemStruct->rcItem.top + LAYER_ACT_ICON_Y, iconInactiveLayer, 16, 16, 0, GetSysColorBrush( COLOR_WINDOW ), DI_NORMAL );
      }
      if ( pLayer->m_Visible )
      {
        DrawIconEx( lpDrawItemStruct->hDC, LAYER_VIS_ICON_X, lpDrawItemStruct->rcItem.top + LAYER_VIS_ICON_Y, iconVisibleLayer, 16, 16, 0, GetSysColorBrush( COLOR_WINDOW ), DI_NORMAL );
      }
      else
      {
        DrawIconEx( lpDrawItemStruct->hDC, LAYER_VIS_ICON_X, lpDrawItemStruct->rcItem.top + LAYER_VIS_ICON_Y, iconInactiveLayer, 16, 16, 0, GetSysColorBrush( COLOR_WINDOW ), DI_NORMAL );
      }
      if ( pLayer->m_HasMask )
      {
        if ( pLayer->m_MaskEnabled )
        {
          DrawIconEx( lpDrawItemStruct->hDC, LAYER_MSK_ICON_X, lpDrawItemStruct->rcItem.top + LAYER_MSK_ICON_Y, iconActiveMask, 16, 16, 0, GetSysColorBrush( COLOR_WINDOW ), DI_NORMAL );
        }
        else
        {
          DrawIconEx( lpDrawItemStruct->hDC, LAYER_MSK_ICON_X, lpDrawItemStruct->rcItem.top + LAYER_MSK_ICON_Y, iconInactiveMask, 16, 16, 0, GetSysColorBrush( COLOR_WINDOW ), DI_NORMAL );
        }
      }
      if ( pLayer->m_Transparent )
      {
        DrawIconEx( lpDrawItemStruct->hDC, LAYER_TSP_ICON_X, lpDrawItemStruct->rcItem.top + LAYER_TSP_ICON_Y, iconColorKeyLayer, 16, 16, 0, GetSysColorBrush( COLOR_WINDOW ), DI_NORMAL );
      }
      else
      {
        DrawIconEx( lpDrawItemStruct->hDC, LAYER_TSP_ICON_X, lpDrawItemStruct->rcItem.top + LAYER_TSP_ICON_Y, iconInactiveLayer, 16, 16, 0, GetSysColorBrush( COLOR_WINDOW ), DI_NORMAL );
      }

      // Layer-Name

      SetBkMode( lpDrawItemStruct->hDC, TRANSPARENT );
      m_Listm_vectLayers.GetText( lpDrawItemStruct->itemID, szBuffer );
      rc.left = lpDrawItemStruct->rcItem.left + LAYER_NAME_X;
      rc.top = lpDrawItemStruct->rcItem.top + LAYER_NAME_Y;
      rc.right = rc.left + LAYER_NAME_W;
      rc.bottom = rc.top + LAYER_NAME_H;
      DrawText( lpDrawItemStruct->hDC, szBuffer, (int)_tcslen( szBuffer ), &rc, DT_VCENTER | DT_SINGLELINE );

      // Alpha-Wert des m_vectLayers
      rc.left = lpDrawItemStruct->rcItem.left + LAYER_ALPHA_X;
      rc.top = lpDrawItemStruct->rcItem.top + LAYER_ALPHA_Y;
      rc.right = rc.left + LAYER_ALPHA_W;
      rc.bottom = rc.top + LAYER_ALPHA_H;

      {
        RECT    rc2;
        rc2 = rc;
        FillRect( lpDrawItemStruct->hDC, &rc2, GetSysColorBrush( COLOR_3DDKSHADOW ) );
        rc2.left = rc.left + ( pLayer->m_Alpha * ( rc.right - rc.left ) ) / 255;
        rc2.right = rc2.left + 1;
        FillRect( lpDrawItemStruct->hDC, &rc2, GetSysColorBrush( COLOR_3DFACE ) );

        SetTextColor( lpDrawItemStruct->hDC, RGB( 255, 255, 255 ) );
        SetBkMode( lpDrawItemStruct->hDC, TRANSPARENT );
        wsprintf( szTemp, _T( "%d" ), pLayer->m_Alpha );

        DrawText( lpDrawItemStruct->hDC, szTemp, (int)_tcslen( szTemp ), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
      }
    }
    else
    {
      FillRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, GetSysColorBrush( COLOR_3DFACE ) );
      SetTextColor( lpDrawItemStruct->hDC, RGB( 255, 255, 255 ) );
      SetBkMode( lpDrawItemStruct->hDC, TRANSPARENT );
      m_Listm_vectLayers.GetText( lpDrawItemStruct->itemID, szBuffer );
      wsprintf( szBuffer, _T( "NULL Item" ) );
      DrawText( lpDrawItemStruct->hDC, szBuffer, (int)_tcslen( szBuffer ), &lpDrawItemStruct->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
    }
    //CDialogBar::OnDrawItem( nIDCtl, lpDrawItemStruct );
    return;
  }
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);

}

void CDlgPToolLayer::OnButtonToolLayerMergeAll() 
{

  CLayer             *pLayer;


  // Alle Layer in einen werfen!
  // m_vectLayers

  GR::Graphic::GDIPage    *pPage = new GR::Graphic::GDIPage();
  pPage->Create( GetSafeHwnd(), m_pDocInfo->Width(), m_pDocInfo->Height(), m_pDocInfo->m_BitDepth );
  GR::Graphic::Palette  *pPalette = NULL;
  
  if ( m_pDocInfo->m_BitDepth <= 8 )
  {
    pPalette = new GR::Graphic::Palette( *m_pDocInfo->GetPalette( m_pDocInfo->CurrentFrame() ) );
  }

  GR::tRect    rc( 0, 0, m_pDocInfo->Width(), m_pDocInfo->Height() );

  for ( size_t i = 0; i < m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount(); i++ )
  {
    pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), i );

    pLayer->DrawImage( pPage, pPalette, rc );
  }

  m_pDocInfo->m_UndoManager.StartUndoGroup();
  for ( int i = m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount() - 1; i >= 1; --i )
  {
    m_pDocInfo->m_UndoManager.AddUndoStep( new CUndoLayerChange( m_pDocInfo,
                                                                 m_pDocInfo->CurrentFrame(),
                                                                 i,
                                                                 0,
                                                                 CUndoLayerChange::ULC_REMOVE_LAYER ),
                                           FALSE );
  }

  m_pDocInfo->m_UndoManager.AddUndoStep( new CUndoImageChange( 0, 
                                                   0, 
                                                   m_pDocInfo->Width() - 1, 
                                                   m_pDocInfo->Height() - 1, 
                                                   m_pDocInfo->CurrentFrame(),
                                                   0, 
                                                   0,
                                                   m_pDocInfo,
                                                   false ),
                                         FALSE );


  m_pDocInfo->GetImage( m_pDocInfo->CurrentFrame(), 0 )->GetImage( pPage, 0, 0 );

  // alle zusätzlichen m_vectLayers rauswerfen
  while ( m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount() >= 2 )
  {
    std::vector<CLayer*>::iterator    it( m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].Layers.begin() );
    ++it;

    m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].Layers.erase( it );
  }

  delete pPage;
  delete pPalette;

  m_pDocInfo->CurrentLayer( 0 );

  UpdateControls();

  m_pDocInfo->RedrawAllViews();
	
}

void CDlgPToolLayer::OnButtonToolLayerMerge() 
{

  CLayer             *pLayer;


  // Alle SICHTBAREN! Layer in einen werfen!
  // m_vectLayers

  // BAUSTELLE?
  GR::Graphic::GDIPage    *pPage = new GR::Graphic::GDIPage();
  pPage->Create( GetSafeHwnd(), m_pDocInfo->Width(), m_pDocInfo->Height(), m_pDocInfo->m_BitDepth );
  GR::Graphic::Palette  *pPalette = new GR::Graphic::Palette();

  size_t   iTargetLayer = -1;

  GR::tRect   rc( 0, 0, m_pDocInfo->Width(), m_pDocInfo->Height() );

  for ( size_t i = 0; i < m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount(); i++ )
  {
    pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), i );
    if ( pLayer->m_Visible )
    {
      iTargetLayer = i;
      break;
    }
  }
  if ( iTargetLayer == -1 )
  {
    return;
  }

  int   iDelta = 0;
  m_pDocInfo->m_UndoManager.StartUndoGroup();
  for ( int i = m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount() - 1; i >= 1; --i )
  {
    pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), i );
    if ( pLayer->m_Visible )
    {
      m_pDocInfo->m_UndoManager.AddUndoStep( new CUndoLayerChange( m_pDocInfo,
                                                                   m_pDocInfo->CurrentFrame(),
                                                                   i + iDelta,
                                                                   0,
                                                                   CUndoLayerChange::ULC_REMOVE_LAYER ),
                                             FALSE );
    }
  }

  m_pDocInfo->m_UndoManager.AddUndoStep( new CUndoImageChange( 0, 
                                                   0, 
                                                   m_pDocInfo->Width() - 1, 
                                                   m_pDocInfo->Height() - 1, 
                                                   m_pDocInfo->CurrentFrame(),
                                                   iTargetLayer, 
                                                   0,
                                                   m_pDocInfo,
                                                   false ),
                                         FALSE );

  for ( int i = 0; i < (int)m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount(); i++ )
  {
    pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), i );
    if ( pLayer->m_Visible )
    {
      pLayer->DrawImage( pPage, pPalette, rc );
    }
  }
  m_pDocInfo->GetImage( m_pDocInfo->CurrentFrame(), iTargetLayer )->GetImage( pPage, 0, 0 );

  // alle zusätzlichen m_vectLayers rauswerfen
  for ( int i = 0; i < (int)m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].LayerCount(); i++ )
  {
    if ( i != iTargetLayer )
    {
      pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), i );
      if ( pLayer->m_Visible )
      {
        std::vector<CLayer*>::iterator    it( m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].Layers.begin() );
        std::advance( it, i );

        m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].Layers.erase( it );
        //m_pDocInfo->RemoveLayer( pLayer );
        i--;
      }
    }
  }

  delete pPage;
  delete pPalette;

  UpdateControls();

  m_pDocInfo->RedrawAllViews();
	
}

void CDlgPToolLayer::OnButtonToolLayerCreateMask() 
{

  if ( m_pDocInfo == NULL )
  {
    pSettings->Log( "no docinfo" );
    return;
  }
  CLayer*   pActiveLayer = m_pDocInfo->ActiveLayer();
  if ( pActiveLayer == NULL )
  {
    // kein aktiver Layer ???
    pSettings->Log( "no active layer??" );
    return;
  }
  if ( pActiveLayer->m_HasMask )
  {
    // es gibt bereits eine Maske!
    // TODO - besser Button grauen?
    pSettings->Log( "layer already has mask" );
    return;
  }

  CDialogCreateMask   cmDlg;


  if ( cmDlg.DoModal() == IDOK )
  {
    pActiveLayer->m_HasMask = true;
    pActiveLayer->SetLayerMask( new GR::Graphic::Image( pActiveLayer->GetImage()->GetWidth(),
                                                pActiveLayer->GetImage()->GetHeight(), 8, 0, 0 ) );

    CMaskedContextDescriptor *pCDMask = new CMaskedContextDescriptor( pActiveLayer->GetMask(),
                                                     &pSettings->m_AlphaPalette );

    switch ( cmDlg.m_CreateMaskType )
    {
      case 0:
        // die Maske wird massiv
        FillMemory( pActiveLayer->GetMask()->GetData(), 
                    pActiveLayer->GetImage()->GetWidth() * pActiveLayer->GetImage()->GetHeight(), 255 );
        break;
      case 1:
        {
          // die Maske wird von der Farbe anhängig
          CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( 
                                                  pActiveLayer->GetImage(),
                                                  m_pDocInfo->GetPalette( m_pDocInfo->CurrentFrame() ) );

          DWORD   dwWorkColor = m_pDocInfo->ToLocalColor( CSettings::ColorCategory::WORKCOLOR );
          if ( m_pDocInfo->m_BitDepth <= 8 )
          {
            // bei 8 bit kann es gleiche Farben geben!
            for ( int i = 0; i < pCD->Width(); i++ )
            {
              for ( int j = 0; j < pCD->Height(); j++ )
              {
                if ( pCD->GetDirectPixel( i, j ) == dwWorkColor )
                {
                  pCDMask->PutDirectPixel( i, j, 0xff );
                }
                else
                {
                  pCDMask->PutDirectPixel( i, j, 0 );
                }
              }
            }
          }
          else
          {
            for ( int i = 0; i < pCD->Width(); i++ )
            {
              for ( int j = 0; j < pCD->Height(); j++ )
              {
                if ( pCD->GetPixel( i, j ) == dwWorkColor )
                {
                  pCDMask->PutDirectPixel( i, j, 0xff );
                }
                else
                {
                  pCDMask->PutDirectPixel( i, j, 0 );
                }
              }
            }
          }
          SafeDelete( pCD );
        }
        break;
      case 2:
        {
          // Maske von der Selektion
          if ( m_pDocInfo->HasSelection() )
          {
            pActiveLayer->SetLayerMask( new GR::Graphic::Image( m_pDocInfo->m_pImageSelection ) );
          }
        }
        break;
      case 3:
        {
          // die Maske wird aus den Graustufen der Farbinformationen gebildet
          CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( 
                                                    pActiveLayer->GetImage(),
                                                    m_pDocInfo->GetPalette( m_pDocInfo->CurrentFrame() ) );

          DWORD   dwPixel;
          for ( int i = 0; i < pCD->Width(); i++ )
          {
            for ( int j = 0; j < pCD->Height(); j++ )
            {
              dwPixel = pCD->GetPixel( i, j );
              pCDMask->PutDirectPixel( i, j,
                                       ( ( ( dwPixel & 0xff0000 ) >> 16 ) * 11
                                      +  ( ( dwPixel & 0xff00 ) >> 8 ) * 80
                                      +  ( ( dwPixel & 0xff ) ) * 9 ) / 100 );
            }
          }
          SafeDelete( pCD );
        }
        break;
    }

    if ( cmDlg.m_InvertMask )
    {
      for ( int i = 0; i < pCDMask->Width(); i++ )
      {
        for ( int j = 0; j < pCDMask->Height(); j++ )
        {
          pCDMask->PutDirectPixel( i, j, 255 - pCDMask->GetDirectPixel( i, j ) );
        }
      }
    }
    SafeDelete( pCDMask );

    pActiveLayer->m_MaskEnabled = true;

    m_pDocInfo->RedrawAllViews();
    m_Listm_vectLayers.Invalidate( FALSE );
  }
	
}

void CDlgPToolLayer::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{

  if ( lpMeasureItemStruct->CtlType == ODT_LISTBOX )
  {
    lpMeasureItemStruct->itemHeight = LAYER_ITEM_HEIGHT;
    return;
  }
	
	CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}




LRESULT CDlgPToolLayer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

  if ( message == g_DragListMsg )
  {
    LPDRAGLISTINFO  pDLI = (LPDRAGLISTINFO)lParam;

    if ( pDLI->uNotification == DL_DROPPED )
    {
      // Layer-Liste neu aufbauen!
      std::vector<CLayer*>     vectLayerTemp;


      if ( m_pDocInfo != NULL )
      {
        m_pDocInfo->m_UndoManager.StartUndoGroup();

        CDialog::WindowProc(message, wParam, lParam);

        std::set<int>   setIndices;

        for ( int i = 0; i < m_Listm_vectLayers.GetCount(); i++ )
        {
          setIndices.insert( i );
        }
        for ( int i = 0; i < m_Listm_vectLayers.GetCount(); i++ )
        {
          vectLayerTemp.push_back( m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), m_Listm_vectLayers.GetItemData( i ) ) );

          int   iOldLayerIndex = 0;
          std::vector<CLayer*>::iterator  it( m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].Layers.begin() );
          while ( it != m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].Layers.end() )
          {
            if ( *it == m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), m_Listm_vectLayers.GetItemData( i ) ) )
            {
              int   iKey = i;
              if ( iKey > iOldLayerIndex )
              {
                iKey = iOldLayerIndex;
              }
              if ( ( i != iOldLayerIndex )
              &&   ( setIndices.find( iKey ) != setIndices.end() ) )
              {
                m_pDocInfo->AddUndoLayerChange( i, iOldLayerIndex, CUndoLayerChange::ULC_EXCHANGE_LAYER, false );

                setIndices.erase( setIndices.find( iKey ) );
              }
            }

            ++iOldLayerIndex;
            ++it;
          }
        }
        m_pDocInfo->m_LayeredFrames[m_pDocInfo->CurrentFrame()].Layers = vectLayerTemp;
        m_pDocInfo->RedrawAllViews();
        return 0;
      }

    }
    

  }
	
	return CDialog::WindowProc(message, wParam, lParam);
}



void CDlgPToolLayer::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember )
{

  switch ( NotifyMessage )
  {
    case NF_ACTIVE_VIEW_CHANGED:
      m_pDocInfo = pSettings->m_pActiveDocInfo;
      UpdateControls();
      break;
    case NF_LAYER_CHANGED:
      m_pDocInfo = pSettings->m_pActiveDocInfo;
      UpdateControls();
      break;
  }

}

BOOL CDlgPToolLayer::PreTranslateMessage(MSG* pMsg)
{

  if ( pMsg->hwnd == m_Listm_vectLayers.GetSafeHwnd() )
  {
    if ( ( pMsg->message == WM_LBUTTONDOWN )
    ||   ( pMsg->message == WM_LBUTTONDBLCLK ) )
    {
      BOOL bOutSide;
      RECT    rc;
      int iX = LOWORD( pMsg->lParam ),
          iY = HIWORD( pMsg->lParam );

      m_ptButtonDown.x = iX;
      m_ptButtonDown.y = iY;

      int iItem = m_Listm_vectLayers.ItemFromPoint( CPoint( iX, iY ), bOutSide );
      if ( m_Listm_vectLayers.GetItemRect( iItem, &rc ) != LB_ERR )
      {
        DWORD_PTR   dwLayerIndex = m_Listm_vectLayers.GetItemData( iItem );
        CLayer*  pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), dwLayerIndex );
        if ( ( iX >= rc.left + LAYER_ACT_ICON_X )
        &&   ( iY >= rc.top + LAYER_ACT_ICON_Y )
        &&   ( iX < rc.left + LAYER_ACT_ICON_X + 16 )
        &&   ( iY < rc.top + LAYER_ACT_ICON_Y + 16 ) )
        {
          // Layer-Aktiv schalten
          SetActiveLayer( dwLayerIndex );
          return 1;
        }
        else if ( ( iX >= rc.left + LAYER_VIS_ICON_X )
        &&        ( iY >= rc.top + LAYER_VIS_ICON_Y )
        &&        ( iX < rc.left + LAYER_VIS_ICON_X + 16 )
        &&        ( iY < rc.top + LAYER_VIS_ICON_Y + 16 ) )
        {
          // Layer sichtbar schalten
          pLayer->m_Visible = !pLayer->m_Visible;
          m_pDocInfo->RedrawAllViews();

          m_Listm_vectLayers.Invalidate( FALSE );
          return 1;
        }
        else if ( ( iX >= rc.left + LAYER_MSK_ICON_X )
        &&        ( iY >= rc.top + LAYER_MSK_ICON_Y )
        &&        ( iX < rc.left + LAYER_MSK_ICON_X + 16 )
        &&        ( iY < rc.top + LAYER_MSK_ICON_Y + 16 ) )
        {
          // Maske sichtbar schalten
          if ( pLayer->m_HasMask )
          {
            //StartDrag( pLayer->pImageMask );

            pLayer->m_MaskEnabled = !pLayer->m_MaskEnabled;
            m_pDocInfo->RedrawAllViews();

            m_Listm_vectLayers.Invalidate( FALSE );
          }
          return 1;
        }
        else if ( ( iX >= rc.left + LAYER_TSP_ICON_X )
        &&        ( iY >= rc.top + LAYER_TSP_ICON_Y )
        &&        ( iX < rc.left + LAYER_TSP_ICON_X + 16 )
        &&        ( iY < rc.top + LAYER_TSP_ICON_Y + 16 ) )
        {
          // Layer auf Color-Key schalten
          pLayer->m_Transparent = !pLayer->m_Transparent;
          m_pDocInfo->RedrawAllViews();

          m_Listm_vectLayers.Invalidate( FALSE );
          return 1;
        }
        else if ( ( iX >= rc.left + LAYER_ALPHA_X )
        &&        ( iY >= rc.top + LAYER_ALPHA_Y )
        &&        ( iX < rc.left + LAYER_ALPHA_X + LAYER_ALPHA_W )
        &&        ( iY < rc.top + LAYER_ALPHA_Y + LAYER_ALPHA_H ) )
        {
          // Alpha-Regler
          pClipLayer = pLayer;
          bClipping = TRUE;
          m_Listm_vectLayers.SetCapture();
          LBAlphaRegler( iX, rc );
          return 1;
        }
      }
    }
    else if ( pMsg->message == WM_MOUSEMOVE )
    {
      int iX = LOWORD( pMsg->lParam ),
          iY = HIWORD( pMsg->lParam );

      if ( ( abs( iX - m_ptButtonDown.x ) >= 3 )
      ||   ( abs( iY - m_ptButtonDown.y ) >= 3 ) )
      {
        RECT    rc;
        BOOL    bOutSide;
        int iItem = m_Listm_vectLayers.ItemFromPoint( CPoint( iX, iY ), bOutSide );
        if ( m_Listm_vectLayers.GetItemRect( iItem, &rc ) != LB_ERR )
        {
          CLayer*     pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), m_Listm_vectLayers.GetItemData( iItem ) );
          if ( ( m_ptButtonDown.x >= rc.left + LAYER_MSK_ICON_X )
          &&   ( m_ptButtonDown.y >= rc.top + LAYER_MSK_ICON_Y )
          &&   ( m_ptButtonDown.x < rc.left + LAYER_MSK_ICON_X + 16 )
          &&   ( m_ptButtonDown.y < rc.top + LAYER_MSK_ICON_Y + 16 ) )
          {
            // Maske sichtbar schalten
            if ( pLayer->m_HasMask )
            {
              //StartDrag( pLayer->pImageMask );
              return 1;
            }
          }
        }
      }
      if ( bClipping )
      {
        BOOL  bOutSide;
        int iX = LOWORD( pMsg->lParam ),
            iY = HIWORD( pMsg->lParam );
        int iItem = m_Listm_vectLayers.ItemFromPoint( CPoint( iX, iY ), bOutSide );
        RECT                  rc;


        m_Listm_vectLayers.GetItemRect( iItem, &rc );
        LBAlphaRegler( iX, rc );
      }
    }
    else if ( ( pMsg->message == WM_LBUTTONUP )
    ||        ( pMsg->message == WM_VSCROLL ) )
    {
      // Cursor vorsichtshalber immer freigeben
      if ( GetCapture() == &m_Listm_vectLayers )
      {
        ReleaseCapture();
      }
      bClipping = FALSE;
      pClipLayer = NULL;
      ClipCursor( NULL );
    }
    else if ( pMsg->message == WM_KILLFOCUS )
    {
      if ( GetCapture() == &m_Listm_vectLayers )
      {
        ReleaseCapture();
      }
    }
    else if ( pMsg->message == WM_RBUTTONDOWN )
    {
      BOOL bOutSide;
      int iX = LOWORD( pMsg->lParam ),
          iY = HIWORD( pMsg->lParam );
      int iItem = m_Listm_vectLayers.ItemFromPoint( CPoint( iX, iY ), bOutSide );
      if ( !bOutSide )
      {
        DWORD_PTR   dwLayerIndex = m_Listm_vectLayers.GetItemData( iItem );
        CLayer*     pLayer = m_pDocInfo->GetLayer( m_pDocInfo->CurrentFrame(), dwLayerIndex );

        if ( pLayer != NULL )
        {
          DoPopup( pLayer, iItem );
          return TRUE;
        }
      }
    }
  }

  return __super::PreTranslateMessage(pMsg);
}



void CDlgPToolLayer::SetActiveLayer( size_t iLayerIndex )
{

  m_pDocInfo->CurrentLayer( iLayerIndex );

  CLayer*   pLayer = m_pDocInfo->ActiveLayer();
  if ( !pLayer->m_Visible )
  {
    pLayer->m_Visible = true;
    m_pDocInfo->RedrawAllViews();
  }
  m_Listm_vectLayers.Invalidate( FALSE );

}
void CDlgPToolLayer::OnOK()
{
}

void CDlgPToolLayer::OnCancel()
{
}

void CDlgPToolLayer::OnDestroy()
{

  __super::OnDestroy();

  DestroyIcon( m_hIconNewLayer );
  DestroyIcon( m_hIconMergeVisiblem_vectLayers );
  DestroyIcon( m_hIconMergeAllm_vectLayers );
  DestroyIcon( m_hIconCreateLayerMask );

}