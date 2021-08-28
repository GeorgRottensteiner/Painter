#include "stdafx.h"
#include "TextDialog.h"
#include "ViewInfo.h"
#include "Settings.h"
#include "PainterGFXTools.h"
#include "DlgSelectEffekt.h"
#include "DlgTEAura.h"

#include <Misc/Misc.h>
#include ".\textdialog.h"

#include <math/math_util.h>

#include <Grafik/PolygonFill.h>

#include <debug/debugclient.h>

#include <String/Convert.h>

#include "PolygonFillAA.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





CTextDialog::CTextDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTextDialog::IDD, pParent),
    pPagePreview( NULL )
{

	//{{AFX_DATA_INIT(CTextDialog)
	//}}AFX_DATA_INIT

  m_Expanded           = false;
  m_Initialized        = false;
  m_ApplyEffectsSingle = false;

  m_logFont.lfCharSet         = 0;
  m_logFont.lfClipPrecision   = 2;
  m_logFont.lfEscapement      = 0;
  wsprintf( m_logFont.lfFaceName, GR::Convert::ToUTF16( pSettings->GetSettingString( "TDFontName", "Arial" ) ).c_str() );
  m_logFont.lfHeight          = pSettings->GetSetting( "TDFontHeight", -16 );
  m_logFont.lfItalic          = 0;
  m_logFont.lfOrientation     = 0;
  m_logFont.lfOutPrecision    = 3;
  m_logFont.lfPitchAndFamily  = 34;
  m_logFont.lfQuality         = 1;
  m_logFont.lfStrikeOut       = 0;
  m_logFont.lfUnderline       = 0;
  m_logFont.lfWeight          = pSettings->GetSetting( "TDFontWidth", 400 );
  m_logFont.lfWidth           = 0;
  
  m_Kerning      = pSettings->GetSetting( "TDFontKerning", 100 );
  m_Tilt         = pSettings->GetSetting( "TDFontTilt", 0 );

  m_Filled       = !!pSettings->GetSetting( "TDTextFilled", true );
}



void CTextDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTextDialog)
  DDX_Control(pDX, IDC_CHECK_EFFEKT_SINGLE, m_CheckEffectsSingle);
  DDX_Control(pDX, IDC_LIST_EFFEKTE, m_ListEffekte);
  DDX_Control(pDX, IDC_BUTTON_EFFEKTLISTE, m_ButtonToggleEffektListe);
  DDX_Control(pDX, IDC_SPIN_TEXT_TILT, m_SpinTilt);
  DDX_Control(pDX, IDC_EDIT_TEXT_TILT, m_EditTilt);
  DDX_Control(pDX, IDC_SPIN_TEXT_KERNING, m_SpinKerning);
  DDX_Control(pDX, IDC_EDIT_TEXT_KERNING, m_EditKerning);
  DDX_Control(pDX, IDC_EDIT_TEXT_TEXT, m_EditText);
  DDX_Control(pDX, IDC_STATIC_TEXT_PREVIEW, m_StaticPreview);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_CHECK_FILLED, m_CheckFilled);
}


BEGIN_MESSAGE_MAP(CTextDialog, CDialog)
	//{{AFX_MSG_MAP(CTextDialog)
	ON_WM_DESTROY()
	ON_WM_DRAWITEM()
	ON_EN_CHANGE(IDC_EDIT_TEXT_TEXT, OnChangeEditTextText)
	ON_EN_CHANGE(IDC_EDIT_TEXT_KERNING, OnChangeEditTextKerning)
	ON_EN_CHANGE(IDC_EDIT_TEXT_TILT, OnChangeEditTextTilt)
	ON_BN_CLICKED(IDC_BUTTON_EFFEKTLISTE, OnButtonEffektListe)
	ON_BN_CLICKED(IDC_BUTTON_NEWEFFEKT, OnButtonNewEffekt)
	ON_LBN_DBLCLK(IDC_LIST_EFFEKTE, OnDblclkListEffekte)
	ON_BN_CLICKED(IDC_CHECK_EFFEKT_SINGLE, OnCheckEffektSingle)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BUTTON_FONT, OnBnClickedButtonFont)
  ON_BN_CLICKED(IDC_CHECK_FILLED, OnBnClickedCheckFilled)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTextDialog 



