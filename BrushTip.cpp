#include "Brushtip.h"



tBrushTip::tBrushTip( GR::Graphic::Image* pImage ) :
  m_ImageTipMask( *pImage ),
  m_hbmTipMask( NULL )
{
}



tBrushTip::~tBrushTip()
{
  DeleteObject( m_hbmTipMask );
}
