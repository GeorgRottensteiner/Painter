#include "stdafx.h"
#include "settings.h"
#include "PainterGfxTools.h"
#include "DialogVerlauf.h"

#include <Misc/Misc.h>
#include "ColorPicker.h"

#include <String/XML.h>
#include <String/Path.h>

#include <String/Convert.h>

#include <Misc/Format.h>

#include <IO/FileUtil.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDialogVerlauf::CDialogVerlauf(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogVerlauf::IDD, pParent)
{
  m_pPagePreview = NULL;
  m_pImageTarget = NULL;
  m_DoNotUpdate  = false;
  m_RedrawPage   = true;
	//{{AFX_DATA_INIT(CDialogVerlauf)
	//}}AFX_DATA_INIT
}



void CDialogVerlauf::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDialogVerlauf)
  DDX_Control(pDX, IDC_STATIC_GRADIENT_PREVIEW, m_StaticPreview);
  DDX_Control(pDX, IDC_SPIN_GRADIENT_ANGLE, m_SpinAngle);
  DDX_Control(pDX, IDC_EDIT_GRADIENT_ANGLE, m_EditAngle);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_ANGLEWND, m_AngleWnd);
  DDX_Control(pDX, IDC_GRADIENT_WND, m_GradientCtrl);
  DDX_Control(pDX, IDC_BUTTON_COLOR, m_ButtonPegColor);
  DDX_Control(pDX, IDC_REDIT_PEGPOS, m_REditPegPos);
  DDX_Control(pDX, IDC_COMBO_PRESET_GRADIENTS, m_ComboPresetGradients);
  DDX_Control(pDX, IDC_EDIT_GRADIENT_NAME, m_EditPresetName);
  DDX_Control(pDX, IDC_COMBO_GRADIENT_TYPE, m_ComboGradientType);
}



BEGIN_MESSAGE_MAP(CDialogVerlauf, CDialog)
	//{{AFX_MSG_MAP(CDialogVerlauf)
	ON_WM_DESTROY()
	ON_WM_DRAWITEM()
	ON_EN_CHANGE(IDC_EDIT_GRADIENT_ANGLE, OnChangeEditGradientAngle)
  ON_EN_CHANGE(IDC_ANGLEWND, OnChangeAngleWnd)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BUTTON_ADD_PEG, OnBnClickedButtonAddPeg)
  ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
  ON_BN_CLICKED(IDC_BUTTON_SAVE_GRADIENT, OnBnClickedButtonSaveGradient)
  ON_CBN_SELCHANGE(IDC_COMBO_PRESET_GRADIENTS, OnCbnSelchangeComboPresetGradients)
  ON_CBN_SELCHANGE(IDC_COMBO_GRADIENT_TYPE, OnCbnSelchangeComboGradientType)
END_MESSAGE_MAP()



BOOL CDialogVerlauf::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_DoNotUpdate  = false;
  m_RedrawPage   = true;

  RECT    rc;

  m_StaticPreview.GetWindowRect( &rc );
	
  m_pPagePreview = new GR::Graphic::GDIPage();
  m_pPagePreview->Create( m_StaticPreview.GetSafeHwnd(), rc.right - rc.left, rc.bottom - rc.top, 32 );

  m_StaticPreview.ModifyStyle( 0, SS_OWNERDRAW, 0 );

  GR::Char    szTemp[50];

  sprintf_s( szTemp, 50, ( Misc::Format( "%1.2f" ) << m_Brush.Angle ).Result().c_str() );
  m_SpinAngle.SetRange( 0, 360 );
  m_EditAngle.SetWindowText( GR::Convert::ToUTF16( szTemp ).c_str() );

  m_AngleWnd.SetAngle( m_Brush.Angle );

  m_pImageTarget = new GR::Graphic::Image( m_pPagePreview->GetWidth(), m_pPagePreview->GetHeight(), 32 );

  m_PegColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR );
  m_ButtonPegColor.SetColor( m_PegColor );

  m_REditPegPos.SetRange( 0.0f, 100.0f );
  m_REditPegPos.SetPosition( 50.0f );

  m_ComboGradientType.AddString( GR::Convert::ToUTF16( "Linear" ).c_str() );
  m_ComboGradientType.AddString( GR::Convert::ToUTF16( "Circular" ).c_str() );

  m_ComboGradientType.SetCurSel( pSettings->GetSetting( "GradientType" ) );

  RefillCombo();

	return TRUE;
}



void CDialogVerlauf::OnDestroy() 
{
	CDialog::OnDestroy();
	
  SafeDelete( m_pImageTarget );
  SafeDelete( m_pPagePreview );
}