BOOL CTextDialog::OnInitDialog() 
{
  if ( pViewInfo == NULL )
  {
    return FALSE;
  }

	CDialog::OnInitDialog();


  RECT                rc;
	

  m_StaticPreview.ModifyStyle( 0, SS_OWNERDRAW );
  m_StaticPreview.GetClientRect( &rc );
  pPagePreview = new GR::Graphic::GDIPage();
  pPagePreview->Create( m_StaticPreview.GetSafeHwnd(), rc.right - rc.left, rc.bottom - rc.top, (BYTE)pViewInfo->GetViewBitDepth() );

  GetWindowRect( &rc );
  
  SetWindowPos( NULL, 0, 0, rc.right - rc.left, 328, SWP_NOZORDER | SWP_NOMOVE );

  if ( pViewInfo->GetViewBitDepth() <= 8 )
  {
    pPagePreview->SetPalette( pViewInfo->m_pPalette );
  }

  m_Text = pSettings->GetSettingString( "TDText", "i" );

  m_EditText.SetWindowText( GR::Convert::ToUTF16( m_Text ).c_str() );

  m_SpinKerning.SetRange( -100, 500 );
  m_EditKerning.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_Kerning ) ).c_str() );

  m_SpinTilt.SetRange( -200, 200 );
  m_EditTilt.SetWindowText( GR::Convert::ToUTF16( CMisc::printf( "%d", m_Tilt ) ).c_str() );

  if ( m_Filled )
  {
    m_CheckFilled.SetCheck( BST_CHECKED );
  }

  m_Initialized = true;

  m_hFont = CreateFontIndirect( &m_logFont );
  UpdateFont();
  RebuildPreview();

	return TRUE;
}



void CTextDialog::OnDestroy() 
{
	CDialog::OnDestroy();

  tListEffekte::iterator    it( m_Effects.begin() );
  while ( it != m_Effects.end() )
  {
    delete *it;
    it = m_Effects.erase( it );
  }

  if ( m_hFont )
  {
    DeleteObject( m_hFont );
    m_hFont = NULL;
  }

  m_KerningPairs.clear();
  if ( pPagePreview != NULL )
  {
    delete pPagePreview;
    pPagePreview = NULL;
  }
}



void CTextDialog::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
  if ( lpDrawItemStruct->hwndItem == m_StaticPreview.GetSafeHwnd() )
  {
    BitBlt( lpDrawItemStruct->hDC,
            0,
            0,
            pPagePreview->GetWidth(),
            pPagePreview->GetHeight(),
            pPagePreview->GetDC(),
            0, 0,
            SRCCOPY );
    return;
  }
	
	CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
}



void CTextDialog::OnChangeEditTextText() 
{
  if ( m_EditText.GetSafeHwnd() == NULL )
  {
    return;
  }

  CString   cstrGnu;

  m_EditText.GetWindowText( cstrGnu );

  m_Text = GR::Convert::ToUTF8( LPCTSTR( cstrGnu ) );

  RebuildPreview();
}



void CTextDialog::OnOK() 
{
  if ( pViewInfo->m_FloatingSelection )
  {
    delete pViewInfo->m_pImageFloatingSelection;
    pViewInfo->m_pImageFloatingSelection = NULL;
    pViewInfo->m_FloatingSelection = FALSE;
  }
  // den Text als Floating Selection einsetzen
  if ( ( m_hFont != NULL )
  &&   ( m_Text.length() ) )
  {
    if ( ( m_TextWidth > 0 )
    &&   ( m_TextHeight > 0 ) )
    {
      pViewInfo->m_pImageFloatingSelection = new GR::Graphic::Image( m_TextWidth, m_TextHeight, pPagePreview->GetDepth(), 0, 0 );
      pViewInfo->m_pImageFloatingSelection->GetImage( pPagePreview, 0, 0 );
      pViewInfo->m_FloatingSelection = TRUE;
    }
  }

  pSettings->SetSetting( "TDText", m_Text );
	
	CDialog::OnOK();
}



