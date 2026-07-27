#include "stdafx.h"
#include "PainterDoc.h"
#include "DialogResize.h"

#include <String/Convert.h>
#include "Resampler.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDialogResize::CDialogResize( CWnd* pParent /*=NULL*/ )
  : CDialog( CDialogResize::IDD, pParent ),
  PercentageX( 100 ),
  PercentageY( 100 ),
  m_pDocInfo( NULL ),
  Width( 0 ),
  Height( 0 ),
  ResizeType( 0 ),
  m_DoNotUpdate( false ),
  KeepAspectRatio( true ),
  _LastChangeWasPercentage( false )
{
	//{{AFX_DATA_INIT(CDialogResize)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CDialogResize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogResize)
	DDX_Control(pDX, IDC_COMBO_RESIZE_ART, m_ComboResize);
	DDX_Control(pDX, IDC_REDIT_NEWWIDTH, m_REditNewWidth);
  DDX_Control(pDX, IDC_REDIT_NEWHEIGHT, m_REditNewHeight);
	DDX_Control(pDX, IDC_REDIT_NEWWIDTH_PERCENT, m_REditNewWidthP);
  DDX_Control(pDX, IDC_REDIT_NEWHEIGHT_PERCENT, m_REditNewHeightP);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogResize, CDialog)
	//{{AFX_MSG_MAP(CDialogResize)
	ON_BN_CLICKED(IDC_RESIZE_CHECK_KEEP_ASPECTRATIO, OnResizeCheckKeepAspectratio)
	ON_CBN_EDITCHANGE(IDC_COMBO_RESIZE_ART, OnEditchangeComboResizeArt)
  ON_EN_CHANGE(IDC_REDIT_NEWWIDTH, OnChangeREditNewWidth)
  ON_EN_CHANGE(IDC_REDIT_NEWHEIGHT, OnChangeREditNewHeight)
  ON_EN_CHANGE(IDC_REDIT_NEWWIDTH_PERCENT, OnChangeREditNewWidthP)
  ON_EN_CHANGE(IDC_REDIT_NEWHEIGHT_PERCENT, OnChangeREditNewHeightP)
	//}}AFX_MSG_MAP
  ON_CBN_SELCHANGE(IDC_COMBO_RESIZE_ART, OnCbnSelchangeComboResizeArt)
END_MESSAGE_MAP()



BOOL CDialogResize::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_DoNotUpdate = true;

  m_REditNewWidth.SetRange( 0, 10000 );
  m_REditNewHeight.SetRange( 0, 10000 );

  m_REditNewWidthP.SetRange( 0, 1000 );
  m_REditNewHeightP.SetRange( 0, 1000 );

  SetDlgItemInt( IDC_RESIZE_BREITEALT,  m_pDocInfo->Width(), FALSE );
  SetDlgItemInt( IDC_RESIZE_HOEHEALT,   m_pDocInfo->Height(), FALSE );

  _LastChangeWasPercentage = !!pSettings->GetSetting( "NewResize.UsingPercentage", 1 );

  ( (CButton*)GetDlgItem( IDC_RESIZE_CHECK_KEEP_ASPECTRATIO ) )->SetCheck( pSettings->GetSetting( "NewResize.KeepRatio", 1 ) );
	
  if ( _LastChangeWasPercentage )
  {
    m_REditNewWidthP.SetPosition( pSettings->GetSetting( "NewResizeWidthPercent", 100 ) );
    m_REditNewHeightP.SetPosition( pSettings->GetSetting( "NewResizeHeightPercent", 100 ) );

    PercentageX = pSettings->GetSetting( "NewResizeWidthPercent", 100 );
    PercentageY = pSettings->GetSetting( "NewResizeHeightPercent", 100 );

    m_REditNewWidth.SetPosition( (int)m_pDocInfo->Width() * pSettings->GetSetting( "NewResizeWidthPercent", 100 ) / 100 );
    m_REditNewHeight.SetPosition( (int)m_pDocInfo->Height() * pSettings->GetSetting( "NewResizeHeightPercent", 100 ) / 100 );
  }
  else
  {
    m_REditNewWidth.SetPosition( pSettings->GetSetting( "NewResizeWidth", 100 ) );
    m_REditNewHeight.SetPosition( pSettings->GetSetting( "NewResizeHeight", 100 ) );

    int   iPercentH = 100 * pSettings->GetSetting( "NewResizeWidth", 100 ) / m_pDocInfo->Width();
    int   iPercentV = 100 * pSettings->GetSetting( "NewResizeHeight", 100 ) / m_pDocInfo->Height();

    m_REditNewWidthP.SetPosition( iPercentH );
    if ( KeepAspectRatio )
    {
      m_REditNewHeightP.SetPosition( iPercentV );
    }
  }

  m_DoNotUpdate = false;

  KeepAspectRatio  = !!pSettings->GetSetting( "NewResize.KeepRatio", 1 );

  m_ComboResize.ResetContent();
  int item = m_ComboResize.AddString( _T( "Pixel für Pixel" ) );
  m_ComboResize.SetItemData( item, RESIZE_PIXEL );
  item = m_ComboResize.AddString( _T( "Bilinear" ) );
  m_ComboResize.SetItemData( item, RESIZE_BILINEAR );

  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Box" ) ), 10 + Resampler::SM_BOX );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Triangle" ) ), 10 + Resampler::SM_TRIANGLE );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Hermite" ) ), 10 + Resampler::SM_HERMITE );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Bell" ) ), 10 + Resampler::SM_BELL );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Cubic B Spline" ) ), 10 + Resampler::SM_CUBICBSPLINE );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Lanczos3" ) ), 10 + Resampler::SM_LANCZOS3 );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Mitchell" ) ), 10 + Resampler::SM_MITCHELL );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Cosine" ) ), 10 + Resampler::SM_COSINE );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "CatmullRom" ) ), 10 + Resampler::SM_CATMULLROM );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Quadratic" ) ), 10 + Resampler::SM_QUADRATIC );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Quadratic B Spline" ) ), 10 + Resampler::SM_QUADRATICBSPLINE );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Cubic Convolution" ) ), 10 + Resampler::SM_CUBICCONVOLUTION );
  m_ComboResize.SetItemData( m_ComboResize.AddString( _T( "Lanczos8" ) ), 10 + Resampler::SM_LANCZOS8 );

  int   setting = pSettings->GetSetting( "NewResizeMethod", 0 );

  if ( setting >= 10 )
  {
    setting -= 10;
    setting += 2;
  }
  m_ComboResize.SetCurSel( setting );

  ResizeType = pSettings->GetSetting( "NewResizeMethod", 0 );

	return TRUE;
}



