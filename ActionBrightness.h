#if !defined(AFX_ACTIONBRIGHTNESS_H__14FAEBD3_F3CC_4F04_B44D_A4209CCC409C__INCLUDED_)
#define AFX_ACTIONBRIGHTNESS_H__14FAEBD3_F3CC_4F04_B44D_A4209CCC409C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageAction.h"



class ActionBrightness : public ImageAction  
{

  public:

    int                   m_BrightnessFaktor;


	  ActionBrightness();


    GR::Graphic::ContextDescriptor*   PerformAction( GR::Graphic::ContextDescriptor* pCDSource );

};

#endif // !defined(AFX_ACTIONBRIGHTNESS_H__14FAEBD3_F3CC_4F04_B44D_A4209CCC409C__INCLUDED_)
