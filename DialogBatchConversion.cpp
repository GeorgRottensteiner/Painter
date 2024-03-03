#include "stdafx.h"
#include "Settings.h"
#include "DialogBatchConversion.h"
#include "DocumentInfo.h"

#include "PainterDoc.h"
#include "PainterGfxTools.h"

#include "FormatManager.h"

#include "ActionConvert.h"

#include <Grafik/ImageFormate/ImageFormatManager.h>

#include <Misc/Misc.h>

#include <String/Convert.h>
#include <String/Path.h>



#undef OS_WINDOWS
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDialogBatchConversion::CDialogBatchConversion(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogBatchConversion::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogBatchConversion)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CDialogBatchConversion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogBatchConversion)
	DDX_Control(pDX, IDC_EDIT_BATCH_PALETTE, m_EditPalette);
	DDX_Control(pDX, IDC_EDIT_BATCH_TARGET, m_EditTargetPath);
	DDX_Control(pDX, IDC_COMBO_BATCH_DEPTH, m_ComboDepth);
	DDX_Control(pDX, IDC_COMBO_BATCH_TYPE, m_ComboType);
	DDX_Control(pDX, IDC_LIST_BATCH_LOG, m_ListLog);
	DDX_Control(pDX, IDC_LIST_BATCH_FILES, m_BatchFileList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogBatchConversion, CDialog)
	//{{AFX_MSG_MAP(CDialogBatchConversion)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_ADDFILE, OnButtonBatchAddFile)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_START, OnButtonBatchStart)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_REMOVEFILE, OnButtonBatchRemovefile)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_REMOVEALLFILES, OnButtonBatchRemoveAllFiles)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_BROWSEPALETTE, OnButtonBatchBrowsePalette)
	ON_BN_CLICKED(IDC_BUTTON_BATCH_BROWSE_TARGET_PATH, OnButtonBatchBrowseTargetPath)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CDialogBatchConversion::OnInitDialog() 
{
	CDialog::OnInitDialog();


  // speicherbare Typen
  CFormatManager::tMapSupportedFormats::iterator    itFormat( CFormatManager::Instance().m_mapSupportedFormats.begin() );
  while ( itFormat != CFormatManager::Instance().m_mapSupportedFormats.end() )
  {
    tFileFormatSupport&   Format = itFormat->second;

    m_ComboType.SetItemData( m_ComboType.AddString( GR::Convert::ToUTF16( Format.m_Description ).c_str() ), itFormat->first );

    ++itFormat;
  }

  // Farbtiefen
  int dummy = m_ComboDepth.AddString( _T( "keep as is" ) );
  m_ComboDepth.SetItemData( dummy, 0 );
  
  dummy = m_ComboDepth.AddString( _T( "8 Bit (256 Farben)" ) );

  ActionConvert* pAction = new ActionConvert();
  pAction->m_FormatTarget = GR::Graphic::IF_PALETTED;
  m_ComboDepth.SetItemData( dummy, (DWORD_PTR)pAction );

  dummy = m_ComboDepth.AddString( _T( "16 Bit (HiColor)" ) );
  pAction = new ActionConvert();
  pAction->m_FormatTarget = GR::Graphic::IF_X1R5G5B5;
  m_ComboDepth.SetItemData( dummy, (DWORD_PTR)pAction );

  dummy = m_ComboDepth.AddString( _T( "24 Bit (True Color)" ) );
  pAction = new ActionConvert();
  pAction->m_FormatTarget = GR::Graphic::IF_R8G8B8;
  m_ComboDepth.SetItemData( dummy, (DWORD_PTR)pAction );

  dummy = m_ComboDepth.AddString( _T( "32 Bit (True Color)" ) );
  pAction = new ActionConvert();
  pAction->m_FormatTarget = GR::Graphic::IF_X8R8G8B8;
  m_ComboDepth.SetItemData( dummy, (DWORD_PTR)pAction );

  m_ComboDepth.SetCurSel( 0 );

	return TRUE;
}



void CDialogBatchConversion::OnButtonBatchAddFile() 
{
  CFileDialog             fileDlg( TRUE, NULL, NULL, 
                                   OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
                                   _T( "Supported Formats|*.IGF;*.BTN;*.ANX;*.ANH;*.GIF;*.PCX;*.TGA;*.BMP;*.BTH;*.IFF|IGF Files (*.IGF)|*.IGF|BTH Files (*.BTH)|*.BTH|BTN Files (*.BTN)|*.BTN|BTX Files (*.BTX)|*.BTX|ANX Files (*.ANX)|*.ANX|PCX Files (*.PCX)|*.PCX|GIF Files (*.GIF)|*.GIF|BMP Files (*.BMP)|*.BMP|TGA Files (*.TGA)|*.TGA|IFF Files (*.IFF)|*.IFF|All Files (*.*)|*.*||\0\0" ),
                                   this );

  POSITION                pos;

  CString                 cs;

  GR::tChar               szTemp[MAX_PATH];

  WCHAR                   Buffer[3000];

  memset( Buffer, 0, sizeof( Buffer ) );

  fileDlg.m_ofn.lpstrFile = Buffer;
  fileDlg.m_ofn.nMaxFile = 2995;

  if ( fileDlg.DoModal() == IDOK )
  {
    pos = fileDlg.GetStartPosition();
    do
    {
      cs = fileDlg.GetNextPathName( pos );
      wsprintf( szTemp, cs );
      m_BatchFileList.AddString( szTemp );
    }
    while ( pos != NULL );
  }
}



