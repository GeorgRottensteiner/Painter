#include "stdafx.h"
#include "EffektShadow.h"
#include "DlgEffekte.h"

#include <String/Convert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CEffektShadow::CEffektShadow(CWnd* pParent /*=NULL*/)
	: EffektTemplate(CEffektShadow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffektShadow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEffektShadow::DoDataExchange(CDataExchange* pDX)
{
  EffektTemplate::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CEffektShadow)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  DDX_Control(pDX, IDC_REDIT_SHADOW_RANGE, m_REditShadowRange);
  DDX_Control(pDX, IDC_REDIT_SHADOW_OFFSET_X, m_REditShadowOffsetX);
  DDX_Control(pDX, IDC_REDIT_SHADOW_OFFSET_Y, m_REditShadowOffsetY);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektShadow, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektShadow)
  ON_EN_CHANGE(IDC_REDIT_SHADOW_RANGE, OnChangeREditShadowRange)
  ON_EN_CHANGE(IDC_REDIT_SHADOW_OFFSET_X, OnChangeREditShadowOffsetX)
  ON_EN_CHANGE(IDC_REDIT_SHADOW_OFFSET_Y, OnChangeREditShadowOffsetY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEffektShadow::OnChangeREditShadowRange()
{

  GR::tChar  szTemp[MAX_PATH];
  
  m_REditShadowRange.GetWindowText( szTemp, MAX_PATH );
  m_iShadowRange = GR::Convert::ToI32( szTemp );

  RestartRendering();

}

void CEffektShadow::OnChangeREditShadowOffsetX()
{

  GR::tChar  szTemp[MAX_PATH];
  
  m_REditShadowOffsetX.GetWindowText( szTemp, MAX_PATH );
  m_iShadowOffsetX = GR::Convert::ToI32( szTemp );

  RestartRendering();

}

void CEffektShadow::OnChangeREditShadowOffsetY()
{

  GR::tChar  szTemp[MAX_PATH];
  
  m_REditShadowOffsetY.GetWindowText( szTemp, MAX_PATH );
  m_iShadowOffsetY = GR::Convert::ToI32( szTemp );

  RestartRendering();
  
}



void CEffektShadow::DoEffekt()
{

  m_pCDTarget->Box( 0, 0, m_pCDTarget->Width(), m_pCDTarget->Height(), pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR_2 ) );
  for ( int j = 0; j < m_pCDSource->Height(); j++ )
  {
    for ( int i = 0; i < m_pCDSource->Width(); i++ )
    {
      //if ( m_pCDSource->GetPixel( i, j ) != dwBackColor )
      if ( m_pCDSource->GetMaskValueAt( i, j ) )
      {
        if ( m_iShadowRange )
        {
          for ( int k = -m_iShadowRange; k <= m_iShadowRange; ++k )
          {
            for ( int l = -m_iShadowRange; l <= m_iShadowRange; ++l )
            {
              m_pCDTarget->PutPixelAlpha( m_iShadowOffsetX + i + k, 
                                          m_iShadowOffsetY + j + l, 
                                          pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR ),
                                          255 - 255 * abs( k ) * abs( l ) / ( ( m_iShadowRange + 1 ) * ( m_iShadowRange + 1 ) ) );
            }
          }
        }
        else
        {
          m_pCDTarget->PutPixel( m_iShadowOffsetX + i, m_iShadowOffsetY + j, pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR ) );
        }
      }
    }
    // gelegentliches Bild-Update
    if ( !m_pDlg->UpdateThread() )
    {
      return;
    }
  }

  /*
  BOOL        bFound;

  int         i,
              j,
              k,
              l,
              iFaktor,
              iR,
              iG,
              iB;


  iFaktor = m_iAuraTiefe;
  if ( iFaktor < 1 )
  {
    iFaktor = 1;
  }
  if ( iFaktor > 100 )
  {
    iFaktor = 100;
  }

  iR = ( ( pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR ) & 0xff0000 ) >> 16 );
  iG = ( ( pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR ) & 0xff00 ) >> 8 );
  iB = ( pSettings->GetRGBColor( CSettings::ColorCategory::WORKCOLOR ) & 0xff );

  for ( j = 0; j < m_pCDSource->GetHeight(); j++ )
  {
    for ( i = 0; i < m_pCDSource->GetWidth(); i++ )
    {
      // PFUI - Transparente Farbe des m_vectLayers statt 0!
      if ( m_pCDSource->GetPixel( i, j ) == 0 )
      {
        // nur an leeren Stellen malen
        // nächstes vorhandenes Pixel suchen
        l = 0;
        do
        {
          l++;
          bFound = FALSE;

          for ( k = i - l; k <= i + l; k++ )
          {
            if ( m_pCDSource->GetPixel( k, j - l ) != 0 )
            {
              bFound = TRUE;
            }
            if ( m_pCDSource->GetPixel( k, j + l ) != 0 )
            {
              bFound = TRUE;
            }
          }
          for ( k = j - l; k <= j + l; k++ )
          {
            if ( m_pCDSource->GetPixel( i - l, k ) != 0 )
            {
              bFound = TRUE;
            }
            if ( m_pCDSource->GetPixel( i + l, k ) != 0 )
            {
              bFound = TRUE;
            }
          }
          if ( l >= iFaktor )
          {
            break;
          }
        }
        while ( !bFound );
        if ( l <= iFaktor )
        {
          // Aura berechnen
          m_pCDTarget->PutPixel( i, j,
                                 ( ( iR * ( iFaktor - l ) / iFaktor ) << 16 ) + ( ( iG * ( iFaktor - l ) / iFaktor ) << 8 ) + ( iB * ( iFaktor - l ) / iFaktor ) );
        }
        else
        {
          m_pCDTarget->PutPixel( i, j, 0 );
        }
      }
      else
      {
        m_pCDTarget->PutPixel( i, j, 0 );
      }
      // gelegentliches Bild-Update
      if ( !m_pDlg->UpdateThread() )
      {
        return;
      }
    }
  }
  */

}




BOOL CEffektShadow::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();
	
	m_iShadowRange    = 5;
  m_iShadowOffsetX  = 3;
  m_iShadowOffsetY  = 3;

  m_DoNotUpdate = true;

  m_REditShadowRange.SetRange( 0, 20 );
  m_REditShadowOffsetX.SetRange( -100, 100 );
  m_REditShadowOffsetY.SetRange( -100, 100 );

  m_DoNotUpdate = false;
	
	return TRUE;

}
