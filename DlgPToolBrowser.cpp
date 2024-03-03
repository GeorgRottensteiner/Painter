#include "stdafx.h"
#include "DlgPToolBrowser.h"
#include "DlgRename.h"

#include <Setup\Groups.h>

#include <String/Path.h>
#include <String/StringUtil.h>

#include "Import.h"
#include "Settings.h"
#include "Painter.h"

#include <Misc/Misc.h>

#include <MFC/ODMenu.h>
#include <debug/debugclient.h>

#include <IO/FileUtil.h>

#include <Grafik/ImageFormate/ImageFormatManager.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPainterApp  theApp;



CDlgPToolBrowser::CDlgPToolBrowser( CWnd* pParent )
	: CDialog( CDlgPToolBrowser::IDD, pParent ),
    m_DoNotReadImages( false ),
    m_hWorkerThread( NULL ),
    m_ThreadClose( false ),
    m_FolderChanged( true ),
    m_DeletedThumbs( true ),
    m_CanReadNow( true )
{
	//{{AFX_DATA_INIT(CDlgPToolBrowser)
	//}}AFX_DATA_INIT
}


void CDlgPToolBrowser::DoDataExchange( CDataExchange* pDX )
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDlgPToolBrowser)
  DDX_Control( pDX, IDC_TREE_FILES, m_TreeFiles );
  //}}AFX_DATA_MAP
  DDX_Control( pDX, IDC_ODLIST_THUMBS, m_ListThumbs );
}


BEGIN_MESSAGE_MAP( CDlgPToolBrowser, CDialog )
	//{{AFX_MSG_MAP(CDlgPToolBrowser)
	ON_WM_DRAWITEM()
	ON_WM_SIZE()
	ON_NOTIFY( TVN_ITEMEXPANDING, IDC_TREE_FILES, OnItemExpandingTreeFiles )
	ON_NOTIFY( TVN_SELCHANGED, IDC_TREE_FILES, OnSelChangedTreeFiles )
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
//  ON_WM_KEYDOWN()
END_MESSAGE_MAP()



void CDlgPToolBrowser::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember )
{

}



HTREEITEM CDlgPToolBrowser::InsertSubDir( HTREEITEM hItemParent, const GR::String& Directory )
{
  GR::String      trueDir = Directory;
  HTREEITEM       hItemResult = NULL;

  // hier Spezial-Folder abfangen
  if ( Directory == "Desktop" )
  {
    trueDir = Path::AddSeparator( CProgramGroups::GetShellFolder( "Desktop" ) );

    // Extra-Wurst Desktop!
    for ( int i = 0; i < 26; i++ )
    {
      GR::tChar    szDummy[5];

      szDummy[0] = 'A' + i;
      szDummy[1] = ':';
      szDummy[2] = '\\';
      szDummy[3] = 0;

      UINT    uType = GetDriveType( szDummy );

      if ( ( uType != DRIVE_UNKNOWN )
      &&   ( uType != DRIVE_NO_ROOT_DIR ) )
      {
        SHFILEINFO    shfi;
        SHGetFileInfo( szDummy, 0, &shfi, sizeof( SHFILEINFO ), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ICON );

        HTREEITEM   hItemDrive = m_TreeFiles.InsertItem( szDummy, shfi.iIcon, shfi.iIcon, hItemParent );

        m_TreeItemToFiles[hItemDrive] = GR::Convert::ToUTF8( szDummy );
      }
    }
  }

  std::list<GR::String>     directories;

  GR::IO::FileUtil::EnumDirsInDirectory( Path::Append( trueDir, "*.*" ), directories );

  std::map<GR::String,std::pair<GR::String,GR::String> >   tempMap;


  std::list<GR::String>::iterator   itD( directories.begin() );
  while ( itD != directories.end() )
  {
    tempMap[GR::Strings::ToUpper( *itD + "\\" )] = std::make_pair( *itD, Path::StripPath( *itD ) );

    ++itD;
  }

  std::map<GR::String, std::pair<GR::String, GR::String> >::iterator   it( tempMap.begin() );
  while ( it != tempMap.end() )
  {
    std::pair<GR::String, GR::String>&   Pair = it->second;

    SHFILEINFO    shfi;
    SHGetFileInfo( GR::Convert::ToUTF16( Pair.first ).c_str(), 0, &shfi, sizeof( SHFILEINFO ), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ICON );

    HTREEITEM   hItemDir = m_TreeFiles.InsertItem( GR::Convert::ToUTF16( Pair.second ).c_str(), shfi.iIcon, shfi.iIcon, hItemParent );
    m_TreeItemToFiles[hItemDir] = Pair.first;

    if ( GR::Strings::CompareUpperCase( Pair.second, Directory ) == 0 )
    {
      hItemResult = hItemDir;
    }

    ++it;
  }

  return hItemResult;
}



