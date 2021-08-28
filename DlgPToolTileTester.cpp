#include "stdafx.h"
#include "PainterEnums.h"
#include "DlgPToolTileTester.h"
#include "DocumentInfo.h"
#include <Misc/Misc.h>

#include <String/Convert.h>

#include "PainterGFXTools.h"
#include ".\dlgptooltiletester.h"



IMPLEMENT_DYNAMIC(CDlgPToolTileTester, CDialog)
CDlgPToolTileTester::CDlgPToolTileTester(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPToolTileTester::IDD, pParent),
    m_pImageTile( NULL )
{
}

CDlgPToolTileTester::~CDlgPToolTileTester()
{

  ClearTileSet();
  m_Page.Destroy();

}

void CDlgPToolTileTester::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT_WIDTH, m_EditWidth);
  DDX_Control(pDX, IDC_EDIT_HEIGHT, m_EditHeight);
  DDX_Control(pDX, IDC_COMBO_DOCUMENTS, m_ComboDocs);
  DDX_Control(pDX, IDC_STATIC_TILETESTBED, m_StaticTestBed);
}


BEGIN_MESSAGE_MAP(CDlgPToolTileTester, CDialog)
  ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnBnClickedButtonRefresh)
  ON_CBN_SELCHANGE(IDC_COMBO_DOCUMENTS, OnCbnSelchangeComboDocuments)
  ON_WM_DRAWITEM()
  ON_WM_GETMINMAXINFO()
  ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgPToolTileTester-Meldungshandler

BOOL CDlgPToolTileTester::OnInitDialog()
{
  __super::OnInitDialog();

  m_EditWidth.SetWindowText( _T( "32" ) );
  m_EditHeight.SetWindowText( _T( "32" ) );

  m_iWidth  = 32;
  m_iHeight = 32;

  m_pCurrentDocInfo = NULL;

  m_StaticTestBed.ModifyStyle( 0, SS_OWNERDRAW );

  return TRUE;

}



void CDlgPToolTileTester::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember )
{

  switch ( NotifyMessage )
  {
    case NF_DOC_ADDED:
      m_ComboDocs.SetItemData( m_ComboDocs.AddString( _T( "Doc" ) ), (DWORD_PTR)pFromMember );
      break;
    case NF_DOC_REMOVED:
      {
        size_t  iCount = m_ComboDocs.GetCount();

        for ( size_t i = 0; i < iCount; ++i )
        {
          if ( (DocumentInfo*)m_ComboDocs.GetItemDataPtr( i ) == (DocumentInfo*)pFromMember )
          {
            if ( m_pCurrentDocInfo == (DocumentInfo*)pFromMember )
            {
              m_pCurrentDocInfo = NULL;
              OnBnClickedButtonRefresh();
            }
            m_ComboDocs.DeleteString( i );
            break;
          }
        }
      }
      break;
  }

}



void CDlgPToolTileTester::ClearTileSet()
{

  SafeDelete( m_pImageTile );

}


  
void CDlgPToolTileTester::OnBnClickedButtonRefresh()
{
  
  ClearTileSet();
  if ( m_pCurrentDocInfo == NULL )
  {
    m_Page.Destroy();
    m_StaticTestBed.Invalidate();
    return;
  }

  RECT    rc;
  m_StaticTestBed.GetClientRect( &rc );


  m_Page.Create( m_StaticTestBed.GetSafeHwnd(), rc.right - rc.left, rc.bottom - rc.top, m_pCurrentDocInfo->GetActiveImage()->GetDepth() );
  m_Page.SetPalette( m_pCurrentDocInfo->GetPalette( m_pCurrentDocInfo->CurrentFrame() ) );

  m_pCurrentDocInfo->GetActiveImage()->PutImage( &m_Page, 0, 0 );

  CString   cstrDummy;

  m_EditWidth.GetWindowText( cstrDummy );
  m_iWidth = GR::Convert::ToI32( LPCTSTR( cstrDummy ) );
  m_EditHeight.GetWindowText( cstrDummy );
  m_iHeight = GR::Convert::ToI32( LPCTSTR( cstrDummy ) );

  if ( m_iWidth < 4 )
  {
    m_iWidth = 4;
  }
  if ( m_iHeight < 4 )
  {
    m_iHeight = 4;
  }

  m_iX = 0;
  m_iY = 0;

  m_StaticTestBed.Invalidate();

}



