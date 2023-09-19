#include "stdafx.h"
#include "Painter.h"
#include "PainterLua.h"

#include "PhotoShopFilterHandler.h"

#include "NewDocumentDialog.h"
#include "PropPrompts.h"
#include "PropGrid.h"
#include "PropDocInfo.h"
#include "PropDocInfo2.h"
#include "PainterGFXTools.h"
#include "PainterView.h"
#include "PainterFontView.h"

#include "GRDialogBar.h"
#include "GRMenuBar.h"
#include "GRToolBar.h"
#include "GRDockingManager.h"
#include "GRODMenuBar.h"

#include "DlgFreeRotate.h"
#include "DlgReplaceColor.h"
#include "DlgPSFilter.h"
#include "DlgSelector.h"
#include "DlgCreateFontFromWindowsFont.h"
#include "DlgCustomizeToolBars.h"

#include <Grafik\ContextDescriptor.h>
#include <MFC\BasicToolDlg.h>
#include <String/Convert.h>

#include <IO/FileChunk.h>
#include <IO/FileUtil.h>

#include <Memory/MemoryStream.h>

#include <debug/debugclient.h>

#include "MainFrm.h"
#include "Settings.h"

#include "ScriptTool.h"

#include "PainterEnums.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "afxadv.h"

extern CPainterApp  theApp;



IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_COMMAND_EX(CG_ID_VIEW_TOOLOPTIONS, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_TOOLOPTIONS, OnUpdateControlBarMenu)
	ON_COMMAND_EX(CG_ID_VIEW_BRUSHDIALOG, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_BRUSHDIALOG, OnUpdateControlBarMenu)
	ON_COMMAND_EX(CG_ID_VIEW_LAYERDIALOG, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_LAYERDIALOG, OnUpdateControlBarMenu)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_BEARBEITEN_PASTEASNEWIMAGE, OnBearbeitenPasteasnewimage)
	ON_COMMAND(ID_BEARBEITEN_EINSTELLUNGEN, OnBearbeitenEinstellungen)
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(ID_MENU_UNDO, OnUpdateMenuUndo)
	ON_COMMAND(ID_MENU_UNDO, OnMenuUndo)
	ON_COMMAND(ID_BEARBEITEN_DOKUMENTEINSTELLUNGEN, OnBearbeitenDokumenteinstellungen)
	ON_UPDATE_COMMAND_UI(ID_BEARBEITEN_DOKUMENTEINSTELLUNGEN, OnUpdateBearbeitenDokumentEinstellungen)
	ON_COMMAND(CG_ID_VIEW_TOGGLETOOLWINDOWS, OnIdViewToggleToolWindows)
	ON_COMMAND(ID_MENU_TOGGLE_SNAP, OnMenuToggleSnap)
	ON_UPDATE_COMMAND_UI(ID_MENU_TOGGLE_SNAP, OnUpdateMenuToggleSnap)
	ON_COMMAND(ID_SPECIAL_SELECTION_VONMASKE, OnSpecialSelectionVonMaske)
	ON_COMMAND(ID_SPECIAL_SELECTION_INVERT, OnSpecialSelectionInvert)
	ON_COMMAND(ID_MENU_REDO, OnMenuRedo)
	ON_UPDATE_COMMAND_UI(ID_MENU_REDO, OnUpdateMenuRedo)
  ON_MESSAGE( WM_USER, OnUser )
	ON_COMMAND(ID_SPECIAL_REMOVESELECTION, OnSpecialRemoveSelection)
	ON_COMMAND(ID_MENU_ROTATE_FREE, OnMenuRotateFree)
	ON_COMMAND(ID_MENU_CREATE_FONT, OnMenuCreateFont)
	ON_COMMAND(ID_MENU_COLOR_INVERT, OnMenuColorInvert)
	ON_COMMAND(ID_SPECIAL_REPLACE, OnSpecialReplace)
	ON_COMMAND(ID_MENU_SCRIPT_TEST, OnMenuScriptTest)
	ON_COMMAND(ID_MENU_PS_FILTER, OnMenuPhotoshopFilter)
	ON_COMMAND(ID_SPECIAL_SELECTION_FROM_DIALOG, OnSpecialSelectionFromDialog)
	//}}AFX_MSG_MAP
  ON_WM_DESTROY()
  ON_WM_CLOSE()
  ON_COMMAND(ID_MENU_TOOLBARS, OnMenuToolbars)
  ON_COMMAND(ID_BEARBEITEN_ISOMETRISIEREN, OnBearbeitenIsometrisieren)
//  ON_WM_INITMENUPOPUP()
  ON_WM_NCLBUTTONDOWN()
//  ON_WM_NCRBUTTONDOWN()
//  ON_WM_NCRBUTTONUP()
  ON_WM_CONTEXTMENU()
//  ON_WM_NCLBUTTONDBLCLK()
//ON_WM_NCMBUTTONDOWN()
//ON_WM_NCRBUTTONDOWN()
ON_WM_ACTIVATEAPP()
ON_COMMAND(ID_SPECIAL_EXCHANGECOLOR, &CMainFrame::OnSpecialExchangecolor)
ON_COMMAND( ID_FILE_CLOSEALL, &CMainFrame::OnFileCloseAll )
ON_COMMAND( ID_SPECIAL_SAFETYSAVE, &CMainFrame::OnSpecialSafetySave )
ON_COMMAND( ID__TEST, &CMainFrame::OnRestoreSafetySave )
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
  ID_STATUSBAR_HOTKEYS,
  ID_STATUSBAR_INFO,
  ID_STATUSBAR_POS,
  ID_STATUSBAR_SIZE,
  ID_STATUSBAR_ITEM,
  
};



CMainFrame::CMainFrame() :
  m_SessionEnding( false )
{
}



CMainFrame::~CMainFrame()
{
}



