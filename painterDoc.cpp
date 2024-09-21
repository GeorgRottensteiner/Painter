#include "stdafx.h"
#include "Painter.h"
#include "Mainfrm.h"
#include "Settings.h"

#include "PainterLua.h"

#include "FormatManager.h"

#include "Import.h"

#include "PainterDoc.h"
#include "painterview.h"
#include "NewDocumentDialog.h"
#include "DlgSaveIGF.h"
#include "DlgResizeCanvas.h"
#include "PainterGfxTools.h"
#include "FormatPNT.h"

#include "JPEGSaveOptions.h"
#include "DlgWebPOptions.h"

#include "ImagePackage.h"

#include "FreeImage.h"

#include <Debug\debugclient.h>

#include <Grafik/Image.h>

#include <Grafik\ContextDescriptor.h>

#include <Grafik\ImageFormate\FormatBMP.h>
#include <Grafik\ImageFormate\FormatIGF.h>
#include <Grafik\ImageFormate\FormatGIF.h>
#include <Grafik\ImageFormate\FormatGRI.h>
#include <Grafik\ImageFormate\FormatJPG.h>
#include <Grafik\ImageFormate\FormatIcon.h>
#include <Grafik\ImageFormate\FormatCur.h>
#include <Grafik\ImageFormate\FormatTGA.h>
#include <Grafik\ImageFormate\FormatPCX.h>
#include <Grafik\ImageFormate\FormatBTN.h>
#include <Grafik\ImageFormate\FormatPNG.h>

#include <String/Path.h>
#include <String/StringUtil.h>

//#include <utility.h>
#undef OS_WINDOWS

#include <shlwapi.h>
#include ".\painterdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CPainterApp  theApp;

extern CSettings    theSettings;


IMPLEMENT_DYNCREATE(CPainterDoc, CDocument)

BEGIN_MESSAGE_MAP(CPainterDoc, CDocument)
	//{{AFX_MSG_MAP(CPainterDoc)
	ON_COMMAND(ID_CONVERT_TO_16BPP, OnConvertTo16bpp)
	ON_COMMAND(ID_CONVERT_TO_24BPP, OnConvertTo24bpp)
	ON_COMMAND(ID_CONVERT_TO_32BPP, OnConvertTo32bpp)
	ON_COMMAND(ID_BEARBEITEN_MIRRORH, OnBearbeitenMirrorH)
	ON_COMMAND(ID_BEARBEITEN_MIRRORV, OnBearbeitenMirrorV)
	ON_COMMAND(ID_BEARBEITEN_ROTIEREN, OnBearbeitenRotieren)
	ON_COMMAND(ID_BEARBEITEN_LOESCHEN, OnBearbeitenLoeschen)
	ON_COMMAND(ID_CONVERT_TO_8BPP, OnConvertTo8bpp)
	ON_COMMAND(ID_BEARBEITEN_RESIZE_CANVAS, OnBearbeitenResizeCanvas)
	ON_COMMAND(ID_BEARBEITEN_RESIZE, OnBearbeitenResize)
	//}}AFX_MSG_MAP
  ON_COMMAND(ID_SPEZIAL_AUF4BITKONVERTIEREN, OnSpezialAuf4bitkonvertieren)
  ON_COMMAND(ID_SPEZIAL_AUF1BITKONVERTIEREN, OnSpezialAuf1bitkonvertieren)
  ON_COMMAND(ID_EXTRAS_2XSAI, OnExtras2xsai)
  ON_COMMAND(ID_BEARBEITEN_EXPORTIEREN32909, OnBearbeitenExportieren32909)
  ON_COMMAND(ID_BEARBEITEN_IMPORTIEREN32910, OnBearbeitenImportieren32910)
  ON_COMMAND(ID_BEARBEITEN_RUNSCRIPT32912, OnBearbeitenRunscript)
END_MESSAGE_MAP()



CPainterDoc::CPainterDoc()
{
  diInfo.m_pDoc = this;
  diInfo.m_StartX = 0;
  diInfo.m_StartY = 0;
  diInfo.m_ActX = 0;
  diInfo.m_ActY = 0;

  diInfo.m_SelectionX              = 0;
  diInfo.m_SelectionY              = 0;
  diInfo.m_SelectionWidth          = 0;
  diInfo.m_SelectionHeight         = 0;

  diInfo.m_OptionFrameSpeed = 50;
}



CPainterDoc::~CPainterDoc()
{

}



BOOL CPainterDoc::OnCreateDocumentFromClipboard()
{
  SetTitle( _T( "New Document" ) );

  POSITION              pos = GetFirstViewPosition();
  CView*                pFirstView = GetNextView( pos );

  if ( !OpenClipboard( NULL ) )
  {
    return FALSE;
  }
    
	if ( !CDocument::OnNewDocument() )
  {
		return FALSE;
  }

  theApp.pMainFrame->MDIActivate( pFirstView );

  if ( !diInfo.PasteFromClipBoard( 1, &( (CPainterView*)pFirstView )->m_viewInfo ) )
  {
    CloseClipboard();
    return FALSE;
  }

  CloseClipboard();

  // und das ganze neu zeichnen
  pFirstView->Invalidate( FALSE );
  pFirstView->GetParentFrame()->Invalidate( FALSE );

	return TRUE;
}



BOOL SaveBTH( GR::Graphic::Image *pImage, const GR::Char* File )
{
  if ( pImage == NULL )
  {
    return FALSE;
  }
  if ( ( pImage->GetDepth() != 15 )
  &&   ( pImage->GetDepth() != 16 ) )
  {
    return FALSE;
  }

  GR::IO::FileStream    ioOut;

  if ( !ioOut.Open( File, IIOStream::OT_WRITE_ONLY ) )
  {
    MessageBoxA( NULL, "Konnte Datei nicht zum Schreiben ˆffnen.", "Fehler", MB_OK | MB_APPLMODAL );
    return FALSE;
  }

  ioOut.WriteU16( (WORD)pImage->GetWidth() );
  ioOut.WriteU16( (WORD)pImage->GetHeight() );

  ioOut.WriteBlock( pImage->GetData(), 2 * ( pImage->GetWidth() * pImage->GetHeight() ) );
  ioOut.Close();

  return TRUE;
}



BOOL CPainterDoc::SaveModified()
{
  if ( theApp.pMainFrame->m_SessionEnding )
  {
    return TRUE;
  }
  return CDocument::SaveModified();
}



