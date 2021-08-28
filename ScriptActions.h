#if !defined(AFX_SCRIPTACTIONS_H__5E31FD26_80C0_48E3_B4AD_671B7EC7C444__INCLUDED_)
#define AFX_SCRIPTACTIONS_H__5E31FD26_80C0_48E3_B4AD_671B7EC7C444__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct lua_State;



class ScriptActions  
{

  public:



	  ScriptActions( lua_State* L );
	  virtual ~ScriptActions();

};


#endif // !defined(AFX_SCRIPTACTIONS_H__5E31FD26_80C0_48E3_B4AD_671B7EC7C444__INCLUDED_)
