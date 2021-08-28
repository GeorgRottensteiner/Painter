#include ".\formatmanager.h"

#include "stdafx.h"

#include <Grafik/ImageFormate/FormatIGF.h>
#include <Grafik/ImageFormate/FormatJPG.h>
#include <Grafik/ImageFormate/FormatGIF.h>
#include <Grafik/ImageFormate/FormatBMP.h>
#include <Grafik/ImageFormate/FormatPCX.h>
#include <Grafik/ImageFormate/FormatTGA.h>
#include <Grafik/ImageFormate/FormatBTN.h>
#include <Grafik/ImageFormate/FormatPNG.h>
#include <Grafik/ImageFormate/FormatIcon.h>
#include <Grafik/ImageFormate/FormatCur.h>

#include <String/Convert.h>

#include <Misc/Misc.h>

#include "FreeImage.h"
#include "PainterGFXTools.h"
#include "JPegSaveOptions.h"
#include "ColorReducer.h"

#include "DlgSaveIGF.h"
#include "DlgGIFOptions.h"



CFormatManager::CFormatManager()
{
  /*
  SAVETYPE_UNKNOWN = 0,
  SAVETYPE_BTH,
  SAVETYPE_FNH,
  SAVETYPE_FNX,
  SAVETYPE_ANX,
  SAVETYPE_ANH,
  SAVETYPE_FNXL,
  SAVETYPE_PNT,
  SAVETYPE_PNG,
  SAVETYPE_ICON,
  */

  m_mapSupportedFormats[SAVETYPE_IGF]   = tFileFormatSupport( "IGF", "igf", &globalIGFPlugin, &globalIGFPlugin );
  m_mapSupportedFormats[SAVETYPE_JPEG]  = tFileFormatSupport( "JPeg", "jpg", &globalJPGPlugin, -1, NULL, FIF_JPEG );
  m_mapSupportedFormats[SAVETYPE_GIF]   = tFileFormatSupport( "GIF", "gif", &globalGIFPlugin, &globalGIFPlugin );
  m_mapSupportedFormats[SAVETYPE_BMP]   = tFileFormatSupport( "BMP Windows Bitmap", "bmp", &globalBMPPlugin, &globalBMPPlugin );
  m_mapSupportedFormats[SAVETYPE_PCX]   = tFileFormatSupport( "PCX", "pcx", &globalPCXPlugin, &globalPCXPlugin );
  m_mapSupportedFormats[SAVETYPE_TGA]   = tFileFormatSupport( "TGA", "tga", &globalTGAPlugin, &globalTGAPlugin );
  m_mapSupportedFormats[SAVETYPE_BTN]   = tFileFormatSupport( "BTN", "btn", &globalBTNPlugin, &globalBTNPlugin );
  m_mapSupportedFormats[SAVETYPE_CURSOR] = tFileFormatSupport( "CUR", "cur", &globalCursorPlugin, &globalCursorPlugin );
  //m_mapSupportedFormats[SAVETYPE_PNG]   = tFileFormatSupport( "PNG", "png", &globalPNGPlugin, &globalPNGPlugin );
}



CFormatManager::~CFormatManager()
{
}



CFormatManager& CFormatManager::Instance()
{

  static CFormatManager   g_Instance;

  return g_Instance;

}



ImageSet* CFormatManager::Load( SaveType& saveType, const GR::Char* FileName )
{
  tMapSupportedFormats::iterator    it( m_mapSupportedFormats.begin() );
  while ( it != m_mapSupportedFormats.end() )
  {
    tFileFormatSupport&   Format = it->second;

    if ( Format.m_pLoadingFormat )
    {
      if ( Format.m_pLoadingFormat->IsFileOfType( FileName ) )
      {
        ImageSet*  pSet = Format.m_pLoadingFormat->LoadSet( FileName );

        if ( pSet )
        {
          saveType = it->first;
          return pSet;
        }
      }
    }

    ++it;
  }
  return NULL;
}



