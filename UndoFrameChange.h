#if !defined(AFX_UNDOFRAMECHANGE_H__F1C73D45_E970_4F8E_AD10_287958A5A528__INCLUDED_)
#define AFX_UNDOFRAMECHANGE_H__F1C73D45_E970_4F8E_AD10_287958A5A528__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoManager.h"

#include <GR/GRTypes.h>

#pragma warning ( disable: 4786 )
#include <vector>

class DocumentInfo;
class CLayer;

class CUndoFrameChange : public CUndoState  
{
  public:

    enum eFrameChangeType
    {
      UFC_INSERT_FRAME = 0,
      UFC_REMOVE_FRAME,
    };

    DocumentInfo*             m_pDocInfo;

    std::vector<CLayer>       m_vectImages;

    eFrameChangeType          m_fcType;

    GR::u32                   m_dwFrameIndex;

    GR::u32                   m_UserData;



    CUndoFrameChange( DocumentInfo* pDocInfo,
                      eFrameChangeType fcType,
                      GR::u32 dwFrameIndex,
                      GR::u32 userData = 0 );

	  virtual ~CUndoFrameChange();

    std::string Type() { return std::string( "FrameChange" ); }

    BOOL Restore();
    CUndoState* CreateComplementCopy();

  protected:
    
    CUndoFrameChange();

};

#endif // !defined(AFX_UNDOFRAMECHANGE_H__F1C73D45_E970_4F8E_AD10_287958A5A528__INCLUDED_)