void CDialogVerlauf::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  if ( nIDCtl == m_StaticPreview.GetDlgCtrlID() )
  {
    if ( m_RedrawPage )
    {
      if ( ( m_pImageTarget != NULL )
      &&   ( m_pPagePreview != NULL ) )
      {
        GR::Graphic::Image*   pImageDummy = new GR::Graphic::Image( m_pPagePreview->GetWidth(), m_pPagePreview->GetHeight(), 8 );

        GR::Graphic::ContextDescriptor    cdDummy( pImageDummy, &GR::Graphic::Palette::AlphaPalette() );

        cdDummy.Box( 0, 0, cdDummy.Width(), cdDummy.Height(), 0xffffffff );

        CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( m_pImageTarget, NULL );
        m_Brush.ApplyBrushTipOnMask( NULL, pCD, pImageDummy );
        SafeDelete( pCD );
        SafeDelete( pImageDummy );
        //m_StaticPreview.Invalidate();
      }


      m_RedrawPage = false;
    }
    m_pImageTarget->PutImage( m_pPagePreview, 0, 0 );
  	
	  BitBlt( lpDrawItemStruct->hDC, 0, 0, m_pPagePreview->GetWidth(), m_pPagePreview->GetHeight(), m_pPagePreview->GetDC(), 0, 0, SRCCOPY );
    return;
  }

  CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
}



void CDialogVerlauf::OnChangeEditGradientAngle() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }
  if ( m_pImageTarget == NULL )
  {
    return;
  }

  m_DoNotUpdate = true;

  GR::tChar    szTemp[MAX_PATH];

  m_EditAngle.GetWindowText( szTemp, MAX_PATH );

  float   fNewValue = GR::Convert::ToF32( szTemp );

  m_Brush.Angle = fNewValue;
  m_AngleWnd.SetAngle( fNewValue );

  m_RedrawPage = true;
  m_StaticPreview.Invalidate();

  m_DoNotUpdate = false;
}



void CDialogVerlauf::OnChangeAngleWnd() 
{
  if ( m_DoNotUpdate )
  {
    return;
  }
  if ( m_pImageTarget == NULL )
  {
    return;
  }

  m_DoNotUpdate = true;

  float newValue  = m_AngleWnd.GetAngle();
  m_Brush.Angle   = newValue;
  m_AngleWnd.SetAngle( newValue );

  GR::Char    szTemp[50];

  sprintf_s( szTemp, 50, ( Misc::Format( "%1:2%" ) << m_Brush.Angle ).Result().c_str() );
  m_EditAngle.SetWindowText( GR::Convert::ToUTF16( szTemp ).c_str() );

  m_RedrawPage = true;
  m_StaticPreview.Invalidate();

  m_DoNotUpdate = false;
}



BOOL CDialogVerlauf::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
  if ( wParam == m_AngleWnd.GetDlgCtrlID() )
  {
    NMHDR*    pHdr = (NMHDR*)lParam;

    if ( pHdr->code == EN_CHANGE )
    {
      OnChangeAngleWnd();
    }
  }

  return CDialog::OnNotify(wParam, lParam, pResult);
}



void CDialogVerlauf::OnBnClickedButtonAddPeg()
{
  float   pos = m_REditPegPos.GetPositionF();
  m_GradientCtrl.AddPeg( m_PegColor, pos );
}



void CDialogVerlauf::OnBnClickedButtonColor()
{
  CColorPicker    dlgCP;

  dlgCP.m_WorkColor = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR );

  if ( dlgCP.DoModal() == IDOK )
  {
    pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR, dlgCP.m_WorkColor );
    m_PegColor = dlgCP.m_WorkColor;
    m_ButtonPegColor.SetColor( m_PegColor );
  }
}



BOOL CDialogVerlauf::PreTranslateMessage(MSG* pMsg)
{
  if ( pMsg->hwnd == m_GradientCtrl.GetSafeHwnd() )
  {
    if ( pMsg->message == WM_LBUTTONDBLCLK )
    {
      POINT   point;

      point.x = LOWORD( pMsg->lParam );
      point.y = HIWORD( pMsg->lParam );

      GR::Graphic::Gradient::tPeg*    pPeg = m_GradientCtrl.PegFromPoint( point );

      if ( pPeg )
      {
        CColorPicker    dlgCP;

        dlgCP.m_WorkColor = pPeg->Color;

        if ( dlgCP.DoModal() == IDOK )
        {
          pPeg->Color = dlgCP.m_WorkColor;
          m_GradientCtrl.Invalidate();
        }
        return TRUE;
      }
    }
  }
  if ( ( pMsg->hwnd == GetSafeHwnd() )
  &&   ( pMsg->message == CGradientCtrl::m_dwNotifyMessage ) )
  {
    pSettings->m_Gradient = m_GradientCtrl.m_Gradient;
    m_RedrawPage = true;
    m_StaticPreview.Invalidate();
  }

  return CDialog::PreTranslateMessage(pMsg);
}



