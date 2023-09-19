#if !defined(AFX_SETTINGS_H__061534C0_63D3_11D5_AC02_00104B6795C0__INCLUDED_)
#define AFX_SETTINGS_H__061534C0_63D3_11D5_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include <string>
#include <map>
#include <vector>
#include <list>

#include "ImagePackage.h"
#include "PainterBrush.h"

#include <Grafik/Gradient.h>

#include <Interface/INotifyMember.h>


enum DocumentType
{
  DT_INVALID          = 0,
  DT_IMAGE            = 1,
  DT_FONT             = 2,
};

class GR::Graphic::Image;

class DocumentInfo;

class ViewInfo;

class CDlgDebug;

class CPattern
{
  public:

    GR::String          m_FileName,
                            m_strDescription;

    CPainterImagePackage*   m_pImagePack;


    CPattern()
      : m_pImagePack( NULL )
    {}

};

class CPhotoShopFilterEntry
{
  public:

    GR::String    m_FileName,
                      m_strDescription,
                      m_strCategory,
                      m_strEntryPoint;
};

typedef std::list<GR::Graphic::Image*>           tImageList;

class CColorPalette;
class CAbstractFormat;

struct tBrushTip;

class CSettings : public INotifyMember<GR::u32>
{

  protected:

    GR::u32                                   m_Color[4];

    std::map<GR::String, GR::String>  m_mapSettings;

    CDlgDebug*                                m_pDebug;


  public:

    enum class ColorCategory
    {
      WORKCOLOR = 0,
      WORKCOLOR_2,
      WORKCOLOR_8BIT,
      WORKCOLOR_2_8BIT,
    };

    enum eBrushType
    {
      BR_SOLID,
      BR_GRADIENT,
      BR_PATTERN,
      BR_BRIGHTEN,
      BR_DARKEN,
      BR_SMEAR,
    };

    enum eFloatingSelectionFlags
    {
      FLOAT_TRANSPARENT       = 0x00000001,
    };

    enum eFileTypes
    {
      FT_IMAGE          = 0x00000001,
      FT_ANIMATION      = 0x00000002,
      FT_FONT           = 0x00000004,
    };

    enum eFunction
    {
      F_NOTHING         = 0,
      F_BOX,
      F_CIRCLE,
      F_FILLEDCIRCLE,
      F_FILL,
      F_FREEHAND_1,
      F_FREEHAND_2,
      F_LINE,
      F_ZOOM,
      F_SELECTION,
      F_SELECTION_COLOR,
      F_BRIGHTEN,
      F_DARKEN,
      F_TEXT,
      F_RECTANGLE,
    };

    enum eFillType
    {
      FILL_FLOOD,
      FILL_RGB,
    };

    PainterBrush                       brushForeground,
                                        brushBackground;

    GR::Graphic::Palette                m_AlphaPalette;

    GR::Graphic::Gradient               m_Gradient;

    GR::u32                               m_dwFunction,
                                        m_dwClipboardFormat;

    HBITMAP                             m_hbmBackground;

    HBRUSH                              m_brushBackground;

    HWND                                m_hwndMainFrame;

    std::list<CPattern*>                m_listPatterns;

    std::list<CPhotoShopFilterEntry>    m_listPhotoShopFilter;

    DocumentInfo                       *m_pActiveDocInfo;

    ViewInfo                           *m_pActiveViewInfo;

    tBrushTip*                          m_pCurrentBrushTip;


  	CSettings();
	  virtual ~CSettings();


    void                                SetColor( ColorCategory Index, GR::u32 Color );
    GR::u32                             GetRGBColor( ColorCategory Index );
    GR::u32                             GetColorRef( ColorCategory Index );

    void SetSetting( const GR::String& strName, int iValue );
    void SetSettingFloat( const GR::String& strName, float fValue );
    void SetSetting( const GR::String& strName, const GR::String& strValue );

    int GetSetting( const GR::String& strName, int iDefault = 0 );
    float GetSettingFloat( const GR::String& strName, float fDefault = 0.0f );
    GR::String GetSettingString( const GR::String& strName, const GR::String& strDefault = GR::String() );

    void SnapToGrid( POINT *pt );

    void AddPattern( CPattern *pP );
    BOOL RemovePattern( CPattern *pP );


    void Log( const GR::Char* Text );

    void UpdateActiveViewInfo();

    virtual void          OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pMemberFrom );

};


extern CSettings    *pSettings;

#endif // !defined(AFX_SETTINGS_H__061534C0_63D3_11D5_AC02_00104B6795C0__INCLUDED_)
