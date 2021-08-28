#include "stdafx.h"
#include "ActionConvert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



ActionConvert::ActionConvert() :
  ImageAction( "Color Convert" ),
  m_FormatTarget( GR::Graphic::IF_UNKNOWN )
{

}



GR::Graphic::ContextDescriptor* ActionConvert::PerformAction( GR::Graphic::ContextDescriptor* pCDSource )
{
  GR::Graphic::ImageData*   pData = pCDSource->ConvertTo( m_FormatTarget );
  if ( pData == NULL )
  {
    return NULL;
  }

  GR::Graphic::ContextDescriptor*   pCDResult = new GR::Graphic::ContextDescriptor();

  pCDResult->CreateData( pData->Width(), pData->Height(), pData->ImageFormat() );

  GR::Graphic::ContextDescriptor    cdDummy;

  cdDummy.Attach( pData->Width(), pData->Height(), pData->LineOffsetInBytes(), pData->ImageFormat(), pData->Data() );

  cdDummy.CopyArea( 0, 0, pData->Width(), pData->Height(), 0, 0, pCDResult );

  delete pData;

  return pCDResult;
}