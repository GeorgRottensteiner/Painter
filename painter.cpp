#include "stdafx.h"
#include "Painter.h"

#undef OS_WINDOWS
#include <Shlwapi.h>

#include "MainFrm.h"
#include "ChildFrm.h"
#include "PainterDoc.h"
#include "PainterView.h"
#include "PainterFontView.h"
#include "PainterFontDoc.h"
#include "NewDocumentDialog.h"
#include "DlgEffekte.h"
#include "ColorPicker.h"
#include "TextDialog.h"
#include "DlgFileOpenPreview.h"
#include "Import.h"

#include "Settings.h"

#include "PainterEnums.h"

#include <Grafik\ContextDescriptor.h>

#include <Setup\Version.h>
#include <String/Path.h>
#include <String/StringUtil.h>

#include <IO/FileUtil.h>
#include <WinSys/WinUtils.h>

#include <debug/debugclient.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma comment( lib, "shlwapi.lib" )

extern CSettings    *pSettings;


BEGIN_MESSAGE_MAP(CPainterApp, CWinApp)
	//{{AFX_MSG_MAP(CPainterApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_PAINT_ARROW, OnPaintArrow)
	ON_COMMAND(ID_PAINT_FREEHAND2, OnPaintFreehand2)
	ON_COMMAND(ID_PAINT_BOX, OnPaintBox)
	ON_COMMAND(ID_PAINT_CIRCLE, OnPaintCircle)
	ON_COMMAND(ID_PAINT_BRIGHTEN, OnPaintBrighten)
	ON_COMMAND(ID_PAINT_DARKEN, OnPaintDarken)
	ON_COMMAND(ID_PAINT_FILL, OnPaintFill)
	ON_COMMAND(ID_PAINT_FREEHAND1, OnPaintFreehand1)
	ON_COMMAND(ID_PAINT_LINE, OnPaintLine)
	ON_COMMAND(ID_PAINT_SELECTION, OnPaintSelection)
  ON_COMMAND(ID_PAINT_SELECTION_COLOR, OnPaintSelectionColor)
	ON_COMMAND(ID_PAINT_ZOOM, OnPaintZoom)
  ON_COMMAND(ID_PAINT_RECTANGLE, OnPaintRectangle)
  ON_COMMAND(ID_PAINT_FILLED_CIRCLE, OnPaintFilledCircle)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_BATCH_CONVERSION, OnBatchConversion)
	ON_COMMAND(ID_PAINT_TEXT, OnPaintText)
	ON_COMMAND(ID_MENU_ANI_CREATOR, OnMenuAniCreator)
	ON_COMMAND(ID_MENU_COLORPICKER, OnMenuColorPicker)
	ON_COMMAND(ID_SPEZIAL_EFFEKTE, OnSpezialEffekte)
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


unsigned char         ucPalColors[768] = { 0,0,0,0,0,42,0,42,0,0,42,42,42,0,0,42,0,42,42,21,0,42,42,42,21,21,21,21,21,63,21,
                                           63,21,21,63,63,63,21,21,63,21,63,63,63,21,63,63,63,59,59,59,55,55,55,52,52,52,48,
                                           48,48,45,45,45,42,42,42,38,38,38,35,35,35,31,31,31,28,28,28,25,25,25,21,21,21,18,
                                           18,18,14,14,14,11,11,11,8,8,8,63,0,0,59,0,0,56,0,0,53,0,0,50,0,0,47,0,0,44,0,0,41,
                                           0,0,38,0,0,34,0,0,31,0,0,28,0,0,25,0,0,22,0,0,19,0,0,16,0,0,63,54,54,63,46,46,63,
                                           39,39,63,31,31,63,23,23,63,16,16,63,8,8,63,0,0,63,42,23,63,38,16,63,34,8,63,30,0,
                                           57,27,0,51,24,0,45,21,0,39,19,0,63,63,54,63,63,46,63,63,39,63,63,31,63,62,23,63,61,
                                           16,63,61,8,63,61,0,57,54,0,51,49,0,45,43,0,39,39,0,33,33,0,28,27,0,22,21,0,16,16,
                                           0,52,63,23,49,63,16,45,63,8,40,63,0,36,57,0,32,51,0,29,45,0,24,39,0,54,63,54,47,63,
                                           46,39,63,39,32,63,31,24,63,23,16,63,16,8,63,8,0,63,0,0,63,0,0,59,0,0,56,0,0,53,0,
                                           1,50,0,1,47,0,1,44,0,1,41,0,1,38,0,1,34,0,1,31,0,1,28,0,1,25,0,1,22,0,1,19,0,1,16,
                                           0,54,63,63,46,63,63,39,63,63,31,63,62,23,63,63,16,63,63,8,63,63,0,63,63,0,57,57,0,
                                           51,51,0,45,45,0,39,39,0,33,33,0,28,28,0,22,22,0,16,16,23,47,63,16,44,63,8,42,63,0,
                                           39,63,0,35,57,0,31,51,0,27,45,0,23,39,54,54,63,46,47,63,39,39,63,31,32,63,23,24,63,
                                           16,16,63,8,9,63,0,1,63,0,0,63,0,0,59,0,0,56,0,0,53,0,0,50,0,0,47,0,0,44,0,0,41,0,
                                           0,38,0,0,34,0,0,31,0,0,28,0,0,25,0,0,22,0,0,19,0,0,16,60,54,63,57,46,63,54,39,63,
                                           52,31,63,50,23,63,47,16,63,45,8,63,42,0,63,38,0,57,32,0,51,29,0,45,24,0,39,20,0,33,
                                           17,0,28,13,0,22,10,0,16,63,54,63,63,46,63,63,39,63,63,31,63,63,23,63,63,16,63,63,
                                           8,63,63,0,63,56,0,57,50,0,51,45,0,45,39,0,39,33,0,33,27,0,28,22,0,22,16,0,16,63,58,
                                           55,63,56,52,63,54,49,63,53,47,63,51,44,63,49,41,63,47,39,63,46,36,63,44,32,63,41,
                                           28,63,39,24,60,37,23,58,35,22,55,34,21,52,32,20,50,31,19,47,30,18,45,28,17,42,26,
                                           16,40,25,15,39,24,14,36,23,13,34,22,12,32,20,11,29,19,10,27,18,9,23,16,8,21,15,7,
                                           18,14,6,16,12,6,14,11,5,10,8,3,38,38,47,33,33,41,28,28,36,23,23,31,19,19,25,14,14,
                                           20,9,9,15,5,5,10,63,59,9,63,55,7,63,52,6,63,48,5,63,45,3,63,41,2,63,38,1,63,35,0,
                                           57,31,0,52,27,0,46,23,0,41,20,0,36,16,0,30,12,0,25,8,0,20,5,0,63,35,0,63,29,0,63,
                                           23,0,63,17,0,63,11,0,63,5,0,63,0,0,0,0,0 };



CPainterApp         theApp;



CPainterApp::CPainterApp()
{

}



