#ifndef FUNCTION_FILL_H
#define FUNCTION_FILL_H

#include "IPainterFunction.h"

#include <Grafik/Image.h>

#include <GR/GRTypes.h>

class CMaskedContextDescriptor;

class CFunctionFill : public IPainterFunction
{

  public:


    virtual void          ApplyAt( ViewInfo* pViewInfo, int X, int Y );

    bool                  CompareColors( GR::u32 Color1, GR::u32 Color2 );

    GR::Graphic::Image*   GetFillMask( CMaskedContextDescriptor* pCD, int x, int y, GR::tRect& Bounds );

};



#endif // FUNCTION_FILL_H