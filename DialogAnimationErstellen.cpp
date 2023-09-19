#include "stdafx.h"
#include "DialogAnimationErstellen.h"
#include "PainterGFXTools.h"

#include <Grafik/Image.h>
#include <Grafik/ImageFormate/ImageFormatManager.h>

#include <IO/FileStream.h>
#include <Misc/Misc.h>

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDialogAnimationErstellen::CDialogAnimationErstellen(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAnimationErstellen::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAnimationErstellen)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}



void CDialogAnimationErstellen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAnimationErstellen)
	DDX_Control(pDX, IDC_COMBO_CA_TYPE, m_ComboType);
	DDX_Control(pDX, IDC_COMBO_CA_DEPTH, m_ComboDepth);
	DDX_Control(pDX, IDC_EDIT_CA_TARGET, m_EditTarget);
	DDX_Control(pDX, IDC_EDIT_CA_PALETTE, m_EditPalette);
	DDX_Control(pDX, IDC_LIST_CA_LOG, m_ListLog);
	DDX_Control(pDX, IDC_LIST_CA_FILES, m_ListFiles);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CDialogAnimationErstellen, CDialog)
	//{{AFX_MSG_MAP(CDialogAnimationErstellen)
	ON_BN_CLICKED(IDC_BUTTON_CA_ADDFILE, OnButtonCAAddFile)
	ON_BN_CLICKED(IDC_BUTTON_CA_BROWSE, OnButtonCABrowse)
	ON_BN_CLICKED(IDC_BUTTON_START_AC, OnButtonStartAC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CDialogAnimationErstellen::OnButtonCAAddFile() 
{
  POSITION                pos;

  CString                 cs;

  GR::tChar*              pBuffer;
  GR::tChar               szTemp[MAX_PATH];


  CFileDialog fileDlg( TRUE, NULL, NULL,
                       OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
                       _T( "Supported Formats|*.IGF;*.BTN;*.ANX;*.ANH;*.GIF;*.PCX;*.TGA;*.BMP;*.BTH;*.IFF|IGF Files (*.IGF)|*.IGF|BTH Files (*.BTH)|*.BTH|BTN Files (*.BTN)|*.BTN|BTX Files (*.BTX)|*.BTX|ANX Files (*.ANX)|*.ANX|PCX Files (*.PCX)|*.PCX|GIF Files (*.GIF)|*.GIF|BMP Files (*.BMP)|*.BMP|TGA Files (*.TGA)|*.TGA|IFF Files (*.IFF)|All Files (*.*)|*.*||\0\0" ),
                       this );

  pBuffer = (GR::tChar*)GlobalAlloc( GPTR, 2000 );
  fileDlg.m_ofn.lpstrFile = pBuffer;
  fileDlg.m_ofn.nMaxFile = 1995;

  if ( fileDlg.DoModal() == IDOK )
  {
    pos = fileDlg.GetStartPosition();
    do
    {
      cs = fileDlg.GetNextPathName( pos );
      wsprintf( szTemp, cs );
      m_ListFiles.AddString( szTemp );
    }
    while ( pos != NULL );
  }
  GlobalFree( pBuffer );
}



void CDialogAnimationErstellen::OnButtonCABrowse() 
{
  GR::tChar*              pBuffer;


  CFileDialog     fileDlg( TRUE, NULL, NULL, 
                           OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
                           _T( "Supported Formats|*.IGF|All Files (*.*)|*.*||\0\0" ),
                           this );

  pBuffer = (GR::tChar*)GlobalAlloc( GPTR, 4000 );
  fileDlg.m_ofn.lpstrFile = pBuffer;
  fileDlg.m_ofn.nMaxFile = 1995;

  if ( fileDlg.DoModal() == IDOK )
  {
    m_EditTarget.SetWindowText( fileDlg.m_ofn.lpstrFile );
  }
  GlobalFree( pBuffer );
}



void CDialogAnimationErstellen::OnButtonStartAC() 
{
  DWORD       dwConverted,
              dwCount,
              dwFaktor;

  GR::WChar   szSource[MAX_PATH];
  GR::WChar   szTarget[MAX_PATH];

  GR::Graphic::Image   *pDummyImage;

  BOOL        bFileCreated;

  GR::IO::FileStream*      pFile;


  pFile         = NULL;
  bFileCreated  = FALSE;
  dwFaktor      = 1;
  dwConverted   = 0;
  dwCount       = m_ListFiles.GetCount();
  m_EditTarget.GetWindowTextW( szTarget, MAX_PATH );

  for ( int i = 0; i < (int)dwCount; i++ )
  {
    m_ListFiles.GetText( i, szSource );

    // jetzt umwandeln
    m_ListLog.AddString( GR::Convert::ToUTF16( CMisc::printf( "Lade %s.", szSource ) ).c_str() );
    pDummyImage = NULL;

    GR::Graphic::ImageData*     pData = ImageFormatManager::Instance().LoadData( GR::Convert::ToUTF8( szSource ).c_str() );

    if ( pData )
    {
      pDummyImage = new GR::Graphic::Image( *pData );
      delete pData;
      if ( pDummyImage )
      {
        if ( !bFileCreated )
        {
          // Kopf der Animation erzeugen
          pFile = new GR::IO::FileStream( GR::Convert::ToUTF8( szTarget ), IIOStream::OT_WRITE_ONLY );

          pFile->WriteU8( GR::Graphic::IGFType::ANIMATION );
          pFile->WriteU8( pDummyImage->GetDepth() );

          // zunächst nur ein Frame eintragen, zum Abschluß wird die Anzahl richtig eingetragen
          pFile->WriteU32( 1 );

          dwFaktor = ( pDummyImage->GetDepth() + 1 ) / 8;
          
          bFileCreated = TRUE;
        }
        // Bild anhängen
        pFile->WriteU16( pDummyImage->GetWidth() );
        pFile->WriteU16( pDummyImage->GetHeight() );
        pFile->WriteU32( pDummyImage->GetTransparentColor() );
        pFile->WriteBlock( (BYTE*)pDummyImage->GetData(), pDummyImage->GetWidth() * pDummyImage->GetHeight() * dwFaktor );

        dwConverted++;
        m_ListLog.AddString( GR::Convert::ToUTF16( CMisc::printf( "Adde %s.", szSource ) ).c_str() );
        m_ListLog.UpdateWindow();

        delete pDummyImage;
        pDummyImage = NULL;
      }
    }
    else
    {
      m_ListLog.AddString( GR::Convert::ToUTF16( "-fehlgeschlagen" ).c_str() );
      m_ListLog.UpdateWindow();
    }
  }
  if ( pFile != NULL )
  {
    pFile->SetPosition( 2, IIOStream::PT_SET );
    pFile->WriteU32( dwConverted );
    pFile->Close();
    delete pFile;
    pFile = NULL;
  }
  m_ListLog.AddString( _T( "Animation erstellt." ) );
  m_ListLog.AddString( GR::Convert::ToUTF16( CMisc::printf( "%d Bilder von %d konvertiert.", dwConverted, dwCount ) ).c_str() );
  m_ListLog.UpdateWindow();
}
