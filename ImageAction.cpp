#include "stdafx.h"
#include "painter.h"
#include "ImageAction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



ImageAction::ImageAction( const GR::String& Name ) :
  m_Description( Name )
{

}



ImageAction::~ImageAction()
{
}



bool ImageAction::PerformAction( const std::list<GR::Graphic::ContextDescriptor*>& Images,
                                 std::list<GR::Graphic::ContextDescriptor*>& Target )
{
  std::list<GR::Graphic::ContextDescriptor*>::const_iterator    it( Images.begin() );
  while ( it != Images.end() )
  {
    GR::Graphic::ContextDescriptor*   pCDSource = *it;

    GR::Graphic::ContextDescriptor*   pCDTarget = PerformAction( pCDSource );

    if ( pCDTarget )
    {
      Target.push_back( pCDTarget );
    }
    else
    {
      std::list<GR::Graphic::ContextDescriptor*>::iterator    it2( Target.begin() );
      while ( it2 != Target.end() )
      {
        GR::Graphic::ContextDescriptor*   pCD = *it2;

        pCD->SetPalette();
        delete pCD;

        ++it2;
      }
      Target.clear();
      return false;
    }

    ++it;
  }

  return true;
}



GR::Graphic::ContextDescriptor* ImageAction::PerformAction( GR::Graphic::ContextDescriptor* pCDSource )
{
  return NULL;
}
