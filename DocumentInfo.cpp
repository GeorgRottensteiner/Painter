#include "stdafx.h"
#include "Painter.h"
#include "Settings.h"
#include "MainFrm.h"
#include "PainterGfxTools.h"
#include "DocumentInfo.h"
#include "PainterView.h"
#include "PainterFontView.h"
#include "MaskedContextDescriptor.h"
#include "GFXUtil.h"

#include "UndoDepthChange.h"
#include "UndoImageChange.h"
#include "UndoSizeChange.h"

#include "ColorReducer.h"

#include "PainterEnums.h"

#include "FormatPNT.h"

#include <Grafik\ContextDescriptor.h>
#include <Grafik/Image.h>
#include <Grafik/GFXPage.h>

#include <IO/FileChunk.h>
#include <Memory/MemoryStream.h>
#include <String/Path.h>

#include <debug/debugclient.h>

#include <WinSys/WinUtils.h>

#include <set>



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CPainterApp        theApp;



DocumentInfo::DocumentInfo()
  : m_MarchingAntCount( 0 ),
    m_Closed( FALSE ),
    m_pImageSelection( NULL ),
    m_pImageEffectMask( NULL ),
    m_DocWidth( 0 ),
    m_DocHeight( 0 ),
    m_DocType( DT_INVALID ),
    m_CurrentFrame( 0 ),
    m_CurrentLayer( 0 ),
    m_HasSelection( false ),
    m_BitDepth( 0 )
{

  m_MButton1Released     = TRUE;

  pSettings->AddNotifyMember( this );

  Notify( NF_DOC_ADDED );
}



DocumentInfo::~DocumentInfo()
{
  Notify( NF_DOC_REMOVED );

  pSettings->RemoveNotifyMember( this );

  tVectLayeredFrames::iterator    itFrames( m_LayeredFrames.begin() );
  while ( itFrames != m_LayeredFrames.end() )
  {
    tVectLayers&    layers = itFrames->Layers;

    tVectLayers::iterator   itLayer( layers.begin() );
    while ( itLayer != layers.end() )
    {
      CLayer*   pLayer = *itLayer;

      delete pLayer;

      ++itLayer;
    }
    ++itFrames;
  }
  m_LayeredFrames.clear();

  SafeDelete( m_pImageSelection );
  SafeDelete( m_pImageEffectMask );
}



void DocumentInfo::RedrawAllViews()
{
  GR::tRect  rcRedraw;

  switch ( m_DocType )
  {
    case DT_FONT:
    case DT_IMAGE:
      rcRedraw.Set( 0, 0, Width(), Height() );
      break;
    default:
      dh::Log( "DocumentInfo::RedrawAllViews unsupported Document type\n" );
      break;
  }
  m_pDoc->UpdateAllViews( NULL, ViewInfo::REDRAW_RECT, (CObject*)&rcRedraw );
}



void DocumentInfo::RedrawAllViews( const GR::tRect& rc, bool bRedrawNow )
{
  if ( bRedrawNow )
  {
    m_pDoc->UpdateAllViews( NULL, ViewInfo::REDRAW_RECT, (CObject*)&rc );
  }
}



void DocumentInfo::ShowAllViews()
{
  m_pDoc->UpdateAllViews( NULL );
}



CLayer* DocumentInfo::AddLayer( GR::Graphic::Image* pOrigin, size_t Frame, size_t LayerInsertPos )
{
  if ( Frame == -1 )
  {
    Frame = m_CurrentFrame;
  }
  if ( m_LayeredFrames.size() <= size_t( Frame ) )
  {
    dh::Log( "AddLayer: Invalid Frame Index (%d)\n", Frame );
    return NULL;
  }

  tVectLayers&  layers = m_LayeredFrames[Frame].Layers;

  CLayer*   pLayer = new CLayer();

  if ( layers.empty() )
  {
    // der erste Layer!
    m_CurrentLayer = 0;
    if ( pOrigin == NULL )
    {
      pLayer->SetLayerImage( new GR::Graphic::Image() );
    }
    else
    {
      pLayer->SetLayerImage( pOrigin );
    }
    pLayer->m_Name = "Hintergrund";
  }
  else
  {
    // ein zus‰tzlicher Layer
    pLayer->SetTransparencyIndex( true, 0 );
    if ( pOrigin == NULL )
    {
      pLayer->SetLayerImage( new GR::Graphic::Image( GetImage( Frame, 0 ) ) );
    }
    else
    {
      pLayer->SetLayerImage( pOrigin );
    }
    pLayer->m_Name = "Layer";
  }

  pLayer->m_MaskEnabled = false;
  pLayer->SetLayerMask( NULL );

  if ( size_t( LayerInsertPos ) < layers.size() )
  {
    layers.insert( layers.begin() + LayerInsertPos, pLayer );
  }
  else
  {
    layers.push_back( pLayer );
  }
  return pLayer;
}



void DocumentInfo::RemoveLayer( CLayer* pLayer, size_t Frame )
{
  if ( pLayer == NULL )
  {
    return;
  }
  if ( Frame >= m_LayeredFrames.size() )
  {
    return;
  }

  int   layerIndex = 0;
  std::vector<CLayer*>::iterator   it( m_LayeredFrames[Frame].Layers.begin() );
  while ( it != m_LayeredFrames[Frame].Layers.end() )
  {
    if ( *it == pLayer )
    {
      pLayer->Clear();
      delete pLayer;
      m_LayeredFrames[Frame].Layers.erase( it );

      if ( m_CurrentLayer == layerIndex )
      {
        m_CurrentLayer = 0;
      }
      break;
    }
    ++layerIndex;
    it++;
  }
}



GR::Graphic::Image* DocumentInfo::GetActiveImage()
{
  CLayer*   pLayer = ActiveLayer();

  if ( pLayer == NULL )
  {
    return NULL;
  }
  return pLayer->GetImage();
}



CLayer* DocumentInfo::ActiveLayer()
{
  if ( m_CurrentFrame >= m_LayeredFrames.size() )
  {
    return NULL;
  }
  tVectLayers&    layers = m_LayeredFrames[m_CurrentFrame].Layers;

  if ( layers.empty() )
  {
    return NULL;
  }
  if ( m_CurrentLayer >= layers.size() )
  {
    m_CurrentLayer = 0;
  }
  return layers[m_CurrentLayer];
}



size_t DocumentInfo::CurrentLayer()
{
  return m_CurrentLayer;
}



void DocumentInfo::CurrentLayer( size_t Layer )
{
  if ( m_CurrentFrame >= m_LayeredFrames.size() )
  {
    return;
  }

  tVectLayers&    layers = m_LayeredFrames[m_CurrentFrame].Layers;

  if ( layers.empty() )
  {
    return;
  }

  if ( Layer >= layers.size() )
  {
    Layer = 0;
  }
  m_CurrentLayer = Layer;
}



GR::Graphic::Image* DocumentInfo::GetSelectionMask()
{
  GR::Graphic::Image*    pImageMask = new GR::Graphic::Image( m_pImageSelection );

  if ( !HasSelection() )
  {
    GR::Graphic::ContextDescriptor    cdSelection( pImageMask, &pSettings->m_AlphaPalette );
    
    cdSelection.Box( 0, 0, cdSelection.Width(), cdSelection.Height(), 0xffffffff );
  }

  return pImageMask;
}



unsigned char DocumentInfo::GetBitDepth()
{
  CLayer*   pLayer = ActiveLayer();

  if ( pLayer == NULL )
  {
    // es gibt noch kein Dokument
    return 0;
  }
  if ( pLayer->GetImage() == NULL )
  {
    // es gibt noch kein Dokument
    return 0;
  }
  return pLayer->GetImage()->GetDepth();
}



unsigned char DocumentInfo::GetTrueBitDepth()
{
  return m_BitDepth;
}



void DocumentInfo::SetModify( BOOL bModified )
{
  CString         cDummy;


  if ( m_pDoc->GetTitle().GetLength() == 0 )
  {
    return;
  }
  if ( bModified )
  {
    if ( m_pDoc->GetTitle().GetAt( m_pDoc->GetTitle().GetLength() - 1 ) != '*' )
    {
      m_pDoc->SetTitle( m_pDoc->GetTitle() + " *" );
    }
  }
  else
  {
    if ( m_pDoc->GetTitle().GetAt( m_pDoc->GetTitle().GetLength() - 1 ) == '*' )
    {
      cDummy = m_pDoc->GetTitle();
      cDummy.Delete( cDummy.GetLength() - 2, 2 );
      m_pDoc->SetTitle( cDummy );
    }
    // Undo-Liste nach Speichern usw. lˆschen
    m_UndoManager.EmptyUndoList();
  }
  m_pDoc->SetModifiedFlag( bModified );
}



