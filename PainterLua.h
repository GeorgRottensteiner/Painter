#pragma once

#include <lua\luainstance.h>

class DocumentInfo;
class CViewInfo;

class CPainterLua : public LuaInstance
{

  public:

    bool                m_StartedPainting;

    CViewInfo*          m_pScriptViewInfo;
    DocumentInfo*      m_pScriptDocInfo;


    CPainterLua();

    void                InitInstance();

    DocumentInfo*       GetCurrentDocumentInfo();

    static CPainterLua* GetApp();

    int                 GetPixel( LuaInstance& Lua );
    int                 GetPixelRGB( LuaInstance& Lua );

    int                 GetImageSize( LuaInstance& Lua );
    int                 GetSelectionPos( LuaInstance& Lua );
    int                 GetSelectionSize( LuaInstance& Lua );
    int                 ClearSelection( LuaInstance& Lua );
    int                 AddSelectionRect( LuaInstance& Lua );

    int                 ResizeCanvas( LuaInstance& Lua );

    int                 PutPixelRGB( LuaInstance& Lua );

    int                 InputValue( LuaInstance& Lua );
    int                 CreateNewImageFromRect( LuaInstance& Lua );

    int                 GetImage( LuaInstance& Lua );

};
