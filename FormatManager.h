#ifndef _FORMATMANAGER_H_
#define _FORMATMANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

#include <GR/GRTypes.h>

#include "PainterEnums.h"


class AbstractImageFileFormat;

struct tFileFormatSupport
{
  AbstractImageFileFormat*      m_pLoadingFormat;
  AbstractImageFileFormat*      m_pSavingFormat;

  int                   m_LoadingFreeImageFormat;
  int                   m_SavingFreeImageFormat;

  GR::String        m_Extension,
                        m_Description;


  tFileFormatSupport( const GR::String& Description = GR::String(),
                      const GR::String& Extension = GR::String(),
                      AbstractImageFileFormat* pLoad = NULL, int fLoad = -1,
                      AbstractImageFileFormat* pSave = NULL, int fSave = -1 ) :
    m_Description( Description ),
    m_Extension( Extension ),
    m_LoadingFreeImageFormat( fLoad ),
    m_SavingFreeImageFormat( fSave ),
    m_pLoadingFormat( pLoad ),
    m_pSavingFormat( pSave )
  {
  }

  tFileFormatSupport( const GR::String& Description,
                      const GR::String& Extension,
                      AbstractImageFileFormat* pLoad, AbstractImageFileFormat* pSave ) :
    m_Description( Description ),
    m_Extension( Extension ),
    m_LoadingFreeImageFormat( -1 ),
    m_SavingFreeImageFormat( -1 ),
    m_pLoadingFormat( pLoad ),
    m_pSavingFormat( pSave )
  {
  }

  tFileFormatSupport( const GR::String& Description,
                      const GR::String& Extension,
                      int fLoad,
                      int fSave ) :
    m_Description( Description ),
    m_Extension( Extension ),
    m_LoadingFreeImageFormat( fLoad ),
    m_SavingFreeImageFormat( fSave ),
    m_pLoadingFormat( NULL ),
    m_pSavingFormat( NULL )
  {
  }


};

struct ImageSet;

namespace GR
{
  namespace Graphic
  {
    class ImageData;
  };
};

class CFormatManager
{

  public:

    typedef std::map<SaveType,tFileFormatSupport>   tMapSupportedFormats;

    tMapSupportedFormats                m_mapSupportedFormats;


    ~CFormatManager();

    static CFormatManager& Instance();

    ImageSet*                          Load( SaveType& saveType, const GR::Char* FileName );
    bool                                Save( const SaveType saveType, const GR::Char* FileName, ImageSet* pSet );

    bool                                SupportsFormat( const SaveType saveType );
    tFileFormatSupport*                 GetFormat( const SaveType saveType );


  protected:



    CFormatManager();

    bool                                SaveWithFreeImage( int fif, const GR::Char* FileName, ImageSet* pSet );
    bool                                ExtraProcessing( const GR::Char* FileName, const SaveType saveType, tFileFormatSupport& Format, GR::Graphic::ImageData* pData );


};


#endif // _FORMATMANAGER_H_