CPainterApp::~CPainterApp()
{

  delete pSettings;
  pSettings = NULL;

}



#define USING_MFC

GR::String    strResName,
                  strFileName;



BOOL CALLBACK EnumPIPLProc( HANDLE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG lParam )
{

  // Achtung, lpszName ist nicht unbedingt ein lesbarer String, kann auch ein Int sein
  HRSRC hrSrc = FindResource( (HINSTANCE)hModule,
                              lpszName,
                              _T( "PIPL" ) );

  if ( hrSrc == NULL )
  {
    dh::Log( "-find resource failed" );
    return TRUE;
  }

  //DWORD dwSize = SizeofResource( hInstDummy, hrSrc );

  HGLOBAL hGlob = LoadResource( (HINSTANCE)hModule,
                                hrSrc );

  if ( hGlob == NULL )
  {
    dh::Log( "-loadresource failed" );
    return TRUE;
  }

struct PIProperty
{
	// Vendor specific identifier:	
	DWORD vendorID;
	
	// Identifcation key for this property type:
	DWORD propertyKey;
	
	// Index within this property type. Must be unique for properties of
	// a given type in a PiPL:
	int  propertyID;
	
	// Length of following data array. Will be rounded to multiple of 4:
	int  propertyLength;
	
	// propertyLength bytes long. Must be multiple of 4:
	char   propertyData [1];

};

struct PIPropertyList
{
	// Properties data structure version number:
	int version;

	// Number of properties contained:
	int count;

	// The actual properties:
	PIProperty properties[1];

};

#define PIKindProperty 0x6b696e64L
#define PINameProperty 0x6e616d65L
#define PICategoryProperty 0x63617467L
#define PIWin32X86CodeProperty 0x77783836L

  PIPropertyList* pData = (PIPropertyList*)( ( (BYTE*)LockResource( hGlob ) ) + 2 );
  if ( pData == NULL )
  {
    dh::Log( "-lockresource failed" );
    return TRUE;
  }

  GR::String      strPSPlugInName,
                      strPSPlugInCategory,
                      strPSPlugInEntryPoint;


  BYTE *pDat = (BYTE*)pData;
  pDat += 8;  // wo kommen die 8 Bytes her, verflixt
  for ( int i = 0; i < pData->count; i++ )
  {
    PIProperty *pProp = (PIProperty*)pDat;

    switch ( pProp->propertyKey )
    {
      case PIKindProperty:
        {
          if ( memcmp ( pProp->propertyData, "MFB8", 4) != 0 )
          {
            dh::Log( "-no filter" );
            return TRUE;
          }
          break;
        }
      case PINameProperty:
        {
          for ( int i = 0; i < pProp->propertyData[0]; i++ )
          {
            strPSPlugInName += pProp->propertyData[1 + i];
          }
        }
        break;
      case PICategoryProperty:
        {
          for ( int i = 0; i < pProp->propertyData[0]; i++ )
          {
            strPSPlugInCategory += pProp->propertyData[1 + i];
          }
        }
        break;
      case PIWin32X86CodeProperty:
        {
          for ( int i = 0; i < pProp->propertyLength; i++ )
          {
            strPSPlugInEntryPoint += pProp->propertyData[i];
          }
        }
        break;
        /*
      default:
        AfxGetMainWnd()->MessageBox( CMisc::printf( "%c%c%c%c", 
                ( ( pProp->propertyKey & 0xff000000 ) >> 24 ),
                ( ( pProp->propertyKey & 0xff0000 ) >> 16 ),
                ( ( pProp->propertyKey & 0xff00 ) >> 8 ),
                ( ( pProp->propertyKey & 0xff ) ) ) );
        break;
        */
    }
    pDat += pProp->propertyLength + 16;
  }

  // sind die Parameter ok?
  if ( !strPSPlugInEntryPoint.length() )
  {
    dh::Log( "-no entrypoint" );
    return TRUE;
  }
  if ( !strPSPlugInCategory.length() )
  {
    dh::Log( "-no category" );
    return TRUE;
  }
  if ( !strPSPlugInName.length() )
  {
    dh::Log( "-no pluginname" );
    return TRUE;
  }

  // Wenn der Filter erkannt ist, eintragen
  CPhotoShopFilterEntry   Filter;

  Filter.m_FileName    = strFileName;
  Filter.m_strDescription = strPSPlugInName;
  Filter.m_strCategory    = strPSPlugInCategory;
  Filter.m_strEntryPoint  = strPSPlugInEntryPoint;
  pSettings->m_listPhotoShopFilter.push_back( Filter );

  return TRUE;

}
 


void CPainterApp::CreateFilterList()
{
  GR::String    strPath = pSettings->GetSettingString( "FilterPath", "filter" ),
                    strAppPath = CMisc::AppPath( "" ),
                    strMask;

  strPath = strAppPath + strPath;

  strMask = strPath + "\\*.*";

  std::list<GR::String>   listFiles;

  GR::IO::FileUtil::EnumFilesInDirectory( strMask.c_str(), listFiles );

  std::list<GR::String>::iterator   itF( listFiles.begin() );
  while ( itF != listFiles.end() )
  {
    HINSTANCE   hInstDummy = LoadLibrary( GR::Convert::ToUTF16( *itF ).c_str() );
    if ( hInstDummy )
    {
      // PiPL laden?
      //dh::Log( "check filter %s", wData.cFileName );

      EnumResourceNamesW( hInstDummy,
                          _T( "PIPL" ),
                          (ENUMRESNAMEPROC)EnumPIPLProc,
                          0 );

      // richtiger Check, nicht nur auf DLL
      FreeLibrary( hInstDummy );
    }
    else
    {
      //dh::Log( "-LoadLibrary failed %0x", GetLastError() );
    }
  }
}



