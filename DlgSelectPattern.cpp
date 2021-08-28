#include "stdafx.h"
#include "Settings.h"
#include "DlgSelectPattern.h"
#include "Import.h"
#include "DlgFileOpenPreview.h"

#include <Grafik/ImageFormate/ImageFormatManager.h>

#include <Misc/Misc.h>
#include <String/Convert.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



class CPatternPack
{
  public:

    CPattern*   pPattern;

    HBITMAP     m_hbm;

    CPatternPack()
      : pPattern( NULL ),
        m_hbm( NULL )
    {}
};



CDlgSelectPattern::CDlgSelectPattern(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectPattern::IDD, pParent),
    m_pSelectedPattern( NULL )
{
	//{{AFX_DATA_INIT(CDlgSelectPattern)
	//}}AFX_DATA_INIT
}


void CDlgSelectPattern::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectPattern)
	DDX_Control(pDX, IDC_ODLIST_PATTERNS, m_DummyList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectPattern, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectPattern)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON_ADD_PATTERN, OnButtonAddPattern)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CDlgSelectPattern::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

  RECT    rc;
  
  m_DummyList.GetWindowRect( &rc );
  ScreenToClient( &rc );
  m_ODListPatterns.Create( rc, this, 1 );

  m_ODListPatterns.ShowWindow( SW_SHOW );
  m_ODListPatterns.SetItemSize( 64, 64 );
  m_ODListPatterns.SetOwnerDraw();
  m_DummyList.ShowWindow( SW_HIDE );


  std::list<CPattern*>::iterator   it( pSettings->m_listPatterns.begin() );

  while ( it != pSettings->m_listPatterns.end() )
  {
    CPattern *pPattern = *it;

    CPatternPack *pPP = new CPatternPack();

    pPP->pPattern = pPattern;

    HBITMAP   hbm = NULL;

    GR::Graphic::ImageData*   pData = ImageFormatManager::Instance().LoadData( pPattern->m_FileName.c_str() );
    if ( pData )
    {
      hbm = CreateBitmapFromImageData( pData );
      delete pData;

      pPP->m_hbm = hbm;
    }

    size_t iItem = m_ODListPatterns.AddString( pPattern->m_FileName.c_str() );
    m_ODListPatterns.SetItemData( iItem, (DWORD_PTR)pPP );

    it++;
  }
  return TRUE;
}



void CDlgSelectPattern::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
  if ( lpDrawItemStruct->hwndItem == m_ODListPatterns.GetSafeHwnd() )
  {
    if ( lpDrawItemStruct->itemState & ODS_SELECTED )
    {
      FillRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, GetSysColorBrush( COLOR_HIGHLIGHT ) );
      SetTextColor( lpDrawItemStruct->hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ) );
    }
    else
    {
      FillRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, GetSysColorBrush( COLOR_WINDOW ) );
      SetTextColor( lpDrawItemStruct->hDC, GetSysColor( COLOR_WINDOWTEXT ) );
    }

    SetBkMode( lpDrawItemStruct->hDC, TRANSPARENT );
    
    GR::tChar szBuffer[MAX_PATH];
    if ( lpDrawItemStruct->itemID != -1 )
    {
      m_ODListPatterns.GetText( lpDrawItemStruct->itemID, szBuffer );

      HDC hdcTemp = CreateCompatibleDC( lpDrawItemStruct->hDC );

      HGDIOBJ oldObj = SelectObject( hdcTemp, ( (CPatternPack*)lpDrawItemStruct->itemData )->m_hbm );

      BITMAP    bm;
      ::GetObject( ( (CPatternPack*)lpDrawItemStruct->itemData )->m_hbm, sizeof( BITMAP ), &bm );

      StretchBlt( lpDrawItemStruct->hDC,
                  lpDrawItemStruct->rcItem.left,
                  lpDrawItemStruct->rcItem.top,
                  lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left,
                  lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top,
                  hdcTemp,
                  0,
                  0,
                  bm.bmWidth,
                  bm.bmHeight,
                  SRCCOPY );

      SelectObject( hdcTemp, oldObj );
      DeleteDC( hdcTemp );
    }
    else
    {
      wsprintf( szBuffer, _T( "NULL Item" ) );
    }
    DrawText( lpDrawItemStruct->hDC, szBuffer, (int)_tcslen( szBuffer ), &lpDrawItemStruct->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE );

    if ( lpDrawItemStruct->itemState & ODS_FOCUS )
    {
      SetBkColor( lpDrawItemStruct->hDC, 0 );
      SetBkMode( lpDrawItemStruct->hDC, OPAQUE );
      DrawFocusRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem );
    }
    return;
  }
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}



