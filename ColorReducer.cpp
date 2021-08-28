#include "stdafx.h"
#include "painter.h"
#include "ColorReducer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CColorReducer::CColorReducer( int NumColors ) :
  m_pCubeInfo( NULL ),
  m_pQuantizeInfo( new QuantizeInfo ),
  m_Depth( 8 ),
  m_Colors( NumColors )
{
  memset( m_pQuantizeInfo, 0, sizeof( QuantizeInfo ) );

  m_pQuantizeInfo->number_colors    = NumColors;
  m_pQuantizeInfo->dither           = FALSE;
  m_pQuantizeInfo->colorspace       = RGBColorspace;

  // Depth of color tree is: Log4(colormap size)+2.

  int colors = NumColors;
  for ( m_Depth = 1; colors != 0; m_Depth++ )
  {
    colors >>= 2;
  }
  if ( m_pQuantizeInfo->dither )
  {
    m_Depth--;
  }
  m_Depth += 2;

  m_pCubeInfo = GetCubeInfo( m_pQuantizeInfo, m_Depth );
}



CColorReducer::~CColorReducer()
{
  DestroyCubeInfo( m_pCubeInfo );

  delete m_pQuantizeInfo;
}





void CColorReducer::PruneChild( CubeInfo *cube_info, const NodeInfo *node_info )
{
  // Traverse any children.
  if ( node_info->census != 0 )
  {
    for ( int id = 0; id < ColorReducer::MaxTreeDepth; id++ )
    {
      if ( node_info->census & ( 1 << id ) )
      {
        PruneChild( cube_info, node_info->child[id] );
      }
    }
  }

  // Merge color statistics into parent.
  NodeInfo*       parent = node_info->parent;

  parent->census        &= ~( 1 << node_info->id );
  parent->number_unique += node_info->number_unique;
  parent->total_red     += node_info->total_red;
  parent->total_green   += node_info->total_green;
  parent->total_blue    += node_info->total_blue;
  cube_info->nodes--;
}



void CColorReducer::PruneLevel( CubeInfo* cube_info, const NodeInfo* node_info )
{
  // Traverse any children.
  if ( node_info->census != 0 )
  {
    for ( int id = 0; id < ColorReducer::MaxTreeDepth; id++ )
    {
      if ( node_info->census & ( 1 << id ) )
      {
        PruneLevel( cube_info, node_info->child[id] );
      }
    }
  }
  if ( node_info->level == cube_info->depth )
  {
    PruneChild( cube_info, node_info );
  }
}



CColorReducer::NodeInfo* CColorReducer::GetNodeInfo( CubeInfo* cube_info, const unsigned int id, const unsigned int level, NodeInfo* parent )
{
  NodeInfo*   node_info;

  if ( cube_info->free_nodes == 0 )
  {
    Nodes*        nodes;

    // Allocate a new nodes of nodes.
    nodes = (Nodes*)malloc( sizeof( Nodes ) );
    if ( nodes == NULL )
    {
      return( NULL );
    }
    nodes->next           = cube_info->node_queue;
    cube_info->node_queue = nodes;
    cube_info->next_node  = nodes->nodes;
    cube_info->free_nodes = ColorReducer::NodesInAList;
  }
  cube_info->nodes++;
  cube_info->free_nodes--;
  node_info = cube_info->next_node++;
  memset( node_info, 0, sizeof( NodeInfo ) );
  node_info->parent = parent;
  node_info->id     = id;
  node_info->level  = level;

  return node_info;
}



