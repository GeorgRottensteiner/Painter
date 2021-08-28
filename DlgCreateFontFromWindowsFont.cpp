#include "stdafx.h"
#include "painter.h"
#include "DlgCreateFontFromWindowsFont.h"

#include <Debug\debugclient.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CPainterApp  theApp;



CDlgCreateFontFromWindowsFont::CDlgCreateFontFromWindowsFont(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCreateFontFromWindowsFont::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCreateFontFromWindowsFont)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgCreateFontFromWindowsFont::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCreateFontFromWindowsFont)
	DDX_Control(pDX, IDC_NEWFONT_DEPTH, m_ComboDepth);
	DDX_Control(pDX, IDC_EDIT_TEXT_FONTWIDTH, m_EditFontWidth);
	DDX_Control(pDX, IDC_EDIT_TEXT_FONTSIZE, m_EditTextSize);
	DDX_Control(pDX, IDC_COMBO_FONT, m_ComboFont);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCreateFontFromWindowsFont, CDialog)
	//{{AFX_MSG_MAP(CDlgCreateFontFromWindowsFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



int CALLBACK EnumFontProcCF( LOGFONT *lplf, TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData )
{
  CComboBox* pCombo = (CComboBox*)lpData;

  // im Moment nur TrueType-Fonts
  pCombo->AddString( lplf->lfFaceName );

  return TRUE;
}


 
BOOL CDlgCreateFontFromWindowsFont::OnInitDialog() 
{
	CDialog::OnInitDialog();

  {
    HDC   hdc = ::GetDC( NULL );

    EnumFonts( hdc, NULL, (FONTENUMPROC)EnumFontProcCF, (LPARAM)&m_ComboFont );

    ::ReleaseDC( NULL, hdc );

    m_ComboFont.SetCurSel( 0 );
  }

  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "8 bit" ) ), 8 );
  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "16 bit" ) ), 15 );
  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "24 bit" ) ), 24 );
  m_ComboDepth.SetItemData( m_ComboDepth.AddString( _T( "32 bit" ) ), 32 );

  m_ComboDepth.SetCurSel( 1 );
	
  m_EditFontWidth.SetWindowText( _T( "0" ) );
  m_EditTextSize.SetWindowText( _T( "14" ) );
	
	return TRUE;
}



void CDlgCreateFontFromWindowsFont::OnOK() 
{
  CString   cstrGnu;
  
  m_EditTextSize.GetWindowText( cstrGnu );
  int   iFontSize = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );

  m_EditFontWidth.GetWindowText( cstrGnu );
  int   iFontWidth = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );

  int iCurSel = m_ComboDepth.GetCurSel();
	BYTE   iDepth = (BYTE)m_ComboDepth.GetItemData( iCurSel );

  GR::tChar    szFont[MAX_PATH];
  m_ComboFont.GetLBText( m_ComboFont.GetCurSel(), szFont );

  HFONT   hFont = CreateFont( iFontSize, iFontWidth, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, szFont );

  GR::Graphic::GDIPage      pageDummy;

  pageDummy.Create( m_hWnd, 100, 100, (unsigned char)iDepth );

  pageDummy.Box( 0, 0, 99, 99, 0 );


  HDC  hdc = pageDummy.GetDC();

  HFONT hOldFont = (HFONT)SelectObject( hdc, hFont );

  SetTextColor( hdc, pSettings->GetColorRef( CSettings::CO_WORKCOLOR ) );
  SetBkColor( hdc, pSettings->GetColorRef( CSettings::CO_WORKCOLOR_2 ) );


  CPainterFontDoc       *pDoc;

  CWnd              *pWnd;


  pDoc = (CPainterFontDoc*)theApp.pFontTemplate->CreateNewDocument();

  pWnd = theApp.pFontTemplate->CreateNewFrame( pDoc, (CFrameWnd*)theApp.pMainFrame );

  pDoc->SetTitle( _T( "Neuer Font" ) );
  POSITION              pos;

  CView*                pFirstView;


  pos = pDoc->GetFirstViewPosition();
  pFirstView = pDoc->GetNextView( pos );

	if ( !pDoc->OnNewDocument() )
  {
    dh::Log( "Failed a\n" );
		return;
  }

  RECT    rc,
          rc2;

  rc.left = 0;
  rc.top = 0;
  rc.right = 100;
  rc.bottom = 100;

  for ( int i = 0; i < 256; i++ )
  {
    GR::tChar    szTemp[3];
    szTemp[1] = 0;
    szTemp[0] = (char)i;

    rc2 = rc;

    DrawText( hdc, szTemp, (int)_tcslen( szTemp ), &rc2, DT_LEFT | DT_SINGLELINE | DT_CALCRECT );
    DrawText( hdc, szTemp, (int)_tcslen( szTemp ), &rc, DT_LEFT | DT_SINGLELINE );

    GR::Graphic::Image*    pImage = new GR::Graphic::Image( (WORD)( rc2.right - rc2.left ), (WORD)( rc2.bottom - rc2.top ), iDepth );

    pImage->GetImage( &pageDummy, 0, 0 );
    pDoc->diInfo.AddLayer( pImage, pDoc->diInfo.AddFrame() );
  }

  SelectObject( hdc, hOldFont );
  DeleteObject( hFont );

  pageDummy.Destroy();


  pDoc->diInfo.m_SaveType     = SAVETYPE_UNKNOWN;
  pDoc->diInfo.SetSize( 600, 600 );
  pDoc->diInfo.m_DocType      = DT_FONT;
  pDoc->diInfo.CurrentFrame( 65 );
  pDoc->diInfo.SetSize( 600, 600 );
  pDoc->diInfo.m_BitDepth = iDepth;

  if ( iDepth <= 8 )
  {
    // und eine Palette
    *pDoc->diInfo.GetPalette( pDoc->diInfo.CurrentFrame() ) = theApp.m_PainterPalette;
  }

  // und das ganze neu zeichnen
  pFirstView->Invalidate( FALSE );
  pFirstView->GetParentFrame()->Invalidate( FALSE );

  pDoc->diInfo.SetModify( TRUE );
  theApp.pFontTemplate->InitialUpdateFrame( (CFrameWnd*)pWnd, pDoc, TRUE );
  
	CDialog::OnOK();
}
