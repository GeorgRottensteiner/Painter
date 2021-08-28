/*-Brightness-----------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

int Brightness( lua_State* L )
{

  if ( lua_gettop( L ) != 2 )
  {
    return 0;
  }
  int   iValue = (int)lua_tonumber( L, -1 );
  lua_pop( L, 1 );


  return 0;

}