void CMainFrame::DockControlBarLeftOf( CControlBar* Bar, CControlBar* LeftOf )
{
  CRect             rect;

  DWORD             dw;

  UINT              n;
  

  // get MFC to adjust the dimensions of all docked ToolBars
  // so that GetWindowRect will be accurate
  RecalcLayout( TRUE );
  
  LeftOf->GetWindowRect( &rect );
  rect.OffsetRect( 1, 0 );
  dw=LeftOf->GetBarStyle();
  n = 0;
  n = ( dw & CBRS_ALIGN_TOP ) ? AFX_IDW_DOCKBAR_TOP : n;
  n = ( dw & CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
  n = ( dw & CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
  n = ( dw & CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
  
  // When we take the default parameters on rect, DockControlBar will dock
  // each Toolbar on a seperate line. By calculating a rectangle, we
  // are simulating a Toolbar being dragged to that location and docked.
  DockControlBar( Bar, n, &rect );
}



void CMainFrame::OnClose()
{
  // always force a safety save
  OnQueryEndSession();
  OnEndSession( TRUE );

  pSettings->RemoveAllNotifyMembers();

  CMDIFrameWnd::OnClose();
}



int CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if ( CMDIFrameWnd::OnCreate( lpCreateStruct ) == -1 )
  {
		return -1;
  }

	if ( ( !m_wndStatusBar.Create( this ) ) 
  ||   ( !m_wndStatusBar.SetIndicators( indicators, sizeof( indicators ) / sizeof( UINT ) ) ) )
	{
		TRACE0( "Statusleiste konnte nicht erstellt werden\n" );
		return -1;      // Fehler bei Erstellung
	}

  pSettings->m_dwClipboardFormat = RegisterClipboardFormat( _T( "GRPainterImageAndMask" ) );
  if ( pSettings->m_dwClipboardFormat == 0 )
  {
    TRACE( "RegisterClipboardFormat failed (%d)\n", GetLastError() );
  }

  RECT    rc;

  SetRect( &rc, 0, 0, 120, 120 );
  m_ComboZoom.Create( CBS_DROPDOWNLIST | WS_CHILD, rc, this, 7 );
  m_ComboZoom.SetFont( CFont::FromHandle( (HFONT)GetStockObject( DEFAULT_GUI_FONT ) ) );
  m_ComboZoom.AddString( _T( "50%" ) );
  m_ComboZoom.AddString( _T( "100%" ) );
  m_ComboZoom.AddString( _T( "200%" ) );
  m_ComboZoom.SetCurSel( 1 );

  m_dlgFarben.Create( IDD_PTOOL_FARBEN, this );
  m_dlgBrush.Create( IDD_PTOOL_BRUSH, this );
  m_dlgLayer.Create( IDD_PTOOL_LAYER, this );
  m_dlgBrowser.Create( IDD_PTOOL_BROWSER, this );
  m_dlgFunctionOptions.Create( IDD_PTOOL_FUNCTIONOPTIONS, this );
  m_dlgTileTester.Create( IDD_PTOOL_TILETESTER, this );
  m_dlgAnimation.Create( IDD_PTOOL_ANIMATIONFRAME, this );

  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_NONE,   ID_PAINT_ARROW );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_ZOOM,   ID_PAINT_ZOOM );
  AddFunction( _T( "Tools" ), _T( "" ),     0, 0, CBCT_SEPARATOR );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_UNDO,   ID_MENU_UNDO );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_REDO,   ID_MENU_REDO );
  AddFunction( _T( "Tools" ), _T( "" ),     0, 0, CBCT_SEPARATOR );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_FREEHAND_PIXEL,   ID_PAINT_FREEHAND1 );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_FREEHAND_LINE,    ID_PAINT_FREEHAND2 );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_LINE,             ID_PAINT_LINE );
  AddFunction( _T( "Tools" ), _T( "" ),     0, 0, CBCT_SEPARATOR );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_RECT,             ID_PAINT_RECTANGLE );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_BOX,              ID_PAINT_BOX );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_ELLIPSE,          ID_PAINT_CIRCLE );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_FILLED_ELLIPSE,   ID_PAINT_FILLED_CIRCLE );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_FILL,             ID_PAINT_FILL );
  AddFunction( _T( "Tools" ), _T( "" ),     0, 0, CBCT_SEPARATOR );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_TEXT,             ID_PAINT_TEXT );
  AddFunction( _T( "Tools" ), _T( "" ),     0, 0, CBCT_SEPARATOR );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_SELECT,           ID_PAINT_SELECTION );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_SELECT_COLOR,     ID_PAINT_SELECTION_COLOR );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_CLEAR,            ID_EDIT_CLEAR_ALL );
  AddFunction( _T( "Tools" ), _T( "" ),     0, 0, CBCT_SEPARATOR );
  AddFunction( _T( "Tools" ), _T( "" ),     IDI_ICON_TOOL_SNAP,             ID_MENU_TOGGLE_SNAP, CBCT_CHECKBUTTON );

  AddFunction( _T( "Main" ), _T( "" ),      IDI_ICON_MAIN_NEW,    ID_FILE_NEW );
  AddFunction( _T( "Main" ), _T( "" ),      IDI_ICON_MAIN_OPEN,   ID_FILE_OPEN );
  AddFunction( _T( "Main" ), _T( "" ),      IDI_ICON_MAIN_SAVE,   ID_FILE_SAVE );
  AddFunction( _T( "Main" ), _T( "" ),      0, 0, CBCT_SEPARATOR );
  AddFunction( _T( "Main" ), _T( "" ),      IDI_ICON_MAIN_PROPS,  ID_BEARBEITEN_EINSTELLUNGEN );
  AddFunction( _T( "Main" ), _T( "" ),      IDI_ICON_MAIN_DOC_PROPS,  ID_BEARBEITEN_DOKUMENTEINSTELLUNGEN );
  AddFunction( _T( "Main" ), _T( "" ),      0, 0, CBCT_SEPARATOR );
  AddFunction( _T( "Main" ), _T( "" ),      IDI_ICON_MAIN_ZOOMIN, ID_PAINT_ZOOMIN );
  AddFunction( _T( "Main" ), _T( "" ),      IDI_ICON_MAIN_ZOOMOUT, ID_PAINT_ZOOMOUT );

  AddFunction( _T( "Animation" ), _T( "" ),     IDI_ICON_ANIM_PLAY,   ID_ANIM_PLAY );
  AddFunction( _T( "Animation" ), _T( "" ),     IDI_ICON_ANIM_STOP,   ID_ANIM_STOP );
  AddFunction( _T( "Animation" ), _T( "" ),     0, 0, CBCT_SEPARATOR );
  AddFunction( _T( "Animation" ), _T( "" ),     IDI_ICON_ANIM_HOME,   ID_ANIM_HOME );
  AddFunction( _T( "Animation" ), _T( "" ),     IDI_ICON_ANIM_PREV,   ID_ANIM_STEP_BACK );
  AddFunction( _T( "Animation" ), _T( "" ),     IDI_ICON_ANIM_NEXT,   ID_ANIM_STEP_FORWARD );
  AddFunction( _T( "Animation" ), _T( "" ),     IDI_ICON_ANIM_END,    ID_ANIM_END );
  AddFunction( _T( "Animation" ), _T( "" ),     IDI_ICON_ANIM_INSERT, ID_ANIM_FRAME_INSERT );
  AddFunction( _T( "Animation" ), _T( "" ),     IDI_ICON_ANIM_INSERT_BEHIND,   ID_ANIM_FRAME_INSERTBEHIND );
  AddFunction( _T( "Animation" ), _T( "" ),     IDI_ICON_ANIM_DELETE, ID_ANIM_FRAME_DELETE );

  /*
  AddFunction( "Zoom", "Zoom:",     0, 0, CPainterControlBar::CT_TEXT );
  AddFunction( "Zoom", "",     0, (DWORD)m_ComboZoom.GetSafeHwnd(), CPainterControlBar::CT_HWND );
  */

  AddFunction( _T( "Farben" ),    _T( "" ),     0, (DWORD_PTR)m_dlgFarben.GetSafeHwnd(), CBCT_DIALOG );

  AddFunction( _T( "Brush" ),     _T( "" ),     0, (DWORD_PTR)m_dlgBrush.GetSafeHwnd(), CBCT_DIALOG );

  AddFunction( _T( "Layer" ),     _T( "" ),     0, (DWORD_PTR)m_dlgLayer.GetSafeHwnd(), CBCT_DIALOG );

  AddFunction( _T( "Browser" ),     _T( "" ),   0, (DWORD_PTR)m_dlgBrowser.GetSafeHwnd(), CBCT_DIALOG );

  AddFunction( _T( "Function Options" ),  _T( "" ),     0, (DWORD_PTR)m_dlgFunctionOptions.GetSafeHwnd(), CBCT_DIALOG );

  AddFunction( _T( "Tile Testbed" ), _T( "" ),  0, (DWORD_PTR)m_dlgTileTester.GetSafeHwnd(), CBCT_DIALOG );

  AddFunction( _T( "Animator" ), _T( "" ),  0, (DWORD_PTR)m_dlgAnimation.GetSafeHwnd(), CBCT_DIALOG );

  AddFunction( _T( "Main Menu" ), _T( "" ),     0, 0, CBCT_MENUBAR );

  rc.left = 200;
  rc.top = 0;
  rc.right = 400;
  rc.bottom = 20;

  int     iIndex = 32000;
  std::list<tPainterFunction>::iterator   it( m_listFunctions.begin() );
  while ( it != m_listFunctions.end() )
  {
    tPainterFunction&   Func = *it;

    tMapControlBars::iterator   itCB( m_mapControlBars.find( Func.m_strGroupName ) );
    if ( itCB == m_mapControlBars.end() )
    {
      // ein neuer Control-Bar muß her
      GRControlBar*   pBar = NULL;
      
      if ( Func.m_dwControlType == CBCT_DIALOG )
      {
        GRDialogBar* pDBar = new GRDialogBar();

        DWORD   dwSizingStyle = GRControlBar::GRCBS_FIXED_SIZE;

        CWnd*   pWnd = CWnd::FromHandle( (HWND)Func.m_dwCommandID );
        if ( pWnd->GetStyle() & WS_THICKFRAME )
        {
          dwSizingStyle = GRControlBar::GRCBS_FREE_SIZE;
          pWnd->ModifyStyle( WS_THICKFRAME | WS_BORDER, WS_CHILD, SWP_DRAWFRAME );
        }
        pDBar->Create( this, (HWND)Func.m_dwCommandID, dwSizingStyle, iIndex, Func.m_strGroupName.c_str() );
        pBar = pDBar;
      }
      else if ( Func.m_dwControlType == CBCT_MENUBAR )
      {
        //GRMenuBar* pMBar = new GRMenuBar();
        GRODMenuBar* pMBar = new GRODMenuBar();

        DWORD   dwSizingStyle = GRControlBar::GRCBS_FIXED_SIZE;

        pMBar->Create( this, GRMenuBar::GRMBS_OVERRIDE_MDI, iIndex, Func.m_strGroupName.c_str() );
        pBar = pMBar;

        pMBar->LoadMenu( IDR_MAINFRAME );
      }
      else
      {
        pBar = new GRToolBar();

        pBar->Create( Func.m_strGroupName.c_str(), GRControlBar::GRCBS_CONTENT_SIZE, this, iIndex );
      }
      m_mapControlBars[Func.m_strGroupName] = pBar;
      ++iIndex;
    }

    GRControlBar*   pBar = m_mapControlBars[Func.m_strGroupName];

    switch ( Func.m_dwControlType )
    {
      case CBCT_SEPARATOR:
        ((GRToolBar*)pBar )->AddSeparator();
        break;
      case CBCT_BUTTON:
        ((GRToolBar*)pBar )->AddButton( Func.m_dwCommandID, 
                                          (HICON)LoadImage( AfxGetInstanceHandle(),
                                          MAKEINTRESOURCE( Func.m_dwIconID ),
                                          IMAGE_ICON,
                                          0, 0,
                                          LR_DEFAULTCOLOR ) );
        break;
      case CBCT_CHECKBUTTON:
        ((GRToolBar*)pBar )->AddCheckButton( Func.m_dwCommandID, 
                                          (HICON)LoadImage( AfxGetInstanceHandle(),
                                          MAKEINTRESOURCE( Func.m_dwIconID ),
                                          IMAGE_ICON,
                                          0, 0,
                                          LR_DEFAULTCOLOR ) );
        break;
      case CBCT_RADIOBUTTON:
        ((GRToolBar*)pBar )->AddRadioButton( Func.m_dwCommandID, 
                                          (HICON)LoadImage( AfxGetInstanceHandle(),
                                          MAKEINTRESOURCE( Func.m_dwIconID ),
                                          IMAGE_ICON,
                                          0, 0,
                                          LR_DEFAULTCOLOR ) );
        break;
    }

    ++it;
  }

  m_wndStatusBar.SetPaneInfo( 0, ID_SEPARATOR,          SBPS_STRETCH, 0 );
  m_wndStatusBar.SetPaneInfo( 1, ID_STATUSBAR_HOTKEYS,  SBPS_NORMAL, 50 );
  m_wndStatusBar.SetPaneInfo( 2, ID_STATUSBAR_INFO,     SBPS_NORMAL, 180 );
  m_wndStatusBar.SetPaneInfo( 3, ID_STATUSBAR_POS,      SBPS_NORMAL, 50 );
  m_wndStatusBar.SetPaneInfo( 4, ID_STATUSBAR_SIZE,     SBPS_NORMAL, 180 );
  m_wndStatusBar.SetPaneInfo( 5, ID_STATUSBAR_ITEM,     SBPS_NORMAL, 100 );

  GRDockingManager::EnableDocking( CBRS_ALIGN_ANY );

	EnableDocking( CBRS_ALIGN_ANY );

  {
    tMapControlBars::iterator   itCB( m_mapControlBars.begin() );
    while ( itCB != m_mapControlBars.end() )
    {
      GRControlBar*   pBar = itCB->second;

      //pBar->Float( 20, 20 );

      //FloatControlBar( pBar, CPoint( -30000, -30000 ) );

      ++itCB;
    }
  }

  //FloatControlBar( &m_wndToolBar, CPoint( 200, 20 ) );
  //FloatControlBar( &m_wndAnimationBar, CPoint( 200, 20 ) );
  //DockControlBar( &m_wndToolBar );
  //DockControlBarLeftOf( &m_wndAnimationBar, &m_wndToolBar );

  // Bar-States wiederherstellen
  LoadBarSetup( "GRPainterBarStates" );


  ModifyStyleEx( 0, WS_EX_ACCEPTFILES );

  pSettings->m_hwndMainFrame = GetSafeHwnd();

  pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR,   pSettings->GetSetting( "ForeColor", 0xffffffff ) );
  pSettings->SetColor( CSettings::ColorCategory::WORKCOLOR_2, pSettings->GetSetting( "BackColor", 0 ) );

  {
    AddNotifyMember( &m_dlgFarben );
    AddNotifyMember( &m_dlgBrush );
    AddNotifyMember( &m_dlgLayer );
    AddNotifyMember( &m_dlgBrowser );
    AddNotifyMember( &m_dlgFunctionOptions );
    AddNotifyMember( &m_dlgTileTester );
    AddNotifyMember( &m_dlgAnimation );

    AddNotifyMember( pSettings );

    Notify( NF_ACTIVE_VIEW_CHANGED );
    Notify( NF_ACTIVE_COLOR_CHANGED );
  }

  // ScriptTools
  /*
  Luna<CScriptTool>::Register( pSettings->m_LuaInstance );

  lua_dostring( pSettings->m_LuaInstance, "Painter = CPainter();" );
  */
	return 0;
}



