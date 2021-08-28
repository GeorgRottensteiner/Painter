// UndoSizeChange.h: interface for the CUndoSizeChange class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOSIZECHANGE_H__F1C73D45_E970_4F8E_AD10_287958A5A528__INCLUDED_)
#define AFX_UNDOSIZECHANGE_H__F1C73D45_E970_4F8E_AD10_287958A5A528__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoManager.h"

#pragma warning ( disable: 4786 )
#include <vector>
#include "PImage.h"

class DocumentInfo;
class GR::Graphic::Image;

class CUndoSizeChange : public CUndoState  
{
  public:

    int                     m_iWidth,
                            m_iHeight;

    DocumentInfo           *m_pDocInfo;

    GR::Graphic::Image               *m_pImageSelection;

    std::vector<std::vector<PImage> >  m_vectImages;

    CUndoSizeChange( DocumentInfo* pDocInfo );

	  virtual ~CUndoSizeChange();

    std::string Type() { return std::string( "SizeChange" ); }

    BOOL Restore();
    CUndoState* CreateComplementCopy();

  protected:
    
    CUndoSizeChange();

};

#endif // !defined(AFX_UNDOSIZECHANGE_H__F1C73D45_E970_4F8E_AD10_287958A5A528__INCLUDED_)
