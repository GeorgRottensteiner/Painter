#if !defined(AFX_IMAGEACTION_H__FE5557EB_BC51_48C4_B953_F8D0786C270A__INCLUDED_)
#define AFX_IMAGEACTION_H__FE5557EB_BC51_48C4_B953_F8D0786C270A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786 )
#include <list>
#include <Grafik/ContextDescriptor.h>



class ImageAction  
{

  public:

    GR::String        m_Description;


    ImageAction( const GR::String& Name );
	  virtual ~ImageAction();


    bool              PerformAction( const std::list<GR::Graphic::ContextDescriptor*>& Images,
                                     std::list<GR::Graphic::ContextDescriptor*>& Target );

    virtual GR::Graphic::ContextDescriptor*     PerformAction( GR::Graphic::ContextDescriptor* pCDSource );

};

#endif // !defined(AFX_IMAGEACTION_H__FE5557EB_BC51_48C4_B953_F8D0786C270A__INCLUDED_)
