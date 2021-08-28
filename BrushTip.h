#pragma once

#include <Grafik/Image.h>

#include <windows.h>



struct tBrushTip
{

  public:

    GR::Graphic::Image      m_ImageTipMask;

    HBITMAP                 m_hbmTipMask;



    tBrushTip( GR::Graphic::Image* pImage );
    ~tBrushTip();
};