void CDlgPToolBrowser::FillTree()
{
  m_TreeFiles.DeleteAllItems();
  m_TreeItemToFiles.clear();

  SHFILEINFO    shfi;

  LPITEMIDLIST  ppidl = NULL;

  HRESULT       hRes = SHGetSpecialFolderLocation( GetSafeHwnd(), CSIDL_DESKTOP, &ppidl );
  if ( SUCCEEDED( hRes ) )
  {
    LPMALLOC pMalloc = NULL;
    HRESULT   hRes = SHGetMalloc( &pMalloc );
    if ( SUCCEEDED( hRes ) )
    {
      SHGetFileInfo( (const GR::tChar*)ppidl, 0, &shfi, sizeof(SHFILEINFO), 
                                                SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ICON);

      pMalloc->Free( ppidl );
      pMalloc->Release();
    }
  }

  HTREEITEM   hItemRoot = m_TreeFiles.InsertItem( _T( "Desktop" ), shfi.iIcon, shfi.iIcon );

  m_TreeItemToFiles[hItemRoot] = "Desktop";
}



void CDlgPToolBrowser::FindPathInTree( const GR::String& Path )
{
  HTREEITEM   hItemRoot = m_TreeFiles.GetRootItem();
  if ( hItemRoot == NULL )
  {
    return;
  }

  GR::String     path = Path;

  m_DoNotReadImages = true;

  do
  {
    size_t   sepPos = path.find( '|' );

    GR::String   temp;
    if ( sepPos == -1 )
    {
      temp = path;
      path.erase();
    }
    else
    {
      temp    = path.substr( 0, sepPos );
      path    = path.substr( sepPos + 1 );
    }

    bool      bContinue = false;
    CString   cstrGnu = m_TreeFiles.GetItemText( hItemRoot );
    GR::String    treePath = GR::Convert::ToUTF8( (LPCTSTR)cstrGnu );

    if ( GR::Strings::CompareUpperCase( treePath, temp ) == 0 )
    {
      m_TreeFiles.SelectItem( hItemRoot );
      bContinue = true;
    }
    else
    {
      HTREEITEM   hItemChild = m_TreeFiles.GetChildItem( hItemRoot );

      while ( hItemChild )
      {
        CString   cstrGnu = m_TreeFiles.GetItemText( hItemChild );
        treePath = GR::Convert::ToUTF8( (LPCTSTR)cstrGnu );

        if ( GR::Strings::CompareUpperCase( treePath, temp ) == 0 )
        {
          m_TreeFiles.SelectItem( hItemChild );
          bContinue = true;
          hItemRoot = hItemChild;
          break;
        }

        hItemChild = m_TreeFiles.GetNextSiblingItem( hItemChild );
      }
    }
    if ( !bContinue )
    {
      m_DoNotReadImages = false;
      return;
    }
  }
  while ( !path.empty() );

  m_DoNotReadImages = false;

  if ( m_TreeItemToFiles.find( hItemRoot ) != m_TreeItemToFiles.end() )
  {
    RefreshThumbs( m_TreeItemToFiles[hItemRoot].c_str() );
  }
}



