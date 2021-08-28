// Settings.cpp: Implementierung der Klasse CSettings.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Settings.h"
#include "Import.h"
#include "resource.h"

#include "ViewInfo.h"

#include "PainterEnums.h"

#include <Debug\debugclient.h>

#include <WinSys/RegistryHelper.h>
#include <Misc/Misc.h>

#include <Grafik/Image.h>
#include <Grafik/ImageFormate/ImageFormatManager.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CSettings     *pSettings = NULL;



CSettings::CSettings()
  : m_pActiveDocInfo( NULL ),
    m_pActiveViewInfo( NULL ),
    m_pCurrentBrushTip( NULL ),
    m_dwClipboardFormat( 0 )
{


  /*
  m_LuaInstance = lua_open( 0 );

  lua_pushcfunction( m_LuaInstance, LuaError );
  lua_setglobal( m_LuaInstance, "_ERRORMESSAGE" );
  */

  brushForeground.Type   = GetSetting( "BrushForegroundType", BR_SOLID );
  brushForeground.Angle  = GetSettingFloat( "BrushForegroundAngle", 0.0f );

  brushBackground.Type   = GetSetting( "BrushBackgroundType", BR_SOLID );
  brushBackground.Angle  = GetSettingFloat( "BrushBackgroundAngle", 0.0f );

  for ( int i = 0; i < 256; i++ )
  {
    m_AlphaPalette.SetColor( i, i, i, i );
  }

  // Patterns holen
  GR::String    strFGPattern = GetSettingString( "BrushForegroundPattern" ),
                    strBGPattern = GetSettingString( "BrushBackgroundPattern" );

  for ( int i = 0; i < GetSetting( "PatternCount" ); i++ )
  {
    GR::Char    szTemp[MAX_PATH];

    CPattern *pPattern = new CPattern();

    sprintf_s( szTemp, MAX_PATH, "PatternFile%d", i + 1 );
    pPattern->m_FileName = GetSettingString( szTemp );
    sprintf_s( szTemp, MAX_PATH, "PatternDesc%d", i + 1 );
    pPattern->m_strDescription = GetSettingString( szTemp );

    m_listPatterns.push_back( pPattern );

    if ( strFGPattern.length() )
    {
      if ( strFGPattern.compare( pPattern->m_FileName ) == 0 )
      {
        brushForeground.pPattern = pPattern;

        GR::Graphic::ImageData*   pData = ImageFormatManager::Instance().LoadData( strFGPattern.c_str() );
        if ( pData )
        {
          pPattern->m_pImagePack = new CPainterImagePackage();
          pPattern->m_pImagePack->m_pImage  = new GR::Graphic::Image( *pData );
          if ( pData->Palette().Data() )
          {
            pPattern->m_pImagePack->m_Palette = pData->Palette();
          }
          delete pData;
        }
      }
    }
    if ( strBGPattern.length() )
    {
      if ( strBGPattern.compare( pPattern->m_FileName ) == 0 )
      {
        brushBackground.pPattern = pPattern;

        GR::Graphic::ImageData*   pData = ImageFormatManager::Instance().LoadData( strBGPattern.c_str() );
        if ( pData )
        {
          pPattern->m_pImagePack->m_pImage  = new GR::Graphic::Image( *pData );
          pPattern->m_pImagePack->m_Palette = pData->Palette();
          delete pData;
        }
      }
    }
  }

  m_dwColor[0] = 0xffffffff;
  m_dwColor[1] = 0xff000000;
  m_dwColor[2] = 0;
  m_dwColor[3] = 255;

}

