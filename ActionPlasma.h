#if !defined(AFX_ACTIONPLASMA_H__07B65D91_F0FB_4C4F_8303_C6498290FF4D__INCLUDED_)
#define AFX_ACTIONPLASMA_H__07B65D91_F0FB_4C4F_8303_C6498290FF4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageAction.h"



class ActionPlasma : public ImageAction  
{

  public:



	  ActionPlasma();

    GR::Graphic::ContextDescriptor*           PerformAction( GR::Graphic::ContextDescriptor* pCDSource );

};

#endif // !defined(AFX_ACTIONPLASMA_H__07B65D91_F0FB_4C4F_8303_C6498290FF4D__INCLUDED_)
