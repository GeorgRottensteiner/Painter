#include "stdafx.h"
#include "EffektSmoothen.h"
#include "DlgEffekte.h"

#include <Misc/Misc.h>

#include <String/Convert.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CEffektSmoothen::CEffektSmoothen(CWnd* pParent /*=NULL*/)
	: EffektTemplate(IDD,pParent)
{
	//{{AFX_DATA_INIT(CEffektSmoothen)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CEffektSmoothen::DoDataExchange(CDataExchange* pDX)
{
  EffektTemplate::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CEffektSmoothen)
  // HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_EDIT_MATRIX_11, m_EditMatrix11);
  DDX_Control(pDX, IDC_EDIT_MATRIX_21, m_EditMatrix21);
  DDX_Control(pDX, IDC_EDIT_MATRIX_31, m_EditMatrix31);
  DDX_Control(pDX, IDC_EDIT_MATRIX_12, m_EditMatrix12);
  DDX_Control(pDX, IDC_EDIT_MATRIX_22, m_EditMatrix22);
  DDX_Control(pDX, IDC_EDIT_MATRIX_32, m_EditMatrix32);
  DDX_Control(pDX, IDC_EDIT_MATRIX_13, m_EditMatrix13);
  DDX_Control(pDX, IDC_EDIT_MATRIX_23, m_EditMatrix23);
  DDX_Control(pDX, IDC_EDIT_MATRIX_33, m_EditMatrix33);
}


BEGIN_MESSAGE_MAP(CEffektSmoothen, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektSmoothen)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
  ON_EN_CHANGE(IDC_EDIT_MATRIX_11, OnEnChangeEditMatrix11)
  ON_EN_CHANGE(IDC_EDIT_MATRIX_12, OnEnChangeEditMatrix12)
  ON_EN_CHANGE(IDC_EDIT_MATRIX_13, OnEnChangeEditMatrix13)
  ON_EN_CHANGE(IDC_EDIT_MATRIX_21, OnEnChangeEditMatrix21)
  ON_EN_CHANGE(IDC_EDIT_MATRIX_22, OnEnChangeEditMatrix22)
  ON_EN_CHANGE(IDC_EDIT_MATRIX_23, OnEnChangeEditMatrix23)
  ON_EN_CHANGE(IDC_EDIT_MATRIX_31, OnEnChangeEditMatrix31)
  ON_EN_CHANGE(IDC_EDIT_MATRIX_32, OnEnChangeEditMatrix32)
  ON_EN_CHANGE(IDC_EDIT_MATRIX_33, OnEnChangeEditMatrix33)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEffektSmoothen 



void CEffektSmoothen::DoEffekt()
{

  int         i,
              j,
              k,
              l,
              iDummy,
              iR,
              iG,
              iB,
              iCount;

  DWORD       dwPixel;


  for ( j = 0; j < m_pCDSource->Height(); j++ )
  {
    for ( i = 0; i < m_pCDSource->Width(); i++ )
    {
      iR = 0;
      iG = 0;
      iB = 0;
      iCount = 0;
      for ( k = 0; k < 3; k++ )
      {
        for ( l = 0; l < 3; l++ )
        {
          if ( ( i + k - 1 >= 0 )
          &&   ( i + k - 1 < m_pCDSource->Width() )
          &&   ( j + l - 1 >= 0 )
          &&   ( j + l - 1 < m_pCDSource->Height() )
          &&   ( m_iMatrixValue[k][l] ) )
          {
            iDummy = m_iMatrixValue[k][l];

            dwPixel = m_pCDSource->GetPixel( i + k - 1, j + l - 1 );
            iR += iDummy * ( ( dwPixel & 0xff0000 ) >> 16 );
            iG += iDummy * ( ( dwPixel & 0xff00 ) >> 8 );
            iB += iDummy * ( dwPixel & 0xff );

            iCount += abs( iDummy );
          }
        }
      }
      if ( iCount > 0 )
      {
        iR = iR / iCount;
        iG = iG / iCount;
        iB = iB / iCount;

        if ( iR < 0 )
        {
          iR = 0;
        }
        if ( iG < 0 )
        {
          iG = 0;
        }
        if ( iB < 0 )
        {
          iB = 0;
        }

        m_pCDTarget->PutPixel( i, j, ( iR << 16 ) + ( iG << 8 ) + iB );
      }
      else
      {
        m_pCDTarget->PutPixel( i, j, 0 );
      }
    }

    // gelegentliches Bild-Update
    if ( !m_pDlg->UpdateThread() )
    {
      return;
    }
  }

}