CSettings::~CSettings()
{

  DeleteObject( m_brushBackground );
  DeleteObject( m_hbmBackground );

  if ( brushForeground.pPattern )
  {
    if ( brushForeground.pPattern->m_pImagePack )
    {
      SafeDelete( brushForeground.pPattern->m_pImagePack->m_pImageMask );
      SafeDelete( brushForeground.pPattern->m_pImagePack->m_pImage );
      SafeDelete( brushForeground.pPattern->m_pImagePack );
    }
  }
  if ( brushBackground.pPattern )
  {
    if ( brushBackground.pPattern->m_pImagePack )
    {
      SafeDelete( brushBackground.pPattern->m_pImagePack->m_pImageMask );
      SafeDelete( brushBackground.pPattern->m_pImagePack->m_pImage );
      SafeDelete( brushBackground.pPattern->m_pImagePack );
    }
  }

  SetSetting( "BrushForegroundType", brushForeground.Type );
  SetSettingFloat( "BrushForegroundAngle", brushForeground.Angle );

  SetSetting( "BrushBackgroundType", brushBackground.Type );
  SetSettingFloat( "BrushBackgroundAngle", brushBackground.Angle );

  SetSetting( "ForeColor", GetRGBColor( CSettings::CO_WORKCOLOR ) );
  SetSetting( "BackColor", GetRGBColor( CSettings::CO_WORKCOLOR_2 ) );

  // Patterns rauswerfen
  SetSetting( "PatternCount", m_listPatterns.size() );

  {
    std::list<CPattern*>::iterator   it( m_listPatterns.begin() );

    DWORD   dwNr = 0;
    GR::Char    szTemp[MAX_PATH];
    while ( it != m_listPatterns.end() )
    {
      CPattern *pPattern = *it;

      dwNr++;
      sprintf_s( szTemp, MAX_PATH, "PatternFile%d", dwNr );
      SetSetting( szTemp, pPattern->m_FileName );
      sprintf_s( szTemp, MAX_PATH, "PatternDesc%d", dwNr );
      SetSetting( szTemp, pPattern->m_strDescription );

      delete *it;

      it = m_listPatterns.erase( it );
    }
  }

  m_listPhotoShopFilter.clear();
  /*
  lua_close( m_LuaInstance );
  m_LuaInstance = NULL;
  */

}



void CSettings::SetSetting( const GR::String& strName, int iValue )
{
  std::map<GR::String, GR::String>::iterator     it( m_mapSettings.find( strName ) );


  if ( it != m_mapSettings.end() )
  {
    m_mapSettings.erase( it );
  }

  GR::Char      szBuffer[200];

  sprintf_s( szBuffer, 200, "%d", iValue );

  m_mapSettings[strName] = szBuffer;

  Registry::SetKey( HKEY_CURRENT_USER, "Software\\Invisible\\Painter\\Settings", strName.c_str(), szBuffer );
}



void CSettings::SetSettingFloat( const GR::String& strName, float fValue )
{
  std::map<GR::String, GR::String>::iterator     it( m_mapSettings.find( strName ) );


  if ( it != m_mapSettings.end() )
  {
    m_mapSettings.erase( it );
  }

  GR::Char      szBuffer[200];

  sprintf_s( szBuffer, 200, ( Misc::Format( "%1:3:2%" ) << fValue ).Result().c_str() );

  m_mapSettings[strName] = szBuffer;

  Registry::SetKey( HKEY_CURRENT_USER, "Software\\Invisible\\Painter\\Settings", strName.c_str(), szBuffer );
}



void CSettings::SetSetting( const GR::String& strName, const GR::String& strValue )
{
  std::map<GR::String, GR::String>::iterator     it( m_mapSettings.find( strName ) );
  if ( it != m_mapSettings.end() )
  {
    m_mapSettings.erase( it );
  }

  m_mapSettings[strName] = strValue;

  Registry::SetKey( HKEY_CURRENT_USER, "Software\\Invisible\\Painter\\Settings", strName.c_str(), strValue.c_str() );
}



int CSettings::GetSetting( const GR::String& strName, int iDefault )
{
  std::map<GR::String, GR::String>::iterator     it( m_mapSettings.find( strName ) );

  if ( it == m_mapSettings.end() )
  {
    GR::String    value;
    if ( !Registry::GetKey( HKEY_CURRENT_USER, "Software\\Invisible\\Painter\\Settings", strName.c_str(), value ) )
    {
      return iDefault;
    }
    return GR::Convert::ToI32( value );
  }
  return GR::Convert::ToI32( it->second.c_str() );
}



