#include "stdafx.h"
#include "Import.h"
#include "ImagePackage.h"
#include "MaskedContextDescriptor.h"
#include "PainterGfxTools.h"
#include "Settings.h"
#include "Painter.h"
#include "DlgFileOpenPreview.h"

#include <Grafik\ContextDescriptor.h>

#include <String/Convert.h>

#include <IO/FileStream.h>
#include <Misc/Misc.h>

#include <map>


extern CPainterApp  theApp;



HBITMAP CreateBitmapFromImageData( GR::Graphic::ImageData* pData )
{

  if ( pData == NULL )
  {
    return NULL;
  }

  GR::Graphic::Image  Image( *pData );

  if ( Image.GetDepth() == 0 )
  {
    return NULL;
  }

  return CreateBitmapFromImage( &Image, &pData->Palette() );

}



HBITMAP CreateBitmapFromImage( GR::Graphic::Image* pImage, GR::Graphic::Palette* pPalette )
{

  if ( pImage == NULL )
  {
    return NULL;
  }

  if ( ( pPalette == NULL )
  &&   ( pImage->GetDepth() <= 8 ) )
  {
    pPalette = &theApp.m_PainterPalette;
  }

  GR::Graphic::Image            ImageTemp( pImage->GetWidth(), pImage->GetHeight(), 24 );

  GR::Graphic::ContextDescriptor    cdSource( pImage, pPalette );
  GR::Graphic::ContextDescriptor    cdTarget( &ImageTemp );

  cdSource.ConvertInto( &cdTarget );

  HBITMAP         hTempBitmap   = NULL;

  BITMAPINFO      bminfo;

  bminfo.bmiHeader.biSize         = sizeof( BITMAPINFOHEADER );
  bminfo.bmiHeader.biPlanes       = 1;
  bminfo.bmiHeader.biBitCount     = 24;
  bminfo.bmiHeader.biCompression  = BI_RGB;
  bminfo.bmiHeader.biSizeImage    = 0;
  bminfo.bmiHeader.biClrUsed      = 0;
  bminfo.bmiHeader.biClrImportant = 0;
  bminfo.bmiHeader.biWidth        = pImage->GetWidth();
  bminfo.bmiHeader.biHeight       = - (signed long)pImage->GetHeight();   // minus, damit es TOP-DOWN ist

  HDC     hdcTemp = CreateCompatibleDC( NULL );
  BYTE    *pData = NULL;

  hTempBitmap = CreateDIBSection( hdcTemp, &bminfo, DIB_PAL_COLORS, (void**)&pData, NULL, 0 );

  for ( int j = 0; j < pImage->GetHeight(); j++ )
  {
    memcpy( pData, cdTarget.GetRowData( j ), pImage->GetWidth() * 3 );

    DWORD   dwStep = pImage->GetWidth() * 3;

    while ( dwStep % 4 )
    {
      dwStep++;
    }
    pData += dwStep;
  }

  DeleteDC( hdcTemp );

  return hTempBitmap;

}



CPainterFileOpen *OpenFileDialog( DWORD dwFileTypes )
{

  CPainterFileOpen*                   pcfDlg;

  GR::tString                          strFormate,
                                      strFormatListe;

  std::map<GR::tString,GR::tString>   mapFormate;


  strFormate = _T( "Unterstützte Formate|" );


  if ( dwFileTypes & CSettings::FT_IMAGE )
  {
    mapFormate[_T( "*.PSD" )] = _T( "PhotoShop Files" );

    mapFormate[_T( "*.PNG" )] = _T( "PNG Files" );

    mapFormate[_T( "*.JPG;*.JPEG" )] = _T( "JPEG Files" );

    mapFormate[_T( "*.IGF" )] = _T( "IGF Files" );

    mapFormate[_T( "*.PNT" )] = _T( "Painter Files" );

    mapFormate[_T( "*.BTN" )] = _T( "BTN Files" );

    mapFormate[_T( "*.BTH" )] = _T( "BTH Files" );

    mapFormate[_T( "*.GIF" )] = _T( "GIF Files" );

    mapFormate[_T( "*.PCX" )] = _T( "PCX Files" );

    mapFormate[_T( "*.TGA" )] = _T( "Targa Files" );

    mapFormate[_T( "*.DDS" )] = _T( "DDS Files" );

    mapFormate[_T( "*.BMP" )] = _T( "Bitmap Files" );

    mapFormate[_T( "*.ICO" )] = _T( "Icon Files" );
    mapFormate[_T( "*.CUR" )] = _T( "Cursor Files" );

    mapFormate[_T( "*.SHP" )] = _T( "SHP Files" );
  }
  if ( dwFileTypes & CSettings::FT_ANIMATION )
  {
    mapFormate[_T( "*.ANX" )] = _T( "ANX Files" );

    mapFormate[_T( "*.ANH" )] = _T( "ANH Files" );

    mapFormate[_T( "*.IGF" )] = _T( "IGF Files" );
  }
  if ( dwFileTypes & CSettings::FT_FONT )
  {
    mapFormate[_T( "*.FNX" )] = _T( "FNX Files" );

    mapFormate[_T( "*.FNH" )] = _T( "FNH Files" );

    mapFormate[_T( "*.FNL" )] = _T( "FNL Files" );

    mapFormate[_T( "*.IGF" )] = _T( "IGF Files" );

  }

  std::map<GR::tString,GR::tString>::iterator   it( mapFormate.begin() );

  while ( it != mapFormate.end() )
  {
    strFormate += it->first + _T( ";" );
    strFormatListe += it->second;
    strFormatListe += _T( "(" ) + it->first;
    strFormatListe += _T( ")|" );
    strFormatListe += it->first;
    strFormatListe += _T( "|" );

    it++;
  }

  // abschließendes Semikolon entfernen
  if ( strFormate.length() )
  {
    strFormate.resize( strFormate.length() - 1 );
    strFormate += _T( "|" );
  }
  strFormatListe += _T( "All Files (*.*)|*.*||\0\0" );

  strFormate += strFormatListe;

  pcfDlg = new CPainterFileOpen( TRUE, NULL, NULL, 
                            OFN_ENABLESIZING | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
                            strFormate.c_str(),
                            AfxGetMainWnd() );

  return pcfDlg;

}