BOOL CPainterDoc::OnSaveDocument( LPCTSTR lpszPathName ) 
{
  GR::String      fileName = GR::Convert::ToUTF8( lpszPathName );


  POSITION pos = GetFirstViewPosition();
  CPainterView* pView = (CPainterView*)GetNextView( pos );

  if ( diInfo.m_SaveType == SAVETYPE_UNKNOWN )
  {
    AfxMessageBox( _T( "Unknown file type!" ) );
    return FALSE;
  }
  // das "*" bei ver‰nderten Dokumenten entfernen
  if ( GR::Strings::EndsWith( fileName, " *" ) )
  {
    fileName = fileName.substr( 0, fileName.length() - 2 );
  }
  GR::String    extension = Path::Extension( fileName );

  if ( extension.empty() )
  {
    // keine Extension dran, eine vom Typ abh‰ngige Extension anh‰ngen
    if ( diInfo.m_SaveType == SAVETYPE_IGF )
    {
      fileName += ".igf";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_BTH )
    {
      fileName += ".bth";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_FNH )
    {
      fileName += ".fnh";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_FNXL )
    {
      fileName += ".fnl";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_BTN )
    {
      fileName += ".btn";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_FNX )
    {
      fileName += ".fnx";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_PCX )
    {
      fileName += ".pcx";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_GIF )
    {
      fileName += ".gif";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_IFF )
    {
      fileName += ".iff";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_JPEG )
    {
      fileName += ".jpg";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_PNG )
    {
      fileName += ".png";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_WEBP )
    {
      fileName += ".webp";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_ANX )
    {
      fileName += ".anx";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_ANH )
    {
      fileName += ".anh";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_PNT )
    {
      fileName += ".pnt";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_BMP )
    {
      fileName += ".bmp";
    }
  }

  if ( ( diInfo.Width() == 0 )
  ||   ( diInfo.Height() == 0 ) )
  {
    MessageBoxA( AfxGetMainWnd()->GetSafeHwnd(), "Das ist ¸¸¸¸bel!", "aha", 0 );
    diInfo.SetSize( 10, 10 );
  }
  else if ( CFormatManager::Instance().SupportsFormat( diInfo.m_SaveType ) )
  {
    // Dokument in ImageSet umwandeln
    ImageSet     Set;

    for ( size_t i = 0; i < diInfo.m_LayeredFrames.size(); ++i )
    {
      GR::Graphic::ImageData*   pImageData = diInfo.GetImage( i, 0 )->CreateImageData();

      CLayer*       pLayer = diInfo.GetLayer( i, 0 );

      if ( pLayer->m_Transparent )
      {
        pImageData->TransparentColorUsed( true );
        pImageData->TransparentColor( pLayer->m_Transparenz );
      }

      if ( pImageData->Palette().Data() )
      {
        pImageData->SetPalette( *diInfo.GetPalette( i ) );
      }
      GR::Graphic::ImageData*   pImageDataMask = NULL;

      GR::Graphic::Image*   pImageMask = diInfo.GetMask( i, 0 );
      if ( pImageMask )
      {
        pImageDataMask = pImageMask->CreateImageData();
      }

      tImageSetFrame* pFrame = Set.AddFrame( pImageData, pImageDataMask );
      pFrame->DelayMS = diInfo.GetFrame( i )->Delay;
    }
    if ( !CFormatManager::Instance().Save( diInfo.m_SaveType, fileName.c_str(), &Set ) )
    {
      return FALSE;
    }
    diInfo.SetModify( FALSE );
    return TRUE;
  }
  else if ( diInfo.m_SaveType == SAVETYPE_BTH )
  {
    // Die Datei ist wohl ein BTH...
    if ( diInfo.m_BitDepth != 15 )
    {
      AfxMessageBox( _T( "BTHs kˆnnen nur mit 15bit Farbtiefe gespeichert werden." ) );
      return FALSE;
    }
    SaveBTH( diInfo.GetImage( 0, 0 ), fileName.c_str() );
    diInfo.SetModify( FALSE );
  }
  else if ( diInfo.m_SaveType == SAVETYPE_PNG )
  {
    // Die Datei ist wohl ein PNG...
    GR::Graphic::Image*     pImg = new GR::Graphic::Image( diInfo.GetImage( 0, 0 ) );
    GR::Graphic::Image*     pMask = diInfo.GetMask( 0, 0 );

    if ( ( pImg->GetDepth() == 32 )
    &&   ( pMask ) )
    {
      // Maske einbringen
      // Alpha in Maske umwandeln
      GR::Graphic::ContextDescriptor    cdImage( pImg );

      for ( int i = 0; i < pMask->GetWidth(); i++ )
      {
        for ( int j = 0; j < pMask->GetHeight(); j++ )
        {
          pImg->SetPixel( i, j, ( pImg->GetPixel( i, j ) & 0x00ffffff ) | ( pMask->GetPixel( i, j ) << 24 ) );
        }
      }
    }

    FIBITMAP* dib = ConvertImageToFreeImageDIB( pImg, pView->m_viewInfo.m_pPalette );

    delete pImg;

    if ( ( diInfo.ActiveLayer()->m_Transparent )
    &&   ( diInfo.GetBitDepth() <= 8 ) )
    {
      // hat eine Transparenzfarbe
      BYTE    Transparent[256];
      
      for ( int i = 0; i < diInfo.GetBitDepth(); ++i )
      {
        if ( i == diInfo.ActiveLayer()->m_Transparenz )
        {
          Transparent[i] = 0;
        }
        else
        {
          Transparent[i] = 255;
        }
      }

      FreeImage_SetTransparent( dib, true );
      FreeImage_SetTransparencyTable( dib, Transparent, diInfo.GetBitDepth() );
    }

    if ( ( diInfo.GetBitDepth() == 16 )
    ||   ( diInfo.GetBitDepth() == 15 ) )
    {
      FIBITMAP* pDIB24 = FreeImage_ConvertTo24Bits( dib );

      FreeImage_Unload( dib );
      dib = pDIB24;
    }

    if ( !FreeImage_SaveU( FIF_PNG, dib, GR::Convert::ToUTF16( fileName ).c_str() ) )
    {
      FreeImage_Unload( dib );
      MessageBoxA( NULL, "Failed to save to PNG! (not supported color depth?)", "Error!", 0 );
      return FALSE;
    }
    FreeImage_Unload( dib );
    diInfo.SetModify( FALSE );
  }
  else if ( diInfo.m_SaveType == SAVETYPE_WEBP )
  {
    GR::Graphic::Image* pImg = new GR::Graphic::Image( diInfo.GetImage( 0, 0 ) );
    GR::Graphic::Image* pMask = diInfo.GetMask( 0, 0 );

    if ( ( pImg->GetDepth() == 32 )
    &&   ( pMask ) )
    {
      // Maske einbringen
      // Alpha in Maske umwandeln
      GR::Graphic::ContextDescriptor    cdImage( pImg );

      for ( int i = 0; i < pMask->GetWidth(); i++ )
      {
        for ( int j = 0; j < pMask->GetHeight(); j++ )
        {
          pImg->SetPixel( i, j, ( pImg->GetPixel( i, j ) & 0x00ffffff ) | ( pMask->GetPixel( i, j ) << 24 ) );
        }
      }
    }

    FIBITMAP* dib = ConvertImageToFreeImageDIB( pImg, pView->m_viewInfo.m_pPalette );

    delete pImg;

    if ( ( diInfo.ActiveLayer()->m_Transparent )
    &&   ( diInfo.GetBitDepth() <= 8 ) )
    {
      // hat eine Transparenzfarbe
      BYTE    Transparent[256];

      for ( int i = 0; i < diInfo.GetBitDepth(); ++i )
      {
        if ( i == diInfo.ActiveLayer()->m_Transparenz )
        {
          Transparent[i] = 0;
        }
        else
        {
          Transparent[i] = 255;
        }
      }

      FreeImage_SetTransparent( dib, true );
      FreeImage_SetTransparencyTable( dib, Transparent, diInfo.GetBitDepth() );
    }

    if ( ( diInfo.GetBitDepth() == 16 )
    ||   ( diInfo.GetBitDepth() == 15 ) )
    {
      FIBITMAP* pDIB24 = FreeImage_ConvertTo24Bits( dib );

      FreeImage_Unload( dib );
      dib = pDIB24;
    }

    DlgWebPOptions  dlgOptions;

    if ( dlgOptions.DoModal() != IDOK )
    {
      return FALSE;
    }

    if ( !FreeImage_SaveU( FIF_WEBP, dib, GR::Convert::ToUTF16( fileName ).c_str(), dlgOptions.m_SaveOption ) )
    {
      FreeImage_Unload( dib );
      MessageBoxA( NULL, "Failed to save to WEBP! (not supported color depth?)", "Error!", 0 );
      return FALSE;
    }
    FreeImage_Unload( dib );
    diInfo.SetModify( FALSE );
  }
  else if ( diInfo.m_SaveType == SAVETYPE_PNT )
  {
    // das neue, supergeile Painter-Format (mit m_vectLayers in sisch drinnen)
    SavePNT( diInfo, fileName );
    diInfo.SetModify( FALSE );
  }
  else if ( diInfo.m_SaveType == SAVETYPE_ICON )
  {
    GR::Graphic::ContextDescriptor    cdImage( diInfo.GetImage( 0, 0 ), pView->m_viewInfo.m_pPalette );
    GR::Graphic::ContextDescriptor    cdMask( diInfo.GetMask( 0, 0 ) );

    if ( GR::Graphic::s_GlobalIconPlugin.CanSave( &cdImage ) )
    {
      GR::Graphic::s_GlobalIconPlugin.Save( fileName.c_str(), &cdImage, &cdMask );
      diInfo.SetModify( FALSE );
    }
    else
    {
      AfxMessageBox( _T( "Unsupported image format for icons" ) );
      return FALSE;
    }
  }
  else
  {
    // Die Datei hat wohl ein unbekanntes Format...
    AfxMessageBox( _T( "Unknown file format" ) );
    return FALSE;
  }
  
	return TRUE;
}



CPainterImagePackage* FreeImageLoad( const GR::Char* FileName )
{
  CPainterImagePackage   *pImagePackage = new CPainterImagePackage();

  FIBITMAP        *dib = NULL;

  GR::WString   utf16Filename = GR::Convert::ToUTF16( FileName );


  switch ( FreeImage_GetFileTypeU( utf16Filename.c_str() ) )
  {
		case FIF_BMP :
      dib = FreeImage_LoadU( FIF_BMP, utf16Filename.c_str() );
			break;
		case FIF_ICO :
      dib = FreeImage_LoadU( FIF_ICO, utf16Filename.c_str() );
			break;
		case FIF_JPEG :
      dib = FreeImage_LoadU( FIF_JPEG, utf16Filename.c_str() );
			break;
		case FIF_KOALA :
      dib = FreeImage_LoadU( FIF_KOALA, utf16Filename.c_str() );
			break;
		case FIF_PCD :
      dib = FreeImage_LoadU( FIF_PCD, utf16Filename.c_str() );
			break;
		case FIF_PCX :
      dib = FreeImage_LoadU( FIF_PCX, utf16Filename.c_str() );
			break;
		case FIF_PNG :
      dib = FreeImage_LoadU( FIF_PNG, utf16Filename.c_str() );
			break;
    case FIF_WEBP:
      dib = FreeImage_LoadU( FIF_WEBP, utf16Filename.c_str() );
      if ( dib != NULL )
      {
        // always upside down?
        FreeImage_FlipVertical( dib );
      }
      break;
    case FIF_PBM :
      dib = FreeImage_LoadU( FIF_PBM, utf16Filename.c_str() );
			break;
		case FIF_PGM :
      dib = FreeImage_LoadU( FIF_PGM, utf16Filename.c_str() );
			break;
		case FIF_PPM :
      dib = FreeImage_LoadU( FIF_PPM, utf16Filename.c_str() );
			break;
		case FIF_RAS :
      dib = FreeImage_LoadU( FIF_RAS, utf16Filename.c_str() );
			break;
		case FIF_TARGA :
      dib = FreeImage_LoadU( FIF_TARGA, utf16Filename.c_str() );
			break;
		case FIF_TIFF :
      dib = FreeImage_LoadU( FIF_TIFF, utf16Filename.c_str() );
			break;
	}

	if ( dib == NULL ) 
  {
    return pImagePackage;
  }

  switch ( FreeImage_GetBPP( dib ) )
  {
    case 4:
      {
        FIBITMAP        *newdib = FreeImage_ConvertTo8Bits( dib );
        FreeImage_Unload( dib );
        dib = newdib;

        pImagePackage->m_Palette = theApp.m_PainterPalette;
        pImagePackage->m_pImage = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 8 );
        for ( DWORD i = 0; i < FreeImage_GetHeight( dib ); i++ )
        {
          memcpy( ( (BYTE*)pImagePackage->m_pImage->GetData() ) + i * pImagePackage->m_pImage->GetWidth(),
                  FreeImage_GetScanLine( dib, i ),
                  pImagePackage->m_pImage->GetWidth() );
        }
        BITMAPINFOHEADER    *pbmHeader = FreeImage_GetInfoHeader( dib );
        RGBQUAD             *pRGB = FreeImage_GetPalette( dib );
        for ( int i = 0; i < (int)pbmHeader->biClrUsed; i++ )
        {
          pImagePackage->m_Palette.SetColor( i, pRGB->rgbRed, pRGB->rgbGreen, pRGB->rgbBlue );
          pRGB++;
        }
      }
      break;
    case 8:
      {
        pImagePackage->m_Palette.Create();
        pImagePackage->m_pImage = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 8 );
        for ( DWORD i = 0; i < FreeImage_GetHeight( dib ); i++ )
        {
          memcpy( ( (BYTE*)pImagePackage->m_pImage->GetData() ) + i * pImagePackage->m_pImage->GetWidth(),
                  FreeImage_GetScanLine( dib, i ),
                  pImagePackage->m_pImage->GetWidth() );
        }
        BITMAPINFOHEADER    *pbmHeader = FreeImage_GetInfoHeader( dib );
        RGBQUAD             *pRGB = FreeImage_GetPalette( dib );
        for ( int i = 0; i < (int)pbmHeader->biClrUsed; i++ )
        {
          pImagePackage->m_Palette.SetColor( i, pRGB->rgbRed, pRGB->rgbGreen, pRGB->rgbBlue );
          pRGB++;
        }
      }
      break;
    case 24:
      {
        pImagePackage->m_pImage = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 24 );
        for ( DWORD i = 0; i < FreeImage_GetHeight( dib ); i++ )
        {
          memcpy( ( (BYTE*)pImagePackage->m_pImage->GetData() ) + i * pImagePackage->m_pImage->GetWidth() * 3,
                  FreeImage_GetScanLine( dib, i ),
                  pImagePackage->m_pImage->GetWidth() * 3 );
        }
      }
      break;
    case 32:
      {
        pImagePackage->m_pImage = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 32 );
        for ( DWORD i = 0; i < FreeImage_GetHeight( dib ); i++ )
        {
          memcpy( ( (BYTE*)pImagePackage->m_pImage->GetData() ) + i * pImagePackage->m_pImage->GetWidth() * 4,
                  FreeImage_GetScanLine( dib, i ),
                  pImagePackage->m_pImage->GetWidth() * 4 );
        }

        if ( FreeImage_GetColorType( dib ) == FIC_RGBALPHA )
        {
          // in den oberen 8 Bits wird vermutlich Alpha sein
          // Alpha in Maske umwandeln
          pImagePackage->m_pImageMask = new GR::Graphic::Image( FreeImage_GetWidth( dib ), FreeImage_GetHeight( dib ), 8 );

          for ( unsigned j = 0; j < FreeImage_GetHeight( dib ); j++ )
          {
            DWORD*    pSource = (DWORD*)FreeImage_GetScanLine( dib, j );
            for ( unsigned i = 0; i < FreeImage_GetWidth( dib ); i++ )
            {
              *( ( (BYTE*)pImagePackage->m_pImageMask->GetData() ) + i + j * FreeImage_GetWidth( dib ) ) = (BYTE)( ( *pSource++ & 0xff000000 ) >> 24 );
            }
          }
        }
      }
      break;
    default:
      dh::Log( "unsupported depth %d", FreeImage_GetBPP( dib ) );
      break;
  }


	FreeImage_Unload( dib );

  return pImagePackage;
}



