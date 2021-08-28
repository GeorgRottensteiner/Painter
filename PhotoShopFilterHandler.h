#if !defined(AFX_PHOTOSHOPFILTERHANDLER1_H__7EED60F7_4BC0_4A70_8272_6C01A0A60A92__INCLUDED_)
#define AFX_PHOTOSHOPFILTERHANDLER1_H__7EED60F7_4BC0_4A70_8272_6C01A0A60A92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CViewInfo;
class GR::Graphic::Image;
struct FilterRecord;
struct Rect
{
	short top;
	short left;
	short bottom;
	short right;
};


class CPhotoShopFilterHandler  
{

  public:

    GR::Graphic::Image*            m_pWorkImage;
    GR::Graphic::Image*            m_pTargetImage;

    FilterRecord*         m_pFilter;

    Rect                  outRect;

    DWORD                 outStride,
                          outPlanes;


	  CPhotoShopFilterHandler();
	  virtual ~CPhotoShopFilterHandler();


    void                  OpenPhotoShopFilter( CViewInfo& viewInfo, const GR::String& strFilterFileName );
    void                  PSStoreResult( FilterRecord& filter, GR::Graphic::Image *pImage );
    void                  PSPrepareFilterRect( FilterRecord& filter, GR::Graphic::Image *pImage, GR::Graphic::Image *pOut );
    void                  PSReportError( FilterRecord& filter, short result );

};

extern CPhotoShopFilterHandler   PhotoShopFilterHandler;

#endif // !defined(AFX_PHOTOSHOPFILTERHANDLER1_H__7EED60F7_4BC0_4A70_8272_6C01A0A60A92__INCLUDED_)
