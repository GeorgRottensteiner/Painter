#pragma once


class GR::Graphic::ContextDescriptor;

namespace Resampler
{

  enum eSampleMethod
  {
    SM_BOX = 0,
    SM_TRIANGLE,
    SM_HERMITE,
    SM_BELL,
    SM_CUBICBSPLINE,
    SM_LANCZOS3,
    SM_MITCHELL,
    SM_COSINE,
    SM_CATMULLROM,
    SM_QUADRATIC,
    SM_QUADRATICBSPLINE,
    SM_CUBICCONVOLUTION,
    SM_LANCZOS8,
  };

  bool      Resample( const GR::Graphic::ContextDescriptor& cdIn, GR::Graphic::ContextDescriptor& cdOut, const eSampleMethod Method );

};