BOOL CDlgPToolBrowser::OnInitDialog() 
{
	CDialog::OnInitDialog();

  EnableToolTips();

  DWORD   dwThreadID = 0;

  m_hWorkerThread = CreateThread( NULL, 0, BrowserThreadProc, this, 0, &dwThreadID );

  //CDialog::SetIcon( ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDI_ICON_TOOL_BROWSER ) ) );

  if ( !m_ThumbsToolTip.CreateEx( this, TTS_ALWAYSTIP, WS_EX_TOPMOST ) )
  {
    dh::Log( "toolcreate failed\n" );
  }

  SHFILEINFO shfi;

  //retrieve the index of the icon in the system image list
  m_ImageList = (HIMAGELIST)SHGetFileInfo( _T( "C:\\" ), 0, &shfi, sizeof(SHFILEINFO), 
                                            SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ICON);

  m_TreeFiles.SetImageList( CImageList::FromHandle( m_ImageList ), TVSIL_NORMAL );

  m_ListThumbs.SetItemSize( pSettings->GetSetting( "ThumbnailWidth", 64 ) + 12, 
                            pSettings->GetSetting( "ThumbnailHeight", 80 ) + 12 );
  m_ListThumbs.SetOwnerDraw();

  CRect rect( 0, 0, 0, 0 );

  m_wndSplitter.Create( WS_CHILD | WS_VISIBLE, rect, this, 999 );
  m_wndSplitter.SetPanes( &m_TreeFiles, &m_ListThumbs );
	

  GR::String     oldDir = pSettings->GetSettingString( "LastBrowseDir" );

  FillTree();

  HTREEITEM hItemRoot = m_TreeFiles.GetRootItem();
  if ( hItemRoot )
  {
    m_TreeFiles.SelectItem( hItemRoot );
  }

  if ( !oldDir.empty() )
  {
    // den Tree soweit aufklappen
    FindPathInTree( oldDir );
  }
  else
  {
    if ( hItemRoot )
    {
      m_TreeFiles.SelectItem( hItemRoot );
      m_TreeFiles.Expand( hItemRoot, TVE_EXPAND );
    }
  }

  if ( !m_ThumbsToolTip.AddTool( &m_ListThumbs ) )
  {
    dh::Log( "Addtool failed\n" );
  }
  m_ThumbsToolTip.Activate( TRUE );
  m_ThumbsToolTip.SetMaxTipWidth( 200 );
  
	return TRUE;
}



void CDlgPToolBrowser::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  if ( lpDrawItemStruct->hwndItem == m_ListThumbs.GetSafeHwnd() )
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
    
    GR::String    itemText = "NULL Item";

    if ( lpDrawItemStruct->itemID != -1 )
    {
      itemText = m_ListThumbs.GetText( lpDrawItemStruct->itemID );

      itemText = Path::FileName( itemText );

      if ( lpDrawItemStruct->itemData )
      {
        HDC hdcTemp = CreateCompatibleDC( NULL );

        HGDIOBJ oldObj = SelectObject( hdcTemp, ( (tThumbnail*)lpDrawItemStruct->itemData )->Bitmap );

        BITMAP    bm;
        ::GetObject( ( (tThumbnail*)lpDrawItemStruct->itemData )->Bitmap, sizeof( BITMAP ), &bm );

        int   iX = lpDrawItemStruct->rcItem.left + ( lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left - bm.bmWidth ) / 2,
              iY = lpDrawItemStruct->rcItem.top + ( lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top - bm.bmHeight - 12 ) / 2;

        BitBlt( lpDrawItemStruct->hDC,
                iX,
                iY,
                bm.bmWidth,
                bm.bmHeight,
                hdcTemp,
                0, 0,
                SRCCOPY );

        SelectObject( hdcTemp, oldObj );
        DeleteDC( hdcTemp );
      }

      DrawEdge( lpDrawItemStruct->hDC,
                &lpDrawItemStruct->rcItem,
                EDGE_RAISED,
                BF_RECT );
    }

    // Text
    RECT    rcItem;
    
    rcItem.left = lpDrawItemStruct->rcItem.left + 2;
    rcItem.top = lpDrawItemStruct->rcItem.bottom - 16;
    rcItem.right = lpDrawItemStruct->rcItem.right - 2;
    rcItem.bottom = lpDrawItemStruct->rcItem.bottom - 2;
      
    DrawText( lpDrawItemStruct->hDC, GR::Convert::ToUTF16( itemText ).c_str(), (int)itemText.length(), &rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
    return;
  }
	
	CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
}



void CDlgPToolBrowser::OnSize( UINT nType, int cx, int cy ) 
{
	CDialog::OnSize( nType, cx, cy );

  //if ( !Minimized() )
  {
    if ( m_wndSplitter.GetSafeHwnd() )
    {
      RECT    rc,
              rc2,
              rc3;

      m_wndSplitter.GetWindowRect( &rc );
      ScreenToClient( &rc );

      m_TreeFiles.GetWindowRect( &rc2 );
      m_TreeFiles.SetWindowPos( NULL, 0, 0, rc2.right - rc2.left, cy, SWP_NOMOVE | SWP_NOZORDER );

      GetClientRect( &rc3 );
      m_ListThumbs.SetWindowPos( NULL, 0, 0, 2 + rc3.right - rc3.left - ( rc.right - rc.left ) - ( rc2.right - rc2.left ), cy, SWP_NOMOVE | SWP_NOZORDER );
      m_wndSplitter.SetWindowPos( NULL, 0, 0, rc.right - rc.left, cy, SWP_NOMOVE | SWP_NOZORDER );

      m_wndSplitter.Resize();
    }
  }
}



