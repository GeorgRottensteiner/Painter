#include "stdafx.h"

#include "painterlua.h"
#include "painterDoc.h"
#include "painter.h"

#include "MaskedContextDescriptor.h"
#include "DocumentInfo.h"
#include "DlgRename.h"

#include <Misc/Misc.h>

#include <String/Convert.h>

#include <Lua/Loona.h>
#include "LuaImage.h"

extern "C"
{
#include <Lua/lua\lua.h>
#include <Lua/lua\lualib.h>
#include <Lua/lua/lauxlib.h>
}



CPainterLua*    pGlobalLua = NULL;

CPainterLua* CPainterLua::GetApp()
{

  return pGlobalLua;

}


CPainterLua::CPainterLua() :
  LuaInstance(),
  m_pScriptDocInfo( NULL ),
  m_pScriptViewInfo( NULL )
{

  InitInstance();

  luaopen_math( m_LuaInstance );

  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::GetPixel ), "GetPixel" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::GetPixelRGB ), "GetPixelRGB" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::PutPixelRGB ), "PutPixelRGB" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::GetSelectionPos ), "GetSelectionPos" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::GetSelectionSize ), "GetSelectionSize" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::GetImageSize ), "GetImageSize" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::GetImage ), "GetImage" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::ClearSelection ), "ClearSelection" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::AddSelectionRect ), "AddSelectionRect" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::ResizeCanvas ), "ResizeCanvas" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::InputValue ), "InputValue" );
  RegisterHandler( fastdelegate::MakeDelegate( this, &CPainterLua::CreateNewImageFromRect ), "CreateNewImageFromRect" );

  Loona<LuaImage>::Register( *this );

  DoFile( CMisc::AppPath( "luabit.lua" ) );

  m_StartedPainting = false;

}


void CPainterLua::InitInstance()
{
  pGlobalLua = this;

  luaopen_base( m_LuaInstance );
  luaopen_string( m_LuaInstance );
  luaopen_table( m_LuaInstance );
}



DocumentInfo* CPainterLua::GetCurrentDocumentInfo()
{
  return m_pScriptDocInfo;
}



int CPainterLua::GetPixel( LuaInstance& Lua )
{

  if ( m_pScriptViewInfo == NULL )
  {
    Lua.PopAll();
    Lua.PushNumber( 0 );
    return 0xff000000;
  }

  int     iX = Lua.ToNumber( 1 );
  int     iY = Lua.ToNumber( 2 );

  Lua.PopAll();

  Lua.PushNumber( m_pScriptViewInfo->GetActiveImage()->GetPixel( iX, iY ) );
  return 1;

}



int CPainterLua::GetPixelRGB( LuaInstance& Lua )
{
  if ( m_pScriptViewInfo == NULL )
  {
    Lua.PopAll();
    Lua.PushNumber( 0 );
    return 0xff000000;
  }

  int     iX = Lua.ToNumber( 1 );
  int     iY = Lua.ToNumber( 2 );

  Lua.PopAll();

  GR::Graphic::ContextDescriptor    cdImage( m_pScriptViewInfo->GetActiveImage(), m_pScriptViewInfo->GetActivePalette() );

  Lua.PushNumber( cdImage.GetPixelRGB( iX, iY ) );
  return 1;
}



int CPainterLua::GetSelectionPos( LuaInstance& Lua )
{
  Lua.PopAll();

  Lua.PushNumber( m_pScriptDocInfo->m_Selection.left );
  Lua.PushNumber( m_pScriptDocInfo->m_Selection.top );

  return 2;
}



int CPainterLua::GetSelectionSize( LuaInstance& Lua )
{
  Lua.PopAll();

  Lua.PushNumber( m_pScriptDocInfo->m_Selection.right - m_pScriptDocInfo->m_Selection.left );
  Lua.PushNumber( m_pScriptDocInfo->m_Selection.bottom - m_pScriptDocInfo->m_Selection.top );

  return 2;
}



int CPainterLua::GetImageSize( LuaInstance& Lua )
{
  Lua.PopAll();

  Lua.PushNumber( m_pScriptDocInfo->Width() );
  Lua.PushNumber( m_pScriptDocInfo->Height() );

  return 2;
}



int CPainterLua::ClearSelection( LuaInstance& Lua )
{
  m_pScriptDocInfo->EmptySelection();
  m_pScriptDocInfo->RedrawAllViews();

  Lua.PopAll();

  return 0;
}



int CPainterLua::AddSelectionRect( LuaInstance& Lua )
{
  if ( Lua.GetTop() == 4 )
  {
    int     iX = Lua.ToNumber( -4 );
    int     iY = Lua.ToNumber( -3 );
    int     iW = Lua.ToNumber( -2 );
    int     iH = Lua.ToNumber( -1 );

    m_pScriptDocInfo->AddRectSelection( iX, iY, iW, iH );
    m_pScriptDocInfo->RedrawAllViews();
  }
  Lua.PopAll();

  return 0;
}



