#pragma once

#include <Grafik/Image.h>



class CPattern;
class CViewInfo;
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

    void                PutBrush( CViewInfo* pViewInfo, CMaskedContextDescriptor *pCD, int iX, int iY, int iAlphaValue, GR::tRect* pRCBounds = NULL );
    void                ApplyBrushTipOnMask( CViewInfo *pViewInfo, 
                                             CMaskedContextDescriptor *pCD, 
                                             GR::Graphic::Image *pImageMask, 
                                             GR::tRect* pRect = NULL );
    void                DrawOnPage( GR::Graphic::GFXPage* pPage, GR::u32 dwColor );

};