BOOL CPainterApp::InitInstance()
{
  // Settings
  pSettings = new CSettings();

  pSettings->m_dwFunction      = CSettings::F_NOTHING;

	SetRegistryKey( _T( "Invisible" ) );

	LoadStdProfileSettings();

	if (!AfxOleInit())
	{
		AfxMessageBox( _T( "OLE hat nicht geklappt" ) );
		return FALSE;
	}

  // Font-Template
  pFontTemplate = new CMultiDocTemplate( IDR_PAINTERFONTVIEW_TMPL,
			                                   RUNTIME_CLASS( CPainterFontDoc ),
    		                                 RUNTIME_CLASS( CMDIChildWnd ),		
 	 	                                     RUNTIME_CLASS( CPainterFontView ) );
  AddDocTemplate( pFontTemplate );

  // Einzelbild-Template
	pDocTemplate = new CMultiDocTemplate( IDR_PAINTETYPE,
		                                    RUNTIME_CLASS( CPainterDoc ),
		                                    RUNTIME_CLASS( CChildFrame ),
		                                    RUNTIME_CLASS( CPainterView ) );
	AddDocTemplate( pDocTemplate );

  // Filter laden/prüfen
  CreateFilterList();

	// Haupt-MDI-Rahmenfenster erzeugen
	pMainFrame = new CMainFrame;
	if ( !pMainFrame->LoadFrame( IDR_MAINFRAME ) )
  {
		return FALSE;
  }
	m_pMainWnd = pMainFrame;


  for ( int i = 0; i < 256; i++ )
  {
    m_PainterPalette.SetColor( i, ucPalColors[i * 3] << 2, ucPalColors[i * 3 + 1] << 2, ucPalColors[i * 3 + 2] << 2 );
  }

  hCursorFreehand1        = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_FREEHAND1 ) );
  hCursorFreehand2        = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_FREEHAND2 ) );
  hCursorSelection        = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_SELECT ) );
  hCursorSelectionAdd     = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_SELECT_ADD ) );
  hCursorSelectionRemove  = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_SELECT_REMOVE ) );
  hCursorLine             = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_LINE ) );
  hCursorZoom             = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_ZOOM ) );
  hCursorRectangle        = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_RECT ) );
  hCursorBox              = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_BOX ) );
  hCursorCircle           = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_CIRCLE ) );
  hCursorFilledCircle     = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_FILLEDCIRCLE ) );
  hCursorFill             = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_FILL ) );
  hCursorText             = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_TEXT ) );
  hCursorDragHand         = LoadCursor( MAKEINTRESOURCE( IDC_CURSOR_DRAGHAND ) );

  pSettings->m_hbmBackground = ::LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDB_BITMAP_BACKGROUND ) );
  pSettings->m_brushBackground = CreatePatternBrush( pSettings->m_hbmBackground );

	// Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
	CCommandLineInfo cmdInfo;
	ParseCommandLine( cmdInfo );

	// Verteilung der in der Befehlszeile angegebenen Befehle
  if ( cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew )
  {
  	if (!ProcessShellCommand(cmdInfo))
	  	return FALSE;
  }
	// Das Hauptfenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
  //pMainFrame->ShowWindow( m_nCmdShow );
  pMainFrame->ShowWindow( SW_MAXIMIZE );
	pMainFrame->UpdateWindow();

  GetMainWnd()->SetIcon( LoadIcon( IDR_MAINFRAME ), TRUE );
  GetMainWnd()->SetIcon( LoadIcon( IDR_MAINFRAME ), FALSE );

  // need to restore something?
  pMainFrame->OnRestoreSafetySave();
	return TRUE;
}



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogdaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CEdit	m_EditAboutInfo;
	//}}AFX_DATA

	// Überladungen für virtuelle Funktionen, die vom Anwendungs-Assistenten erzeugt wurden
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//  afx_msg void OnEnChangeEditAboutInfo();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_EDIT_ABOUT_INFO, m_EditAboutInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
//  ON_EN_CHANGE(IDC_EDIT_ABOUT_INFO, OnEnChangeEditAboutInfo)
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CPainterApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}



CDocument* CPainterApp::CheckAndOpen( const GR::Char* File )
{

#define OPEN_AS_BTN         0
#define OPEN_AS_FONT        1

  DWORD       dwOpenFormat;

  if ( !GR::IO::FileUtil::FileExists( File ) )
  {
    AfxMessageBox( GR::Convert::ToUTF16( CMisc::printf( "File %s could not be opened.\n", File ) ).c_str() );
    return NULL;
  }

  GR::String      extension = GR::Strings::ToUpper( Path::Extension( File ) );
  GR::WString         fullPath = GR::Convert::ToUTF16( File );

  dwOpenFormat = OPEN_AS_BTN;
  if ( extension == "FNX" )
  {
    dwOpenFormat = OPEN_AS_FONT;
  }
  else if ( extension == "FNH" )
  {
    dwOpenFormat = OPEN_AS_FONT;
  }
  else if ( extension == "IGF" )
  {
    GR::IO::FileStream   fileTemp;

    if ( fileTemp.Open( File ) )
    {
      if ( fileTemp.ReadU8() == GR::Graphic::IGFType::FONT )
      {
        dwOpenFormat = OPEN_AS_FONT;
      }
      fileTemp.Close();
    }
  }
  if ( dwOpenFormat == OPEN_AS_BTN )
  {
    return pDocTemplate->OpenDocumentFile( fullPath.c_str() );
  }
  else if ( dwOpenFormat == OPEN_AS_FONT )
  {
    return pFontTemplate->OpenDocumentFile( fullPath.c_str() );
  }
  return NULL;
}



void CPainterApp::OnFileOpen()
{
  CPainterFileOpen*     pcfDlg;


  pcfDlg = OpenFileDialog( CSettings::FT_IMAGE | CSettings::FT_ANIMATION | CSettings::FT_FONT );

  POSITION                pos;

  CString                 cs;

  GR::WChar*              pBuffer;
  GR::WChar               szInitDir[MAX_PATH];

  wsprintf( szInitDir, GR::Convert::ToUTF16( pSettings->GetSettingString( "LoadFromDirectory" ) ).c_str() );

  pBuffer = (GR::WChar*)GlobalAlloc( GPTR, 4000 );
  pcfDlg->m_ofn.lpstrFile = pBuffer;
  pcfDlg->m_ofn.nMaxFile = 1995;
  pcfDlg->m_ofn.lpstrInitialDir = szInitDir;

  if ( pcfDlg->DoModal() == IDOK )
  {
    pos = pcfDlg->GetStartPosition();
    while ( pos != NULL )
    {
      cs = pcfDlg->GetNextPathName( pos );

      GR::String    fileName = GR::Convert::ToUTF8( (LPCTSTR)cs );

      // prüfen, ob ein Font dabei ist
      CheckAndOpen( fileName.c_str() );

      GR::String    lastPath = Path::AddSeparator( Path::RemoveFileSpec( fileName ) );

      pSettings->SetSetting( "LoadFromDirectory", lastPath );
    }
  }
  GlobalFree( pBuffer );
  delete pcfDlg;

}



int CPainterApp::ExitInstance() 
{

  DestroyCursor( hCursorFreehand1 );
  DestroyCursor( hCursorFreehand2 );
  DestroyCursor( hCursorSelection );
  DestroyCursor( hCursorSelectionAdd );
  DestroyCursor( hCursorSelectionRemove );
  DestroyCursor( hCursorLine );
  DestroyCursor( hCursorZoom );
  DestroyCursor( hCursorRectangle );
  DestroyCursor( hCursorBox );
  DestroyCursor( hCursorCircle );
  DestroyCursor( hCursorFilledCircle );
  DestroyCursor( hCursorFill );
  DestroyCursor( hCursorText );
  DestroyCursor( hCursorDragHand );

	return CWinApp::ExitInstance();

}



DocumentInfo* CPainterApp::GetActiveDocumentInfo() 
{
  CDocument         *pDoc;


  if ( pMainFrame == NULL )
  {
    return NULL;
  }
  if ( pMainFrame->MDIGetActive() == NULL )
  {
    return NULL;
  }
  pDoc = pMainFrame->MDIGetActive()->GetActiveDocument();
  if ( pDoc != NULL )
  {
    return GetDocumentInfoFromDocument( pDoc );
  }
  return NULL;
}