void CMainFrame::AddFunction( const GR::tString& strGroup, 
                              const GR::tString& strName, 
                              DWORD dwIconID, 
                              DWORD_PTR dwCommandID,
                              eControlBarControlType eType )
{
  m_listFunctions.push_back( tPainterFunction() );
  tPainterFunction&   Function = m_listFunctions.back();

  Function.m_strGroupName     = strGroup;
  
  Function.m_strFunctionDesc  = strName;
  Function.m_dwIconID         = dwIconID;
  Function.m_dwCommandID      = dwCommandID;
  Function.m_dwControlType    = eType;

  GR::tChar    szTemp[300];

  LoadString( AfxGetInstanceHandle(), dwCommandID, szTemp, 260 );
  Function.m_strFunctionName  = szTemp;

}



/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnose

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG



void CMainFrame::OnBearbeitenPasteasnewimage() 
{
  CPainterDoc       *pDoc;

  CWnd              *pWnd;


  pDoc = (CPainterDoc*)theApp.pDocTemplate->CreateNewDocument();

  pWnd = theApp.pDocTemplate->CreateNewFrame( pDoc, theApp.pMainFrame );

  if ( !pDoc->OnCreateDocumentFromClipboard() )
  {
    theApp.pDocTemplate->RemoveDocument( pDoc );
    pWnd->DestroyWindow();
    return;
  }
  pDoc->diInfo.SetModify( TRUE );
  theApp.pDocTemplate->InitialUpdateFrame( (CFrameWnd*)pWnd, pDoc, TRUE );
}



