#include <Lua/LuaInstance.h>

#include "ScriptActions.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



ScriptActions::ScriptActions( lua_State* L )
{
  /*
  lua_pushcfunction( L, Brightness );
  lua_setglobal( L, "Brightness" );
  */
}



ScriptActions::~ScriptActions()
{

}



