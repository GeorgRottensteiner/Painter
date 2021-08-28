#if !defined(AFX_DLGPTOOLBROWSER_H__58309652_2039_4597_ABEF_86C8F9AD43AC__INCLUDED_)
#define AFX_DLGPTOOLBROWSER_H__58309652_2039_4597_ABEF_86C8F9AD43AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "resource.h"

#include <MFC\BasicToolDlg.h>
#include <MFC\ODListBox.h>
#include <MFC\SplitterBar.h>

#include <Interface/INotifyMember.h>

#include <map>



class CDlgPToolBrowser : public CDialog, public INotifyMember<GR::u32>
{
  // Construction
  public:

    struct tThumbnail
    {
      HBITMAP           Bitmap;
      GR::String        FileName;
      int               Width,
                        Height,
                        Depth;

      tThumbnail() :
        Bitmap( NULL ),
        Width( 0 ),
        Height( 0 ),
        Depth( 0 )
      {
      }
    };

    bool                m_DoNotReadImages;

    typedef std::map<HTREEITEM, GR::String>   tTreeItemToFiles;

    typedef std::list<GR::String>             tFilesToLoad;


    SplitterBar        m_wndSplitter;

    GR::String          m_CurrentThumbDir;

    HANDLE              m_hWorkerThread;

    bool                m_ThreadClose,
                        m_FolderChanged,
                        m_DeletedThumbs,
                        m_CanReadNow;

    tFilesToLoad        m_FilesToLoad;


	  CDlgPToolBrowser( CWnd* pParent = NULL );

	  //{{AFX_DATA(CDlgPToolBrowser)
	  enum { IDD = IDD_PTOOL_BROWSER };
	  CListBox	m_DummyList;
	  CTreeCtrl	m_TreeFiles;
	  //}}AFX_DATA

    CODListBox      m_ListThumbs;

    CToolTipCtrl    m_ThumbsToolTip;

    HIMAGELIST      m_ImageList;

    tTreeItemToFiles  m_TreeItemToFiles;

    virtual void    OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember );

    void            FillTree();
    HTREEITEM       InsertSubDir( HTREEITEM hItemParent, const GR::String& Directory );
    void            FindPathInTree( const GR::String& Path );

    void            RefreshThumbs( const GR::String& Directory );

    void            ThumbPopup( int Index );

    DWORD           ThreadProc();


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CDlgPToolBrowser)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CDlgPToolBrowser)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemExpandingTreeFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangedTreeFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
    virtual void OnOK();
    virtual void OnCancel();

    static DWORD WINAPI       BrowserThreadProc( LPVOID lpParam );

    void QueryAndDeleteFile( const GR::String& Filename, int ItemIndex );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPTOOLBROWSER_H__58309652_2039_4597_ABEF_86C8F9AD43AC__INCLUDED_)