void CDlgSelectPattern::OnCancel() 
{
	CDialog::OnCancel();
}



void CDlgSelectPattern::OnButtonAddPattern() 
{
  CPainterFileOpen*   pFO = OpenFileDialog( CSettings::FT_IMAGE );

  GR::tChar*  pBuffer = (GR::tChar*)GlobalAlloc( GPTR, 4000 );
  pFO->m_ofn.lpstrFile = pBuffer;
  pFO->m_ofn.nMaxFile = 1995;

  if ( pFO->DoModal() == IDOK )
  {
    POSITION pos = pFO->GetStartPosition();
    while ( pos != NULL )
    {
      GR::String    fileName;

      CString cs = pFO->GetNextPathName( pos );

      fileName = GR::Convert::ToUTF8( (LPCTSTR)cs );

      {
        CPattern *pPattern = new CPattern();
        pPattern->m_strDescription = "neu";
        pPattern->m_FileName = fileName;

        CPatternPack  *pPP = new CPatternPack();

        pPP->pPattern = pPattern;

        pSettings->m_listPatterns.push_back( pPattern );

        GR::Graphic::ImageData*   pData = ImageFormatManager::Instance().LoadData( pPattern->m_FileName.c_str() );

        HBITMAP   hbm = NULL;

        if ( pData )
        {
          hbm = CreateBitmapFromImageData( pData );

          pPP->m_hbm = hbm;
        }

        pPP->pPattern->m_pImagePack = new CPainterImagePackage( new GR::Graphic::Image( *pData ) );
        if ( pData->Palette().Data() )
        {
          pPP->pPattern->m_pImagePack->m_Palette = pData->Palette();
        }
                                                              
        delete pData;

        size_t iItem = m_ODListPatterns.AddString( pPattern->m_FileName.c_str() );
        m_ODListPatterns.SetItemData( iItem, (DWORD_PTR)pPP );
      }
    }
  }
  GlobalFree( pBuffer );

  delete pFO;
}




void CDlgSelectPattern::OnDestroy() 
{
  for ( size_t i = 0; i < m_ODListPatterns.GetCount(); i++ )
  {
    CPatternPack *pPP = (CPatternPack*)m_ODListPatterns.GetItemData( i );

    if ( pPP->m_hbm )
    {
      DeleteObject( pPP->m_hbm );
    }
    delete pPP;
  }

	CDialog::OnDestroy();
}



void CDlgSelectPattern::OnOK() 
{
  int   item = m_ODListPatterns.GetCurSel();
  if ( item != LB_ERR )
  {
    CPatternPack *pP = (CPatternPack*)m_ODListPatterns.GetItemData( item );

    m_pSelectedPattern = pP->pPattern;
  }
	
	CDialog::OnOK();
}



void CDlgSelectPattern::OnContextMenu( CWnd* pWnd, CPoint point ) 
{
	if ( pWnd->GetSafeHwnd() == m_ODListPatterns.GetSafeHwnd() )
  {
    CPoint    pt( point );
    m_ODListPatterns.ScreenToClient( &pt );

    size_t iItem = m_ODListPatterns.ItemFromPoint( pt );

    if ( iItem == LB_ERR )
    {
      return;
    }
    CPatternPack *pPP = (CPatternPack*)m_ODListPatterns.GetItemData( iItem );

    CMenu   menuPopup;

    menuPopup.CreatePopupMenu();
    menuPopup.AppendMenu( MF_STRING, 1, _T( "Eigenschaften..." ) );
    menuPopup.AppendMenu( MF_SEPARATOR );
    menuPopup.AppendMenu( MF_STRING, 2, _T( "Von Liste entfernen" ) );

    DWORD dwResult = menuPopup.TrackPopupMenu( TPM_RETURNCMD | TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
                              point.x,
                              point.y,
                              this );

    switch ( dwResult )
    {
      case 1:
        break;
      case 2:
        {
          if ( AfxGetMainWnd()->MessageBox( GR::Convert::ToUTF16( CMisc::printf( "Pattern \"%s\" wirklich aus der Liste entfernen?", pPP->pPattern->m_strDescription.c_str() ) ).c_str(),
                           _T( "Sind Sie sicher?" ), MB_YESNO ) != IDYES )
          {
            return;
          }

          if ( pSettings->RemovePattern( pPP->pPattern ) )
          {
            if ( pPP->m_hbm )
            {
              DeleteObject( pPP->m_hbm );
            }
            delete pPP;

            m_ODListPatterns.DeleteString( iItem );
          }
        }
        break;
    }
  }
	
}
