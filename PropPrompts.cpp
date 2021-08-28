#include "stdafx.h"
#include "PropPrompts.h"

#include <WinSys\RegistryHelper.h>

#include <String/Convert.h>



#pragma warning( disable:4786 )
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPropPrompts, CPropertyPage)

CPropPrompts::CPropPrompts() : CPropertyPage(CPropPrompts::IDD)
{
	//{{AFX_DATA_INIT(CPropPrompts)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

CPropPrompts::~CPropPrompts()
{
}

void CPropPrompts::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPrompts)
	DDX_Control(pDX, IDC_CHECK_REGISTER_PNT, m_CheckPNT);
	DDX_Control(pDX, IDC_CHECK_REGISTER_FNX, m_CheckFNX);
	DDX_Control(pDX, IDC_CHECK_REGISTER_FNH, m_CheckFNH);
	DDX_Control(pDX, IDC_CHECK_REGISTER_FILETYPES, m_CheckIGF);
	DDX_Control(pDX, IDC_CHECK_REGISTER_BTN, m_CheckBTN);
	DDX_Control(pDX, IDC_CHECK_REGISTER_BTH, m_CheckBTH);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPrompts, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPrompts)
	ON_BN_CLICKED(IDC_CHECK_REGISTER_FILETYPES, OnCheckRegisterFileTypes)
	ON_BN_CLICKED(IDC_CHECK_REGISTER_BTN, OnCheckRegisterBtn)
	ON_BN_CLICKED(IDC_CHECK_REGISTER_BTH, OnCheckRegisterBth)
	ON_BN_CLICKED(IDC_CHECK_REGISTER_FNX, OnCheckRegisterFnx)
	ON_BN_CLICKED(IDC_CHECK_REGISTER_FNH, OnCheckRegisterFnh)
	ON_BN_CLICKED(IDC_CHECK_REGISTER_PNT, OnCheckRegisterPNT)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPropPrompts 



void CPropPrompts::OnOK() 
{
	
	CPropertyPage::OnOK();

}



void CPropPrompts::OnCheckRegisterFileTypes() 
{
  GR::WChar    szDummy[MAX_PATH];
  GetModuleFileNameW( NULL, szDummy, MAX_PATH );

  GR::String   executable = GR::Convert::ToUTF8( szDummy );
  GR::String   strKey = executable + " \"%1\"";

  if ( m_CheckIGF.GetCheck() )
  {
    Registry::RegisterFileType( ".igf", "Painter Image", executable.c_str(), strKey.c_str() );
  }
  else
  {
    Registry::UnregisterFileType( ".igf", "Painter Image" );
  }
}



void CPropPrompts::OnCheckRegisterBtn() 
{
  GR::WChar    szDummy[MAX_PATH];
  GetModuleFileNameW( NULL, szDummy, MAX_PATH );

  GR::String   executable = GR::Convert::ToUTF8( szDummy );
  GR::String   strKey = executable + " \"%1\"";

  if ( m_CheckBTN.GetCheck() )
  {
    Registry::RegisterFileType( ".btn", "Painter Image", executable.c_str(), strKey.c_str() ); 
  }
  else
  {
    Registry::UnregisterFileType( ".btn", "Painter Image" );
  }
}



void CPropPrompts::OnCheckRegisterBth() 
{
  GR::WChar    szDummy[MAX_PATH];
  GetModuleFileNameW( NULL, szDummy, MAX_PATH );

  GR::String   executable = GR::Convert::ToUTF8( szDummy );
  GR::String   strKey = executable + " \"%1\"";

  if ( m_CheckBTH.GetCheck() )
  {
    Registry::RegisterFileType( ".bth", "Painter Image", executable.c_str(), strKey.c_str() ); 
  }
  else
  {
    Registry::UnregisterFileType( ".bth", "Painter Image" );
  }
}



void CPropPrompts::OnCheckRegisterFnx() 
{
  GR::WChar    szDummy[MAX_PATH];
  GetModuleFileNameW( NULL, szDummy, MAX_PATH );

  GR::String   executable = GR::Convert::ToUTF8( szDummy );
  GR::String   strKey = executable + " \"%1\"";

  if ( m_CheckFNX.GetCheck() )
  {
    Registry::RegisterFileType( ".fnx", "Painter Image", executable.c_str(), strKey.c_str() ); 
  }
  else
  {
    Registry::UnregisterFileType( ".fnx", "Painter Image" );
  }
}



void CPropPrompts::OnCheckRegisterFnh() 
{
  GR::WChar    szDummy[MAX_PATH];
  GetModuleFileNameW( NULL, szDummy, MAX_PATH );

  GR::String   executable = GR::Convert::ToUTF8( szDummy );
  GR::String   strKey = executable + " \"%1\"";

  if ( m_CheckFNH.GetCheck() )
  {
    Registry::RegisterFileType( ".fnh", "Painter Image", executable.c_str(), strKey.c_str() ); 
  }
  else
  {
    Registry::UnregisterFileType( ".fnh", "Painter Image" );
  }
}



BOOL CPropPrompts::OnInitDialog() 
{

	CPropertyPage::OnInitDialog();
	

  m_CheckIGF.SetCheck( Registry::IsFileTypeRegisteredTo( ".igf", "Painter Image" ) );
  m_CheckBTN.SetCheck( Registry::IsFileTypeRegisteredTo( ".btn", "Painter Image" ) );
  m_CheckBTH.SetCheck( Registry::IsFileTypeRegisteredTo( ".bth", "Painter Image" ) );
  m_CheckFNH.SetCheck( Registry::IsFileTypeRegisteredTo( ".fnh", "Painter Image" ) );
  m_CheckFNX.SetCheck( Registry::IsFileTypeRegisteredTo( ".fnx", "Painter Image" ) );
  m_CheckPNT.SetCheck( Registry::IsFileTypeRegisteredTo( ".pnt", "Painter Image" ) );
	
	return TRUE;

}



void CPropPrompts::OnCheckRegisterPNT() 
{
  GR::WChar    szDummy[MAX_PATH];
  GetModuleFileNameW( NULL, szDummy, MAX_PATH );

  GR::String   executable = GR::Convert::ToUTF8( szDummy );
  GR::String   strKey = executable + " \"%1\"";

  if ( m_CheckPNT.GetCheck() )
  {
    Registry::RegisterFileType( ".pnt", "Painter Image", executable.c_str(), strKey.c_str() ); 
  }
  else
  {
    Registry::UnregisterFileType( ".pnt", "Painter Image" );
  }
}



