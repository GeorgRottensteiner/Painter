#include "stdafx.h"
#include "Painter.h"
#include "MainFrm.h"
#include "Settings.h"
#include "PainterFontDoc.h"
#include "PainterFontView.h"

#include <debug/debugclient.h>
#include <String/Path.h>
#include <String/StringUtil.h>

#include ".\painterfontdoc.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPainterApp        theApp;

extern CSettings          theSettings;



IMPLEMENT_DYNCREATE(CPainterFontDoc, CDocument)

CPainterFontDoc::CPainterFontDoc()
{

  diInfo.m_pDoc = this;
  diInfo.SetSize( 600, 600 );
  diInfo.m_BitDepth = 16;
  diInfo.m_StartX    = 0;
  diInfo.m_StartY    = 0;
  diInfo.m_ActX      = 0;
  diInfo.m_ActY      = 0;

  diInfo.m_SelectionX            = 0;
  diInfo.m_SelectionY            = 0;
  diInfo.m_SelectionWidth        = 0;
  diInfo.m_SelectionHeight       = 0;

}



CPainterFontDoc::~CPainterFontDoc()
{
}


BEGIN_MESSAGE_MAP(CPainterFontDoc, CDocument)
	//{{AFX_MSG_MAP(CPainterFontDoc)
	ON_COMMAND(ID_CONVERT_TO_16BPP, OnConvertTo16bpp)
	ON_COMMAND(ID_CONVERT_TO_24BPP, OnConvertTo24bpp)
	ON_COMMAND(ID_CONVERT_TO_32BPP, OnConvertTo32bpp)
	ON_COMMAND(ID_CONVERT_TO_8BPP, OnConvertTo8bpp)
	//}}AFX_MSG_MAP
  ON_COMMAND(ID_SPEZIAL_INVERTIEREN, OnSpezialInvertieren)
  ON_COMMAND(ID_BEARBEITEN_EXPORTIEREN, OnBearbeitenExportieren)
  ON_COMMAND(ID_BEARBEITEN_IMPORTIEREN, OnBearbeitenImportieren)
  ON_COMMAND(ID_BEARBEITEN_RESIZE, OnBearbeitenResize)
END_MESSAGE_MAP()


#ifdef _DEBUG
void CPainterFontDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPainterFontDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



BOOL SaveFNXL( CPainterFontDoc *pDoc, const GR::Char* File )
{
  WORD            wDummy;

  unsigned char   ucDummy,
                  ucBuffer[2048];

  int             i;


  GR::IO::FileStream    ioOut;

  if ( !ioOut.Open( File, IIOStream::OT_WRITE_ONLY ) )
  {
    MessageBoxA( NULL, "Konnte Datei nicht zum Schreiben öffnen.", "Fehler", MB_OK | MB_APPLMODAL );
    return FALSE;
  }

  wsprintf( (GR::WChar*)ucBuffer, _T( "(C)Invisible Bit Font" ) );
  ucBuffer[21] = 0;
  ucBuffer[22] = 26;
  ioOut.WriteBlock( ucBuffer, 23 );
  wDummy = 256;
  

  ioOut.WriteU16( 256 ); // Anzahl Zeichen
  ioOut.WriteU16( 256 ); // max. Breite
  ioOut.WriteU16( 256 ); // max. Höhe
  ioOut.WriteU16( 256 ); // max. Größe
  ucDummy = (BYTE)pDoc->diInfo.ToLocalColor( CSettings::CO_WORKCOLOR );
  ioOut.WriteU8( ucDummy ); // Default-Farbe des Fonts
  for ( i = 0; i < 256; i++ )
  {
    GR::Graphic::Image*   pLetter = pDoc->diInfo.GetImage( i, 0 );

    ioOut.WriteU16( pLetter->GetWidth() );
    ioOut.WriteU16( pLetter->GetHeight() );
    ioOut.WriteBlock( pLetter->GetData(), 3 * pLetter->GetWidth() * pLetter->GetHeight() );
  }
  ioOut.Close();

  return TRUE;
}



