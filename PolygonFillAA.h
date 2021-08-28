#pragma once

#include <grafik\polygonfill.h>



class CPolygonFillAA : public CPolygonFill
{

  public:

    void                    DrawScanLine( int YToScan, GR::Graphic::ContextDescriptor& cdTarget, GR::u32 dwColor );

};