bool DocumentInfo::PasteFromClipBoard( int iMode, ViewInfo* pAlternativeViewInfo )
{
  // zuerst auf Formate pr¸fen

  HANDLE          hPrivateData  = NULL;
  HANDLE          hBitmap       = NULL;
  HANDLE          hText         = NULL;
  HENHMETAFILE    hEnhMF        = NULL;

  hPrivateData = GetClipboardData( pSettings->m_dwClipboardFormat );
  if ( hPrivateData == NULL )
  {
    hBitmap = GetClipboardData( CF_DIB );
    if ( hBitmap == NULL )
    {
      hEnhMF = (HENHMETAFILE)GetClipboardData( CF_ENHMETAFILE );
      if ( hEnhMF == NULL )
      {
        // Get handle of clipboard object for ANSI text
        hText = GetClipboardData( CF_UNICODETEXT );
      }
    }
  }

  if ( ( hBitmap == NULL )
  &&   ( hPrivateData == NULL )
  &&   ( hEnhMF == NULL )
  &&   ( hText == NULL ) )
  {
    MessageBoxA( AfxGetMainWnd()->GetSafeHwnd(), "Clipboard doesn't contain any paste-able image or is empty!", "Error", MB_OK | MB_APPLMODAL | MB_ICONSTOP );
    return false;
  }

  ViewInfo*    pViewInfo = pAlternativeViewInfo;
  if ( pViewInfo == NULL )
  {
    pViewInfo = theApp.GetActiveViewInfo();
  }

  CPainterImagePackage*   pPack = NULL;

  if ( hPrivateData != NULL )
  {
    BYTE*   pData = (BYTE*)GlobalLock( hPrivateData );

    DWORD   dwWidth   = *(DWORD*)pData;
    DWORD   dwHeight  = *(DWORD*)( pData + 4 );
    DWORD   dwDepth   = *(DWORD*)( pData + 8 );

    pPack = new CPainterImagePackage();

    pPack->m_pImage       = new GR::Graphic::Image( dwWidth, dwHeight, (BYTE)dwDepth );
    pPack->m_pImageMask   = new GR::Graphic::Image( dwWidth, dwHeight, 8 );

    if ( dwDepth <= 8 )
    {
      pPack->m_Palette.Create( 1 << dwDepth );
    }

    GR::Graphic::ContextDescriptor    cdImage( pPack->m_pImage, &pPack->m_Palette );
    GR::Graphic::ContextDescriptor    cdMask( pPack->m_pImageMask );

    int   iMaskOffset = 12 + cdImage.DataSize();
    memcpy( cdImage.Data(), pData + 12, cdImage.DataSize() );

    if ( cdImage.BitsProPixel() <= 8 )
    {
      memcpy( pPack->m_Palette.Data(), pData + 12 + cdImage.DataSize(), 3 * pPack->m_Palette.Entries() );
      iMaskOffset += 3 * cdImage.Palette().Entries();
    }
    memcpy( cdMask.Data(), pData + iMaskOffset, cdMask.DataSize() );
  }
  else if ( hEnhMF != NULL )
  {
    //dh::Log( "CF_ENHMETAFILE available" );
    HDC   hdcScreen = GetDC( NULL );
    int hRes = GetDeviceCaps(hdcScreen, HORZRES);
    int vRes = GetDeviceCaps(hdcScreen, VERTRES);
    int hSize = GetDeviceCaps(hdcScreen, HORZSIZE);
    int vSize = GetDeviceCaps(hdcScreen, VERTSIZE);
    int dpiX = GetDeviceCaps(hdcScreen, LOGPIXELSX);
    int dpiY = GetDeviceCaps(hdcScreen, LOGPIXELSY);
    float dpiScreenX = ((float)hRes / (float)hSize) * 25.4f; // 25.4mm per inch
    float dpiScreenY = ((float)vRes / (float)vSize) * 25.4f; // 25.4mm per inch
    float dpiScaleX = (float)dpiScreenX / dpiX;
    float dpiScaleY = (float)dpiScreenY / dpiY;
    ReleaseDC(NULL, hdcScreen);

    ENHMETAHEADER     header;

    ZeroMemory( &header, sizeof( header ) );
    header.nSize = sizeof( ENHMETAHEADER );

    GetEnhMetaFileHeader( hEnhMF, sizeof( header ), &header );

    // from the metafile header
    RECT      rect;

    rect.top = (int)((float)(header.rclFrame.top) * 
            vRes / (vSize*100.0f));
    rect.left = (int)((float)(header.rclFrame.left) * 
            hRes / (hSize*100.0f));
    rect.right = (int)((float)(header.rclFrame.right) * 
            hRes / (hSize*100.0f));
    rect.bottom = (int)((float)(header.rclFrame.bottom) * 
            vRes / (vSize*100.0f));

    //Calculate the Width and Height of the metafile
    int imageWidth  = abs( rect.right - rect.left );
    int imageHeight = abs( rect.bottom-rect.top );

    pPack = new CPainterImagePackage();
    pPack->m_pImage = new GR::Graphic::Image( imageWidth, imageHeight, 24 );

    GR::Graphic::GDIPage   page;
    
    page.Create( theApp.m_pMainWnd->GetSafeHwnd(), imageWidth, imageHeight, 24 );
    page.Box( 0, 0, imageWidth - 1, imageHeight - 1, 0xffffffff );


    HDC hDC = page.GetDC();

    RECT    rc;

    SetRect( &rc, 0, 0, imageWidth, imageHeight );
     
    PlayEnhMetaFile( hDC, hEnhMF, &rc ); 
     
    // do not release the metafile handle, it belongs to the clipboard

    page.ReleaseDC();

    pPack->m_pImage->GetImage( &page, 0, 0 );
  } 
  else if ( hText != NULL )
  {
    GR::String    clipText;

    HANDLE hClipboardData = GetClipboardData( CF_UNICODETEXT );
    if ( hClipboardData )
    {
      WCHAR* pchData = (WCHAR*)GlobalLock( hClipboardData );
      if ( pchData )
      {
        GR::WString   wString = pchData;
        GlobalUnlock( hClipboardData );

        clipText = GR::Convert::ToUTF8( wString );

        if ( clipText.StartsWith( "<img " ) )
        {
          // it's an HTML image (Teams you stupid POS)
          pPack = CreateImagePackFromHTMLTag( clipText );
        }
      }
    }
  }
  else
  {
    pPack = CreateImagePackFromHDIB( hBitmap );
  }

  if ( !pPack )
  {
    CloseClipboard();
    MessageBoxA( AfxGetMainWnd()->GetSafeHwnd(), "Fehler beim Lesen vom Clipboard!", "Fehler", MB_OK | MB_APPLMODAL | MB_ICONSTOP );
    return false;
  }

  SafeDelete( pViewInfo->m_pImageFloatingSelection );
  SafeDelete( pViewInfo->m_pImageFloatingSelectionMask );

  // im Moment
  pViewInfo->m_pImageFloatingSelectionMask = pPack->m_pImageMask;
  pPack->m_pImageMask = NULL;

  GR::Graphic::Palette      palTemp;

  //DWORD   dwTargetDepth = GetBitDepth();
  DWORD     dwTargetDepth = pViewInfo->GetViewBitDepth();
  if ( iMode == 1 )
  {
    dwTargetDepth = pPack->m_pImage->GetDepth();
  }

  if ( dwTargetDepth <= 8 )
  {
    SafeDelete( pViewInfo->m_pImageFloatingSelection );

    if ( pViewInfo->m_pPalette )
    {
      // nur Palette mappen, wenn unterschiedlich
      if ( ( GetPalette( CurrentFrame() ) )
      &&   ( !GetPalette( CurrentFrame() )->IsIdentic( *pViewInfo->m_pPalette ) ) )
      {
        CColorReducer     Reducer( 1 << dwTargetDepth );

        Reducer.AddSourceToColorCube( pViewInfo->m_pPalette );
        Reducer.Calculate();

        CPainterImagePackage*   pPackTarget = Reducer.Reduce( pPack );

        SafeDelete( pPack->m_pImage );
        SafeDelete( pPack );

        pViewInfo->m_pImageFloatingSelection = pPackTarget->m_pImage;

        pPackTarget->m_pImage = NULL;
        SafeDelete( pPackTarget );
      }
      else
      {
        pViewInfo->m_pImageFloatingSelection = pPack->m_pImage;
        SafeDelete( pPack );
      }
    }
    else
    {
      pViewInfo->m_pImageFloatingSelection = pPack->m_pImage;

      palTemp = pPack->m_Palette;

      SafeDelete( pPack );
    }

    pViewInfo->m_FloatingSelection = true;
    m_SelectionFlags = 0;
    RecreateAllViews();
  }
  else if ( ( dwTargetDepth == 16 )
  ||        ( dwTargetDepth == 15 ) )
  {
    GR::Graphic::Image*   pImageTarget = NULL;
    switch ( pPack->m_pImage->GetDepth() )
    {
      case 8:
        {
          pImageTarget = ConvertImage8to16( pPack->m_pImage, &pPack->m_Palette );
        }
        break;
      case 24:
        {
          pImageTarget = ConvertImage24to16( pPack->m_pImage );
        }
        break;
      case 32:
        {
          pImageTarget = ConvertImage32to16( pPack->m_pImage );
        }
        break;
    }
    if ( pImageTarget )
    {
      // eine Umwandlung hat stattgefunden
      SafeDelete( pPack->m_pImage );
      pPack->m_pImage = pImageTarget;
    }

    if ( pPack->m_pImage->GetDepth() != dwTargetDepth )
    {
      AfxMessageBox( GR::Convert::ToUTF16( CMisc::printf( "Conversion from %d to %d not supported!", pPack->m_pImage->GetDepth(), dwTargetDepth ) ).c_str() );
      SafeDelete( pPack->m_pImage );
      SafeDelete( pPack );
      return false;
    }

    pViewInfo->m_pImageFloatingSelection = pPack->m_pImage;
    SafeDelete( pPack );
  }
  else if ( dwTargetDepth == 24 )
  {
    GR::Graphic::Image   *pImageTarget = NULL;
    switch ( pPack->m_pImage->GetDepth() )
    {
      case 8:
        {
          pImageTarget = ConvertImage8to24( pPack->m_pImage, &pPack->m_Palette );
        }
        break;
      case 15:
      case 16:
        {
          pImageTarget = ConvertImage16to24( pPack->m_pImage );
        }
        break;
      case 32:
        {
          pImageTarget = ConvertImage32to24( pPack->m_pImage );
        }
        break;
    }
    if ( pImageTarget )
    {
      // eine Umwandlung hat stattgefunden
      SafeDelete( pPack->m_pImage );
      pPack->m_pImage = pImageTarget;
    }

    if ( pPack->m_pImage->GetDepth() != dwTargetDepth )
    {
      AfxMessageBox( GR::Convert::ToUTF16( CMisc::printf( "Conversion from %d to %d not supported!", pPack->m_pImage->GetDepth(), dwTargetDepth ) ).c_str() );
      SafeDelete( pPack->m_pImage );
      SafeDelete( pPack );
      return false;
    }

    pViewInfo->m_pImageFloatingSelection = pPack->m_pImage;
    SafeDelete( pPack );
  }
  else if ( dwTargetDepth == 32 )
  {
    GR::Graphic::Image*   pImageTarget = NULL;
    switch ( pPack->m_pImage->GetDepth() )
    {
      case 8:
        {
          pImageTarget = ConvertImage8to32( pPack->m_pImage, &pPack->m_Palette );
        }
        break;
      case 15:
      case 16:
        {
          pImageTarget = ConvertImage16to32( pPack->m_pImage );
        }
        break;
      case 24:
        {
          pImageTarget = ConvertImage24to32( pPack->m_pImage );
        }
        break;
    }
    if ( pImageTarget )
    {
      // eine Umwandlung hat stattgefunden
      SafeDelete( pPack->m_pImage );
      pPack->m_pImage = pImageTarget;
    }

    if ( pPack->m_pImage->GetDepth() != dwTargetDepth )
    {
      AfxMessageBox( GR::Convert::ToUTF16( CMisc::printf( "Conversion from %d to %d not supported!", pPack->m_pImage->GetDepth(), dwTargetDepth ) ).c_str() );
      SafeDelete( pPack->m_pImage );
      SafeDelete( pPack );
      return false;
    }

    pViewInfo->m_pImageFloatingSelection = pPack->m_pImage;
    SafeDelete( pPack );
  }
  else
  {
    AfxMessageBox( GR::Convert::ToUTF16( CMisc::printf( "Color depth %d not supported!", pPack->m_pImage->GetDepth() ) ).c_str() );
    SafeDelete( pPack->m_pImage );
    SafeDelete( pPack );
    return false;
  }

  if ( iMode == 1 )
  {
    // das Bild soll direkt eingesetzt werden (Paste as New Image)
    m_DocType = DT_IMAGE;
    m_BitDepth = pViewInfo->m_pImageFloatingSelection->GetDepth();
    SetSize( pViewInfo->m_pImageFloatingSelection->GetWidth(), pViewInfo->m_pImageFloatingSelection->GetHeight() );
    SetModify( TRUE );

    AddLayer( pViewInfo->m_pImageFloatingSelection, AddFrame() );

    *GetPalette( 0 ) = palTemp;

    {
      // PFUI - PFUI - PFUI (nuja, schon besser)
      POSITION              pos;

      CView*         pFirstView;

      pos = m_pDoc->GetFirstViewPosition();
      pFirstView = m_pDoc->GetNextView( pos );

      pFirstView->OnInitialUpdate();
    }

    RecreateAllViews();

    pViewInfo->m_FloatingSelection = FALSE;
    pViewInfo->m_pImageFloatingSelection = NULL;
  }
  else
  {
    // jetzt gibt es eine Floating Selection
    pViewInfo->m_FloatingSelection = TRUE;
  }
  return true;
}