void CDialogBatchConversion::OnButtonBatchStart() 
{
  SaveType          saveType;

  DWORD             dwCount,
                    i,
                    iDummy,
                    dwConverted,
                    dwTargetDepth;

  GR::String    sourceFile,
                    paletteFile;

  GR::Graphic::Image*        pDummyImage;

  GR::Graphic::Palette*   pPalette = NULL;
  GR::Graphic::Palette*   pSourcePalette = NULL;


  m_ListLog.ResetContent();
  iDummy = m_ComboDepth.GetCurSel();
  if ( iDummy != CB_ERR )
  {
    dwTargetDepth = (DWORD)m_ComboDepth.GetItemData( iDummy );
  }
  else
  {
    // ungültige Farbtiefe??
    CDialog::OnOK();
    return;
  }
  iDummy = m_ComboType.GetCurSel();
  if ( iDummy != CB_ERR )
  {
    saveType = (SaveType)m_ComboType.GetItemData( iDummy );
  }
  else
  {
    // ungültiger Save-Typ?
    CDialog::OnOK();
    return;
  }

  tFileFormatSupport* pFormat = CFormatManager::Instance().GetFormat( saveType );
  if ( pFormat == NULL )
  {
    CDialog::OnOK();
    return;
  }

  pPalette = NULL;
  CString     cstrGnu;

  m_EditPalette.GetWindowText( cstrGnu );
  if ( !cstrGnu.IsEmpty() )
  {
    // es gibt ein Palettenfile!
    pPalette = new GR::Graphic::Palette();
    pPalette->LoadPAL( GR::Convert::ToUTF8( (LPCTSTR)cstrGnu ).c_str() );
  }


  dwConverted = 0;
  dwCount = m_BatchFileList.GetCount();
  for ( i = 0; i < dwCount; i++ )
  {
    GR::String      targetFile,
                        targetPath;

    CString             cstrGnu;

    m_BatchFileList.GetText( i, cstrGnu );
    sourceFile = GR::Convert::ToUTF8( (LPCTSTR)cstrGnu );

    m_EditTargetPath.GetWindowText( cstrGnu );
    targetPath = GR::Convert::ToUTF8( (LPCTSTR)cstrGnu );
    targetPath = Path::AddSeparator( targetPath );
    targetFile = Path::StripPath( sourceFile );

    targetFile = Path::RenameExtension( targetFile, CMisc::printf( ".%s", pFormat->m_Extension.c_str() ) );
    targetPath = Path::Append( targetPath, targetFile );

    if ( targetPath.empty() )
    {
      // dieses File wird übersprungen
      m_ListLog.AddString( GR::Convert::ToUTF16( CMisc::printf( "File %s kann nicht geschrieben werden.", sourceFile.c_str() ) ).c_str() );
      m_ListLog.UpdateWindow();
      continue;
    }

    // jetzt umwandeln
    m_ListLog.AddString( GR::Convert::ToUTF16( CMisc::printf( "Lade %s.", sourceFile.c_str() ) ).c_str() );
    pDummyImage = NULL;

    GR::Graphic::ImageData*   pData = ImageFormatManager::Instance().LoadData( sourceFile.c_str() );
    if ( pData != NULL )
    {
      pDummyImage = new GR::Graphic::Image( *pData );
      pSourcePalette = new GR::Graphic::Palette( pData->Palette() );
      delete pData;
    }
    if ( pDummyImage != NULL )
    {
      ImageAction*     pAction = (ImageAction*)dwTargetDepth;

      GR::Graphic::Palette*   pTargetPalette = NULL;
      
      if ( pPalette != NULL )
      {
        SafeDelete( pTargetPalette );
        pTargetPalette = new GR::Graphic::Palette( *pPalette );
      }

      if ( pAction )
      {
        GR::Graphic::ContextDescriptor*   pCDSource = new GR::Graphic::ContextDescriptor( pDummyImage, pSourcePalette );

        GR::Graphic::ContextDescriptor*   pCDTarget = pAction->PerformAction( pCDSource );

        SafeDelete( pCDSource );
        SafeDelete( pDummyImage );

        if ( pCDTarget )
        {
          pDummyImage = pCDTarget->CreateImage();

          if ( ( pTargetPalette == NULL )
          &&   ( pCDTarget->Palette().Entries() ) )
          {
            pTargetPalette = new GR::Graphic::Palette( pCDTarget->Palette() );
          }
          delete pCDTarget;
        }
      }
      
      if ( pDummyImage )
      {
        ImageSet   Set;

        GR::Graphic::ImageData*   pData = pDummyImage->CreateImageData();
        if ( pSourcePalette )
        {
          pData->SetPalette( *pSourcePalette );
        }

        Set.AddFrame( pData );

        if ( CFormatManager::Instance().Save( saveType, targetPath.c_str(), &Set ) )
        {
          dwConverted++;
          m_ListLog.AddString( GR::Convert::ToUTF16( CMisc::printf( "Schreibe %s.", targetPath.c_str() ) ).c_str() );
        }
        else
        {
          m_ListLog.AddString( GR::Convert::ToUTF16( CMisc::printf( "Fehler beim Schreiben von %s.", targetPath.c_str() ) ).c_str() );
        }
        m_ListLog.UpdateWindow();
        SafeDelete( pDummyImage );
        SafeDelete( pTargetPalette );
      }
      else
      {
        m_ListLog.AddString( _T( "-fehlgeschlagen" ) );
        m_ListLog.UpdateWindow();
      }
    }
    else
    {
      m_ListLog.AddString( _T( "-fehlgeschlagen" ) );
      m_ListLog.UpdateWindow();
    }
    delete pSourcePalette;
    pSourcePalette = NULL;
  }
  if ( pPalette != NULL )
  {
    delete pPalette;
    pPalette = NULL;
  }
  m_ListLog.AddString( _T( "Konvertierung beendet." ) );
  m_ListLog.AddString( GR::Convert::ToUTF16( CMisc::printf( "%d Bilder von %d konvertiert.", dwConverted, dwCount ) ).c_str() );
  m_ListLog.UpdateWindow();
}