void CDialogResize::OnOK() 
{
  CString     cstrGnu;

  m_REditNewWidth.GetWindowText( cstrGnu );
  Width = GR::Convert::ToU32( LPCTSTR( cstrGnu ) );
  m_REditNewHeight.GetWindowText( cstrGnu );
  Height = GR::Convert::ToU32( LPCTSTR( cstrGnu ) );

  KeepAspectRatio = !!( (CButton*)GetDlgItem( IDC_RESIZE_CHECK_KEEP_ASPECTRATIO ) )->GetCheck();

  pSettings->SetSetting( "NewResize.KeepRatio", KeepAspectRatio ? 1 : 0 );
  pSettings->SetSetting( "NewResize.UsingPercentage", _LastChangeWasPercentage ? 1 : 0 );

  int item = m_ComboResize.GetCurSel();
  if ( item != CB_ERR )
  {
    ResizeType = (DWORD)m_ComboResize.GetItemData( item );

    pSettings->SetSetting( "NewResizeMethod", ResizeType );
  }

	CDialog::OnOK();
}



void CDialogResize::OnChangeREditNewWidth() 
{
  DWORD         dwWidth,
                dwHeight;


  if ( m_DoNotUpdate )
  {
    return;
  }

  m_DoNotUpdate = true;

  _LastChangeWasPercentage = false;
  
  CString   cstrGnu;
  m_REditNewWidth.GetWindowText( cstrGnu );
  dwWidth = GR::Convert::ToU32( LPCTSTR( cstrGnu ) );
  m_REditNewHeight.GetWindowText( cstrGnu );
  dwHeight = GR::Convert::ToU32( LPCTSTR( cstrGnu ) );

  if ( KeepAspectRatio )
  {
    if ( m_pDocInfo->Width() != 0 )
    {
      dwHeight = ( dwWidth * m_pDocInfo->Height() ) / m_pDocInfo->Width();
      m_REditNewHeight.SetPosition( (int)dwHeight );
    }
  }

  pSettings->SetSetting( "NewResizeWidth", dwWidth );
  pSettings->SetSetting( "NewResizeHeight", dwHeight );

  int   iPercentH = 100 * dwWidth / m_pDocInfo->Width();
  int   iPercentV = 100 * dwHeight / m_pDocInfo->Height();

  PercentageX = iPercentH;
  PercentageY = iPercentV;

  pSettings->SetSetting( "NewResizeWidthPercent", iPercentH );
  pSettings->SetSetting( "NewResizeHeightPercent", iPercentV );

  m_REditNewWidthP.SetPosition( iPercentH );
  if ( KeepAspectRatio )
  {
    m_REditNewHeightP.SetPosition( iPercentV );
  }
  m_DoNotUpdate = false;
}