void DocumentInfo::RecreateAllViews()
{
  POSITION pos = m_pDoc->GetFirstViewPosition();
  while ( pos != NULL )
  {
    if ( m_DocType == DT_IMAGE )
    {
      CPainterView  *pView = (CPainterView*)m_pDoc->GetNextView( pos );
      pView->m_viewInfo.InitializePage();
      pView->m_viewInfo.AdjustWindowSize();
    }
    else if ( m_DocType == DT_FONT )
    {
      CPainterFontView  *pView = (CPainterFontView*)m_pDoc->GetNextView( pos );
      pView->m_viewInfo.InitializePage();
      pView->m_viewInfo.AdjustWindowSize();
    }
    else
    {
      TRACE( "DocumentInfo::RecreateAllViews unknown document type\n" );
      return;
    }
  }   
}



void DocumentInfo::ReduceColors( int BitsPerColor )
{
  CColorReducer     Reducer( BitsPerColor );

  for ( size_t i = 0; i < m_LayeredFrames.size(); ++i )
  {
    for ( size_t iLayer = 0; iLayer < m_LayeredFrames[i].LayerCount(); ++iLayer )
    {
      CPainterImagePackage*   pPack = new CPainterImagePackage( GetImage( i, iLayer ), GetPalette( i ) );

      Reducer.AddSourceToColorCube( pPack );

      delete pPack;
    }
  }

  Reducer.Calculate();

  for ( size_t i = 0; i < m_LayeredFrames.size(); ++i )
  {
    for ( size_t iLayer = 0; iLayer < m_LayeredFrames[i].LayerCount(); ++iLayer )
    {
      CPainterImagePackage*   pPack = new CPainterImagePackage( GetImage( i, iLayer ), GetPalette( i ) );

      CPainterImagePackage*   pResult;

      pResult = Reducer.Reduce( pPack );

      GetLayer( i, iLayer )->SetLayerImage( pResult->m_pImage );

      *GetPalette( i ) = pResult->m_Palette;

      delete pResult;
      delete pPack;
    }
  }
}



void DocumentInfo::ConvertTo1bpp()
{
  if ( ( m_DocType == DT_IMAGE )
  ||   ( m_DocType == DT_FONT ) )
  {
    AddUndoDepthChange();

    ReduceColors( 2 );

    m_BitDepth = 1;
  }
  else
  {
    AfxMessageBox( _T( "Unsupported Document Type!" ) );
  }

  RecreateAllViews();

  SetModify( TRUE );

  pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );

  RedrawAllViews();
}



void DocumentInfo::ConvertTo4bpp()
{
  if ( ( m_DocType == DT_IMAGE )
  ||   ( m_DocType == DT_FONT ) )
  {
    AddUndoDepthChange();

    ReduceColors( 16 );

    m_BitDepth = 4;
  }
  else
  {
    AfxMessageBox( _T( "Unsupported Document Type!" ) );
  }

  RecreateAllViews();

  SetModify( TRUE );

  pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );

  RedrawAllViews();
}



void DocumentInfo::ConvertTo8bpp()
{
  if ( ( m_DocType == DT_IMAGE )
  ||   ( m_DocType == DT_FONT ) )
  {
    AddUndoDepthChange();
    ReduceColors( 256 );
    m_BitDepth = 8;
  }
  else
  {
    AfxMessageBox( _T( "Unsupported Document Type!" ) );
  }

  RecreateAllViews();

  SetModify( TRUE );

  pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );

  RedrawAllViews();
}



void DocumentInfo::ConvertTo16bpp()
{

  GR::Graphic::Image         *pNewImage;

  CLayer         *pLayer;

  size_t         i;



  if ( ( m_DocType == DT_IMAGE )
  ||   ( m_DocType == DT_FONT ) )
  {
    if ( m_BitDepth == 1 )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage1to16( pLayer->GetImage(), GetPalette( iFrame ) );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else if ( m_BitDepth == 4 )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage4to16( pLayer->GetImage(), GetPalette( iFrame ) );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else if ( m_BitDepth == 8 )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage8to16( pLayer->GetImage(), GetPalette( iFrame ) );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else if ( m_BitDepth == 24 )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage24to16( pLayer->GetImage() );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else if ( m_BitDepth == 32 )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage32to16( pLayer->GetImage() );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else
    {
      AfxMessageBox( _T( "This conversion is not yet supported!" ) );
      return;
    }
    m_BitDepth = 16;
  }
  else
  {
    AfxMessageBox( _T( "Unknown Document Type!" ) );
  }

  RecreateAllViews();

  SetModify( TRUE );

  pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );

  RedrawAllViews();

}



void DocumentInfo::ConvertTo24bpp()
{
  GR::Graphic::Image         *pNewImage;

  size_t            i;

  CLayer*           pLayer;


  if ( ( m_DocType == DT_IMAGE )
  ||   ( m_DocType == DT_FONT ) )
  {
    if ( m_BitDepth == 1 )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage1to24( pLayer->GetImage(), GetPalette( iFrame ) );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else if ( m_BitDepth == 4 )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage4to24( pLayer->GetImage(), GetPalette( iFrame ) );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else if ( m_BitDepth == 8 )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage8to24( pLayer->GetImage(), GetPalette( iFrame ) );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else if ( ( m_BitDepth == 15 )
    ||        ( m_BitDepth == 16 ) )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage16to24( pLayer->GetImage() );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else if ( m_BitDepth == 32 )
    {
      AddUndoDepthChange();
      for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
      {
        for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
        {
          pLayer = GetLayer( iFrame, i );
          pNewImage = ConvertImage32to24( pLayer->GetImage() );

          pLayer->SetLayerImage( pNewImage );
        }
      }
    }
    else
    {
      AfxMessageBox( _T( "This conversion is not yet supported!" ) );
      return;
    }
    m_BitDepth = 24;
  }

  RecreateAllViews();

  SetModify( TRUE );

  pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );

  RedrawAllViews();

}



void DocumentInfo::ConvertTo32bpp()
{

  size_t            i;

  CLayer*           pLayer;


  AddUndoDepthChange();
  for ( int iFrame = 0; iFrame < (int)m_LayeredFrames.size(); iFrame++ )
  {
    for ( i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
    {
      pLayer = GetLayer( iFrame, i );

      GR::Graphic::Image*    pNewImage = new GR::Graphic::Image( pLayer->GetImage()->GetWidth(), 
                                                  pLayer->GetImage()->GetHeight(), 
                                                  32, 0, 0 );

      GR::Graphic::ContextDescriptor  cdImage( pLayer->GetImage(), GetPalette( iFrame ) );
      
      if ( !cdImage.ConvertInto( &GR::Graphic::ContextDescriptor( pNewImage ) ) )
      {
        dh::Log( "Conversion failed (%d to 32)\n", pLayer->GetImage()->GetDepth() );
      }
      pLayer->SetLayerImage( pNewImage );
    }
  }
  m_BitDepth = 32;

  RecreateAllViews();

  SetModify( TRUE );

  pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );

  RedrawAllViews();

}



