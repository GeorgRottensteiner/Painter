#pragma once

#include <list>

#include <GR/GRTypes.h>



class GRMiniDockFrameWnd;
class GRControlBar;

class GRDockingManager
{

  public:

    static bool                           m_LoadingState;

    static GRMiniDockFrameWnd*            m_pDockBars[4];

    static std::list<GRControlBar*>       m_ControlBars;

    static void                           EnableDocking( GR::u32 dwDockStyles );

    static void                           DockControlBar( GRControlBar* pBar, GR::u32 DockPos, POINT& BarPos );

    static void                           AddControlBar( GRControlBar* pBar );
    static void                           RemoveControlBar( GRControlBar* pBar );

    static DWORD                          CheckForDockingPosition( GRControlBar* pBar, POINT& Pos );

    static void                           ShowControlBar( GRControlBar* pBar, bool Show );

    static void                           SaveBarState();
    static void                           LoadBarState();

};
