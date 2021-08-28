#include "Frame.h"



Frame::Frame() :
  Delay( 100 )
{
}



size_t Frame::LayerCount()
{
  return Layers.size();
}