float CSettings::GetSettingFloat( const GR::String& strName, float fDefault )
{
  std::map<GR::String, GR::String>::iterator     it( m_mapSettings.find( strName ) );


  if ( it == m_mapSettings.end() )
  {
    GR::String    value;
    if ( !Registry::GetKey( HKEY_CURRENT_USER, "Software\\Invisible\\Painter\\Settings", strName.c_str(), value ) )
    {
      return fDefault;
    }
    return GR::Convert::ToF32( value );
  }
  return GR::Convert::ToF32( it->second );
}



GR::String CSettings::GetSettingString( const GR::String& strName, const GR::String& strDefault )
{
  std::map<GR::String, GR::String>::iterator     it( m_mapSettings.find( strName ) );

  if ( it == m_mapSettings.end() )
  {
    GR::String    value;
    if ( !Registry::GetKey( HKEY_CURRENT_USER, "Software\\Invisible\\Painter\\Settings", strName.c_str(), value ) )
    {
      return strDefault;
    }
    return value;
  }
  return it->second;
}



void CSettings::SetColor( GR::u32 dwIndex, GR::u32 dwColor )
{
  if ( dwIndex >= 4 )
  {
    return;
  }
  m_dwColor[dwIndex] = dwColor;
}



GR::u32 CSettings::GetRGBColor( GR::u32 dwIndex )
{
  if ( dwIndex >= 4 )
  {
    return 0;
  }
  return m_dwColor[dwIndex];
}



GR::u32 CSettings::GetColorRef( GR::u32 dwIndex )
{
  GR::u32     dwColor = GetRGBColor( dwIndex );

  return   ( ( dwColor & 0xff0000 ) >> 16 )
         +   ( dwColor & 0x00ff00 )
         + ( ( dwColor & 0x0000ff ) << 16 );
}



void CSettings::SnapToGrid( POINT *pt )
{
  if ( GetSetting( "SnapEnabled", 1 ) )
  {
    int     iSnappedX = pt->x,
            iSnappedY = pt->y;

    int     iSnapWidth = GetSetting( "SnapWidth", 20 );
    int     iSnapHeight = GetSetting( "SnapHeight", 20 );

    if ( iSnapWidth <= 0 )
    {
      iSnapWidth = 16;
    }
    if ( iSnapHeight <= 0 )
    {
      iSnapHeight = 16;
    }
    iSnappedX -= ( iSnappedX % iSnapWidth );
    iSnappedY -= ( iSnappedY % iSnapHeight );

    iSnappedX += GetSetting( "SnapXOffset" );
    iSnappedY += GetSetting( "SnapYOffset" );

    pt->x = iSnappedX;
    pt->y = iSnappedY;
  }
}



BOOL CSettings::RemovePattern( CPattern *pP )
{
  if ( pP == NULL )
  {
    return FALSE;
  }

  std::list<CPattern*>::iterator    it( m_listPatterns.begin() );

  while ( it != m_listPatterns.end() )
  {
    if ( *it == pP )
    {
      if ( pP == brushForeground.pPattern )
      {
        brushForeground.SetPattern( NULL );
      }
      if ( pP == brushBackground.pPattern )
      {
        brushBackground.SetPattern( NULL );
      }
      m_listPatterns.erase( it );
      break;
    }
    it++;
  }

  delete pP;

  return TRUE;
}



void CSettings::Log( const GR::Char* Text )
{
  dh::Log( Text );
}



void CSettings::UpdateActiveViewInfo()
{
  // Heul, ist das häßlich, bloß weil ActivateView nicht ganz das tut, was es tun sollte
  m_pActiveViewInfo = (CViewInfo*)SendMessage( m_hwndMainFrame, WM_USER, 0, 0 );

  if ( m_pActiveViewInfo )
  {
    m_pActiveDocInfo  = m_pActiveViewInfo->m_pDocInfo;
  }
  else
  {
    m_pActiveDocInfo  = NULL;
  }

  Notify( NF_ACTIVE_VIEW_CHANGED );
}



void CSettings::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pMemberFrom )
{
  switch ( NotifyMessage )
  {
    case NF_DOC_ADDED:
      break;
    case NF_DOC_REMOVED:
      break;
  }
}