DocumentInfo* CPainterApp::GetDocumentInfoFromDocument( CDocument* pDoc )
{
  if ( pDoc->GetDocTemplate() == pDocTemplate )
  {
    return &( (CPainterDoc*)pDoc )->diInfo;
  }
  else if ( pDoc->GetDocTemplate() == pFontTemplate )
  {
    return &( (CPainterFontDoc*)pDoc )->diInfo;
  }
  return NULL;
}



ViewInfo *CPainterApp::GetActiveViewInfo()
{

  CScrollView         *pView;

  CDocument           *pDoc;


  if ( pMainFrame == NULL )
  {
    return NULL;
  }
  if ( pMainFrame->MDIGetActive() == NULL )
  {
    return NULL;
  }
  pView = (CScrollView*)( pMainFrame->MDIGetActive()->GetActiveView() );
  pDoc = pMainFrame->MDIGetActive()->GetActiveDocument();
  if ( pDoc != NULL )
  {
    if ( pDoc->GetDocTemplate() == pDocTemplate )
    {
      // ein PainterDoc
      return &( (CPainterView*)pView )->m_viewInfo;
    }
    else if ( pDoc->GetDocTemplate() == pFontTemplate )
    {
      // BAUSTELLE
      return &( (CPainterFontView*)pView )->m_viewInfo;
      //return &( (CPainterFontDoc*)pDoc )->diInfo;
    }
  }
  return NULL;

}



void CPainterApp::ChangeFunction( DWORD dwNewFunction ) 
{

  if ( dwNewFunction == pSettings->m_dwFunction )
  {
    return;
  }

  pSettings->m_dwFunction = dwNewFunction;

  pSettings->Notify( NF_FUNCTION_CHANGED );

}



void CPainterApp::OnPaintArrow() 
{

  ChangeFunction( CSettings::F_NOTHING );
	
}



void CPainterApp::OnPaintFreehand2() 
{

  ChangeFunction( CSettings::F_FREEHAND_2 );
  
	
}



void CPainterApp::OnPaintRectangle() 
{

  ChangeFunction( CSettings::F_RECTANGLE );
	
}



void CPainterApp::OnPaintBox() 
{

  ChangeFunction( CSettings::F_BOX );
	
}



void CPainterApp::OnPaintFilledCircle() 
{

  ChangeFunction( CSettings::F_FILLEDCIRCLE );
	
}



void CPainterApp::OnPaintCircle() 
{

  ChangeFunction( CSettings::F_CIRCLE );
	
}



void CPainterApp::OnPaintBrighten() 
{

  ChangeFunction( CSettings::F_BRIGHTEN );
	
}



void CPainterApp::OnPaintDarken() 
{

  ChangeFunction( CSettings::F_DARKEN );
	
}



void CPainterApp::OnPaintFill() 
{

  ChangeFunction( CSettings::F_FILL );
	
}



void CPainterApp::OnPaintFreehand1() 
{

  ChangeFunction( CSettings::F_FREEHAND_1 );
	
}



void CPainterApp::OnPaintLine() 
{

  ChangeFunction( CSettings::F_LINE );
	
}



void CPainterApp::OnPaintSelection() 
{

  ChangeFunction( CSettings::F_SELECTION );
	
}



void CPainterApp::OnPaintSelectionColor() 
{

  ChangeFunction( CSettings::F_SELECTION_COLOR );
	
}



void CPainterApp::OnPaintZoom() 
{

  ChangeFunction( CSettings::F_ZOOM );
	
}



void CPainterApp::OnPaintText() 
{

  ChangeFunction( CSettings::F_TEXT );
	
}



void CPainterApp::SetAppCursor( DocumentInfo *pDocInfo ) 
{

  /*
  if ( pDocInfo->bDragging )
  {
    SetCursor( hCursorDragHand );
    return;
  }
  */
  switch ( pSettings->m_dwFunction )
  {
    case CSettings::F_FREEHAND_1:
      SetCursor( hCursorFreehand1 );
      break;
    case CSettings::F_FREEHAND_2:
      SetCursor( hCursorFreehand2 );
      break;
    case CSettings::F_SELECTION:
      if ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
      {
        SetCursor( hCursorSelectionAdd );
      }
      else if ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
      {
        SetCursor( hCursorSelectionRemove );
      }
      else
      {
        SetCursor( hCursorSelection );
      }
      break;
    case CSettings::F_LINE:
      SetCursor( hCursorLine );
      break;
    case CSettings::F_ZOOM:
      SetCursor( hCursorZoom );
      break;
    case CSettings::F_RECTANGLE:
      SetCursor( hCursorRectangle );
      break;
    case CSettings::F_BOX:
      SetCursor( hCursorBox );
      break;
    case CSettings::F_FILLEDCIRCLE:
      SetCursor( hCursorFilledCircle );
      break;
    case CSettings::F_CIRCLE:
      SetCursor( hCursorCircle );
      break;
    case CSettings::F_FILL:
      SetCursor( hCursorFill );
      break;
    case CSettings::F_TEXT:
      SetCursor( hCursorText );
      break;
    case CSettings::F_NOTHING:
      SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
      break;
  }

}



void CPainterApp::OnFileNew() 
{

  CNewDocumentDialog    *pNewDocument;

  DocumentInfo         *pDocInfo;

  CPainterDoc           *pDoc;

  CPainterFontDoc       *pFontDoc;

  DWORD                 i;

  POSITION              pos;

  GR::Graphic::Image             *pDummyImage;

  CFrameWnd             *pFrameWnd;


  pNewDocument = new CNewDocumentDialog;
  if ( pNewDocument->DoModal() == IDCANCEL )
  {
    delete pNewDocument;
    return;
  }

  pDocInfo = NULL;

  if ( pNewDocument->m_Type == DT_IMAGE )
  {
    // ein ANX!
    pDoc = (CPainterDoc*)theApp.pDocTemplate->CreateNewDocument();

    pDocInfo = &pDoc->diInfo;

    pFrameWnd = theApp.pDocTemplate->CreateNewFrame( pDoc, NULL );
    
    pos = pDoc->GetFirstViewPosition();

    pDoc->diInfo.m_DocType = DT_IMAGE;

    pDoc->diInfo.SetSize( pNewDocument->m_Width, pNewDocument->m_Height );
    pDoc->diInfo.m_BitDepth = pNewDocument->m_Depth;

    for ( int i = 0; i < pNewDocument->m_Frames; i++ )
    {
      pDoc->diInfo.AddFrame();
      pDoc->diInfo.AddLayer( new GR::Graphic::Image( (WORD)pDoc->diInfo.Width(), 
                                   (WORD)pDoc->diInfo.Height(), 
                                   pDoc->diInfo.m_BitDepth, 0, 0 ),
                             i );
      *pDoc->diInfo.GetPalette( i ) = m_PainterPalette;
    }
    pDoc->diInfo.m_AnimationRunning = FALSE;

    theApp.pDocTemplate->InitialUpdateFrame( pFrameWnd, pDoc, TRUE );

    pDoc->SetTitle( _T( "New Document" ) );
  }
  else if ( pNewDocument->m_Type == DT_FONT )
  {
    // ein Font!
    pFontDoc = (CPainterFontDoc*)theApp.pFontTemplate->CreateNewDocument();

    pDocInfo = &pFontDoc->diInfo;

    pFontDoc->diInfo.SetSize( 600, 600 );
    pFontDoc->diInfo.m_DocType    = DT_FONT;
    pFontDoc->diInfo.m_BitDepth   = pNewDocument->m_Depth;

    pFrameWnd = theApp.pFontTemplate->CreateNewFrame( pFontDoc, NULL );
    theApp.pFontTemplate->InitialUpdateFrame( pFrameWnd, pFontDoc, TRUE );
    pos = pFontDoc->GetFirstViewPosition();

    for ( i = 0; i < 256; i++ )
    {
      pDummyImage = new GR::Graphic::Image( (WORD)pNewDocument->m_Width, (WORD)pNewDocument->m_Height, pFontDoc->diInfo.m_BitDepth, 0, 0 );

      pFontDoc->diInfo.AddLayer( pDummyImage, pFontDoc->diInfo.AddFrame() );
    }

    pFontDoc->SetTitle( _T( "New Document" ) );
  }

  delete pNewDocument;
}



