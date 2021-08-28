#if !defined(AFX_ActionContrast_H__14FAEBD3_F3CC_4F04_B44D_A4209CCC409C__INCLUDED_)
#define AFX_ActionContrast_H__14FAEBD3_F3CC_4F04_B44D_A4209CCC409C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageAction.h"



class ActionContrast : public ImageAction  
{

  public:

    int                               m_LowerBound;
    int                               m_UpperBound;



    GR::Graphic::ContextDescriptor    m_CDWork;


	  ActionContrast();

    GR::Graphic::ContextDescriptor*   PerformAction( GR::Graphic::ContextDescriptor* pCDSource );

};

#endif // !defined(AFX_ActionContrast_H__14FAEBD3_F3CC_4F04_B44D_A4209CCC409C__INCLUDED_)
