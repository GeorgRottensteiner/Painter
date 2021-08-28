#ifndef __IMPORT_INCLUDED__
#define __IMPORT_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <GR/GRTypes.h>




class CPainterFileOpen;
class DocumentInfo;

namespace GR
{
  namespace Graphic
  {
    class Image;
    class Palette;
    class ImageData;
  };
};


HBITMAP CreateBitmapFromImageData( GR::Graphic::ImageData* pData );
HBITMAP CreateBitmapFromImage( GR::Graphic::Image* pImage, GR::Graphic::Palette* pPalette );
bool LoadPSD( DocumentInfo* pDocInfo, const GR::Char* FileName );

CPainterFileOpen *OpenFileDialog( DWORD dwFileTypes );


#endif // __IMPORT_INCLUDED__