void CTextDialog::ApplyEffects( CMaskedContextDescriptor *pCDSource, CMaskedContextDescriptor *pCDTarget,
                                int iX1, int iY1, int iX2, int iY2 )
{
  GR::u32   backColor = pPagePreview->GetRGB256( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) ),
            fontColor = pPagePreview->GetRGB256( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );

  for ( int k = iY1; k <= iY2; k++ )
  {
    for ( int l = iX1; l <= iX2; l++ )
    {
      if ( pCDSource->GetDirectPixel( l, k ) == fontColor )
      {
        // ein Font-Pixel!
        pCDTarget->PutPixel( l, k, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );
      }
    }
  }

  tListEffekte::iterator    it( m_Effects.begin() );
  while ( it != m_Effects.end() )
  {
    CTextEffekt *pEffekt = *it;

    if ( ( pEffekt->m_Type == 1 )
    &&   ( pEffekt->m_Param1 > 0 ) )
    {
      // Aura
      GR::Graphic::Image *pImageMask = new GR::Graphic::Image( pCDSource->Width(), pCDSource->Height(), 8 );

      CMaskedContextDescriptor  *pCDM = new CMaskedContextDescriptor( pImageMask, NULL );

      for ( int j = iY1; j <= iY2; j++ )
      {
        for ( int i = iX1; i <= iX2; i++ )
        {
          if ( pEffekt->m_Type == 1 )
          {
            if ( pCDSource->GetDirectPixel( i, j ) == fontColor )
            {
              pCDM->PutPixel( i, j, 1 );
            }
          }
        }
      }

      DWORD iIterationsStufe = 1;
      do
      {
        for ( int j = iY1; j <= iY2; j++ )
        {
          for ( int i = iX1; i <= iX2; i++ )
          {
            if ( pCDM->GetPixel( i, j ) == iIterationsStufe )
            {
              if ( pCDM->GetPixel( i, j - 1 ) == 0 )
              {
                pCDM->PutPixel( i, j - 1, iIterationsStufe + 1 );
              }
              if ( pCDM->GetPixel( i, j + 1 ) == 0 )
              {
                pCDM->PutPixel( i, j + 1, iIterationsStufe + 1 );
              }
              if ( pCDM->GetPixel( i - 1, j ) == 0 )
              {
                pCDM->PutPixel( i - 1, j, iIterationsStufe + 1 );
              }
              if ( pCDM->GetPixel( i + 1, j ) == 0 )
              {
                pCDM->PutPixel( i + 1, j, iIterationsStufe + 1 );
              }
            }
          }
        }
        iIterationsStufe++;
      }
      while ( iIterationsStufe < pEffekt->m_Param1 );

      {
        for ( int j = iY1 - iIterationsStufe; j <= iY2 + (int)iIterationsStufe; j++ )
        {
          for ( int i = iX1 - iIterationsStufe; i <= iX2 + (int)iIterationsStufe; i++ )
          {
            if ( pCDM->GetPixel( i, j ) > 1 )
            {
              pCDTarget->PutPixel( i, j, pEffekt->m_Color );
            }
          }
        }
      }
      delete pCDM;
      delete pImageMask;

    }

    it++;
  }
}



