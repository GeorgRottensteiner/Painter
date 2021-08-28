// ContextDescriptor.h: Schnittstelle für die Klasse CMaskedContextDescriptor.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTEXTDESCRIPTOR_H__5938D9A0_79E4_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_CONTEXTDESCRIPTOR_H__5938D9A0_79E4_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Grafik\ContextDescriptor.h>

#include <GR/GRTypes.h>

namespace GR
{
  namespace Graphic
  {
    class Palette;
    class Image;
    class GFXPage;
  };
};

class CMaskedContextDescriptor : public GR::Graphic::ContextDescriptor
{

public:

  void*             m_pMaskData;
  /*
  void              *m_pData,
                    *m_pMaskData;

  GR::Graphic::Palette    *m_pPalette;

  DWORD             m_dwLineOffsetInBytes,
                    m_dwBytesPerPixel,
                    m_dwBitDepth;

  int               m_iWidth,
                    m_iHeight;
                    */


	CMaskedContextDescriptor();
  CMaskedContextDescriptor( GR::Graphic::Image* pImage, GR::Graphic::Palette* pPalette );
  CMaskedContextDescriptor( GR::Graphic::GFXPage* pPage, GR::Graphic::Palette* pPalette );
	virtual ~CMaskedContextDescriptor();

  virtual void      PutPixel( int iX, int iY, unsigned long dwColor );
  GR::u8            GetMaskValueAt( int iX, int iY );

};

#endif // !defined(AFX_CONTEXTDESCRIPTOR_H__5938D9A0_79E4_11D5_AC02_00104B6795C0__INCLUDED_)
