// UndoSizeChange.cpp: implementation of the CUndoSizeChange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UndoSizeChange.h"
#include "DocumentInfo.h"

#include <Misc/Misc.h>
#include <Grafik/Image.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUndoSizeChange::CUndoSizeChange()
  : m_pDocInfo( NULL ),
    m_pImageSelection( NULL )
{

}

CUndoSizeChange::~CUndoSizeChange()
{

  m_vectImages.clear();
  SafeDelete( m_pImageSelection );

}


CUndoSizeChange::CUndoSizeChange( DocumentInfo* pDocInfo )
{
  if ( !pDocInfo )
  {
    AfxGetMainWnd()->MessageBox( _T( "CUndoSizeChange without DocumentInfo!" ) );
  }

  m_pDocInfo = pDocInfo;

  m_vectImages.resize( pDocInfo->m_LayeredFrames.size() );
  for ( size_t iFrame = 0; iFrame < pDocInfo->m_LayeredFrames.size(); ++iFrame )
  {
    for ( size_t i = 0; i < pDocInfo->m_LayeredFrames[iFrame].LayerCount(); i++ )
    {
      std::vector<PImage>&   vectm_vectLayers = m_vectImages[iFrame];

      vectm_vectLayers.push_back( PImage() );
      PImage&    PImage = vectm_vectLayers.back();

      PImage.m_pImage     = new GR::Graphic::Image( pDocInfo->GetImage( iFrame, i ) );
      if ( pDocInfo->GetMask( iFrame, i ) )
      {
        PImage.m_pImageMask = new GR::Graphic::Image( pDocInfo->GetMask( iFrame, i ) );
      }
    }
  }

  m_pImageSelection = new GR::Graphic::Image( pDocInfo->m_pImageSelection );
  m_iWidth  = pDocInfo->Width();
  m_iHeight = pDocInfo->Height();
}



BOOL CUndoSizeChange::Restore()
{

  for ( size_t iFrame = 0; iFrame < m_pDocInfo->m_LayeredFrames.size(); ++iFrame )
  {
    for ( size_t i = 0; i < m_pDocInfo->m_LayeredFrames[iFrame].LayerCount(); i++ )
    {
      m_pDocInfo->GetLayer( iFrame, i )->SetLayerImage( m_vectImages[iFrame][i].m_pImage );
      m_vectImages[iFrame][i].m_pImage = NULL;

      m_pDocInfo->GetLayer( iFrame, i )->SetLayerMask( m_vectImages[iFrame][i].m_pImageMask );
      m_vectImages[iFrame][i].m_pImageMask = NULL;
    }
  }

  m_pDocInfo->SetSize( m_iWidth, m_iHeight );

  SafeDelete( m_pDocInfo->m_pImageSelection );
  m_pDocInfo->m_pImageSelection = m_pImageSelection;
  m_pImageSelection = NULL;
  m_pDocInfo->UpdateMarchingAnts();

  m_pDocInfo->RecreateAllViews();
  m_pDocInfo->RedrawAllViews();

  return TRUE;

}



CUndoState* CUndoSizeChange::CreateComplementCopy()
{

  CUndoSizeChange *pCompState = new CUndoSizeChange( m_pDocInfo );
  return pCompState;

}