void CDialogVerlauf::OnBnClickedButtonSaveGradient()
{
  GR::Graphic::Gradient&    Gradient = m_GradientCtrl.m_Gradient;

  CString       cstrFileName;
  
  m_EditPresetName.GetWindowText( cstrFileName );

  if ( cstrFileName.GetLength() == 0 )
  {
    AfxMessageBox( _T( "Invalid File Name" ) );
    return;
  }

  GR::Strings::XML      xmlGradient;

  GR::Strings::XMLElement*    pGradient = new GR::Strings::XMLElement( "Gradient" );

  xmlGradient.AddElement( NULL, pGradient );

  for ( size_t i = 0; i < Gradient.PegCount(); ++i )
  {
    GR::Strings::XMLElement*    pXMLPeg = new GR::Strings::XMLElement( "Peg" );

    GR::Graphic::Gradient::tPeg*    pPeg = Gradient.GetPeg( i );

    pXMLPeg->AddAttribute( "Color", CMisc::printf( "%08X", pPeg->Color ) );
    if ( pPeg->Type == GR::Graphic::Gradient::tPeg::PEG_START )
    {
      pXMLPeg->AddAttribute( "Type", "Start" );
    }
    else if ( pPeg->Type == GR::Graphic::Gradient::tPeg::PEG_END )
    {
      pXMLPeg->AddAttribute( "Type", "End" );
    }
    else
    {
      pXMLPeg->AddAttribute( "Position", CMisc::printf( "%.4f", pPeg->Position ) );
    }

    pGradient->InsertChild( pXMLPeg );
  }

  if ( !xmlGradient.Save( CMisc::AppPath( "gradients\\%s.grd", GR::Convert::ToUTF8( (LPCTSTR)cstrFileName ).c_str() ).c_str() ) )
  {
    AfxMessageBox( GR::Convert::ToUTF16( CMisc::printf( "Failed to write File (%s)", CMisc::AppPath( "gradients\\%s.grd", (const GR::Char*)GR::Convert::ToUTF8( (LPCTSTR)cstrFileName ).c_str() ) ) ).c_str() );
  }
  else
  {
    RefillCombo();
  }
}



void CDialogVerlauf::RefillCombo()
{
  m_ComboPresetGradients.ResetContent();

  std::list<GR::String>    listFiles;
  
  GR::IO::FileUtil::EnumFilesInDirectory( CMisc::AppPath( "gradients\\*.grd" ), listFiles );

  std::list<GR::String>::iterator    it( listFiles.begin() );
  while ( it != listFiles.end() )
  {
    GR::String     strFileName = Path::RemoveExtension( *it );

    strFileName = Path::StripPath( strFileName );

    m_ComboPresetGradients.AddString( GR::Convert::ToUTF16( strFileName ).c_str() );

    ++it;
  }
}



void CDialogVerlauf::OnCbnSelchangeComboPresetGradients()
{
  int iItem = m_ComboPresetGradients.GetCurSel();

  if ( iItem == CB_ERR )
  {
    return;
  }

  CString   cstrFileName;

  m_ComboPresetGradients.GetLBText( iItem, cstrFileName );

  
  GR::Graphic::Gradient&    Gradient = m_GradientCtrl.m_Gradient;
  
  Gradient.Clear();

  GR::Strings::XML      xmlGradient;

  if ( !xmlGradient.Load( CMisc::AppPath( "gradients\\%s.grd", GR::Convert::ToUTF8( (LPCTSTR)cstrFileName ).c_str() ).c_str() ) )
  {
    return;
  }

  GR::Strings::XMLElement*    pGradient = xmlGradient.FindByType( "Gradient" );

  if ( pGradient == NULL )
  {
    return;
  }

  GR::Graphic::Gradient::tPeg*    pPeg = NULL;

  GR::Strings::XML::iterator    it( pGradient->FirstChild() );
  while ( it != GR::Strings::XML::iterator() )
  {
    GR::Strings::XMLElement*    pXMLPeg = *it;

    if ( pXMLPeg->Type() == "Peg" )
    {
      pPeg = Gradient.AddPeg( GR::Convert::ToU32( pXMLPeg->Attribute( "Color" ), 16 ), 
                              GR::Convert::ToF32( pXMLPeg->Attribute( "Position" ) ) );
    }

    it = it.next_sibling();
  }

  pSettings->m_Gradient = Gradient;

  m_GradientCtrl.Invalidate();

  m_RedrawPage = true;
  m_StaticPreview.Invalidate();
}



void CDialogVerlauf::OnCbnSelchangeComboGradientType()
{
  pSettings->SetSetting( "GradientType", m_ComboGradientType.GetCurSel() );
  m_RedrawPage = true;
  m_StaticPreview.Invalidate();
}
