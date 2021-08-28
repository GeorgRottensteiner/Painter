#include "stdafx.h"
#include "DlgPToolFunctionOptions.h"

#include "DlgTOptionsFill.h"
#include "DlgTOptionsLine.h"
#include "DlgTOptionsFreehand2.h"
#include "DlgTOptionsSelection.h"

#include "Settings.h"

#include "PainterEnums.h"
#include ".\dlgptoolfunctionoptions.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgPToolFunctionOptions::CDlgPToolFunctionOptions(CWnd* pParent /*=NULL*/)
  : CDialog(CDlgPToolFunctionOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolFarben)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

  m_pSubDialog = NULL;

}


void CDlgPToolFunctionOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolFarben)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPToolFunctionOptions, CDialog)
	//{{AFX_MSG_MAP(CDlgPToolFunctionOptions)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




void CDlgPToolFunctionOptions::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember )
{

  switch ( NotifyMessage )
  {
    case NF_FUNCTION_CHANGED:
      // die Funktion wurde geändert
      if ( m_pSubDialog )
      {
        m_pSubDialog->DestroyWindow();
        delete m_pSubDialog;
        m_pSubDialog = NULL;
      }

      DWORD   dwResID = 0;
      switch ( pSettings->m_dwFunction )
      {
        case CSettings::F_FILL:
        case CSettings::F_SELECTION_COLOR:
          m_pSubDialog = new CDlgTOptionsFill();
          dwResID = CDlgTOptionsFill::IDD;
          break;
        case CSettings::F_LINE:
          m_pSubDialog = new CDlgTOptionsLine();
          dwResID = CDlgTOptionsLine::IDD;
          break;
        case CSettings::F_FREEHAND_2:
          m_pSubDialog = new CDlgTOptionsFreehand2();
          dwResID = CDlgTOptionsFreehand2::IDD;
          break;
        case CSettings::F_SELECTION:
          m_pSubDialog = new CDlgTOptionsSelection();
          dwResID = CDlgTOptionsSelection::IDD;
          break;
      }

      if ( m_pSubDialog )
      {
        m_pSubDialog->Create( MAKEINTRESOURCE( dwResID ), this );

        m_pSubDialog->SetWindowPos( NULL, 3, 3, 200, 135, SWP_NOZORDER | SWP_DRAWFRAME );
        m_pSubDialog->ShowWindow( SW_SHOW );

      }
      break;
  }

}



void CDlgPToolFunctionOptions::OnDestroy() 
{

  if ( m_pSubDialog )
  {
    m_pSubDialog->DestroyWindow();
    delete m_pSubDialog;
    m_pSubDialog = NULL;
  }	

	CDialog::OnDestroy();
	
}



BOOL CDlgPToolFunctionOptions::OnInitDialog() 
{

	CDialog::OnInitDialog();
	
	return TRUE;

}

void CDlgPToolFunctionOptions::OnOK()
{
}

void CDlgPToolFunctionOptions::OnCancel()
{
}