bool CFormatManager::Save( const SaveType saveType, const GR::Char* FileName, ImageSet* pSet )
{
  tMapSupportedFormats::iterator    it( m_mapSupportedFormats.find( saveType ) );
  if ( it == m_mapSupportedFormats.end() )
  {
    return false;
  }

  tFileFormatSupport&   Format = it->second;

  if ( Format.m_pSavingFormat )
  {
    if ( Format.m_pSavingFormat->CanSave( pSet ) )
    {
      if ( ( pSet->FrameCount() == 1 )
      &&   ( pSet->Frames[0]->Layers.size() == 1 )
      &&   ( saveType == SAVETYPE_IGF ) )
      {
        CDlgSaveIGF   dlgIGF;

        if ( pSet->Image( 0, 0 )->BitsProPixel() <= 8 )
        {
          dlgIGF.m_SaveMitPalette = true;
        }
        else
        {
          dlgIGF.m_HidePaletteCheckBox = true;
        }
        if ( dlgIGF.DoModal() != IDOK )
        {
          return false;
        }
        GR::Graphic::ImageData*   pImage = pSet->Image( 0, 0 );

        GR::Graphic::Image    imgToSave( *pImage );
        if ( dlgIGF.m_SaveMitCompressList )
        {
          GR::Graphic::ContextDescriptor    cdDummy( &imgToSave, &pImage->Palette() );

          imgToSave.SetTransparentColor( cdDummy.MapColor( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) ) );
          imgToSave.Compress();
        }
        if ( dlgIGF.m_SaveMitPalette )
        {
          return imgToSave.Save( FileName, dlgIGF.m_SaveMitCompressList, &pImage->Palette() ) == TRUE;
        }
        return imgToSave.Save( FileName, dlgIGF.m_SaveMitCompressList, NULL ) == TRUE;
      }
      else if ( ( pSet->Frames[0]->Layers.size() == 1 )
      &&        ( saveType == SAVETYPE_GIF ) )
      {
        DlgGIFOptions dlgGIF;

        dlgGIF.m_SaveASGIF89 = true;

        if ( dlgGIF.DoModal() != IDOK )
        {
          return false;
        }
        if ( !dlgGIF.m_SaveASGIF89 )
        {
          return ( (GR::Graphic::FormatGIF*)Format.m_pSavingFormat )->SaveAsGIF87( FileName, pSet );
        }
      }
      return Format.m_pSavingFormat->Save( FileName, pSet );
    }
    else
    {
      // kann das Set als Einzelbild gespeichert werden?
      if ( ( ( pSet->FrameCount() > 1 )
      &&     ( AfxMessageBox( _T( "Das gewünschte Format unterstützt nur Einzelbilder.\nSoll nur der erste Frame gespeichert werden?" ), MB_YESNO ) == IDYES ) )
      ||   ( pSet->FrameCount() == 1 ) )
      {
        GR::Graphic::ImageData*   pData = pSet->Image( 0, 0 );

        if ( pData )
        {
          if ( ExtraProcessing( FileName, saveType, Format, pData ) )
          {
            return true;
          }
          if ( !Format.m_pSavingFormat->CanSave( pData ) )
          {
            AfxMessageBox( _T( "The format plugin does not support this image format." ) );
            return false;
          }
          return Format.m_pSavingFormat->Save( FileName, pData, pSet->Mask( 0, 0 ) );
        }
      }
      return false;
    }
  }
  else if ( Format.m_SavingFreeImageFormat != FIF_UNKNOWN )
  {
    return SaveWithFreeImage( Format.m_SavingFreeImageFormat, FileName, pSet );
  }
  return false;
}



bool CFormatManager::SupportsFormat( const SaveType saveType )
{

  tMapSupportedFormats::iterator    it( m_mapSupportedFormats.find( saveType ) );
  if ( it == m_mapSupportedFormats.end() )
  {
    return false;
  }
  return true;

}



