#include "stdafx.h"
#include "settings.h"
#include "ScriptTool.h"
#include "PainterGFXTools.h"

#include <Grafik/ContextDescriptor.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



ScriptTool::ScriptTool()
{

}



ScriptTool::~ScriptTool()
{

}



void ScriptTool::Brightness( int Brightness )
{
  CLayer*   pActiveLayer = pSettings->m_pActiveDocInfo->ActiveLayer();

  GR::Graphic::ContextDescriptor   cdSource( pActiveLayer->GetImage(),
                                             pSettings->m_pActiveDocInfo->GetPalette( pSettings->m_pActiveDocInfo->CurrentFrame() ) );

  GR::Graphic::Image               imageTarget( pActiveLayer->GetImage() );

  GR::Graphic::ContextDescriptor   cdTarget( &imageTarget,
                                             pSettings->m_pActiveDocInfo->GetPalette( pSettings->m_pActiveDocInfo->CurrentFrame() ) );

  int         faktor,
              r,
              g,
              b;

  DWORD       pixel;


  faktor = Brightness;
  if ( faktor < 0 )
  {
    faktor = 0;
  }
  if ( faktor >= 10000 )
  {
    faktor = 10000;
  }

  for ( int j = 0; j < cdSource.Height(); j++ )
  {
    for ( int i = 0; i < cdSource.Width(); i++ )
    {
      pixel = cdSource.GetPixel( i, j );
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
      cdTarget.PutPixel( i, j, ( r << 16 ) + ( g << 8 ) + b );
    }
  }

  cdTarget.CopyArea( 0, 0, cdTarget.Width(), cdTarget.Height(), 0, 0, &cdSource );
}