bool CColorReducer::Classification( CubeInfo* cube_info, CMaskedContextDescriptor* pCD )
{
  double        bisect,
                mid_red,
                mid_green,
                mid_blue;

  long          count;

  NodeInfo*     node_info;

  double        blue,
                green,
                red;

  unsigned int  index,
                level,
                id;

  for ( long y = 0; y < (long)pCD->Height(); ++y )
  {
    if ( cube_info->nodes > ColorReducer::MaxNodes )
    {
      // Prune one level if the color tree is too large.
      PruneLevel( cube_info, cube_info->root );
      --cube_info->depth;
    }

    for ( long x = 0; x < (long)pCD->Width(); x += count )
    {
      // Start at the root and descend the color cube tree.
      count       = 1;
      index       = ColorReducer::MaxTreeDepth-1;
      bisect      = ( MaxRGB() + 1 ) / 2.0;
      mid_red     = MaxRGB() / 2.0;
      mid_green   = MaxRGB() / 2.0;
      mid_blue    = MaxRGB() / 2.0;
      node_info   = cube_info->root;

      GR::u32 pixel = pCD->GetPixel( x, y );
      int     r = ( ( pixel & 0xff0000 ) >> 16 ),
              g = ( ( pixel & 0xff00 ) >> 8 ),
              b = ( pixel & 0xff );

      for ( level = 1; level <= cube_info->depth; level++ )
      {
        bisect  *= 0.5;
        id      = (unsigned int)( ( ( Downscale( r ) >> index ) & 0x01 ) << 2 |
                                  ( ( Downscale( g ) >> index ) & 0x01 ) << 1 |
                                  ( ( Downscale( b ) >> index ) & 0x01 ) );
        mid_red   += id & 4 ? bisect : -bisect;
        mid_green += id & 2 ? bisect : -bisect;
        mid_blue  += id & 1 ? bisect : -bisect;
        if ( node_info->child[id] == NULL )
        {
          // Set colors of new node to contain pixel.
          node_info->census     |= ( 1 << id );
          node_info->child[id]  = GetNodeInfo( cube_info, id, level, node_info );
          if ( node_info->child[id] == NULL )
          {
            MessageBoxA( NULL, "alles hin", "soso", 0 );
          }
          if ( level == cube_info->depth )
          {
            ++cube_info->colors;
          }
        }

        // Approximate the quantization error represented by this node.
        node_info = node_info->child[id];
        red       = (double)r - mid_red;
        green     = (double)g - mid_green;
        blue      = (double)b - mid_blue;
        node_info->quantize_error += count * red * red + count * green * green + count * blue * blue;
        cube_info->root->quantize_error += node_info->quantize_error;
        index--;
      }
      // Sum RGB for this leaf for later derivation of the mean cube color.
      node_info->number_unique  += count;
      node_info->total_red      += count * r;
      node_info->total_green    += count * g;
      node_info->total_blue     += count * b;
    }
  }
  return true;
}



bool CColorReducer::Classification( CubeInfo* cube_info, GR::Graphic::Palette* pPalette )
{
  double          bisect,
                  mid_red,
                  mid_green,
                  mid_blue;

  long            count;

  NodeInfo*       node_info;

  double          blue,
                  green,
                  red;

  unsigned int    index,
                  level,
                  id;

  for ( int i = 0; i < 256; i++ )
  {
    if (cube_info->nodes > ColorReducer::MaxNodes)
    {
      // Prune one level if the color tree is too large.
      PruneLevel( cube_info, cube_info->root );
      cube_info->depth--;
    }

    // Start at the root and descend the color cube tree.
    count       = 1;
    index       = ColorReducer::MaxTreeDepth - 1;
    bisect      = (MaxRGB() + 1 ) / 2.0;
    mid_red     = MaxRGB() / 2.0;
    mid_green   = MaxRGB() / 2.0;
    mid_blue    = MaxRGB() / 2.0;
    node_info   = cube_info->root;

    int     r = pPalette->Red( i ),
            g = pPalette->Green( i ),
            b = pPalette->Blue( i );

    for ( level = 1; level <= cube_info->depth; level++ )
    {
      bisect  *= 0.5;
      id      = (unsigned int)( ( ( Downscale( r ) >> index ) & 0x01 ) << 2 |
                                ( ( Downscale( g ) >> index ) & 0x01 ) << 1 |
                                ( ( Downscale( b ) >> index ) & 0x01 ) );
      mid_red   += id & 4 ? bisect : -bisect;
      mid_green += id & 2 ? bisect : -bisect;
      mid_blue  += id & 1 ? bisect : -bisect;
      if ( node_info->child[id] == NULL )
      {
        // Set colors of new node to contain pixel.
        node_info->census     |= ( 1 << id );
        node_info->child[id]  = GetNodeInfo( cube_info, id, level, node_info );
        if ( node_info->child[id] == NULL )
        {
          MessageBoxA( NULL, "alles hin", "soso", 0 );
        }
        if ( level == cube_info->depth )
        {
          ++cube_info->colors;
        }
      }
      // Approximate the quantization error represented by this node.
      node_info = node_info->child[id];
      red       = (double)r - mid_red;
      green     = (double)g - mid_green;
      blue      = (double)b - mid_blue;
      node_info->quantize_error+= count * red * red + count * green * green + count * blue * blue;
      cube_info->root->quantize_error += node_info->quantize_error;
      index--;
    }
    // Sum RGB for this leaf for later derivation of the mean cube color.
    node_info->number_unique  += count;
    node_info->total_red      += count * r;
    node_info->total_green    += count * g;
    node_info->total_blue     += count * b;
  }
  return true;
}