void CDialogResize::OnChangeREditNewHeight() 
{
  DWORD         dwWidth,
                dwHeight;


  if ( m_DoNotUpdate )
  {
    return;
  }

  m_DoNotUpdate = true;
  _LastChangeWasPercentage = false;
  
  CString   cstrGnu;
  m_REditNewWidth.GetWindowText( cstrGnu );
  dwWidth = GR::Convert::ToU32( LPCTSTR( cstrGnu ) );
  m_REditNewHeight.GetWindowText( cstrGnu );
  dwHeight = GR::Convert::ToU32( LPCTSTR( cstrGnu ) );

  if ( KeepAspectRatio )
  {
    if ( m_pDocInfo->Height() != 0 )
    {
      dwWidth = ( dwHeight * m_pDocInfo->Width() ) / m_pDocInfo->Height();
      m_REditNewWidth.SetPosition( (int)dwWidth );
    }
  }

  pSettings->SetSetting( "NewResizeWidth", dwWidth );
  pSettings->SetSetting( "NewResizeHeight", dwHeight );

  int   iPercentH = 100 * dwWidth / m_pDocInfo->Width();
  int   iPercentV = 100 * dwHeight / m_pDocInfo->Height();

  PercentageX = iPercentH;
  PercentageY = iPercentV;

  pSettings->SetSetting( "NewResizeWidthPercent", iPercentH );
  pSettings->SetSetting( "NewResizeHeightPercent", iPercentV );

  if ( KeepAspectRatio )  
  {
    m_REditNewWidthP.SetPosition( iPercentH );
  }
  m_REditNewHeightP.SetPosition( iPercentV );

  m_DoNotUpdate = false;
}



void CDialogResize::OnChangeREditNewWidthP() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }
  DWORD         dwWidth,
                dwHeight;

  int           iWidthP,
                iHeightP;
                

  m_DoNotUpdate = true;
  _LastChangeWasPercentage = true;

  CString   cstrGnu;
  m_REditNewWidthP.GetWindowText( cstrGnu );
  iWidthP = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );
  PercentageX = iWidthP;

  m_REditNewHeightP.GetWindowText( cstrGnu );
  iHeightP = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );
  PercentageY = iHeightP;

  dwWidth   = m_pDocInfo->Width() * iWidthP / 100;
  dwHeight  = m_pDocInfo->Height() * iHeightP / 100;

  if ( KeepAspectRatio )
  {
    dwHeight  = m_pDocInfo->Height() * iWidthP / 100;
    iHeightP = iWidthP;
    PercentageY = iHeightP;
    m_REditNewHeightP.SetPosition( iWidthP );
  }

  pSettings->SetSetting( "NewResizeWidthPercent", iWidthP );
  pSettings->SetSetting( "NewResizeHeightPercent", iHeightP );

  if ( m_pDocInfo->Width() != 0 )
  {
    m_REditNewHeight.SetPosition( (int)dwHeight );
  }
  if ( m_pDocInfo->Height() != 0 )
  {
    m_REditNewWidth.SetPosition( (int)dwWidth );
  }
  m_DoNotUpdate = false;
}



void CDialogResize::OnChangeREditNewHeightP() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }
  DWORD         dwWidth,
                dwHeight;

  int           iWidthP,
                iHeightP;
                

  m_DoNotUpdate = true;

  _LastChangeWasPercentage = true;
  
  CString   cstrGnu;
  m_REditNewWidthP.GetWindowText( cstrGnu );
  iWidthP = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );
  PercentageX = iWidthP;

  m_REditNewHeightP.GetWindowText( cstrGnu );
  iHeightP = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );
  PercentageY = iHeightP;

  dwWidth   = m_pDocInfo->Width() * iWidthP / 100;
  dwHeight  = m_pDocInfo->Height() * iHeightP / 100;

  if ( KeepAspectRatio )
  {
    dwWidth  = m_pDocInfo->Width() * iHeightP / 100;
    iWidthP = iHeightP;
    m_REditNewWidthP.SetPosition( iHeightP );
    PercentageX = iWidthP;
  }

  pSettings->SetSetting( "NewResizeWidthPercent", iWidthP );
  pSettings->SetSetting( "NewResizeHeightPercent", iHeightP );

  if ( m_pDocInfo->Width() != 0 )
  {
    m_REditNewHeight.SetPosition( (int)dwHeight );
  }
  if ( m_pDocInfo->Height() != 0 )
  {
    m_REditNewWidth.SetPosition( (int)dwWidth );
  }
  m_DoNotUpdate = false;
}



void CDialogResize::OnResizeCheckKeepAspectratio() 
{
  KeepAspectRatio = !!SendDlgItemMessage( IDC_RESIZE_CHECK_KEEP_ASPECTRATIO, BM_GETCHECK, 0, 0 );
}



void CDialogResize::OnEditchangeComboResizeArt() 
{
  DWORD       dwDummy;
  

  dwDummy = m_ComboResize.GetCurSel();
  if ( dwDummy != CB_ERR )
  {
    ResizeType = (DWORD)m_ComboResize.GetItemData( dwDummy );
  }
}



void CDialogResize::OnCbnSelchangeComboResizeArt()
{
}
