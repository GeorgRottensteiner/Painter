#include "stdafx.h"
#include "painter.h"
#include "PImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



PImage::PImage() :
  m_pImage( NULL ),
  m_pImageMask( NULL )
{
}



PImage::~PImage()
{
  delete m_pImage;
  m_pImage = NULL;
  delete m_pImageMask;
  m_pImageMask = NULL;
}



PImage::PImage( const PImage& rhs ) :
  m_pImage( NULL ),
  m_pImageMask( NULL )
{
  if ( rhs.m_pImage )
  {
    m_pImage = new GR::Graphic::Image( rhs.m_pImage );
  }
  if ( rhs.m_pImageMask )
  {
    m_pImageMask = new GR::Graphic::Image( rhs.m_pImageMask );
  }
}