void DocumentInfo::MapToPalette( GR::Graphic::Palette *pNewPalette )
{

  GR::Graphic::Image*       pNewImage;

  CProgress*        pProgress;



  CLayer*   pActiveLayer = ActiveLayer();
  if ( pActiveLayer == NULL )
  {
    return;
  }

  if ( m_DocType == DT_IMAGE )
  {
    AddUndoDepthChange();
    for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); ++iFrame )
    {
      for ( size_t iLayer = 0; iLayer < m_LayeredFrames[iFrame].LayerCount(); ++iLayer )
      {
        pNewImage = new GR::Graphic::Image( GetImage( iFrame, iLayer )->GetWidth(), 
                                    GetImage( iFrame, iLayer )->GetHeight(), 8, 0, 0 );

        pProgress = new CProgress( "Converting image...", 0, m_DocWidth * m_DocHeight );

        GR::Graphic::ContextDescriptor    cdTarget( pNewImage, pNewPalette ),
                              cdSource( GetImage( iFrame, iLayer ), GetPalette( iFrame ) );

        for ( DWORD i = 0; i < m_DocWidth; i++ )
        {
          for ( DWORD j = 0; j < m_DocHeight; j++ )
          {
            cdTarget.PutPixel( i, j, cdSource.GetPixel( i, j ) );
          }
          pProgress->SetPosition( i * m_DocHeight, m_DocWidth * m_DocHeight );
        }
        pProgress->DestroyWindow();
        delete pProgress;

        if ( pNewImage != NULL )
        {
          pActiveLayer->SetLayerImage( pNewImage );
        }
      }
    }

    *GetPalette( CurrentFrame() ) = *pNewPalette;

    m_BitDepth = 8;
  }
  else
  {
    theApp.m_pMainWnd->MessageBox( _T( "Format not supported for multi frames (only single images)." ), _T( "Notice" ), 0 );
    return;
  }

  RecreateAllViews();
  SetModify( TRUE );

  pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );

  RedrawAllViews();

}



void DocumentInfo::Rotate()
{
  AddUndoSizeChange();

  CLayer     *pLayer;

  GR::Graphic::Image     *pNewImage;

  DWORD         dwBreite,
                dwHoehe;


  //AddUndoRect( 0, 0, 
  for ( size_t i = 0; i < m_LayeredFrames[0].LayerCount(); i++ )
  {
    pLayer = GetLayer( 0, i );

    pNewImage = RotateImage( pLayer->GetImage() );
    pLayer->SetLayerImage( pNewImage );

    if ( pLayer->m_HasMask )
    {
      pNewImage = RotateImage( pLayer->GetMask() );
      pLayer->SetLayerMask( pNewImage );
    }
  }

  // Selection
  pNewImage = RotateImage( m_pImageSelection );
  delete m_pImageSelection;
  m_pImageSelection = pNewImage;

  pNewImage = RotateImage( m_pImageEffectMask );
  delete m_pImageEffectMask;
  m_pImageEffectMask = pNewImage;

  UpdateMarchingAnts();

  dwBreite  = m_DocHeight;
  dwHoehe   = m_DocWidth;

  m_DocWidth  = dwBreite;
  m_DocHeight = dwHoehe;

  RecreateAllViews();

  SetModify( TRUE );

  RedrawAllViews();

}



void DocumentInfo::ZoomIn( void )
{

  /*
  RECT          rc;

  int           iWidth,
                iHeight;


  MessageBox( NULL, "zoomin", "aha", 0 );
  POSITION pos = m_pDoc->GetFirstViewPosition();
  CView* pView = m_pDoc->GetNextView( pos );
  pView->GetClientRect( &rc );
  if ( wZoomFaktor < 1600 )
  {
    wZoomFaktor *= 2;

    //GetParent()->GetParent()->GetClientRect( &rc );

    // Fenstergrˆﬂe anpassen
    if ( !( GetWindowLong( pView->GetParent()->GetSafeHwnd(), GWL_STYLE ) & WS_MAXIMIZE ) )
    {
      if ( ucDocumentType == DOCUMENT_TYPE_FONT )
      {
        if ( ( (int)pFont->GetLetter( ucFontLetter )->GetWidth() * wZoomFaktor / 100 < rc.right - rc.left )
        &&   ( (int)pFont->GetLetter( ucFontLetter )->GetHeight() * wZoomFaktor / 100 < rc.bottom - rc.top ) )
        {
          iWidth = 200 + pFont->GetLetter( ucFontLetter )->GetWidth() * wZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CXSIZEFRAME ) + 2 * GetSystemMetrics( SM_CXEDGE );
          if ( iWidth < 400 )
          {
            iWidth = 400;
          }
          iHeight = pFont->GetLetter( ucFontLetter )->GetHeight() * wZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CYSIZEFRAME ) + 2 * GetSystemMetrics( SM_CYEDGE ) + GetSystemMetrics( SM_CYCAPTION );
          if ( iHeight < 200 )
          {
            iHeight = 200;
          }
          pView->GetParentFrame()->SetWindowPos( &CWnd::wndTop, 0, 0, iWidth, iHeight, SWP_NOMOVE );
        }
      }
      else
      {
        {
          iWidth = ulDocWidth * wZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CXSIZEFRAME ) + 2 * GetSystemMetrics( SM_CXEDGE );
          iHeight = ulDocHeight * wZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CYSIZEFRAME ) + 2 * GetSystemMetrics( SM_CYEDGE ) + GetSystemMetrics( SM_CYCAPTION );
          if ( ucDocumentType == DOCUMENT_TYPE_FONT )
          {
            iWidth += 200;
          }
          pView->GetParentFrame()->SetWindowPos( &CWnd::wndTop, 0, 0, iWidth, iHeight, SWP_NOMOVE );
        }
      }
    }
    if ( ucDocumentType == DOCUMENT_TYPE_BTN )
    {
      //( (CScrollView*)pView )->SetScrollSizes( MM_TEXT, CSize( ( ulDocWidth * wZoomFaktor ) / 100, ( ulDocHeight * wZoomFaktor ) / 100 ), CSize( 10, 10 ), CSize( 1, 1 ) );
      ( (CScrollView*)pView )->SetScrollSizes( MM_TEXT, 
                        CSize( ( ulDocWidth * wZoomFaktor ) / 100, ( ulDocHeight * wZoomFaktor ) / 100 ), 
                        CSize( 10, 10 ), 
                        CSize( wZoomFaktor / 100, wZoomFaktor / 100 ) );
      ( (CScrollView*)pView )->ResizeParentToFit();
    }
    else
    {
      theApp.ResizeWindow( pView->GetDocument(), this, pView->GetSafeHwnd(), wZoomFaktor, FALSE );
    }
    pView->Invalidate( FALSE );
  }

  theApp.ShowStatusMessage();
  */

}



void DocumentInfo::ZoomOut( void )
{

  /*
  RECT                  rc;

  int                   iWidth,
                        iHeight;


  POSITION pos = m_pDoc->GetFirstViewPosition();
  CView* pView = m_pDoc->GetNextView( pos );
  pView->GetClientRect( &rc );
  if ( wZoomFaktor > 25 )
  {
    wZoomFaktor /= 2;

    // Fenstergrˆﬂe anpassen
    if ( !( GetWindowLong( pView->GetParent()->GetSafeHwnd(), GWL_STYLE ) & WS_MAXIMIZE ) )
    {
      if ( ucDocumentType == DOCUMENT_TYPE_FONT )
      {
        if ( ( (int)pFont->GetLetter( ucFontLetter )->GetWidth() * wZoomFaktor / 100 < rc.right - rc.left )
        &&   ( (int)pFont->GetLetter( ucFontLetter )->GetHeight() * wZoomFaktor / 100 < rc.bottom - rc.top ) )
        {
          iWidth = 200 + pFont->GetLetter( ucFontLetter )->GetWidth() * wZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CXSIZEFRAME ) + 2 * GetSystemMetrics( SM_CXEDGE );
          if ( iWidth < 400 )
          {
            iWidth = 400;
          }
          iHeight = pFont->GetLetter( ucFontLetter )->GetHeight() * wZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CYSIZEFRAME ) + 2 * GetSystemMetrics( SM_CYEDGE ) + GetSystemMetrics( SM_CYCAPTION );
          if ( iHeight < 200 )
          {
            iHeight = 200;
          }
          pView->GetParentFrame()->SetWindowPos( &CWnd::wndTop, 0, 0, iWidth, iHeight, SWP_NOMOVE );
        }
      }
      else
      {
        if ( ( (int)ulDocWidth * wZoomFaktor / 100 < rc.right - rc.left )
        &&   ( (int)ulDocHeight * wZoomFaktor / 100 < rc.bottom - rc.top ) )
        {
          iWidth = ulDocWidth * wZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CXSIZEFRAME ) + 2 * GetSystemMetrics( SM_CXEDGE );
          iHeight = ulDocHeight * wZoomFaktor / 100 + 2 * GetSystemMetrics( SM_CYSIZEFRAME ) + 2 * GetSystemMetrics( SM_CYEDGE ) + GetSystemMetrics( SM_CYCAPTION );
          if ( ucDocumentType == DOCUMENT_TYPE_FONT )
          {
            iWidth += 200;
          }
          pView->GetParentFrame()->SetWindowPos( &CWnd::wndTop, 0, 0, iWidth, iHeight, SWP_NOMOVE );
        }
      }
    }

    if ( ucDocumentType == DOCUMENT_TYPE_BTN )
    {
      ( (CScrollView*)pView )->SetScrollSizes( MM_TEXT, 
                        CSize( ( ulDocWidth * wZoomFaktor ) / 100, ( ulDocHeight * wZoomFaktor ) / 100 ), 
                        CSize( 10, 10 ), 
                        CSize( wZoomFaktor / 100, wZoomFaktor / 100 ) );

      ( (CScrollView*)pView )->ResizeParentToFit();
    }
    else
    {
      theApp.ResizeWindow( pView->GetDocument(), this, pView->GetSafeHwnd(), wZoomFaktor / 2, FALSE );
    }
    pView->Invalidate( FALSE );
  }

  theApp.ShowStatusMessage();
  */

}



