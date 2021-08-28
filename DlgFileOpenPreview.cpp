#include "stdafx.h"
#include "Painter.h"
#include "DlgFileOpenPreview.h"
#include "Settings.h"

#include <String/Convert.h>
#include <String/Path.h>
#include <String/StringUtil.h>

#include <Grafik/ImageFormate/ImageFormatManager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CPainterApp      theApp;




IMPLEMENT_DYNAMIC(CPainterFileOpen, CFileDialogEx)

CPainterFileOpen::CPainterFileOpen(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialogEx(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_ofn.Flags |= OFN_ENABLETEMPLATE;
	m_ofn.hInstance = AfxGetInstanceHandle();
  m_ofn.lpTemplateName = MAKEINTRESOURCE( IDD_DIALOG_FILE_OPEN );
}



BEGIN_MESSAGE_MAP(CPainterFileOpen, CFileDialogEx)
	//{{AFX_MSG_MAP(CPainterFileOpen)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CPainterFileOpen::OnDestroy()
{
  SafeDelete( pPreviewPage );
}



void CPainterFileOpen::GetPreview( const GR::Char* Temp )
{
  GR::Font*           pFont;

  RECT                rc;

  int                 i,
                      j;


  if ( pPreviewPage != NULL )
  {
    delete pPreviewPage;
    pPreviewPage = NULL;
  }

  GR::String      fileName = Temp;
  GR::String      fileExtension = GR::Strings::ToUpper( Path::Extension( fileName ) );

  GR::Graphic::ImageData*   pData = ImageFormatManager::Instance().LoadData( Temp );

  if ( pData )
  {
    GR::Graphic::Image    Image( *pData );

    if ( Image.GetDepth() )
    {
      pPreviewPage = new GR::Graphic::GDIPage();
      pPreviewPage->Create( GetSafeHwnd(), pData->Width(), pData->Height(), Image.GetDepth() );
      if ( pData->Palette().Entries() )
      {
        pPreviewPage->SetPalette( &pData->Palette() );
      }
      else if ( pPreviewPage->GetDepth() <= 8 )
      {
        pPreviewPage->SetPalette( &theApp.m_PainterPalette );
      }

      Image.PutImage( pPreviewPage, 0, 0 );
    }

    Width     = pData->Width();
    Height    = pData->Height();
    BitDepth  = pData->BitsProPixel();
    FileType  = OPEN_FILE::TYPE::EINZELBILD;
    delete pData;
  }
  else if ( fileExtension == "FNX" )
  {
    // ein 8-bit-Font
    GetDlgItem( IDC_STATIC_FILE_OPEN_PREVIEW )->GetClientRect( &rc );
    pFont = new GR::Font();
    pFont->LoadFNT( Temp, 8 );
    pPreviewPage = new GR::Graphic::GDIPage();
    pPreviewPage->Create( GetSafeHwnd(), rc.right - rc.left, rc.bottom - rc.top, 8 );
    pPreviewPage->SetPalette( &theApp.m_PainterPalette );
    pPreviewPage->Box( 0, 0, pPreviewPage->GetWidth() - 1, pPreviewPage->GetHeight() - 1, 0 );
    pFont->PrintFont( pPreviewPage, 1, 1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", IMAGE_METHOD_PLAIN, 0 );
    pFont->PrintFont( pPreviewPage, 1, 1 + pFont->GetLetter( 65 )->GetHeight() + 2, "abcdefghijklmnopqrstuvwxyz", IMAGE_METHOD_PLAIN, 0 );
    pFont->PrintFont( pPreviewPage, 1, 1 + 2 * pFont->GetLetter( 65 )->GetHeight() + 4, "0123456789", IMAGE_METHOD_PLAIN, 0 );

    Width     = pFont->GetLetter( 65 )->GetWidth();
    Height    = pFont->GetLetter( 65 )->GetHeight();
    BitDepth  = 8;
    FileType  = OPEN_FILE::TYPE::FONT;

    delete pFont;
  }
  else if ( fileExtension == "FNH" )
  {
    // ein 16-bit-Font
    GetDlgItem( IDC_STATIC_FILE_OPEN_PREVIEW )->GetClientRect( &rc );
    pFont = new GR::Font();
    pFont->LoadFNT( Temp, 16 );
    pPreviewPage = new GR::Graphic::GDIPage();
    pPreviewPage->Create( GetSafeHwnd(), rc.right - rc.left, rc.bottom - rc.top, 15 );
    pPreviewPage->SetPalette( &theApp.m_PainterPalette );
    pPreviewPage->Box( 0, 0, pPreviewPage->GetWidth() - 1, pPreviewPage->GetHeight() - 1, 0 );
    pFont->PrintFont( pPreviewPage, 1, 1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", IMAGE_METHOD_PLAIN, 0 );
    pFont->PrintFont( pPreviewPage, 1, 1 + pFont->GetLetter( 65 )->GetHeight() + 2, "abcdefghijklmnopqrstuvwxyz", IMAGE_METHOD_PLAIN, 0 );
    pFont->PrintFont( pPreviewPage, 1, 1 + 2 * pFont->GetLetter( 65 )->GetHeight() + 4, "0123456789", IMAGE_METHOD_PLAIN, 0 );

    Width     = pFont->GetLetter( 65 )->GetWidth();
    Height    = pFont->GetLetter( 65 )->GetHeight();
    BitDepth  = 16;
    FileType  = OPEN_FILE::TYPE::FONT;

    delete pFont;
  }
  else if ( fileExtension == "FNL" )
  {
    // ein 24-bit-Font
    GetDlgItem( IDC_STATIC_FILE_OPEN_PREVIEW )->GetClientRect( &rc );
    pFont = new GR::Font();
    pFont->LoadFNT( Temp, 24 );
    pPreviewPage = new GR::Graphic::GDIPage();
    pPreviewPage->Create( GetSafeHwnd(), rc.right - rc.left, rc.bottom - rc.top, 24 );
    pPreviewPage->SetPalette( &theApp.m_PainterPalette );
    pPreviewPage->Box( 0, 0, pPreviewPage->GetWidth() - 1, pPreviewPage->GetHeight() - 1, 0 );
    pFont->PrintFont( pPreviewPage, 1, 1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", IMAGE_METHOD_PLAIN, 0 );
    pFont->PrintFont( pPreviewPage, 1, 1 + pFont->GetLetter( 65 )->GetHeight() + 2, "abcdefghijklmnopqrstuvwxyz", IMAGE_METHOD_PLAIN, 0 );
    pFont->PrintFont( pPreviewPage, 1, 1 + 2 * pFont->GetLetter( 65 )->GetHeight() + 4, "0123456789", IMAGE_METHOD_PLAIN, 0 );

    Width     = pFont->GetLetter( 65 )->GetWidth();
    Height    = pFont->GetLetter( 65 )->GetHeight();
    BitDepth  = 24;
    FileType  = OPEN_FILE::TYPE::FONT;

    delete pFont;
  }
  else if ( fileExtension == "PAL" )
  {
    GR::IO::FileStream*   pFile;
    unsigned char         ucPalette[768];


    // eine Palette
    pPreviewPage = new GR::Graphic::GDIPage();
    pPreviewPage->Create( GetSafeHwnd(), 160, 160, 16 );

    pFile = new GR::IO::FileStream( Temp );
    pFile->ReadBlock( ucPalette, 768 );
    pFile->Close();
    delete pFile;
    for ( i = 0; i < 16; i++ )
    {
      for ( j = 0; j < 16; j++ )
      {
        pPreviewPage->Box( i * 10, j * 10, i * 10 + 9, j * 10 + 9,
              pPreviewPage->GetRGB256(   ( ucPalette[3 * ( i + j * 16 )] << 16 )
                                      + ( ucPalette[3 * ( i + j * 16 ) + 1] << 8 )
                                      + ( ucPalette[3 * ( i + j * 16 ) + 2] ) ) );
      }
    }
    BitDepth = 8;
    FileType = OPEN_FILE::TYPE::PALETTE;
  }
  else if ( fileExtension == "ACT" )
  {
    GR::IO::FileStream        *pFile;
    unsigned char   ucPalette[768];


    // eine Photoshop-Palette
    pPreviewPage = new GR::Graphic::GDIPage();
    pPreviewPage->Create( GetSafeHwnd(), 160, 160, 16 );

    pFile = new GR::IO::FileStream( Temp );
    pFile->ReadBlock( ucPalette, 768 );
    pFile->Close();
    delete pFile;
    for ( i = 0; i < 16; i++ )
    {
      for ( j = 0; j < 16; j++ )
      {
        pPreviewPage->Box( i * 10, j * 10, i * 10 + 9, j * 10 + 9,
              pPreviewPage->GetRGB256(   ( ucPalette[3 * ( i + j * 16 )] << 16 )
                                      + ( ucPalette[3 * ( i + j * 16 ) + 1] << 8 )
                                      + ( ucPalette[3 * ( i + j * 16 ) + 2] ) ) );
      }
    }
    BitDepth = 8;
    FileType = OPEN_FILE::TYPE::PALETTE;
  }
  else if ( fileExtension == "IGF" )
  {
    // ein IGF
    GR::IO::FileStream        *pFile;
    unsigned char   ucType;


    pFile = new GR::IO::FileStream( Temp );
    if ( pFile->IsGood() )
    {
      ucType = pFile->ReadU8();
      if ( ( ucType == GR::Graphic::IGFType::ANIMATION )
      ||   ( ucType == GR::Graphic::IGFType::ANIMATION_EXTENDED ) )
      {
        // Animation
        GR::Animation     animDummy = GR::Animation( Temp );
        pPreviewPage = new GR::Graphic::GDIPage();
        pPreviewPage->Create( GetSafeHwnd(), animDummy.GetWidth(), animDummy.GetHeight(), animDummy.GetFirstImage()->GetDepth() );
        if ( animDummy.GetFirstImage()->GetDepth() <= 8 )
        {
          pPreviewPage->SetPalette( &theApp.m_PainterPalette );
        }
        animDummy.PutAnimation( pPreviewPage, 0, 0, IMAGE_METHOD_PLAIN );

        Width     = animDummy.GetWidth();
        Height    = animDummy.GetHeight();
        BitDepth  = animDummy.GetFirstImage()->GetDepth();
        Frames    = animDummy.GetFrames();
        FileType  = OPEN_FILE::TYPE::ANIMATION;
      }
      else if ( ucType == GR::Graphic::IGFType::PALETTE )
      {
        // eine Palette
        unsigned char   ucPalette[768];

        pPreviewPage = new GR::Graphic::GDIPage();
        pPreviewPage->Create( GetSafeHwnd(), 160, 160, 16 );
        pFile->ReadU8();
        pFile->ReadU32();

        pFile->ReadBlock( ucPalette, 768 );
        for ( i = 0; i < 16; i++ )
        {
          for ( j = 0; j < 16; j++ )
          {
            pPreviewPage->Box( i * 10, j * 10, i * 10 + 9, j * 10 + 9,
            pPreviewPage->GetRGB256(   ( ucPalette[3 * ( i + j * 16 )] << 16 )
                                    + ( ucPalette[3 * ( i + j * 16 ) + 1] << 8 )
                                    + ( ucPalette[3 * ( i + j * 16 ) + 2] ) ) );
          }
        }
        BitDepth = 8;
        FileType = OPEN_FILE::TYPE::PALETTE;
      }
    }
    pFile->Close();
    delete pFile;
  }
  GetDlgItem( IDC_STATIC_FILE_OPEN_PREVIEW )->InvalidateRect( NULL, FALSE );
}



BOOL CPainterFileOpen::OnInitDialog() 
{
	CFileDialogEx::OnInitDialog();

	return TRUE;
}



void CPainterFileOpen::OnInitDone() 
{
  // Preview auf OWNER_DRAW setzen
  GetDlgItem( IDC_STATIC_FILE_OPEN_PREVIEW )->ModifyStyle( 0, SS_OWNERDRAW );

  // Preview enabled?
  pPreviewPage = NULL;
  if ( pSettings->GetSetting( "ShowLoadPreview" ) )
  {
    SendDlgItemMessage( IDC_CHECK_FILE_OPEN_PREVIEW, BM_SETCHECK, 1, 0 );
  }

  GetParent()->CenterWindow();
}



UINT_PTR CPainterFileOpen::HookProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
  if ( uiMsg == WM_DRAWITEM )
  {
    OnDrawItem( (int)wParam, (DRAWITEMSTRUCT*)lParam );
    return TRUE;
  }
  else if ( uiMsg == WM_COMMAND )
  {
    if ( (HWND)lParam == GetDlgItem( IDC_CHECK_FILE_OPEN_PREVIEW )->GetSafeHwnd() )
    {
      if ( SendDlgItemMessage( IDC_CHECK_FILE_OPEN_PREVIEW, BM_GETCHECK, 0, 0 ) )
      {
        pSettings->SetSetting( "ShowLoadPreview", 1 );
      }
      else
      {
        pSettings->SetSetting( "ShowLoadPreview", 0 );
      }
      if ( pSettings->GetSetting( "ShowLoadPreview" ) )
      {
        GR::WChar        szTemp[MAX_PATH];


        GetParent()->SendMessageW( CDM_GETFILEPATH, (WPARAM)MAX_PATH, (LPARAM)(LPTSTR)szTemp );

        GetPreview( GR::Convert::ToUTF8( szTemp ).c_str() );
      }
      else
      {
        if ( pPreviewPage != NULL )
        {
          delete pPreviewPage;
          pPreviewPage = NULL;
        }
        GetDlgItem( IDC_STATIC_FILE_OPEN_PREVIEW )->InvalidateRect( NULL, FALSE );
      }
    }
  }
  return CFileDialogEx::HookProc( hWnd, uiMsg, wParam, lParam );
}



void CPainterFileOpen::OnFileNameChange()
{
  GR::WChar            szTemp[MAX_PATH];


  FileType  = OPEN_FILE::TYPE::UNKNOWN;
  Width     = 0;
  Height    = 0;
  BitDepth  = 0;
  Frames    = 0;
  GetParent()->SendMessageW( CDM_GETFILEPATH, (WPARAM)MAX_PATH, (LPARAM)(LPTSTR)szTemp );

  GR::String    fileName = GR::Convert::ToUTF8( szTemp );

  if ( Path::Extension( fileName ).length() == 3 )
  {
    if ( pSettings->GetSetting( "ShowLoadPreview" ) )
    {
      GetPreview( fileName.c_str() );
    }
  }
  if ( BitDepth == 15 )
  {
    BitDepth = 16;
  }
  if ( Width == 0 )
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_WIDTH, WM_SETTEXT, 0, (LPARAM)_T( "unbekannt" ) );
  }
  else
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_WIDTH, WM_SETTEXT, 0, (LPARAM)GR::Convert::ToUTF16( CMisc::printf( "%d", Width ) ).c_str() );
  }
  if ( Height == 0 )
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_HEIGHT, WM_SETTEXT, 0, (LPARAM)_T( "unbekannt" ) );
  }
  else
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_HEIGHT, WM_SETTEXT, 0, (LPARAM)GR::Convert::ToUTF16( CMisc::printf( "%d", Height ) ).c_str() );
  }
  if ( BitDepth == 0 )
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_DEPTH, WM_SETTEXT, 0, (LPARAM)_T( "unbekannt" ) );
  }
  else
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_DEPTH, WM_SETTEXT, 0, (LPARAM)GR::Convert::ToUTF16( CMisc::printf( "%d", BitDepth ) ).c_str() );
  }
  if ( Frames == 0 )
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_FRAMES, WM_SETTEXT, 0, (LPARAM)_T( "n.v." ) );
  }
  else
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_FRAMES, WM_SETTEXT, 0, (LPARAM)GR::Convert::ToUTF16( CMisc::printf( "%d", Frames ) ).c_str() );
  }
  if ( FileType == OPEN_FILE::TYPE::UNKNOWN )
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_TYPE, WM_SETTEXT, 0, (LPARAM)_T( "unbekannt" ) );
  }
  else if ( FileType == OPEN_FILE::TYPE::EINZELBILD )
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_TYPE, WM_SETTEXT, 0, (LPARAM)_T( "Einzelbild" ) );
  }
  else if ( FileType == OPEN_FILE::TYPE::FONT )
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_TYPE, WM_SETTEXT, 0, (LPARAM)_T( "Font" ) );
  }
  else if ( FileType == OPEN_FILE::TYPE::ANIMATION )
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_TYPE, WM_SETTEXT, 0, (LPARAM)_T( "Animation" ) );
  }
  else if ( FileType == OPEN_FILE::TYPE::PALETTE )
  {
    SendDlgItemMessage( IDC_STATIC_FILE_OPEN_TYPE, WM_SETTEXT, 0, (LPARAM)_T( "Palette" ) );
  }
}



