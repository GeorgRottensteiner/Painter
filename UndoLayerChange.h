// UndoRename.h: Schnittstelle für die Klasse CUndoRename.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOLAYERCHANGE_H__918290A4_D81A_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_UNDOLAYERCHANGE_H__918290A4_D81A_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoManager.h"

class DocumentInfo;
class CLayer;

class CUndoLayerChange : public CUndoState  
{
  public:

    enum eLayerChangeType
    {
      ULC_ADD_LAYER = 0,
      ULC_REMOVE_LAYER,
      ULC_EXCHANGE_LAYER,
    };

    DocumentInfo*        m_pDocInfo;

    CLayer*               m_pLayer;

    size_t                m_dwFrameIndex,
                          m_dwLayerIndex,
                          m_dwSecondLayerIndex;

    eLayerChangeType      m_ChangeType;


    CUndoLayerChange( DocumentInfo* pDocInfo, 
                      size_t dwFrame,
                      size_t dwLayerIndex,
                      size_t dwLayerIndex2,
                      eLayerChangeType ChangeType );

	  virtual ~CUndoLayerChange();

    std::string Type() { return std::string( "LayerChange" ); }

    BOOL Restore();
    CUndoState* CreateComplementCopy();

  protected:

    CUndoLayerChange();

};

#endif // !defined(AFX_UNDOLAYERCHANGE_H__918290A4_D81A_11D5_AC02_00104B6795C0__INCLUDED_)
