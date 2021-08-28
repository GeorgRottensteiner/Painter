// UndoRename.cpp: Implementierung der Klasse CUndoRename.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UndoLayerChange.h"
#include "Layer.h"
#include "DocumentInfo.h"

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

CUndoLayerChange::CUndoLayerChange()
  : m_pDocInfo( NULL ),
    m_pLayer( NULL ),
    m_ChangeType( ULC_ADD_LAYER ),
    m_dwFrameIndex( 0 ),
    m_dwLayerIndex( 0 ),
    m_dwSecondLayerIndex( 0 )
{

}

CUndoLayerChange::~CUndoLayerChange()
{

  if ( m_pLayer )
  {
    m_pLayer->Clear();
    delete m_pLayer;
  }

}



CUndoLayerChange::CUndoLayerChange( DocumentInfo* pDocInfo, 
                                    size_t dwFrameIndex,
                                    size_t dwLayerIndex,
                                    size_t dwLayerIndex2,
                                    eLayerChangeType ChangeType )
{

  m_pDocInfo            = pDocInfo;

  m_dwLayerIndex        = dwLayerIndex;
  m_dwSecondLayerIndex  = dwLayerIndex2;
  m_dwFrameIndex        = dwFrameIndex;

  m_ChangeType          = ChangeType;

  if ( m_ChangeType == ULC_REMOVE_LAYER )
  {
    // Layer merken
    m_pLayer = m_pDocInfo->GetLayer( m_dwFrameIndex, m_dwLayerIndex );
  }
  else if ( m_ChangeType == ULC_ADD_LAYER )
  {
    m_pLayer = NULL;
  }
  else if ( m_ChangeType == ULC_EXCHANGE_LAYER )
  {
    m_pLayer = NULL;
  }

  if ( !m_pDocInfo )
  {
    AfxGetMainWnd()->MessageBox( _T( "CUndoLayerChange without DocumentInfo!" ) );
  }

}



BOOL CUndoLayerChange::Restore()
{
  if ( m_ChangeType == ULC_ADD_LAYER )
  {
    // der Layer wurde eingesetzt, weg damit
    if ( m_dwLayerIndex >= m_pDocInfo->m_LayeredFrames[m_dwFrameIndex].LayerCount() )
    {
      AfxGetMainWnd()->MessageBox( _T( "Cannot Undo, Layer missing!" ) );
      return FALSE;
    }
    std::vector<CLayer*>::iterator    it( m_pDocInfo->m_LayeredFrames[m_dwFrameIndex].Layers.begin() );
    std::advance( it, m_dwLayerIndex );

    m_pDocInfo->m_LayeredFrames[m_dwFrameIndex].Layers.erase( it );
  }
  else if ( m_ChangeType == ULC_REMOVE_LAYER )
  {
    // den Layer wieder einsetzen
    std::vector<CLayer*>::iterator    it( m_pDocInfo->m_LayeredFrames[m_dwFrameIndex].Layers.begin() );
    std::advance( it, m_dwLayerIndex );

    m_pDocInfo->m_LayeredFrames[m_dwFrameIndex].Layers.insert( it, m_pLayer );

    m_pLayer = NULL;
  }
  else if ( m_ChangeType == ULC_EXCHANGE_LAYER )
  {
    CLayer*   pLayer = m_pDocInfo->GetLayer( m_dwFrameIndex, m_dwLayerIndex );

    m_pDocInfo->m_LayeredFrames[m_dwFrameIndex].Layers[m_dwLayerIndex] = m_pDocInfo->m_LayeredFrames[m_dwFrameIndex].Layers[m_dwSecondLayerIndex];
    m_pDocInfo->m_LayeredFrames[m_dwFrameIndex].Layers[m_dwSecondLayerIndex] = pLayer;
  }

  pSettings->Notify( NF_LAYER_CHANGED );

  m_pDocInfo->RedrawAllViews();

  return TRUE;

}



CUndoState* CUndoLayerChange::CreateComplementCopy()
{

  if ( m_ChangeType == ULC_ADD_LAYER )
  {
    CUndoLayerChange* pCompState = new CUndoLayerChange( m_pDocInfo,
                                                         m_dwFrameIndex,
                                                         m_dwLayerIndex,
                                                         0,
                                                         ULC_REMOVE_LAYER );
    return pCompState;
  }
  if ( m_ChangeType == ULC_REMOVE_LAYER )
  {
    CUndoLayerChange* pCompState = new CUndoLayerChange( m_pDocInfo,
                                                         m_dwFrameIndex,
                                                         m_dwLayerIndex,
                                                         0,
                                                         ULC_ADD_LAYER );
    return pCompState;
  }
  if ( m_ChangeType == ULC_EXCHANGE_LAYER )
  {
    CUndoLayerChange* pCompState = new CUndoLayerChange( m_pDocInfo,
                                                         m_dwFrameIndex,
                                                         m_dwLayerIndex,
                                                         m_dwSecondLayerIndex,
                                                         ULC_EXCHANGE_LAYER );
    return pCompState;
  }
  return NULL;

}
