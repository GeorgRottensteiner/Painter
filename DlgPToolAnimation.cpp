#include "stdafx.h"
#include "painter.h"
#include "DlgPToolAnimation.h"

#include <String/Convert.h>



IMPLEMENT_DYNAMIC(CDlgPToolAnimation, CDialog)
CDlgPToolAnimation::CDlgPToolAnimation(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPToolAnimation::IDD, pParent),
    m_pViewInfo( NULL )
{
}

CDlgPToolAnimation::~CDlgPToolAnimation()
{
}

void CDlgPToolAnimation::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT_DELAY, m_EditDelay);
  DDX_Control(pDX, IDC_BUTTON_PLAY_ANIM, m_BtnPlay);
  DDX_Control(pDX, IDC_BUTTON_STOP_ANIM, m_BtnStop);
  DDX_Control(pDX, IDC_STATIC_DELAY, m_StaticDelay);
}


BEGIN_MESSAGE_MAP(CDlgPToolAnimation, CDialog)
  ON_BN_CLICKED(IDC_BUTTON_PLAY_ANIM, OnBnClickedButtonPlayAnim)
  ON_BN_CLICKED(IDC_BUTTON_STOP_ANIM, OnBnClickedButtonStopAnim)
  ON_EN_CHANGE(IDC_EDIT_DELAY, OnEnChangeEditDelay)
END_MESSAGE_MAP()


// CDlgPToolAnimation-Meldungshandler

void CDlgPToolAnimation::OnBnClickedButtonPlayAnim()
{
  // TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
}

void CDlgPToolAnimation::OnBnClickedButtonStopAnim()
{
  // TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
}

BOOL CDlgPToolAnimation::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_EditDelay.SetWindowText( _T( "100" ) );

  return TRUE;
}



void CDlgPToolAnimation::OnEnChangeEditDelay()
{
  CString   cstrGnu;
  m_EditDelay.GetWindowText( cstrGnu );

  GR::u32 delay = GR::Convert::ToU32( (LPCTSTR)cstrGnu );

  if ( m_pViewInfo )
  {
    m_pViewInfo->m_pDocInfo->GetFrame( m_pViewInfo->m_pDocInfo->CurrentFrame() )->Delay = delay;
  }
}



void CDlgPToolAnimation::OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember )
{
  switch ( NotifyMessage )
  {
    case NF_LAYER_CHANGED:
      if ( m_pViewInfo )
      {
        m_EditDelay.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_pViewInfo->m_pDocInfo->GetFrame( m_pViewInfo->m_pDocInfo->CurrentFrame() )->Delay ) ).c_str() );
      }
      break;
    case NF_ACTIVE_VIEW_CHANGED:
      m_pViewInfo = pSettings->m_pActiveViewInfo;
      if ( m_pViewInfo == NULL )
      {
        m_BtnPlay.EnableWindow( FALSE );
        m_BtnStop.EnableWindow( FALSE );
        m_EditDelay.EnableWindow( FALSE );
        m_StaticDelay.EnableWindow( FALSE );
      }
      else
      {
        m_BtnPlay.EnableWindow();
        m_BtnStop.EnableWindow();
        m_EditDelay.EnableWindow();
        m_StaticDelay.EnableWindow();

        if ( !m_pViewInfo->m_pDocInfo->m_LayeredFrames.empty() )
        {
          m_EditDelay.SetWindowText( GR::Convert::ToUTF16( GR::Convert::ToString( m_pViewInfo->m_pDocInfo->GetFrame( m_pViewInfo->m_pDocInfo->CurrentFrame() )->Delay ) ).c_str() );
        }
      }
      break;
  }
}