int CPainterLua::PutPixelRGB( LuaInstance& Lua )
{
  if ( Lua.GetTop() == 3 )
  {
    int     iX = Lua.ToNumber( -3 );
    int     iY = Lua.ToNumber( -2 );
    GR::u32 Color = Lua.ToNumber( -1 );

    if ( !m_StartedPainting )
    {
      m_StartedPainting = true;
      m_pScriptDocInfo->AddUndoRectAll();
    }
    GR::Graphic::ContextDescriptor    cdImage( m_pScriptDocInfo->GetActiveImage(), m_pScriptViewInfo->GetActivePalette() );

    cdImage.PutPixel( iX, iY, Color );
  }
  Lua.PopAll();

  return 0;
}



int CPainterLua::ResizeCanvas( LuaInstance& Lua )
{
  if ( Lua.GetTop() == 4 )
  {
    int     NewWidth  = Lua.ToNumber( -4 );
    int     NewHeight = Lua.ToNumber( -3 );
    bool    CenterH   = Lua.ToBoolean( -2 );
    bool    CenterV   = Lua.ToBoolean( -1 );

    m_pScriptDocInfo->ResizeCanvas( NewWidth, NewHeight, CenterH, CenterV );
  }
  Lua.PopAll();

  return 0;

}



int CPainterLua::InputValue( LuaInstance& Lua )
{
  GR::String   result;

  if ( Lua.GetTop() == 2 )
  {
    GR::String    valueInfo = Lua.ToString( -1 );
    GR::String    valueDefault = Lua.ToString( -2 );

    CDlgRename    dlgRename( GR::Convert::ToString( valueInfo ), GR::Convert::ToString( valueDefault ) );

    if ( dlgRename.DoModal() == IDOK )
    {
      result = GR::Convert::ToStringA( dlgRename.m_Name );
    }
  }
  Lua.PopAll();

  Lua.PushString( result );
  return 1;

}



int CPainterLua::CreateNewImageFromRect( LuaInstance& Lua )
{
  if ( Lua.GetTop() == 4 )
  {
    int     X = Lua.ToNumber( -4 );
    int     Y = Lua.ToNumber( -3 );
    int     Width = Lua.ToNumber( -2 );
    int     Height = Lua.ToNumber( -1 );


    GR::Graphic::Image*     pImage = m_pScriptDocInfo->GetActiveImage();

    GR::Graphic::Palette*   pSourcePal = m_pScriptDocInfo->GetPalette( 0 );

    GR::Graphic::ContextDescriptor    cdImage( pImage, pSourcePal );

    GR::Graphic::ContextDescriptor    cdTarget;

    cdTarget.CreateData( Width, Height, cdImage.ImageFormat() );

    cdImage.CopyArea( X, Y, Width, Height, 0, 0, &cdTarget );

    CPainterDoc* pDoc = (CPainterDoc*)theApp.pDocTemplate->CreateNewDocument();

    DocumentInfo* pDocInfo = &pDoc->diInfo;

    CFrameWnd* pFrameWnd = theApp.pDocTemplate->CreateNewFrame( pDoc, NULL );
    
    POSITION pos = pDoc->GetFirstViewPosition();

    pDoc->diInfo.m_DocType = DT_IMAGE;

    pDoc->diInfo.SetSize( Width, Height );
    pDoc->diInfo.m_BitDepth = (GR::u8)cdTarget.DepthFromImageFormat( cdTarget.ImageFormat() );

    pDoc->diInfo.AddLayer( cdTarget.CreateImage(), pDoc->diInfo.AddFrame() );
    if ( pSourcePal )
    {
      *pDoc->diInfo.GetPalette( 0 ) = *pSettings->m_pActiveDocInfo->GetPalette( 0 );
    }
    pDoc->diInfo.m_AnimationRunning = FALSE;

    theApp.pDocTemplate->InitialUpdateFrame( pFrameWnd, pDoc, TRUE );

    pDoc->SetTitle( _T( "Neues Dokument" ) );
    pDocInfo->RecreateAllViews();
  }
  Lua.PopAll();

  return 0;

}



int CPainterLua::GetImage( LuaInstance& Lua )
{
  if ( Lua.GetTop() == 4 )
  {
    int     X = Lua.ToNumber( -4 );
    int     Y = Lua.ToNumber( -3 );
    int     Width = Lua.ToNumber( -2 );
    int     Height = Lua.ToNumber( -1 );

    Lua.PopAll();

    GR::Graphic::Image*     pImage = m_pScriptDocInfo->GetActiveImage();

    GR::Graphic::Palette*   pSourcePal = m_pScriptDocInfo->GetPalette( 0 );

    auto pLuaImage = new LuaImage();
    pLuaImage->Image = *pImage;
    if ( pSourcePal != NULL )
    {
      pLuaImage->Palette = *pSourcePal;
    }

    Loona<LuaImage>::PushObject( Lua, pLuaImage );
    return 1;
  }
  Lua.PopAll();

  return 0;
}