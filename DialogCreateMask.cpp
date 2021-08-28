#include "stdafx.h"
#include "DialogCreateMask.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDialogCreateMask::CDialogCreateMask(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogCreateMask::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogCreateMask)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CDialogCreateMask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogCreateMask)
	DDX_Control(pDX, IDC_CHECK_CM_INVERT_MASK, m_CheckInvertMask);
	DDX_Control(pDX, IDC_RADIO_CM_VOLL, m_RadioCMVoll);
	DDX_Control(pDX, IDC_RADIO_CM_WORKCOLOR, m_RadioCMWorkColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogCreateMask, CDialog)
	//{{AFX_MSG_MAP(CDialogCreateMask)
	ON_BN_CLICKED(IDC_RADIO_CM_VOLL, OnRadioCmVoll)
	ON_BN_CLICKED(IDC_RADIO_CM_WORKCOLOR, OnRadioCmWorkcolor)
	ON_BN_CLICKED(IDC_CHECK_CM_INVERT_MASK, OnCheckCmInvertMask)
	ON_BN_CLICKED(IDC_RADIO_CM_SELECTION, OnRadioCmSelection)
	ON_BN_CLICKED(IDC_RADIO_CM_GRAYSCALE, OnRadioCmGrayscale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CDialogCreateMask::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_CreateMaskType  = 1;
  m_InvertMask      = true;

  m_RadioCMWorkColor.SetCheck( 1 );
  m_CheckInvertMask.SetCheck( 1 );
	
	return TRUE;
}



void CDialogCreateMask::OnRadioCmVoll() 
{
  m_CreateMaskType = 0;
}



void CDialogCreateMask::OnRadioCmWorkcolor() 
{
  m_CreateMaskType = 1;
}



void CDialogCreateMask::OnRadioCmSelection() 
{
  m_CreateMaskType = 2;
}



void CDialogCreateMask::OnRadioCmGrayscale() 
{
  m_CreateMaskType = 3;
}



void CDialogCreateMask::OnCheckCmInvertMask() 
{
  m_InvertMask = !!m_CheckInvertMask.GetCheck();
}