BOOL SaveFNX( CPainterFontDoc *pDoc, const GR::Char* File )
{
  WORD            wDummy;

  unsigned char   ucDummy,
                  ucBuffer[2048];

  int             i;


  GR::IO::FileStream    ioOut;

  if ( !ioOut.Open( File, IIOStream::OT_WRITE_ONLY ) )
  {
    MessageBoxA( NULL, "Konnte Datei nicht zum Schreiben öffnen.", "Fehler", MB_OK | MB_APPLMODAL );
    return FALSE;
  }

  wsprintf( ( GR::WChar* )ucBuffer, _T( "(C)Invisible Bit Font" ) );
  ucBuffer[21] = 0;
  ucBuffer[22] = 26;
  ioOut.WriteBlock( ucBuffer, 23 );
  wDummy = 256;


  ioOut.WriteU16( 256 ); // Anzahl Zeichen
  ioOut.WriteU16( 256 ); // max. Breite
  ioOut.WriteU16( 256 ); // max. Höhe
  ioOut.WriteU16( 256 ); // max. Größe

  ucDummy = (BYTE)pDoc->diInfo.ToLocalColor( CSettings::CO_WORKCOLOR );
  ioOut.WriteU8( ucDummy );
  for ( i = 0; i < 256; i++ )
  {
    GR::Graphic::Image*   pLetter = pDoc->diInfo.GetImage( i, 0 );

    ioOut.WriteU16( pLetter->GetWidth() );
    ioOut.WriteU16( pLetter->GetHeight() );
    ioOut.WriteBlock( pLetter->GetData(), pLetter->GetWidth() * pLetter->GetHeight() );
  }
  ioOut.Close();

  return TRUE;
}



BOOL SaveFNH( CPainterFontDoc *pDoc, const GR::Char* File )
{
  WORD            wDummy;

  unsigned char   ucDummy,
                  ucBuffer[2048];

  int             i;


  GR::IO::FileStream    ioOut;

  if ( !ioOut.Open( File, IIOStream::OT_WRITE_ONLY ) )
  {
    MessageBoxA( NULL, "Konnte Datei nicht zum Schreiben öffnen.", "Fehler", MB_OK | MB_APPLMODAL );
    return FALSE;
  }

  wsprintf( ( GR::WChar* )ucBuffer, _T( "(C)Invisible Bit Font" ) );
  ucBuffer[21] = 0;
  ucBuffer[22] = 26;
  ioOut.WriteBlock( ucBuffer, 23 );
  wDummy = 256;


  ioOut.WriteU16( 256 ); // Anzahl Zeichen
  ioOut.WriteU16( 256 ); // max. Breite
  ioOut.WriteU16( 256 ); // max. Höhe
  ioOut.WriteU16( 256 ); // max. Größe
  ucDummy = (BYTE)pDoc->diInfo.ToLocalColor( CSettings::CO_WORKCOLOR );
  ioOut.WriteU8( ucDummy );
  for ( i = 0; i < 256; i++ )
  {
    GR::Graphic::Image*   pLetter = pDoc->diInfo.GetImage( i, 0 );

    ioOut.WriteU16( pLetter->GetWidth() );
    ioOut.WriteU16( pLetter->GetHeight() );
    ioOut.WriteBlock( pLetter->GetData(), 2 * pLetter->GetWidth() * pLetter->GetHeight() );
  }
  ioOut.Close();

  return TRUE;
}



