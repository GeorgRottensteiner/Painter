// UndoRename.cpp: Implementierung der Klasse CUndoRename.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UndoImageChange.h"
#include "Layer.h"
#include "DocumentInfo.h"

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

CUndoImageChange::CUndoImageChange()
  : m_pDocInfo( NULL ),
    m_pImage( NULL ),
    m_dwFrameIndex( 0 )
{

}

CUndoImageChange::~CUndoImageChange()
{

  SafeDelete( m_pImage );

}



CUndoImageChange::CUndoImageChange( int iX1, int iY1, int iX2, int iY2, 
                                    size_t dwFrameIndex,
                                    size_t dwLayerIndex,
                                    size_t dwFunction, DocumentInfo *pDocInfo,
                                    bool bMask )
{

  m_pDocInfo        = pDocInfo;

  m_iX1             = iX1;
  m_iY1             = iY1;
  m_iX2             = iX2;
  m_iY2             = iY2;

  m_dwFrameIndex    = dwFrameIndex;
  m_dwLayerIndex    = dwLayerIndex;
  if ( bMask )
  {
    m_pImage          = new GR::Graphic::Image( pDocInfo->GetMask( m_dwFrameIndex, m_dwLayerIndex ) );
  }
  else
  {
    m_pImage          = new GR::Graphic::Image( pDocInfo->GetImage( m_dwFrameIndex, m_dwLayerIndex ) );
  }
  m_dwFunction      = dwFunction;
  m_bMask           = bMask;

  if ( !m_pDocInfo )
  {
    AfxGetMainWnd()->MessageBox( _T( "CUndoImageChange without DocumentInfo!" ) );
  }

}



BOOL CUndoImageChange::Restore()
{
  CLayer*     pLayer;


  if ( m_dwLayerIndex >= m_pDocInfo->m_LayeredFrames[m_dwFrameIndex].LayerCount() )
  {
    AfxGetMainWnd()->MessageBox( _T( "Cannot Undo, Layer missing!" ) );
    return FALSE;
  }

  pLayer = m_pDocInfo->GetLayer( m_dwFrameIndex, m_dwLayerIndex );

  // BAUSTELLE - PFUI - geht überhaupt nicht auf das Rect ein!
  if ( m_bMask )
  {
    memcpy( pLayer->GetMask()->GetData(), 
            m_pImage->GetData(),
            m_pImage->GetSize() );
  }
  else
  {
    memcpy( pLayer->GetImage()->GetData(), 
            m_pImage->GetData(),
            m_pImage->GetSize() );
  }

  m_pDocInfo->RedrawAllViews();

  return TRUE;
}



CUndoState* CUndoImageChange::CreateComplementCopy()
{

  CUndoImageChange *pCompState = new CUndoImageChange( m_iX1, m_iY1, m_iX2, m_iY2,
                                                       m_dwFrameIndex,
                                                       m_dwLayerIndex,
                                                       m_dwFunction,
                                                       m_pDocInfo,
                                                       m_bMask );
  return pCompState;

}