void CMainFrame::OnBearbeitenEinstellungen() 
{
  CPropertySheet      dlgEinstellungen;


  CPropPrompts *pPropPrompts = new CPropPrompts();
  dlgEinstellungen.AddPage( pPropPrompts );

  CPropGrid *pPropGrid = new CPropGrid();
  dlgEinstellungen.AddPage( pPropGrid );

  pPropGrid->m_iGridWidth = pSettings->GetSetting( "SnapWidth" );
  pPropGrid->m_iGridHeight = pSettings->GetSetting( "SnapHeight" );
  pPropGrid->m_iGridXOffset = pSettings->GetSetting( "SnapXOffset" );
  pPropGrid->m_iGridYOffset = pSettings->GetSetting( "SnapYOffset" );
  pPropGrid->m_bEnableSnap = pSettings->GetSetting( "SnapEnabled", 0 );

  dlgEinstellungen.SetTitle( _T( "Einstellungen" ) );

  if ( dlgEinstellungen.DoModal() == IDOK )
  {
    pSettings->SetSetting( "SnapWidth", pPropGrid->m_iGridWidth );
    pSettings->SetSetting( "SnapHeight", pPropGrid->m_iGridHeight );
    pSettings->SetSetting( "SnapXOffset", pPropGrid->m_iGridXOffset );
    pSettings->SetSetting( "SnapYOffset", pPropGrid->m_iGridYOffset );
    pSettings->SetSetting( "SnapEnabled", pPropGrid->m_bEnableSnap );
  }

  delete pPropPrompts;
  delete pPropGrid;
}



void CMainFrame::OnDropFiles( HDROP hDropInfo )
{
  DWORD         dwCount,
                dwI;

  GR::WChar     szTemp[MAX_PATH];


  dwCount = DragQueryFileW( hDropInfo, 0xFFFFFFFF, NULL, 0 );
  for ( dwI = 0; dwI < dwCount; dwI++ )
  {
    DragQueryFileW( hDropInfo, dwI, szTemp, MAX_PATH );
    theApp.CheckAndOpen( GR::Convert::ToUTF8( szTemp ).c_str() );
  }
  DragFinish( hDropInfo );
}



void CMainFrame::OnUpdateMenuUndo( CCmdUI* pCmdUI ) 
{
  DocumentInfo*      pDocInfo;

  pDocInfo = theApp.GetActiveDocumentInfo();
  if ( pDocInfo == NULL )
  {
    pCmdUI->Enable( FALSE );
    return;
  }
  pCmdUI->Enable( pDocInfo->m_UndoManager.IsUndoPossible() );
}



void CMainFrame::OnMenuUndo() 
{
  DocumentInfo*      pDocInfo;

  
  pDocInfo = theApp.GetActiveDocumentInfo();

  if ( pDocInfo )
  {
    pDocInfo->Undo();
  }
}



void CMainFrame::OnMenuRedo() 
{
  DocumentInfo*      pDocInfo;

  pDocInfo = theApp.GetActiveDocumentInfo();
  if ( pDocInfo )
  {
    pDocInfo->m_UndoManager.Redo();
  }
}



void CMainFrame::OnUpdateMenuRedo( CCmdUI* pCmdUI ) 
{
  DocumentInfo*      pDocInfo;

  
  pDocInfo = theApp.GetActiveDocumentInfo();
  if ( pDocInfo == NULL )
  {
    pCmdUI->Enable( FALSE );
    return;
  }
  pCmdUI->Enable( pDocInfo->m_UndoManager.IsRedoPossible() );
}



void CMainFrame::OnBearbeitenDokumenteinstellungen() 
{
  DocumentInfo*      pDocInfo;

  
  pDocInfo = pSettings->m_pActiveDocInfo;
  if ( pDocInfo == NULL )
  {
    return;
  }

  CPropertySheet      *pSheet;

  pSheet = new CPropertySheet();

  CPropDocInfo *pPropDocInfo = new CPropDocInfo();
  pPropDocInfo->m_pDocInfo = pDocInfo;

  ViewInfo *pVI = pSettings->m_pActiveViewInfo;
  if ( pVI == NULL )
  {
    return;
  }
  CPropDocInfo2 *pPropDocInfo2 = new CPropDocInfo2();

  pSheet->AddPage( pPropDocInfo );
  pSheet->AddPage( pPropDocInfo2 );

  CSize csize = pVI->m_pView->GetTotalSize();
  pPropDocInfo2->m_dwScrollWidth = csize.cx;
  pPropDocInfo2->m_dwScrollHeight = csize.cy;

  pSheet->SetTitle( _T( "Dokument-Einstellungen" ) );

  pSheet->DoModal();

  delete pPropDocInfo2;
  delete pPropDocInfo;
  delete pSheet;
}



