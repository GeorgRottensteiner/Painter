#include "FormatPNT.h"
#include "Layer.h"
#include "DocumentInfo.h"
#include "Painter.h"

#include <IO/FileStream.h>

#include <String/Convert.h>



bool SavePNT( DocumentInfo& DocInfo, const GR::String& Filename )
{
  GR::IO::FileStream    file( Filename, IIOStream::OT_WRITE_ONLY );
  if ( !SavePNT( DocInfo, file ) )
  {
    MessageBoxA( NULL, "Konnte Datei nicht zum Schreiben öffnen.", "Fehler", MB_OK | MB_APPLMODAL );
    return false;
  }
  return true;
}



bool SavePNT( DocumentInfo& DocInfo, IIOStream& IOOut )
{
  if ( !IOOut.IsGood() )
  {
    return false;
  }

  CLayer*         pLayer;

  unsigned char   ucDummy,
    ucFaktor;

  DWORD           dwDummy;

  char            szHead[10];




  szHead[0] = 'P';
  szHead[1] = 'N';
  szHead[2] = 'T';
  szHead[3] = 'F';

  IOOut.WriteBlock( (BYTE*)szHead, 4 );

  //pFile->WriteU16( 17 );     // Magic-Version-Number
  IOOut.WriteU16( 18 );     // Magic-Version-Number

  IOOut.WriteU32( DocInfo.Width() );
  IOOut.WriteU32( DocInfo.Height() );
  IOOut.WriteU8( DocInfo.m_BitDepth );

  ucFaktor = 0;
  switch ( DocInfo.m_BitDepth )
  {
    case 8:
      ucFaktor = 1;
      break;
    case 15:
    case 16:
      ucFaktor = 2;
      break;
    case 24:
      ucFaktor = 3;
      break;
    case 32:
      ucFaktor = 4;
      break;
  }

  // Frames durchzählen
  IOOut.WriteU32( DocInfo.m_LayeredFrames.size() );

  for ( size_t iFrame = 0; iFrame < DocInfo.m_LayeredFrames.size(); ++iFrame )
  {
    ucDummy = ( GR::u8 )DocInfo.m_LayeredFrames[iFrame].Layers.size();

    IOOut.WriteU8( ucDummy );

    // die einzelnen Layer schreiben

    for ( size_t i = 0; i < DocInfo.m_LayeredFrames[iFrame].LayerCount(); i++ )
    {
      pLayer = DocInfo.GetLayer( iFrame, i );

      // Layer-Flags zusammensetzen
      dwDummy = 0;
      if ( pLayer->GetMask() != NULL )
      {
        // dieser Layer hat eine Maske
        dwDummy |= 1;
      }
      IOOut.WriteU32( dwDummy );
      IOOut.WriteU32( pLayer->m_Alpha );
      IOOut.WriteU32( pLayer->m_Type );
      IOOut.WriteU32( pLayer->m_Transparenz );
      IOOut.WriteU32( 1 );      // Version 1
      IOOut.WriteU32( 0 );      // vier Longs für später
      IOOut.WriteU32( 0 );
      IOOut.WriteU32( 0 );
      IOOut.WriteU32( 0 );
      IOOut.WriteBlock( (BYTE*)pLayer->GetImage()->GetData(), pLayer->GetImage()->GetWidth() * pLayer->GetImage()->GetHeight() * ucFaktor );
      if ( pLayer->GetMask() != NULL )
      {
        IOOut.WriteBlock( (BYTE*)pLayer->GetMask()->GetData(), pLayer->GetImage()->GetWidth() * pLayer->GetImage()->GetHeight() );
      }
      IOOut.WriteString( pLayer->m_Name );
    }
  }

  IOOut.Close();

  return true;
}



bool LoadPNT( DocumentInfo& DocInfo, const GR::String& Filename )
{
  GR::IO::FileStream       file( Filename );

  if ( !LoadPNT( DocInfo, file ) )
  {
    MessageBoxA( NULL, "Konnte Datei nicht öffnen.", "Fehler", MB_OK | MB_APPLMODAL );
    return false;
  }
  return true;
}



