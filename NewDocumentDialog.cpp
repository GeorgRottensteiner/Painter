#include "stdafx.h"
#include "Settings.h"
#include "NewDocumentDialog.h"

#include <Misc/Misc.h>

#include <String/Convert.h>
#include ".\newdocumentdialog.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CNewDocumentDialog::CNewDocumentDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNewDocumentDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewDocumentDialog)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CNewDocumentDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewDocumentDialog)
	DDX_Control(pDX, IDC_EDIT_HOEHE, m_EditHeight);
	DDX_Control(pDX, IDC_EDIT_BREITE, m_EditWidth);
	DDX_Control(pDX, IDC_EDIT_FRAMES, m_EditFrames);
	DDX_Control(pDX, IDC_NEWDOCUMENT_TYPE, m_ComboType);
	DDX_Control(pDX, IDC_NEWDOCUMENT_BITDEPTH, m_ComboDepth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewDocumentDialog, CDialog)
	//{{AFX_MSG_MAP(CNewDocumentDialog)
	ON_CBN_SELCHANGE(IDC_NEWDOCUMENT_TYPE, OnSelchangeNewdocumentType)
	ON_CBN_SELCHANGE(IDC_NEWDOCUMENT_BITDEPTH, OnSelchangeNewdocumentBitdepth)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CNewDocumentDialog 


void CNewDocumentDialog::OnSelchangeNewdocumentType() 
{
  switch ( m_ComboType.GetCurSel() )
  {
    case 1:
      // Animation
      m_EditFrames.EnableWindow();
      m_EditWidth.EnableWindow();
      m_EditHeight.EnableWindow();
      m_Type = DT_IMAGE;
      break;
    case 0:
      // Einzelbild
      m_EditFrames.EnableWindow( FALSE );
      m_EditFrames.SetWindowText( _T( "1" ) );
      m_EditWidth.EnableWindow();
      m_EditHeight.EnableWindow();
      m_Type = DT_IMAGE;
      break;
    case 2:
      // Font
      m_EditFrames.EnableWindow( FALSE );
      m_EditWidth.EnableWindow();
      m_EditHeight.EnableWindow();
      m_Type = DT_FONT;
      break;
  }
  pSettings->SetSetting( "NewDocType", m_ComboType.GetCurSel() );
}



void CNewDocumentDialog::OnSelchangeNewdocumentBitdepth() 
{
  m_Depth = (GR::u8)m_ComboDepth.GetItemData( m_ComboDepth.GetCurSel() );
  pSettings->SetSetting( "NewDocDepth", m_ComboDepth.GetCurSel() );
}



BOOL CNewDocumentDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

  m_Type = DT_IMAGE;
  m_EditFrames.EnableWindow( FALSE );

  m_EditWidth.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", pSettings->GetSetting( "NewDocWidth", GetSystemMetrics( SM_CXSCREEN ) ) ) ).c_str() );
  m_EditHeight.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", pSettings->GetSetting( "NewDocHeight", GetSystemMetrics( SM_CYSCREEN ) ) ) ).c_str() );
  m_EditFrames.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", pSettings->GetSetting( "NewDocFrames", 1 ) ) ).c_str() );

  m_ComboType.ResetContent();
  m_ComboType.AddString( _T( "Single Image" ) );
  m_ComboType.AddString( _T( "Animation" ) );
  m_ComboType.AddString( _T( "Font" ) );

  m_ComboType.SetCurSel( pSettings->GetSetting( "NewDocType", 0 ) );
  OnSelchangeNewdocumentType();

  m_ComboDepth.ResetContent();
  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "2 colors (1bit)" ) ), 1 );
  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "16 colors (4bit)" ) ), 4 );
  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "256 colors (8bit)" ) ), 8 );
  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "HiColor 16bit" ) ), 15 );
  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "TrueColor 24bit" ) ), 24 );
  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "A8R8G8B8 32bit" ) ), 32 );
  m_ComboDepth.SetCurSel( pSettings->GetSetting( "NewDocDepth", 1 ) );

  OnSelchangeNewdocumentBitdepth();

	return TRUE;
}



void CNewDocumentDialog::OnOK() 
{
  GR::tChar          szDummy[MAX_PATH];


  // jetzt Variablen einlesen!
  m_EditWidth.GetWindowText( szDummy, MAX_PATH );
  m_Width = GR::Convert::ToI32( szDummy );
  m_EditHeight.GetWindowText( szDummy, MAX_PATH );
  m_Height = GR::Convert::ToI32( szDummy );
  m_EditFrames .GetWindowText( szDummy, MAX_PATH );
  m_Frames = GR::Convert::ToI32( szDummy );

  pSettings->SetSetting( "NewDocWidth", m_Width );
  pSettings->SetSetting( "NewDocHeight", m_Height );
  pSettings->SetSetting( "NewDocFrames", m_Frames );
	
	CDialog::OnOK();
}



void CNewDocumentDialog::OnBnClickedOk()
{
  OnOK();
}