void CDlgPToolBrowser::OnItemExpandingTreeFiles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

  if ( pNMTreeView->action == TVE_EXPAND )
  {
    if ( m_TreeFiles.ItemHasChildren( pNMTreeView->itemNew.hItem ) )
    {
      // das Item hat bereits Childs
    }
    else
    {
      tTreeItemToFiles::iterator   it( m_TreeItemToFiles.find( pNMTreeView->itemNew.hItem ) );
      InsertSubDir( pNMTreeView->itemNew.hItem, it->second );
    }
  }
  Default();
}



void CDlgPToolBrowser::OnSelChangedTreeFiles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

  tTreeItemToFiles::iterator   it( m_TreeItemToFiles.find( pNMTreeView->itemNew.hItem ) );
  if ( it == m_TreeItemToFiles.end() )
  {
    dh::Log( "Tree item without file entry!" );
    *pResult = 0;
    return;
  }

  if ( m_TreeFiles.ItemHasChildren( pNMTreeView->itemNew.hItem ) )
  {
    // das Item hat bereits Childs
  }
  else
  {
    InsertSubDir( pNMTreeView->itemNew.hItem, it->second );
  }

  HTREEITEM   hItemCurDir = pNMTreeView->itemNew.hItem;

  GR::String   strCurDir;
  while ( hItemCurDir )
  {
    CString         cstrGnu = m_TreeFiles.GetItemText( hItemCurDir );

    GR::String      tempDir = GR::Convert::ToUTF8( LPCTSTR( cstrGnu ) );

    if ( !tempDir.empty() )
    {
      if ( strCurDir.empty() )
      {
        strCurDir = tempDir;
      }
      else
      {
        strCurDir = tempDir + '|' + strCurDir;
      }
    }

    hItemCurDir = m_TreeFiles.GetParentItem( hItemCurDir );
  }

  pSettings->SetSetting( "LastBrowseDir", strCurDir );

  if ( !m_DoNotReadImages )
  {
    RefreshThumbs( it->second.c_str() );
  }
	
	*pResult = 0;
}



void CDlgPToolBrowser::RefreshThumbs( const GR::String& Directory )
{
  GR::String      trueDir;


  m_CanReadNow        = false;
  m_CurrentThumbDir   = Directory;
  m_FolderChanged     = true;

  for ( size_t i = 0; i < m_ListThumbs.GetCount(); i++ )
  {
    tThumbnail*   pThumb = (tThumbnail*)m_ListThumbs.GetItemData( i );

    if ( pThumb )
    {
      if ( pThumb->Bitmap )
      {
        DeleteObject( pThumb->Bitmap );
      }
      delete pThumb;
    }
  }

  m_ListThumbs.ResetContent();
  m_FilesToLoad.clear();

  trueDir = Directory;

  // hier Spezial-Folder abfangen
  if ( Directory == "Desktop" )
  {
    trueDir = Path::AddSeparator( CProgramGroups::GetShellFolder( "Desktop" ) );
  }

  GR::IO::FileUtil::EnumFilesInDirectory( Path::Append( trueDir, "*.*" ), m_FilesToLoad );
  m_CanReadNow = true;
}



void CDlgPToolBrowser::OnDestroy() 
{
  if ( m_hWorkerThread )
  {
    m_ThreadClose = true;
    while ( m_hWorkerThread )
    {
      Sleep( 50 );
    }
  }
	
  for ( size_t i = 0; i < m_ListThumbs.GetCount(); i++ )
  {
    tThumbnail*   pThumb = (tThumbnail*)m_ListThumbs.GetItemData( i );

    if ( pThumb )
    {
      if ( pThumb->Bitmap )
      {
        DeleteObject( pThumb->Bitmap );
      }
      delete pThumb;
    }
  }
  m_ListThumbs.ResetContent();

  CDialog::OnDestroy();
}



