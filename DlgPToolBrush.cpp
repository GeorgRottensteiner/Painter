// ToolFarben.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "DlgPToolBrush.h"
#include "BrushTip.h"

#include "DialogVerlauf.h"
#include "DlgSelectPattern.h"
#include "PainterGfxTools.h"
#include "Settings.h"
#include "Import.h"

#include "PainterEnums.h"

#include <Grafik/GfxPage.h>

#include <Grafik/ImageFormate/ImageFormatManager.h>

#include <MFC/ODMenu.h>

#include <Misc/Misc.h>

#include <IO/FileUtil.h>

#include <WinSys/WinUtils.h>

#include <String/Convert.h>

#include ".\dlgptoolbrush.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CToolFarben 


CDlgPToolBrush::CDlgPToolBrush(CWnd* pParent /*=NULL*/)
  : CDialog(CDlgPToolBrush::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPToolBrush)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
  m_pPageForeground = NULL;
  m_pPageBackground = NULL;

}


void CDlgPToolBrush::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDlgPToolBrush)
  DDX_Control(pDX, IDC_BUTTON_BRUSH_FOREGROUNDTYPE, m_ButtonBrushForegroundType);
  DDX_Control(pDX, IDC_BUTTON_BRUSH_FOREGROUND, m_ButtonBrushForeground);
  DDX_Control(pDX, IDC_BUTTON_BRUSH_BACKGROUNDTYPE, m_ButtonBrushBackgroundType);
  DDX_Control(pDX, IDC_BUTTON_BRUSH_BACKGROUND, m_ButtonBrushBackground);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_ODLIST_BRUSHTIPS, m_ListBrushTips);
}


BEGIN_MESSAGE_MAP(CDlgPToolBrush, CDialog)
	//{{AFX_MSG_MAP(CDlgPToolBrush)
	ON_BN_CLICKED(IDC_BUTTON_BRUSH_FOREGROUND, OnButtonBrushForeground)
	ON_BN_CLICKED(IDC_BUTTON_BRUSH_BACKGROUND, OnButtonBrushBackground)
	ON_BN_CLICKED(IDC_BUTTON_BRUSH_FOREGROUNDTYPE, OnButtonBrushForegroundtype)
	ON_BN_CLICKED(IDC_BUTTON_BRUSH_BACKGROUNDTYPE, OnButtonBrushBackgroundtype)
	ON_BN_CLICKED(IDC_BUTTON_SWAPBRUSH, OnButtonSwapBrush)
  ON_LBN_SELCHANGE( IDC_ODLIST_BRUSHTIPS, OnListBrushTipSelChange )
	ON_WM_DRAWITEM()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





BOOL CDlgPToolBrush::OnInitDialog() 
{
	CDialog::OnInitDialog();

  RECT    rc;
  m_ButtonBrushForeground.GetWindowRect( &rc );

  m_pPageForeground = new GR::Graphic::GDIPage();
  m_pPageForeground->Create( m_ButtonBrushForeground.GetSafeHwnd(), rc.right - rc.left, rc.bottom - rc.top, 32 );

  m_pPageBackground = new GR::Graphic::GDIPage();
  m_ButtonBrushBackground.GetWindowRect( &rc );
  m_pPageBackground->Create( m_ButtonBrushBackground.GetSafeHwnd(), rc.right - rc.left, rc.bottom - rc.top, 32 );
	
  // Brush-Tip-Images laden
  m_ListBrushTips.SetItemSize( 16, 16 );
  m_ListBrushTips.SetOwnerDraw();

  std::list<GR::String>    listFiles;

  GR::Graphic::Image    ImageDefaultTip;

  ImageDefaultTip.Load( Win::Util::MemoryStreamFromResource( NULL, MAKEINTRESOURCEA( IDR_BRUSHTIP_1PIXEL ), "BRUSHTIP" ) );

  pSettings->m_pCurrentBrushTip = AddBrushTip( &ImageDefaultTip );

  GR::IO::FileUtil::EnumFilesInDirectory( CMisc::AppPath( "brushtips\\*.*" ), listFiles );

  std::list<GR::String>::iterator   itBT( listFiles.begin() );
  while ( itBT != listFiles.end() )
  {
    GR::String&    strFileName = *itBT;

    GR::Graphic::ImageData*     pData = ImageFormatManager::Instance().LoadData( strFileName.c_str() );

    if ( pData )
    {
      GR::Graphic::Image      Image( *pData );

      if ( Image.GetDepth() )
      {
        AddBrushTip( &Image );
      }

      delete pData;
    }

    itBT++;
  }

	return TRUE;

}



tBrushTip* CDlgPToolBrush::AddBrushTip( GR::Graphic::Image* pImage )
{

  GR::Graphic::Image*   pImageConverted = NULL;

  if ( pImage->GetDepth() > 8 )
  {
    CPainterImagePackage    Pack( pImage );
    pImageConverted = MapImageToPalette( &Pack, &GR::Graphic::Palette::AlphaPalette() );
    pImage = pImageConverted;
  }

  tBrushTip*    pTip = new tBrushTip( pImage );

  pTip->m_hbmTipMask = CreateBitmapFromImage( pImage, &GR::Graphic::Palette::AlphaPalette() );

  size_t iItem = m_ListBrushTips.AddString( "" );
  m_ListBrushTips.SetItemData( iItem, (DWORD_PTR)pTip );

  if ( pImageConverted )
  {
    delete pImageConverted;
  }

  return pTip;

}



