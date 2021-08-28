#include "stdafx.h"
#include "ActionPlasma.h"

#include <Grafik/ImageGenerator/Plasma.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



ActionPlasma::ActionPlasma() :
  ImageAction( "Plasma" )
{
}



GR::Graphic::ContextDescriptor* ActionPlasma::PerformAction( GR::Graphic::ContextDescriptor* pCDSource )
{
  if ( pCDSource->ImageFormat() != GR::Graphic::IF_X8R8G8B8 )
  {
    return NULL;
  }

  GR::Graphic::ContextDescriptor*   pCDTarget = new GR::Graphic::ContextDescriptor();
  pCDTarget->CreateData( pCDSource->Width(), pCDSource->Height(), pCDSource->ImageFormat() );

  PlasmaGenerator    Plasma;

  Plasma.Generate( *pCDTarget );

  return pCDTarget;
}