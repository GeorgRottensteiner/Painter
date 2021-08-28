// DialogAniCreation.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "painter.h"
#include "DialogAniCreation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld DialogAniCreation 


DialogAniCreation::DialogAniCreation(CWnd* pParent /*=NULL*/)
	: CDialog(DialogAniCreation::IDD, pParent)
{
	//{{AFX_DATA_INIT(DialogAniCreation)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void DialogAniCreation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DialogAniCreation)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DialogAniCreation, CDialog)
	//{{AFX_MSG_MAP(DialogAniCreation)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten DialogAniCreation 
