#include "stdafx.h"
#include "DlgEffekte.h"
#include "EffektBevel.h"
#include "Settings.h"

#include <Grafik\ContextDescriptor.h>

#include <Misc/Misc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektBevel::CEffektBevel(CWnd* pParent /*=NULL*/)
	: EffektTemplate(CEffektBevel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffektBevel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEffektBevel::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektBevel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektBevel, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektBevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CEffektBevel::DoEffekt()
{
  GR::Graphic::Image             fillImage( m_pCDSource->Width(), m_pCDSource->Height(), 15 );

  GR::Graphic::ContextDescriptor    cdFill( &fillImage );

  bool                  bChanged = false;

  int                   iStep = 1,
                        iOldValue;

  BYTE*                 pMask = (BYTE*)m_pCDSource->m_pMaskData;

  if ( pMask == NULL )
  {
    pMask = new BYTE[m_pCDSource->Width() * m_pCDSource->Height()];
    memset( pMask, 255, m_pCDSource->Width() * m_pCDSource->Height() );
  }


  cdFill.Box( 0, 0, cdFill.Width(), cdFill.Height(), 0 );

  // initialisieren
  for ( int i = 0; i < cdFill.Width(); i++ )
  {
    for ( int j = 0; j < cdFill.Height(); j++ )
    {
      if ( pMask[i + j * cdFill.Width()] )
      {
        if ( ( ( j > 0 )
        &&     ( pMask[i + ( j - 1 ) * cdFill.Width()] == 0 ) )
        ||   ( ( j < cdFill.Height() - 1 )
        &&     ( pMask[i + ( j + 1 ) * cdFill.Width()] == 0 ) )
        ||   ( ( i > 0 )
        &&     ( pMask[i - 1 + j * cdFill.Width()] == 0 ) )
        ||   ( ( i < cdFill.Width() - 1 )
        &&     ( pMask[i + 1 + j * cdFill.Width()] == 0 ) )
        ||   ( i == 0 )
        ||   ( j == 0 )
        ||   ( i == cdFill.Width() - 1 )
        ||   ( j == cdFill.Height() - 1 ) )
        {
          cdFill.PutDirectPixel( i, j, 1 );
        }
      }
    }
  }

  do
  {
    bChanged = false;
    iStep++;
    int   iCount = 0;
    for ( int i = 0; i < cdFill.Width(); i++ )
    {
      for ( int j = 0; j < cdFill.Height(); j++ )
      {
        if ( pMask[i + j * cdFill.Width()] )
        {
          // ein Teil der Selektion
          iOldValue = cdFill.GetDirectPixel( i, j );
          if ( iOldValue == iStep - 1 )
          {
            if ( ( j > 0 )
            &&   ( pMask[i + ( j - 1 ) * cdFill.Width()] )
            &&   ( cdFill.GetDirectPixel( i, j - 1 ) == 0 ) )
            {
              cdFill.PutDirectPixel( i, j - 1, iStep );
              bChanged = true;
              iCount++;
            }
            if ( ( j < cdFill.Height() - 1 )
            &&   ( pMask[i + ( j + 1 ) * cdFill.Width()] )
            &&   ( cdFill.GetDirectPixel( i, j + 1 ) == 0 ) )
            {
              cdFill.PutDirectPixel( i, j + 1, iStep );
              bChanged = true;
              iCount++;
            }
            if ( ( i > 0 )
            &&   ( pMask[i - 1 + j * cdFill.Width()] )
            &&   ( cdFill.GetDirectPixel( i - 1, j ) == 0 ) )
            {
              cdFill.PutDirectPixel( i - 1, j, iStep );
              bChanged = true;
              iCount++;
            }
            if ( ( i < cdFill.Width() - 1 )
            &&   ( pMask[i + 1 + j * cdFill.Width()] )
            &&   ( cdFill.GetDirectPixel( i + 1, j ) == 0 ) )
            {
              cdFill.PutDirectPixel( i + 1, j, iStep );
              bChanged = true;
              iCount++;
            }
          }
        }
      }
    }
  }
  while ( bChanged );

  int   iR[3],
        iG[3],
        iB[3];

  iR[0] = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) & 0xff0000 ) >> 16 );
  iG[0] = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) & 0x00ff00 ) >> 8 );
  iB[0] = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR ) & 0x0000ff ) );
  iR[1] = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) & 0xff0000 ) >> 16 );
  iG[1] = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) & 0x00ff00 ) >> 8 );
  iB[1] = ( ( pSettings->GetRGBColor( CSettings::CO_WORKCOLOR_2 ) & 0x0000ff ) );


  for ( int i = 0; i < cdFill.Width(); i++ )
  {
    for ( int j = 0; j < cdFill.Height(); j++ )
    {
      if ( pMask[i + j * cdFill.Width()] )
      {
        iOldValue = cdFill.GetDirectPixel( i, j );
        if ( iOldValue )
        {
          iR[2] = ( iR[0] * iOldValue + iR[1] * ( iStep - iOldValue ) ) / iStep;
          iG[2] = ( iG[0] * iOldValue + iG[1] * ( iStep - iOldValue ) ) / iStep;
          iB[2] = ( iB[0] * iOldValue + iB[1] * ( iStep - iOldValue ) ) / iStep;

          m_pCDTarget->PutPixel( i, j, ( iR[2] << 16 ) + ( iG[2] << 8 ) + iB[2] );
        }
      }
    }
  }

  if ( !m_pCDSource->m_pMaskData )
  {
    delete[] pMask;
  }

  if ( !m_pDlg->UpdateThread() )
  {
    return;
  }
}



BOOL CEffektBevel::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();

  m_DoNotUpdate = TRUE;

  m_DoNotUpdate = FALSE;
	
	return TRUE;
}
