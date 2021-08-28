#include "stdafx.h"
#include "ActionContrast.h"
#include <Grafik/Palette.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



ActionContrast::ActionContrast() :
  ImageAction( "Contrast" ),
  m_LowerBound( 0 ),
  m_UpperBound( 100 )
{
}



GR::Graphic::ContextDescriptor* ActionContrast::PerformAction( GR::Graphic::ContextDescriptor* pCDSource )
{
  if ( m_LowerBound > m_UpperBound )
  {
    m_LowerBound = m_UpperBound;
  }

  int     min = 100;
  int     max = 0;

  for ( int i = 0; i < pCDSource->Width(); ++i )
  {
    for ( int j = 0; j < pCDSource->Height(); ++j )
    {
      GR::u32   pixel = pCDSource->GetPixel( i, j );

      int       faktor = ( ( pixel & 0xff0000 ) >> 16 ) * 11
                        + ( ( pixel & 0x00ff00 ) >>  8 ) * 80
                        +   ( pixel & 0x0000ff ) * 9;
      faktor /= 100;
      if ( faktor > 255 )
      {
        faktor = 255;
      }
      if ( min > faktor )
      {
        min = faktor;
      }
      if ( max < faktor )
      {
        max = faktor;
      }
    }
  }

  GR::Graphic::ContextDescriptor*   pCDTarget = new GR::Graphic::ContextDescriptor();
  pCDTarget->CreateData( pCDSource->Width(), pCDSource->Height(), pCDSource->ImageFormat() );
  pCDTarget->SetPalette( pCDSource->Palette() );

  int         faktor,
              r,
              g,
              b;

  DWORD       pixel;


  for ( int j = 0; j < pCDSource->Height(); j++ )
  {
    for ( int i = 0; i < pCDSource->Width(); i++ )
    {
      pixel = pCDSource->GetPixel( i, j );
      r = ( ( pixel & 0xff0000 ) >> 16 );
      g = ( ( pixel & 0xff00 ) >> 8 );
      b = ( pixel & 0xff );

      faktor = ( r * 11 + g * 80 + b * 9 ) / 100;
      if ( faktor > 255 )
      {
        faktor = 255;
      }

      int   endFaktor = faktor;
      
      if ( max != min )
      {
        endFaktor = m_LowerBound + (int)( ( m_UpperBound - m_LowerBound ) * (float)faktor / ( max - min ) );
      }

      r = ( r * endFaktor ) / 100;
      g = ( g * endFaktor ) / 100;
      b = ( b * endFaktor ) / 100;
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