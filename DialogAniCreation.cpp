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
		// HINWEIS: Der Klassen-Assistent f�gt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void DialogAniCreation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DialogAniCreation)
		// HINWEIS: Der Klassen-Assistent f�gt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DialogAniCreation, CDialog)
	//{{AFX_MSG_MAP(DialogAniCreation)
		// HINWEIS: Der Klassen-Assistent f�gt hier Zuordnungsmakros f�r Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen f�r Nachrichten DialogAniCreation 
