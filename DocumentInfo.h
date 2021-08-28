#if !defined(AFX_DOCUMENTINFO_H__826330A1_D715_11D3_924A_00104B6795C0__INCLUDED_)
#define AFX_DOCUMENTINFO_H__826330A1_D715_11D3_924A_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable:4786 )
#include <Grafik/GfxPage.h>
#include <Grafik/Palette.h>
#include <Grafik/Font.h>
#include "Layer.h"
#include "Frame.h"
#include "Settings.h"
#include "Progress.h"
#include "UndoManager.h"
#include "UndoFrameChange.h"
#include "UndoLayerChange.h"
#include "ViewInfo.h"
#include "MaskedContextDescriptor.h"
#include "PainterEnums.h"

#include <Memory/ByteBuffer.h>


#include <vector>


class CUndoManager;

class CMarchingAntDot
{
  public:
    CPoint  pt;
    BYTE    ucDotType;

    CMarchingAntDot( int iX = 0, int iY = 0, BYTE bDot = 0 )
      : ucDotType( bDot ),
        pt( iX, iY )
    {
    }
};



class DocumentInfo : public INotifyMember<GR::u32>
{

  protected:

    size_t                        m_CurrentFrame,
                                  m_CurrentLayer;

    bool                          m_HasSelection;

    DWORD                         m_DocWidth,
                                  m_DocHeight;


    void                          ReduceColors( int BitsPerColor );

    void ResizeCanvas( GR::Graphic::ContextDescriptor& Source, GR::Graphic::ContextDescriptor& Target,
                       int NewWidth, int NewHeight, bool CenterH, bool CenterV );



  public:

    typedef std::vector<CLayer*>  tVectLayers;

    typedef std::vector<Frame>    tVectLayeredFrames;

    tVectLayeredFrames            m_LayeredFrames;

    std::vector<CMarchingAntDot>  m_MarchingAnts;

    RECT                          m_Selection;

    bool                          m_AnimationRunning,
                                  m_Closed,
                                  m_MButton1Released;

    DWORD                         m_SelectionFlags,
                                  m_MarchingAntCount;

    SaveType                      m_SaveType;

    DocumentType                  m_DocType;

    unsigned char                 m_BitDepth;

    GR::String                    m_FileName;

    DWORD                         m_Timer,
                                  m_OptionFrameSpeed;

    CPoint                        m_DragStart;

    GR::Graphic::Image*           m_pImageSelection;
    GR::Graphic::Image*           m_pImageEffectMask;

    CDocument*                    m_pDoc;

    CUndoManager                  m_UndoManager;

    int                           m_SelectionX,
                                  m_SelectionY,
                                  m_SelectionWidth,
                                  m_SelectionHeight;

    signed long                   m_StartX,
                                  m_StartY,
                                  m_ActX,
                                  m_ActY;



    bool PasteFromClipBoard( int iMode, CViewInfo* pViewInfo = NULL );

    void ZoomIn( void );
    void ZoomOut( void );

    // Dokument-Werte betreffend
    void ResizeCanvas( int iNewWidth, int iNewHeight, bool bCenterH, bool bCenterV );

    void Resize( int iNewWidth, int iNewHeight, DWORD dwResizeType,
                 int ResizePercentageX, int ResizePercentageY );

    void ConvertTo1bpp();
    void ConvertTo4bpp();
    void ConvertTo8bpp();
    void ConvertTo16bpp();
    void ConvertTo24bpp();
    void ConvertTo32bpp();
    void MapToPalette( GR::Graphic::Palette* pNewPalette );

    void SetSize( DWORD dwWidth, DWORD dwHeight );
    void Rotate( void );
    void                        Shift( int iDX, int iDY, size_t CurrentFrame = (size_t)-1 );

    bool                        HasSelection() const;
    void                        HasSelection( bool bSetSelection );

    CLayer*                     ActiveLayer();

    size_t                      AddFrame( size_t iFrameIndex = -1 );
    void                        RemoveFrame( size_t dwFrameNr );
    size_t                      CurrentFrame();
    void                        CurrentFrame( size_t dwFrameNr );
    Frame*                      GetFrame( size_t iFrameNr );

    CLayer*                     AddLayer( GR::Graphic::Image *pOrigin = NULL, size_t iFrame = -1, size_t iLayerInsertPos = -1 );
    void                        RemoveLayer( CLayer *pLayer, size_t iFrame = 0 );
    GR::Graphic::Image*                 GetActiveImage();

    CLayer*                     GetLayer( size_t iFrame, size_t iLayer );
    GR::Graphic::Image*                 GetImage( size_t iFrame, size_t iLayer );
    GR::Graphic::Image*                 GetMask( size_t iFrame, size_t iLayer );
    GR::Graphic::Palette*               GetPalette( size_t iFrame );

    size_t                      CurrentLayer();
    void                        CurrentLayer( size_t dwLayerNr );

    DWORD                       Width() const;
    DWORD                       Height() const;

    CMaskedContextDescriptor    GetCD();

    GR::Graphic::Image*                  GetSelectionMask();

    void                        ReplaceColor( DWORD dwColor1, DWORD dwColor2 );
    void                        SwapColor( GR::u32 Color1, GR::u32 Color2 );
    void                        InvertColor();

    void                        FillListWithAllImages( tImageList& listImages, bool bFillMasks = false );

    unsigned char               GetBitDepth();
    unsigned char               GetTrueBitDepth();

    void RecreateAllViews();
    void SetModify( BOOL bModified );

    void                    EmptySelection();
    void                    AddRectSelection( int iX, int iY, int iWidth, int iHeight );
    void                    RemoveRectSelection( int iX, int iY, int iWidth, int iHeight );
    void                    SetSelection( GR::Graphic::Image* pImage );

    void                    UpdateMarchingAnts();

    // Undo/Redo
    void                    AddUndoRectAll();
    void                    AddUndoRect( int iX1, int iY1, int iX2, int iY2, int iFrame, int iLayer, bool bMask = false, BOOL bStartNewGroup = TRUE );
    void                    AddUndoDepthChange();
    void                    AddUndoSizeChange();
    void                    AddUndoFrameChange( size_t dwFrameIndex, CUndoFrameChange::eFrameChangeType fcType );
    void                    AddUndoLayerChange( size_t dwLayerIndex, size_t dwLayerIndex2, CUndoLayerChange::eLayerChangeType lcType, BOOL bStartNewGroup = TRUE );

    void                    Undo();

    void                    RedrawAllViews();
    void                    RedrawAllViews( const GR::tRect& rc, bool bRedrawNow = true );
    void                    ShowAllViews();

    DWORD                   CountUsedColors();

    DWORD                   ToLocalColor( DWORD dwColor );

    virtual void            OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember );


    void                    ShiftFrameLeft( size_t iFrame );
    void                    ShiftFrameRight( size_t iFrame );
    void                    ShiftFrameUp( size_t iFrame );
    void                    ShiftFrameDown( size_t iFrame );

    ByteBuffer              SaveTempData();
    bool                    RestoreFromTempData( IIOStream& IOIn );


	  DocumentInfo();
	  virtual ~DocumentInfo();

};

#endif // !defined(AFX_DOCUMENTINFO_H__826330A1_D715_11D3_924A_00104B6795C0__INCLUDED_)
