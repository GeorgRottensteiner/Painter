#include "stdafx.h"
#include "ActionBrightness.h"
#include <Grafik/Palette.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



ActionBrightness::ActionBrightness() :
  ImageAction( "Brightness" ),
  m_BrightnessFaktor( 100 )
{
}



GR::Graphic::ContextDescriptor* ActionBrightness::PerformAction( GR::Graphic::ContextDescriptor* pCDSource )
{
  GR::Graphic::ContextDescriptor*   pCDTarget = new GR::Graphic::ContextDescriptor();
  pCDTarget->CreateData( pCDSource->Width(), pCDSource->Height(), pCDSource->ImageFormat() );
  if ( pCDSource->Palette().Data() )
  {
    pCDTarget->SetPalette( pCDSource->Palette() );
  }

  int         faktor = m_BrightnessFaktor,
              r,
              g,
              b;

  DWORD       pixel;


  faktor = m_BrightnessFaktor;
  if ( faktor < 0 )
  {
    faktor = 0;
  }
  if ( faktor >= 10000 )
  {
    faktor = 10000;
  }

  for ( int j = 0; j < pCDSource->Height(); j++ )
  {
    for ( int i = 0; i < pCDSource->Width(); i++ )
    {
      pixel = pCDSource->GetPixel( i, j );
      r = ( ( pixel & 0xff0000 ) >> 16 );
      g = ( ( pixel & 0xff00 ) >> 8 );
      b = ( pixel & 0xff );
      r = ( r * faktor ) / 100;
      g = ( g * faktor ) / 100;
      b = ( b * faktor ) / 100;
      if ( r > 255 )
      {
        r = 255;
      }
      if ( g > 255 )
      {
        g = 255;
      }
      if ( b > 255 )
      {
        b = 255;
      }
      pCDTarget->PutPixel( i, j, ( r << 16 ) + ( g << 8 ) + b );
    }
  }
  return pCDTarget;
}