void CPainterDoc::AddNewImage( GR::Graphic::Image* pImage, GR::Graphic::Palette* pPalette, GR::Graphic::Image* pImageMask ) 
{

  POSITION pos = GetFirstViewPosition();
  CView* pView = GetNextView( pos );

  diInfo.SetSize( pImage->GetWidth(), pImage->GetHeight() );
  diInfo.m_DocType = DT_IMAGE;
  diInfo.m_BitDepth = pImage->GetDepth();
  if ( diInfo.m_BitDepth == 15 )
  {
    diInfo.m_BitDepth = 16;
  }
  if ( diInfo.GetPalette( diInfo.CurrentFrame() ) )
  {
    *diInfo.GetPalette( diInfo.CurrentFrame() ) = *pPalette;
  }

  CLayer* pLayer = diInfo.AddLayer( pImage );
  pLayer->SetLayerMask( pImageMask );

  pView->OnInitialUpdate();

}



void CPainterDoc::AddAlphaMask( GR::Graphic::Image *pImage ) 
{

  if ( pImage == NULL )
  {
    return;
  }

  CLayer*   pLayer = diInfo.ActiveLayer();
  if ( pLayer == NULL )
  {
    return;
  }

  pLayer->SetLayerMask( pImage );
  pLayer->m_MaskEnabled = true;

  diInfo.RedrawAllViews();

}



