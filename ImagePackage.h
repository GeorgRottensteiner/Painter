#if !defined(AFX_IMAGEPACKAGE_H__45ED4B41_6C8F_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_IMAGEPACKAGE_H__45ED4B41_6C8F_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Grafik/Image.h>
#include <Grafik/Palette.h>



class CPainterImagePackage  
{
  public:

    GR::Graphic::Image*       m_pImage;
    GR::Graphic::Image*       m_pImageMask;

    GR::Graphic::Palette      m_Palette;

	  CPainterImagePackage();
    CPainterImagePackage( GR::Graphic::Image* pImage, GR::Graphic::Palette* pPal = NULL, GR::Graphic::Image* pImageMask = NULL );
	  virtual ~CPainterImagePackage();

};

#endif // !defined(AFX_IMAGEPACKAGE_H__45ED4B41_6C8F_11D5_AC02_00104B6795C0__INCLUDED_)