void DocumentInfo::AddUndoRectAll()
{
  m_UndoManager.StartUndoGroup();

  if ( ( m_DocType == DT_IMAGE )
  ||   ( m_DocType == DT_FONT ) )
  {
    for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); iFrame++ )
    {
      for ( size_t i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
      {
        m_UndoManager.AddUndoStep( new CUndoImageChange( 0, 
                                                         0, 
                                                         m_DocWidth - 1, 
                                                         m_DocHeight - 1, 
                                                         iFrame,
                                                         i, 
                                                         pSettings->m_dwFunction,
                                                         this,
                                                         false ),
                                   FALSE );

      }
    }
  }
}



void DocumentInfo::AddUndoRect( int iX1, int iY1, int iX2, int iY2, int iFrame, int iLayer, bool bMask, BOOL bStartNewUndoGroup )
{

  CLayer*   pActiveLayer = ActiveLayer();
  if ( pActiveLayer == NULL )
  {
    return;
  }

  if ( iFrame == -1 )
  {
    iFrame = m_CurrentFrame;
  }
  if ( iLayer == -1 )
  {
    iLayer = m_CurrentLayer;
  }

  // RECT-Koordinaten ordnen
  if ( iX1 < 0 )
  {
    iX1 = 0;
  }
  if ( iX2 >= (int)m_DocWidth )
  {
    iX2 = (int)m_DocWidth - 1;
  }
  if ( iY1 < 0 )
  {
    iY1 = 0;
  }
  if ( iY2 >= (int)m_DocHeight )
  {
    iY2 = (int)m_DocHeight - 1;
  }
  if ( iX1 > iX2 )
  {
    int iDummy;
    iDummy = iX1;
    iX1 = iX2;
    iX2 = iDummy;
  }
  if ( iY1 > iY2 )
  {
    int iDummy;
    iDummy = iY1;
    iY1 = iY2;
    iY2 = iDummy;
  }

  m_UndoManager.AddUndoStep( new CUndoImageChange( iX1, 
                                                   iY1, 
                                                   iX2, 
                                                   iY2,
                                                   iFrame,
                                                   iLayer, 
                                                   pSettings->m_dwFunction,
                                                   this,
                                                   bMask ),
                             bStartNewUndoGroup );

}



void DocumentInfo::AddUndoDepthChange()
{

  m_UndoManager.AddUndoStep( new CUndoDepthChange( this ) );

}



void DocumentInfo::AddUndoSizeChange()
{

  m_UndoManager.AddUndoStep( new CUndoSizeChange( this ) );

}



void DocumentInfo::AddUndoFrameChange( size_t dwFrameIndex, CUndoFrameChange::eFrameChangeType fcType )
{

  m_UndoManager.AddUndoStep( new CUndoFrameChange( this,
                                                   fcType,
                                                   dwFrameIndex ) );

}



void DocumentInfo::AddUndoLayerChange( size_t dwLayerIndex, size_t dwLayerIndex2, 
                                        CUndoLayerChange::eLayerChangeType lcType,
                                        BOOL bStartNewGroup )
{

  m_UndoManager.AddUndoStep( new CUndoLayerChange( this,
                              CurrentFrame(),
                              dwLayerIndex,
                              dwLayerIndex2,
                              lcType ),
                             bStartNewGroup );

}



void DocumentInfo::Undo()
{

  m_UndoManager.Undo();
  if ( !m_UndoManager.IsUndoPossible() )
  {
    // alle Undos abgearbeitet
    SetModify( FALSE );
  }

}



void DocumentInfo::SetSize( DWORD dwWidth, DWORD dwHeight )
{
  m_DocWidth  = dwWidth;
  m_DocHeight = dwHeight;

  SafeDelete( m_pImageSelection );
  SafeDelete( m_pImageEffectMask );

  m_pImageSelection   = new GR::Graphic::Image( m_DocWidth, m_DocHeight, 8 );
  m_pImageEffectMask  = new GR::Graphic::Image( m_DocWidth, m_DocHeight, 8 );

  UpdateMarchingAnts();
}



void DocumentInfo::EmptySelection()
{
  CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( m_pImageSelection, &pSettings->m_AlphaPalette );

  pCD->Box( 0, 0, m_DocWidth, m_DocHeight, 0 );

  delete pCD;

  m_HasSelection = false;
  UpdateMarchingAnts();
}



void DocumentInfo::AddRectSelection( int iX, int iY, int iWidth, int iHeight )
{
  m_HasSelection = true;

  for ( int j = iY; j < iY + iHeight; j++ )
  {
    memset( ( (BYTE*)m_pImageSelection->GetData() ) + iX + j * m_pImageSelection->GetWidth(), 255, iWidth );
  }
  UpdateMarchingAnts();
}



void DocumentInfo::RemoveRectSelection( int iX, int iY, int iWidth, int iHeight )
{
  for ( int j = iY; j < iY + iHeight; j++ )
  {
    memset( ( (BYTE*)m_pImageSelection->GetData() ) + iX + j * m_pImageSelection->GetWidth(), 0, iWidth );
  }
  UpdateMarchingAnts();
}



void DocumentInfo::UpdateMarchingAnts()
{
  // alle Marching Ants tˆten
  m_MarchingAnts.clear();
  m_MarchingAntCount = 0;


  int     iX1 = m_pImageSelection->GetWidth(),
          iY1 = m_pImageSelection->GetHeight(),
          iX2 = 0,
          iY2 = 0;

  DWORD   dwSideFlags = 0;

  // Ants erzeugen
  CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( m_pImageSelection, &pSettings->m_AlphaPalette );

  for ( int j = 0; j < m_pImageSelection->GetHeight(); j++ )
  {
    for ( int i = 0; i < m_pImageSelection->GetWidth(); i++ )
    {
      if ( pCD->GetDirectPixel( i, j ) )
      {
        dwSideFlags = 0;

        if ( !pCD->GetDirectPixel( i, j - 1 ) )
        {
          dwSideFlags |= 0x10;
        }
        if ( !pCD->GetDirectPixel( i, j + 1 ) )
        {
          dwSideFlags |= 0x20;
        }
        if ( !pCD->GetDirectPixel( i - 1, j ) )
        {
          dwSideFlags |= 0x40;
        }
        if ( !pCD->GetDirectPixel( i + 1, j ) )
        {
          dwSideFlags |= 0x80;
        }

        if ( dwSideFlags )
        {
          // hier muﬂ eine Ameise hin
          if ( m_MarchingAnts.capacity() < m_MarchingAntCount + 1 )
          {
            // der Vector muﬂ vergrˆﬂert werden
            m_MarchingAnts.reserve( m_MarchingAnts.capacity() + 200 );
          }

          m_MarchingAnts.push_back( CMarchingAntDot( i, j, BYTE( dwSideFlags | ( ( i + j ) % 4 ) ) ) );

          iX1 = min( i, iX1 );
          iY1 = min( j, iY1 );
          iX2 = max( i, iX2 );
          iY2 = max( j, iY2 );

          m_MarchingAntCount++;
        }
      }
    }
  }

  delete pCD;

  m_Selection.left    = iX1;
  m_Selection.top     = iY1;
  m_Selection.right   = iX2;
  m_Selection.bottom  = iY2;

  if ( m_MarchingAntCount == 0 )
  {
    m_HasSelection = false;
    GR::Graphic::ContextDescriptor    cdSelection( m_pImageSelection, &pSettings->m_AlphaPalette );
    
    cdSelection.Box( 0, 0, cdSelection.Width(), cdSelection.Height(), 0 );
    return;
  }
  else
  {
    m_HasSelection = true;
  }
}



void DocumentInfo::ResizeCanvas( GR::Graphic::ContextDescriptor& Source, GR::Graphic::ContextDescriptor& Target, 
                                 int iNewWidth, int iNewHeight, bool bCenterH, bool bCenterV )
{
  int   iXOffset = 0,
        iYOffset = 0,
        iXO = 0,
        iYO = 0,
        iCopyWidth = Source.Width(),
        iCopyHeight = Source.Height();


  if ( bCenterH )
  {
    iXOffset = ( iNewWidth - iCopyWidth ) / 2;
  }
  if ( bCenterV )
  {
    iYOffset = ( iNewHeight - iCopyHeight ) / 2;
  }

  if ( iCopyWidth > iNewWidth )
  {
    if ( bCenterH )
    {
      iXO = ( iCopyWidth - iNewWidth ) / 2;
      iXOffset = 0;
    }
    iCopyWidth = iNewWidth;
  }
  if ( iCopyHeight > iNewHeight )
  {
    if ( bCenterV )
    {
      iYO = ( iCopyHeight - iNewHeight ) / 2;
      iYOffset = 0;
    }
    iCopyHeight = iNewHeight;
  }

  Source.CopyArea( iXO, iYO, iCopyWidth, iCopyHeight,
                   iXOffset, iYOffset,
                   &Target);
}



