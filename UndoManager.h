// UndoManager.h: Schnittstelle für die Klasse CUndoManager.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOMANAGER_H__8A44B423_CE9E_11D4_9D7D_0001029EF128__INCLUDED_)
#define AFX_UNDOMANAGER_H__8A44B423_CE9E_11D4_9D7D_0001029EF128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786 )
#include <string>
#include <list>

class CUndoState
{

  public:

    DWORD   m_dwGroupID;

    virtual ~CUndoState() {};

    virtual BOOL Restore()     = 0;

    virtual std::string Type() = 0;

    virtual CUndoState* CreateComplementCopy() = 0;

  protected:

    CUndoState() {};
};

typedef std::list<CUndoState*>            tUndoList;
typedef std::list<CUndoState*>::iterator  itUndoList;

class CUndoManager  
{

  public:

    DWORD             m_dwMaxUndoSteps,
                      m_dwLastUsedID;

    tUndoList         m_listUndo,
                      m_listRedo;

	  CUndoManager();
	  virtual ~CUndoManager();


    void EmptyUndoList();
    void EmptyRedoList();
    void Empty();

    void StartUndoGroup();

    BOOL IsUndoPossible();
    BOOL IsRedoPossible();

    void AddUndoStep( CUndoState *pState, BOOL bIncreaseIndex = TRUE );
    void Undo();
    void Redo();

};

#endif // !defined(AFX_UNDOMANAGER_H__8A44B423_CE9E_11D4_9D7D_0001029EF128__INCLUDED_)