void CMainFrame::OnUpdateBearbeitenDokumentEinstellungen( CCmdUI* pCmdUI ) 
{
  if ( theApp.GetActiveDocumentInfo() != NULL )
  {
    pCmdUI->Enable();
  }
  else
  {
    pCmdUI->Enable( FALSE );
  }
}



void CMainFrame::OnIdViewToggleToolWindows() 
{
  if ( pSettings->GetSetting( "ShowAllToolWindows", TRUE ) )
  {
    // alle ToolWindows aus
    pSettings->SetSetting( "ShowAllToolWindows", FALSE );
  }
  else
  {
    // alle ToolWindows an
    pSettings->SetSetting( "ShowAllToolWindows", TRUE );
    if ( pSettings->GetSetting( "ShowPaletteToolWindow", TRUE ) )
    {
      //ShowControlBar( &m_wndPaletteTool, TRUE, TRUE );
    }
    if ( pSettings->GetSetting( "ShowLayerToolWindow", TRUE ) )
    {
      //ShowControlBar( &m_wndLayerDialog, TRUE, TRUE );
    }
    if ( pSettings->GetSetting( "ShowBrushToolWindow", TRUE ) )
    {
      //ShowControlBar( &m_wndBrushDialog, TRUE, TRUE );
    }
    if ( pSettings->GetSetting( "ShowToolOptionsWindow", TRUE ) )
    {
      //ShowControlBar( &m_wndToolOptions, TRUE, TRUE );
    }
  }
}



void CMainFrame::OnMenuToggleSnap() 
{
  pSettings->SetSetting( "SnapEnabled", !pSettings->GetSetting( "SnapEnabled" ) );
}



void CMainFrame::OnUpdateMenuToggleSnap(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck( pSettings->GetSetting( "SnapEnabled" ) );
}


void CMainFrame::OnSpecialSelectionVonMaske() 
{

	ViewInfo *pViewInfo = theApp.GetActiveViewInfo();

  if ( !pViewInfo )
  {
    return;
  }

  CLayer*   pActiveLayer = pViewInfo->m_pDocInfo->ActiveLayer();
  if ( pActiveLayer == NULL )
  {
    return;
  }

  if ( !pActiveLayer->m_HasMask )
  {
    // keine Maske da!
    return;
  }
  pViewInfo->m_pDocInfo->EmptySelection();

  pViewInfo->m_pDocInfo->SetSelection( new GR::Graphic::Image( pActiveLayer->GetMask() ) );

  pViewInfo->m_pDocInfo->UpdateMarchingAnts();

  pViewInfo->m_pDocInfo->ShowAllViews();
	
}

void CMainFrame::OnSpecialSelectionInvert() 
{

	ViewInfo *pViewInfo = theApp.GetActiveViewInfo();

  if ( !pViewInfo )
  {
    return;
  }

  BYTE    *pData = (BYTE*)pViewInfo->m_pDocInfo->m_pImageSelection->GetData();

  for ( int j = 0; j < pViewInfo->m_pDocInfo->m_pImageSelection->GetHeight(); j++ )
  {
    for ( int i = 0; i < pViewInfo->m_pDocInfo->m_pImageSelection->GetWidth(); i++ )
    {
      *pData = 255 - *pData;
      pData++;
    }
  }
  
  pViewInfo->m_pDocInfo->UpdateMarchingAnts();
  pViewInfo->m_pDocInfo->ShowAllViews();
	
}



