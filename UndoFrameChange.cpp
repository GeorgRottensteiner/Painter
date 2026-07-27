#include "stdafx.h"
#include "UndoFrameChange.h"
#include "DocumentInfo.h"

#include "PainterFontDoc.h"

#include <Misc/Misc.h>
#include <Grafik/Image.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CUndoFrameChange::CUndoFrameChange()
  : m_pDocInfo( NULL ),
    m_dwFrameIndex( 0 ),
    m_fcType( UFC_INSERT_FRAME )
{

}



CUndoFrameChange::~CUndoFrameChange()
{
  m_vectImages.clear();
}


CUndoFrameChange::CUndoFrameChange( DocumentInfo* pDocInfo,
                                    eFrameChangeType fcType,
                                    GR::u32 dwFrameIndex,
                                    GR::u32 userData )
{

  if ( !pDocInfo )
  {
    AfxGetMainWnd()->MessageBox( _T( "CUndoFrameChange without DocumentInfo!" ) );
  }

  m_pDocInfo        = pDocInfo;
  m_dwFrameIndex    = dwFrameIndex;
  m_fcType          = fcType;
  m_UserData        = userData;

  if ( m_fcType == UFC_REMOVE_FRAME )
  {
    DocumentInfo::tVectLayers&    layers = m_pDocInfo->m_LayeredFrames[dwFrameIndex].Layers;

    DocumentInfo::tVectLayers::iterator   it( layers.begin() );
    while ( it != layers.end() )
    {
      CLayer& layer = *it;

      CLayer   savedLayer( layer );

      m_vectImages.push_back( savedLayer );

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
    if ( m_pDocInfo->m_DocType == DT_FONT )
    {
      CPainterFontDoc* pFontDoc = (CPainterFontDoc*)m_pDocInfo->m_pDoc;

      pFontDoc->m_ActualLetters.insert( pFontDoc->m_ActualLetters.begin() + dwFrame, (GR::UTF8Char)m_UserData );
    }

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
                                 m_dwFrameIndex,
                                 m_UserData );
  }
  else if ( m_fcType == UFC_REMOVE_FRAME )
  {
    return new CUndoFrameChange( m_pDocInfo,
                                 UFC_INSERT_FRAME,
                                 m_dwFrameIndex,
                                 m_UserData );
  }
  return NULL;

}