void CDialogBatchConversion::OnButtonBatchRemovefile() 
{
  int dummy = m_BatchFileList.GetCurSel();
  if ( dummy != LB_ERR )
  {
    m_BatchFileList.DeleteString( dummy );
  }
}



void CDialogBatchConversion::OnButtonBatchRemoveAllFiles() 
{
  m_BatchFileList.ResetContent();
}



void CDialogBatchConversion::OnButtonBatchBrowsePalette() 
{
  GR::tChar*              pBuffer;


  CFileDialog     fileDlg( TRUE, NULL, NULL, 
                           OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
                           _T( "Unterstützte Formate|*.PAL;*.ACT|PAL Files (*.PAL)|*.PAL|ACT Files (*.ACT)|*.ACT|All Files (*.*)|*.*||\0\0" ),
                           this );

  pBuffer = (GR::tChar*)GlobalAlloc( GPTR, 4000 );
  fileDlg.m_ofn.lpstrFile = pBuffer;
  fileDlg.m_ofn.nMaxFile = 1995;

  if ( fileDlg.DoModal() == IDOK )
  {
    m_EditPalette.SetWindowText( fileDlg.m_ofn.lpstrFile );
  }
  GlobalFree( pBuffer );
}



void CDialogBatchConversion::OnButtonBatchBrowseTargetPath() 
{
  GR::tChar      szTemp[MAX_PATH];

  BROWSEINFO    bi;
  LPITEMIDLIST  lpiil;

  bi.hwndOwner = GetSafeHwnd();
  bi.pidlRoot = NULL;
  bi.pszDisplayName = szTemp;
  bi.lpszTitle = _T( "Choose Target Path:" );
  bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;
  bi.lpfn = NULL;
  bi.lParam = 0;
  bi.iImage = 0;

  lpiil = SHBrowseForFolder( &bi );

  if ( lpiil != NULL )
  {
    SHGetPathFromIDList( lpiil, szTemp );

    if ( szTemp[_tcslen( szTemp ) - 1] != 92 )
    {
      wsprintf( szTemp, _T( "%s\\" ), szTemp );
    }
    LPMALLOC    lpMalloc;
    SHGetMalloc( &lpMalloc );

    lpMalloc->Free( lpiil );
    m_EditTargetPath.SetWindowText( szTemp );
  }
}



void CDialogBatchConversion::OnDestroy() 
{
  for ( int i = 0; i < m_ComboDepth.GetCount(); i++ )
  {
    ImageAction*   pAction = (ImageAction*)m_ComboDepth.GetItemData( i );
    if ( pAction )
    {
      delete pAction;
    }
  }
	CDialog::OnDestroy();
}
