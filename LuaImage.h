#pragma once

#include <lua\luainstance.h>

#include <Lua/Loona.h>

#include <Grafik/Image.h>
#include <Grafik/Palette.h>



class LuaImage : public LuaInstance
{

  public:

    GR::Graphic::Image      Image;

    GR::Graphic::Palette    Palette;

    static char*                          s_ClassName;

    static Loona<LuaImage>::tRegType      s_Methods[];



    LuaImage();


    int                 GetPixel( LuaInstance& Lua );
    int                 GetPixelRGB( LuaInstance& Lua );
    int                 GetImageSize( LuaInstance& Lua );
    int                 PutPixelRGB( LuaInstance& Lua );

};