void CPainterApp::ResizeWindow( CDocument *pDoc, DocumentInfo *pDocInfo, HWND hWnd, WORD ucOldZoom, BOOL bUpdateOffset )
{
}



void CPainterApp::ShowStatusMessage()
{
  DocumentInfo*   pDocInfo;

  ViewInfo*      pViewInfo;
  
  
  pDocInfo = GetActiveDocumentInfo();
  if ( pDocInfo == NULL )
  {
    // BAUSTELLE
    // Anzeigen mit Leerzeichen!!
    return;
  }
  pViewInfo = GetActiveViewInfo();
  if ( pViewInfo == NULL )
  {
    // BAUSTELLE
    // Anzeigen mit Leerzeichen!!
    return;
  }

  pMainFrame->m_wndStatusBar.SetFont( m_pMainWnd->GetFont() );

  // Info
  pMainFrame->m_wndStatusBar.SetPaneText( 2, GR::Convert::ToUTF16( CMisc::printf( "%dx%dx%d Zoom %d%% Frame %d/%d ", 
                    pDocInfo->Width(), pDocInfo->Height(), pDocInfo->GetTrueBitDepth(),
                    pViewInfo->m_ZoomFaktor,
                    pDocInfo->CurrentFrame() + 1,
                    pDocInfo->m_LayeredFrames.size() ) ).c_str() );

  // Koordinaten
  pMainFrame->m_wndStatusBar.SetPaneText( 3, GR::Convert::ToUTF16( CMisc::printf( "%d,%d", pDocInfo->m_ActX, pDocInfo->m_ActY ) ).c_str() );

  // Auswahl
  if ( pViewInfo->m_FloatingSelection )
  {
    // da fliegt grade eine Selection rum!
    pMainFrame->m_wndStatusBar.SetPaneText( 4, GR::Convert::ToUTF16( CMisc::printf( "%d,%d - %d,%d  (%dx%d)",
                pDocInfo->m_ActX - pDocInfo->m_SelectionWidth / 2,
                pDocInfo->m_ActY - pDocInfo->m_SelectionHeight / 2,
                pDocInfo->m_ActX - pDocInfo->m_SelectionWidth / 2 + pDocInfo->m_SelectionWidth - 1,
                pDocInfo->m_ActY - pDocInfo->m_SelectionHeight / 2 + pDocInfo->m_SelectionHeight - 1,
                pDocInfo->m_SelectionWidth,
                pDocInfo->m_SelectionHeight ) ).c_str() );
  }
  else if ( ( pViewInfo->m_Selecting )
  &&        ( ( pSettings->m_dwFunction == CSettings::F_SELECTION )
  ||          ( pSettings->m_dwFunction == CSettings::F_BOX )
  ||          ( pSettings->m_dwFunction == CSettings::F_RECTANGLE ) ) )
  {
    // Auswahl, die gerade getätigt wird
    pMainFrame->m_wndStatusBar.SetPaneText( 4, GR::Convert::ToUTF16( CMisc::printf( "%d,%d - %d,%d  (%dx%d)",
                pDocInfo->m_StartX,
                pDocInfo->m_StartY,
                pDocInfo->m_StartX + abs( pDocInfo->m_StartX - pDocInfo->m_ActX ),
                pDocInfo->m_StartY + abs( pDocInfo->m_StartY - pDocInfo->m_ActY ),
                abs( pDocInfo->m_StartX - pDocInfo->m_ActX ) + 1,
                abs( pDocInfo->m_StartY - pDocInfo->m_ActY ) + 1 ) ).c_str() );
  }
  else if ( pDocInfo->HasSelection() )
  {
    // fertige Auswahl
    pMainFrame->m_wndStatusBar.SetPaneText( 4, GR::Convert::ToUTF16( CMisc::printf( "%d,%d - %d,%d  (%dx%d)",
                pDocInfo->m_Selection.left,
                pDocInfo->m_Selection.top,
                pDocInfo->m_Selection.right,
                pDocInfo->m_Selection.bottom,
                pDocInfo->m_Selection.right - pDocInfo->m_Selection.left + 1,
                pDocInfo->m_Selection.bottom - pDocInfo->m_Selection.top + 1 ) ).c_str() );
  }
  else
  {
    pMainFrame->m_wndStatusBar.SetPaneText( 4, _T( "" ) );
  }

  // BAUSTELLE
  //pMainFrame->m_wndStatusBar.SetPaneText( 5, LPCTSTR( pDoc->GetTitle() ) );
}



void CPainterApp::OnBatchConversion() 
{
  CDialogBatchConversion    dlgBatch;


  dlgBatch.DoModal();
}



CDocument* CPainterApp::OpenDocumentFile( LPCTSTR lpszFileName ) 
{
  return CheckAndOpen( GR::Convert::ToUTF8( lpszFileName ).c_str() );
}



void CPainterApp::OnMenuAniCreator() 
{
  CDialogAnimationErstellen       dlgAC;

  dlgAC.DoModal();
}



void CPainterApp::OnMenuColorPicker() 
{
  CColorPicker      dlgCP;


  dlgCP.m_WorkColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR );
  if ( dlgCP.DoModal() == IDOK )
  {
    pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR, dlgCP.m_WorkColor );
  }
}



