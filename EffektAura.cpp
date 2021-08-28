#include "stdafx.h"
#include "DlgEffekte.h"
#include "EffektAura.h"
#include "EffektTemplate.h"
#include "Settings.h"
#include "PainterGFXTools.h"

#include <String/Convert.h>
#include ".\effektaura.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektAura::CEffektAura(CWnd* pParent /*=NULL*/)
: EffektTemplate(CEffektAura::IDD,pParent)
{
	//{{AFX_DATA_INIT(CEffektAura)
	m_AuraTiefe = 0;
	//}}AFX_DATA_INIT
}


void CEffektAura::DoDataExchange(CDataExchange* pDX)
{
  EffektTemplate::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CEffektAura)
  DDX_Control(pDX, IDC_COMBO_AURA_ART, m_ComboType);
  DDX_Control(pDX, IDC_EDIT_TIEFE, m_EditTiefe);
  DDX_Text(pDX, IDC_EDIT_TIEFE, m_AuraTiefe);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_CHECK_ROUNDED, m_CheckRounded);
}


BEGIN_MESSAGE_MAP(CEffektAura, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektAura)
	ON_EN_CHANGE(IDC_EDIT_TIEFE, OnChangeEditTiefe)
	ON_CBN_SELCHANGE(IDC_COMBO_AURA_ART, OnSelchangeComboAuraArt)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_CHECK_ROUNDED, OnBnClickedCheckRounded)
END_MESSAGE_MAP()



void CEffektAura::DoEffekt()
{
  bool        bRounded = m_AuraRounded;

  BOOL        bFound;

  int         i,
              j,
              k,
              l,
              iFaktor,
              iR,
              iG,
              iB,
              iR2,
              iG2,
              iB2;


  iFaktor = m_AuraTiefe;
  if ( iFaktor < 1 )
  {
    iFaktor = 1;
  }
  if ( iFaktor > 100 )
  {
    iFaktor = 100;
  }

  iR = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) & 0xff0000 ) >> 16 );
  iG = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) & 0xff00 ) >> 8 );
  iB = ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) & 0xff );

  iR2 = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) & 0xff0000 ) >> 16 );
  iG2 = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) & 0xff00 ) >> 8 );
  iB2 = ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) & 0xff );

  GR::u32         dwTransparent = m_pCDSource->TransparentColor();

  for ( j = 0; j < m_pCDSource->Height(); j++ )
  {
    for ( i = 0; i < m_pCDSource->Width(); i++ )
    {
      // PFUI - Transparente Farbe des m_vectLayers statt 0!
      if ( m_pCDSource->GetPixel( i, j ) == dwTransparent )
      {
        // nur an leeren Stellen malen
        // nächstes vorhandenes Pixel suchen

        int     iX = -1;
        int     iY = -1;

        GR::f32 fClosestDistance = 500000000.0f;

        l = 0;
        do
        {
          l++;
          bFound = FALSE;

          for ( k = i - l; k <= i + l; k++ )
          {
            if ( m_pCDSource->GetPixel( k, j - l ) != dwTransparent )
            {
              bFound = TRUE;

              GR::f32   fDistance = sqrtf( (float)( ( k - i ) * ( k - i ) + l * l ) );
              if ( fDistance < fClosestDistance )
              {
                fClosestDistance = fDistance;
                iX = k;
                iY = j - l;
              }
            }
            if ( m_pCDSource->GetPixel( k, j + l ) != dwTransparent )
            {
              bFound = TRUE;
              GR::f32   fDistance = sqrtf( (float)( ( k - i ) * ( k - i ) + l * l ) );
              if ( fDistance < fClosestDistance )
              {
                fClosestDistance = fDistance;
                iX = k;
                iY = j + l;
              }
            }
          }
          for ( k = j - l; k <= j + l; k++ )
          {
            if ( m_pCDSource->GetPixel( i - l, k ) != dwTransparent )
            {
              bFound = TRUE;
              GR::f32   fDistance = sqrtf( (float)( ( k - j ) * ( k - j ) + l * l ) );
              if ( fDistance < fClosestDistance )
              {
                fClosestDistance = fDistance;
                iX = i - l;
                iY = k;
              }
            }
            if ( m_pCDSource->GetPixel( i + l, k ) != dwTransparent )
            {
              bFound = TRUE;
              GR::f32   fDistance = sqrtf( (float)( ( k - j ) * ( k - j ) + l * l ) );
              if ( fDistance < fClosestDistance )
              {
                fClosestDistance = fDistance;
                iX = i + l;
                iY = k;
              }
            }
          }
          if ( l > iFaktor )
          {
            break;
          }
        }
        while ( !bFound );

        if ( !bRounded )
        {
          fClosestDistance = (GR::f32)l;
        }

        if ( fClosestDistance <= iFaktor )
        {
          // Aura berechnen
          switch ( m_AuraType )
          {
            case 0:
              m_pCDTarget->PutPixel( i, j,
                                     ( (int)( iR * ( iFaktor - fClosestDistance ) / iFaktor ) << 16 ) + ( (int)( iG * ( iFaktor - fClosestDistance ) / iFaktor ) << 8 ) + (int)( iB * ( iFaktor - fClosestDistance ) / iFaktor ) );
              break;
            case 1:
              {
                m_pCDTarget->PutPixel( i, j,
                    ( ( (int)( iR * ( iFaktor - fClosestDistance ) + iR2 * fClosestDistance ) / iFaktor ) << 16 )
                  + ( ( (int)( iG * ( iFaktor - fClosestDistance ) + iG2 * fClosestDistance ) / iFaktor ) << 8 )
                  +   ( (int)( iB * ( iFaktor - fClosestDistance ) + iB2 * fClosestDistance ) / iFaktor ) );
              }
              break;
            case 2:
              {
                m_pCDTarget->PutPixel( i, j, ( iR << 16 ) + ( iG << 8 ) + iB );
              }
              break;
          }
        }
        else
        {
          m_pCDTarget->PutPixel( i, j, 0 );
        }
      }
      else
      {
        m_pCDTarget->PutPixel( i, j, m_pCDSource->GetPixel( i, j ) );
      }
      // gelegentliches Bild-Update
      if ( !m_pDlg->UpdateThread() )
      {
        return;
      }
    }
  }
}



void CEffektAura::OnChangeEditTiefe() 
{
  CString   cstrGnu;

  m_EditTiefe.GetWindowText( cstrGnu );

  m_AuraTiefe = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );

  RestartRendering();
}



BOOL CEffektAura::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();

  m_DoNotUpdate = TRUE;

  m_AuraTiefe = 5;
  m_EditTiefe.SetWindowText( _T( "5" ) );

  m_AuraType = 0;
  m_ComboType.AddString( _T( "Ausfaden zu schwarz" ) );
  m_ComboType.AddString( _T( "Ausfaden zu 2. Farbe" ) );
  m_ComboType.AddString( _T( "kein Ausfaden" ) );
  m_ComboType.SetCurSel( 0 );
  m_AuraRounded = false;

  m_DoNotUpdate = FALSE;
	
	return TRUE;
}



void CEffektAura::OnSelchangeComboAuraArt() 
{
  m_AuraType = m_ComboType.GetCurSel();

  RestartRendering();
}



void CEffektAura::OnBnClickedCheckRounded()
{
  m_AuraRounded = ( m_CheckRounded.GetCheck() & BST_CHECKED );
  RestartRendering();
}
