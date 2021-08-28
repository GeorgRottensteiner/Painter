// PainterControlBar.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "PainterControlBar.h"

#include <debug/debugclient.h>

#include "resource.h"


// CPainterControlBar


BOOL CPainterControlBar::hasclass = CPainterControlBar::RegisterMe();



/*-RegisterMe-----------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

BOOL CPainterControlBar::RegisterMe()
{
  WNDCLASS wc;   
  wc.style = 0;                                                 
  wc.lpfnWndProc = ::DefWindowProc; // must be this value
  wc.cbClsExtra = 0;                         
  wc.cbWndExtra = 0;                               
  wc.hInstance = (HINSTANCE)::GetModuleHandle(NULL);
  wc.hIcon = NULL;     // child window has no icon         
  wc.hCursor = LoadCursor( NULL, IDC_ARROW );
  wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  wc.lpszMenuName = NULL;  // no menu                             
  wc.lpszClassName = GRCONTROLBAR_CLASS_NAME;
  return AfxRegisterClass(&wc);
}



IMPLEMENT_DYNAMIC(CPainterControlBar, CControlBar)
CPainterControlBar::CPainterControlBar() :
  m_iMouseOverItem( -1 ),
  m_iPushedItem( -1 ),
  m_dwTimerID( 0 ),
  m_dwCommittedMode( 0 ),
  m_bHorizontal( true ),
  m_bDelayedLayout( true )
{
}

CPainterControlBar::~CPainterControlBar()
{
}


BEGIN_MESSAGE_MAP(CPainterControlBar, CControlBar)
  ON_WM_PAINT()
  ON_WM_CREATE()
  ON_WM_MOUSEMOVE()
  ON_WM_DESTROY()
  ON_WM_TIMER()
//  ON_WM_NCHITTEST()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_KILLFOCUS()
ON_WM_SETCURSOR()
ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()




class CPainterControlBarCmdUI : public CCmdUI        // class private to this file !
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
};

void CPainterControlBarCmdUI::Enable(BOOL bOn)
{
  CPainterControlBar*   pBar = (CPainterControlBar*)m_pOther;

  m_bEnableChanged = TRUE;

  pBar->EnableControl( m_nIndex, bOn ? true : false );
}

void CPainterControlBarCmdUI::SetCheck(int nCheck)
{
  CPainterControlBar*   pBar = (CPainterControlBar*)m_pOther;

  pBar->SetCheck( m_nIndex, nCheck ? true : false );

}

void CPainterControlBarCmdUI::SetText(LPCTSTR)
{
	// ignore it
}


void CPainterControlBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{

	CPainterControlBarCmdUI state;


	state.m_pOther = this;

	state.m_nIndexMax = (UINT)m_vectControls.size();
	for ( state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++ )
	{
		// get buttons state
    state.m_nID = m_vectControls[state.m_nIndex].m_dwID;

    if ( state.m_nID == ID_EDIT_UNDO )
    {
      dh::Log( "Update undo\n" );
    }

		// ignore separators
    if ( ( m_vectControls[state.m_nIndex].m_dwType != CT_GRIPPER )
    &&   ( m_vectControls[state.m_nIndex].m_dwType != CT_AUTOHIDING_GRIPPER )
    &&   ( m_vectControls[state.m_nIndex].m_dwType != CT_SEPARATOR ) )
		{
			// allow reflections
			if (CWnd::OnCmdMsg(0, 
				MAKELONG(CN_UPDATE_COMMAND_UI&0xff, WM_COMMAND+WM_REFLECT_BASE), 
				&state, NULL))
				continue;

			// allow the toolbar itself to have update handlers
			if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
				continue;

			// allow the owner to process the update
			state.DoUpdate(pTarget, bDisableIfNoHndler);
		}
	}

  UpdateDialogControls( pTarget, bDisableIfNoHndler );

}



void CPainterControlBar::RecalcItemSize( tControlEntry& Entry )
{

  RECT    rc;

  GetClientRect( &rc );

  switch ( Entry.m_dwType )
  {
    case CT_AUTOHIDING_GRIPPER:
    case CT_GRIPPER:
      if ( m_bHorizontal )
      {
        Entry.m_rcControl.bottom = Entry.m_rcControl.top + rc.bottom - rc.top;
        Entry.m_rcControl.right = Entry.m_rcControl.left + 8;
      }
      else
      {
        Entry.m_rcControl.right = Entry.m_rcControl.left + rc.right - rc.left;
        Entry.m_rcControl.bottom = Entry.m_rcControl.top + 8;
      }
      if ( Entry.m_dwType == CT_AUTOHIDING_GRIPPER )
      {
        Entry.m_bHidden = IsFloating() ? true : false;
      }
      break;
    case CT_SEPARATOR:
      if ( m_bHorizontal )
      {
        Entry.m_rcControl.right   = Entry.m_rcControl.left + 4;
        Entry.m_rcControl.bottom  = Entry.m_rcControl.top + rc.bottom - rc.top;
      }
      else
      {
        Entry.m_rcControl.right   = Entry.m_rcControl.left + rc.right - rc.left;
        Entry.m_rcControl.bottom  = Entry.m_rcControl.top + 4;
      }
      break;
  }

}



CSize CPainterControlBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{

	DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;

	return CalcLayout(dwMode);

}



CSize CPainterControlBar::CalcDynamicLayout( int nLength, DWORD dwMode )
{

  /*
  dh::Log( "CalcDynamic(%d) ", dwMode );
  if ( dwMode & LM_MRUWIDTH )
  {
    dh() << "LM_MRUWIDTH ";
  }
  if ( dwMode & LM_VERTDOCK )
  {
    dh() << "LM_VERTDOCK ";
  }
  if ( dwMode & LM_HORZDOCK )
  {
    dh() << "LM_HORZDOCK ";
  }
  if ( dwMode & LM_COMMIT )
  {
    dh() << "LM_COMMIT ";
  }
  if ( dwMode & LM_HORZ )
  {
    dh() << "LM_HORZ ";
  }
  if ( dwMode & LM_STRETCH )
  {
    dh() << "LM_STRETCH ";
  }
  if ( dwMode & LM_LENGTHY )
  {
    dh() << "LM_LENGTHY ";
  }
  dh::Log( "\n" );
  */

  /*
	if ((nLength == -1) && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
		((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)))
	{
		return CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZDOCK);
	}
  */
	return CalcLayout(dwMode, nLength);

}