void CDlgPToolBrowser::ThumbPopup( int Index )
{
  CPoint          point;

  DWORD           dwResult;

  ODMenu         odPopup( ODMenu::ODMS_XP );

  odPopup.AppendMenu( MF_STRING, 1, _T( "Open" ) );
  odPopup.AppendMenu( MF_SEPARATOR );
  odPopup.AppendMenu( MF_STRING, 2, _T( "Rename" ) );
  odPopup.AppendMenu( MF_STRING, 3, _T( "Delete" ) );

  GetCursorPos( &point );
  dwResult = odPopup.TrackPopupMenu( TPM_RETURNCMD, point.x, point.y, this );

  GR::String     fileName = m_ListThumbs.GetText( Index );

  switch ( dwResult )
  {
    case 1:
      theApp.CheckAndOpen( fileName.c_str() );
      break;
    case 2:
      {
        GR::String   strPath      = Path::RemoveFileSpec( fileName );
        GR::String   strFileName  = Path::StripPath( fileName );

        CDlgRename    dlgRename( strFileName, "Rename File", this );

        if ( dlgRename.DoModal() == IDOK )
        {
          strFileName = dlgRename.m_Name;

          strFileName = Path::Append( strPath, strFileName );

          if ( !GR::IO::FileUtil::MoveFile( fileName, strFileName ) )
          {
            if ( GetLastError() == 0xb7 )
            {
              AfxMessageBox( _T( "Target file name already exists." ) );
            }
            else
            {
              AfxMessageBox( _T( "Failed to rename file." ) );
            }
            return;
          }
          DWORD_PTR     dwItemData = m_ListThumbs.GetItemData( Index );
          m_ListThumbs.DeleteString( Index );

          int   iItem = m_ListThumbs.AddString( strFileName.c_str() );
          m_ListThumbs.SetItemData( iItem, dwItemData );
          m_ListThumbs.Invalidate();
        }
      }
      break;
    case 3:
      QueryAndDeleteFile( fileName, Index );
      break;
  }
}



void CDlgPToolBrowser::QueryAndDeleteFile( const GR::String& Filename, int ItemIndex )
{
  if ( AfxMessageBox( GR::Convert::ToUTF16( CMisc::printf( "File '%s' will be deleted.\nAre you sure?", Filename.c_str() ) ).c_str(), MB_YESNO ) == IDYES )
  {
    tThumbnail*   pThumb = (tThumbnail*)m_ListThumbs.GetItemData( ItemIndex );
    if ( pThumb )
    {
      if ( pThumb->Bitmap )
      {
        DeleteObject( pThumb->Bitmap );
      }
      delete pThumb;
    }
    m_ListThumbs.DeleteString( ItemIndex );

    GR::IO::FileUtil::Delete( Filename );
  }
}



BOOL CDlgPToolBrowser::PreTranslateMessage(MSG* pMsg) 
{
  if ( pMsg->message == WM_KEYDOWN )
  {
    if ( pMsg->wParam == VK_F5 )
    {
      RefreshThumbs( m_CurrentThumbDir.c_str() );
    }
    else if ( pMsg->wParam == VK_DELETE )
    {
      int     itemIndex = m_ListThumbs.GetCurSel();
      if ( itemIndex != LB_ERR )
      {
        GR::String     fileName = m_ListThumbs.GetText( itemIndex );

        QueryAndDeleteFile( fileName, itemIndex );
      }
    }
  }
  if ( pMsg->hwnd == m_ListThumbs.GetSafeHwnd() )
  {
    m_ThumbsToolTip.RelayEvent( pMsg );
    m_ThumbsToolTip.Update();
    m_ThumbsToolTip.SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
    if ( pMsg->message == WM_LBUTTONDBLCLK )
    {
      int     iCurSel = m_ListThumbs.GetCurSel();
      if ( iCurSel != LB_ERR )
      {
        GR::String     fileName = m_ListThumbs.GetText( iCurSel );

        theApp.CheckAndOpen( fileName.c_str() );
      }
    }
    else if ( pMsg->message == WM_RBUTTONDOWN )
    {
      POINT   pt;
      
      pt.x = LOWORD( pMsg->lParam );
      pt.y = HIWORD( pMsg->lParam );

      int     iItem = m_ListThumbs.ItemFromPoint( pt );

      if ( iItem != LB_ERR )
      {
        ThumbPopup( iItem );
        return TRUE;
      }
    }
  }
	return CDialog::PreTranslateMessage( pMsg );
}



