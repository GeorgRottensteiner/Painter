#include "stdafx.h"
#include "JpegSaveOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CJpegSaveOptions::CJpegSaveOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CJpegSaveOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJpegSaveOptions)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void CJpegSaveOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJpegSaveOptions)
	DDX_Control(pDX, IDC_RADIO_JPEG_FAST, m_RadioJPGFast);
	DDX_Control(pDX, IDC_RADIO_JPEG_ACCURATE, m_RadioJPGAccurate);
	DDX_Control(pDX, IDC_RADIO_JPEG_DURCHSCHNITT, m_RadioJPGPerfect);
	DDX_Control(pDX, IDC_RADIO_JPEG_GUT, m_RadioJPGGood);
	DDX_Control(pDX, IDC_RADIO_JPEG_NORMAL, m_RadioJPGNormal);
  DDX_Control(pDX, IDC_RADIO_JPEG_PERFEKT, m_RadioJPGAverage);
	DDX_Control(pDX, IDC_RADIO_JPEG_SCHLECHT, m_RadioJPGBad);
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJpegSaveOptions, CDialog)
	//{{AFX_MSG_MAP(CJpegSaveOptions)
	ON_BN_CLICKED(IDC_RADIO_JPEG_FAST, OnRadioJpegFast)
	ON_BN_CLICKED(IDC_RADIO_JPEG_ACCURATE, OnRadioJpegAccurate)
	ON_BN_CLICKED(IDC_RADIO_JPEG_DURCHSCHNITT, OnRadioJpegDurchschnitt)
	ON_BN_CLICKED(IDC_RADIO_JPEG_GUT, OnRadioJpegGut)
	ON_BN_CLICKED(IDC_RADIO_JPEG_NORMAL, OnRadioJpegNormal)
	ON_BN_CLICKED(IDC_RADIO_JPEG_PERFEKT, OnRadioJpegPerfekt)
	ON_BN_CLICKED(IDC_RADIO_JPEG_SCHLECHT, OnRadioJpegSchlecht)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CJpegSaveOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_RadioJPGFast.SetCheck( 1 );
	return TRUE;  
}



void CJpegSaveOptions::OnRadioJpegFast() 
{
  m_SaveOption = 1;
}



void CJpegSaveOptions::OnRadioJpegAccurate() 
{
  m_SaveOption = 2;
}



void CJpegSaveOptions::OnRadioJpegDurchschnitt() 
{
  m_SaveOption = 6;
}



void CJpegSaveOptions::OnRadioJpegGut() 
{
  m_SaveOption = 4;
}



void CJpegSaveOptions::OnRadioJpegNormal() 
{
  m_SaveOption = 5;
}



void CJpegSaveOptions::OnRadioJpegPerfekt() 
{
  m_SaveOption = 3;
}



void CJpegSaveOptions::OnRadioJpegSchlecht() 
{
  m_SaveOption = 7;
}