void DocumentInfo::ResizeCanvas( int iNewWidth, int iNewHeight, bool bCenterH, bool bCenterV )
{
  AddUndoSizeChange();

  for ( size_t frame = 0; frame < m_LayeredFrames.size(); ++frame )
  {
    std::vector<CLayer*>::iterator   it( m_LayeredFrames[frame].Layers.begin() );

    while ( it != m_LayeredFrames[frame].Layers.end() )
    {
      CLayer* pLayer = *it;

      GR::Graphic::Image* pImage = new GR::Graphic::Image( iNewWidth, iNewHeight, GetTrueBitDepth(), 0, 0 );
      GR::Graphic::Image* pOrigImage = pLayer->GetImage();

      GR::Graphic::ContextDescriptor    cdSrc( pOrigImage, GetPalette( 0 ) );
      GR::Graphic::ContextDescriptor    cdTgt( pImage, GetPalette( 0 ) );

      ResizeCanvas( cdSrc, cdTgt, iNewWidth, iNewHeight, bCenterH, bCenterV );

      pLayer->SetLayerImage( pImage );

      // Maske, falls vorhanden
      if ( pLayer->GetMask() )
      {
        GR::Graphic::Image* pImage = new GR::Graphic::Image( iNewWidth, iNewHeight, pLayer->GetMask()->GetDepth(), 0, 0 );
        GR::Graphic::Image* pOrigImage = pLayer->GetMask();

        GR::Graphic::ContextDescriptor    cdSrc( pOrigImage );
        GR::Graphic::ContextDescriptor    cdTgt( pImage );

        ResizeCanvas( cdSrc, cdTgt, iNewWidth, iNewHeight, bCenterH, bCenterV );
        pLayer->SetLayerMask( pImage );
      }
      it++;
    }

    // Effekt-Maske
    {
      GR::Graphic::Image* pImage = new GR::Graphic::Image( iNewWidth, iNewHeight, 8, 0, 0 );
      GR::Graphic::Image* pOrigImage = m_pImageEffectMask;

      GR::Graphic::ContextDescriptor    cdSrc( pOrigImage );
      GR::Graphic::ContextDescriptor    cdTgt( pImage );

      ResizeCanvas( cdSrc, cdTgt, iNewWidth, iNewHeight, bCenterH, bCenterV );

      GR::Graphic::Image* pImageDummy = pImage;
      pImage = m_pImageEffectMask;
      m_pImageEffectMask = pImageDummy;
      delete pImage;
    }

    // Selection-Image
    {
      GR::Graphic::Image* pImage = new GR::Graphic::Image( iNewWidth, iNewHeight, 8, 0, 0 );
      GR::Graphic::Image* pOrigImage = m_pImageSelection;

      GR::Graphic::ContextDescriptor    cdSrc( pOrigImage );
      GR::Graphic::ContextDescriptor    cdTgt( pImage );

      ResizeCanvas( cdSrc, cdTgt, iNewWidth, iNewHeight, bCenterH, bCenterV );

      GR::Graphic::Image* pImageDummy = pImage;
      pImage = m_pImageSelection;
      m_pImageSelection = pImageDummy;
      delete pImage;
      UpdateMarchingAnts();
    }
  }
  m_DocWidth = iNewWidth;
  m_DocHeight = iNewHeight;

  RecreateAllViews();
  RedrawAllViews();
}



void DocumentInfo::Resize( int iNewWidth, int iNewHeight, DWORD dwResizeType, int ResizePercentageX, int ResizePercentageY )
{

  AddUndoSizeChange();


  GR::Graphic::Image*  pImage = NULL;

  if ( m_DocType == DT_IMAGE )
  {
    for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); ++iFrame )
    {
      for ( size_t i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )    
      {
        pImage = ResizeImage( GetImage( iFrame, i ), GetPalette( iFrame ), iNewWidth, iNewHeight, dwResizeType );
        GetLayer( iFrame, i )->SetLayerImage( pImage );

        pImage = ResizeImage( GetMask( iFrame, i ), &pSettings->m_AlphaPalette, iNewWidth, iNewHeight, dwResizeType );
        GetLayer( iFrame, i )->SetLayerMask( pImage );
      }
    }
    pImage = ResizeImage( m_pImageSelection, &pSettings->m_AlphaPalette, iNewWidth, iNewHeight, dwResizeType );
    SafeDelete( m_pImageSelection );
    m_pImageSelection = pImage;

    pImage = ResizeImage( m_pImageEffectMask, &pSettings->m_AlphaPalette, iNewWidth, iNewHeight, dwResizeType );
    SafeDelete( m_pImageEffectMask );
    m_pImageEffectMask = pImage;
  }
  else if ( m_DocType == DT_FONT )
  {
    for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); ++iFrame )
    {
      for ( size_t i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )    
      {
        GR::Graphic::Image*   pLetterImage = GetImage( iFrame, i );
        GR::Graphic::Image*   pLetterMask = GetMask( iFrame, i );

        int   NewWidth  = pLetterImage->GetWidth() * ResizePercentageX / 100;
        int   NewHeight = pLetterImage->GetHeight() * ResizePercentageY / 100;

        pImage = ResizeImage( pLetterImage, GetPalette( iFrame ), NewWidth, NewHeight, dwResizeType );
        GetLayer( iFrame, i )->SetLayerImage( pImage );

        pImage = ResizeImage( GetMask( iFrame, i ), &pSettings->m_AlphaPalette, NewWidth, NewHeight, dwResizeType );
        GetLayer( iFrame, i )->SetLayerMask( pImage );
      }
    }
    int   NewWidth  = m_pImageSelection->GetWidth() * ResizePercentageX / 100;
    int   NewHeight = m_pImageSelection->GetHeight() * ResizePercentageY / 100;

    pImage = ResizeImage( m_pImageSelection, &pSettings->m_AlphaPalette, NewWidth, NewHeight, dwResizeType );
    SafeDelete( m_pImageSelection );
    m_pImageSelection = pImage;

    NewWidth  = m_pImageEffectMask->GetWidth() * ResizePercentageX / 100;
    NewHeight = m_pImageEffectMask->GetHeight() * ResizePercentageY / 100;
    pImage = ResizeImage( m_pImageEffectMask, &pSettings->m_AlphaPalette, NewWidth, NewHeight, dwResizeType );
    SafeDelete( m_pImageEffectMask );
    m_pImageEffectMask = pImage;
  }

  UpdateMarchingAnts();

  m_DocWidth = iNewWidth;
  m_DocHeight = iNewHeight;

  RecreateAllViews();
  RedrawAllViews();

}



void DocumentInfo::ShiftFrameLeft( size_t iFrame )
{
  GR::Graphic::Image*     pImageSpalte;
  GR::Graphic::Image*     pImageRest;

  for ( size_t i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
  {
    GR::Graphic::Image*    pLayerImage = GetImage( iFrame, i );

    AddUndoRect( 0, 0, pLayerImage->GetWidth() - 1, pLayerImage->GetHeight() - 1, iFrame, i, false, FALSE );

    pImageSpalte = pLayerImage->GetTileImage( 0, 0, 1, (WORD)pLayerImage->GetHeight() );
    pImageRest   = pLayerImage->GetTileImage( 1, 0, WORD( pLayerImage->GetWidth() - 1 ), WORD( pLayerImage->GetHeight() ) );

    GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( iFrame ) );
    GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( iFrame ) );
    GR::Graphic::ContextDescriptor   cdImage( pLayerImage, GetPalette( iFrame ) );

    cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 0, 0, &cdImage );
    cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), pLayerImage->GetWidth() - 1, 0, &cdImage );

    delete pImageSpalte;
    delete pImageRest;

    GR::Graphic::Image*    pLayerImageMask = GetMask( iFrame, i );

    if ( pLayerImageMask )
    {
      AddUndoRect( 0, 0, pLayerImage->GetWidth() - 1, pLayerImage->GetHeight() - 1, iFrame, i, true, FALSE );

      pImageSpalte = pLayerImageMask->GetTileImage( 0, 0, 1, WORD( pLayerImage->GetHeight() ) );
      pImageRest   = pLayerImageMask->GetTileImage( 1, 0, WORD( pLayerImage->GetWidth() - 1 ), WORD( pLayerImage->GetHeight() ) );

      GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( iFrame ) );
      GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( iFrame ) );
      GR::Graphic::ContextDescriptor   cdImage( pLayerImageMask, GetPalette( iFrame ) );

      cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 0, 0, &cdImage );
      cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), pLayerImage->GetWidth() - 1, 0, &cdImage );

      delete pImageSpalte;
      delete pImageRest;
    }
  }
}



void DocumentInfo::ShiftFrameRight( size_t iFrame )
{
  GR::Graphic::Image*     pImageSpalte;
  GR::Graphic::Image*     pImageRest;

  for ( size_t i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
  {
    GR::Graphic::Image*    pLayerImage = GetImage( iFrame, i );

    AddUndoRect( 0, 0, pLayerImage->GetWidth() - 1, pLayerImage->GetHeight() - 1, iFrame, i, false, FALSE );

    pImageSpalte = pLayerImage->GetTileImage( WORD( pLayerImage->GetWidth() - 1 ), 0, 1, WORD( pLayerImage->GetHeight() ) );
    pImageRest   = pLayerImage->GetTileImage( 0, 0, WORD( pLayerImage->GetWidth() - 1 ), WORD( pLayerImage->GetHeight() ) );

    GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( iFrame ) );
    GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( iFrame ) );
    GR::Graphic::ContextDescriptor   cdImage( pLayerImage, GetPalette( iFrame ) );

    cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 1, 0, &cdImage );
    cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), 0, 0, &cdImage );

    delete pImageSpalte;
    delete pImageRest;

    GR::Graphic::Image*    pLayerImageMask = GetMask( iFrame, i );

    if ( pLayerImageMask )
    {
      AddUndoRect( 0, 0, pLayerImage->GetWidth() - 1, pLayerImage->GetHeight() - 1, iFrame, i, true, FALSE );

      pImageSpalte = pLayerImageMask->GetTileImage( WORD( pLayerImage->GetWidth() - 1 ), 0, 1, WORD( pLayerImage->GetHeight() ) );
      pImageRest   = pLayerImageMask->GetTileImage( 0, 0, WORD( pLayerImage->GetWidth() - 1 ), WORD( pLayerImage->GetHeight() ) );

      GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( iFrame ) );
      GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( iFrame ) );
      GR::Graphic::ContextDescriptor   cdImage( pLayerImageMask, GetPalette( iFrame ) );

      cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 1, 0, &cdImage );
      cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), 0, 0, &cdImage );

      delete pImageSpalte;
      delete pImageRest;
    }
  }
}



