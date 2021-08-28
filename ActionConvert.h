#if !defined(AFX_ACTIONCONVERT_H__E891B34C_8602_40AC_8E22_C7E1D3F10670__INCLUDED_)
#define AFX_ACTIONCONVERT_H__E891B34C_8602_40AC_8E22_C7E1D3F10670__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageAction.h"



class ActionConvert : public ImageAction  
{

  public:

    GR::Graphic::eImageFormat      m_FormatTarget;



	  ActionConvert();


    virtual GR::Graphic::ContextDescriptor*       PerformAction( GR::Graphic::ContextDescriptor* pCDSource );

};

#endif // !defined(AFX_ACTIONCONVERT_H__E891B34C_8602_40AC_8E22_C7E1D3F10670__INCLUDED_)