CSize CPainterControlBar::CalcLayout( DWORD dwMode, int nLength )
{

  bool    bChangePositions = false;

  if ( ( dwMode & LM_HORZDOCK )
  ||   ( dwMode & LM_VERTDOCK ) )
  {
    bChangePositions = true;
    dwMode = m_dwCommittedMode;
    if ( dwMode & LM_VERTDOCK )
    {
      m_bHorizontal = false;
    }
    else
    {
      m_bHorizontal = true;
    }
  }

  if ( dwMode & LM_COMMIT )
  {
    dwMode = m_dwCommittedMode |= LM_COMMIT;
    bChangePositions = true;
    if ( dwMode & LM_LENGTHY )
    {
      m_bHorizontal = false;
    }
    else
    {
      m_bHorizontal = true;
    }
  }
  if ( dwMode & LM_MRUWIDTH )
  {
    dwMode = m_dwCommittedMode;
    if ( dwMode & LM_LENGTHY )
    {
      m_bHorizontal = false;
    }
    else
    {
      m_bHorizontal = true;
    }
  }

  if ( ( dwMode & LM_LENGTHY )
  ||   ( dwMode & LM_VERTDOCK ) )
  {
    m_bHorizontal = false;
  }
  else
  {
    m_bHorizontal = true;
  }

  RECT    rc;

  GetWindowRect( &rc );

  CSize     sizeOther( 20, 20 );

  if ( sizeOther.cx < 0 )
  {
    sizeOther = CControlBar::CalcDynamicLayout( nLength, dwMode );
  }

  int   iX = 2,
        iY = 0;

  tVectControls::iterator   it( m_vectControls.begin() );
  while ( it != m_vectControls.end() )
  {
    tControlEntry&  Entry = *it;

    int   iOldX = iX,
          iOldY = iY;

    if ( !( dwMode & LM_LENGTHY ) )
    {
      if ( Entry.m_dwType == CT_GRIPPER )
      {
        Entry.m_rcControl.right   = Entry.m_rcControl.left + 8;
        Entry.m_rcControl.bottom  = Entry.m_rcControl.top + 20;
      }
      else if ( Entry.m_dwType == CT_AUTOHIDING_GRIPPER )
      {
        /*
        if ( IsFloating() )
        {
          Entry.m_bHidden = true;
          ++it;
          continue;
        }
        */
        Entry.m_bHidden = false;
        Entry.m_rcControl.right   = Entry.m_rcControl.left + 8;
        Entry.m_rcControl.bottom  = Entry.m_rcControl.top + 20;
      }
      else if ( Entry.m_dwType == CT_SEPARATOR )
      {
        Entry.m_rcControl.right   = Entry.m_rcControl.left + 4;
        Entry.m_rcControl.bottom  = Entry.m_rcControl.top + 20;
      }

      iX += Entry.m_rcControl.right - Entry.m_rcControl.left;
      if ( Entry.m_rcControl.bottom - Entry.m_rcControl.top + 2 > iY )
      {
        iY = Entry.m_rcControl.bottom - Entry.m_rcControl.top;
      }

      if ( bChangePositions )
      {
        OffsetRect( &Entry.m_rcControl, iOldX - Entry.m_rcControl.left, 2 - Entry.m_rcControl.top );
        if ( Entry.m_dwType == CT_HWND )
        {
          CWnd*   pWnd = CWnd::FromHandle( Entry.m_hwnd );
          pWnd->MoveWindow( &Entry.m_rcControl );
        }
      }
    }
    else
    {
      // vertikaler Aufbau
      if ( Entry.m_dwType == CT_GRIPPER )
      {
        Entry.m_rcControl.right   = Entry.m_rcControl.left + 20;
        Entry.m_rcControl.bottom  = Entry.m_rcControl.top + 8;
      }
      else if ( Entry.m_dwType == CT_AUTOHIDING_GRIPPER )
      {
        if ( IsFloating() )
        {
          Entry.m_bHidden = true;
          ++it;
          continue;
        }
        Entry.m_bHidden = false;
        Entry.m_rcControl.right   = Entry.m_rcControl.left + 20;
        Entry.m_rcControl.bottom  = Entry.m_rcControl.top + 8;
      }
      else if ( Entry.m_dwType == CT_SEPARATOR )
      {
        Entry.m_rcControl.right   = Entry.m_rcControl.left + 20;
        Entry.m_rcControl.bottom  = Entry.m_rcControl.top + 4;
      }

      iY += Entry.m_rcControl.bottom - Entry.m_rcControl.top;
      if ( Entry.m_rcControl.right - Entry.m_rcControl.left + 2 > iX )
      {
        iX = Entry.m_rcControl.right - Entry.m_rcControl.left;
      }
      if ( bChangePositions )
      {
        OffsetRect( &Entry.m_rcControl, 2 - Entry.m_rcControl.left, iOldY - Entry.m_rcControl.top );
        if ( Entry.m_dwType == CT_HWND )
        {
          CWnd*   pWnd = CWnd::FromHandle( Entry.m_hwnd );
          pWnd->MoveWindow( &Entry.m_rcControl );
        }
      }
    }

    ++it;
  }
  iX += 4;
  iY += 4;

  if ( sizeOther.cx < iX )
  {
    sizeOther.cx = iX;
  }
  if ( sizeOther.cy < iY )
  {
    sizeOther.cy = iY;
  }

  if ( sizeOther.cx < 22 )
  {
    sizeOther.cx = 22;
  }
  if ( sizeOther.cy < 22 )
  {
    sizeOther.cy = 22;
  }

  if ( ( !( dwMode & LM_COMMIT ) )
  &&   ( !( dwMode & LM_MRUWIDTH ) ) )
  {
    m_dwCommittedMode = dwMode;
    m_szCommitted = sizeOther;

    if ( m_bHorizontal )
    {
      m_dwStyle |= CBRS_ALIGN_TOP;
      m_dwStyle &= ~CBRS_ALIGN_LEFT;
    }
    else
    {
      m_dwStyle &= ~CBRS_ALIGN_TOP;
      m_dwStyle |= CBRS_ALIGN_LEFT;
    }
  }
  else
  {
    Invalidate();
  }

  if ( dwMode & LM_COMMIT )
  {
    tVectControls::iterator   it( m_vectControls.begin() );
    while ( it != m_vectControls.end() )
    {
      tControlEntry&  Entry = *it;

      RecalcItemSize( Entry );

      ++it;
    }
  }

  return m_szCommitted;

}



