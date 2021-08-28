// UndoRename.cpp: Implementierung der Klasse CUndoRename.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UndoDepthChange.h"
#include "Layer.h"
#include "DocumentInfo.h"
#include "Settings.h"

#include "PainterEnums.h"

#include <Grafik/Image.h>
#include <Misc/Misc.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CUndoDepthChange::CUndoDepthChange()
  : m_pDocInfo( NULL )
{

}

CUndoDepthChange::~CUndoDepthChange()
{

  m_vectImages.clear();
  m_vectPalettes.clear();

}



CUndoDepthChange::CUndoDepthChange( DocumentInfo *pDocInfo )
  : m_pDocInfo( NULL )
{
  m_pDocInfo        = pDocInfo;

  m_dwOldDepth      = m_pDocInfo->m_BitDepth;

  m_vectImages.resize( pDocInfo->m_LayeredFrames.size() );
  for ( size_t iFrame = 0; iFrame < pDocInfo->m_LayeredFrames.size(); ++iFrame )
  {
    for ( size_t i = 0; i < pDocInfo->m_LayeredFrames[iFrame].LayerCount(); i++ )
    {
      std::vector<PImage>&   vectm_vectLayers = m_vectImages[iFrame];

      vectm_vectLayers.push_back( PImage() );
      PImage&    PImage = vectm_vectLayers.back();

      PImage.m_pImage = new GR::Graphic::Image( pDocInfo->GetImage( iFrame, i ) );
    }
    if ( pDocInfo->GetPalette( iFrame ) )
    {
      m_vectPalettes.push_back( GR::Graphic::Palette( *pDocInfo->GetPalette( iFrame ) ) );
    }
  }

  if ( !m_pDocInfo )
  {
    AfxGetMainWnd()->MessageBox( _T( "CUndoDepthChange without DocumentInfo!" ) );
  }
}



BOOL CUndoDepthChange::Restore()
{
  for ( size_t iFrame = 0; iFrame < m_pDocInfo->m_LayeredFrames.size(); ++iFrame )
  {
    for ( size_t i = 0; i < m_pDocInfo->m_LayeredFrames[iFrame].LayerCount(); i++ )
    {
      m_pDocInfo->GetLayer( iFrame, i )->SetLayerImage( m_vectImages[iFrame][i].m_pImage );
      m_vectImages[iFrame][i].m_pImage = NULL;
    }
    if ( iFrame < m_vectPalettes.size() )
    {
      *m_pDocInfo->GetPalette( iFrame ) = m_vectPalettes[iFrame];
    }
  }

  m_pDocInfo->m_BitDepth = (BYTE)m_dwOldDepth;

  m_pDocInfo->RecreateAllViews();
  m_pDocInfo->RedrawAllViews();

  pSettings->Notify( NF_ACTIVE_COLOR_CHANGED );

  return TRUE;
}



CUndoState* CUndoDepthChange::CreateComplementCopy()
{
  CUndoDepthChange *pCompState = new CUndoDepthChange( m_pDocInfo );

  return pCompState;
}
