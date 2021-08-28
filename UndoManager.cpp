// UndoManager.cpp: Implementierung der Klasse CUndoManager.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UndoManager.h"
#include "DocumentInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

/*-Constructor----------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

CUndoManager::CUndoManager()
{

  m_dwMaxUndoSteps  = 100;
  m_dwLastUsedID    = 0;

}



/*-Destructor-----------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

CUndoManager::~CUndoManager()
{

  EmptyUndoList();
  EmptyRedoList();

}



/*-EmptyUndoList--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CUndoManager::EmptyUndoList()
{

  itUndoList  it( m_listUndo.begin() );

  while ( it != m_listUndo.end() )
  {
    delete *it;
    it = m_listUndo.erase( it );
  }

}



/*-EmptyRedoList--------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CUndoManager::EmptyRedoList()
{

  itUndoList  it( m_listRedo.begin() );

  while ( it != m_listRedo.end() )
  {
    delete *it;
    it = m_listRedo.erase( it );
  }

}



/*-IsUndoPossible-------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

BOOL CUndoManager::IsUndoPossible()
{

  if ( m_listUndo.size() )
  {
    return TRUE;
  }
  return FALSE;

}



/*-IsRedoPossible-------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

BOOL CUndoManager::IsRedoPossible()
{

  if ( m_listRedo.size() )
  {
    return TRUE;
  }
  return FALSE;

}



/*-StartUndoGroup-------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CUndoManager::StartUndoGroup()
{

  m_dwLastUsedID = ( ( m_dwLastUsedID + 1 ) & 0xffff );

}



/*-AddUndoStep----------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CUndoManager::AddUndoStep( CUndoState *pState, BOOL bIncreaseIndex )
{

  if ( !pState )
  {
    return;
  }

  EmptyRedoList();

  if ( bIncreaseIndex )
  {
    StartUndoGroup();
  }
  pState->m_dwGroupID = m_dwLastUsedID;

  m_listUndo.push_front( pState );

  while ( abs( (int)( m_listUndo.back()->m_dwGroupID - m_dwLastUsedID ) ) > (int)m_dwMaxUndoSteps )
  {
    // zuviele Undo-Steps, die letzte Gruppe rauswerfen
    CUndoState *pState = m_listUndo.back();

    delete pState;
    m_listUndo.pop_back();
    if ( m_listUndo.empty() )
    {
      break;
    }
  }

}



/*-Undo-----------------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CUndoManager::Undo()
{

  if ( !IsUndoPossible() )
  {
    return;
  }

  DWORD   dwActIndex = m_listUndo.front()->m_dwGroupID;

  while ( ( IsUndoPossible() )
  &&      ( m_listUndo.front()->m_dwGroupID == dwActIndex ) )
  {
    //- aktuelle Kopie von Undo-Schritt erzeugen...
    CUndoState *pStateTemp = m_listUndo.front()->CreateComplementCopy();
    pStateTemp->m_dwGroupID = dwActIndex;

    //- ...und auf Redo-Buffer tun
    m_listRedo.push_front( pStateTemp );
  
    //- Undo-State wiederherstellen
    m_listUndo.front()->Restore();

    //- und löschen
    delete m_listUndo.front();
    m_listUndo.pop_front();
  }

}



/*-Redo-----------------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CUndoManager::Redo()
{

  if ( !IsRedoPossible() )
  {
    return;
  }

  DWORD   dwActIndex = m_listRedo.front()->m_dwGroupID;

  while ( ( IsRedoPossible() )
  &&      ( m_listRedo.front()->m_dwGroupID == dwActIndex ) )
  {
    //- aktuelle Kopie von Redo-Schritt erzeugen...
    CUndoState *stateTemp = m_listRedo.front()->CreateComplementCopy();
    stateTemp->m_dwGroupID = dwActIndex;

    //- ...und auf Undo-Buffer tun
    m_listUndo.push_front( stateTemp );
  
    //- Redo-State wiederherstellen
    m_listRedo.front()->Restore();

    //- und löschen
    delete m_listRedo.front();
    m_listRedo.pop_front();
  }
  
}



/*-Empty----------------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CUndoManager::Empty()
{

  EmptyUndoList();
  EmptyRedoList();

}