void CPainterControlBar::AddGripper( bool bAutoHiding )
{

  m_vectControls.push_back( tControlEntry() );

  tControlEntry&      Entry = m_vectControls.back();

  if ( bAutoHiding )
  {
    Entry.m_dwType  = CT_AUTOHIDING_GRIPPER;
  }
  else
  {
    Entry.m_dwType  = CT_GRIPPER;
  }
  Entry.m_rcControl.right   = 8;
  Entry.m_rcControl.bottom  = 20;

}



void CPainterControlBar::AddSeparator()
{

  m_vectControls.push_back( tControlEntry() );

  tControlEntry&      Entry = m_vectControls.back();

  Entry.m_dwType  = CT_SEPARATOR;
  Entry.m_rcControl.right   = 4;
  Entry.m_rcControl.bottom  = 20;

}



void CPainterControlBar::AddHWND( HWND hWnd )
{

  SIZE    szControl;

  RECT    rc;

  ::GetWindowRect( hWnd, &rc );

  szControl.cx = rc.right - rc.left;
  szControl.cy = rc.bottom - rc.top;

  AddControl( CT_HWND, "", (DWORD)hWnd, 0, &szControl );

  CWnd*   pWnd = CWnd::FromHandle( hWnd );

  pWnd->SetParent( this );
  pWnd->SetOwner( this );

}