void DocumentInfo::ShiftFrameUp( size_t iFrame )
{
  GR::Graphic::Image*     pImageSpalte;
  GR::Graphic::Image*     pImageRest;

  for ( size_t i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
  {
    GR::Graphic::Image*    pLayerImage = GetImage( iFrame, i );

    AddUndoRect( 0, 0, pLayerImage->GetWidth() - 1, pLayerImage->GetHeight() - 1, iFrame, i, false, FALSE );

    pImageSpalte = pLayerImage->GetTileImage( 0, 0, WORD( pLayerImage->GetWidth() ), 1 );
    pImageRest   = pLayerImage->GetTileImage( 0, 1, WORD( pLayerImage->GetWidth() ), WORD( pLayerImage->GetHeight() - 1 ) );

    GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( iFrame ) );
    GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( iFrame ) );
    GR::Graphic::ContextDescriptor   cdImage( pLayerImage, GetPalette( iFrame ) );

    cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 0, 0, &cdImage );
    cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), 0, pLayerImage->GetHeight() - 1, &cdImage );

    delete pImageSpalte;
    delete pImageRest;

    GR::Graphic::Image*    pLayerImageMask = GetMask( iFrame, i );

    if ( pLayerImageMask )
    {
      AddUndoRect( 0, 0, pLayerImage->GetWidth() - 1, pLayerImage->GetHeight() - 1, iFrame, i, true, FALSE );

      pImageSpalte = pLayerImageMask->GetTileImage( 0, 0, WORD( pLayerImage->GetWidth() ), 1 );
      pImageRest   = pLayerImageMask->GetTileImage( 0, 1, WORD( pLayerImage->GetWidth() ), WORD( pLayerImage->GetHeight() - 1 ) );

      GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( iFrame ) );
      GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( iFrame ) );
      GR::Graphic::ContextDescriptor   cdImage( pLayerImageMask, GetPalette( iFrame ) );

      cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 0, 0, &cdImage );
      cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), 0, pLayerImage->GetHeight() - 1, &cdImage );

      delete pImageSpalte;
      delete pImageRest;
    }
  }
}



void DocumentInfo::ShiftFrameDown( size_t iFrame )
{
  GR::Graphic::Image*     pImageSpalte;
  GR::Graphic::Image*     pImageRest;

  for ( size_t i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
  {
    GR::Graphic::Image*    pLayerImage = GetImage( iFrame, i );

    AddUndoRect( 0, 0, pLayerImage->GetWidth() - 1, pLayerImage->GetHeight() - 1, iFrame, i, false, FALSE );

    pImageSpalte = pLayerImage->GetTileImage( 0, WORD( pLayerImage->GetHeight() - 1 ), WORD( pLayerImage->GetWidth() ), 1 );
    pImageRest   = pLayerImage->GetTileImage( 0, 0, WORD( pLayerImage->GetWidth() ), WORD( pLayerImage->GetHeight() - 1 ) );

    GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( iFrame ) );
    GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( iFrame ) );
    GR::Graphic::ContextDescriptor   cdImage( pLayerImage, GetPalette( iFrame ) );

    cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 0, 1, &cdImage );
    cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), 0, 0, &cdImage );

    delete pImageSpalte;
    delete pImageRest;

    GR::Graphic::Image*    pLayerImageMask = GetMask( iFrame, i );

    if ( pLayerImageMask )
    {
      AddUndoRect( 0, 0, pLayerImage->GetWidth() - 1, pLayerImage->GetHeight() - 1, iFrame, i, true, FALSE );

      pImageSpalte = pLayerImageMask->GetTileImage( 0, WORD( pLayerImage->GetHeight() - 1 ), WORD( pLayerImage->GetWidth() ), 1 );
      pImageRest   = pLayerImageMask->GetTileImage( 0, 0, WORD( pLayerImage->GetWidth() ), WORD( pLayerImage->GetHeight() - 1 ) );

      GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( iFrame ) );
      GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( iFrame ) );
      GR::Graphic::ContextDescriptor   cdImage( pLayerImageMask, GetPalette( iFrame ) );

      cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 0, 1, &cdImage );
      cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), 0, 0, &cdImage );

      delete pImageSpalte;
      delete pImageRest;
    }
  }
}



void DocumentInfo::Shift( int iDX, int iDY, size_t CurrentFrame )
{
  BYTE*   pTemp = NULL;

  if ( ( iDX )
  &&   ( m_DocWidth <= 1 ) )
  {
    iDX = 0;
  }
  if ( ( iDY )
  &&   ( m_DocHeight <= 1 ) )
  {
    iDY = 0;
  }

  if ( ( iDX == 0 )
  &&   ( iDY == 0 ) )
  {
    return;
  }

  m_UndoManager.StartUndoGroup();
  
  while ( iDX < 0 )
  {
    GR::Graphic::Image*     pImageSpalte;
    GR::Graphic::Image*     pImageRest;

    if ( CurrentFrame != (size_t)-1 )
    {
      ShiftFrameLeft( CurrentFrame );
    }
    else
    {
      for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); ++iFrame )
      {
        ShiftFrameLeft( iFrame );
      }
    }

    pImageSpalte = m_pImageSelection->GetTileImage( 0, 0, 1, WORD( m_DocHeight ) );
    pImageRest   = m_pImageSelection->GetTileImage( 1, 0, WORD( m_DocWidth - 1 ), WORD( m_DocHeight ) );

    GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( 0 ) );
    GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( 0 ) );
    GR::Graphic::ContextDescriptor   cdImage( m_pImageSelection, GetPalette( 0 ) );

    cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 0, 0, &cdImage );
    cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), m_DocWidth - 1, 0, &cdImage );

    delete pImageSpalte;
    delete pImageRest;

    iDX++;
  }

  while ( iDX > 0 )
  {
    GR::Graphic::Image*     pImageSpalte;
    GR::Graphic::Image*     pImageRest;

    if ( CurrentFrame != (size_t)-1 )
    {
      ShiftFrameRight( CurrentFrame );
    }
    else
    {
      for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); ++iFrame )
      {
        ShiftFrameRight( iFrame );
      }
    }

    pImageSpalte = m_pImageSelection->GetTileImage( WORD( m_DocWidth - 1 ), 0, 1, WORD( m_DocHeight ) );
    pImageRest   = m_pImageSelection->GetTileImage( 0, 0, WORD( m_DocWidth - 1 ), WORD( m_DocHeight ) );

    GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( 0 ) );
    GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( 0 ) );
    GR::Graphic::ContextDescriptor   cdImage( m_pImageSelection, GetPalette( 0 ) );

    cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 1, 0, &cdImage );
    cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), 0, 0, &cdImage );

    delete pImageSpalte;
    delete pImageRest;

    iDX--;
  }

  while ( iDY < 0 )
  {
    GR::Graphic::Image*     pImageSpalte;
    GR::Graphic::Image*     pImageRest;

    if ( CurrentFrame != (size_t)-1 )
    {
      ShiftFrameUp( CurrentFrame );
    }
    else
    {
      for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); ++iFrame )
      {
        ShiftFrameUp( iFrame );
      }
    }

    pImageSpalte = m_pImageSelection->GetTileImage( 0, 0, WORD( m_DocWidth ), 1 );
    pImageRest   = m_pImageSelection->GetTileImage( 0, 1, WORD( m_DocWidth ), WORD( m_DocHeight - 1 ) );

    GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( 0 ) );
    GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( 0 ) );
    GR::Graphic::ContextDescriptor   cdImage( m_pImageSelection, GetPalette( 0 ) );

    cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 0, 0, &cdImage );
    cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), 0, m_DocHeight - 1, &cdImage );

    delete pImageSpalte;
    delete pImageRest;

    iDY++;
  }

  while ( iDY > 0 )
  {
    GR::Graphic::Image*     pImageSpalte;
    GR::Graphic::Image*     pImageRest;

    if ( CurrentFrame != (size_t)-1 )
    {
      ShiftFrameDown( CurrentFrame );
    }
    else
    {
      for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); ++iFrame )
      {
        ShiftFrameDown( iFrame );
      }
    }

    pImageSpalte = m_pImageSelection->GetTileImage( 0, WORD( m_DocHeight - 1 ), WORD( m_DocWidth ), 1 );
    pImageRest   = m_pImageSelection->GetTileImage( 0, 0, WORD( m_DocWidth ), WORD( m_DocHeight - 1 ) );

    GR::Graphic::ContextDescriptor   cdRest( pImageRest, GetPalette( 0 ) );
    GR::Graphic::ContextDescriptor   cdSpalte( pImageSpalte, GetPalette( 0 ) );
    GR::Graphic::ContextDescriptor   cdImage( m_pImageSelection, GetPalette( 0 ) );

    cdRest.CopyArea( 0, 0, cdRest.Width(), cdRest.Height(), 0, 1, &cdImage );
    cdSpalte.CopyArea( 0, 0, cdSpalte.Width(), cdSpalte.Height(), 0, 0, &cdImage );

    delete pImageSpalte;
    delete pImageRest;

    iDY--;
  }

  UpdateMarchingAnts();
  RedrawAllViews();

}



void DocumentInfo::FillListWithAllImages( tImageList& listImages, bool bFillMasks )
{

  switch ( m_DocType )
  {
    case DT_FONT:
    case DT_IMAGE:
      {
        for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); ++iFrame )
        {
          for ( size_t i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
          {
            listImages.push_back( GetImage( iFrame, i ) );
            if ( bFillMasks )
            {
              if ( GetMask( iFrame, i ) )
              {
                listImages.push_back( GetMask( iFrame, i ) );
              }
            }
          }
        }
      }
      break;
  }

}