void CTextDialog::RebuildPreview()
{
  if ( !m_Initialized )
  {
    return;
  }

  m_TextHeight = 0;
  m_TextWidth = 0;

  HDC   hdcTarget = pPagePreview->GetDC();

  pPagePreview->Box( 0, 0, pPagePreview->GetWidth() - 1, pPagePreview->GetHeight() - 1, 
                     pPagePreview->GetRGB256( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) ) );


  RECT    rc;


  rc.left = 0;
  rc.top = 0;
  rc.right = pPagePreview->GetWidth();
  rc.bottom = pPagePreview->GetHeight();

  HFONT   hOldFont = (HFONT)::SelectObject( hdcTarget, m_hFont );

  SetBkMode( hdcTarget, TRANSPARENT );
  SetTextColor( hdcTarget, pSettings->GetColorRef( CSettings::CO_WORKCOLOR ) );
  SetBkColor( hdcTarget, pSettings->GetColorRef( CSettings::CO_WORKCOLOR_2 ) );

  ::SetMapMode( hdcTarget, MM_TEXT );

  {
    int   iX = -m_Tilt,
          iStartX = 0,
          iDeltaX = 0;

    GR::tChar  szDummy[5];

    szDummy[1] = 0;

    if ( iX < 0 )
    {
      iX = 0;
    }

    GR::Graphic::ContextDescriptor  cdPage( pPagePreview );

    for ( size_t i = 0; i < m_Text.length(); i++ )
    {
      szDummy[0] = m_Text[i];
      if ( BeginPath( hdcTarget ) )
      {
        TextOut( hdcTarget,
                 iX,
                 0,
                 szDummy,
                 1 );
        EndPath( hdcTarget );

        if ( !FlattenPath( hdcTarget ) )
        {
          dh::Log( "FlattenPath failed" );
        }

        int   iPointsInPath = GetPath( hdcTarget, NULL, NULL, 0 );

        POINT*    pPoints = new POINT[iPointsInPath];

        BYTE*     pPointTypes = new BYTE[iPointsInPath];

        int iPoints = GetPath( hdcTarget, pPoints, pPointTypes, iPointsInPath );

        if ( m_Filled )
        {
          /*
          HBRUSH  hBrush = CreateSolidBrush( pSettings->GetColorRef( CSettings::CO_WORKCOLOR ) ),
                  hOldBrush;
          hOldBrush = (HBRUSH)SelectObject( hdcTarget, hBrush );
          SetPolyFillMode( hdcTarget, ALTERNATE );

          FillPath( hdcTarget );

          SelectObject( hdcTarget, hOldBrush );
          DeleteObject( hBrush );
          */
          int   iX1, iY1, iX2, iY2,   iStartX, iStartY;

          std::list<POINT>    listPoly;

          std::vector<tEdge>    listLines;

          for ( int j = 0; j < iPointsInPath; ++j )
          {
            if ( ( pPointTypes[j] & PT_MOVETO ) == PT_MOVETO )
            {
              iX1 = pPoints[j].x;
              iY1 = pPoints[j].y;
              iStartX = iX1;
              iStartY = iY1;
            }
            else if ( ( pPointTypes[j] & PT_LINETO ) == PT_LINETO )
            {
              iX2 = pPoints[j].x;
              iY2 = pPoints[j].y;


              listLines.push_back( std::make_pair( GR::tFPoint( (float)iX1, (float)iY1 ),
                                                   GR::tFPoint( (float)iX2, (float)iY2 ) ) );
              //listLines.push_back( GR::tFPoint( (float)iX2, (float)iY2 ) );

              //pPagePreview->Line( iX1, iY1, iX2, iY2, 32767 );
              //sline( &cdPage, iX1, iY1, iX2, iY2 );
              //AntiLine( &cdPage, iX1, iY1, iX2, iY2, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );
              //cdPage.AALine( iX1, iY1, iX2, iY2, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );

              iX1 = iX2;
              iY1 = iY2;
            }
            else if ( ( pPointTypes[j] & PT_CLOSEFIGURE ) == PT_CLOSEFIGURE )
            {
              //cdPage.AALine( iX1, iY1, iStartX, iStartY, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );

              /*
              listLines.push_back( GR::tFPoint( (float)iX1, (float)iY1 ) );
              listLines.push_back( GR::tFPoint( (float)iStartX, (float)iStartY ) );
              */
              listLines.push_back( std::make_pair( GR::tFPoint( (float)iX1, (float)iY1 ),
                                                   GR::tFPoint( (float)iX2, (float)iY2 ) ) );
              //listLines.push_back( GR::tFPoint( (float)iStartX, (float)iStartY ) );

              /*
              FillAAPoly( cdPage, listLines );
              listLines.clear();
              */
            }
          }
          if ( !listLines.empty() )
          {
            CPolygonFillAA    Filler;


            Filler.FillPolygon( cdPage, listLines, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );
          }

        }
        else
        {
          int   iX1, iY1, iX2, iY2,   iStartX, iStartY;
          for ( int j = 0; j < iPointsInPath; ++j )
          {
            if ( ( pPointTypes[j] & PT_MOVETO ) == PT_MOVETO )
            {
              iX1 = pPoints[j].x;
              iY1 = pPoints[j].y;
              iStartX = iX1;
              iStartY = iY1;
            }
            else if ( ( pPointTypes[j] & PT_LINETO ) == PT_LINETO )
            {
              iX2 = pPoints[j].x;
              iY2 = pPoints[j].y;

              //pPagePreview->Line( iX1, iY1, iX2, iY2, 32767 );
              //sline( &cdPage, iX1, iY1, iX2, iY2 );
              //AntiLine( &cdPage, iX1, iY1, iX2, iY2, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );
              cdPage.AALine( iX1, iY1, iX2, iY2, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );

              iX1 = iX2;
              iY1 = iY2;
            }
            else if ( ( pPointTypes[j] & PT_CLOSEFIGURE ) == PT_CLOSEFIGURE )
            {
              cdPage.AALine( iX1, iY1, iStartX, iStartY, pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) );
            }
          }
        }


        delete[] pPointTypes;
        delete[] pPoints;

        /*
        HBRUSH  hBrush = CreateSolidBrush( pSettings->GetColorRef( CSettings::CO_WORKCOLOR ) ),
                hOldBrush;
        hOldBrush = (HBRUSH)SelectObject( hdcTarget, hBrush );
        //SetPolyFillMode( hdcTarget, ALTERNATE );

        FillPath( hdcTarget );

        SelectObject( hdcTarget, hOldBrush );
        DeleteObject( hBrush );
        */

        /*
        if ( m_bApplyEffectsSingle )
        {
          tListEffekte::iterator    it( m_listEffects.begin() );

          while ( it != m_listEffects.end() )
          {
            CTextEffekt *pEffekt = *it;

            if ( ( pEffekt->m_dwType == 1 )
            &&   ( pEffekt->m_dwParam1 > 0 ) )
            {
              // Aura
              BeginPath( pPagePreview->GetDC() );
              TextOut( pPagePreview->GetDC(),
                       iX,
                       0,
                       szDummy,
                       1 );
              EndPath( pPagePreview->GetDC() );

              HPEN  hPen = CreatePen( PS_SOLID, pEffekt->m_dwParam1, RGB( ( pEffekt->m_dwColor & 0xff0000 ) >> 16,
                                                        ( pEffekt->m_dwColor & 0x00ff00 ) >>  8,
                                                        ( pEffekt->m_dwColor & 0x0000ff ) ) ),
                    hOldPen;

              hOldPen = (HPEN)SelectObject( pPagePreview->GetDC(), hPen );
              StrokePath( pPagePreview->GetDC() );
              SelectObject( pPagePreview->GetDC(), hOldPen );
              DeleteObject( hPen );
            }


            it++;
          }
        }
        */

        /*
        TextOut( hdcTarget,
                 iX,
                 0,
                 szDummy,
                 1 );
                 */

        SIZE    stSize;
        GetTextExtentPoint32( hdcTarget,
                              szDummy,
                              1,
                              &stSize );

        // Kerning
        if ( i < m_Text.length() - 1 )
        {
          tmapKP::iterator    it( m_KerningPairs.find( ( m_Text[i] << 16 ) + m_Text[i + 1] ) );
          if ( it != m_KerningPairs.end() )
          {
            stSize.cx += it->second;
          }
        }
        iX += stSize.cx;// * m_iKerning;

        //iX = pPagePreview->GetWidth() * 50;
        //m_iTextHeight = pPagePreview->GetHeight() * 50;
        if ( m_TextHeight < stSize.cy )
        {
          m_TextHeight = stSize.cy;
        }
      }
      /*
      else
      {
        dh::Log( "beginpath failed" );
      }
      */
    }
    if ( !m_ApplyEffectsSingle )
    {
      /*
      tListEffekte::iterator    it( m_listEffects.begin() );
      while ( it != m_listEffects.end() )
      {
        CTextEffekt *pEffekt = *it;

        if ( ( pEffekt->m_dwType == 1 )
        &&   ( pEffekt->m_dwParam1 > 0 ) )
        {
          // Aura
          iX = -m_iTilt;
          if ( iX < 0 )
          {
            iX = 0;
          }

          BeginPath( pPagePreview->GetDC() );

          for ( size_t i = 0; i < m_strText.length(); i++ )
          {
            szDummy[0] = m_strText[i];
            TextOut( pPagePreview->GetDC(),
                     iX,
                     0,
                     szDummy,
                     1 );

            SIZE    stSize;
            GetTextExtentPoint32( pPagePreview->GetDC(),
                                  szDummy,
                                  1,
                                  &stSize );

            // Kerning
            if ( i < m_strText.length() - 1 )
            {
              tmapKP::iterator    it( m_KerningPairs.find( ( m_strText[i] << 16 ) + m_strText[i + 1] ) );
              if ( it != m_KerningPairs.end() )
              {
                stSize.cx += it->second;
              }
            }
            iX += stSize.cx;// * m_iKerning;
          }
          EndPath( pPagePreview->GetDC() );

          HPEN  hPen = CreatePen( PS_SOLID, pEffekt->m_dwParam1, RGB( ( pEffekt->m_dwColor & 0xff0000 ) >> 16,
                                                    ( pEffekt->m_dwColor & 0x00ff00 ) >>  8,
                                                    ( pEffekt->m_dwColor & 0x0000ff ) ) ),
                hOldPen;

          hOldPen = (HPEN)SelectObject( pPagePreview->GetDC(), hPen );
          StrokePath( pPagePreview->GetDC() );
          SelectObject( pPagePreview->GetDC(), hOldPen );
          DeleteObject( hPen );
        }


        it++;
      }
      */
    }

    /*
    for ( int i = 0; i < m_strText.length(); i++ )
    {
      rc.left = iX;
      rc.top = iY;
      rc.right = pPagePreview->GetWidth();
      rc.bottom = pPagePreview->GetHeight();

      szDummy[0] = m_strText[i];
      DrawText( pTempPage->GetDC(), 
                szDummy,
                1,
                &rc,
                DT_LEFT | DT_CALCRECT );

      if ( rc.left < iX )
      {
        OffsetRect( &rc, iX - rc.left, 0 );
      }

      iStartX = iX;
      iDeltaX = rc.right - iX;
      m_iTextHeight = rc.bottom - rc.top;
      // Kerning
      if ( i < m_strText.length() - 1 )
      {
        tmapKP::iterator    it( m_KerningPairs.find( ( m_strText[i] << 16 ) + m_strText[i + 1] ) );
        if ( it != m_KerningPairs.end() )
        {
          iDeltaX += it->second;
        }
      }
      iX += iDeltaX * m_iKerning / 100;

      // Inhalt zwischenspeichern
      if ( m_bApplyEffectsSingle )
      {
        pTempPage->Box( 0, 0, pPagePreview->GetWidth() - 1, pPagePreview->GetHeight() - 1, 
                           pPagePreview->GetRGB256( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) ) );
      }

      DrawText( pTempPage->GetDC(), 
                szDummy,
                1,
                &rc,
                DT_LEFT | DT_NOCLIP );

      // Effekte drauf und Inhalt kopieren
      if ( m_bApplyEffectsSingle )
      {
        ApplyEffects( pCDSource, pCDTarget, iStartX / 100 - 2, 0, iX / 100 + 15, m_iTextHeight );
      }
    }
    */

    /*
    if ( !m_bApplyEffectsSingle )
    {
      ApplyEffects( pCDSource, pCDTarget, 0, 0, iX / 100 + 15, m_iTextHeight );
    }
    */

    //pCDSource->CopyArea( 0, 0, pCDSource->Width(), pCDSource->Height(), 0, 0, pCDTarget );

    m_TextWidth = iX;
  }

  //hdcTarget = pPagePreview->GetDC();

  /*
  ::SetMapMode( hdcTarget, MM_TEXT );
  ::SetWindowExtEx( hdcTarget, 
                    pPagePreview->GetWidth(),
                    pPagePreview->GetHeight(),
                    NULL );
  ::SetViewportExtEx( hdcTarget, 
                    pPagePreview->GetWidth(),
                    pPagePreview->GetHeight(),
                    NULL );
                    */

  // Tilt-en
  /*
  {
    GR::Graphic::Image *pImageLine = new GR::Graphic::Image( pPagePreview->GetWidth(), 1, pPagePreview->GetDepth() );

    for ( int i = 0; i < pPagePreview->GetHeight(); i++ )
    {
      //if ( i < m_iHeight * 2 )
      if ( i < m_iHeight )
      {
        //pImageLine->GetImage( pPagePreview, -m_iTilt * ( 2 * m_iHeight - i - 1 ) / ( 2 * m_iHeight ), i );
        pImageLine->GetImage( pPagePreview, -m_iTilt * ( m_iHeight - i - 1 ) / m_iHeight, i );
        pImageLine->PutImage( pPagePreview, 0, i );

        // das Loch am linken Rand zugipsen
        //pPagePreview->Box( 0, i, m_iTilt * ( 2 * m_iHeight - i - 1 ) / ( 2 * m_iHeight ), i, pPagePreview->GetRGB256( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) ) );
        pPagePreview->Box( 0, i, m_iTilt * ( m_iHeight - i - 1 ) / ( m_iHeight ), i, pPagePreview->GetRGB256( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) ) );
      }
    }
    delete pImageLine;
  }
  */

  // Anti-Aliasing
  /*
  {
    GR::Graphic::Image *pImageBig = new GR::Graphic::Image( pPagePreview->GetWidth(), pPagePreview->GetHeight(), pPagePreview->GetDepth() );

    pImageBig->GetImage( pPagePreview, 0, 0 );

    CMaskedContextDescriptor  *pCD = new CMaskedContextDescriptor( pImageBig, pViewInfo->m_pPalette ),
                        *pCDTarget = new CMaskedContextDescriptor( pPagePreview, pViewInfo->m_pPalette );

    int   iR,
          iG,
          iB;

    DWORD   dwPixel;

    for ( int i = 0; i < pImageBig->GetWidth() / 2; i++ )
    {
      for ( int j = 0; j < pImageBig->GetHeight() / 2; j++ )
      {
        dwPixel = pCD->GetPixel( i * 2, j * 2 );

        iR = ( dwPixel & 0xff0000 ) >> 16;
        iG = ( dwPixel & 0x00ff00 ) >> 8;
        iB = ( dwPixel & 0x0000ff );

        dwPixel = pCD->GetPixel( i * 2 + 1, j * 2 );

        iR += ( dwPixel & 0xff0000 ) >> 16;
        iG += ( dwPixel & 0x00ff00 ) >> 8;
        iB += ( dwPixel & 0x0000ff );

        dwPixel = pCD->GetPixel( i * 2 + 1, j * 2 + 1 );

        iR += ( dwPixel & 0xff0000 ) >> 16;
        iG += ( dwPixel & 0x00ff00 ) >> 8;
        iB += ( dwPixel & 0x0000ff );

        dwPixel = pCD->GetPixel( i * 2, j * 2 + 1 );

        iR += ( dwPixel & 0xff0000 ) >> 16;
        iG += ( dwPixel & 0x00ff00 ) >> 8;
        iB += ( dwPixel & 0x0000ff );

        pCDTarget->PutPixel( i, j, ( ( iR >> 2 ) << 16 ) + ( ( iG >> 2 ) << 8 ) + ( ( iB >> 2 ) ) );
        //pCDTarget->PutPixel( i, j, dwPixel );
      }
    }

    delete pCDTarget;
    delete pCD;

    delete pImageBig;
  }
  */


  /*
  GetClientRect( &rc );
  ::DrawText( pPagePreview->GetDC(), m_strText.c_str(), m_strText.length(), &rc, DT_CALCRECT );
  ::DrawText( pPagePreview->GetDC(), m_strText.c_str(), m_strText.length(), &rc, 0 );

  m_iTextWidth = rc.right - rc.left;
  m_iTextHeight = rc.bottom - rc.top;
  */

  ::SelectObject( hdcTarget, hOldFont );

  pPagePreview->ReleaseDC();

  m_StaticPreview.Invalidate();
}



