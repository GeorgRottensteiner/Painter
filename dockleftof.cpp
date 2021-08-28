/*-DockControlBarLeftOf-------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CMainFrame::DockControlBarLeftOf( CToolBar* Bar, CToolBar* LeftOf )
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