void CEffektSmoothen::OnEnChangeEditMatrix11()
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditMatrix11.GetWindowText( cstrGnu );

  m_iMatrixValue[0][0] = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();

}

void CEffektSmoothen::OnEnChangeEditMatrix12()
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditMatrix12.GetWindowText( cstrGnu );

  m_iMatrixValue[1][0] = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();

}

void CEffektSmoothen::OnEnChangeEditMatrix13()
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditMatrix13.GetWindowText( cstrGnu );

  m_iMatrixValue[2][0] = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();

}

void CEffektSmoothen::OnEnChangeEditMatrix21()
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditMatrix21.GetWindowText( cstrGnu );

  m_iMatrixValue[0][1] = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();

}

void CEffektSmoothen::OnEnChangeEditMatrix22()
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditMatrix22.GetWindowText( cstrGnu );

  m_iMatrixValue[1][1] = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();

}

void CEffektSmoothen::OnEnChangeEditMatrix23()
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditMatrix23.GetWindowText( cstrGnu );

  m_iMatrixValue[2][1] = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();

}

void CEffektSmoothen::OnEnChangeEditMatrix31()
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditMatrix31.GetWindowText( cstrGnu );

  m_iMatrixValue[0][2] = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();

}

void CEffektSmoothen::OnEnChangeEditMatrix32()
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditMatrix32.GetWindowText( cstrGnu );

  m_iMatrixValue[1][2] = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();

}

void CEffektSmoothen::OnEnChangeEditMatrix33()
{

  if ( m_DoNotUpdate )
  {
    return;
  }
  CString   cstrGnu;

  m_EditMatrix33.GetWindowText( cstrGnu );

  m_iMatrixValue[2][2] = GR::Convert::ToI32( (LPCTSTR)cstrGnu );

  RestartRendering();

}

BOOL CEffektSmoothen::OnInitDialog()
{

  EffektTemplate::OnInitDialog();


  m_iMatrixValue[0][0] = 1;
  m_iMatrixValue[1][0] = 3;
  m_iMatrixValue[2][0] = 1;
  m_iMatrixValue[0][1] = 3;
  m_iMatrixValue[1][1] = 8;
  m_iMatrixValue[2][1] = 3;
  m_iMatrixValue[0][2] = 1;
  m_iMatrixValue[1][2] = 3;
  m_iMatrixValue[2][2] = 1;

  m_DoNotUpdate = true;

  m_EditMatrix11.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_iMatrixValue[0][0] ) ).c_str() );
  m_EditMatrix12.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_iMatrixValue[1][0] ) ).c_str() );
  m_EditMatrix13.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_iMatrixValue[2][0] ) ).c_str() );
  m_EditMatrix21.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_iMatrixValue[0][1] ) ).c_str() );
  m_EditMatrix22.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_iMatrixValue[1][1] ) ).c_str() );
  m_EditMatrix23.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_iMatrixValue[2][1] ) ).c_str() );
  m_EditMatrix31.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_iMatrixValue[0][2] ) ).c_str() );
  m_EditMatrix32.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_iMatrixValue[1][2] ) ).c_str() );
  m_EditMatrix33.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_iMatrixValue[2][2] ) ).c_str() );

  m_DoNotUpdate = false;

  return TRUE;

}
