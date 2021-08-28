#include "stdafx.h"
#include "ImagePackage.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CPainterImagePackage::CPainterImagePackage() :
  m_pImage( NULL ),
  m_pImageMask( NULL )
{

}



CPainterImagePackage::CPainterImagePackage( GR::Graphic::Image* pImage, GR::Graphic::Palette* pPal, GR::Graphic::Image* pImageMask ) :
  m_pImage( pImage ),
  m_pImageMask( pImageMask )
{
  if ( pPal )
  {
    m_Palette = *pPal;
  }
}



CPainterImagePackage::~CPainterImagePackage()
{

}