void CPainterControlBar::AddButton( const char* szText, DWORD dwID, HICON hIcon, SIZE* pSize )
{

  AddControl( CT_BUTTON, szText, dwID, hIcon, pSize );

}



void CPainterControlBar::AddButtonDropDown( const char* szText, DWORD dwID, HICON hIcon, SIZE* pSize )
{

  AddControl( CT_BUTTON, szText, dwID, hIcon, pSize );
  AddControl( CT_BUTTON_DROPDOWN, szText, dwID, NULL, &CSize( 11, 20 ) );

}



void CPainterControlBar::AddText( const char* szText, DWORD dwID, HICON hIcon, SIZE* pSize )
{

  AddControl( CT_TEXT, szText, dwID, hIcon, pSize );

}



void CPainterControlBar::AddCheckButton( const char* szText, DWORD dwID, HICON hIcon, SIZE* pSize )
{

  AddControl( CT_CHECKBUTTON, szText, dwID, hIcon, pSize );

}



void CPainterControlBar::AddRadioButton( const char* szText, DWORD dwID, HICON hIcon, SIZE* pSize )
{

  AddControl( CT_RADIOBUTTON, szText, dwID, hIcon, pSize );

}



void CPainterControlBar::AddControl( eControlType eType, const char* szText, DWORD dwID, HICON hIcon, SIZE* pSize )
{

  m_vectControls.push_back( tControlEntry() );

  tControlEntry&      Entry = m_vectControls.back();

  Entry.m_dwID      = dwID;
  Entry.m_strText   = szText;
  Entry.m_dwType    = eType;
  Entry.m_hIcon     = hIcon;
  if ( pSize )
  {
    Entry.m_rcControl.right   = pSize->cx;
    Entry.m_rcControl.bottom  = pSize->cy;
  }

  if ( ( eType == CT_BUTTON )
  ||   ( eType == CT_CHECKBUTTON )
  ||   ( eType == CT_TEXT )
  ||   ( eType == CT_RADIOBUTTON )
  ||   ( eType == CT_BUTTON_DROPDOWN ) )
  {
    CDC*    pDC = GetDC();

    RECT    rc;



    if ( !Entry.m_strText.empty() )
    {
      rc.left = 0;
      rc.top = 0;
      rc.right = 200;
      rc.bottom = 200;
      pDC->DrawText( szText, Entry.m_strText.length(), &rc, DT_CALCRECT | DT_SINGLELINE );

      Entry.m_rcControl.right   = rc.right - rc.left;
      Entry.m_rcControl.bottom  = rc.bottom - rc.top;
    }
    if ( Entry.m_hIcon )
    {
      ICONINFO    ii;

      GetIconInfo( Entry.m_hIcon, &ii );

      BITMAP      bm;

      GetObject( ii.hbmColor, sizeof( bm ), &bm );

      DeleteObject( ii.hbmColor );
      DeleteObject( ii.hbmMask );

      if ( Entry.m_rcControl.right )
      {
        // 4 Pixel Abstand zum Text
        Entry.m_rcControl.right += 6;
      }
      // Platz für den Rahmen
      Entry.m_rcControl.right += 6;

      Entry.m_rcControl.right += bm.bmWidth;
      if ( bm.bmHeight + 4 > Entry.m_rcControl.bottom )
      {
        Entry.m_rcControl.bottom = bm.bmHeight + 4;
      }
    }

    if ( Entry.m_rcControl.bottom - Entry.m_rcControl.top < 20 )
    {
      Entry.m_rcControl.bottom = Entry.m_rcControl.top + 20;
    }
  }
  else if ( eType == CT_HWND )
  {
    SetRect( &Entry.m_rcControl, 0, 0, pSize->cx, pSize->cy );
    Entry.m_hwnd = (HWND)dwID;
  }

  Invalidate();

}