void CTextDialog::UpdateFont()
{
  if ( m_hFont )
  {
    m_KerningPairs.clear();

    DWORD   dwPairs = 0;

    HFONT   holdfont = (HFONT)SelectObject( pPagePreview->GetDC(), m_hFont );

    dwPairs = GetKerningPairs( pPagePreview->GetDC(),
                               0,
                               NULL );

    KERNINGPAIR *pPair = new KERNINGPAIR[dwPairs];

    GetKerningPairs( pPagePreview->GetDC(),
                     dwPairs,
                     pPair );

    for ( DWORD i = 0; i < dwPairs; i++ )
    {
      m_KerningPairs[( pPair[i].wFirst << 16 ) + ( pPair[i].wSecond )] = pPair[i].iKernAmount;
    }
    delete[] pPair;

    SelectObject( pPagePreview->GetDC(), holdfont );
  }

  RebuildPreview();
}



void CTextDialog::OnChangeEditTextKerning() 
{
  if ( m_EditKerning.GetSafeHwnd() == NULL )
  {
    return;
  }

  CString   cstrGnu;

  m_EditKerning.GetWindowText( cstrGnu );
  m_Kerning = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );

  pSettings->SetSetting( "TDFontKerning", m_Kerning );

  UpdateFont();
}



void CTextDialog::OnChangeEditTextTilt() 
{
  if ( m_EditTilt.GetSafeHwnd() == NULL )
  {
    return;
  }

  CString   cstrGnu;

  m_EditTilt.GetWindowText( cstrGnu );
  m_Tilt = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );

  pSettings->SetSetting( "TDFontTilt", m_Tilt );

  UpdateFont();
}



