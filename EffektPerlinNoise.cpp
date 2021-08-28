#include "stdafx.h"
#include "DlgEffekte.h"
#include "EffektPerlinNoise.h"

#include <math.h>

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektPerlinNoise::CEffektPerlinNoise(CWnd* pParent /*=NULL*/)
	: EffektTemplate(CEffektPerlinNoise::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffektPerlinNoise)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEffektPerlinNoise::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektPerlinNoise)
	DDX_Control(pDX, IDC_EDIT_SCALE, m_EditScale);
	DDX_Control(pDX, IDC_EDIT_STEPS, m_EditSteps);
	DDX_Control(pDX, IDC_EDIT_SEED, m_EditSeed);
	DDX_Control(pDX, IDC_CHECK_RED_INVERSE, m_CheckRedInverse);
	DDX_Control(pDX, IDC_CHECK_RED, m_CheckRed);
	DDX_Control(pDX, IDC_CHECK_GREEN_INVERSE, m_CheckGreenInverse);
	DDX_Control(pDX, IDC_CHECK_GREEN, m_CheckGreen);
	DDX_Control(pDX, IDC_CHECK_BLUE_INVERSE, m_CheckBlueInverse);
	DDX_Control(pDX, IDC_CHECK_BLUE, m_CheckBlue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektPerlinNoise, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektPerlinNoise)
	ON_EN_CHANGE(IDC_EDIT_SEED, OnChangeEditSeed)
	ON_EN_CHANGE(IDC_EDIT_STEPS, OnChangeEditSteps)
	ON_BN_CLICKED(IDC_CHECK_BLUE, OnCheckBlue)
	ON_BN_CLICKED(IDC_CHECK_BLUE_INVERSE, OnCheckBlueInverse)
	ON_BN_CLICKED(IDC_CHECK_GREEN, OnCheckGreen)
	ON_BN_CLICKED(IDC_CHECK_GREEN_INVERSE, OnCheckGreenInverse)
	ON_BN_CLICKED(IDC_CHECK_RED, OnCheckRed)
	ON_BN_CLICKED(IDC_CHECK_RED_INVERSE, OnCheckRedInverse)
	ON_EN_CHANGE(IDC_EDIT_SCALE, OnChangeEditScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffektPerlinNoise message handlers

class CPerlinNoise
{
  protected:

    #define GRADIENT_TABLE_SIZE 256

    float   fGradients[GRADIENT_TABLE_SIZE * 3];

    void InitGradients();
    int Permutate( int iX );
    int Index( int iX, int iY, int iZ );
    float Lattice( int iX, int iY, int iZ, float fX, float fY, float fZ );
    float Lerp( float fT, float fX0, float fX1 );
    float Smooth( float fX );
    BYTE CPerlinNoise::Gradients( int iIndex );

  public:

    CPerlinNoise( int iSeed );
    float Noise( float fX, float fY, float fZ, int iSteps = 1 );

};


void CPerlinNoise::InitGradients()
{

  float fZ,
        fR,
        fTheta;


  // Generate random gradient vectors.
  for ( int i = 0; i < GRADIENT_TABLE_SIZE; ++i )
  {
    fZ = (float)( 1 - 2 * rand() / RAND_MAX );
    fR = (float)sqrt( 1 - fZ * fZ );
    fTheta = 2 * 3.1415926f * rand() / RAND_MAX;
    fGradients[i * 3]     = (float)( fR * cos( fTheta ) );
    fGradients[i * 3 + 1] = fR * sinf( fTheta );
    fGradients[i * 3 + 2] = fZ;
  }

}


BYTE CPerlinNoise::Gradients( int iIndex )
{

 static const   BYTE ucGradients[GRADIENT_TABLE_SIZE] = {
      225,155,210,108,175,199,221,144,203,116, 70,213, 69,158, 33,252,
        5, 82,173,133,222,139,174, 27,  9, 71, 90,246, 75,130, 91,191,
      169,138,  2,151,194,235, 81,  7, 25,113,228,159,205,253,134,142,
      248, 65,224,217, 22,121,229, 63, 89,103, 96,104,156, 17,201,129,
       36,  8,165,110,237,117,231, 56,132,211,152, 20,181,111,239,218,
      170,163, 51,172,157, 47, 80,212,176,250, 87, 49, 99,242,136,189,
      162,115, 44, 43,124, 94,150, 16,141,247, 32, 10,198,223,255, 72,
       53,131, 84, 57,220,197, 58, 50,208, 11,241, 28,  3,192, 62,202,
       18,215,153, 24, 76, 41, 15,179, 39, 46, 55,  6,128,167, 23,188,
      106, 34,187,140,164, 73,112,182,244,195,227, 13, 35, 77,196,185,
       26,200,226,119, 31,123,168,125,249, 68,183,230,177,135,160,180,
       12,  1,243,148,102,166, 38,238,251, 37,240,126, 64, 74,161, 40,
      184,149,171,178,101, 66, 29, 59,146, 61,254,107, 42, 86,154,  4,
      236,232,120, 21,233,209, 45, 98,193,114, 78, 19,206, 14,118,127,
       48, 79,147, 85, 30,207,219, 54, 88,234,190,122, 95, 67,143,109,
       137,214,145, 93, 92,100,245,  0,216,186, 60, 83,105, 97,204, 52 };

      return ucGradients[iIndex];

}

CPerlinNoise::CPerlinNoise( int iSeed )
{

  srand( iSeed );
  InitGradients();

}



int CPerlinNoise::Permutate( int iX )
{

  return Gradients( iX & ( GRADIENT_TABLE_SIZE - 1 ) );

}


int CPerlinNoise::Index( int iX, int iY, int iZ )
{

  return Permutate( iX + Permutate( iY + Permutate( iZ ) ) );

}



float CPerlinNoise::Lattice( int iX, int iY, int iZ, float fX, float fY, float fZ )
{

  int   iG;

  // Look up a random gradient at [ix,iy,iz] and dot it with the [fx,fy,fz] vector.
  iG = Index( iX, iY, iZ ) * 3;
  return fGradients[iG] * fX + fGradients[iG + 1] * fY + fGradients[iG + 2] * fZ;

}



float CPerlinNoise::Lerp( float fT, float fX0, float fX1 )
{

  // Simple linear interpolation.
  return fX0 + fT * ( fX1 - fX0 );

}



float CPerlinNoise::Smooth( float fX )
{
  // Smoothing curve. This is used to calculate interpolants so that the noise
  // doesn't look blocky when the frequency is low.
  return fX * fX * ( 3 - 2 * fX );

}


float CPerlinNoise::Noise( float fX, float fY, float fZ, int iSteps )
{

  int   iX,
        iY,
        iZ;

  float fX0,
        fX1,
        fY0,
        fY1,
        fZ0,
        fZ1,
        fWX,
        fWY,
        fWZ,
        fVX0,
        fVX1,
        fVY0,
        fVY1,
        fVZ0,
        fVZ1;


  // The main noise function. Looks up the pseudorandom gradients at the nearest
  // lattice points, dots them with the input vector, and interpolates the
  // results to produce a single output value in [0, 1] range. }

  iX = (int)floor( fX );
  fX0 = fX - iX;
  fX1 = fX0 - 1;
  fWX = Smooth( fX0 );

  iY = (int)floor( fY );
  fY0 = fY - iY;
  fY1 = fY0 - 1;
  fWY = Smooth( fY0 );

  iZ = (int)floor( fZ );
  fZ0 = fZ - iZ;
  fZ1 = fZ0 - 1;
  fWZ = Smooth( fZ0 );

  fVX0 = Lattice( iX, iY, iZ, fX0, fY0, fZ0 );
  fVX1 = Lattice( iX + 1, iY, iZ, fX1, fY0, fZ0 );
  fVY0 = Lerp( fWX, fVX0, fVX1 );

  fVX0 = Lattice( iX, iY + 1, iZ, fX0, fY1, fZ0 );
  fVX1 = Lattice( iX + 1, iY + 1, iZ, fX1, fY1, fZ0 );
  fVY1 = Lerp( fWX, fVX0, fVX1 );

  fVZ0 = Lerp( fWY, fVY0, fVY1 );

  fVX0 = Lattice( iX, iY, iZ + 1, fX0, fY0, fZ1 );
  fVX1 = Lattice( iX + 1, iY, iZ + 1, fX1, fY0, fZ1 );
  fVY0 = Lerp( fWX, fVX0, fVX1 );

  fVX0 = Lattice( iX, iY + 1, iZ + 1, fX0, fY1, fZ1 );
  fVX1 = Lattice( iX + 1, iY + 1, iZ + 1, fX1, fY1, fZ1 );
  fVY1 = Lerp( fWX, fVX0, fVX1 );

  fVZ1 = Lerp( fWY, fVY0, fVY1 );

  float   fResult = Lerp( fWZ, fVZ0, fVZ1 );

  if ( iSteps >= 1 )
  {
    fResult += Noise( fX * 2.0f, fY * 2.0f, fZ * 2.0f, iSteps - 1 ) / 2.0f;
  }
  return fResult;

}



/*-DoEffekt-------------------------------------------------------------------+
 |                                                                            |
 +----------------------------------------------------------------------------*/

void CEffektPerlinNoise::DoEffekt()
{

  CPerlinNoise      aNoise( m_iSeed );

  int iWidth   = m_pCDSource->Width(),
      iHeight  = m_pCDSource->Height();

  int iR,
      iG,
      iB;

  for ( int i = 0; i < iWidth; i++ )
  {
    for ( int j = 0; j < iHeight; j++ )
    {
      float nf = aNoise.Noise( (float)i / m_fScale, (float)j / m_fScale, 0, m_iSteps );
      // Write the result to the texture image.
      int n = int( 255 * ( nf + 1 ) / 2 );
      iR = 0;
      iG = 0;
      iB = 0;
      if ( m_bRed )
      {
        if ( m_bRedInverse )
        {
          iR = 255 - n;
        }
        else
        {
          iR = n;
        }
      }
      if ( m_bGreen )
      {
        if ( m_bGreenInverse )
        {
          iG = 255 - n;
        }
        else
        {
          iG = n;
        }
      }
      if ( m_bBlue )
      {
        if ( m_bBlueInverse )
        {
          iB = 255 - n;
        }
        else
        {
          iB = n;
        }
      }
      m_pCDTarget->PutPixel( i, j, ( iR << 16 ) + ( iG << 8 ) + iB );
    }
    if ( !m_pDlg->UpdateThread() )
    {
      return;
    }
  }

}



BOOL CEffektPerlinNoise::OnInitDialog() 
{

	EffektTemplate::OnInitDialog();

  m_DoNotUpdate = TRUE;

  m_iSteps          = 5;
  m_EditSteps.SetWindowText( _T( "5" ) );
  m_iSeed           = 17;
  m_EditSeed.SetWindowText( _T( "17" ) );
  m_fScale          = 2.0f;
  m_EditScale.SetWindowText( _T( "2.0" ) );

  m_bGreen          = true;
  m_CheckGreen.SetCheck( 1 );
  m_bGreenInverse   = false;
  m_bRed            = true;
  m_CheckRed.SetCheck( 1 );
  m_bRedInverse     = false;
  m_bBlue           = true;
  m_CheckBlue.SetCheck( 1 );
  m_bBlueInverse    = false;

  m_DoNotUpdate = FALSE;
	
	return TRUE;

}

void CEffektPerlinNoise::OnChangeEditSeed() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString cstrGnu;
  m_EditSeed.GetWindowText( cstrGnu );
  m_iSeed = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );

  RestartRendering();
	
}

