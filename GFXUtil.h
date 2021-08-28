// GFXUtil.h: interface for the CGFXUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GFXUTIL_H__BD9081D5_AD6B_4F15_9545_7F4B76DAB906__INCLUDED_)
#define AFX_GFXUTIL_H__BD9081D5_AD6B_4F15_9545_7F4B76DAB906__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning ( disable:4786 )
#include <list>
typedef std::list<GR::Graphic::Image*>           tImageList;

class CGFXUtil  
{

  public:


  	CGFXUtil();
	  virtual ~CGFXUtil();


    static void             ReplaceColor( tImageList& list, DWORD dwColor1, DWORD dwColor2, GR::Graphic::Image* pMask = NULL );
    static void             SwapColor( tImageList& list, GR::u32 Color1, GR::u32 Color2, GR::Graphic::Image* pMask = NULL );

};

#endif // !defined(AFX_GFXUTIL_H__BD9081D5_AD6B_4F15_9545_7F4B76DAB906__INCLUDED_)