void CColorReducer::Reduce( CubeInfo* cube_info, const NodeInfo* node_info )
{
  // Traverse any children.
  if ( node_info->census != 0 )
  {
    for ( int id = 0; id < ColorReducer::MaxTreeDepth; id++ )
    {
      if ( node_info->census & ( 1 << id ) )
      {
        Reduce( cube_info, node_info->child[id] );
      }
    }
  }
  if ( node_info->quantize_error <= cube_info->pruning_threshold )
  {
    PruneChild( cube_info, node_info );
  }
  else
  {
    // Find minimum pruning threshold.
    if ( node_info->number_unique > 0 )
    {
      cube_info->colors++;
    }
    if ( node_info->quantize_error < cube_info->next_threshold )
    {
      cube_info->next_threshold = node_info->quantize_error;
    }
  }
}



void CColorReducer::Reduction( CubeInfo* cube_info, const unsigned long number_colors )
{
  unsigned long   span = cube_info->colors;

  cube_info->next_threshold = 0.0;
  while ( cube_info->colors > number_colors )
  {
    cube_info->pruning_threshold  = cube_info->next_threshold;
    cube_info->next_threshold     = cube_info->root->quantize_error - 1;
    cube_info->colors             = 0;
    Reduce( cube_info, cube_info->root );
  }
}



void CColorReducer::DefineColormap( CubeInfo* cube_info, NodeInfo* node_info )
{
  // Traverse any children.
  if ( node_info->census != 0 )
  {
    for ( int id = 0; id < ColorReducer::MaxTreeDepth; id++ )
    {
      if ( node_info->census & ( 1 << id ) )
      {
        DefineColormap( cube_info, node_info->child[id] );
      }
    }
  }
  if ( node_info->number_unique != 0 )
  {
    // Colormap entry is defined by the mean color in this cube.
      
    double      number_unique = node_info->number_unique;
    cube_info->colormap[cube_info->colors].red    = (Quantum)( ( node_info->total_red + 0.5 * number_unique ) / number_unique );
    cube_info->colormap[cube_info->colors].green  = (Quantum)( ( node_info->total_green + 0.5 * number_unique ) / number_unique );
    cube_info->colormap[cube_info->colors].blue   = (Quantum)( ( node_info->total_blue + 0.5 * number_unique ) / number_unique );
    node_info->color_number                       = cube_info->colors++;
  }
}



void CColorReducer::ClosestColor( CubeInfo* cube_info, const NodeInfo* node_info )
{
  if ( cube_info->distance != 0.0 )
  {
    // Traverse any children.
    if ( node_info->census != 0 )
    {
      for ( int id = 0; id < ColorReducer::MaxTreeDepth; id++ )
      {
        if ( node_info->census & ( 1 << id ) )
        {
          ClosestColor( cube_info, node_info->child[id] );
        }
      }
    }
    if ( node_info->number_unique != 0 )
    {
      double          distance;

      double          blue,
                      green,
                      red;

      PixelPacket*    color;

      // Determine if this color is "closest".
      color     = cube_info->colormap + node_info->color_number;
      red       = (double)( color->red - cube_info->color.red );
      green     = (double)( color->green - cube_info->color.green );
      blue      = (double)( color->blue - cube_info->color.blue );
      distance  = red * red + green * green + blue * blue;
      if ( distance < cube_info->distance )
      {
        cube_info->distance     = distance;
        cube_info->color_number = node_info->color_number;
      }
    }
  }
}