void DWReverse( DWORD& dwValue )
{

  DWORD   dwResult =  ( ( dwValue & 0xff000000 ) >> 24 )
                    + ( ( dwValue & 0x00ff0000 ) >> 8 )
                    + ( ( dwValue & 0x0000ff00 ) << 8 )
                    + ( ( dwValue & 0x000000ff ) << 24 );

  dwValue = dwResult;

}

void iReverse( int& dwValue )
{

  int   dwResult =  ( ( dwValue & 0xff000000 ) >> 24 )
                    + ( ( dwValue & 0x00ff0000 ) >> 8 )
                    + ( ( dwValue & 0x0000ff00 ) << 8 )
                    + ( ( dwValue & 0x000000ff ) << 24 );

  dwValue = dwResult;

}



void WReverse( WORD& wValue )
{

  WORD   wResult =  ( ( wValue & 0xff00 ) >> 8 )
                  + ( ( wValue & 0x00ff ) << 8 );

  wValue = wResult;

}



void SHReverse( short& wValue )
{

  short   wResult =  ( ( wValue & 0xff00 ) >> 8 )
                  + ( ( wValue & 0x00ff ) << 8 );

  wValue = wResult;

}



    struct tChannelInfo
    {
      short shChannelId;
      DWORD dwDataLength;
    };

    struct tLayer
    {
      int     dwLayerLeft,
              dwLayerTop,
              dwLayerRight,
              dwLayerBottom;

      WORD    wNumberChannels;
      
      GR::String   m_Name;

      std::vector<tChannelInfo>   vectChannels;
    };