/*-OnNotify-------------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CMainFrame::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember )
{

  switch ( NotifyMessage )
  {
    case NF_ACTIVE_VIEW_CHANGED:
      // Toolbars anpassen
      if ( pSettings->m_pActiveDocInfo )
      {
        if ( pSettings->m_pActiveDocInfo->m_DocType == DT_IMAGE )
        {
          //m_wndAnimationBar.ShowWindow( SW_SHOW );
          //m_wndPaintBar.ShowWindow( SW_SHOW );
          //DockControlBarLeftOf( &m_wndAnimationBar, &m_wndToolBar );
        }
        else if ( pSettings->m_pActiveDocInfo->m_DocType == DT_FONT )
        {
          //m_wndAnimationBar.ShowWindow( SW_HIDE );
          //m_wndPaintBar.ShowWindow( SW_SHOW );
        }
        
      }
      break;
  }

}



LRESULT CMainFrame::OnUser( WPARAM wParam, LPARAM lParam )
{
  CScrollView*    pView;
  CDocument*      pDoc;


  if ( MDIGetActive() == NULL )
  {
    return NULL;
  }
  pView = (CScrollView*)( MDIGetActive()->GetActiveView() );
  pDoc = MDIGetActive()->GetActiveDocument();
  if ( pDoc != NULL )
  {
    if ( pDoc->GetDocTemplate() == theApp.pDocTemplate )
    {
      return (LRESULT)&( (CPainterView*)pView )->m_viewInfo;
    }
    else if ( pDoc->GetDocTemplate() == theApp.pFontTemplate )
    {
      return (LRESULT)&( (CPainterFontView*)pView )->m_viewInfo;
    }
  }

  return 0;
}




void CMainFrame::OnSpecialRemoveSelection() 
{

  DocumentInfo *pDocInfo = pSettings->m_pActiveDocInfo;

  if ( pDocInfo )
  {
    pDocInfo->EmptySelection();
    pDocInfo->RedrawAllViews();
  }
	
}



void CMainFrame::OnMenuRotateFree() 
{
  if ( pSettings->m_pActiveDocInfo == NULL )
  {
    return;
  }

  CDlgFreeRotate    dlgRotate;

  if ( dlgRotate.DoModal() == IDOK )
  {
    pSettings->m_pActiveDocInfo->AddUndoSizeChange();
    
    if ( !dlgRotate.m_RotateCW )
    {
      dlgRotate.m_Angle = -dlgRotate.m_Angle;
    }
    if ( dlgRotate.m_AllowResize )
    {
      int     duCol = (int)( sin(-dlgRotate.m_Angle * 3.1415926f / 180.0f ) * 65536.0 ),
              dvCol = (int)( cos(-dlgRotate.m_Angle * 3.1415926f / 180.0f ) * 65536.0 ),
              duRow = dvCol,
              dvRow = -duCol;

      float   fSrcRotCX = pSettings->m_pActiveDocInfo->Width() / 2.0f,
              fSrcRotCY = pSettings->m_pActiveDocInfo->Height() / 2.0f;

      #define MAX(a,b)    (((a) > (b)) ? (a) : (b))
      double cx, cy, sx, sy;
      int dstwidthhalf, dstheighthalf;

      double sanglezoom, canglezoom;

      // Determine destination width and height by rotating a centered source box 
      sanglezoom = sin( dlgRotate.m_Angle * ( 3.1415926f / 180.0f ) );
      canglezoom = cos( dlgRotate.m_Angle * ( 3.1415926f / 180.0f ) );
      cx = canglezoom * ( pSettings->m_pActiveDocInfo->Width() / 2 );
      cy = canglezoom * ( pSettings->m_pActiveDocInfo->Height() / 2 );
      sx = sanglezoom * ( pSettings->m_pActiveDocInfo->Width() / 2 );
      sy = sanglezoom * ( pSettings->m_pActiveDocInfo->Height() / 2 );

      dstwidthhalf = MAX((int)
		          ceil(MAX(MAX(MAX(fabs(cx + sy), fabs(cx - sy)), fabs(-cx + sy)), fabs(-cx - sy))), 1);
      dstheighthalf = MAX((int)
		          ceil(MAX(MAX(MAX(fabs(sx + cy), fabs(sx - cy)), fabs(-sx + cy)), fabs(-sx - cy))), 1);

      int   iNewWidth = dstwidthhalf * 2,
            iNewHeight = dstheighthalf * 2;

      if ( iNewWidth < (int)pSettings->m_pActiveDocInfo->Width() )
      {
        iNewWidth = pSettings->m_pActiveDocInfo->Width();
      }
      if ( iNewHeight < (int)pSettings->m_pActiveDocInfo->Height() )
      {
        iNewHeight = pSettings->m_pActiveDocInfo->Height();
      }

      pSettings->m_pActiveDocInfo->SetSize( iNewWidth, iNewHeight );

      for ( size_t i = 0; i < pSettings->m_pActiveDocInfo->m_LayeredFrames[0].LayerCount(); i++ )
      {
        GR::Graphic::Image*    pImage = new GR::Graphic::Image( (WORD)pSettings->m_pActiveDocInfo->Width(),
                                              (WORD)pSettings->m_pActiveDocInfo->Height(),
                                              pSettings->m_pActiveDocInfo->m_BitDepth );

        GR::Graphic::ContextDescriptor  cdSource( pSettings->m_pActiveDocInfo->GetImage( 0, i ), 
                                      pSettings->m_pActiveDocInfo->GetPalette( 0 ) ),

                            cdTarget( pImage, pSettings->m_pActiveDocInfo->GetPalette( 0 ) );

        cdSource.CopyRotated( cdTarget.Width() / 2, 
                              cdTarget.Height() / 2,
                              dlgRotate.m_Angle,
                              1.0f,
                              1.0f,
                              &cdTarget );

        pSettings->m_pActiveDocInfo->GetLayer( 0, i )->SetLayerImage( pImage );
      }
      pSettings->m_pActiveDocInfo->RecreateAllViews();
    }
    else
    {
      for ( size_t i = 0; i < pSettings->m_pActiveDocInfo->m_LayeredFrames[0].LayerCount(); i++ )
      {
        GR::Graphic::Image*    pImage = new GR::Graphic::Image( pSettings->m_pActiveDocInfo->GetImage( 0, i ) );

        GR::Graphic::ContextDescriptor  cdSource( pSettings->m_pActiveDocInfo->GetImage( 0, i ), 
                                      pSettings->m_pActiveDocInfo->GetPalette( 0 ) ),
                            cdTarget( pImage, pSettings->m_pActiveDocInfo->GetPalette( 0 ) );

        cdSource.CopyRotated( cdSource.Width() / 2, 
                              cdSource.Height() / 2,
                              dlgRotate.m_Angle,
                              1.0f,
                              1.0f,
                              &cdTarget );

        pSettings->m_pActiveDocInfo->GetLayer( 0, i )->SetLayerImage( pImage );
      }
    }
    pSettings->m_pActiveDocInfo->RedrawAllViews();
  }
}



void CMainFrame::OnMenuCreateFont() 
{
  CDlgCreateFontFromWindowsFont   dlgFont;

  if ( dlgFont.DoModal() == IDOK )
  {
  }
}



void CMainFrame::OnMenuColorInvert() 
{
  if ( pSettings->m_pActiveViewInfo == NULL )
  {
    return;
  }
  pSettings->m_pActiveViewInfo->Invert();
}



void CMainFrame::OnSpecialReplace() 
{
  DocumentInfo*    pDocInfo = theApp.GetActiveDocumentInfo();
  if ( pDocInfo == NULL )
  {
    return;
  }

  CDlgReplaceColor    dlgColor;

  dlgColor.m_Color1   = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR );
  dlgColor.m_Color2   = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 );

  if ( dlgColor.DoModal() == IDOK )
  {
    if ( pDocInfo->m_BitDepth <= 8 )
    {
      dlgColor.m_Color1   = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_8BIT );
      dlgColor.m_Color2   = pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2_8BIT );
    }
    pDocInfo->ReplaceColor( dlgColor.m_Color1, dlgColor.m_Color2 );
  }
}



#include "ActionConvert.h"


void CMainFrame::OnMenuScriptTest() 
{
  if ( pSettings->m_pActiveDocInfo == NULL )
  {
    return;
  }

  CPainterLua     Lua;


  Lua.DoString( "Doc = App:GetCurrentDocumentInfo();   CD = Doc:GetCD();  CD:PutPixel( 0, 0, tonumber( \"0xffffffff\", 16 ) ); " );

  pSettings->m_pActiveDocInfo->RedrawAllViews();

  /*
  CLayer*   pActiveLayer = pSettings->m_pActiveDocInfo->ActiveLayer();
  if ( pActiveLayer == NULL )
  {
    return;
  }

	//lua_dostring( pSettings->m_LuaInstance, "Painter:Brightness( 50 );" );
  CActionConvert    actionConvert;

  actionConvert.m_FormatTarget = IF_R8G8B8;
  GR::Graphic::ContextDescriptor cdSource( pActiveLayer->m_PImage.m_pImage, NULL );

  GR::Graphic::ContextDescriptor* pCDTarget = actionConvert.PerformAction( &cdSource );

  if ( pCDTarget )
  {
    GR::Graphic::Image*  pImage = pCDTarget->CreateImage();
    pImage->Save( "hurzzz.igf" );
    delete pImage;
    delete pCDTarget;
  }
  */
	
}



void CMainFrame::OnMenuPhotoshopFilter() 
{
	CDlgPSFilter    dlgFilter;

  dlgFilter.DoModal();
}



