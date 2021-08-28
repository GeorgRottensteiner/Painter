#if !defined(AFX_DLGFILEOPEN_H__FB715AC2_CD89_11D3_924A_00104B6795C0__INCLUDED_)
#define AFX_DLGFILEOPEN_H__FB715AC2_CD89_11D3_924A_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include <MFC/FileDialogEx.h>

#include <Grafik/GDIPage.h>



namespace OPEN_FILE
{
  namespace TYPE
  {
    const DWORD UNKNOWN               = 0;
    const DWORD EINZELBILD            = 1;
    const DWORD FONT                  = 2;
    const DWORD ANIMATION             = 3;
    const DWORD PALETTE               = 4;
  }
}



class CPainterFileOpen : public CFileDialogEx
{
	  DECLARE_DYNAMIC(CPainterFileOpen)

  public:

    // Dialogfelddaten
	  //{{AFX_DATA(CPainterFileOpen)
	  enum { IDD = IDD_DIALOG_FILE_OPEN };
		  // HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	  //}}AFX_DATA

	  CPainterFileOpen(BOOL bOpenFileDialog, // TRUE für FileOpen, FALSE für FileSaveAs
		  LPCTSTR lpszDefExt = NULL,
		  LPCTSTR lpszFileName = NULL,
		  DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		  LPCTSTR lpszFilter = NULL,
		  CWnd* pParentWnd = NULL);

  protected:

    GR::Graphic::GDIPage*        pPreviewPage;

    GR::u16                 Width,
                            Height;

    GR::u32                 Frames;

    unsigned char           BitDepth,
                            FileType;



	  //{{AFX_MSG(CPainterFileOpen)
    virtual BOOL OnInitDialog();
	  afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()

      void GetPreview( const GR::Char* Temp );

	    virtual void            OnFileNameChange();
	    virtual void            OnFolderChange();
	    virtual void            OnInitDone();
      virtual void            OnDestroy();

      virtual UINT_PTR        HookProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam );

  //    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGFILEOPEN_H__FB715AC2_CD89_11D3_924A_00104B6795C0__INCLUDED_