BOOL CPainterFontDoc::OnOpenDocument( LPCTSTR lpszPathName ) 
{
  POSITION        pos;


  pos = GetFirstViewPosition();
  CScrollView *pView = (CPainterFontView*)GetNextView( pos );

  pView->GetDlgItem( IDC_STATIC_FONT_INHALT )->ModifyStyle( 0, SS_OWNERDRAW );

  GR::String      filename = GR::Convert::ToUTF8( lpszPathName );
  GR::String      extension = GR::Strings::ToUpper( Path::Extension( filename ) );


  if ( extension == "FNX" )
  {
    // ein Font!
    diInfo.m_SaveType     = SAVETYPE_FNX;
    diInfo.SetSize( 600, 600 );
    diInfo.m_DocType      = DT_FONT;

    diInfo.m_BitDepth = 8;

    GR::Font    Font;

    Font.LoadFNT( filename.c_str(), 8 );

    for ( int i = 0; i < 256; i++ )
    {
      diInfo.AddFrame();

      *diInfo.GetPalette( diInfo.CurrentFrame() ) = theApp.m_PainterPalette;
      
      diInfo.AddLayer( new GR::Graphic::Image( Font.GetLetter( i ) ), 
                       diInfo.AddFrame() );
    }

    SetTitle( _T( "Neues Dokument" ) );

    diInfo.CurrentFrame( 65 );
  }
  else if ( extension == "FNH" )
  {
    // ein HiColor-Font!
    diInfo.m_SaveType = SAVETYPE_FNH;
    diInfo.SetSize( 600, 600 );
    diInfo.m_DocType      = DT_FONT;

    diInfo.m_BitDepth = 16;

    GR::Font    Font;

    Font.LoadFNT( filename.c_str(), 16 );

    for ( int i = 0; i < 256; i++ )
    {
      if ( Font.GetLetter( i ) )
      {
        diInfo.AddLayer( new GR::Graphic::Image( Font.GetLetter( i ) ), 
                         diInfo.AddFrame() );
      }
      else
      {
        diInfo.AddFrame();
      }
    }

    SetTitle( _T( "Neues Dokument" ) );

    diInfo.CurrentFrame( 65 );
  }
  else if ( extension == "IGF" )  
  {
    // ein IGF-Font!
    diInfo.m_SaveType = SAVETYPE_IGF;
    diInfo.SetSize( 600, 600 );
    diInfo.m_DocType      = DT_FONT;

    GR::Font    Font;

    Font.Load( filename.c_str() );

    diInfo.m_BitDepth = (GR::u8)Font.GetDepth();

    for ( int i = 0; i < 256; i++ )
    {
      size_t    iFrame = diInfo.AddFrame();
      diInfo.m_LayeredFrames[iFrame].Palette = GR::Graphic::Palette::AlphaPalette();

      GR::Graphic::Image*   pLetter = Font.GetLetter( i );
      if ( pLetter )
      {
        diInfo.AddLayer( new GR::Graphic::Image( pLetter ), iFrame );

        if ( diInfo.m_BitDepth == 32 )
        {
          // Spezial-Wurst Font mit Alpha-Maske
          GR::Graphic::Image*   pMask = new GR::Graphic::Image( pLetter->GetWidth(), pLetter->GetHeight(), 8 );

          for ( int iX = 0; iX < pLetter->GetWidth(); ++iX )
          {
            for ( int iY = 0; iY < pLetter->GetHeight(); ++iY )
            {
              pMask->SetPixel( iX, iY, pLetter->GetPixel( iX, iY ) >> 24 );
            }
          }
          diInfo.GetLayer( iFrame, 0 )->SetLayerMask( pMask );
        }
      }
    }

    SetTitle( _T( "Neues Dokument" ) );

    diInfo.CurrentFrame( 65 );
  }

  pView->OnInitialUpdate();

  diInfo.m_FileName = filename;

	return TRUE;
}



void CPainterFontDoc::OnCloseDocument() 
{
  if ( theApp.pMainFrame->m_SessionEnding )
  {
    return;
  }
	CDocument::OnCloseDocument();
}