void CDlgPToolTileTester::OnCbnSelchangeComboDocuments()
{
  int   iItem = m_ComboDocs.GetCurSel();
  if ( iItem == CB_ERR )
  {
    return;
  }

  m_pCurrentDocInfo = (DocumentInfo*)m_ComboDocs.GetItemData( iItem );
}



void CDlgPToolTileTester::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  if ( nIDCtl == IDC_STATIC_TILETESTBED )
  {
    BitBlt( lpDrawItemStruct->hDC, 0, 0, m_Page.GetWidth(), m_Page.GetHeight(), m_Page.GetDC(), 0, 0, SRCCOPY );

    HBRUSH  hBrush = CreateSolidBrush( 0xffffff );

    RECT    rc;

    SetRect( &rc, m_iX * m_iWidth, m_iY * m_iHeight, m_iX * m_iWidth + m_iWidth, m_iY * m_iHeight + m_iHeight );
    ::FrameRect( lpDrawItemStruct->hDC, &rc, hBrush );

    DeleteObject( hBrush );

    return;
  }
  __super::OnDrawItem(nIDCtl, lpDrawItemStruct);
}



BOOL CDlgPToolTileTester::PreTranslateMessage(MSG* pMsg)
{
  if ( pMsg->hwnd == m_StaticTestBed.GetSafeHwnd() )
  {
    switch ( pMsg->message )
    {
      case WM_LBUTTONDOWN:
        m_iX = LOWORD( pMsg->lParam ) / m_iWidth;
        m_iY = HIWORD( pMsg->lParam ) / m_iHeight;
        m_StaticTestBed.Invalidate();
        m_StaticTestBed.SetFocus();
        break;
      case WM_KEYDOWN:
        if ( pMsg->wParam == VK_F2 )
        {
          SafeDelete( m_pImageTile );
          m_pImageTile = new GR::Graphic::Image( m_iWidth, m_iHeight, m_Page.GetDepth() );
          m_pImageTile->GetImage( &m_Page, m_iX * m_iWidth, m_iY * m_iHeight );
          m_StaticTestBed.Invalidate();
        }
        else if ( pMsg->wParam == VK_F3 )
        {
          if ( m_pImageTile )
          {
            m_pImageTile->PutImage( &m_Page, m_iX * m_iWidth, m_iY * m_iHeight );
          }
          m_StaticTestBed.Invalidate();
        }
        else if ( pMsg->wParam == 'H' )
        {
          if ( m_pImageTile )
          {
            HMirrorImage( m_pImageTile );
          }
        }
        else if ( pMsg->wParam == 'V' )
        {
          if ( m_pImageTile )
          {
            VMirrorImage( m_pImageTile );
          }
        }
        else if ( pMsg->wParam == 'R' )
        {
          if ( ( m_iWidth == m_iHeight )
          &&   ( m_pImageTile ) )
          {
            GR::Graphic::Image*   pImageNew = RotateImage( m_pImageTile );
            delete m_pImageTile;
            m_pImageTile = pImageNew;
          }
        }
        break;
    }
  }
  return __super::PreTranslateMessage(pMsg);
}



void CDlgPToolTileTester::OnOK()
{
}



void CDlgPToolTileTester::OnCancel()
{
}



void CDlgPToolTileTester::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
  lpMMI->ptMinTrackSize.x = 392;
  lpMMI->ptMinTrackSize.y = 276;
}



void CDlgPToolTileTester::OnSize(UINT nType, int cx, int cy)
{
  __super::OnSize(nType, cx, cy);

  if ( m_StaticTestBed.GetSafeHwnd() == NULL )
  {
    return;
  }

  RECT      rc;

  m_StaticTestBed.GetClientRect( &rc );
  m_StaticTestBed.ClientToScreen( &rc );
  ScreenToClient( &rc );

  if ( m_pCurrentDocInfo )
  {
    GR::Graphic::Image*   pImageOld = new GR::Graphic::Image( m_Page.GetWidth(), m_Page.GetHeight(), m_Page.GetDepth() );

    pImageOld->GetImage( &m_Page, 0, 0 );

    m_Page.Destroy();
    m_Page.Create( m_StaticTestBed.GetSafeHwnd(), cx, cy - rc.top, pImageOld->GetDepth() );
    pImageOld->PutImage( &m_Page, 0, 0 );
    delete pImageOld;
  }

  m_StaticTestBed.MoveWindow( 0, rc.top, cx, cy - rc.top );
}