void CMainFrame::OnSpecialSelectionFromDialog() 
{
  CDlgSelector    dlgSelector;

  if ( dlgSelector.DoModal() == IDOK )
  {
  }
}



void CMainFrame::SaveBarSetup( const char* szProfileName )
{

  GRDockingManager::SaveBarState();
  /*
  CDockState    stDockState;

  GetDockState( stDockState );

  stDockState.SaveState( "BarStates" );


  int   iCount = m_mapControlBars.size();

  theApp.WriteProfileInt( "BarStates", "BarCount", iCount );

  int   iBarIndex = 0;
  tMapControlBars::iterator   it( m_mapControlBars.begin() );
  while ( it != m_mapControlBars.end() )
  {
    CPainterControlBar*   pBar = it->second;

    theApp.WriteProfileInt( "BarStates", CMisc::printf( "Bar%dID", iBarIndex ), pBar->GetDlgCtrlID() );
    theApp.WriteProfileInt( "BarStates", CMisc::printf( "Bar%dHorz", iBarIndex ), pBar->IsHorz() ? 1 : 0 );

    ++iBarIndex;
    ++it;
  }
  */

}



void CMainFrame::LoadBarSetup( const char* szProfileName )
{

  GRDockingManager::LoadBarState();
  /*
  CDockState    stDockState;

  stDockState.LoadState( "BarStates" );

  SetDockState( stDockState );

  // Horz-Vert wieder richten
  int   iCount = theApp.GetProfileInt( "BarStates", "BarCount", 0 );

  for ( int i = 0; i < iCount; ++i )
  {
    DWORD   dwBarID = theApp.GetProfileInt( "BarStates", CMisc::printf( "Bar%dID", i ), 0 );
    bool    bHorz   = theApp.GetProfileInt( "BarStates", CMisc::printf( "Bar%dHorz", i ), 1 ) > 0;

    tMapControlBars::iterator   it( m_mapControlBars.begin() );
    while ( it != m_mapControlBars.end() )
    {
      CPainterControlBar*   pBar = it->second;

      if ( pBar->GetDlgCtrlID() == dwBarID )
      {
        if ( bHorz )
        {
          pBar->CalcDynamicLayout( -1, LM_HORZDOCK );
        }
        else
        {
          pBar->CalcDynamicLayout( -1, LM_LENGTHY );
          pBar->CalcDynamicLayout( -1, LM_LENGTHY | LM_COMMIT );
          pBar->CalcDynamicLayout( -1, LM_VERTDOCK );

          if ( pBar->GetDockingFrame() )
          {
            pBar->GetDockingFrame()->RecalcLayout();
          }
        }
      }
      ++it;
    }
  }
  */

}



void CMainFrame::OnDestroy()
{

  __super::OnDestroy();
}



BOOL CMainFrame::DestroyWindow()
{
  // Bar-States speichern
  SaveBarSetup( "GRPainterBarStates" );

  tMapControlBars::iterator   itCB( m_mapControlBars.begin() );
  while ( itCB != m_mapControlBars.end() )
  {
    delete itCB->second;

    ++itCB;
  }

  return __super::DestroyWindow();
}



void CMainFrame::OnMenuToolbars()
{
  CDlgCustomizeToolBars   dlgCTB;

  dlgCTB.DoModal();
}



void CMainFrame::OnBearbeitenIsometrisieren()
{
  if ( pSettings->m_pActiveDocInfo == NULL )
  {
    return;
  }

  pSettings->m_pActiveDocInfo->AddUndoSizeChange();

  int     iNewWidth   = pSettings->m_pActiveDocInfo->Width() + pSettings->m_pActiveDocInfo->Height();
  int     iNewHeight  = pSettings->m_pActiveDocInfo->Height() / 2 + pSettings->m_pActiveDocInfo->Width() / 2;

  pSettings->m_pActiveDocInfo->SetSize( iNewWidth, iNewHeight );

  for ( size_t i = 0; i < pSettings->m_pActiveDocInfo->m_LayeredFrames[0].LayerCount(); i++ )
  {
    GR::Graphic::Image*    pImage = new GR::Graphic::Image( (WORD)pSettings->m_pActiveDocInfo->Width(),
                                          (WORD)pSettings->m_pActiveDocInfo->Height(),
                                          pSettings->m_pActiveDocInfo->m_BitDepth );

    GR::Graphic::ContextDescriptor  cdSource( pSettings->m_pActiveDocInfo->GetImage( 0, i ), 
                                  pSettings->m_pActiveDocInfo->GetPalette( 0 ) ),
                        cdTarget( pImage, pSettings->m_pActiveDocInfo->GetPalette( 0 ) );

    for ( int iX = 0; iX < cdSource.Width(); ++iX )
    {
      for ( int iY = 0; iY < cdSource.Height(); ++iY )
      {
        cdTarget.PutDirectPixel( iX + iY,
                                 cdSource.Width() / 2 + ( iY - iX ) / 2,
                                 cdSource.GetDirectPixel( iX, iY ) );
        /*
        // um 90 Grad gedreht
        cdTarget.PutDirectPixel( iX - iY + cdSource.Height(),
                                 ( iX + iY ) / 2,
                                 cdSource.GetDirectPixel( iX, iY ) );
                                 */
      }
    }
    pSettings->m_pActiveDocInfo->GetLayer( 0, i )->SetLayerImage( pImage );
  }
  pSettings->m_pActiveDocInfo->RecreateAllViews();
  pSettings->m_pActiveDocInfo->RedrawAllViews();

}



void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
  if ( nHitTest == HTSYSMENU )
  {
    ODMenu   odSysMenu( ODMenu::ODMS_XP );

    HMENU hSysMenu = GetSystemMenu( FALSE )->GetSafeHmenu();

    SendMessage( WM_INITMENU, (WPARAM)hSysMenu, 0 );
    SendMessage( WM_INITMENUPOPUP, (WPARAM)hSysMenu, MAKELPARAM( 0, TRUE ) );

    odSysMenu.Clone( hSysMenu, GetSafeHwnd() );

    RECT    rc,
            rcClient;

    GetWindowRect( &rc );
    GetClientRect( &rcClient );
    ClientToScreen( &rcClient );

    int   iX = rcClient.left;
    int   iY = rc.top + GetSystemMetrics( SM_CYCAPTION );

    DWORD   dwResult = odSysMenu.TrackPopupMenu( TPM_NONOTIFY | TPM_RETURNCMD, iX, iY, this );

    if ( dwResult )
    {
      PostMessage( WM_SYSCOMMAND, dwResult, 0 );
    }

    return;
  }
  __super::OnNcLButtonDown(nHitTest, point);
}