BOOL CPainterDoc::OnOpenDocument( LPCTSTR lpszPathName ) 
{
  GR::String          fileName = GR::Convert::ToUTF8( lpszPathName );

  GR::IO::FileStream* pFile;

  GR::Graphic::Image* pDummyImage;

  POSITION        pos;


  pos = GetFirstViewPosition();
  // KREISCH
  CPainterView *pView = (CPainterView*)GetNextView( pos );

  // globalen Format-Manager benutzen
  ImageSet*  pSet = CFormatManager::Instance().Load( diInfo.m_SaveType, fileName.c_str() );
  if ( pSet )
  {
    ImageSet::tFrames::iterator    it( pSet->Frames.begin() );
    while ( it != pSet->Frames.end() )
    {
      tImageSetFrame* pFrame = *it;

      GR::Graphic::ImageData*   pData = pFrame->Layers[0]->pImageData;

      GR::Graphic::Image*   pImageMask = NULL;

      if ( pData->ImageFormat() == GR::Graphic::IF_A8R8G8B8 )
      {
        // Alpha in Maske umwandeln
        pImageMask = new GR::Graphic::Image( pData->Width(), pData->Height(), 8 );

        for ( int i = 0; i < pData->Width(); i++ )
        {
          for ( int j = 0; j < pData->Height(); j++ )
          {
            DWORD   dwColor = *( ( (DWORD*)pData->Data() ) + i + j * pData->Width() );

            *( ( (BYTE*)pImageMask->GetData() ) + i + j * pData->Width() ) = (BYTE)( ( dwColor & 0xff000000 ) >> 24 );
          }
        }
        pData->ConvertSelfTo( GR::Graphic::IF_R8G8B8 );
      }

      GR::Graphic::Image*    pImage = new GR::Graphic::Image( *pData );

      int       iNewFrame = diInfo.AddFrame();
      diInfo.GetFrame( iNewFrame )->Delay = pFrame->DelayMS;
      CLayer*   pLayer = diInfo.AddLayer( pImage, iNewFrame );
      if ( diInfo.m_BitDepth <= 8 )
      {
        if ( pData->Palette().Data() )
        {
          *diInfo.GetPalette( iNewFrame ) = pData->Palette();
        }
        else
        {
          *diInfo.GetPalette( iNewFrame ) = theApp.m_PainterPalette;
        }
      }

      if ( pImageMask )
      {
        pLayer->SetLayerMask( pImageMask );
      }

      if ( pData->TransparentColorUsed() )
      {
        pLayer->SetTransparencyIndex( true, pData->TransparentColor() );
      }

      if ( pFrame->Layers[0]->pImageDataMask )
      {
        // Alpha in Maske umwandeln
        GR::Graphic::Image*    pImageMask = new GR::Graphic::Image( *pFrame->Layers[0]->pImageDataMask );

        pLayer->SetLayerMask( pImageMask );
      }

      ++it;
    }
    diInfo.SetSize( diInfo.GetImage( 0, 0 )->GetWidth(),
                    diInfo.GetImage( 0, 0 )->GetHeight() );

    diInfo.m_BitDepth = diInfo.GetImage( 0, 0 )->GetDepth();
    diInfo.m_AnimationRunning = FALSE;
    diInfo.m_DocType = DT_IMAGE;

    delete pSet;

    pView->OnInitialUpdate();

    diInfo.m_FileName = fileName;
	  return TRUE;
  }

  // alt
  AbstractImageFileFormat*    pFormat = NULL;

  pSet = ImageFormatManager::Instance().LoadSet( fileName.c_str(), &pFormat );
  if ( pSet )
  {
    ImageSet::tFrames::iterator    it( pSet->Frames.begin() );
    while ( it != pSet->Frames.end() )
    {
      tImageSetFrame* pFrame = *it;

      GR::Graphic::ImageData*   pData = pFrame->Layers[0]->pImageData;

      GR::Graphic::Image*    pImage = new GR::Graphic::Image( *pData );

      int   iFrame = diInfo.AddFrame();
      CLayer*   pLayer = diInfo.AddLayer( pImage, iFrame );

      diInfo.GetFrame( iFrame )->Delay = pFrame->DelayMS;

      if ( diInfo.m_BitDepth <= 8 )
      {
        if ( pData->Palette().Entries() )
        {
          *diInfo.GetPalette( iFrame ) = pData->Palette();
        }
        else
        {
          *diInfo.GetPalette( iFrame ) = theApp.m_PainterPalette;
        }
      }

      if ( pData->TransparentColorUsed() )
      {
        pLayer->SetTransparencyIndex( true, pData->TransparentColor() );
      }

      if ( pData->ImageFormat() == GR::Graphic::IF_A8R8G8B8 )
      {
        // Alpha in Maske umwandeln
        pLayer->SetLayerMask( new GR::Graphic::Image( pImage->GetWidth(), pImage->GetHeight(), 8 ) );

        for ( int i = 0; i < pImage->GetWidth(); i++ )
        {
          for ( int j = 0; j < pImage->GetHeight(); j++ )
          {
            DWORD   dwColor = *( ( (DWORD*)pImage->GetData() ) + i + j * pImage->GetWidth() );

            *( ( (BYTE*)pLayer->GetMask()->GetData() ) + i + j * pImage->GetWidth() ) = (BYTE)( ( dwColor & 0xff000000 ) >> 24 );
          }
        }
      }
      if ( pFrame->Layers[0]->pImageDataMask )
      {
        // Alpha in Maske umwandeln
        GR::Graphic::Image*    pImageMask = new GR::Graphic::Image( *pFrame->Layers[0]->pImageDataMask );

        pLayer->SetLayerMask( pImageMask );
      }

      ++it;
    }
    diInfo.SetSize( diInfo.GetImage( 0, 0 )->GetWidth(),
                    diInfo.GetImage( 0, 0 )->GetHeight() );

    diInfo.m_BitDepth = diInfo.GetImage( 0, 0 )->GetDepth();
    diInfo.m_AnimationRunning = FALSE;
    diInfo.m_DocType = DT_IMAGE;

    delete pSet;

    diInfo.m_SaveType = SAVETYPE_UNKNOWN;

    // TODO( Hier den SaveType direkt als Pointer nehmen? )
    if ( pFormat == &globalBMPPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_BMP;
    }
    else if ( pFormat == &GR::Graphic::s_GlobalIconPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_ICON;
    }
    else if ( pFormat == &globalIGFPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_IGF;
    }
    else if ( pFormat == &globalGRIPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_GRI;
    }
    else if ( pFormat == &globalPCXPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_PCX;
    }
    else if ( pFormat == &globalTGAPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_TGA;
    }
    else if ( pFormat == &globalBTNPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_BTN;
    }
    else if ( pFormat == &globalGIFPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_GIF;
    }
    else if ( pFormat == &globalJPGPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_JPEG;
    }
    else if ( pFormat == &GR::Graphic::s_GlobalPNGPlugin )
    {
      diInfo.m_SaveType = SAVETYPE_PNG;
    }

    pView->OnInitialUpdate();

    //diInfo.dwTimer = SetTimer( diInfo.pView->GetSafeHwnd(), 1, diInfo.dwOptionFrameSpeed, NULL );

    diInfo.m_FileName = fileName;
	  return TRUE;
  }

  GR::String    extension = GR::Strings::ToUpper( Path::Extension( fileName ) );

  if ( extension == "BTN" )
  {
    // Die Datei ist wohl ein BTN...
    diInfo.m_SaveType = SAVETYPE_BTN;

    CLayer* pLayer = diInfo.AddLayer();
    pDummyImage = pLayer->GetImage();
    pDummyImage->LoadBTN( fileName.c_str(), 8 );
    diInfo.SetSize( pDummyImage->GetWidth(), pDummyImage->GetHeight() );
    diInfo.m_DocType = DT_IMAGE;
    diInfo.m_BitDepth = 8;
    *diInfo.GetPalette( diInfo.CurrentFrame() ) = theApp.m_PainterPalette;

    pView->OnInitialUpdate();
  }
  else if ( extension == "PNT" )
  {
    // Die Datei ist wohl ein PNT...
    if ( !LoadPNT( diInfo, fileName ) )
    {
      return FALSE;
    }
  }
  else if ( extension == "PSD" )
  {
    // Die Datei ist wohl ein PSD...
    if ( !LoadPSD( &diInfo, fileName.c_str() ) )
    {
      return FALSE;
    }
  }
  else if ( extension == "SHP" )
  {
    // Ein Uralt-Shape-File, na hurra
    pFile = new GR::IO::FileStream();
    pFile->Open( fileName.c_str() );
    if ( !pFile->IsGood() )
    {
      delete pFile;
      return FALSE;
    }

    int   iSize = 4;

    WORD    wWidth1 = 4; 
    WORD    wHeight1  = 4;


    while ( iSize < 50 )
    {
      pFile->SetPosition( iSize * iSize + 4, IIOStream::PT_SET );

      wWidth1   = pFile->ReadU16();
      wHeight1  = pFile->ReadU16();

      pFile->SetPosition( ( iSize * iSize + 4 ) * 2, IIOStream::PT_SET );

      WORD    wWidth2   = pFile->ReadU16();
      WORD    wHeight2  = pFile->ReadU16();

      if ( ( wWidth1 == wHeight1 )
      &&   ( wWidth2 == wHeight2 )
      &&   ( wWidth1 != 0 )
      &&   ( wWidth1 == wWidth2 ) )
      {
        break;
      }

      ++iSize;
    }

    if ( iSize == 50 )
    {
      delete pFile;
      return FALSE;
    }

    pFile->SetPosition( 8 + wWidth1 * wHeight1, IIOStream::PT_SET );

    diInfo.SetSize( 640, 480 );
    diInfo.m_BitDepth = 8;
    pView->OnInitialUpdate();

    GR::Graphic::Image         *pDummyImage = new GR::Graphic::Image( wWidth1, wHeight1, 8 );

    for ( int i = 0; i < 200; i++ )
    {
      pFile->ReadBlock( (BYTE*)pDummyImage->GetData(), wWidth1 * wHeight1 );
      pFile->ReadU32();
      pDummyImage->PutImage( pView->m_viewInfo.m_pPage, ( i % ( 640 / wWidth1 ) ) * wWidth1, ( i / ( 640 / wWidth1 ) ) * wHeight1, IMAGE_METHOD_PLAIN );
    }
    pFile->Close();
    delete pFile;
    delete pDummyImage;
    pDummyImage = new GR::Graphic::Image( 640, 480, 8 );
    pDummyImage->GetImage( pView->m_viewInfo.m_pPage, 0, 0 );

    

    diInfo.m_SaveType = SAVETYPE_BTN;
    diInfo.AddFrame();
    AddNewImage( pDummyImage, diInfo.GetPalette( diInfo.CurrentFrame() ) );

    *diInfo.GetPalette( diInfo.CurrentFrame() ) = theApp.m_PainterPalette;
  }
  else if ( extension == "IGF" )
  {
    // ein IGF-File -> nachsehen, was es ist
    diInfo.m_SaveType = SAVETYPE_IGF;
    pFile = new GR::IO::FileStream();
    pFile->Open( fileName.c_str() );
    BYTE ucType = pFile->ReadU8();
    pFile->Close();
    if ( ( ucType == GR::Graphic::IGFType::ANIMATION )
    ||   ( ucType == GR::Graphic::IGFType::ANIMATION_EXTENDED ) )
    {
      // Die Datei ist wohl ein ANX...
      GR::Animation   tempAnim;

      tempAnim.Load( fileName.c_str() );

      diInfo.SetSize( tempAnim.GetWidth(), tempAnim.GetHeight() );
      diInfo.m_BitDepth = tempAnim.GetFirstImage()->GetDepth();
      diInfo.m_AnimationRunning = FALSE;
      diInfo.m_DocType = DT_IMAGE;
      if ( diInfo.m_BitDepth <= 8 )
      {
        *diInfo.GetPalette( diInfo.CurrentFrame() ) = theApp.m_PainterPalette;
      }

      for ( DWORD iFrame = 0; iFrame < tempAnim.GetFrames(); ++iFrame )
      {
        diInfo.AddLayer( new GR::Graphic::Image( tempAnim.GetImage( iFrame ) ), diInfo.AddFrame() );
      }

      pView->OnInitialUpdate();
      //diInfo.dwTimer = SetTimer( diInfo.pView->GetSafeHwnd(), 1, diInfo.dwOptionFrameSpeed, NULL );
    }
    delete pFile;
  }
  else if ( extension == "FNX" )
  {
    // ein Font!
    diInfo.m_SaveType = SAVETYPE_FNX;
    diInfo.m_BitDepth = 8;
    diInfo.SetSize( 200, 200 );
    diInfo.m_DocType = DT_FONT;
    *diInfo.GetPalette( diInfo.CurrentFrame() ) = theApp.m_PainterPalette;

    pView->OnInitialUpdate();
    
    GR::Font      Font;

    Font.LoadFNT( fileName.c_str(), 8 );

    for ( int i = 0; i < 256; i++ )
    {
      diInfo.AddLayer( new GR::Graphic::Image( Font.GetLetter( i ) ),
                       diInfo.AddFrame() );
    }

    SetTitle( _T( "New Document" ) );
  }
  else if ( extension == "FNH" )
  {
    // ein HiColor-Font!
    diInfo.m_SaveType = SAVETYPE_FNH;
    diInfo.m_BitDepth = 16;
    diInfo.SetSize( 200, 200 );
    diInfo.m_DocType = DT_FONT;

    pView->OnInitialUpdate();
    diInfo.m_BitDepth = 16;

    GR::Font    Font;

    Font.LoadFNT( fileName.c_str(), 16 );

    for ( int i = 0; i < 256; i++ )
    {
      diInfo.AddLayer( new GR::Graphic::Image( Font.GetLetter( i ) ),
                       diInfo.AddFrame() );
    }

    SetTitle( _T( "New Document" ) );
  }
  else if ( extension == "BTH" )
  {
    // Die Datei ist wohl ein BTH...
    diInfo.m_SaveType = SAVETYPE_BTH;
    diInfo.AddFrame();
    CLayer*   pLayer = diInfo.AddLayer();
    pDummyImage = pLayer->GetImage();
    pDummyImage->LoadBTN( fileName.c_str(), 16 );
    diInfo.SetSize( pDummyImage->GetWidth(), pDummyImage->GetHeight() );
    diInfo.m_DocType = DT_IMAGE;
    diInfo.m_BitDepth = 16;
    pView->OnInitialUpdate();
  }
  else if ( ( extension == "PNG" )
  ||        ( extension == "WEBP" ) )
  {
    // freeimage
    GR::Graphic::Palette *pPalette = NULL;

    diInfo.m_SaveType = SAVETYPE_PNG;

    
    CPainterImagePackage* pPackage = FreeImageLoad( fileName.c_str() );

    if ( pPackage->m_pImage == NULL )
    {
      delete pPackage;
      AfxMessageBox( L"Unsupported or invalid file format!\n(less than 256 colors?)", MB_OK | MB_APPLMODAL );
      return FALSE;
    }

    diInfo.AddFrame();
    AddNewImage( pPackage->m_pImage, &pPackage->m_Palette, pPackage->m_pImageMask );
    delete pPackage;
  }
  else if ( ( extension == "TIFF" )
  ||        ( extension == "TIF" ) )
  {
    // Die Datei ist wohl ein TIFF...
    GR::Graphic::Palette *pPalette = NULL;

    diInfo.m_SaveType = SAVETYPE_JPEG;

    CPainterImagePackage *pPackage = FreeImageLoad( fileName.c_str() );

    if ( pPackage->m_pImage == NULL )
    {
      delete pPackage;
      MessageBoxA( NULL, "Invalid or not supported file format!\n(less than 256 colors?)", "Error", MB_OK | MB_APPLMODAL );
      return FALSE;
    }

    diInfo.AddFrame();
    AddNewImage( pPackage->m_pImage, &pPackage->m_Palette );
    delete pPackage;
  }
  else if ( extension == "ANX" )
  {
    // Die Datei ist wohl ein ANX...
    GR::Animation     tempAnim;

    diInfo.m_SaveType = SAVETYPE_ANX;
    tempAnim.LoadAni( fileName.c_str(), 8 );
    diInfo.SetSize( tempAnim.GetWidth(), tempAnim.GetHeight() );
    diInfo.m_AnimationRunning = FALSE;
    diInfo.m_DocType = DT_IMAGE;

    diInfo.m_BitDepth = 8;
    *diInfo.GetPalette( diInfo.CurrentFrame() ) = theApp.m_PainterPalette;

    for ( DWORD iFrame = 0; iFrame < tempAnim.GetFrames(); ++iFrame )
    {
      diInfo.AddLayer( new GR::Graphic::Image( tempAnim.GetImage( iFrame ) ), diInfo.AddFrame() );
    }
    
    pView->OnInitialUpdate();
    //diInfo.dwTimer = SetTimer( diInfo.pView->GetSafeHwnd(), 1, diInfo.dwOptionFrameSpeed, NULL );
  }
  else if ( extension == "ANH" )
  {
    // Die Datei ist wohl ein ANH...
    GR::Animation   tempAnim;

    diInfo.m_SaveType = SAVETYPE_ANH;
    tempAnim.LoadAni( fileName.c_str(), 16 );
    diInfo.SetSize( tempAnim.GetWidth(), tempAnim.GetHeight() );
    diInfo.m_AnimationRunning = FALSE;
    diInfo.m_DocType    = DT_IMAGE;
    diInfo.m_BitDepth = 16;

    for ( DWORD iFrame = 0; iFrame < tempAnim.GetFrames(); ++iFrame )
    {
      diInfo.AddLayer( new GR::Graphic::Image( tempAnim.GetImage( iFrame ) ), diInfo.AddFrame() );
    }
    
    pView->OnInitialUpdate();
    //diInfo.dwTimer = SetTimer( diInfo.pView->GetSafeHwnd(), 1, diInfo.dwOptionFrameSpeed, NULL );
  }
  else if ( extension == "PAL" )
  {
    // Die Datei ist wohl ein PAL...
    dh::Log( "Loading PAL-Files not supported." );
    return FALSE;
  }
  else
  {
    // Die Datei hat wohl ein unbekanntes Format...
    AfxMessageBox( _T( "Unknown file format" ) );
    return FALSE;
  }

  diInfo.m_FileName = fileName;

	return TRUE;
}



