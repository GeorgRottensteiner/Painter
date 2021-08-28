#include "stdafx.h"
#include "EffektReduce.h"
#include "ImagePackage.h"
#include "ColorReducer.h"

#include "PainterGFXTools.h"

#include <Misc/Misc.h>

#include <String/Convert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CEffektReduce::CEffektReduce(CWnd* pParent /*=NULL*/)
	: EffektTemplate(CEffektReduce::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffektReduce)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CEffektReduce::DoDataExchange(CDataExchange* pDX)
{
	EffektTemplate::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffektReduce)
	DDX_Control(pDX, IDC_EDIT_COLORS, m_EditColors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffektReduce, EffektTemplate)
	//{{AFX_MSG_MAP(CEffektReduce)
	ON_EN_CHANGE(IDC_EDIT_COLORS, OnChangeEditColors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEffektReduce 

void CEffektReduce::OnChangeEditColors() 
{

  if ( m_DoNotUpdate )
  {
    return;
  }

  CString   cstrGnu;

  m_EditColors.GetWindowText( cstrGnu );
  m_dwColors = GR::Convert::ToI32( LPCTSTR( cstrGnu ) );
  if ( m_dwColors < 1 )
  {
    m_dwColors = 1;
  }
  if ( m_dwColors >= 256 )
  {
    m_dwColors = 256;
  }

  RestartRendering();
	
}



BOOL CEffektReduce::OnInitDialog() 
{
	EffektTemplate::OnInitDialog();
	
  m_DoNotUpdate = TRUE;

	m_dwColors = 256;
  m_EditColors.SetWindowText( _T( "256" ) );

  m_DoNotUpdate = FALSE;
	
	return TRUE;
}



void CEffektReduce::DoEffekt()
{

  CPainterImagePackage *pPackSource = new CPainterImagePackage(),
                    *pPackTarget = NULL;

  pPackSource->m_pImage = m_pCDSource->CreateImage();
  if ( m_pCDSource->Palette().Data() )
  {
    pPackSource->m_Palette = m_pCDSource->Palette();
  }

  CColorReducer           Reducer( m_dwColors );

  Reducer.AddSourceToColorCube( pPackSource );
  Reducer.Calculate();
  pPackTarget = Reducer.Reduce( pPackSource );


  CMaskedContextDescriptor *pCD = new CMaskedContextDescriptor( pPackTarget->m_pImage, &pPackTarget->m_Palette );

  if ( pPackTarget )
  {
    for ( int i = 0; i < m_pCDTarget->Width(); i++ )
    {
      for ( int j = 0; j < m_pCDTarget->Height(); j++ )
      {
        m_pCDTarget->PutPixel( i, j, pCD->GetPixel( i, j ) );
      }
    }
  }

  delete pCD;
  SafeDelete( pPackSource->m_pImage );
  SafeDelete( pPackTarget->m_pImage );
  SafeDelete( pPackTarget );

  SafeDelete( pPackSource );

}