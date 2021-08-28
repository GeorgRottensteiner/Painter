// UndoRename.h: Schnittstelle für die Klasse CUndoRename.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDORENAME_H__918290A4_D81A_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_UNDORENAME_H__918290A4_D81A_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoManager.h"

class DocumentInfo;

namespace GR
{
  namespace Graphic
  {
    class Image;
  };
};

class CUndoImageChange : public CUndoState  
{
  public:

    int                   m_iX1,
                          m_iY1,
                          m_iX2,
                          m_iY2;

    bool                  m_bMask;

    DocumentInfo         *m_pDocInfo;

    GR::Graphic::Image*            m_pImage;

    size_t                m_dwLayerIndex,
                          m_dwFrameIndex,
                          m_dwFunction;


    CUndoImageChange( int iX1, int iY1, int iX2, int iY2, 
                      size_t dwFrameIndex, size_t dwLayerIndex,
                      size_t dwFunction, DocumentInfo *pDocInfo,
                      bool bMask );

	  virtual ~CUndoImageChange();

    std::string Type() { return std::string( "ImageChange" ); }

    BOOL Restore();
    CUndoState* CreateComplementCopy();

  protected:

    CUndoImageChange();

};

#endif // !defined(AFX_UNDORENAME_H__918290A4_D81A_11D5_AC02_00104B6795C0__INCLUDED_)