void CTextDialog::OnButtonEffektListe() 
{
	RECT    rc;

  GetWindowRect( &rc );

  m_Expanded = !m_Expanded;
  if ( m_Expanded )
  {
    m_ButtonToggleEffektListe.SetWindowText( _T( "Effekte <<" ) );
	  SetWindowPos( NULL, 0, 0, rc.right - rc.left, 465, SWP_NOZORDER | SWP_NOMOVE );
  }
  else
  {
    m_ButtonToggleEffektListe.SetWindowText( _T( "Effekte >>" ) );
    SetWindowPos( NULL, 0, 0, rc.right - rc.left, 328, SWP_NOZORDER | SWP_NOMOVE );
  }
}



void CTextDialog::RebuildEffektList() 
{
  m_ListEffekte.ResetContent();

  tListEffekte::iterator    it( m_Effects.begin() );
  while ( it != m_Effects.end() )
  {
    CTextEffekt* pEffekt = *it;

    GR::tString   strName;

    switch ( pEffekt->m_Type )
    {
      case 1:
        strName = _T( "Aura" );
        break;
      default:
        strName = _T( "unbekannt" );
        break;
    }

    int iItem = m_ListEffekte.AddString( strName.c_str() );
    m_ListEffekte.SetItemData( iItem, (DWORD_PTR)pEffekt );

    it++;
  }
}