void CPainterApp::OnSpezialEffekte() 
{
  CDlgEffekte     *pDlgEffekte;

  ViewInfo       *pViewInfo;


  pViewInfo = GetActiveViewInfo();

  if ( !pViewInfo )
  {
    return;
  }
  pDlgEffekte = new CDlgEffekte();
  pDlgEffekte->pDocInfo = pViewInfo->m_pDocInfo;
  pDlgEffekte->pImageSource = pViewInfo->GetWorkImage();
  pDlgEffekte->pPalette = pViewInfo->GetActivePalette();
  if ( pViewInfo->m_pDocInfo->HasSelection() )
  {
    pDlgEffekte->m_pImageMask = pViewInfo->m_pDocInfo->m_pImageSelection->GetTileImage( 
          (WORD)pViewInfo->m_pDocInfo->m_Selection.left, 
          (WORD)pViewInfo->m_pDocInfo->m_Selection.top, 
          (WORD)( pViewInfo->m_pDocInfo->m_Selection.right - pViewInfo->m_pDocInfo->m_Selection.left + 1 ),
          (WORD)( pViewInfo->m_pDocInfo->m_Selection.bottom - pViewInfo->m_pDocInfo->m_Selection.top + 1 ) );
  }

  if ( pDlgEffekte->DoModal() == IDOK )
  {
    int   iXTarget = 0,
          iYTarget = 0;

    if ( pViewInfo->m_pDocInfo->HasSelection() )
    {
      iXTarget = pViewInfo->m_pDocInfo->m_Selection.left;
      iYTarget = pViewInfo->m_pDocInfo->m_Selection.top;
    }
    if ( pViewInfo->m_pDocInfo->m_DocType == DT_FONT )
    {
      BOOL      startNewGroup = TRUE;
      for ( size_t frame = 0; frame < pViewInfo->m_pDocInfo->m_LayeredFrames.size(); ++frame )
      {
        if ( pViewInfo->m_pDocInfo->GetImage( frame, 0 ) == NULL )
        {
          // skip empty chars
          continue;
        }
        pViewInfo->m_pDocInfo->AddUndoRect( iXTarget,
                                            iYTarget,
                                            pViewInfo->m_pDocInfo->m_Selection.right - 1,
                                            pViewInfo->m_pDocInfo->m_Selection.bottom - 1,
                                            frame,
                                            -1,
                                            false,
                                            startNewGroup );
        startNewGroup = FALSE;

        GR::Graphic::Image      imgResult( *pViewInfo->m_pDocInfo->GetImage( frame, 0 ) );


        pDlgEffekte->m_pCurrentEffekt->m_pCDSource  = new CMaskedContextDescriptor( pViewInfo->m_pDocInfo->GetImage( frame, 0 ), pViewInfo->m_pDocInfo->GetPalette( frame ) );
        pDlgEffekte->m_pCurrentEffekt->m_pCDTarget  = new CMaskedContextDescriptor( &imgResult,  pDlgEffekte->pPalette );
        if ( pDlgEffekte->m_pImageMask )
        {
          pDlgEffekte->m_pCurrentEffekt->m_pCDSource->m_pMaskData = pDlgEffekte->m_pImageMask->GetData();
        }
        pDlgEffekte->m_pCurrentEffekt->Init();
        pDlgEffekte->m_pCurrentEffekt->DoEffekt();
        pDlgEffekte->m_pCurrentEffekt->CleanUp();

        if ( pDlgEffekte->m_pImageMask )
        {
          GR::Graphic::ContextDescriptor    cdSource( &imgResult, 
                                                      pViewInfo->m_pDocInfo->GetPalette( frame ) ),
                                            cdTarget( pViewInfo->m_pDocInfo->GetImage( frame, 0 ),
                                                      pViewInfo->m_pDocInfo->GetPalette( frame ) );

          for ( int i = 0; i < cdSource.Width(); i++ )
          {
            for ( int j = 0; j < cdSource.Height(); j++ )
            {
              if ( ( (BYTE*)pDlgEffekte->m_pImageMask->GetData() )[i + j * cdSource.Width()] )
              {
                cdTarget.PutDirectPixel( pViewInfo->m_pDocInfo->m_Selection.left + i, 
                                         pViewInfo->m_pDocInfo->m_Selection.top + j, 
                                         cdSource.GetDirectPixel( i, j ) );
              }
            }
          }
        }
        else
        {
          GR::Graphic::ContextDescriptor    cdSource( imgResult );
          GR::Graphic::ContextDescriptor    cdTarget( pViewInfo->m_pDocInfo->GetImage( frame, 0 ) );

          cdSource.CopyArea( 0, 0, cdSource.Width(), cdSource.Height(), iXTarget, iYTarget, &cdTarget );
        }
        delete pDlgEffekte->m_pCurrentEffekt->m_pCDSource;
        delete pDlgEffekte->m_pCurrentEffekt->m_pCDTarget;
      }
    }
    else
    {
      if ( pDlgEffekte->AsNewLayer )
      {
        pViewInfo->m_pDocInfo->AddUndoLayerChange( 
                pViewInfo->m_pDocInfo->m_LayeredFrames[pViewInfo->m_pDocInfo->CurrentFrame()].LayerCount(),
                0,
                CUndoLayerChange::ULC_ADD_LAYER );

        CLayer* pNewLayer = pViewInfo->m_pDocInfo->AddLayer();

        if ( pViewInfo->m_Type == ViewInfo::VI_ALPHA )
        {
          pNewLayer->SetLayerImage( new GR::Graphic::Image( (WORD)pViewInfo->m_pDocInfo->Width(), 
                                                    (WORD)pViewInfo->m_pDocInfo->Height(), 
                                                    pViewInfo->m_pDocInfo->m_BitDepth ) );
          pNewLayer->SetLayerMask( new GR::Graphic::Image( (WORD)pViewInfo->m_pDocInfo->Width(), 
                                                   (WORD)pViewInfo->m_pDocInfo->Height(), 
                                                   8 ) );
          pNewLayer->m_MaskEnabled   = true;

          GR::Graphic::ContextDescriptor    cdSource( pDlgEffekte->pImageSource );
          GR::Graphic::ContextDescriptor    cdTarget( pNewLayer->GetMask() );

          cdSource.CopyArea( 0, 0, cdSource.Width(), cdSource.Height(), iXTarget, iYTarget, &cdTarget );
        }
        else
        {
          GR::Graphic::ContextDescriptor    cdSource( pDlgEffekte->pImageSource );
          GR::Graphic::ContextDescriptor    cdTarget( pNewLayer->GetImage() );

          cdSource.CopyArea( 0, 0, cdSource.Width(), cdSource.Height(), iXTarget, iYTarget, &cdTarget );
        }

        pSettings->Notify( NF_LAYER_CHANGED );
      }
      else
      {
        pViewInfo->m_pDocInfo->AddUndoRect( iXTarget,
                                            iYTarget,
                                            pViewInfo->m_pDocInfo->m_Selection.right - 1,
                                            pViewInfo->m_pDocInfo->m_Selection.bottom - 1,
                                            -1,
                                            -1,
                                            false );

        if ( pDlgEffekte->m_pImageMask )
        {
          GR::Graphic::ContextDescriptor    cdSource( pDlgEffekte->pImageSource, pViewInfo->GetActivePalette() ),
                                cdTarget( pViewInfo->GetActiveImage(), pViewInfo->GetActivePalette() );

          for ( int i = 0; i < cdSource.Width(); i++ )
          {
            for ( int j = 0; j < cdSource.Height(); j++ )
            {
              if ( ( (BYTE*)pDlgEffekte->m_pImageMask->GetData() )[i + j * cdSource.Width()] )
              {
                cdTarget.PutDirectPixel( pViewInfo->m_pDocInfo->m_Selection.left + i, 
                                         pViewInfo->m_pDocInfo->m_Selection.top + j, 
                                         cdSource.GetDirectPixel( i, j ) );
              }
            }
          }
        }
        else
        {
          GR::Graphic::ContextDescriptor    cdSource( pDlgEffekte->pImageSource );
          GR::Graphic::ContextDescriptor    cdTarget( pViewInfo->GetActiveImage() );

          cdSource.CopyArea( 0, 0, cdSource.Width(), cdSource.Height(), iXTarget, iYTarget, &cdTarget );
        }
      }
    }
    pViewInfo->m_pDocInfo->RedrawAllViews();
    pViewInfo->m_pDocInfo->SetModify( TRUE );
  }
  SafeDelete( pDlgEffekte->m_pImageMask );
  delete pDlgEffekte->pImageSource;

  delete pDlgEffekte;
}



