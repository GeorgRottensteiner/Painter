#if !defined(AFX_LAYER_H__6D7F84E0_C685_11D3_924A_00104B6795C0__INCLUDED_)
#define AFX_LAYER_H__6D7F84E0_C685_11D3_924A_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable: 4786 )
#include <Grafik/Image.h>
#include <string>

#include "PImage.h"



class CLayer  
{
  
  protected:

    PImage         m_PImage;


  public:

    enum LayerType
    {
      LT_INVALID = 0,
      LT_NORMAL,
      LT_ADDITIV,
      LT_SUBTRAKTIV,
    };

    LayerType       m_Type;

    bool            m_Visible,
                    m_Transparent,
                    m_HasMask,
                    m_MaskEnabled;

    GR::u32         m_Transparenz,
                    m_Alpha;

    GR::String      m_Name;


	  CLayer();
	  virtual ~CLayer();


    void            SetLayerImage( GR::Graphic::Image* pImage );
    void            SetLayerMask( GR::Graphic::Image* pImageMask );

    GR::Graphic::Image*     GetImage();
    GR::Graphic::Image*     GetMask();
    void            Clear();

    void            DrawLayerWithMask1( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage* pPage, GR::Graphic::Palette* pPalette, const GR::tRect& rc );
    void            DrawLayerWithMask4( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage* pPage, GR::Graphic::Palette *pPalette, const GR::tRect& rc );
    void            DrawLayerWithMask8( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage* pPage, GR::Graphic::Palette *pPalette, const GR::tRect& rc );
    void            DrawLayerWithMask555( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage* pPage, const GR::tRect& rc );
    void            DrawLayerWithMask565( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage* pPage, const GR::tRect& rc );
    void            DrawLayerWithMask24( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage* pPage, const GR::tRect& rc );
    void            DrawLayerWithMask32( GR::Graphic::Image* pImage, int iX, int iY, GR::Graphic::GFXPage* pPage, const GR::tRect& rc );

    void            DrawImage( GR::Graphic::GFXPage* pPage, GR::Graphic::Palette *pPalette, const GR::tRect& rect, GR::Graphic::Image* pImage = NULL, int iX = 0, int iY = 0 );

    void            SetTransparencyIndex( bool Transparent, GR::u32 TransparentColor );

};

#endif // !defined(AFX_LAYER_H__6D7F84E0_C685_11D3_924A_00104B6795C0__INCLUDED_)
