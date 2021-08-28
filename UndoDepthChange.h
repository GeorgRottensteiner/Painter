// UndoRename.h: Schnittstelle für die Klasse CUndoRename.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDODEPTHCHANGE_H__918290A4_D81A_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_UNDODEPTHCHANGE_H__918290A4_D81A_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UndoManager.h"

#pragma warning ( disable: 4786 )
#include <vector>
#include "PImage.h"

class DocumentInfo;

namespace GR
{
  namespace Graphic
  {
    class Palette;
    class Image;
  };
};

class CUndoDepthChange : public CUndoState  
{
  public:

    DWORD                     m_dwOldDepth;

    DocumentInfo*            m_pDocInfo;

    std::vector<std::vector<PImage> >  m_vectImages;
    std::vector<GR::Graphic::Palette>           m_vectPalettes;


    CUndoDepthChange( DocumentInfo *pDocInfo );

	  virtual ~CUndoDepthChange();

    std::string Type() { return std::string( "DepthChange" ); }

    BOOL Restore();
    CUndoState* CreateComplementCopy();

  protected:

    CUndoDepthChange();

};

#endif // !defined(AFX_UNDODEPTHCHANGE_H__918290A4_D81A_11D5_AC02_00104B6795C0__INCLUDED_)