void CEffektPerlinNoise::OnChangeEditSteps() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString cstrGnu;
  m_EditSteps.GetWindowText( cstrGnu );
  m_iSteps = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );

  RestartRendering();
	
}

void CEffektPerlinNoise::OnCheckBlue() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  if ( m_CheckBlue.GetCheck() )
  {
    m_bBlue = true;
  }
  else
  {
    m_bBlue = false;
  }
  RestartRendering();
	
}

void CEffektPerlinNoise::OnCheckBlueInverse() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  if ( m_CheckBlueInverse.GetCheck() )
  {
    m_bBlueInverse = true;
  }
  else
  {
    m_bBlueInverse = false;
  }
  RestartRendering();
	
}

void CEffektPerlinNoise::OnCheckGreen() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  if ( m_CheckGreen.GetCheck() )
  {
    m_bGreen = true;
  }
  else
  {
    m_bGreen = false;
  }
  RestartRendering();
	
}

void CEffektPerlinNoise::OnCheckGreenInverse() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  if ( m_CheckGreenInverse.GetCheck() )
  {
    m_bGreenInverse = true;
  }
  else
  {
    m_bGreenInverse = false;
  }
  RestartRendering();
	
}

void CEffektPerlinNoise::OnCheckRed() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  if ( m_CheckRed.GetCheck() )
  {
    m_bRed = true;
  }
  else
  {
    m_bRed = false;
  }
  RestartRendering();
	
}

void CEffektPerlinNoise::OnCheckRedInverse() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  if ( m_CheckRedInverse.GetCheck() )
  {
    m_bRedInverse = true;
  }
  else
  {
    m_bRedInverse = false;
  }
  RestartRendering();
	
}

void CEffektPerlinNoise::OnChangeEditScale() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString cstrGnu;
  m_EditScale.GetWindowText( cstrGnu );
  m_fScale = GR::Convert::ToF32( LPCTSTR( cstrGnu ) );
  if ( m_fScale <= 0.0f )
  {
    m_fScale = 1.0f;
  }

  RestartRendering();
	
}
