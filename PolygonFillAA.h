#pragma once

#include <grafik\polygonfill.h>



class CPolygonFillAA : public GR::Graphic::PolygonFill
{

  public:

    void                    DrawScanLine( int YToScan, GR::Graphic::ContextDescriptor& cdTarget, GR::u32 dwColor );

};