void CPainterDoc::OnCloseDocument() 
{
  if ( theApp.pMainFrame->m_SessionEnding )
  {
    return;
  }
  /*
  if ( ( diInfo.ucDocumentType == DOCUMENT_TYPE_ANX )
  &&   ( diInfo.dwTimer ) )
  {
    diInfo.pView->KillTimer( diInfo.dwTimer );
  }
  */

  diInfo.m_Closed = TRUE;

	CDocument::OnCloseDocument();
}



void CPainterDoc::OnSpezialAuf1bitkonvertieren()
{
  if ( diInfo.m_BitDepth == 1 )
  {
    return;
  }
  diInfo.ConvertTo1bpp();
}



void CPainterDoc::OnSpezialAuf4bitkonvertieren()
{

  if ( diInfo.m_BitDepth == 4 )
  {
    return;
  }
  diInfo.ConvertTo4bpp();

}



void CPainterDoc::OnConvertTo8bpp() 
{

  if ( diInfo.m_BitDepth == 8 )
  {
    // muﬂ nicht umgewandelt werden!
    return;
  }
  diInfo.ConvertTo8bpp();
	
}



void CPainterDoc::OnConvertTo16bpp() 
{

  if ( ( diInfo.m_BitDepth == 15 )
  ||   ( diInfo.m_BitDepth == 16 ) )
  {
    // muﬂ nicht umgewandelt werden!
    return;
  }
  diInfo.ConvertTo16bpp();

}



void CPainterDoc::OnConvertTo24bpp() 
{

  if ( diInfo.m_BitDepth == 24 )
  {
    // muﬂ nicht umgewandelt werden!
    return;
  }
  diInfo.ConvertTo24bpp();

}



void CPainterDoc::OnConvertTo32bpp() 
{

  if ( diInfo.m_BitDepth == 32 )
  {
    // muﬂ nicht umgewandelt werden!
    return;
  }
  diInfo.ConvertTo32bpp();

}



void CPainterDoc::OnBearbeitenMirrorH()
{
  CLayer     *pLayer;


  for ( size_t i = 0; i < diInfo.m_LayeredFrames[0].LayerCount(); i++ )
  {
    pLayer = diInfo.GetLayer( 0, i );

    HMirrorImage( pLayer->GetImage() );
  }
  diInfo.RedrawAllViews();
}



void CPainterDoc::OnBearbeitenMirrorV() 
{
  CLayer     *pLayer;


  for ( size_t i = 0; i < diInfo.m_LayeredFrames[0].LayerCount(); i++ )
  {
    pLayer = diInfo.GetLayer( 0, i );
    VMirrorImage( pLayer->GetImage() );
  }
  diInfo.RedrawAllViews();
}



void CPainterDoc::OnBearbeitenRotieren()
{
  ViewInfo *pViewInfo = theApp.GetActiveViewInfo();
  if ( pViewInfo->m_pImageFloatingSelection != NULL )
  {
    return;
  }
  diInfo.Rotate();

  diInfo.RedrawAllViews();
}



void CPainterDoc::OnBearbeitenLoeschen() 
{
  CLayer*   pLayer = diInfo.ActiveLayer();
  if ( pLayer == NULL )
  {
    return;
  }
  
  if ( AfxGetMainWnd()->MessageBox( _T( "Sind Sie sicher?" ), _T( "Lˆschen des Bildes" ), MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL ) == IDYES )
  {
    CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( pLayer->GetImage(), diInfo.GetPalette( diInfo.CurrentFrame() ) );
    pCD->Box( 0, 0, pCD->Width(), pCD->Height(), 0 );
    SafeDelete( pCD );

    diInfo.RedrawAllViews();
  }
}



void CPainterDoc::OnBearbeitenResizeCanvas() 
{
  CDlgResizeCanvas    theDlg;


  theDlg.m_pDocInfo = &diInfo;
  if ( theDlg.DoModal() == IDOK )
  {
    diInfo.ResizeCanvas( theDlg.m_NewWidth, 
                         theDlg.m_NewHeight,
                         theDlg.m_CenterH,
                         theDlg.m_CenterV );
  }
}



void CPainterDoc::OnBearbeitenResize() 
{
  CDialogResize     theDlg;

  theDlg.m_pDocInfo = &diInfo;
  if ( theDlg.DoModal() == IDOK )
  {
    if ( ( theDlg.Width == 0 )
    ||   ( theDlg.Height == 0 ) )
    {
      return;
    }
    if ( ( theDlg.Width == diInfo.Width() )
    &&   ( theDlg.Height == diInfo.Height() ) )
    {
      return;
    }
    diInfo.Resize( theDlg.Width, theDlg.Height, theDlg.ResizeType, theDlg.PercentageX, theDlg.PercentageY );
  }
}



#define uint32 unsigned long
#define uint16 unsigned short
#define uint8 unsigned char

/********** 2XSAI Filter *****************/
static uint32 colorMask = 0xF7DEF7DE;
static uint32 lowPixelMask = 0x08210821;
static uint32 qcolorMask = 0xE79CE79C;
static uint32 qlowpixelMask = 0x18631863;
static uint32 redblueMask = 0xF81F;
static uint32 greenMask = 0x7E0;

int Init_2xSaI(uint32 BitFormat)
{
	if (BitFormat == 565) {
		colorMask = 0xF7DEF7DE;
		lowPixelMask = 0x08210821;
		qcolorMask = 0xE79CE79C;
		qlowpixelMask = 0x18631863;
		redblueMask = 0xF81F;
		greenMask = 0x7E0;
	} else if (BitFormat == 555) {
		colorMask = 0x7BDE7BDE;
		lowPixelMask = 0x04210421;
		qcolorMask = 0x739C739C;
		qlowpixelMask = 0x0C630C63;
		redblueMask = 0x7C1F;
		greenMask = 0x3E0;
	} else {
		return 0;
	}

	return 1;
}

static inline int GetResult1(uint32 A, uint32 B, uint32 C, uint32 D, uint32 /* E */ )
{
	int x = 0;
	int y = 0;
	int r = 0;

	if (A == C)
		x += 1;
	else if (B == C)
		y += 1;
	if (A == D)
		x += 1;
	else if (B == D)
		y += 1;
	if (x <= 1)
		r += 1;
	if (y <= 1)
		r -= 1;
	return r;
}

static inline int GetResult2(uint32 A, uint32 B, uint32 C, uint32 D, uint32 /* E */ )
{
	int x = 0;
	int y = 0;
	int r = 0;

	if (A == C)
		x += 1;
	else if (B == C)
		y += 1;
	if (A == D)
		x += 1;
	else if (B == D)
		y += 1;
	if (x <= 1)
		r -= 1;
	if (y <= 1)
		r += 1;
	return r;
}

static inline int GetResult(uint32 A, uint32 B, uint32 C, uint32 D)
{
	int x = 0;
	int y = 0;
	int r = 0;

	if (A == C)
		x += 1;
	else if (B == C)
		y += 1;
	if (A == D)
		x += 1;
	else if (B == D)
		y += 1;
	if (x <= 1)
		r += 1;
	if (y <= 1)
		r -= 1;
	return r;
}

