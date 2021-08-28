/*-Includes-------------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

#ifndef __PAINTERGFXTOOLS_INCLUDED__
#define __PAINTERGFXTOOLS_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning ( disable:4786 )
#include <Grafik/Image.h>
#include <IO/FileStream.h>
#include <Grafik/GfxPage.h>
#include <Grafik/Palette.h>

#include "MaskedContextDescriptor.h"
#include "DocumentInfo.h"
#include "Layer.h"

#include "FreeImage.h"

#include "ImagePackage.h"



/*-Prototypen-----------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

class PainterBrush;
class CViewInfo;

GR::Graphic::Image* ConvertImage1to16( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette );
GR::Graphic::Image* ConvertImage4to16( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette );
GR::Graphic::Image* ConvertImage8to16( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette );
GR::Graphic::Image* ConvertImage24to16( GR::Graphic::Image *pImageSource );
GR::Graphic::Image* ConvertImage32to16( GR::Graphic::Image *pImageSource );
           
GR::Graphic::Image* ConvertImage1to24( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette );
GR::Graphic::Image* ConvertImage4to24( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette );
GR::Graphic::Image* ConvertImage8to24( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette );
GR::Graphic::Image* ConvertImage16to24( GR::Graphic::Image *pImageSource );
GR::Graphic::Image* ConvertImage32to24( GR::Graphic::Image *pImageSource );
           
GR::Graphic::Image* ConvertImage1to32( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette );
GR::Graphic::Image* ConvertImage4to32( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette );
GR::Graphic::Image* ConvertImage8to32( GR::Graphic::Image *pImageSource, GR::Graphic::Palette *pPalette );
GR::Graphic::Image* ConvertImage16to32( GR::Graphic::Image *pImageSource );
GR::Graphic::Image* ConvertImage24to32( GR::Graphic::Image *pImageSource );
            
GR::Graphic::Image* MapImageToPalette( CPainterImagePackage* pPackSource, GR::Graphic::Palette* pPalette );

GR::Graphic::Image *RotateImage( GR::Graphic::Image *pOldImage );
void HMirrorImage( GR::Graphic::Image *pImage );
void VMirrorImage( GR::Graphic::Image *pImage );
GR::Graphic::Image* ResizeImage( GR::Graphic::Image* pSource, GR::Graphic::Palette* pPal, int iWidth, int iHeight, DWORD dwResizeType );

void DarkenPixel( GR::Graphic::Image *pImage, GR::Graphic::Palette *pPalette, int iX, int iY );
void BrightenPixel( GR::Graphic::Image *pImage, GR::Graphic::Palette *pPalette, int iX, int iY );
void MFCGFXStretchUpdate( HWND hWnd, HDC hDC,
                          signed long slTargetXOffset, signed long slTargetYOffset,
                          signed long slTargetWidth, signed long slTargetHeight,
                          GR::Graphic::GDIPage *pPage,
                          signed long slSourceXOffset, signed long slSourceYOffset,
                          signed long slSourceWidth, signed long slSourceHeight );


FIBITMAP *ConvertImageToFreeImageDIB( GR::Graphic::Image *pImage, GR::Graphic::Palette *pPalette );

GR::Graphic::Image *GetImageFromBitmap( HBITMAP hbm );

HANDLE BitmapToHDIB( HBITMAP hBitmap, HPALETTE hPal );
CPainterImagePackage *CreateImagePackFromHDIB( HGLOBAL hmem );
HANDLE CreateHDIBFromImage( GR::Graphic::Image *pImage, GR::Graphic::Palette *pPal );

void OpenPhotoShopFilter( CViewInfo& viewInfo, std::string strFilterFileName );


#endif