bool LoadPNT( DocumentInfo& DocInfo, IIOStream& IOIn )
{
  GR::Graphic::Image*   pImage;

  CLayer*               pLayer;

  unsigned char         ucLayerCount,
                        ucI,
                        ucFaktor;

  WORD                  wDummy;

  char                  szHead[10];


  if ( !IOIn.IsGood() )
  {
    return false;
  }

  IOIn.ReadBlock( (BYTE*)szHead, 4 );
  wDummy = IOIn.ReadU16();

  if ( ( szHead[0] != 'P' )
    || ( szHead[1] != 'N' )
    || ( szHead[2] != 'T' )
    || ( szHead[3] != 'F' )
    || ( ( wDummy != 17 )
      && ( wDummy != 18 ) ) )
  {
    // kein PNTF!
    MessageBoxA( NULL, "Keine gültige PNT-Datei.", "Fehler", MB_OK | MB_APPLMODAL );
    return FALSE;
  }

  DocInfo.m_SaveType = SAVETYPE_PNT;

  DocInfo.m_DocType = DT_IMAGE;
  DWORD   dwWidth = IOIn.ReadU32(),
          dwHeight = IOIn.ReadU32();
  DocInfo.SetSize( dwWidth, dwHeight );
  DocInfo.m_BitDepth = IOIn.ReadU8();

  /*
  if ( DocInfo.m_BitDepth == 16 )
  {
  DocInfo.m_BitDepth = 15;
  }
  */

  ucFaktor = 0;
  switch ( DocInfo.m_BitDepth )
  {
    case 8:
      ucFaktor = 1;
      break;
    case 15:
    case 16:
      ucFaktor = 2;
      break;
    case 24:
      ucFaktor = 3;
      break;
    case 32:
      ucFaktor = 4;
      break;
  }

  POSITION pos = DocInfo.m_pDoc->GetFirstViewPosition();
  CView* pView = DocInfo.m_pDoc->GetNextView( pos );

  if ( wDummy == 17 )
  {
    DocInfo.AddFrame();

    // Layer durchzählen
    ucLayerCount = IOIn.ReadU8();

    for ( ucI = 0; ucI < ucLayerCount; ucI++ )
    {
      DWORD   dwFlags,
        dwAlpha,
        dwLayerMode,
        dwTransparenz;


      dwFlags = IOIn.ReadU32();
      dwAlpha = IOIn.ReadU32();
      dwLayerMode = IOIn.ReadU32();
      dwTransparenz = IOIn.ReadU32();
      DWORD   dwVersion = IOIn.ReadU32();
      IOIn.ReadU32();
      IOIn.ReadU32();
      IOIn.ReadU32();
      IOIn.ReadU32();

      if ( ucI == 0 )
      {
        // der erste Layer
        pImage = new GR::Graphic::Image( (WORD)DocInfo.Width(), (WORD)DocInfo.Height(), DocInfo.m_BitDepth, 0, 0 );
        IOIn.ReadBlock( (BYTE*)pImage->GetData(), pImage->GetWidth() * pImage->GetHeight() * ucFaktor );
        DocInfo.AddLayer( pImage );
      }
      else
      {
        // ein weiterer Layer
        GR::Graphic::Image*    pImage = new GR::Graphic::Image( DocInfo.GetImage( 0, 0 ) );
        CLayer*   pLayer = DocInfo.AddLayer( pImage );
        IOIn.ReadBlock( (BYTE*)pLayer->GetImage()->GetData(), pLayer->GetImage()->GetWidth() * pLayer->GetImage()->GetHeight() * ucFaktor );
      }
      pLayer = DocInfo.GetLayer( 0, DocInfo.m_LayeredFrames[0].LayerCount() - 1 );

      if ( dwFlags & 1 )
      {
        // dieser Layer hat auch eine Alphamaske
        pImage = new GR::Graphic::Image( (WORD)DocInfo.Width(), (WORD)DocInfo.Height(), 8, 0, 0 );

        IOIn.ReadBlock( (BYTE*)pImage->GetData(), pImage->GetWidth() * pImage->GetHeight() );
        pLayer->SetLayerMask( pImage );
      }
      pLayer->m_Alpha = dwAlpha;
      pLayer->m_Type = ( CLayer::LayerType )dwLayerMode;
      pLayer->m_Transparenz = dwTransparenz;

      if ( dwVersion == 1 )
      {
        pLayer->m_Name = IOIn.ReadString();
      }
    }
  }
  else if ( wDummy == 18 )
  {
    // neu, Frames UND m_vectLayers
    size_t    iFrames = IOIn.ReadU32();

    for ( size_t iFrame = 0; iFrame < iFrames; ++iFrame )
    {
      DocInfo.AddFrame();

      // Layer durchzählen
      ucLayerCount = IOIn.ReadU8();

      POSITION pos = DocInfo.m_pDoc->GetFirstViewPosition();
      CView* pView = DocInfo.m_pDoc->GetNextView( pos );

      for ( ucI = 0; ucI < ucLayerCount; ucI++ )
      {
        DWORD   dwFlags,
          dwAlpha,
          dwLayerMode,
          dwTransparenz;


        dwFlags = IOIn.ReadU32();
        dwAlpha = IOIn.ReadU32();
        dwLayerMode = IOIn.ReadU32();
        dwTransparenz = IOIn.ReadU32();
        DWORD   dwVersion = IOIn.ReadU32();
        IOIn.ReadU32();
        IOIn.ReadU32();
        IOIn.ReadU32();
        IOIn.ReadU32();

        if ( ucI == 0 )
        {
          // der erste Layer
          pImage = new GR::Graphic::Image( (WORD)DocInfo.Width(), (WORD)DocInfo.Height(), DocInfo.m_BitDepth, 0, 0 );
          IOIn.ReadBlock( (BYTE*)pImage->GetData(), pImage->GetWidth() * pImage->GetHeight() * ucFaktor );
          DocInfo.AddLayer( pImage, iFrame );
        }
        else
        {
          // ein weiterer Layer
          GR::Graphic::Image*    pImage = new GR::Graphic::Image( DocInfo.GetImage( iFrame, 0 ) );
          CLayer*   pLayer = DocInfo.AddLayer( pImage, iFrame );
          IOIn.ReadBlock( (BYTE*)pLayer->GetImage()->GetData(), pLayer->GetImage()->GetWidth() * pLayer->GetImage()->GetHeight() * ucFaktor );
        }
        pLayer = DocInfo.GetLayer( iFrame, ucI );

        if ( dwFlags & 1 )
        {
          // dieser Layer hat auch eine Alphamaske
          pImage = new GR::Graphic::Image( (WORD)DocInfo.Width(), (WORD)DocInfo.Height(), 8, 0, 0 );

          IOIn.ReadBlock( (BYTE*)pImage->GetData(), pImage->GetWidth() * pImage->GetHeight() );
          pLayer->SetLayerMask( pImage );
        }
        pLayer->m_Alpha = dwAlpha;
        pLayer->m_Type = ( CLayer::LayerType )dwLayerMode;
        pLayer->m_Transparenz = dwTransparenz;

        if ( dwVersion == 1 )
        {
          pLayer->m_Name = GR::Convert::ToString( IOIn.ReadStringW() );
        }
      }
    }
  }
  IOIn.Close();

  if ( DocInfo.m_BitDepth <= 8 )
  {
    if ( DocInfo.GetPalette( DocInfo.CurrentFrame() ) == NULL )
    {
      DocInfo.GetFrame( DocInfo.CurrentFrame() )->Palette = theApp.m_PainterPalette;
    }
    else
    {
      *DocInfo.GetPalette( DocInfo.CurrentFrame() ) = theApp.m_PainterPalette;
    }
  }

  return TRUE;
}