BOOL CDlgPToolBrowser::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
  NMHDR*  pHdr = (NMHDR*)lParam;
  if ( pHdr->code == TTN_NEEDTEXT )
  {
    NMTTDISPINFO*   pTTHdr = (NMTTDISPINFO*)pHdr;

    POINT   pt;

    ::GetCursorPos( &pt );
    m_ListThumbs.ScreenToClient( &pt );

    size_t   iIndex = m_ListThumbs.ItemFromPoint( pt );
    if ( iIndex != LB_ERR )
    {
      tThumbnail*   pThumb = (tThumbnail*)m_ListThumbs.GetItemData( iIndex );
      if ( pThumb )
      {
        static GR::tChar   szTemp[500];

        wsprintf( szTemp, _T( "%s\r\n%dx%dx%d" ), 
                  GR::Convert::ToUTF16( pThumb->FileName ).c_str(),
                  pThumb->Width,
                  pThumb->Height,
                  pThumb->Depth );
        pTTHdr->lpszText = szTemp;
        pTTHdr->hinst = NULL;
        return TRUE;
      }
    }
    return FALSE;
  }
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}



void CDlgPToolBrowser::OnOK()
{
}



void CDlgPToolBrowser::OnCancel()
{
}



DWORD WINAPI CDlgPToolBrowser::BrowserThreadProc( LPVOID lpParam )
{
  CDlgPToolBrowser*   pDlg = (CDlgPToolBrowser*)lpParam;

  return pDlg->ThreadProc();
}



DWORD CDlgPToolBrowser::ThreadProc()
{
  int   iThumbnailWidth = pSettings->GetSetting( "ThumbnailWidth", 64 ),
        iThumbnailHeight = pSettings->GetSetting( "ThumbnailHeight", 70 );

  while ( !m_ThreadClose )
  {
    if ( ( !m_CanReadNow )
    ||   ( m_FilesToLoad.empty() ) )
    {
      Sleep( 10 );
      continue;
    }

    m_FolderChanged = false;

    GR::String     nextFile = m_FilesToLoad.front();

    m_FilesToLoad.pop_front();

    // Grafik laden
    AbstractImageFileFormat*  pFormat = NULL;

    GR::Graphic::ImageData*     pData = ImageFormatManager::Instance().LoadData( nextFile.c_str(), &pFormat );

    HBITMAP   hbm = NULL;

    tThumbnail*   pThumb = NULL;

    if ( pData )
    {
      GR::Graphic::Image*   pImage = new GR::Graphic::Image( *pData );

      if ( pImage->GetDepth() )
      {
        pThumb = new tThumbnail();
        pThumb->Width    = pImage->GetWidth();
        pThumb->Height   = pImage->GetHeight();
        pThumb->Depth    = pImage->GetDepth();
        if ( ( pImage->GetWidth() > iThumbnailWidth )
        ||   ( pImage->GetHeight() > iThumbnailHeight ) )
        {
          int   iNewWidth   = pImage->GetWidth(),
                iNewHeight  = pImage->GetHeight();

          if ( iNewWidth / iNewHeight > iThumbnailWidth / iThumbnailHeight )
          {
            float   fFaktor = (float)iThumbnailWidth / (float)iNewWidth;

            iNewHeight = (int)( iNewHeight * fFaktor );
            iNewWidth = (int)( iNewWidth * fFaktor );
          }
          else
          {
            float   fFaktor = (float)iThumbnailHeight / (float)iNewHeight;

            iNewHeight = (int)( iNewHeight * fFaktor );
            iNewWidth = (int)( iNewWidth * fFaktor );

            if ( iNewWidth > iThumbnailWidth )
            {
              fFaktor = (float)iThumbnailWidth / (float)iNewWidth;

              iNewHeight = (int)( iNewHeight * fFaktor );
              iNewWidth = (int)( iNewWidth * fFaktor );
            }
          }

          GR::Graphic::Image*    pSmallerImage = ResizeImage( pImage, &pData->Palette(), iNewWidth, iNewHeight, 0 );

          delete pImage;
          pImage = pSmallerImage;
        }

        pThumb->Bitmap = CreateBitmapFromImage( pImage, &pData->Palette() );
      }
      delete pImage;
      delete pData;
    }

    if ( pThumb )
    {
      if ( pThumb->Bitmap )
      {
        pThumb->FileName = nextFile;

        if ( ( m_CanReadNow )
        &&   ( !m_FolderChanged ) )
        {
          size_t    iItem = m_ListThumbs.AddString( nextFile.c_str() );
          m_ListThumbs.SetItemData( iItem, (DWORD_PTR)pThumb );
          m_ListThumbs.InvalidateItem( iItem );
        }
        else
        {
          delete pThumb;
        }
      }
      else
      {
        delete pThumb;
      }
    }
  }

  CloseHandle( m_hWorkerThread );
  m_hWorkerThread = NULL;

  return 0;
}