void CDlgPToolBrush::OnButtonBrushForeground() 
{
  switch ( pSettings->brushForeground.Type )
  {
    case CSettings::BR_GRADIENT:
      {
        CDialogVerlauf      myDlg;

        myDlg.m_GradientCtrl.m_Gradient = pSettings->m_Gradient;
        myDlg.m_Brush = pSettings->brushForeground;
        if ( myDlg.DoModal() == IDOK )
        {
          pSettings->brushForeground = myDlg.m_Brush;
          pSettings->m_Gradient = myDlg.m_GradientCtrl.m_Gradient;
          RedrawPreviews();
        }
      }
      break;
    case CSettings::BR_PATTERN:
      {
        CDlgSelectPattern   myDlg;

        if ( myDlg.DoModal() == IDOK )
        {
          pSettings->brushForeground.SetPattern( myDlg.m_pSelectedPattern );
          if ( myDlg.m_pSelectedPattern )
          {
            pSettings->SetSetting( "BrushForegroundPattern", myDlg.m_pSelectedPattern->m_FileName );
          }
          RedrawPreviews();
        }
      }
      break;
  }
}



void CDlgPToolBrush::OnButtonBrushBackground() 
{
  switch ( pSettings->brushBackground.Type )
  {
    case CSettings::BR_GRADIENT:
      {
        CDialogVerlauf      myDlg;

        myDlg.m_GradientCtrl.m_Gradient = pSettings->m_Gradient;
        myDlg.m_Brush                   = pSettings->brushBackground;
        if ( myDlg.DoModal() == IDOK )
        {
          pSettings->brushBackground  = myDlg.m_Brush;
          pSettings->m_Gradient       = myDlg.m_GradientCtrl.m_Gradient;
          RedrawPreviews();
        }
      }
      break;
    case CSettings::BR_PATTERN:
      {
        CDlgSelectPattern   myDlg;

        if ( myDlg.DoModal() == IDOK )
        {
          pSettings->brushBackground.SetPattern( myDlg.m_pSelectedPattern );
          if ( myDlg.m_pSelectedPattern )
          {
            pSettings->SetSetting( "BrushBackgroundPattern", myDlg.m_pSelectedPattern->m_FileName );
          }
          RedrawPreviews();
        }
      }
      break;
  }
}