CPainterImagePackage* CColorReducer::Assignment( CubeInfo* cube_info, CMaskedContextDescriptor* pCD )
{
  GR::u8    bitDepth = 8;

  if ( m_Colors <= 2 )
  {
    bitDepth = 1;
  }
  else if ( m_Colors <= 4 )
  {
    bitDepth = 2;
  }
  else if ( m_Colors <= 16 )
  {
    bitDepth = 4;
  }
  GR::Graphic::Image*   pImage = new GR::Graphic::Image( pCD->Width(), pCD->Height(), bitDepth );

  CPainterImagePackage* pIP = new CPainterImagePackage( pImage );

  pIP->m_Palette.Create( m_Colors );

  CMaskedContextDescriptor* pCDResult = new CMaskedContextDescriptor( pImage, &pIP->m_Palette );

  IndexPacket   index;

  size_t        count,
                i;

  const NodeInfo*   node_info;

  unsigned int  dither,
                id;


  // Allocate image colormap.
  cube_info->colormap = new PixelPacket[256];
  cube_info->colors   = 0;
  DefineColormap( cube_info, cube_info->root );

  // Create a reduced color image.
  dither = cube_info->quantize_info->dither;
  for ( long y = 0; y < (long)pCD->Height(); y++ )
  {
    //indexes=GetIndexes(image);
    for ( long x = 0; x < pCD->Width(); x += count )
    {
      // Identify the deepest node containing the pixel's color.
      count = 1;

      GR::u32 pixel = pCD->GetPixel( x, y );
      int     r = ( ( pixel & 0xff0000 ) >> 16 ),
              g = ( ( pixel & 0xff00 ) >> 8 ),
              b = ( pixel & 0xff );

      node_info = cube_info->root;
      for ( index = ColorReducer::MaxTreeDepth - 1; (int)index > 0; index-- )
      {
        id = (unsigned int)( ( ( Downscale( r ) >> index ) & 0x01 ) << 2 |
                             ( ( Downscale( g ) >> index ) & 0x01 ) << 1 |
                             ( ( Downscale( b ) >> index ) & 0x01 ) );
        if ( ( node_info->census & ( 1 << id ) ) == 0 )
        {
          break;
        }
        node_info = node_info->child[id];
      }

      // Find closest color among siblings and their children.
      cube_info->color.red    = r;
      cube_info->color.green  = g;
      cube_info->color.blue   = b;
      cube_info->distance     = 3.0 * ( MaxRGB() + 1 ) * ( MaxRGB() + 1 );
      ClosestColor( cube_info, node_info->parent );
      index                   = (unsigned int)cube_info->color_number;
      for ( i = 0; i < count; i++ )
      {
        if ( !cube_info->quantize_info->measure_error )
        {
          pCDResult->PutDirectPixel( x, y, index );
        }
      }
    }
  }

  for ( i = 0; i < cube_info->colors; i++ )
  {
    pIP->m_Palette.SetColor( i, cube_info->colormap[i].red, cube_info->colormap[i].green, cube_info->colormap[i].blue );
  }

  delete pCDResult;

  delete[] cube_info->colormap;

  return pIP;
}



CColorReducer::CubeInfo* CColorReducer::GetCubeInfo( const QuantizeInfo *quantize_info, unsigned int depth )
{
  CubeInfo*       cube_info;

  double          sum,
                  weight;


  // Initialize tree to describe color cube_info.
  cube_info = new CubeInfo();
  if ( cube_info == NULL )
  {
    return NULL;
  }
  memset( cube_info, 0, sizeof( CubeInfo ) );
  if ( depth > ColorReducer::MaxTreeDepth )
  {
    depth = ColorReducer::MaxTreeDepth;
  }
  if ( depth < 2 )
  {
    depth = 2;
  }
  cube_info->depth = depth;

  // Initialize root node.
  cube_info->root = GetNodeInfo( cube_info, 0, 0, NULL );
  if ( cube_info->root == NULL )
  {
    return NULL;
  }

  cube_info->root->parent   = cube_info->root;
  cube_info->quantize_info  = quantize_info;
  if ( !cube_info->quantize_info->dither )
  {
    return cube_info;
  }

  // Initialize dither resources.
  cube_info->cache = (long*)malloc( ( 1 << 18 ) * sizeof( long ) );
  if ( cube_info->cache == NULL )
  {
    return NULL;
  }

  // Initialize color cache.
  for ( int i = 0; i < ( 1 << 18 ); i++ )
  {
    cube_info->cache[i] = ( -1 );
  }

  // Distribute weights along a curve of exponential decay.
  weight = 1.0;
  for ( int i = 0; i < ColorReducer::ExceptionQueueLength; i++ )
  {
    cube_info->weights[ColorReducer::ExceptionQueueLength - i - 1] = 1.0 / weight;
    weight *= exp( log( (double)( MaxRGB() + 1 ) ) / ( ColorReducer::ExceptionQueueLength - 1.0 ) );
  }

  // Normalize the weighting factors.
  weight = 0.0;
  for ( int i = 0; i < ColorReducer::ExceptionQueueLength; i++ )
  {
    weight += cube_info->weights[i];
  }

  sum = 0.0;
  for ( int i = 0; i < ColorReducer::ExceptionQueueLength; i++)
  {
    cube_info->weights[i] /= weight;
    sum += cube_info->weights[i];
  }
  cube_info->weights[0] += 1.0 - sum;
  return cube_info;
}



