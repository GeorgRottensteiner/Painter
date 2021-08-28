#include <Grafik/ContextDescriptor.h>

#include ".\polygonfillaa.h"

#include <debug/debugclient.h>



void CPolygonFillAA::DrawScanLine( int YToScan, GR::Graphic::ContextDescriptor& cdTarget, GR::u32 dwColor )
{

  int           LeftX;

  int           iErrorTerm = 0;
  int           iErrorAdj = 0;

  EdgeState*    CurrentEdge;

  // Scan through the AET, drawing line segments as each pair of edge crossings is encountered. 
  // The nearest pixel on or to the right of left edges is drawn, and the nearest pixel to the left 
  // of but not on right edges is drawn

  CurrentEdge = AETPtr;

  dh::Log( "====Y %d====", YToScan );

  while ( CurrentEdge != NULL ) 
  {
    bool        bSwap = false;
    LeftX       = CurrentEdge->X;
    iErrorTerm  = abs( CurrentEdge->ErrorTerm );
    iErrorAdj   = CurrentEdge->ErrorTermAdjUp + CurrentEdge->ErrorTermAdjDown;
    if ( CurrentEdge->XDirection >= 0 )
    {
      bSwap = true;
    }

    dh::Log( "Edge %d, %d, %d, %d", CurrentEdge->WholePixelXMove, CurrentEdge->ErrorTerm, CurrentEdge->ErrorTermAdjDown, CurrentEdge->ErrorTermAdjUp );

    CurrentEdge = CurrentEdge->NextEdge;

    dh::Log( "-Edge %d bis %d", LeftX, CurrentEdge->X );

	  if ( CurrentEdge == NULL )
    {
      break;
    }

    // TODO - nicht nur ein Pixel antialiasen!!!
    if ( CurrentEdge->X - 1 >= LeftX + 1 )
    {
      if ( iErrorAdj )
      {
        cdTarget.PutPixelAlpha( LeftX, YToScan, dwColor, !bSwap ? ( 255 - 255 * iErrorTerm / iErrorAdj ) : ( 255 * iErrorTerm / iErrorAdj ) );
      }
      else
      {
        cdTarget.PutPixel( LeftX, YToScan, dwColor );
      }
      if ( LeftX + 1 <= CurrentEdge->X - 2 )
      {
        cdTarget.HLine( LeftX + 1, CurrentEdge->X - 2, YToScan, dwColor );
      }
      if ( CurrentEdge->ErrorTermAdjUp + CurrentEdge->ErrorTermAdjDown )
      {
        cdTarget.PutPixelAlpha( CurrentEdge->X - 1, YToScan, dwColor, 
                      !bSwap ? ( 255 - 255 * abs( CurrentEdge->ErrorTerm ) / ( CurrentEdge->ErrorTermAdjUp + CurrentEdge->ErrorTermAdjDown ) )
                      : ( 255 * abs( CurrentEdge->ErrorTerm ) / ( CurrentEdge->ErrorTermAdjUp + CurrentEdge->ErrorTermAdjDown ) ) );
      }
      else
      {
        cdTarget.PutPixel( CurrentEdge->X - 1, YToScan, dwColor );
      }
    }
    else
    {
      cdTarget.HLine( LeftX, CurrentEdge->X - 1, YToScan, dwColor );
    }
    CurrentEdge = CurrentEdge->NextEdge;
  }


}