BOOL CPainterFontDoc::OnSaveDocument( LPCTSTR lpszPathName ) 
{
  GR::String        fileName = GR::Convert::ToUTF8( lpszPathName );

  if ( diInfo.m_SaveType == SAVETYPE_UNKNOWN )
  {
    AfxMessageBox( _T( "Unbekannter Filetyp!" ) );
    return FALSE;
  }

  if ( GR::Strings::EndsWith( fileName, " *" ) )
  {
    fileName = fileName.substr( 0, fileName.length() - 2 );
  }

  GR::String    extension = Path::Extension( fileName );

  if ( extension.empty() )
  {
    // keine Extension dran, eine vom Typ abhängige Extension anhängen
    if ( diInfo.m_SaveType == SAVETYPE_FNH )
    {
      fileName += ".fnh";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_FNXL )
    {
      fileName += ".fnl";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_FNX )
    {
      fileName += ".fnx";
    }
    else if ( diInfo.m_SaveType == SAVETYPE_IGF )
    {
      fileName += ".igf";
    }
  }

  if ( ( diInfo.Width() == 0 )
  ||   ( diInfo.Height() == 0 ) )
  {
    // BAUSTELLE!
    diInfo.SetSize( 10, 10 );
  }
  if ( diInfo.m_SaveType == SAVETYPE_FNX )
  {
    // Die Datei ist ein Font...
    SaveFNX( this, fileName.c_str() );
    diInfo.SetModify( FALSE );
  }
  else if ( diInfo.m_SaveType == SAVETYPE_FNH )
  {
    // Die Datei ist ein Font...
    SaveFNH( this, fileName.c_str() );
    diInfo.SetModify( FALSE );
  }
  else if ( diInfo.m_SaveType == SAVETYPE_FNXL )
  {
    // Die Datei ist ein Font...
    SaveFNXL( this, fileName.c_str() );
    diInfo.SetModify( FALSE );
  }
  else if ( diInfo.m_SaveType == SAVETYPE_IGF )
  {
    // Die Datei ist ein Font...
    GR::Font         aFont;

    // TODO - 256-Beschränkung raus
    for ( int i = 0; i < 256; ++i )
    {
      GR::Graphic::Image*   pLetter = diInfo.GetImage( i, 0 );

      if ( pLetter )
      {
        GR::Graphic::Image*   pNewLetter = new GR::Graphic::Image( pLetter );

        if ( diInfo.m_BitDepth == 32 )
        {
          // Spezial-Wurst Font mit Alpha-Maske
          GR::Graphic::Image*   pMask = diInfo.GetMask( i, 0 );

          if ( ( pMask )
          &&   ( pMask->GetWidth() == pNewLetter->GetWidth() )
          &&   ( pMask->GetHeight() == pNewLetter->GetHeight() ) )
          {
            for ( int iX = 0; iX < pNewLetter->GetWidth(); ++iX )
            {
              for ( int iY = 0; iY < pNewLetter->GetHeight(); ++iY )
              {
                pNewLetter->SetPixel( iX, iY, ( pNewLetter->GetPixel( iX, iY ) & 0x00ffffff ) | ( pMask->GetPixel( iX, iY ) << 24 ) );
              }
            }
          }
        }
        aFont.SetLetter( i, pNewLetter );
      }
    }
    aFont.Save( fileName.c_str() );
    diInfo.SetModify( FALSE );
  }
  else
  {
    // Die Datei hat wohl ein unbekanntes Format...
    AfxMessageBox( _T( "Unbekanntes Dateiformat" ) );
  }
	return TRUE;
}



void CPainterFontDoc::OnConvertTo16bpp() 
{

  if ( ( diInfo.m_BitDepth == 15 )
  ||   ( diInfo.m_BitDepth == 16 ) )
  {
    // muß nicht umgewandelt werden!
    return;
  }
  diInfo.ConvertTo16bpp();
	
}



/*-OnConvertTo24bpp-----------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CPainterFontDoc::OnConvertTo24bpp() 
{

  if ( diInfo.m_BitDepth == 24 )
  {
    // muß nicht umgewandelt werden!
    return;
  }
  diInfo.ConvertTo24bpp();
	
}



/*-OnConvertTo32bpp-----------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CPainterFontDoc::OnConvertTo32bpp() 
{

  if ( diInfo.m_BitDepth == 32 )
  {
    // muß nicht umgewandelt werden!
    return;
  }
  diInfo.ConvertTo32bpp();
	
}



void CPainterFontDoc::OnConvertTo8bpp() 
{

  if ( diInfo.m_BitDepth == 8 )
  {
    // muß nicht umgewandelt werden!
    return;
  }
  diInfo.ConvertTo8bpp();
	
}



void CPainterFontDoc::OnSpezialInvertieren()
{

  diInfo.InvertColor();

}



void CPainterFontDoc::OnBearbeitenExportieren()
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

          cdFullImage.Rectangle( 0, iY - 1, cdImage.Width() + 2, cdImage.Height() + 2, 0xffff00ff );

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



void CPainterFontDoc::OnBearbeitenImportieren()
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



void CPainterFontDoc::OnBearbeitenResize()
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