BOOL CPainterApp::OnIdle( LONG lCount )
{
  static int iLastTime = GetTickCount();

  if ( GetTickCount() - iLastTime >= 1000 )
  {
    ViewInfo*    pViewInfo = GetActiveViewInfo();

    if ( pViewInfo )
    {
      DWORD   dwRedrawFlags = ViewInfo::REDRAW_NONE;

      if ( !pViewInfo->m_rectRedraw.Empty() )
      {
        dwRedrawFlags |= ViewInfo::REDRAW_RECT;
      }
      if ( pViewInfo->m_pDocInfo->HasSelection() )
      {
        dwRedrawFlags |= ViewInfo::REDRAW_SELECTION;
      }
      if ( dwRedrawFlags != ViewInfo::REDRAW_NONE )
      {
        pViewInfo->m_pDocInfo->m_pDoc->UpdateAllViews( NULL, dwRedrawFlags );
      }
    }
    iLastTime = GetTickCount();
  }
	
	return CWinApp::OnIdle(lCount);
}



BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

  GR::u32   dwMajor,
            dwMinor,
            dwBuildNumber,
            dwSubBuild;

  GR::String   strContent;


  if ( Win::Util::GetVersionInfo( AfxGetInstanceHandle(), 
                                  VS_VERSION_INFO, 
                                  dwMajor, 
                                  dwMinor, 
                                  dwBuildNumber,
                                  dwSubBuild ) )
  {
    strContent += "MFC-Painter V";
    strContent += CMisc::printf( "%lu.%lu.%lu.%lu", dwMajor, dwMinor, dwBuildNumber, dwSubBuild );
    strContent += "\r\n\r\n";
  }

  strContent += "Using FreeImage Version ";
  strContent += FreeImage_GetVersion();
  strContent += "\r\n\r\n";
  strContent += FreeImage_GetCopyrightMessage();
  strContent += "\r\n";

  strContent += "\r\n";
  strContent += "Big Thanks go out to www.codeguru.com\r\n";
  strContent += "and to P. Jerz for a lot of help struggling with MFC";

  m_EditAboutInfo.SetWindowText( GR::Convert::ToUTF16( strContent ).c_str() );
	
	return TRUE;
}