bool CFormatManager::SaveWithFreeImage( int fif, const GR::Char* FileName, ImageSet* pSet )
{
  if ( ( pSet->FrameCount() > 1 )
  &&   ( AfxMessageBox( _T( "Das gewünschte Format unterstützt nur Einzelbilder.\nSoll nur der erste Frame gespeichert werden?" ), MB_YESNO ) != IDYES ) )
  {
    return false;
  }

  GR::Graphic::Image    imageTemp( *pSet->Image( 0, 0 ) );

  switch ( fif )
  {
    case FIF_JPEG:
      {
        if ( ( pSet->Image( 0, 0 )->BitsProPixel() != 24 )
        &&   ( AfxMessageBox( _T( "Das gewünschte Format unterstützt nur 24bit Farbtiefe.\nSoll die Grafik umgewandelt werden?" ), MB_YESNO ) != IDYES ) )
        {
          return false;
        }
        imageTemp.Convert( 24, &pSet->Image( 0, 0 )->Palette() );

        FIBITMAP *dib = ConvertImageToFreeImageDIB( &imageTemp, &pSet->Image( 0, 0 )->Palette() );

        CJpegSaveOptions  dlgJPGOption;

        dlgJPGOption.m_SaveOption = 0;
        if ( dlgJPGOption.DoModal() != IDOK )
        {
          return FALSE;
        }
        DWORD     dwOption = JPEG_DEFAULT;
        switch ( dlgJPGOption.m_SaveOption )
        {
          case 0:
            dwOption = JPEG_DEFAULT;
            break;
          case 1:
            dwOption = JPEG_FAST;
            break;
          case 2:
            dwOption = JPEG_ACCURATE;
            break;
          case 3:
            dwOption = JPEG_QUALITYSUPERB;
            break;
          case 4:
            dwOption = JPEG_QUALITYGOOD;
            break;
          case 5:
            dwOption = JPEG_QUALITYNORMAL;
            break;
          case 6:
            dwOption = JPEG_QUALITYAVERAGE;
            break;
          case 7:
            dwOption = JPEG_QUALITYBAD;
            break;
        }
        if ( !FreeImage_SaveU( FIF_JPEG, dib, GR::Convert::ToUTF16( FileName ).c_str(), dwOption ) )
        {
          AfxMessageBox( _T( "Konnte JPEG nicht speichern! (nicht unterstützte Farbtiefe?)" ) );
          return false;
        }
      }
      return true;
  }

  return false;
}



tFileFormatSupport* CFormatManager::GetFormat( const SaveType saveType )
{
  tMapSupportedFormats::iterator   it( m_mapSupportedFormats.find( saveType ) );
  if ( it == m_mapSupportedFormats.end() )
  {
    return NULL;
  }
  return &it->second;
}



bool CFormatManager::ExtraProcessing( const GR::Char* FileName, const SaveType saveType, tFileFormatSupport& Format, GR::Graphic::ImageData* pData )
{
  switch ( saveType )
  {
    case SAVETYPE_IGF:
      {
        CDlgSaveIGF   dlgIGF;


        if ( dlgIGF.DoModal() != IDOK )
        {
          return true;
        }
        GR::Graphic::Image      imageToSave( *pData );

        if ( dlgIGF.m_SaveMitCompressList )
        {
          
          GR::Graphic::ContextDescriptor    cdDummy( &imageToSave, &pData->Palette() );

          imageToSave.SetTransparentColor( cdDummy.MapColor( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) ) );
          imageToSave.Compress();
        }
        if ( dlgIGF.m_SaveMitPalette )
        {
          imageToSave.Save( FileName, dlgIGF.m_SaveMitCompressList, &pData->Palette() );
        }
        else
        {
          imageToSave.Save( FileName, dlgIGF.m_SaveMitCompressList, NULL );
        }
        return true;
      }
      break;
    case SAVETYPE_BTN:
      if ( pData->ImageFormat() != GR::Graphic::IF_PALETTED )
      {
        if ( AfxMessageBox( _T( "BTN supports only 8bit paletted color format.\nDo you want to convert and save?" ), MB_YESNO ) != IDYES )
        {
          return false;
        }

        CColorReducer     Reducer;

        CPainterImagePackage*   pPack = new CPainterImagePackage( new GR::Graphic::Image( *pData ), &pData->Palette() );

        Reducer.AddSourceToColorCube( pPack );

        Reducer.Calculate();

        CPainterImagePackage*   pResult = Reducer.Reduce( pPack );

        SafeDelete( pPack->m_pImage );
        delete pPack;

        GR::Graphic::ImageData*   pReplacement = pResult->m_pImage->CreateImageData();

        pReplacement->SetPalette( pResult->m_Palette );

        Format.m_pSavingFormat->Save( FileName, pReplacement );

        SafeDelete( pResult->m_pImage );
        delete pReplacement;
        delete pResult;

        return true;
      }
      break;
  }
  return false;
}