void CDlgPToolBrush::OnButtonBrushForegroundtype() 
{
  CPoint          point;

  DWORD           dwResult;

  ODMenu         odPopup( ODMenu::ODMS_XP );

  odPopup.AppendMenu( MF_STRING, CSettings::BR_SOLID,     _T( "Solid" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_GRADIENT,  _T( "Gradient" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_PATTERN,   _T( "Pattern" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_BRIGHTEN,  _T( "Brighten" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_DARKEN,    _T( "Darken" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_SMEAR,     _T( "Smear" ) );

  odPopup.CheckMenuItem( pSettings->brushForeground.Type, MF_BYCOMMAND | MF_CHECKED );

  GetCursorPos( &point );
  dwResult = odPopup.TrackPopupMenu( TPM_RETURNCMD, point.x, point.y, this );

  pSettings->brushForeground.Type = dwResult;

  pSettings->Notify( NF_BRUSH_MODE_CHANGED );
  
  RedrawPreviews();	
}



void CDlgPToolBrush::OnButtonBrushBackgroundtype() 
{
  CPoint          point;

  DWORD           dwResult;


  ODMenu         odPopup( ODMenu::ODMS_XP );

  odPopup.AppendMenu( MF_STRING, CSettings::BR_SOLID,     _T( "Solid" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_GRADIENT,  _T( "Gradient" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_PATTERN,   _T( "Pattern" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_BRIGHTEN,  _T( "Brighten" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_DARKEN,    _T( "Darken" ) );
  odPopup.AppendMenu( MF_STRING, CSettings::BR_SMEAR,     _T( "Smear" ) );

  odPopup.CheckMenuItem( pSettings->brushBackground.Type, MF_BYCOMMAND | MF_CHECKED );

  GetCursorPos( &point );
  dwResult = odPopup.TrackPopupMenu( TPM_RETURNCMD, point.x, point.y, this );

  pSettings->brushBackground.Type = dwResult;

  pSettings->Notify( NF_BRUSH_MODE_CHANGED );
  
  RedrawPreviews();	
}



void CDlgPToolBrush::OnButtonSwapBrush() 
{
  PainterBrush   BrushDummy = pSettings->brushBackground;

  pSettings->brushBackground = pSettings->brushForeground;
	pSettings->brushForeground = BrushDummy;

  RedrawPreviews();
}



void CDlgPToolBrush::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  if ( nIDCtl == IDC_BUTTON_BRUSH_FOREGROUND )
  {
    if ( m_pPageForeground == NULL )
    {
      return;
    }
    if ( m_ButtonBrushForeground.GetState() & 0x0004 )
    {
      DrawEdge( m_pPageForeground->GetDC(), &lpDrawItemStruct->rcItem, EDGE_SUNKEN, BF_RECT | BF_SOFT );
    }
    else
    {
      DrawEdge( m_pPageForeground->GetDC(), &lpDrawItemStruct->rcItem, EDGE_BUMP, BF_RECT | BF_SOFT );
    }
    BitBlt( lpDrawItemStruct->hDC, 0, 0, m_pPageForeground->GetWidth(), m_pPageForeground->GetHeight(), m_pPageForeground->GetDC(), 0, 0, SRCCOPY );
    return;
  }
  else if ( nIDCtl == IDC_BUTTON_BRUSH_BACKGROUND )
  {
    if ( m_pPageBackground == NULL )
    {
      return;
    }
    if ( m_ButtonBrushBackground.GetState() & 0x0004 )
    {
      DrawEdge( m_pPageBackground->GetDC(), &lpDrawItemStruct->rcItem, EDGE_SUNKEN, BF_RECT | BF_SOFT );
    }
    else
    {
      DrawEdge( m_pPageBackground->GetDC(), &lpDrawItemStruct->rcItem, EDGE_BUMP, BF_RECT | BF_SOFT );
    }
    BitBlt( lpDrawItemStruct->hDC, 0, 0, m_pPageBackground->GetWidth(), m_pPageBackground->GetHeight(), m_pPageBackground->GetDC(), 0, 0, SRCCOPY );
    return;
  }
  else if ( lpDrawItemStruct->hwndItem == m_ListBrushTips.GetSafeHwnd() )
  {
    FillRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, GetSysColorBrush( COLOR_WINDOW ) );

    if ( lpDrawItemStruct->itemID != -1 )
    {
      tBrushTip*    pTip = (tBrushTip*)lpDrawItemStruct->itemData;

      HDC hdcTemp = CreateCompatibleDC( lpDrawItemStruct->hDC );

      HGDIOBJ oldObj = SelectObject( hdcTemp, pTip->m_hbmTipMask );

      BITMAP    bm;
      ::GetObject( pTip->m_hbmTipMask, sizeof( BITMAP ), &bm );

      BitBlt( lpDrawItemStruct->hDC,
              lpDrawItemStruct->rcItem.left + ( lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left - bm.bmWidth ) / 2,
              lpDrawItemStruct->rcItem.top + ( lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top - bm.bmHeight ) / 2,
              bm.bmWidth,
              bm.bmHeight,
              hdcTemp,
              0,
              0,
              SRCCOPY );

      SelectObject( hdcTemp, oldObj );
      DeleteDC( hdcTemp );
    }
    if ( lpDrawItemStruct->itemState & ODS_FOCUS )
    {
      SetBkColor( lpDrawItemStruct->hDC, 0 );
      SetBkMode( lpDrawItemStruct->hDC, OPAQUE );
      DrawFocusRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem );
    }
    if ( lpDrawItemStruct->itemState & ODS_SELECTED )
    {
      DrawEdge( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, EDGE_RAISED, BF_RECT | BF_SOFT );
    }
    return;
  }
	
}

void CDlgPToolBrush::OnDestroy() 
{

  size_t    iItems = m_ListBrushTips.GetCount();

  for ( size_t i = 0; i < iItems; ++i )
  {
    tBrushTip*    pTip = (tBrushTip*)m_ListBrushTips.GetItemData( i );
    delete pTip;
  }
  m_ListBrushTips.ResetContent();

  if ( m_pPageForeground )
  {
    m_pPageForeground->Destroy();
    delete m_pPageForeground;
    m_pPageForeground = NULL;
  }
  if ( m_pPageBackground )
  {
    m_pPageBackground->Destroy();
    delete m_pPageBackground;
    m_pPageBackground = NULL;
  }

	CDialog::OnDestroy();
	
}



void CDlgPToolBrush::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember )
{

  if ( m_ButtonBrushForeground.GetSafeHwnd() == NULL )
  {
    return;
  }

  switch ( NotifyMessage )
  {
    case NF_ACTIVE_COLOR_CHANGED:
      {
        // Brush-Pages neu darstellen
        RedrawPreviews();
      }
      break;
  }


}



void CDlgPToolBrush::OnListBrushTipSelChange()
{

  int iItem = m_ListBrushTips.GetCurSel();

  if ( iItem == LB_ERR )
  {
    return;
  }
  pSettings->m_pCurrentBrushTip = (tBrushTip*)m_ListBrushTips.GetItemData( iItem );

}




void CDlgPToolBrush::RedrawPreviews()
{

  pSettings->brushForeground.DrawOnPage( m_pPageForeground, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );
  pSettings->brushBackground.DrawOnPage( m_pPageBackground, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) );
  m_ButtonBrushForeground.Invalidate( FALSE );
  m_ButtonBrushBackground.Invalidate( FALSE );

}

void CDlgPToolBrush::OnOK()
{
}

void CDlgPToolBrush::OnCancel()
{
}
