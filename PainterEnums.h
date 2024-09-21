#ifndef _PAINTER_ENUMS_INCLUDED_
#define _PAINTER_ENUMS_INCLUDED_

#include <string>

#include <GR/GRTypes.h>



enum SaveType
{
  SAVETYPE_UNKNOWN = 0,
  SAVETYPE_IGF,
  SAVETYPE_BTH,
  SAVETYPE_FNH,
  SAVETYPE_BTN,
  SAVETYPE_FNX,
  SAVETYPE_PCX,
  SAVETYPE_GIF,
  SAVETYPE_ANX,
  SAVETYPE_ANH,
  SAVETYPE_TGA,
  SAVETYPE_BMP, 
  SAVETYPE_FNXL,
  SAVETYPE_PNT,
  SAVETYPE_JPEG,
  SAVETYPE_PNG,
  SAVETYPE_ICON,
  SAVETYPE_CURSOR,
  SAVETYPE_IFF,
  SAVETYPE_GRI,
  SAVETYPE_WEBP,

  SAVETYPE_LASTENTRY,
};

enum eNotifyMessage
{
  NF_NULL = 0,
  NF_ACTIVE_COLOR_CHANGED,
  NF_ACTIVE_VIEW_CHANGED,
  NF_LAYER_CHANGED,
  NF_FUNCTION_CHANGED,
  NF_BRUSH_MODE_CHANGED,      // der Brush-Typ hat sich geändert
  NF_DOC_ADDED,
  NF_DOC_REMOVED,
  NF_FRAME_CHANGED
};

struct tPainterFunction
{
  GR::tString       m_strGroupName;
  GR::tString       m_strFunctionDesc;
  GR::tString       m_strFunctionName;
  unsigned long     m_dwIconID;
  unsigned long     m_dwCommandID;
  unsigned long     m_dwControlType;

  tPainterFunction() :
    m_dwIconID( 0 ),
    m_dwCommandID( 0 ),
    m_dwControlType( 0 )
  {
  }
};

enum eControlBarControlType
{
  CBCT_BUTTON,
  CBCT_SEPARATOR,
  CBCT_CHECKBUTTON,
  CBCT_RADIOBUTTON,
  CBCT_DIALOG,
  CBCT_MENUBAR,
};
#endif