void CPainterFileOpen::OnFolderChange()
{
}



void CPainterFileOpen::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
  if ( lpDrawItemStruct->hwndItem == GetDlgItem( IDC_STATIC_FILE_OPEN_PREVIEW )->GetSafeHwnd() )
  {
    HPEN      hPen,
              hOldPen;

    HBRUSH    hBrush;


    if ( pPreviewPage != NULL )
    {
      hBrush = CreateSolidBrush( RGB( 255, 255, 255 ) );

      hPen = CreatePen( PS_SOLID, 1, 0 );
      hOldPen = (HPEN)SelectObject( lpDrawItemStruct->hDC, hPen );
      Rectangle( lpDrawItemStruct->hDC, 
          lpDrawItemStruct->rcItem.left,
          lpDrawItemStruct->rcItem.top,
          lpDrawItemStruct->rcItem.right,
          lpDrawItemStruct->rcItem.bottom );
      SelectObject( lpDrawItemStruct->hDC, hOldPen );
      DeleteObject( hPen );
      DeleteObject( hBrush );

      // Größenverhältnis beibehalten!
      int width = lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left,
          height = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top,
          xOffset,
          yOffset;
      float faktorW,
            faktorH,
            zoomFactor;
      faktorW = (float)pPreviewPage->GetWidth() / (float)width;
      faktorH = (float)pPreviewPage->GetHeight() / (float)height;
      if ( faktorW > faktorH )
      {
        zoomFactor = faktorW;
      }
      else
      {
        zoomFactor = faktorH;
      }

      width = (int)( (float)pPreviewPage->GetWidth() / zoomFactor );
      height = (int)( (float)pPreviewPage->GetHeight() / zoomFactor );

      xOffset = ( lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left - width ) / 2;
      yOffset = ( lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top - height ) / 2;

      hBrush = CreateSolidBrush( 0 );
      // Stauch-Bild
      SetStretchBltMode( lpDrawItemStruct->hDC, COLORONCOLOR );
      StretchBlt( lpDrawItemStruct->hDC, xOffset, yOffset,
                  width,
                  height,
                  pPreviewPage->GetDC(), 0, 0, 
                  pPreviewPage->GetWidth(), pPreviewPage->GetHeight(),
                  SRCCOPY );
      // schwarzer Rahmen drumrum
      hPen = CreatePen( PS_SOLID, 1, 0 );
      hOldPen = (HPEN)SelectObject( lpDrawItemStruct->hDC, hPen );
      FrameRect( lpDrawItemStruct->hDC, 
                 &lpDrawItemStruct->rcItem,
                 hBrush );
      SelectObject( lpDrawItemStruct->hDC, hOldPen );
      DeleteObject( hPen );
      DeleteObject( hBrush );
    }
    else
    {
      hBrush = CreateSolidBrush( RGB( 255, 255, 255 ) );

      hPen = CreatePen( PS_SOLID, 1, 0 );
      hOldPen = (HPEN)SelectObject( lpDrawItemStruct->hDC, hPen );
      Rectangle( lpDrawItemStruct->hDC, 
          lpDrawItemStruct->rcItem.left,
          lpDrawItemStruct->rcItem.top,
          lpDrawItemStruct->rcItem.right,
          lpDrawItemStruct->rcItem.bottom );
      SelectObject( lpDrawItemStruct->hDC, hOldPen );
      DeleteObject( hPen );
      DeleteObject( hBrush );
    }
  }

	CFileDialogEx::OnDrawItem( nIDCtl, lpDrawItemStruct );
}


