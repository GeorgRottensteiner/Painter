#include "stdafx.h"

#include "LuaImage.h"

#include <Grafik/ContextDescriptor.h>

extern "C"
{
#include <Lua/lua\lua.h>
#include <Lua/lua\lualib.h>
#include <Lua/lua/lauxlib.h>
}

#include <Lua/Loona.h>


char* LuaImage::s_ClassName = "LuaImage";

Loona<LuaImage>::tRegType LuaImage::s_Methods[] =
{
  { "GetPixel",                   &LuaImage::GetPixel },
  { "GetPixelRGB",                &LuaImage::GetPixelRGB },
  { "PutPixelRGB",                &LuaImage::PutPixelRGB },
  { "GetImageSize",               &LuaImage::GetImageSize },
  { 0, 0 }
};



LuaImage::LuaImage() :
  LuaInstance()
{
}



int LuaImage::GetPixel( LuaInstance& Lua )
{
  int     x = Lua.ToNumber( 1 );
  int     y = Lua.ToNumber( 2 );

  Lua.PopAll();

  Lua.PushNumber( Image.GetPixel( x, y ) );
  return 1;

}



int LuaImage::GetPixelRGB( LuaInstance& Lua )
{
  int     x = Lua.ToNumber( 1 );
  int     y = Lua.ToNumber( 2 );

  Lua.PopAll();

  GR::Graphic::ContextDescriptor    cdImage( &Image, &Palette );

  Lua.PushNumber( cdImage.GetPixelRGB( x, y ) );
  return 1;

}



int LuaImage::GetImageSize( LuaInstance& Lua )
{
  Lua.PopAll();

  Lua.PushNumber( Image.GetWidth() );
  Lua.PushNumber( Image.GetHeight() );

  return 2;
}



int LuaImage::PutPixelRGB( LuaInstance& Lua )
{
  if ( Lua.GetTop() == 3 )
  {
    int     x = Lua.ToNumber( -3 );
    int     y = Lua.ToNumber( -2 );
    GR::u32 Color = Lua.ToNumber( -1 );

    GR::Graphic::ContextDescriptor    cdImage( &Image, &Palette );

    cdImage.PutPixel( x, y, Color );
  }
  Lua.PopAll();

  return 0;
}



