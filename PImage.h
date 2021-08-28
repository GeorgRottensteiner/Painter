#if !defined(AFX_PIMAGE_H__90CC1BD4_D836_4CCA_9BD8_5A1C0BA7C57C__INCLUDED_)
#define AFX_PIMAGE_H__90CC1BD4_D836_4CCA_9BD8_5A1C0BA7C57C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



namespace GR
{
  namespace Graphic
  {
    class Image;
  }
}



class PImage  
{

  public:

    PImage();
    ~PImage();

    PImage( const PImage& rhs );

    GR::Graphic::Image*        m_pImage;
    GR::Graphic::Image*        m_pImageMask;

};

#endif // !defined(AFX_PIMAGE_H__90CC1BD4_D836_4CCA_9BD8_5A1C0BA7C57C__INCLUDED_)
