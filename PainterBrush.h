#pragma once

#include <Grafik/Image.h>



class CPattern;
class ViewInfo;
class CMaskedContextDescriptor;

class PainterBrush
{

  public:

    int                 Type;

    float               Angle;

    CPattern*           pPattern;


    PainterBrush();
    ~PainterBrush();

    void                SetPattern( CPattern* pP );

    void                PutBrush( ViewInfo* pViewInfo, CMaskedContextDescriptor *pCD, int iX, int iY, int iAlphaValue, GR::tRect* pRCBounds = NULL );
    void                ApplyBrushTipOnMask( ViewInfo *pViewInfo, 
                                             CMaskedContextDescriptor *pCD, 
                                             GR::Graphic::Image *pImageMask, 
                                             GR::tRect* pRect = NULL );
    void                DrawOnPage( GR::Graphic::GFXPage* pPage, GR::u32 dwColor );

};