void CPainterControlBar::DrawItem( tControlEntry& Entry, CDC& dc, RECT rc )
{

  switch ( Entry.m_dwType )
  {
    case CT_GRIPPER:
    case CT_AUTOHIDING_GRIPPER:
      {
        if ( m_bHorizontal )
        {
          CPen      penSolid( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );

          CPen*     pOldPen = dc.SelectObject( &penSolid );
          for ( int i = 0; i <= ( rc.bottom - rc.top ) / 2; i++ )
          {
            dc.MoveTo( rc.left + 2, rc.top + i * 2 );
            dc.LineTo( rc.right - 2, rc.top + i * 2 );
          }
          dc.SelectObject( pOldPen );
          /*
          dc.Draw3dRect( rc.left + 1, rc.top, 3, rc.bottom - rc.top, 
                        GetSysColor( COLOR_3DHIGHLIGHT ),
                        GetSysColor( COLOR_3DSHADOW ) );
          dc.Draw3dRect( rc.left + 4, rc.top, 3, rc.bottom - rc.top,
                        GetSysColor( COLOR_3DHIGHLIGHT ),
                        GetSysColor( COLOR_3DSHADOW ) );
                        */
        }
        else
        {
          CPen      penSolid( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );

          CPen*     pOldPen = dc.SelectObject( &penSolid );
          for ( int i = 0; i <= ( rc.right - rc.left ) / 2; i++ )
          {
            dc.MoveTo( rc.left + i * 2, rc.top + 2 );
            dc.LineTo( rc.left + i * 2, rc.bottom - 2 );
          }
          dc.SelectObject( pOldPen );

          /*
          dc.Draw3dRect( rc.left, rc.top + 1, rc.right - rc.left, 3,
                        GetSysColor( COLOR_3DHIGHLIGHT ),
                        GetSysColor( COLOR_3DSHADOW ) );
          dc.Draw3dRect( rc.left, rc.top + 4, rc.right - rc.left, 3,
                        GetSysColor( COLOR_3DHIGHLIGHT ),
                        GetSysColor( COLOR_3DSHADOW ) );
                        */
        }
      }
      break;
    case CT_SEPARATOR:
      {
        if ( m_bHorizontal )
        {
          dc.Draw3dRect( rc.left + 1, rc.top, 2, rc.bottom - rc.top, 
                         GetSysColor( COLOR_3DSHADOW ),
                         GetSysColor( COLOR_3DHIGHLIGHT ) );
        }
        else
        {
          dc.Draw3dRect( rc.left, rc.top + 1, rc.right - rc.left, 2,
                         GetSysColor( COLOR_3DSHADOW ),
                         GetSysColor( COLOR_3DHIGHLIGHT ) );
        }
      }
      break;
    case CT_TEXT:
      {
        RECT    origRect = rc;

        dc.SetBkMode( TRANSPARENT );
        dc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) );

        if ( Entry.m_hIcon )
        {
          ICONINFO    ii;

          GetIconInfo( Entry.m_hIcon, &ii );

          BITMAP      bm;

          GetObject( ii.hbmColor, sizeof( bm ), &bm );

          DeleteObject( ii.hbmColor );
          DeleteObject( ii.hbmMask );

          RECT    rcIcon;

          rcIcon = rc;
          if ( m_bHorizontal )
          {
            rcIcon.right = rcIcon.left + rcIcon.bottom - rcIcon.top;
          }
          else
          {
            rcIcon.bottom = rcIcon.top + rcIcon.right - rcIcon.left;
          }

          DrawIconEx( dc.GetSafeHdc(),
                      rcIcon.left + ( rcIcon.right - rcIcon.left - bm.bmWidth ) / 2, 
                      rcIcon.top + ( rcIcon.bottom - rcIcon.top - bm.bmHeight ) / 2,
                      Entry.m_hIcon,
                      0,
                      0,
                      0, 0, DI_NORMAL );
          rc.left += 1 + bm.bmWidth + 4;
        }
        dc.DrawText( Entry.m_strText.c_str(), Entry.m_strText.length(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
      }
      break;
    case CT_BUTTON:
    case CT_CHECKBUTTON:
    case CT_RADIOBUTTON:
    case CT_BUTTON_DROPDOWN:
      {
        RECT    origRect = rc;

        dc.SetBkMode( TRANSPARENT );
        dc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) );

        if ( Entry.m_hIcon )
        {
          ICONINFO    ii;

          GetIconInfo( Entry.m_hIcon, &ii );

          BITMAP      bm;

          GetObject( ii.hbmColor, sizeof( bm ), &bm );

          DeleteObject( ii.hbmColor );
          DeleteObject( ii.hbmMask );

          if ( Entry.m_bChecked )
          {
            dc.FillSolidRect( &origRect, GetSysColor( COLOR_3DHILIGHT ) );
          }

          RECT    rcIcon;

          rcIcon = rc;
          if ( m_bHorizontal )
          {
            rcIcon.right = rcIcon.left + rcIcon.bottom - rcIcon.top;
          }
          else
          {
            rcIcon.bottom = rcIcon.top + rcIcon.right - rcIcon.left;
          }

          if ( !Entry.m_bDisabled )
          {
            if ( ( Entry.m_bPushed )
            ||   ( Entry.m_bChecked ) )
            {
              OffsetRect( &rcIcon, 1, 1 );
            }
          }

          if ( Entry.m_bDisabled )
          {
            DrawState( dc.GetSafeHdc(),
                       0, 0,
                       (LPARAM)Entry.m_hIcon,
                       0,
                       rcIcon.left + ( rcIcon.right - rcIcon.left - bm.bmWidth ) / 2, 
                       rcIcon.top + ( rcIcon.bottom - rcIcon.top - bm.bmHeight ) / 2,
                       bm.bmWidth,
                       bm.bmHeight,
                       DST_ICON | DSS_DISABLED );
          }
          else
          {
            DrawIconEx( dc.GetSafeHdc(),
                        rcIcon.left + ( rcIcon.right - rcIcon.left - bm.bmWidth ) / 2, 
                        rcIcon.top + ( rcIcon.bottom - rcIcon.top - bm.bmHeight ) / 2,
                        Entry.m_hIcon,
                        0,
                        0,
                        0, 0, DI_NORMAL );
          }

          rc.left += 1 + bm.bmWidth + 4;
        }

        // Drop-Down-Pfeil
        if ( Entry.m_dwType == CT_BUTTON_DROPDOWN )
        {
          dc.FillSolidRect( rc.left + 3, rc.bottom - 6, rc.right - rc.left - 7, 1, 0 );
          dc.FillSolidRect( rc.left + 4, rc.bottom - 5, rc.right - rc.left - 9, 1, 0 );
          dc.FillSolidRect( rc.left + 5, rc.bottom - 4, rc.right - rc.left - 11, 1, 0 );
        }
        
        if ( ( Entry.m_bPushed )
        ||   ( Entry.m_bChecked ) )
        {
          if ( !Entry.m_bDisabled )
          {
            dc.Draw3dRect( &origRect, GetSysColor( COLOR_3DSHADOW ), GetSysColor( COLOR_3DHIGHLIGHT ) );
            OffsetRect( &rc, 1, 1 );
          }
          dc.DrawText( Entry.m_strText.c_str(), Entry.m_strText.length(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
        }
        else if ( Entry.m_bMouseOver )
        {
          if ( !Entry.m_bDisabled )
          {
            dc.Draw3dRect( &origRect, GetSysColor( COLOR_3DHIGHLIGHT ), GetSysColor( COLOR_3DSHADOW ) );
          }
          dc.DrawText( Entry.m_strText.c_str(), Entry.m_strText.length(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
        }
        else
        {
          dc.DrawText( Entry.m_strText.c_str(), Entry.m_strText.length(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
        }
      }
      break;
  }

}



void CPainterControlBar::OnPaint()
{

  CPaintDC dc(this);

  HFONT hOldFont = (HFONT)dc.SelectObject( (HFONT)GetStockObject( DEFAULT_GUI_FONT ) );


  RECT    rc,
          rcClient;

  GetClientRect( &rcClient );

  dc.FillSolidRect( &rcClient, GetSysColor( COLOR_BTNFACE ) );

  tVectControls::iterator   it( m_vectControls.begin() );
  while ( it != m_vectControls.end() )
  {
    tControlEntry&  Entry = *it;

    if ( !Entry.m_bHidden )
    {
      rc = Entry.m_rcControl;
      DrawItem( Entry, dc, rc );
    }

    ++it;
  }

  dc.SelectObject( hOldFont );

}

int CPainterControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CControlBar::OnCreate(lpCreateStruct) == -1)
    return -1;

  SendMessage( WM_SETFONT, (WPARAM)GetStockObject( DEFAULT_GUI_FONT ), 0 );
  EnableToolTips();

  return 0;
}



int CPainterControlBar::ItemFromPoint( POINT& pt )
{

  RECT    rc,
          rcClient;

  GetClientRect( &rcClient );

  for ( size_t i = 0; i < m_vectControls.size(); ++i )
  {
    if ( GetItemRect( i, rc ) )
    {
      if ( PtInRect( &rc, pt ) )
      {
        return i;
      }
    }
  }
  return -1;

}



void CPainterControlBar::OnMouseMove(UINT nFlags, CPoint point)
{

  int   iItem = ItemFromPoint( point );

  if ( iItem != -1 )
  {
    if ( iItem != m_iMouseOverItem )
    {
      if ( ( m_iMouseOverItem != -1 )
      &&   ( !m_vectControls[m_iMouseOverItem].m_bDisabled ) )
      {
        m_vectControls[m_iMouseOverItem].m_bMouseOver = false;
        m_vectControls[m_iMouseOverItem].m_bPushed = false;

        RECT    rc;
        if ( GetItemRect( m_iMouseOverItem, rc ) )
        {
          InvalidateRect( &rc );
        }
      }

      if ( ( !m_vectControls[iItem].m_bDisabled )
      &&   ( ( m_iPushedItem == iItem )
      ||     ( m_iPushedItem == -1 ) ) )
      {
        m_iMouseOverItem = iItem;
        RECT    rc;
        if ( GetItemRect( iItem, rc ) )
        {
          m_vectControls[iItem].m_bMouseOver = true;
          if ( iItem == m_iPushedItem )
          {
            m_vectControls[iItem].m_bPushed = true;
          }

          InvalidateRect( &rc );

          if ( m_dwTimerID == 0 )
          {
            m_dwTimerID = 17;
            SetTimer( m_dwTimerID, 50, NULL );
          }
        }
      }
    }
  }
  else if ( m_iMouseOverItem != -1 )
  {
    if ( !m_vectControls[m_iMouseOverItem].m_bDisabled )
    {
      m_vectControls[m_iMouseOverItem].m_bMouseOver = false;
      m_vectControls[m_iMouseOverItem].m_bPushed = false;
      RECT    rc;
      if ( GetItemRect( m_iMouseOverItem, rc ) )
      {
        InvalidateRect( &rc );
      }
      m_iMouseOverItem = -1;
    }
  }

  CControlBar::OnMouseMove(nFlags, point);
}



bool CPainterControlBar::GetItemRect( size_t iItem, RECT& rc )
{

  if ( iItem >= m_vectControls.size() )
  {
    SetRectEmpty( &rc );
    return false;
  }
  if ( m_vectControls[iItem].m_bHidden )
  {
    SetRectEmpty( &rc );
    return false;
  }

  rc = m_vectControls[iItem].m_rcControl;

  return true;

}
void CPainterControlBar::OnDestroy()
{

  if ( m_dwTimerID )
  {
    KillTimer( m_dwTimerID );
    m_dwTimerID = 0;
  }

  CControlBar::OnDestroy();

}

void CPainterControlBar::OnTimer(UINT nIDEvent)
{

  if ( m_iMouseOverItem != -1 )
  {
    POINT   pt;
    GetCursorPos( &pt );
    ScreenToClient( &pt );

    RECT    rc;
    if ( GetItemRect( m_iMouseOverItem, rc ) )
    {
      if ( !PtInRect( &rc, pt ) )
      {
        m_vectControls[m_iMouseOverItem].m_bMouseOver = false;
        m_vectControls[m_iMouseOverItem].m_bPushed = false;
        m_iMouseOverItem = -1;
        InvalidateRect( &rc );
        KillTimer( m_dwTimerID );
        m_dwTimerID = 0;
      }
      else
      {
        SetStatusText( m_vectControls[m_iMouseOverItem].m_dwID );
      }
    }
  }

  CControlBar::OnTimer(nIDEvent);
}


void CPainterControlBar::OnLButtonDown(UINT nFlags, CPoint point)
{

  ActivateTopParent();

  int   iItem = ItemFromPoint( point );

  if ( iItem != -1 )
  {
    if ( ( m_vectControls[iItem].m_dwType != CT_GRIPPER )
    &&   ( m_vectControls[iItem].m_dwType != CT_AUTOHIDING_GRIPPER ) )
    {
      if ( ( !m_vectControls[iItem].m_bPushed )
      &&   ( !m_vectControls[iItem].m_bDisabled )
      &&   ( m_vectControls[iItem].m_dwType != CT_SEPARATOR ) )
      {
        m_iPushedItem = iItem;
        m_vectControls[iItem].m_bPushed = true;
        InvalidateRect( &m_vectControls[iItem].m_rcControl );
        SetCapture();
      }
      return;
    }
  }

  CControlBar::OnLButtonDown(nFlags, point);
}



void CPainterControlBar::OnLButtonUp(UINT nFlags, CPoint point)
{

  if ( GetCapture() == this )
  {
    ReleaseCapture();
  }
  ActivateTopParent();

  int   iItem = ItemFromPoint( point );

  if ( ( iItem == m_iPushedItem )
  &&   ( iItem != -1 ) )
  {
    // Commando auslösen
    if ( ( m_vectControls[m_iPushedItem].m_dwType == CT_BUTTON )
    ||   ( m_vectControls[m_iPushedItem].m_dwType == CT_CHECKBUTTON )
    ||   ( m_vectControls[m_iPushedItem].m_dwType == CT_RADIOBUTTON ) )
    {
      if ( m_vectControls[m_iPushedItem].m_dwType == CT_CHECKBUTTON )
      {
        m_vectControls[m_iPushedItem].m_bChecked = !m_vectControls[m_iPushedItem].m_bChecked;
      }
      else if ( m_vectControls[m_iPushedItem].m_dwType == CT_RADIOBUTTON )
      {
        SetCheck( m_iPushedItem );
      }
      if ( GetOwner() )
      {
        GetOwner()->PostMessage( WM_COMMAND, MAKEWPARAM( m_vectControls[m_iPushedItem].m_dwID, 0 ), (LPARAM)GetSafeHwnd() );
      }
    }
    else if ( m_vectControls[m_iPushedItem].m_dwType == CT_BUTTON_DROPDOWN )
    {
      if ( GetOwner() )
      {
        NMTOOLBAR     nmMessage;

        nmMessage.hdr.code      = (TBN_FIRST - 10);   // TBN_DROPDOWN
        nmMessage.hdr.hwndFrom  = GetSafeHwnd();
        nmMessage.hdr.idFrom    = GetDlgCtrlID();
        nmMessage.iItem         = m_iPushedItem;

        GetOwner()->PostMessage( WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&nmMessage );
      }
    }
    InvalidateRect( &m_vectControls[m_iPushedItem].m_rcControl );
  }

  if ( m_iPushedItem != -1 )
  {
    if ( m_vectControls[m_iPushedItem].m_bPushed )
    {
      m_vectControls[m_iPushedItem].m_bPushed = false;
      InvalidateRect( &m_vectControls[m_iPushedItem].m_rcControl );
    }
    m_iPushedItem = -1;
    return;
  }

  CControlBar::OnLButtonUp(nFlags, point);
}



void CPainterControlBar::EnableControl( size_t iControlIndex, bool bEnable )
{

  if ( iControlIndex >= m_vectControls.size() )
  {
    return;
  }

  if ( m_vectControls[iControlIndex].m_bDisabled != bEnable )
  {
    return;
  }

  m_vectControls[iControlIndex].m_bDisabled = !bEnable;
  InvalidateRect( &m_vectControls[iControlIndex].m_rcControl );

}



void CPainterControlBar::SetCheck( size_t iButtonIndex, bool bSetCheck )
{

  if ( iButtonIndex >= m_vectControls.size() )
  {
    return;
  }

  if ( m_vectControls[iButtonIndex].m_bChecked == bSetCheck )
  {
    return;
  }

  m_vectControls[iButtonIndex].m_bChecked = bSetCheck;
  InvalidateRect( &m_vectControls[iButtonIndex].m_rcControl );

  if ( m_vectControls[iButtonIndex].m_dwType != CT_RADIOBUTTON )
  {
    return;
  }

  // Radio-Buttons - andere Buttons entchecken
  size_t    iLeft = iButtonIndex;

  while ( iLeft > 0 )
  {
    iLeft--;
    if ( m_vectControls[iLeft].m_dwType != CT_RADIOBUTTON )
    {
      break;
    }
    m_vectControls[iLeft].m_bChecked = false;
    InvalidateRect( &m_vectControls[iLeft].m_rcControl );
  }

  iLeft = iButtonIndex;
  while ( iLeft + 1 < m_vectControls.size() )
  {
    iLeft++;
    if ( m_vectControls[iLeft].m_dwType != CT_RADIOBUTTON )
    {
      break;
    }
    m_vectControls[iLeft].m_bChecked = false;
    InvalidateRect( &m_vectControls[iLeft].m_rcControl );
  }

}



bool CPainterControlBar::IsChecked( size_t iButtonIndex ) const
{

  if ( iButtonIndex >= m_vectControls.size() )
  {
    return false;
  }

  return m_vectControls[iButtonIndex].m_bChecked;

}



void CPainterControlBar::OnKillFocus(CWnd* pNewWnd)
{

  CControlBar::OnKillFocus(pNewWnd);

  if ( GetCapture() == this )
  {
    dh::Log( "release capture focus\n" );
    ReleaseCapture();
  }

}

BOOL CPainterControlBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{

  POINT   pt;

  GetCursorPos( &pt );
  ScreenToClient( &pt );

  int   iItem = ItemFromPoint( pt );
  if ( ( iItem != -1 )
  &&   ( ( m_vectControls[iItem].m_dwType == CT_GRIPPER )
  ||     ( m_vectControls[iItem].m_dwType == CT_AUTOHIDING_GRIPPER ) ) )
  {
    SetCursor( LoadCursor( NULL, IDC_SIZEALL ) );
    return TRUE;
  }

  return CControlBar::OnSetCursor(pWnd, nHitTest, message);
}



bool CPainterControlBar::IsHorz() const
{
  
  return m_bHorizontal;

}
void CPainterControlBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
  CControlBar::OnShowWindow(bShow, nStatus);

  tVectControls::iterator   it( m_vectControls.begin() );
  while ( it != m_vectControls.end() )
  {
    tControlEntry&  Entry = *it;

    if ( Entry.m_dwType == CT_HWND )
    {
      CWnd*   pWnd = CWnd::FromHandle( Entry.m_hwnd );
      pWnd->ShowWindow( bShow ? SW_SHOW : SW_HIDE );
    }

    ++it;
  }
}