void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point )
{
  if ( SendMessage( WM_NCHITTEST, 0, MAKELPARAM( point.x, point.y ) ) == HTCAPTION )
  {
    ODMenu   odSysMenu( ODMenu::ODMS_XP );

    HMENU   hSysMenu = ::GetSystemMenu( m_hWnd, FALSE );
 
    //GetTopLevelFrame()->SendMessage( WM_INITMENU, (WPARAM)hSysMenu, 0 );
    GetTopLevelFrame()->SendMessage( WM_INITMENUPOPUP, (WPARAM)hSysMenu, MAKELPARAM( 0, TRUE ) );

    odSysMenu.Clone( hSysMenu, GetSafeHwnd() );


    DWORD   dwResult = odSysMenu.TrackPopupMenu( TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, this );

    if ( dwResult )
    {
      PostMessage( WM_SYSCOMMAND, dwResult, 0 );
    }

    return;
  }
}



LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  if ( message == WM_QUERYENDSESSION )
  {
    return OnQueryEndSession();
  }
  else if ( message == WM_ENDSESSION )
  {
    OnEndSession( (BOOL)wParam );
    return TRUE;
  }

  if ( message == 0x0313 )
  {
    // in der Startleiste auf Painter rechtsgeklickt
    ODMenu   odSysMenu( ODMenu::ODMS_XP );

    HMENU   hSysMenu = ::GetSystemMenu( m_hWnd, FALSE );
 
    GetTopLevelFrame()->SendMessage( WM_INITMENUPOPUP, (WPARAM)hSysMenu, MAKELPARAM( 0, TRUE ) );

    odSysMenu.Clone( hSysMenu, GetSafeHwnd() );

    POINT     point;

    GetCursorPos( &point );

    DWORD   dwResult = odSysMenu.TrackPopupMenu( TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, this );

    if ( dwResult )
    {
      PostMessage( WM_SYSCOMMAND, dwResult, 0 );
    }
    return TRUE;
  }

  return __super::WindowProc(message, wParam, lParam);
}



void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
  if ( bActive )
  {
    ViewInfo*    pViewInfo = theApp.GetActiveViewInfo();
    if ( pViewInfo )
    {
      pViewInfo->m_pDocInfo->m_pDoc->UpdateAllViews( NULL, ViewInfo::REDRAW_ALL );
    }
  }
  __super::OnActivateApp(bActive, dwThreadID);
}



void CMainFrame::OnSpecialExchangecolor()
{
  DocumentInfo*    pDocInfo = theApp.GetActiveDocumentInfo();
  if ( pDocInfo == NULL )
  {
    return;
  }

  if ( pDocInfo->m_BitDepth <= 8 )
  {
    pDocInfo->SwapColor( pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_8BIT ), pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2_8BIT ) );
  }
  else
  {
    pDocInfo->SwapColor( pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR ), pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 ) );
  }
}



void CMainFrame::OnFileCloseAll()
{
  std::list<CView*>   viewsToClose;


  for ( POSITION posTemplate = AfxGetApp()->GetFirstDocTemplatePosition(); posTemplate != NULL; )
  {
    auto pDocTemplate = AfxGetApp()->GetNextDocTemplate( posTemplate );

    POSITION posDoc = pDocTemplate->GetFirstDocPosition();
    while ( posDoc )
    {
      CDocument* pDoc = pDocTemplate->GetNextDoc( posDoc );

      POSITION posView = pDoc->GetFirstViewPosition();
      while ( posView )
      {
        CView* pView = pDoc->GetNextView( posView );

        viewsToClose.push_back( pView );
      }
    }
  }

  auto  itV( viewsToClose.begin() );
  while ( itV != viewsToClose.end() )
  {
    auto  pView = *itV;

    pView->GetParentFrame()->DestroyWindow();

    ++itV;
  }
}



BOOL CMainFrame::OnQueryEndSession()
{
  m_SessionEnding = true;
  return TRUE;
}



void CMainFrame::OnEndSession( BOOL Ending )
{
  if ( !Ending )
  {
    m_SessionEnding = false;
    return;
  }

  GR::String    tempPath = Path::Append( GR::IO::FileUtil::UserAppDataPath(), "GR Games/Painter/restart.dat" );

  GR::IO::FileUtil::CreateSubDir( Path::RemoveFileSpec( tempPath ) );

  ByteBuffer        tempDataTotal;

  // save temporary state for restoring!
  for ( POSITION posTemplate = AfxGetApp()->GetFirstDocTemplatePosition(); posTemplate != NULL; )
  {
    auto pDocTemplate = AfxGetApp()->GetNextDocTemplate( posTemplate );

    POSITION posDoc = pDocTemplate->GetFirstDocPosition();
    while ( posDoc )
    {
      CDocument* pDoc = pDocTemplate->GetNextDoc( posDoc );

      auto  pDocInfo = theApp.GetDocumentInfoFromDocument( pDoc );
      if ( pDocInfo != NULL )
      {
        ByteBuffer    tempData = pDocInfo->SaveTempData();

        tempDataTotal.AppendBuffer( tempData );
      }

      /*
      POSITION posView = pDoc->GetFirstViewPosition();
      while ( posView )
      {
        CView* pView = pDoc->GetNextView( posView );

      }*/
    }
  }
  GR::IO::FileUtil::WriteFileFromBuffer( tempPath, tempDataTotal );
}



void CMainFrame::OnSpecialSafetySave()
{
  OnEndSession( TRUE );
}



void CMainFrame::OnRestoreSafetySave()
{
  GR::String    tempPath = Path::Append( GR::IO::FileUtil::UserAppDataPath(), "GR Games/Painter/restart.dat" );

  ByteBuffer    data = GR::IO::FileUtil::ReadFileAsBuffer( tempPath );

  GR::IO::FileUtil::Delete( tempPath );

  GR::IO::FileChunk   chunk;

  MemoryStream  memIn( data );

  while ( chunk.Read( memIn ) )
  {
    MemoryStream   chunkReader( chunk.GetMemoryStream() );
    switch ( chunk.Type() )
    {
      case 0x0010:
        {
          GR::IO::FileChunk   chunkDoc;

          DocumentType  docType = (DocumentType)chunkReader.ReadI32();
          SaveType      saveType = (SaveType)chunkReader.ReadI32();

          CDocument*    pDoc = NULL;

          switch ( docType )
          {
            case DT_IMAGE:
              pDoc = theApp.pDocTemplate->CreateNewDocument();
              break;
            case DT_FONT:
              pDoc = theApp.pFontTemplate->CreateNewDocument();
              break;
            default:
              dh::Log( "Unsupported DocumentType %d encountered!", docType );
              return;
          }

          auto  pDocInfo = theApp.GetDocumentInfoFromDocument( pDoc );
          pDocInfo->RestoreFromTempData( chunkReader );
          pDocInfo->m_SaveType = saveType;

          auto pFrameWnd = theApp.pDocTemplate->CreateNewFrame( pDoc, NULL );

          theApp.pDocTemplate->InitialUpdateFrame( pFrameWnd, pDoc, TRUE );
        }
        break;
    }
  }
}
