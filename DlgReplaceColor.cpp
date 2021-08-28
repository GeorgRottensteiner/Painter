#include "stdafx.h"
#include "painter.h"
#include "DlgReplaceColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgReplaceColor::CDlgReplaceColor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReplaceColor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgReplaceColor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgReplaceColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReplaceColor)
	DDX_Control(pDX, IDC_BUTTON_COLOR_2, m_ButtonColor2);
	DDX_Control(pDX, IDC_BUTTON_COLOR_1, m_ButtonColor1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgReplaceColor, CDialog)
	//{{AFX_MSG_MAP(CDlgReplaceColor)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CDlgReplaceColor::OnOK() 
{
	CDialog::OnOK();
}



void CDlgReplaceColor::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  if ( lpDrawItemStruct->itemState & ODS_DISABLED )
  {
    // disabled
    DrawFrameControl( lpDrawItemStruct->hDC,
                      &lpDrawItemStruct->rcItem,
                      DFC_BUTTON,
                      DFCS_BUTTONPUSH | DFCS_INACTIVE );
    return;
  }

  if ( nIDCtl == IDC_BUTTON_COLOR_1 )
  {
    GR::u32   color = ( ( m_Color1 & 0xff0000 ) >> 16 )
                    + ( ( m_Color1 & 0x0000ff ) << 16 )
                    +   ( m_Color1 & 0x00ff00 );

    HBRUSH    hBrush = CreateSolidBrush( color );

    FillRect( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              hBrush );
    if ( m_ButtonColor1.GetState() & 0x0004 )
    {
      // gedrückt
      DrawEdge( lpDrawItemStruct->hDC,
                &lpDrawItemStruct->rcItem,
                EDGE_SUNKEN,
                BF_RECT );
    }
    else
    {
      DrawEdge( lpDrawItemStruct->hDC,
                &lpDrawItemStruct->rcItem,
                EDGE_RAISED,
                BF_RECT );
    }

    DeleteObject( hBrush );
  }
  else if ( nIDCtl == IDC_BUTTON_COLOR_2 )
  {
    GR::u32 color = ( ( m_Color2 & 0xff0000 ) >> 16 )
                  + ( ( m_Color2 & 0x0000ff ) << 16 )
                  +   ( m_Color2 & 0x00ff00 );

    HBRUSH    hBrush = CreateSolidBrush( color );

    FillRect( lpDrawItemStruct->hDC,
              &lpDrawItemStruct->rcItem,
              hBrush );
    if ( m_ButtonColor2.GetState() & 0x0004 )
    {
      // gedrückt
      DrawEdge( lpDrawItemStruct->hDC,
                &lpDrawItemStruct->rcItem,
                EDGE_SUNKEN,
                BF_RECT );
    }
    else
    {
      DrawEdge( lpDrawItemStruct->hDC,
                &lpDrawItemStruct->rcItem,
                EDGE_RAISED,
                BF_RECT );
    }

    DeleteObject( hBrush );
  }
}