static inline uint32 INTERPOLATE(uint32 A, uint32 B)
{
	if (A != B) {
		return (((A & colorMask) >> 1) + ((B & colorMask) >> 1) + (A & B & lowPixelMask));
	} else
		return A;
}

static inline uint32 Q_INTERPOLATE(uint32 A, uint32 B, uint32 C, uint32 D)
{
	register uint32 x = ((A & qcolorMask) >> 2) +
		((B & qcolorMask) >> 2) + ((C & qcolorMask) >> 2) + ((D & qcolorMask) >> 2);
	register uint32 y = (A & qlowpixelMask) +
		(B & qlowpixelMask) + (C & qlowpixelMask) + (D & qlowpixelMask);

	y = (y >> 2) & qlowpixelMask;
	return x + y;
}

#define BLUE_MASK565 0x001F001F
#define RED_MASK565 0xF800F800
#define GREEN_MASK565 0x07E007E0

#define BLUE_MASK555 0x001F001F
#define RED_MASK555 0x7C007C00
#define GREEN_MASK555 0x03E003E0

void Super2xSaI(uint8 *srcPtr, uint32 srcPitch,
								uint8 *deltaPtr, uint8 *dstPtr, uint32 dstPitch, int width, int height)
{
	uint16 *bP;
	uint8 *dP;
	uint32 inc_bP;

	{
		uint32 Nextline = srcPitch >> 1;
		inc_bP = 1;

		while (height--) {
			bP = (uint16 *)srcPtr;
			dP = (uint8 *)dstPtr;

			for (uint32 finish = width; finish; finish -= inc_bP) {
				uint32 color4, color5, color6;
				uint32 color1, color2, color3;
				uint32 colorA0, colorA1, colorA2, colorA3, colorB0, colorB1, colorB2, colorB3, colorS1, colorS2;
				uint32 product1a, product1b, product2a, product2b;

//---------------------------------------    B1 B2
//                                         4  5  6 S2
//                                         1  2  3 S1
//                                           A1 A2

				colorB0 = *(bP - Nextline - 1);
				colorB1 = *(bP - Nextline);
				colorB2 = *(bP - Nextline + 1);
				colorB3 = *(bP - Nextline + 2);

				color4 = *(bP - 1);
				color5 = *(bP);
				color6 = *(bP + 1);
				colorS2 = *(bP + 2);

				color1 = *(bP + Nextline - 1);
				color2 = *(bP + Nextline);
				color3 = *(bP + Nextline + 1);
				colorS1 = *(bP + Nextline + 2);

				colorA0 = *(bP + Nextline + Nextline - 1);
				colorA1 = *(bP + Nextline + Nextline);
				colorA2 = *(bP + Nextline + Nextline + 1);
				colorA3 = *(bP + Nextline + Nextline + 2);

//--------------------------------------
				if (color2 == color6 && color5 != color3) {
					product2b = product1b = color2;
				} else if (color5 == color3 && color2 != color6) {
					product2b = product1b = color5;
				} else if (color5 == color3 && color2 == color6) {
					register int r = 0;

					r += GetResult(color6, color5, color1, colorA1);
					r += GetResult(color6, color5, color4, colorB1);
					r += GetResult(color6, color5, colorA2, colorS1);
					r += GetResult(color6, color5, colorB2, colorS2);

					if (r > 0)
						product2b = product1b = color6;
					else if (r < 0)
						product2b = product1b = color5;
					else {
						product2b = product1b = INTERPOLATE(color5, color6);
					}
				} else {
					if (color6 == color3 && color3 == colorA1 && color2 != colorA2 && color3 != colorA0)
						product2b = Q_INTERPOLATE(color3, color3, color3, color2);
					else if (color5 == color2 && color2 == colorA2 && colorA1 != color3 && color2 != colorA3)
						product2b = Q_INTERPOLATE(color2, color2, color2, color3);
					else
						product2b = INTERPOLATE(color2, color3);

					if (color6 == color3 && color6 == colorB1 && color5 != colorB2 && color6 != colorB0)
						product1b = Q_INTERPOLATE(color6, color6, color6, color5);
					else if (color5 == color2 && color5 == colorB2 && colorB1 != color6 && color5 != colorB3)
						product1b = Q_INTERPOLATE(color6, color5, color5, color5);
					else
						product1b = INTERPOLATE(color5, color6);
				}

				if (color5 == color3 && color2 != color6 && color4 == color5 && color5 != colorA2)
					product2a = INTERPOLATE(color2, color5);
				else if (color5 == color1 && color6 == color5 && color4 != color2 && color5 != colorA0)
					product2a = INTERPOLATE(color2, color5);
				else
					product2a = color2;

				if (color2 == color6 && color5 != color3 && color1 == color2 && color2 != colorB2)
					product1a = INTERPOLATE(color2, color5);
				else if (color4 == color2 && color3 == color2 && color1 != color5 && color2 != colorB0)
					product1a = INTERPOLATE(color2, color5);
				else
					product1a = color5;

#ifdef SCUMM_LITTLE_ENDIAN
				product1a = product1a | (product1b << 16);
				product2a = product2a | (product2b << 16);
#endif

#ifdef SCUMM_BIG_ENDIAN
				product1a = product1b | (product1a << 16);
				product2a = product2b | (product2a << 16);
#endif
				*((uint32 *)dP) = product1a;
				*((uint32 *)(dP + dstPitch)) = product2a;

				bP += inc_bP;
				dP += sizeof(uint32);
			}													// end of for ( finish= width etc..)

			srcPtr += srcPitch;
			dstPtr += dstPitch * 2;
			deltaPtr += srcPitch;
		}														// while (height--)
	}
}

void SuperEagle(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr,
								uint8 *dstPtr, uint32 dstPitch, int width, int height)
{
	uint8 *dP;
	uint16 *bP;
	uint32 inc_bP;

	{
		inc_bP = 1;

		uint32 Nextline = srcPitch >> 1;

		while (height--) {
			bP = (uint16 *)srcPtr;
			dP = dstPtr;
			for (uint32 finish = width; finish; finish -= inc_bP) {
				uint32 color4, color5, color6;
				uint32 color1, color2, color3;
				uint32 colorA1, colorA2, colorB1, colorB2, colorS1, colorS2;
				uint32 product1a, product1b, product2a, product2b;

				colorB1 = *(bP - Nextline);
				colorB2 = *(bP - Nextline + 1);

				color4 = *(bP - 1);
				color5 = *(bP);
				color6 = *(bP + 1);
				colorS2 = *(bP + 2);

				color1 = *(bP + Nextline - 1);
				color2 = *(bP + Nextline);
				color3 = *(bP + Nextline + 1);
				colorS1 = *(bP + Nextline + 2);

				colorA1 = *(bP + Nextline + Nextline);
				colorA2 = *(bP + Nextline + Nextline + 1);

				// --------------------------------------
				if (color2 == color6 && color5 != color3) {
					product1b = product2a = color2;
					if ((color1 == color2) || (color6 == colorB2)) {
						product1a = INTERPOLATE(color2, color5);
						product1a = INTERPOLATE(color2, product1a);
					} else {
						product1a = INTERPOLATE(color5, color6);
					}

					if ((color6 == colorS2) || (color2 == colorA1)) {
						product2b = INTERPOLATE(color2, color3);
						product2b = INTERPOLATE(color2, product2b);
					} else {
						product2b = INTERPOLATE(color2, color3);
					}
				} else if (color5 == color3 && color2 != color6) {
					product2b = product1a = color5;

					if ((colorB1 == color5) || (color3 == colorS1)) {
						product1b = INTERPOLATE(color5, color6);
						product1b = INTERPOLATE(color5, product1b);
					} else {
						product1b = INTERPOLATE(color5, color6);
					}

					if ((color3 == colorA2) || (color4 == color5)) {
						product2a = INTERPOLATE(color5, color2);
						product2a = INTERPOLATE(color5, product2a);
					} else {
						product2a = INTERPOLATE(color2, color3);
					}

				} else if (color5 == color3 && color2 == color6) {
					register int r = 0;

					r += GetResult(color6, color5, color1, colorA1);
					r += GetResult(color6, color5, color4, colorB1);
					r += GetResult(color6, color5, colorA2, colorS1);
					r += GetResult(color6, color5, colorB2, colorS2);

					if (r > 0) {
						product1b = product2a = color2;
						product1a = product2b = INTERPOLATE(color5, color6);
					} else if (r < 0) {
						product2b = product1a = color5;
						product1b = product2a = INTERPOLATE(color5, color6);
					} else {
						product2b = product1a = color5;
						product1b = product2a = color2;
					}
				} else {
					product2b = product1a = INTERPOLATE(color2, color6);
					product2b = Q_INTERPOLATE(color3, color3, color3, product2b);
					product1a = Q_INTERPOLATE(color5, color5, color5, product1a);

					product2a = product1b = INTERPOLATE(color5, color3);
					product2a = Q_INTERPOLATE(color2, color2, color2, product2a);
					product1b = Q_INTERPOLATE(color6, color6, color6, product1b);
				}
#ifdef SCUMM_LITTLE_ENDIAN
				product1a = product1a | (product1b << 16);
				product2a = product2a | (product2b << 16);
#endif

#ifdef SCUMM_BIG_ENDIAN
				product1a = product1b | (product1a << 16);
				product2a = product2b | (product2a << 16);
#endif
				*((uint32 *)dP) = product1a;
				*((uint32 *)(dP + dstPitch)) = product2a;

				bP += inc_bP;
				dP += sizeof(uint32);
			}													// end of for ( finish= width etc..)

			srcPtr += srcPitch;
			dstPtr += dstPitch * 2;
			deltaPtr += srcPitch;
		}														// endof: while (height--)
	}
}

