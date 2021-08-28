// UndoSizeChange.cpp: implementation of the CUndoSizeChange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UndoFrameChange.h"
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

CUndoFrameChange::CUndoFrameChange()
  : m_pDocInfo( NULL ),
    m_dwFrameIndex( 0 ),
    m_fcType( UFC_INSERT_FRAME )
{

}

CUndoFrameChange::~CUndoFrameChange()
{

  std::vector<CLayer*>::iterator    it( m_vectImages.begin() );
  while ( it != m_vectImages.end() )
  {
    delete *it;

    ++it;
  }
  m_vectImages.clear();

}


CUndoFrameChange::CUndoFrameChange( DocumentInfo* pDocInfo,
                                    eFrameChangeType fcType,
                                    DWORD dwFrameIndex )
{

  if ( !pDocInfo )
  {
    AfxGetMainWnd()->MessageBox( _T( "CUndoFrameChange without DocumentInfo!" ) );
  }

  m_pDocInfo        = pDocInfo;
  m_dwFrameIndex    = dwFrameIndex;
  m_fcType          = fcType;

  if ( m_fcType == UFC_REMOVE_FRAME )
  {
    DocumentInfo::tVectLayers&    layers = m_pDocInfo->m_LayeredFrames[dwFrameIndex].Layers;

    DocumentInfo::tVectLayers::iterator   it( layers.begin() );
    while ( it != layers.end() )
    {
      CLayer*   pLayer = *it;


      CLayer*   pSavedLayer = new CLayer( *pLayer );

      /*
      pSavedLayer->SetLayerImage( new GR::Graphic::Image( pLayer->GetImage() ) );
      if ( pLayer->m_HasMask )
      {
        pSavedLayer->SetLayerMask( new GR::Graphic::Image( pLayer->GetMask() ) );
      }
      */

      m_vectImages.push_back( pSavedLayer );

      ++it;
    }
  }
  else if ( m_fcType == UFC_INSERT_FRAME )
  {
    

  }

}



BOOL CUndoFrameChange::Restore()
{

  if ( m_fcType == UFC_INSERT_FRAME )
  {
    m_pDocInfo->RemoveFrame( m_dwFrameIndex );
  }
  else if ( m_fcType == UFC_REMOVE_FRAME )
  {
    size_t   dwFrame = m_pDocInfo->AddFrame( m_dwFrameIndex );

    DocumentInfo::tVectLayers&   Vectm_vectLayers = m_pDocInfo->m_LayeredFrames[dwFrame].Layers;

    for ( size_t i = 0; i < m_vectImages.size(); ++i )
    {
      Vectm_vectLayers.push_back( m_vectImages[i] );
    }
    m_vectImages.clear();
  }

  m_pDocInfo->RecreateAllViews();
  m_pDocInfo->RedrawAllViews();

  return TRUE;

}



CUndoState* CUndoFrameChange::CreateComplementCopy()
{

  if ( m_fcType == UFC_INSERT_FRAME )
  {
    return new CUndoFrameChange( m_pDocInfo,
                                 UFC_REMOVE_FRAME,
                                 m_dwFrameIndex );
  }
  else if ( m_fcType == UFC_REMOVE_FRAME )
  {
    return new CUndoFrameChange( m_pDocInfo,
                                 UFC_INSERT_FRAME,
                                 m_dwFrameIndex );
  }
  return NULL;

}