void DocumentInfo::ReplaceColor( DWORD dwColor1, DWORD dwColor2 )
{
  AddUndoRectAll();

  tImageList    listImages;

  GR::Graphic::Image*   pMask = GetSelectionMask();

  FillListWithAllImages( listImages );

  CGFXUtil::ReplaceColor( listImages, dwColor1, dwColor2, pMask );

  delete pMask;

  RedrawAllViews();

  SetModify( TRUE );
}



void DocumentInfo::SwapColor( GR::u32 Color1, GR::u32 Color2 )
{
  AddUndoRectAll();

  tImageList    listImages;

  GR::Graphic::Image*   pMask = GetSelectionMask();

  FillListWithAllImages( listImages );

  CGFXUtil::SwapColor( listImages, Color1, Color2, pMask );

  delete pMask;

  RedrawAllViews();

  SetModify( TRUE );
}



size_t DocumentInfo::AddFrame( size_t iFrameIndex )
{
  if ( iFrameIndex == -1 )
  {
    iFrameIndex = m_LayeredFrames.size();
  }
  if ( m_LayeredFrames.size() < size_t( iFrameIndex ) )
  {
    dh::Log( "AddFrame: Invalid Frame Index\n" );
    return NULL;
  }

  if ( iFrameIndex == m_LayeredFrames.size() )
  {
    // ans Ende h‰ngen
    m_LayeredFrames.push_back( Frame() );
    return m_LayeredFrames.size() - 1;
  }

  m_LayeredFrames.insert( m_LayeredFrames.begin() + iFrameIndex, Frame() );

  return iFrameIndex;
}



size_t DocumentInfo::CurrentFrame()
{
  return m_CurrentFrame;
}



void DocumentInfo::CurrentFrame( size_t dwFrameNr )
{
  if ( m_LayeredFrames.empty() )
  {
    return;
  }
  if ( dwFrameNr >= m_LayeredFrames.size() )
  {
    dwFrameNr = 0;
  }
  m_CurrentFrame = dwFrameNr;

  pSettings->Notify( NF_LAYER_CHANGED );
}



void DocumentInfo::RemoveFrame( size_t dwFrameNr )
{

  if ( dwFrameNr >= m_LayeredFrames.size() )
  {
    return;
  }
  if ( m_LayeredFrames.size() == 1 )
  {
    AfxMessageBox( _T( "Es muﬂ mindestens ein Frame vorhanden sein!" ) );
    return;
  }
  tVectLayers&    vectm_vectLayers = m_LayeredFrames[dwFrameNr].Layers;

  tVectLayers::iterator   itLayer( vectm_vectLayers.begin() );
  while ( itLayer != vectm_vectLayers.end() )
  {
    CLayer*   pLayer = *itLayer;

    delete pLayer;

    ++itLayer;
  }
  m_LayeredFrames.erase( m_LayeredFrames.begin() + dwFrameNr );

}



GR::u32 DocumentInfo::CountUsedColors()
{
  std::set<DWORD>     setColors;

  for ( size_t iFrame = 0; iFrame < m_LayeredFrames.size(); ++iFrame )
  {
    for ( size_t i = 0; i < m_LayeredFrames[iFrame].LayerCount(); i++ )
    {
      GR::Graphic::Image*    pImage = GetImage( iFrame, i );

      GR::Graphic::ContextDescriptor    cdImage( pImage, GetPalette( iFrame ) );

      for ( int iY = 0; iY < cdImage.Height(); ++iY )
      {
        for ( int iX = 0; iX < cdImage.Width(); ++iX )
        {
          setColors.insert( cdImage.GetPixel( iX, iY ) );
        }
      }
    }
  }

  return (GR::u32)setColors.size();
}



CMaskedContextDescriptor DocumentInfo::GetCD()
{
  return CMaskedContextDescriptor( GetImage( CurrentFrame(), CurrentLayer() ), GetPalette( CurrentFrame() ) );
}



GR::u32 DocumentInfo::ToLocalColor( CSettings::ColorCategory Color )
{
  if ( GetTrueBitDepth() <= 8 )
  {
    return pSettings->GetRGBColor( (CSettings::ColorCategory)( (int)Color + 2 ) );
  }

  return pSettings->GetRGBColor( Color );
}



bool DocumentInfo::HasSelection() const
{
  return m_HasSelection;
}



void DocumentInfo::HasSelection( bool bSetSelection )
{
  m_HasSelection = bSetSelection;
}



void DocumentInfo::SetSelection( GR::Graphic::Image* pImage )
{
  SafeDelete( m_pImageSelection );

  m_pImageSelection = pImage;

  UpdateMarchingAnts();
}



void DocumentInfo::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember )
{
  switch ( NotifyMessage )
  {
    case NF_FUNCTION_CHANGED:
    case NF_BRUSH_MODE_CHANGED:
      {
        GR::Graphic::ContextDescriptor    cdFXMask( m_pImageEffectMask, &GR::Graphic::Palette::AlphaPalette() );

        cdFXMask.Box( 0, 0, cdFXMask.Width(), cdFXMask.Height(), 0 );
      }
      break;
  }
}



GR::Graphic::Image* DocumentInfo::GetImage( size_t iFrame, size_t iLayer )
{

  if ( iFrame >= m_LayeredFrames.size() )
  {
    return NULL;
  }
  if ( iLayer >= m_LayeredFrames[iFrame].LayerCount() )
  {
    return NULL;
  }
  return m_LayeredFrames[iFrame].Layers[iLayer]->GetImage();

}



GR::Graphic::Image* DocumentInfo::GetMask( size_t iFrame, size_t iLayer )
{

  if ( iFrame >= m_LayeredFrames.size() )
  {
    return NULL;
  }
  if ( iLayer >= m_LayeredFrames[iFrame].LayerCount() )
  {
    return NULL;
  }
  return m_LayeredFrames[iFrame].Layers[iLayer]->GetMask();

}



CLayer* DocumentInfo::GetLayer( size_t iFrame, size_t iLayer )
{

  if ( iFrame >= m_LayeredFrames.size() )
  {
    return NULL;
  }
  if ( iLayer >= m_LayeredFrames[iFrame].LayerCount() )
  {
    return NULL;
  }
  return m_LayeredFrames[iFrame].Layers[iLayer];

}



GR::Graphic::Palette* DocumentInfo::GetPalette( size_t iFrame )
{
  if ( iFrame >= m_LayeredFrames.size() )
  {
    return NULL;
  }
  return &m_LayeredFrames[iFrame].Palette;
}



Frame* DocumentInfo::GetFrame( size_t iFrameNr )
{
  if ( iFrameNr >= m_LayeredFrames.size() )
  {
    return NULL;
  }
  return &m_LayeredFrames[iFrameNr];
}



DWORD DocumentInfo::Width() const
{

  return m_DocWidth;

}



DWORD DocumentInfo::Height() const
{

  return m_DocHeight;

}



void DocumentInfo::InvertColor()
{
  AddUndoRectAll();
  for ( size_t i = 0; i < m_LayeredFrames.size(); ++i )
  {
    for ( size_t j = 0; j < m_LayeredFrames[i].Layers.size(); ++j )
    {
      GR::Graphic::Image*   pImg = GetImage( i, j );
      if ( pImg )
      {
        GR::Graphic::ContextDescriptor    cdImg( pImg, &m_LayeredFrames[i].Palette );

        for ( int x = 0; x < cdImg.Width(); ++x )
        {
          for ( int y = 0; y < cdImg.Height(); ++y )
          {
            GR::u32     dwPixel = cdImg.GetPixel( x, y );

            dwPixel = ( ( 255 - ( ( dwPixel & 0xff0000 ) >> 16 ) ) << 16 )
                    + ( ( 255 - ( ( dwPixel & 0x00ff00 ) >> 8 ) ) << 8 )
                    + ( 255 - ( dwPixel & 0xff ) )
                    + ( dwPixel & 0xff000000 );

            cdImg.PutPixel( x, y, dwPixel );
          }
        }
      }
    }
  }
  RedrawAllViews();
}



ByteBuffer DocumentInfo::SaveTempData()
{
  GR::IO::FileChunk     tempDocData( 0x0010 );
  tempDocData.AppendI32( m_DocType );
  tempDocData.AppendI32( m_SaveType );

  GR::IO::FileChunk     tempDocDataInfo( 0x0011 );
  tempDocDataInfo.AppendString( m_FileName );
  tempDocData.AppendBuffer( tempDocDataInfo.ToBuffer() );

  GR::IO::FileChunk     docFrame( 0x0020 );

  ByteBuffer        outBuffer;

  MemoryStream      memOut( outBuffer, IIOStream::OT_WRITE_ONLY );

  if ( !SavePNT( *this, memOut ) )
  {
    return ByteBuffer();
  }
  docFrame.AppendBuffer( outBuffer );
  tempDocData.AppendBuffer( docFrame.ToBuffer() );

  return tempDocData.ToBuffer();
}



bool DocumentInfo::RestoreFromTempData( IIOStream& IOIn )
{
  GR::IO::FileChunk     chunk;

  while ( chunk.Read( IOIn ) )
  {
    MemoryStream   chunkReader( chunk.GetMemoryStream() );
    switch ( chunk.Type() )
    {
      case 0x0011:
        m_FileName = chunkReader.ReadString();

        m_pDoc->SetTitle( GR::Convert::ToUTF16( Path::FileName( m_FileName ) ).c_str() );
        break;
      case 0x0020:
        if ( !LoadPNT( *this, chunkReader ) )
        {
          return false;
        }
        break;
    }
  }
  SetModify( TRUE );
  return true;
}