void _2xSaI(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr,
						uint8 *dstPtr, uint32 dstPitch, int width, int height)
{
	uint8 *dP;
	uint16 *bP;
	uint32 inc_bP;


	{
		inc_bP = 1;

		uint32 Nextline = srcPitch >> 1;

		while (height--) {
			bP = (uint16 *)srcPtr;
			dP = dstPtr;

			for (uint32 finish = width; finish; finish -= inc_bP) {

				register uint32 colorA, colorB;
				uint32 colorC, colorD,
					colorE, colorF, colorG, colorH, colorI, colorJ, colorK, colorL, colorM, colorN, colorO, colorP;
				uint32 product, product1, product2;

//---------------------------------------
// Map of the pixels:                    I|E F|J
//                                       G|A B|K
//                                       H|C D|L
//                                       M|N O|P
				colorI = *(bP - Nextline - 1);
				colorE = *(bP - Nextline);
				colorF = *(bP - Nextline + 1);
				colorJ = *(bP - Nextline + 2);

				colorG = *(bP - 1);
				colorA = *(bP);
				colorB = *(bP + 1);
				colorK = *(bP + 2);

				colorH = *(bP + Nextline - 1);
				colorC = *(bP + Nextline);
				colorD = *(bP + Nextline + 1);
				colorL = *(bP + Nextline + 2);

				colorM = *(bP + Nextline + Nextline - 1);
				colorN = *(bP + Nextline + Nextline);
				colorO = *(bP + Nextline + Nextline + 1);
				colorP = *(bP + Nextline + Nextline + 2);

				if ((colorA == colorD) && (colorB != colorC)) {
					if (((colorA == colorE) && (colorB == colorL)) || ((colorA == colorC) && (colorA == colorF)
																														 && (colorB != colorE)
																														 && (colorB == colorJ))) {
						product = colorA;
					} else {
						product = INTERPOLATE(colorA, colorB);
					}

					if (((colorA == colorG) && (colorC == colorO)) || ((colorA == colorB) && (colorA == colorH)
																														 && (colorG != colorC)
																														 && (colorC == colorM))) {
						product1 = colorA;
					} else {
						product1 = INTERPOLATE(colorA, colorC);
					}
					product2 = colorA;
				} else if ((colorB == colorC) && (colorA != colorD)) {
					if (((colorB == colorF) && (colorA == colorH)) || ((colorB == colorE) && (colorB == colorD)
																														 && (colorA != colorF)
																														 && (colorA == colorI))) {
						product = colorB;
					} else {
						product = INTERPOLATE(colorA, colorB);
					}

					if (((colorC == colorH) && (colorA == colorF)) || ((colorC == colorG) && (colorC == colorD)
																														 && (colorA != colorH)
																														 && (colorA == colorI))) {
						product1 = colorC;
					} else {
						product1 = INTERPOLATE(colorA, colorC);
					}
					product2 = colorB;
				} else if ((colorA == colorD) && (colorB == colorC)) {
					if (colorA == colorB) {
						product = colorA;
						product1 = colorA;
						product2 = colorA;
					} else {
						register int r = 0;

						product1 = INTERPOLATE(colorA, colorC);
						product = INTERPOLATE(colorA, colorB);

						r += GetResult1(colorA, colorB, colorG, colorE, colorI);
						r += GetResult2(colorB, colorA, colorK, colorF, colorJ);
						r += GetResult2(colorB, colorA, colorH, colorN, colorM);
						r += GetResult1(colorA, colorB, colorL, colorO, colorP);

						if (r > 0)
							product2 = colorA;
						else if (r < 0)
							product2 = colorB;
						else {
							product2 = Q_INTERPOLATE(colorA, colorB, colorC, colorD);
						}
					}
				} else {
					product2 = Q_INTERPOLATE(colorA, colorB, colorC, colorD);

					if ((colorA == colorC) && (colorA == colorF)
							&& (colorB != colorE) && (colorB == colorJ)) {
						product = colorA;
					} else if ((colorB == colorE) && (colorB == colorD)
										 && (colorA != colorF) && (colorA == colorI)) {
						product = colorB;
					} else {
						product = INTERPOLATE(colorA, colorB);
					}

					if ((colorA == colorB) && (colorA == colorH)
							&& (colorG != colorC) && (colorC == colorM)) {
						product1 = colorA;
					} else if ((colorC == colorG) && (colorC == colorD)
										 && (colorA != colorH) && (colorA == colorI)) {
						product1 = colorC;
					} else {
						product1 = INTERPOLATE(colorA, colorC);
					}
				}

#ifdef SCUMM_LITTLE_ENDIAN
				product = colorA | (product << 16);
				product1 = product1 | (product2 << 16);
#endif

#ifdef SCUMM_BIG_ENDIAN
				product = (colorA << 16) | product;
				product1 = (product1 << 16) | product2;
#endif
				*((uint32 *)dP) = product;
				*((uint32 *)(dP + dstPitch)) = product1;

				bP += inc_bP;
				dP += sizeof(uint32);
			}													// end of for ( finish= width etc..)

			srcPtr += srcPitch;
			dstPtr += dstPitch * 2;
			deltaPtr += srcPitch;
		}														// endof: while (height--)
	}
}

static uint32 Bilinear(uint32 A, uint32 B, uint32 x)
{
	unsigned long areaA, areaB;
	unsigned long result;

	if (A == B)
		return A;

	areaB = (x >> 11) & 0x1f;			// reduce 16 bit fraction to 5 bits
	areaA = 0x20 - areaB;

	A = (A & redblueMask) | ((A & greenMask) << 16);
	B = (B & redblueMask) | ((B & greenMask) << 16);

	result = ((areaA * A) + (areaB * B)) >> 5;

	return (result & redblueMask) | ((result >> 16) & greenMask);

}

static uint32 Bilinear4(uint32 A, uint32 B, uint32 C, uint32 D, uint32 x, uint32 y)
{
	unsigned long areaA, areaB, areaC, areaD;
	unsigned long result, xy;

	x = (x >> 11) & 0x1f;
	y = (y >> 11) & 0x1f;
	xy = (x * y) >> 5;

	A = (A & redblueMask) | ((A & greenMask) << 16);
	B = (B & redblueMask) | ((B & greenMask) << 16);
	C = (C & redblueMask) | ((C & greenMask) << 16);
	D = (D & redblueMask) | ((D & greenMask) << 16);

	areaA = 0x20 + xy - x - y;
	areaB = x - xy;
	areaC = y - xy;
	areaD = xy;

	result = ((areaA * A) + (areaB * B) + (areaC * C) + (areaD * D)) >> 5;

	return (result & redblueMask) | ((result >> 16) & greenMask);
}