UINT_PTR CALLBACK SaveHookProc( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{

  if ( uiMsg == WM_INITDIALOG )
  {
    HWND    hwnd = GetParent( hdlg );

    RECT    rc;

    GetWindowRect( hwnd, &rc );

    SetWindowPos( hwnd, NULL, 
                  ( GetSystemMetrics( SM_CXSCREEN ) - rc.right + rc.left ) / 2,
                  ( GetSystemMetrics( SM_CYSCREEN ) - rc.bottom + rc.top ) / 2,
                  0, 0, SWP_NOZORDER | SWP_NOSIZE );
    return 1;
  }

  return 0;

}



void CPainterApp::OnFileSaveAs( DocumentInfo& diInfo )
{
  GR::WString       initialDir = GR::Convert::ToUTF16( pSettings->GetSettingString( "SaveToDirectory" ) );

  CFileDialogEx     dlgFile( FALSE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, AfxGetMainWnd() );

  GR::WChar         fileNameBuffer[3000];

  dlgFile.m_ofn.hwndOwner           = AfxGetMainWnd()->GetSafeHwnd();
  dlgFile.m_ofn.hInstance           = NULL;
  dlgFile.m_ofn.lpstrFile           = fileNameBuffer;
  dlgFile.m_ofn.nMaxFile            = 3000;
  dlgFile.m_ofn.lpstrCustomFilter   = NULL;
  dlgFile.m_ofn.nMaxCustFilter      = 0;
  dlgFile.m_ofn.lpstrInitialDir     = initialDir.c_str();

  switch( diInfo.m_SaveType )
  {
    case SAVETYPE_GRI:
      dlgFile.m_ofn.nFilterIndex = 1;
      break;
    case SAVETYPE_IGF:
      dlgFile.m_ofn.nFilterIndex = 2;
      break;
    case SAVETYPE_PNT:
      dlgFile.m_ofn.nFilterIndex = 3;
      break;
    case SAVETYPE_BTN:
      dlgFile.m_ofn.nFilterIndex = 4;
      break;
    case SAVETYPE_ANX:
      dlgFile.m_ofn.nFilterIndex = 5;
      break;
    case SAVETYPE_ANH:
      dlgFile.m_ofn.nFilterIndex = 6;
      break;
    case SAVETYPE_BTH:
      dlgFile.m_ofn.nFilterIndex = 7;
      break;
    case SAVETYPE_BMP:
      dlgFile.m_ofn.nFilterIndex = 8;
      break;
    case SAVETYPE_PCX:
      dlgFile.m_ofn.nFilterIndex = 9;
      break;
    case SAVETYPE_GIF:
      dlgFile.m_ofn.nFilterIndex = 10;
      break;
    case SAVETYPE_JPEG:
      dlgFile.m_ofn.nFilterIndex = 11;
      break;
    case SAVETYPE_PNG:
      dlgFile.m_ofn.nFilterIndex = 12;
      break;
    case SAVETYPE_WEBP:
      dlgFile.m_ofn.nFilterIndex = 13;
      break;
    case SAVETYPE_TGA:
      dlgFile.m_ofn.nFilterIndex = 14;
      break;
    case SAVETYPE_ICON:
      dlgFile.m_ofn.nFilterIndex = 15;
      break;
    case SAVETYPE_CURSOR:
      dlgFile.m_ofn.nFilterIndex = 16;
      break;
    case SAVETYPE_IFF:
      dlgFile.m_ofn.nFilterIndex = 17;
      break;
    default:
    case SAVETYPE_UNKNOWN:
      dlgFile.m_ofn.nFilterIndex = pSettings->GetSetting( "SaveFilterIndex" );
      break;
  }

  dlgFile.m_ofn.lpstrTitle          = NULL;
  fileNameBuffer[0] = 0;

  if ( diInfo.m_DocType == DT_IMAGE )
  {
    dlgFile.m_ofn.lpstrFilter = _T( "GRI Files (*.GRI)\0*.gri\0" )
                                _T( "IGF Files (*.IGF)\0*.igf\0" )
                                _T( "PNT Files (*.PNT)\0*.pnt\0" )
                                _T( "BTN Files (*.BTN)\0*.btn\0" )
                                _T( "ANX Files (*.ANX)\0*.anx\0" )
                                _T( "ANH Files (*.ANH)\0*.anh\0" )
                                _T( "BTH Files (*.BTH)\0*.bth\0" )
                                _T( "Bitmap Files (*.BMP)\0*.bmp\0" )
                                _T( "PCX Files (*.PCX)\0*.pcx\0" )
                                _T( "GIF Files (*.GIF)\0*.gif\0" )
                                _T( "JPG Files (*.JPG)\0*.jpg\0" )
                                _T( "PNG Files (*.PNG)\0*.png\0" )
                                _T( "WEBP Files (*.WEBP)\0*.webp\0" )
                                _T( "TGA Files (*.TGA)\0*.tga\0" )
                                _T( "Icon Files (*.ICO)\0*.ico\0" )
                                _T( "Cursor Files (*.CUR)\0*.cur\0" )
                                _T( "IFF (*.IFF)\0*.iff\0" )
                                _T( "\0\0\0" );
    if ( !dlgFile.DoModal() )
    {
      return;
    }

    pSettings->SetSetting( "SaveFilterIndex", dlgFile.m_ofn.nFilterIndex );

    GR::String    targetName = GR::Convert::ToUTF8( fileNameBuffer );

    if ( dlgFile.m_ofn.nFilterIndex == 1 )
    {
      diInfo.m_SaveType = SAVETYPE_GRI;
      targetName = Path::RenameExtension( targetName, "gri" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 2 )
    {
      diInfo.m_SaveType = SAVETYPE_IGF;
      targetName = Path::RenameExtension( targetName, "igf" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 3 )
    {
      diInfo.m_SaveType = SAVETYPE_PNT;
      targetName = Path::RenameExtension( targetName, "pnt" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 4 )
    {
      diInfo.m_SaveType = SAVETYPE_BTN;
      targetName = Path::RenameExtension( targetName, "btn" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 5 )
    {
      diInfo.m_SaveType = SAVETYPE_BTH;
      targetName = Path::RenameExtension( targetName, "bth" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 6 )
    {
      diInfo.m_SaveType = SAVETYPE_ANX;
      targetName = Path::RenameExtension( targetName, "anx" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 7 )
    {
      diInfo.m_SaveType = SAVETYPE_ANH;
      targetName = Path::RenameExtension( targetName, "anh" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 8 )
    {
      diInfo.m_SaveType = SAVETYPE_BMP;
      targetName = Path::RenameExtension( targetName, "bmp" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 9 )
    {
      diInfo.m_SaveType = SAVETYPE_PCX;
      targetName = Path::RenameExtension( targetName, "pcx" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 10 )
    {
      diInfo.m_SaveType = SAVETYPE_GIF;
      targetName = Path::RenameExtension( targetName, "gif" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 11 )
    {
      diInfo.m_SaveType = SAVETYPE_JPEG;
      targetName = Path::RenameExtension( targetName, "jpg" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 12 )
    {
      diInfo.m_SaveType = SAVETYPE_PNG;
      targetName = Path::RenameExtension( targetName, "png" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 13 )
    {
      diInfo.m_SaveType = SAVETYPE_WEBP;
      targetName = Path::RenameExtension( targetName, "webp" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 14 )
    {
      diInfo.m_SaveType = SAVETYPE_TGA;
      targetName = Path::RenameExtension( targetName, "tga" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 15 )
    {
      diInfo.m_SaveType = SAVETYPE_ICON;
      targetName = Path::RenameExtension( targetName, "ico" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 16 )
    {
      diInfo.m_SaveType = SAVETYPE_CURSOR;
      targetName = Path::RenameExtension( targetName, "cur" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 17 )
    {
      diInfo.m_SaveType = SAVETYPE_IFF;
      targetName = Path::RenameExtension( targetName, "iff" );
    }

    pSettings->SetSetting( "SaveFilterIndex", dlgFile.m_ofn.nFilterIndex );

    if ( diInfo.m_pDoc->OnSaveDocument( GR::Convert::ToUTF16( targetName ).c_str() ) )
    {
      diInfo.m_pDoc->SetTitle( GR::Convert::ToUTF16( targetName ).c_str() );
      diInfo.m_FileName = targetName;

      // SaveTo-Verzeichnis merken
      GR::String    initDir = Path::AddSeparator( Path::RemoveFileSpec( targetName ) );

      pSettings->SetSetting( "SaveToDirectory", initDir );
    }
  }
  else if ( diInfo.m_DocType == DT_FONT )
  {
    dlgFile.m_ofn.lpstrFilter = _T( "Font File (*.igf)\0*.igf\0" )
                                _T( "Font File (*.FNL)\0*.fnl\0" )
                                _T( "Font File (*.FNX)\0*.fnx\0" )
                                _T( "Font File (*.FNH)\0*.fnh\0\0\0\0" );

    if ( !dlgFile.DoModal() )
    {
      return;
    }

    GR::String    targetName = GR::Convert::ToUTF8( fileNameBuffer );

    if ( dlgFile.m_ofn.nFilterIndex == 1 )
    {
      diInfo.m_SaveType = SAVETYPE_IGF;
      targetName = Path::RenameExtension( targetName, "igf" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 2 )
    {
      diInfo.m_SaveType = SAVETYPE_FNXL;
      targetName = Path::RenameExtension( targetName, "fnl" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 3 )
    {
      diInfo.m_SaveType = SAVETYPE_FNX;
      targetName = Path::RenameExtension( targetName, "fnx" );
    }
    else if ( dlgFile.m_ofn.nFilterIndex == 4 )
    {
      diInfo.m_SaveType = SAVETYPE_FNH;
      targetName = Path::RenameExtension( targetName, "fnh" );
    }

    pSettings->SetSetting( "SaveFilterIndex", dlgFile.m_ofn.nFilterIndex );

    if ( diInfo.m_pDoc->OnSaveDocument( GR::Convert::ToUTF16( targetName ).c_str() ) )
    {
      diInfo.m_pDoc->SetTitle( (LPCTSTR)dlgFile.GetFileName() );
      diInfo.m_FileName = targetName;
    }
  }
}