bool LoadPSD( DocumentInfo* pDocInfo, const GR::Char* FileName )
{
  GR::IO::FileStream    aFile;

  if( !aFile.Open( FileName ) )
  {
    return NULL;
  }

#pragma pack(1)
  struct tPSDHeader
  {
    DWORD   dwSignature;
    WORD    wVersion;
    BYTE    ucReserved[6];
    WORD    wChannels;
    DWORD   dwHeight;
    DWORD   dwWidth;
    WORD    wDepth;
    WORD    wMode;
  };
#pragma pack()

  tPSDHeader    psdHeader;

  aFile.ReadBlock( &psdHeader, sizeof( psdHeader ) );

  DWReverse( psdHeader.dwSignature );
  DWReverse( psdHeader.dwWidth );
  DWReverse( psdHeader.dwHeight );
  WReverse( psdHeader.wVersion );
  WReverse( psdHeader.wChannels );
  WReverse( psdHeader.wDepth );
  WReverse( psdHeader.wMode );

  if ( psdHeader.wMode != 3 )
  {
    AfxMessageBox( _T( "Im Moment werden nur RGB-PSD-Dateien unterstützt!" ) );
    return false;
  }

  pSettings->Log( CMisc::printf( "%c%c%c%c V %d CH %d W %d H %d D %d M %d", 
                  ( psdHeader.dwSignature & 0xff000000 ) >> 24, 
                  ( psdHeader.dwSignature & 0x00ff0000 ) >> 16, 
                  ( psdHeader.dwSignature & 0x0000ff00 ) >> 8, 
                  ( psdHeader.dwSignature & 0x000000ff ), 
                  psdHeader.wVersion,
                  psdHeader.wChannels,
                  psdHeader.dwWidth,
                  psdHeader.dwHeight,
                  psdHeader.wDepth,
                  psdHeader.wMode ) );

  // Color-Mode-Data
  DWORD   dwBlockLength;

  dwBlockLength = aFile.ReadU32();
  DWReverse( dwBlockLength );
  pSettings->Log( CMisc::printf( "color mode data size %d", dwBlockLength ) );
  aFile.SetPosition( dwBlockLength, IIOStream::PT_CURRENT );

  // Image-Resources
  dwBlockLength = aFile.ReadU32();
  DWReverse( dwBlockLength );
  pSettings->Log( CMisc::printf( "image resources data size %d", dwBlockLength ) );

  while ( dwBlockLength )
  {
    DWORD   dwBim = aFile.ReadU32(); // 8BIM
    DWReverse( dwBim );

    pSettings->Log( CMisc::printf( "BIM %c%c%c%c", 
                  ( dwBim & 0xff000000 ) >> 24, 
                  ( dwBim & 0x00ff0000 ) >> 16, 
                  ( dwBim & 0x0000ff00 ) >> 8, 
                  ( dwBim & 0x000000ff ) ) );

    WORD    wId = aFile.ReadU16(); // ID
    WReverse( wId );
    pSettings->Log( CMisc::printf( "ID %d", wId ) );

    int iLength = aFile.ReadU8();
    if ( iLength )
    {
      pSettings->Log( CMisc::printf( "PString %d (padded)", iLength ) );
      std::string   strGnu = "";
      for ( int i = 0; i < iLength; i++ )
      {
        strGnu += (char)aFile.ReadU8();
      }
      iLength++; // das Längen-Byte
      if ( iLength % 2 )
      {
        iLength++;
        aFile.ReadU8();
      }
      pSettings->Log( CMisc::printf( "Name %s", strGnu.c_str() ) );
    }
    else
    {
      aFile.ReadU8();
      iLength = 2;
    }
    DWORD   dwSize = aFile.ReadU32();
    DWReverse( dwSize );
    if ( dwSize % 2 )
    {
      dwSize++;
    }
    pSettings->Log( CMisc::printf( "Data length %d (padded)", dwSize ) );
    for ( DWORD i = 0; i < dwSize; i++ )
    {
      aFile.ReadU8();
    }
    
    DWORD   dwBlockSize = 4 + 2 + iLength + 4 + dwSize;
    if ( dwBlockLength >= dwBlockSize )
    {
      dwBlockLength -= dwBlockSize;
    }
    else
    {
      pSettings->Log( CMisc::printf( "INVALID BLOCK (size %d, left %d)", dwBlockSize, dwBlockLength ) );
      break;
    }
    pSettings->Log( CMisc::printf( "read resource block (size %d, left %d)", dwBlockSize, dwBlockLength ) );
  }

  // Layer und Masken
  DWORD dwMiscBlockLength = aFile.ReadU32();
  DWReverse( dwMiscBlockLength );

  pSettings->Log( CMisc::printf( "misc block size %d", dwMiscBlockLength ) );

  DWORD dwLayerInfoBlock = aFile.ReadU32();
  DWReverse( dwLayerInfoBlock );

  pSettings->Log( CMisc::printf( "layer info block size %d", dwLayerInfoBlock ) );

  if ( dwLayerInfoBlock )
  {
    short shLayerCount = (short)aFile.ReadU16();

    SHReverse( shLayerCount );
    if ( shLayerCount < 0 )
    {
      shLayerCount = abs( shLayerCount );
      pSettings->Log( CMisc::printf( "Anzahl layer %d - erster Alpha-Channel enthält transparency data for merged", shLayerCount ) );
    }
    else
    {
      pSettings->Log( CMisc::printf( "Anzahl layer %d", shLayerCount ) );
    }

    std::vector<tLayer>   vectLayer;

    for ( int iLayer = 0; iLayer < shLayerCount; iLayer++ )
    {
      tLayer    aLayer;

      aLayer.dwLayerTop = aFile.ReadU32();
      aLayer.dwLayerLeft = aFile.ReadU32();
      aLayer.dwLayerBottom = aFile.ReadU32();
      aLayer.dwLayerRight = aFile.ReadU32();

      aLayer.wNumberChannels = aFile.ReadU16();

      iReverse( aLayer.dwLayerTop );
      iReverse( aLayer.dwLayerLeft );
      iReverse( aLayer.dwLayerBottom );
      iReverse( aLayer.dwLayerRight );
      WReverse( aLayer.wNumberChannels );

      pSettings->Log( CMisc::printf( "Layer Rect %d,%d - %d,%d",
              aLayer.dwLayerLeft,
              aLayer.dwLayerTop,
              aLayer.dwLayerRight,
              aLayer.dwLayerBottom ) );


      dwLayerInfoBlock -= 18;

      pSettings->Log( CMisc::printf( "%d channels", aLayer.wNumberChannels ) );

      for ( int j = 0; j < aLayer.wNumberChannels; j++ )
      {
        tChannelInfo  stInfo;

        short shChannelId = (short)aFile.ReadU16();
        SHReverse( shChannelId );

        stInfo.shChannelId = shChannelId;

        pSettings->Log( CMisc::printf( "channelid %d", shChannelId ) );

        DWORD   dwChannelDataLength = aFile.ReadU32();
        DWReverse( dwChannelDataLength );

        stInfo.dwDataLength = dwChannelDataLength;
        pSettings->Log( CMisc::printf( "channeldatalength %d", dwChannelDataLength ) );

        aLayer.vectChannels.push_back( stInfo );

        dwLayerInfoBlock -= 6;
      }

      DWORD   dwBlendModeSignature = aFile.ReadU32();
      DWReverse( dwBlendModeSignature );

      DWORD   dwBlendModeKey = aFile.ReadU32();
      DWReverse( dwBlendModeKey );

      dwLayerInfoBlock -= 8;

      pSettings->Log( CMisc::printf( "BlendModeKey %c%c%c%c", 
                    ( dwBlendModeKey & 0xff000000 ) >> 24, 
                    ( dwBlendModeKey & 0x00ff0000 ) >> 16, 
                    ( dwBlendModeKey & 0x0000ff00 ) >> 8, 
                    ( dwBlendModeKey & 0x000000ff ) ) );

      BYTE    ucOpacity = aFile.ReadU8(),
              ucClipping = aFile.ReadU8(),
              ucFlags = aFile.ReadU8();

      aFile.ReadU8();   // Filler

      dwLayerInfoBlock -= 4;


      DWORD dwExtraDataSize = aFile.ReadU32();

      DWReverse( dwExtraDataSize );

      dwLayerInfoBlock -= 4;

      pSettings->Log( CMisc::printf( "extra data size %d", dwExtraDataSize ) );


      GR::u32    dwExtraDataStartPos = (GR::u32)aFile.GetPosition();

      /*
      for ( int g2 = 0; g2 < dwExtraDataSize + 160; g2++ )
      {
        char  cDummy = (char)aFile.ReadU8();
        if ( g2 == dwExtraDataSize )
        {
          pSettings->Log( "-------ExtraDataEnd----------------" );
        }
        pSettings->Log( CMisc::printf( "(%3d)Char %c (%d)", g2, cDummy, cDummy ) );
        
      }
      */
      

      DWORD dwLayerMaskData = aFile.ReadU32();
      DWReverse( dwLayerMaskData );

      if ( dwLayerMaskData == 14 )
      {
        DWORD   dwLayerALeft,
                dwLayerATop,
                dwLayerABottom,
                dwLayerARight;

        dwLayerATop = aFile.ReadU32();
        dwLayerALeft = aFile.ReadU32();
        dwLayerABottom = aFile.ReadU32();
        dwLayerARight = aFile.ReadU32();

        DWReverse( dwLayerATop );
        DWReverse( dwLayerALeft );
        DWReverse( dwLayerABottom );
        DWReverse( dwLayerARight );

        pSettings->Log( CMisc::printf( "Layer Adjustment Rect %d,%d - %d,%d",
                dwLayerALeft,
                dwLayerATop,
                dwLayerARight,
                dwLayerABottom ) );

        aFile.ReadU8();    // Default color
        aFile.ReadU8();    // flags
        aFile.ReadU16();   // Padding

        //aFile.SetPosition( dwLayerMaskData, FILE_CURRENT );
      }

      pSettings->Log( CMisc::printf( "dwLayerMaskData size %d", dwLayerMaskData ) );


      

      DWORD dwLayerBlendingRangesData = aFile.ReadU32();
      DWReverse( dwLayerBlendingRangesData );

      aFile.SetPosition( dwLayerBlendingRangesData, IIOStream::PT_CURRENT );

      BYTE  ucNameLength = aFile.ReadU8();

      GR::String   strChannelName;
      for ( int u = 0; u < ucNameLength; u++ )
      {
        strChannelName += (char)aFile.ReadU8();
      }
      pSettings->Log( CMisc::printf( "Layer Name %s (%d/%d)", strChannelName.c_str(), strChannelName.length(), ucNameLength ) );
      aLayer.m_Name = GR::Convert::ToString( strChannelName );

      ucNameLength++;
      while ( ucNameLength % 4 )
      {
        ucNameLength++;
        aFile.ReadU8();
      }

      aFile.SetPosition( dwExtraDataStartPos + dwExtraDataSize, IIOStream::PT_SET );

      /*

      // Adjustment Layer
      DWORD   dwAdjustmentSignature = aFile.ReadU32();
      DWReverse( dwAdjustmentSignature );

      pSettings->Log( CMisc::printf( "dwAdjustmentSignature %c%c%c%c", 
                    ( dwAdjustmentSignature & 0xff000000 ) >> 24, 
                    ( dwAdjustmentSignature & 0x00ff0000 ) >> 16, 
                    ( dwAdjustmentSignature & 0x0000ff00 ) >> 8, 
                    ( dwAdjustmentSignature & 0x000000ff ) ) );

      DWORD   dwAdjustmentKey = aFile.ReadU32();
      DWReverse( dwAdjustmentKey );

      pSettings->Log( CMisc::printf( "dwAdjustmentKey %c%c%c%c", 
                    ( dwAdjustmentKey & 0xff000000 ) >> 24, 
                    ( dwAdjustmentKey & 0x00ff0000 ) >> 16, 
                    ( dwAdjustmentKey & 0x0000ff00 ) >> 8, 
                    ( dwAdjustmentKey & 0x000000ff ) ) );
      DWORD   dwAdjustmentLength = aFile.ReadU32();
      DWReverse( dwAdjustmentLength );

      pSettings->Log( CMisc::printf( "Adjustment Length %d", dwAdjustmentLength ) );

      aFile.SetPosition( dwAdjustmentLength, FILE_CURRENT );

      dwExtraDataSize -= dwAdjustmentLength + 12;

      dwLayerInfoBlock -= dwAdjustmentLength + 12;

      pSettings->Log( CMisc::printf( "noch übrig %d", dwLayerInfoBlock ) );
      */

      /*
        //dwExtraDataSize -= 4;

      //aFile.SetPosition( dwOrigExtraSize, FILE_CURRENT );
    
      // eine layer-id?
      DWORD dwGnu = aFile.ReadU32();
      DWReverse( dwGnu );
      dwExtraDataSize -= 4;
      dwGnu = aFile.ReadU32();
      DWReverse( dwGnu );
      pSettings->Log( CMisc::printf( "Layer-Id %c%c%c%c", 
                    ( dwGnu & 0xff000000 ) >> 24, 
                    ( dwGnu & 0x00ff0000 ) >> 16, 
                    ( dwGnu & 0x0000ff00 ) >> 8, 
                    ( dwGnu & 0x000000ff ) ) );
      dwExtraDataSize -= 4;
      aFile.ReadU32();
      aFile.ReadU32();
      dwExtraDataSize -= 8;

      // fummel clbl?
      dwGnu = aFile.ReadU32();
      DWReverse( dwGnu );
      pSettings->Log( CMisc::printf( "BlendClipping %c%c%c%c", 
                    ( dwGnu & 0xff000000 ) >> 24, 
                    ( dwGnu & 0x00ff0000 ) >> 16, 
                    ( dwGnu & 0x0000ff00 ) >> 8, 
                    ( dwGnu & 0x000000ff ) ) );

      dwGnu = aFile.ReadU32();
      DWReverse( dwGnu );
      DWORD dwCLBLLength = aFile.ReadU32();
      DWReverse( dwCLBLLength );
      aFile.SetPosition( dwCLBLLength, FILE_CURRENT );

      pSettings->Log( CMisc::printf( "dwCLBLLength Length %d", dwCLBLLength ) );

    


      // infx
      dwGnu = aFile.ReadU32();
      DWReverse( dwGnu );
      dwGnu = aFile.ReadU32();
      DWReverse( dwGnu );
      DWORD dwInfoLength = aFile.ReadU32();
      DWReverse( dwInfoLength );
      aFile.SetPosition( dwInfoLength, FILE_CURRENT );

      dwGnu = aFile.ReadU32();
      DWReverse( dwGnu );
      dwGnu = aFile.ReadU32();
      DWReverse( dwGnu );
      pSettings->Log( CMisc::printf( "dwGnu %c%c%c%c", 
                    ( dwGnu & 0xff000000 ) >> 24, 
                    ( dwGnu & 0x00ff0000 ) >> 16, 
                    ( dwGnu & 0x0000ff00 ) >> 8, 
                    ( dwGnu & 0x000000ff ) ) );

      //aFile.SetPosition( dwExtraDataSize, FILE_CURRENT );

      pSettings->Log( CMisc::printf( "Extra Data left %d", dwExtraDataSize ) );
      */


      vectLayer.push_back( aLayer );
    }


    for ( size_t iLayer2 = 0; iLayer2 < vectLayer.size(); iLayer2++ )
    {
      tLayer& aLayer = vectLayer[iLayer2];
      GR::Graphic::Image*  pImageMask = NULL;

      GR::Graphic::Image*  pImage = NULL;

      int     iLayerXOffset = 0,
              iLayerYOffset = 0;


      for ( int iChannels = 0; iChannels < aLayer.wNumberChannels; iChannels++ )
      {
        short  ucCompression = (short)aFile.ReadU16();

        SHReverse( ucCompression );

        pSettings->Log( CMisc::printf( "=== LAYER %s CHANNEL %d (ID %d)", aLayer.m_Name.c_str(), iChannels, aLayer.vectChannels[iChannels].shChannelId ) );
        pSettings->Log( CMisc::printf( "%d statt %d Breite", aLayer.dwLayerRight - aLayer.dwLayerLeft, psdHeader.dwWidth ) );
        pSettings->Log( CMisc::printf( "%d statt %d Höhe", aLayer.dwLayerBottom - aLayer.dwLayerTop, psdHeader.dwHeight ) );


        pSettings->Log( CMisc::printf( "image data compression %d", ucCompression ) );

        pSettings->Log( CMisc::printf( "layerrect %d,%d - %d,%d", 
                                    aLayer.dwLayerLeft,
                                    aLayer.dwLayerTop,
                                    aLayer.dwLayerRight,
                                    aLayer.dwLayerBottom ) );

        // BAUSTELLE - Images, die größer als das Dokument sind!!

        BYTE* pData = NULL;

        if ( aLayer.vectChannels[iChannels].shChannelId == -1 )
        {
          //pImage = new GR::Graphic::Image( psdHeader.dwWidth, psdHeader.dwHeight, 8 );
          pImage = new GR::Graphic::Image( aLayer.dwLayerRight - aLayer.dwLayerLeft, 
                                  aLayer.dwLayerBottom - aLayer.dwLayerTop,
                                  8 );
          pData = (BYTE*)pImage->GetData();
          pImageMask = pImage;
          //pData += ( aLayer.dwLayerLeft + aLayer.dwLayerTop * psdHeader.dwWidth );
        }
        else if ( aLayer.vectChannels[iChannels].shChannelId == 0 )
        {
          //pImage = new GR::Graphic::Image( psdHeader.dwWidth, psdHeader.dwHeight, 24 );
          pImage = new GR::Graphic::Image( aLayer.dwLayerRight - aLayer.dwLayerLeft, 
                                  aLayer.dwLayerBottom - aLayer.dwLayerTop,
                                  24 );
          pData = (BYTE*)pImage->GetData();
          //pData += ( aLayer.dwLayerLeft + aLayer.dwLayerTop * psdHeader.dwWidth ) * 3;
        }
        else
        {
          if ( pImage == NULL )
          {
            pSettings->Log( CMisc::printf( "Kein IMAGE bei Channel %d", aLayer.vectChannels[iChannels].shChannelId ) );
          }
          else
          {
            pData = (BYTE*)pImage->GetData();
          }
          //pData += ( aLayer.dwLayerLeft + aLayer.dwLayerTop * psdHeader.dwWidth ) * 3;
        }

        if ( aLayer.vectChannels[iChannels].shChannelId == 0 )
        {
          pData += 2;
        }
        if ( aLayer.vectChannels[iChannels].shChannelId == 1 )
        {
          pData += 1;
        }

        int   iRowLength = 0;
        BYTE  bByte = 0;

        if ( ucCompression == 1 )
        {
          // Anfangs in "shorts" die Anzahl der Byte-Counts
          aFile.SetPosition( ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) * 2, IIOStream::PT_CURRENT );

          int   iPixelAnzahl = ( aLayer.dwLayerRight - aLayer.dwLayerLeft );
          int   x = 0;

          for ( int y = 0; y < ( aLayer.dwLayerBottom - aLayer.dwLayerTop ); y++ )
          {
            iPixelAnzahl = ( aLayer.dwLayerRight - aLayer.dwLayerLeft );
            x = 0;
            while ( iPixelAnzahl > 0 )
            {
              char    cByte = (char)aFile.ReadU8();
              /*
              if ( cByte >= 128 )
              {
                cByte -= 256;
              }
              */
              if ( cByte < 0 )
              {
                // nächstes Byte (-n + 1) mal
                if ( cByte == -128 )
                {
                  continue;
                }
                int iCount = -(int)cByte + 1;

                BYTE  cFiller = aFile.ReadU8();
                for ( int o = 0; o < iCount; o++ )
                {
                  if ( aLayer.vectChannels[iChannels].shChannelId == -1 )
                  {
                    *pData++ = cFiller;
                  }
                  else if ( aLayer.vectChannels[iChannels].shChannelId == -2 )
                  {
                    pData++;
                  }
                  else
                  {
                    *pData++ = cFiller;
                    pData += 2;
                  }
                  iPixelAnzahl--;
                  x++;
                }
              }
              else
              {
                int iCount = cByte + 1;
                // die nächsten n + 1 Bytes direkt einfügen
                for ( int o = 0; o < iCount; o++ )
                {
                  BYTE  cFiller = aFile.ReadU8();
                  if ( aLayer.vectChannels[iChannels].shChannelId == -1 )
                  {
                    *pData++ = cFiller;
                  }
                  else if ( aLayer.vectChannels[iChannels].shChannelId == -2 )
                  {
                    pData++;
                  }
                  else
                  {
                    *pData++ = cFiller;
                    pData += 2;
                  }
                  iPixelAnzahl--;
                  x++;
                }
              }
            }
            /*
            if ( aLayer.vectChannels[iChannels].shChannelId == -1 )
            {
              pData += ( psdHeader.dwWidth - ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) );
            }
            else
            {
              pData += ( psdHeader.dwWidth - ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) ) * 3;
            }
            */
            /*
            if ( ( ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) * ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) * aLayer.wNumberChannels ) % 2 )
            {
              aFile.ReadU8();
            }
            */
          }
          /*
          if ( ( ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) * ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) ) % 2 )
          {
            aFile.ReadU8();
            pSettings->Log( "====================================" );
            pSettings->Log( "reading extra byte (compressed data)" );
            pSettings->Log( "====================================" );
          }
          */
        }
        else
        {
          if ( (DWORD)( aLayer.dwLayerRight - aLayer.dwLayerLeft ) * ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) > aLayer.vectChannels[iChannels].dwDataLength )
          {
            pSettings->Log( "-------------------------------------------" );
            pSettings->Log( "Rect bigger than Data, should be compressed!!!" );
            pSettings->Log( "-------------------------------------------" );
          }


          for ( int y = 0; y < ( aLayer.dwLayerBottom - aLayer.dwLayerTop ); y++ )
          {
            iRowLength = 0;
            for ( int x = 0; x < ( aLayer.dwLayerRight - aLayer.dwLayerLeft ); x++ )
            {
              bByte = aFile.ReadU8();
              iRowLength++;
              if ( aLayer.vectChannels[iChannels].shChannelId == -1 )
              {
                *pData++ = bByte;
              }
              else if ( aLayer.vectChannels[iChannels].shChannelId == -2 )
              {
                pData++;
              }
              else
              {
                if ( aLayer.vectChannels[iChannels].shChannelId == 0 )
                {
                  *pData = bByte;
                }
                pData += 3;
              }
            }
            /*
            if ( aLayer.vectChannels[iChannels].shChannelId == -1 )
            {
              pData += ( psdHeader.dwWidth - ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) );
            }
            else
            {
              pData += ( psdHeader.dwWidth - ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) ) * 3;
            }
            */
          }
          /*
          while ( iRowLength % 2 )
          {
            aFile.ReadU8();
            iRowLength++;
          }
          */

          pSettings->Log( CMisc::printf( "Read (%dx%d=%d Bytes should be %d Bytes", 
            ( aLayer.dwLayerRight - aLayer.dwLayerLeft ),
            ( aLayer.dwLayerBottom - aLayer.dwLayerTop ),
            ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) * ( aLayer.dwLayerBottom - aLayer.dwLayerTop ),
            aLayer.vectChannels[iChannels].dwDataLength ) );

          if ( (DWORD)( aLayer.dwLayerRight - aLayer.dwLayerLeft ) * ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) > aLayer.vectChannels[iChannels].dwDataLength )
          {
            static int iShot = 0;
            //pImage->Save( CMisc::printf( "toobig%d.igf", iShot++ ) );
          }
                    
          if ( ( ( ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) * ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) ) % 2 )
          &&   ( ( ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) * ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) ) > 1 ) )
          {
            for ( int i = 0; i < 260; i++ )
            {
              char    cDummy = (char)aFile.ReadU8();
              pSettings->Log( CMisc::printf( "%4d Char %d /%d = %c", i, cDummy, (BYTE)cDummy, cDummy ) );
            }
            aFile.ReadU8();
            pSettings->Log( "=============================" );
            pSettings->Log( "reading extra byte (raw data)" );
            pSettings->Log( "=============================" );
          }
        }

        /*
        // auf WORD padden
        if ( ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) * ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) % 2 )
        {
          aFile.ReadU8();
        }
        */

        /*
        if ( ( ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) == 1 )
        &&   ( ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) == 1 ) )
        {
          for ( int i = 0; i < 260; i++ )
          {
            char    cDummy = (char)aFile.ReadU8();
            pSettings->Log( CMisc::printf( "%4d Char %d /%d = %c", i, cDummy, (BYTE)cDummy, cDummy ) );
          }
          AfxMessageBox( "hurga" );
        }
        */


        pDocInfo->SetSize( psdHeader.dwWidth, psdHeader.dwHeight );
        pDocInfo->m_DocType = DT_IMAGE;
        pDocInfo->m_BitDepth = 24;
        if ( pDocInfo->m_BitDepth == 15 )
        {
          pDocInfo->m_BitDepth = 16;
        }

        pDocInfo->AddFrame();

        if ( aLayer.vectChannels[iChannels].shChannelId == 2 )
        {
          // das Layer-Image kann größer oder kleiner sein als das Dokument
          GR::Graphic::Image*    pImageLayer = new GR::Graphic::Image( psdHeader.dwWidth, psdHeader.dwHeight, 24 );

          GR::Graphic::ContextDescriptor  cdSource( pImage ),
                              cdTarget( pImageLayer );

          cdSource.CopyArea( 0, 0, 
                              aLayer.dwLayerRight - aLayer.dwLayerLeft, aLayer.dwLayerBottom - aLayer.dwLayerTop,
                              aLayer.dwLayerLeft, aLayer.dwLayerTop,
                              &cdTarget );

          /*
          static int iShot = 0;
          pImage->Save( CMisc::printf( "test%d.igf", iShot++ ) );
          */
          delete pImage;
          pImage = NULL;
          pDocInfo->AddLayer( pImageLayer );
          if ( pImageMask )
          {
            GR::Graphic::Image* pImageLayerMask = new GR::Graphic::Image( psdHeader.dwWidth, psdHeader.dwHeight, 8 );
            GR::Graphic::ContextDescriptor  cdSourceMask( pImageMask ),
                                cdTargetMask( pImageLayerMask );

            cdSourceMask.CopyArea( 0, 0, 
                                aLayer.dwLayerRight - aLayer.dwLayerLeft, aLayer.dwLayerBottom - aLayer.dwLayerTop,
                                aLayer.dwLayerLeft, aLayer.dwLayerTop,
                                &cdTargetMask );

            delete pImageMask;
            pImageMask = NULL;

            CLayer*     pLastLayer = pDocInfo->GetLayer( 0, pDocInfo->m_LayeredFrames[0].LayerCount() - 1 );

            pLastLayer->SetLayerMask( pImageLayerMask );
            pLastLayer->m_MaskEnabled = true;
            pLastLayer->m_Name = aLayer.m_Name;
          }
        }
        //pDocInfo->AddLayer( pImage );
      }

      /*
      if ( ( ( aLayer.dwLayerRight - aLayer.dwLayerLeft ) * ( aLayer.dwLayerBottom - aLayer.dwLayerTop ) * aLayer.wNumberChannels ) % 2 )
      {
        aFile.ReadU8();
      }
      */
      if ( pImage )
      {
        pSettings->Log( "======== IMAGE NOCH DA!" );
        delete pImage;
        pImage = NULL;
      }
    }
  }
  else
  {
    pSettings->Log( "keine layerinfo" );

    aFile.SetPosition( dwMiscBlockLength - 4, IIOStream::PT_CURRENT );
    short  ucCompression = (short)aFile.ReadU16();

    SHReverse( ucCompression );
    pSettings->Log( CMisc::printf( "image data compression %d", ucCompression ) );

    if ( ucCompression == 1 )
    {
      aFile.SetPosition( psdHeader.dwHeight * 2 * psdHeader.wChannels, IIOStream::PT_CURRENT );
    }

    GR::Graphic::Image* pImage = new GR::Graphic::Image( psdHeader.dwWidth, psdHeader.dwHeight, 24 );

    for ( int i = 0; i < psdHeader.wChannels; i++ )
    {
      BYTE* pData = (BYTE*)pImage->GetData();

      if ( i == 0 )
      {
        pData += 2;
      }
      if ( i == 1 )
      {
        pData += 1;
      }


      int   iRowLength = 0;
      BYTE  bByte = 0;

      if ( ucCompression == 1 )
      {
        int   iPixelAnzahl = psdHeader.dwWidth;

        for ( DWORD y = 0; y < psdHeader.dwHeight; y++ )
        {
          iPixelAnzahl = psdHeader.dwWidth;
          while ( iPixelAnzahl > 0 )
          {
            char    cByte = (char)aFile.ReadU8();
            /*
            if ( cByte >= 128 )
            {
              cByte -= 256;
            }
            */
            if ( cByte < 0 )
            {
              // nächstes Byte (-n + 1) mal
              if ( cByte == -128 )
              {
                continue;
              }
              int iCount = -(int)cByte + 1;

              BYTE  cFiller = aFile.ReadU8();
              for ( int o = 0; o < iCount; o++ )
              {
                *pData++ = cFiller;
                pData += 2;
                iPixelAnzahl--;
              }
            }
            else
            {
              int iCount = cByte + 1;
              // die nächsten n + 1 Bytes direkt einfügen
              for ( int o = 0; o < iCount; o++ )
              {
                BYTE  cFiller = aFile.ReadU8();
                *pData++ = cFiller;
                pData += 2;
                iPixelAnzahl--;
              }
            }
          }
          if ( psdHeader.dwWidth % 2 )
          {
            aFile.ReadU8();
          }
        }

      }
      else
      {
        for ( DWORD y = 0; y < psdHeader.dwHeight; y++ )
        {
          iRowLength = 0;
          for ( DWORD x = 0; x < psdHeader.dwWidth; x++ )
          {
            bByte = aFile.ReadU8();
            iRowLength++;
            *pData = bByte;
            pData += 3;
          }
          while ( iRowLength % 2 )
          {
            aFile.ReadU8();
            iRowLength++;
          }
        }
      }
    }

    pDocInfo->SetSize( pImage->GetWidth(), pImage->GetHeight() );
    pDocInfo->m_DocType     = DT_IMAGE;
    pDocInfo->m_BitDepth  = pImage->GetDepth();
    if ( pDocInfo->m_BitDepth == 15 )
    {
      pDocInfo->m_BitDepth = 16;
    }

    pDocInfo->AddLayer( pImage );
  }

  aFile.Close();

  return true;

}