void CColorReducer::DestroyCubeInfo( CubeInfo* cube_info )
{
  Nodes*       nodes;

  // Release color cube tree storage.
  do
  {
    nodes = cube_info->node_queue->next;
    if ( cube_info->node_queue != NULL )
    {
      free( cube_info->node_queue );
    }
    cube_info->node_queue=nodes;
  } 
  while ( cube_info->node_queue != NULL );

  if ( !cube_info->quantize_info->dither )
  {
    delete cube_info;
    return;
  }
  free( cube_info->cache );
  delete cube_info;
}



void CColorReducer::PalFromCube( GR::Graphic::Palette* pPal, CubeInfo* pCI )
{
  Nodes* pNode = pCI->node_queue;

  GR::u32 dwGlobal = 0;
  while ( pNode != NULL )
  {
    NodeInfo* pNI = pNode->nodes;

    GR::u32   dwNr = 0;
    while ( pNI != NULL )
    {
      dwNr++;
      if ( pNI->number_unique )
      {
        pPal->SetColor( pNI->color_number, 
            (int)( pNI->total_red / pNI->number_unique ),
            (int)( pNI->total_green / pNI->number_unique ),
            (int)( pNI->total_blue / pNI->number_unique ) );

        dwGlobal++;
        if ( dwGlobal >= pCI->colors )
        {
          break;
        }
      }

      pNI++;
      if ( dwNr >= 1536 )
      {
        pNI = NULL;
      }
    }
    pNode = pNode->next;
  }
}



GR::Graphic::Palette* CColorReducer::CreatePalette()
{
  GR::Graphic::Palette*   pPal = new GR::Graphic::Palette();

  pPal->Create( m_Colors );
  PalFromCube( pPal, m_pCubeInfo );

  return pPal;
}



void CColorReducer::AddSourceToColorCube( CPainterImagePackage* pPackSource )
{
  CMaskedContextDescriptor*     pCDSource = new CMaskedContextDescriptor( pPackSource->m_pImage, &pPackSource->m_Palette );


  Classification( m_pCubeInfo, pCDSource );

  delete pCDSource;
}



void CColorReducer::AddSourceToColorCube( GR::Graphic::Palette* pPalette )
{
  Classification( m_pCubeInfo, pPalette );
}



void CColorReducer::Calculate()
{
  Reduction( m_pCubeInfo, m_Colors );
}



CPainterImagePackage* CColorReducer::Reduce( CPainterImagePackage* pPackSource )
{
  if ( pPackSource == NULL )
  {
    return NULL;
  }

  CMaskedContextDescriptor*   pCDSource = new CMaskedContextDescriptor( pPackSource->m_pImage, &pPackSource->m_Palette );

  CPainterImagePackage*       pIPResult = NULL;

  pIPResult = Assignment( m_pCubeInfo, pCDSource );

  delete pCDSource;

  return pIPResult;
}



CColorReducer::Quantum CColorReducer::MaxRGB()
{
  return (Quantum)( ( 1L << ColorReducer::QuantumDepth ) - 1L );
}



CColorReducer::Quantum CColorReducer::Downscale( int Value )
{
  return (Quantum)Value;
}



CColorReducer::Quantum CColorReducer::Upscale( int Value )
{
  return (Quantum)Value;
}
