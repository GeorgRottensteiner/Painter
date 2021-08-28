#pragma once


#include "Layer.h"

#include <vector>


class Frame
{

  public:


    typedef std::vector<CLayer*>        tVectLayers;


    tVectLayers               Layers;

    GR::Graphic::Palette      Palette;

    GR::u32                   Delay;


    size_t                    LayerCount();


    Frame();

};