void Scale_2xSaI( GR::Graphic::ContextDescriptor* pCDSrc, GR::Graphic::ContextDescriptor* pCDTgt )
{

	uint8*    dP;
	uint16*   bP;

	uint32 w;
	uint32 h;
	uint32 dw;
	uint32 dh;
	uint32 hfinish;
	uint32 wfinish;

  uint8* dstPtr = (BYTE*)pCDTgt->Data();

  uint32 Nextline = pCDSrc->LineOffsetInBytes() >> 1;

  wfinish = ( pCDSrc->Width() - 1 ) << 16;	// convert to fixed point
  dw      = wfinish / ( pCDTgt->Width() - 1 );
	hfinish = ( pCDSrc->Height() - 1 ) << 16;	// convert to fixed point
  dh      = hfinish / ( pCDTgt->Height() - 1 );

	for ( h = 0; h < hfinish; h += dh ) 
  {
		uint32    y1, 
              y2;

		y1 = h & 0xffff;						// fraction part of fixed point
    bP = (uint16 *)( (BYTE*)pCDSrc->Data() + ( ( h >> 16 ) * pCDSrc->LineOffsetInBytes() ) );
		dP = dstPtr;
		y2 = 0x10000 - y1;

		w = 0;

    while ( w < wfinish )
    {
			uint32 A, B, C, D;
			uint32 E, F, G, H;
			uint32 I, J, K, L;
			uint32 x1, x2, a1, f1, f2;
			uint32 position, product1;

			
      position = w >> 16;

      position = ( w >> 16 ) + ( h >> 16 ) * pCDSrc->Width();

      A = pCDSrc->GetDirectPixel( ( position ) % pCDSrc->Width(), ( position ) / pCDSrc->Width() );
      B = pCDSrc->GetDirectPixel( ( position + 1 ) % pCDSrc->Width(), ( position + 1 ) / pCDSrc->Width() );
      C = pCDSrc->GetDirectPixel( ( position + Nextline ) % pCDSrc->Width(), ( position + Nextline ) / pCDSrc->Width() );
      D = pCDSrc->GetDirectPixel( ( position + Nextline + 1 ) % pCDSrc->Width(), ( position + Nextline + 1 ) / pCDSrc->Width() );
      E = pCDSrc->GetDirectPixel( ( position - Nextline ) % pCDSrc->Width(), ( position - Nextline ) / pCDSrc->Width() );
      F = pCDSrc->GetDirectPixel( ( position - Nextline + 1 ) % pCDSrc->Width(), ( position - Nextline + 1 ) / pCDSrc->Width() );
      G = pCDSrc->GetDirectPixel( ( position - 1 ) % pCDSrc->Width(), ( position - 1 ) / pCDSrc->Width() );
      H = pCDSrc->GetDirectPixel( ( position + Nextline - 1 ) % pCDSrc->Width(), ( position + Nextline - 1 ) / pCDSrc->Width() );
      I = pCDSrc->GetDirectPixel( ( position + 2 ) % pCDSrc->Width(), ( position + 2 ) / pCDSrc->Width() );
      J = pCDSrc->GetDirectPixel( ( position + Nextline + 2 ) % pCDSrc->Width(), ( position + Nextline + 2 ) / pCDSrc->Width() );
      K = pCDSrc->GetDirectPixel( ( position + Nextline + Nextline ) % pCDSrc->Width(), ( position + Nextline + Nextline ) / pCDSrc->Width() );
      L = pCDSrc->GetDirectPixel( ( position + Nextline + Nextline + 1 ) % pCDSrc->Width(), ( position + Nextline + Nextline + 1 ) / pCDSrc->Width() );

      /*
			A = bP[position];					// current pixel
			B = bP[position + 1];			// next pixel
			C = bP[position + Nextline];
			D = bP[position + Nextline + 1];
			E = bP[position - Nextline];
			F = bP[position - Nextline + 1];
			G = bP[position - 1];
			H = bP[position + Nextline - 1];
			I = bP[position + 2];
			J = bP[position + Nextline + 2];
			K = bP[position + Nextline + Nextline];
			L = bP[position + Nextline + Nextline + 1];
      */

			x1 = w & 0xffff;					// fraction part of fixed point
			x2 = 0x10000 - x1;

			if ( ( A == B )
      &&   ( C == D )
      &&   ( A == C ) )
      {
        // 0
				product1 = A;
      }
			else if ( ( A == D )
      &&        ( B != C ) )
      {
        // 1
				f1 = ( x1 >> 1 ) + ( 0x10000 >> 2 );
				f2 = ( y1 >> 1 ) + ( 0x10000 >> 2 );
				if ( ( y1 <= f1 )
        &&   ( A == J )
        &&   ( A != E ) )	
				{
          // close to B
					a1 = f1 - y1;
					product1 = Bilinear( A, B, a1 );
				} 
        else if ( ( y1 >= f1 )
        &&        ( A == G )
        &&        ( A != L ) )	
				{
          // close to C
					a1 = y1 - f1;
					product1 = Bilinear( A, C, a1 );
				} 
        else if ( ( x1 >= f2 )
        &&        ( A == E )
        &&        ( A != J ) )	
				{
          // close to B
					a1 = x1 - f2;
					product1 = Bilinear( A, B, a1 );
				} 
        else if ( ( x1 <= f2 )
        &&        ( A == L )
        &&        ( A != G ) )	
				{
          // close to C
					a1 = f2 - x1;
					product1 = Bilinear( A, C, a1 );
				} 
        else if ( y1 >= x1 )		
				{
          // close to C
					a1 = y1 - x1;
					product1 = Bilinear( A, C, a1 );
				} 
        else if ( y1 <= x1 )		
				{
          // close to B
					a1 = x1 - y1;
					product1 = Bilinear( A, B, a1 );
				}
			} 
      else if ( ( B == C )
      &&        ( A != D ) )
      {
        // 2
				f1 = ( x1 >> 1 ) + ( 0x10000 >> 2 );
				f2 = ( y1 >> 1 ) + ( 0x10000 >> 2 );
				if ( ( y2 >= f1 )
        &&   ( B == H )
        &&   ( B != F ) )	
				{
          // close to A
					a1 = y2 - f1;
					product1 = Bilinear( B, A, a1 );
				} 
        else if ( ( y2 <= f1 )
        &&        ( B == I )
        &&        ( B != K ) )	
				{
          // close to D
					a1 = f1 - y2;
					product1 = Bilinear( B, D, a1 );
				} 
        else if ( ( x2 >= f2 )
        &&        ( B == F )
        &&        ( B != H ) )	
				{
          // close to A
					a1 = x2 - f2;
					product1 = Bilinear( B, A, a1 );
				} 
        else if ( ( x2 <= f2 )
        &&        ( B == K )
        &&        ( B != I ) )	
				{
          // close to D
					a1 = f2 - x2;
					product1 = Bilinear( B, D, a1 );
				} 
        else if ( y2 >= x1 )		
				{
          // close to A
					a1 = y2 - x1;
					product1 = Bilinear( B, A, a1 );
				} 
        else if ( y2 <= x1 )		
				{
          // close to D
					a1 = x1 - y2;
					product1 = Bilinear( B, D, a1 );
				}
			}
			else 
      {
        // 3
				product1 = Bilinear4( A, B, C, D, x1, y1 );
			}

      //end First Pixel
      int   iOffset = ( (BYTE*)dP - (BYTE*)pCDTgt->Data() ) / 2;

      pCDTgt->PutDirectPixel( iOffset % pCDTgt->Width(), iOffset / pCDTgt->Width(),         product1 & 0xffff );
      pCDTgt->PutDirectPixel( ( iOffset % pCDTgt->Width() ) + 1, iOffset / pCDTgt->Width(), ( product1 >> 16 ) & 0xffff );
			dP += 2;
			w += dw;
		}
    dstPtr += pCDTgt->LineOffsetInBytes();
	}

}



void CPainterDoc::OnExtras2xsai()
{

  if ( diInfo.m_BitDepth != 15 )
  {
    AfxMessageBox( _T( "2xSAI unterst¸tzt im Moment nur 16 Bit Farbtiefe!" ) );
    return;
  }
  GR::Graphic::ContextDescriptor      cdMeins( diInfo.ActiveLayer()->GetImage() );

  GR::Graphic::Image*             pImageTarget = new GR::Graphic::Image( cdMeins.Width() * 2, cdMeins.Height() * 2, cdMeins.BitsProPixel() );

  GR::Graphic::ContextDescriptor      cdTarget( pImageTarget );

  if ( Init_2xSaI( 555 ) )
  {
    Scale_2xSaI( &cdMeins, &cdTarget );

    diInfo.GetLayer( 0, 0 )->SetLayerImage( pImageTarget );
    diInfo.SetSize( cdTarget.Width(), cdTarget.Height() );

    diInfo.RecreateAllViews();
    diInfo.RedrawAllViews();
  }

}



void CPainterDoc::OnBearbeitenExportieren32909()
{
  CFileDialog     dlgFile( FALSE );

  if ( dlgFile.DoModal() == IDOK )
  {
    // Export!
    GR::u32       dwWidth = 0;
    GR::u32       dwHeight = 1;

    GR::Graphic::eImageFormat  imgFormat = GR::Graphic::IF_UNKNOWN;

    GR::Graphic::Palette  Pal;

    for ( size_t i = 0; i < diInfo.m_LayeredFrames.size(); ++i )
    {
      GR::Graphic::Image* pImage = diInfo.GetImage( i, 0 );
      if ( pImage )
      {
        if ( dwWidth < pImage->GetWidth() )
        {
          dwWidth = pImage->GetWidth();
        }
        dwHeight += 3 + pImage->GetHeight();

        imgFormat = GR::Graphic::ImageData::ImageFormatFromDepth( pImage->GetDepth() );

        if ( diInfo.GetPalette( i ) )
        {
          Pal = *diInfo.GetPalette( i );
        }
      }
    }

    if ( imgFormat != GR::Graphic::IF_UNKNOWN )
    {
      GR::Graphic::ContextDescriptor      cdFullImage;

      cdFullImage.CreateData( dwWidth + 2, dwHeight, imgFormat );
      cdFullImage.SetPalette( Pal );

      int     iY = 1;

      for ( size_t i = 0; i < diInfo.m_LayeredFrames.size(); ++i )
      {
        GR::Graphic::Image* pImage = diInfo.GetImage( i, 0 );
        if ( pImage )
        {
          GR::Graphic::ContextDescriptor    cdImage( pImage, &Pal );

          cdFullImage.Rectangle( 0, iY - 1, cdImage.Width() + 2, cdImage.Height() + 2, 0xff00ff00 );

          cdImage.CopyArea( 0, 0, cdImage.Width(), cdImage.Height(), 1, iY, &cdFullImage );

          iY += cdImage.Height() + 3;
        }
      }

      GR::Graphic::Image* pImgResult = cdFullImage.CreateImage();

      pImgResult->Save( GR::Convert::ToUTF8( (LPCTSTR)dlgFile.GetPathName() ).c_str(), FALSE, &Pal );

      delete pImgResult;
    }
  }
}



void CPainterDoc::OnBearbeitenImportieren32910()
{
  CFileDialog     dlgFile( TRUE );

  if ( dlgFile.DoModal() == IDOK )
  {
    // Import
    GR::Graphic::Palette    Pal;

    for ( size_t i = 0; i < diInfo.m_LayeredFrames.size(); ++i )
    {
      GR::Graphic::Image* pImage = diInfo.GetImage( i, 0 );
      if ( pImage )
      {
        Pal = *diInfo.GetPalette( i );
        break;
      }
    }

    GR::Graphic::Image      imgFont;

    imgFont.Load( GR::Convert::ToUTF8( (LPCTSTR)dlgFile.GetPathName() ).c_str() );

    GR::Graphic::ContextDescriptor    cdFont( imgFont );
    cdFont.SetPalette( Pal );

    GR::u32       dwBorderColor = cdFont.GetPixel( 0, 0 );

    int     iY = 1;

    for ( size_t i = 0; i < diInfo.m_LayeredFrames.size(); ++i )
    {
      GR::Graphic::Image* pImage = diInfo.GetImage( i, 0 );
      if ( pImage )
      {
        int           iX = 1;
        int           iY2 = iY - 1;

        while ( cdFont.GetPixel( iX, iY2 ) == dwBorderColor )
        {
          ++iX;
        }
        if ( iX > 0 )
        {
          --iX;
        }
        iY2++;
        while ( cdFont.GetPixel( iX, iY2 ) == dwBorderColor )
        {
          ++iY2;
        }

        GR::Graphic::Image*   pImgLetter = imgFont.GetTileImage( 1, iY, iX - 1, iY2 - iY - 1 );

        diInfo.GetLayer( i, 0 )->SetLayerImage( pImgLetter );

        iY = iY2 + 2;
      }
    }
    diInfo.SetModify( TRUE );
  }
}



void CPainterDoc::OnBearbeitenRunscript()
{
  CPainterLua     Lua;

  CFileDialog   dlgFile( TRUE, _T( "lua" ), 0, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, 0, AfxGetMainWnd() );

  if ( dlgFile.DoModal() == IDOK )
  {
    CString     cstrGnu = dlgFile.GetPathName();

    GR::String strFile = GR::Convert::ToUTF8( (LPCTSTR)cstrGnu );

    Lua.m_pScriptDocInfo  = theApp.GetActiveDocumentInfo();
    Lua.m_pScriptViewInfo = theApp.GetActiveViewInfo();
    Lua.DoFile( strFile );
  }
}