void CTextDialog::OnButtonNewEffekt() 
{
	CDlgSelectEffekt    dlg;

  if ( dlg.DoModal() == IDOK )
  {
    if ( dlg.m_Effekt )
    {
      CTextEffekt *pEffekt = new CTextEffekt();

      pEffekt->m_Type = dlg.m_Effekt;

      m_Effects.push_back( pEffekt );

      RebuildEffektList();

      RebuildPreview();
    }
  }
}



void CTextDialog::OnDblclkListEffekte() 
{
	int item = m_ListEffekte.GetCurSel();
  if ( item == LB_ERR )
  {
    return;
  }
  CTextEffekt* pEffekt = (CTextEffekt*)m_ListEffekte.GetItemData( item );

  switch ( pEffekt->m_Type )
  {
    case 1:
      {
        CDlgTEAura  dlgAura;

        dlgAura.m_pEffekt = pEffekt;
        if ( dlgAura.DoModal() == IDOK )
        {
          RebuildPreview();
        }
      }
      break;
  }
}



void CTextDialog::OnCheckEffektSingle() 
{
  m_ApplyEffectsSingle = !!m_CheckEffectsSingle.GetCheck();
  RebuildPreview();
}



void CTextDialog::OnBnClickedButtonFont()
{
  CHOOSEFONT    stCF;


  memset( &stCF, 0, sizeof( stCF ) );
  stCF.lStructSize    = sizeof( stCF );
  stCF.hwndOwner      = GetSafeHwnd();
  stCF.lpLogFont      = &m_logFont;
  stCF.Flags          = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
  stCF.rgbColors      = pSettings->GetColorRef( CSettings::CO_WORKCOLOR );


  if ( ChooseFont( &stCF ) )
  {
    pSettings->SetSetting( "TDFontName", GR::Convert::ToUTF8( m_logFont.lfFaceName ) );
    pSettings->SetSetting( "TDFontWidth", m_logFont.lfWeight );
    pSettings->SetSetting( "TDFontHeight", m_logFont.lfHeight );

    if ( m_hFont )
    {
      DeleteObject( m_hFont );
    }
    m_hFont = CreateFontIndirect( &m_logFont );

    UpdateFont();
    RebuildPreview();
  }
}



void CTextDialog::OnBnClickedCheckFilled()
{
  m_Filled = ( m_CheckFilled.GetCheck() == BST_CHECKED );

  pSettings->SetSetting( "TDTextFilled", m_Filled );
  RebuildPreview();
}


