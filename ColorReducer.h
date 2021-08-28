#if !defined(AFX_COLORREDUCER_H__0D5E8697_25D3_4D39_BF85_7B9F66600238__INCLUDED_)
#define AFX_COLORREDUCER_H__0D5E8697_25D3_4D39_BF85_7B9F66600238__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <math.h>



namespace ColorReducer
{
  const int MaxTreeDepth          = 8;
  const int NodesInAList          = 1536;
  const int ExceptionQueueLength  = 16;
  const int MaxNodes              = 266817;
  const int QuantumDepth          = 8;
}



class CColorReducer  
{
  protected:

    int           m_Depth;
    int           m_Colors;


    struct NodeInfo
    {
      NodeInfo*     parent;
      NodeInfo*     child[8];

      double        number_unique,
                    total_red,
                    total_green,
                    total_blue,
                    quantize_error;

      unsigned long color_number;

      unsigned char id,
                    level,
                    census;

      NodeInfo() : 
        total_red( 0.0f ),
        total_green( 0.0f ),
        total_blue( 0.0f ),
        number_unique( 0.0f )
      {
      }
    };

    struct Nodes
    {
      NodeInfo  nodes[ColorReducer::NodesInAList];
      Nodes*    next;
    };


    typedef unsigned char Quantum;

    enum ColorspaceType
    {
      UndefinedColorspace,
      RGBColorspace,
      GRAYColorspace,
      TransparentColorspace,
      OHTAColorspace,
      XYZColorspace,
      YCbCrColorspace,
      YCCColorspace,
      YIQColorspace,
      YPbPrColorspace,
      YUVColorspace,
      CMYKColorspace,
      sRGBColorspace
    };

    struct PixelPacket
    {
      GR::u8    red,
                green,
                blue;
    };

    typedef Quantum IndexPacket;

    struct QuantizeInfo
    {
      unsigned long     signature;
      unsigned long     number_colors;
      unsigned int      tree_depth,
                        dither;
      ColorspaceType    colorspace;

      unsigned int      measure_error;
    };

    struct CubeInfo
    {
      NodeInfo*       root;    

      unsigned long   colors;

      PixelPacket     color;
      PixelPacket*    colormap;

      double          distance,
                      pruning_threshold,
                      next_threshold;

      unsigned long   nodes,
                      free_nodes,
                      color_number;

      NodeInfo*       next_node;
      Nodes*          node_queue;

      long*           cache;

      double          weights[ColorReducer::ExceptionQueueLength];

      const QuantizeInfo*   quantize_info;

      long            x,
                      y;

      unsigned int    depth;
    };


    void                      PruneChild( CubeInfo* cube_info, const NodeInfo* node_info );
    void                      PruneLevel( CubeInfo* cube_info, const NodeInfo* node_info );
    CubeInfo*                 GetCubeInfo( const QuantizeInfo *quantize_info, unsigned int depth );
    NodeInfo*                 GetNodeInfo( CubeInfo* cube_info, const unsigned int id, const unsigned int level, NodeInfo* parent );
    void                      DestroyCubeInfo( CubeInfo* cube_info );

    bool                      Classification( CubeInfo* cube_info, CMaskedContextDescriptor *pCD );
    bool                      Classification( CubeInfo* cube_info, GR::Graphic::Palette* pPalette );
    void                      Reduce( CubeInfo* cube_info, const NodeInfo *node_info );
    void                      Reduction( CubeInfo* cube_info,const unsigned long number_colors );

    void                      DefineColormap( CubeInfo* cube_info, NodeInfo* node_info );
    void                      ClosestColor( CubeInfo* cube_info,const NodeInfo* node_info );
    CPainterImagePackage*     Assignment( CubeInfo* cube_info, CMaskedContextDescriptor* pCD );

    void                      PalFromCube( GR::Graphic::Palette* pPal, CubeInfo* pCI );

    Quantum                   MaxRGB();
    Quantum                   Downscale( int Value );
    Quantum                   Upscale( int Value );



  public:


    CubeInfo*                 m_pCubeInfo;

    QuantizeInfo*             m_pQuantizeInfo;


	  CColorReducer( int NumColors = 256 );
	  virtual ~CColorReducer();


    void                      AddSourceToColorCube( CPainterImagePackage* pPackSource );
    void                      AddSourceToColorCube( GR::Graphic::Palette* pPalette );
    void                      Calculate();
    CPainterImagePackage*     Reduce( CPainterImagePackage* pPackSource );
    GR::Graphic::Palette*     CreatePalette();



};


#endif // !defined(AFX_COLORREDUCER_H__0D5E8697_25D3_4D39_BF85_7B9F66600238__INCLUDED_)

