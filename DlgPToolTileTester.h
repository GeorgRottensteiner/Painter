#pragma once


#include <Interface/INotifyMember.h>

#include <vector>

#include "afxwin.h"
#include "resource.h"

#include <Grafik/GDIPage.h>




class DocumentInfo;

namespace GR
{
  namespace Graphic
  {
    class Image;
  };
};

class CDlgPToolTileTester : public CDialog, public INotifyMember<GR::u32>
{
	DECLARE_DYNAMIC(CDlgPToolTileTester)

  public:

    DocumentInfo*              m_pCurrentDocInfo;

    int                         m_iX,
                                m_iY,
                                m_iWidth,
                                m_iHeight;

    GR::Graphic::Image*         m_pImageTile;

    GR::Graphic::GDIPage        m_Page;


  	CDlgPToolTileTester(CWnd* pParent = NULL);   // Standardkonstruktor
	  virtual ~CDlgPToolTileTester();

    virtual void            OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember );

    void                    ClearTileSet();


// Dialogfelddaten
	enum { IDD = IDD_PTOOL_TILETESTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
  CEdit m_EditWidth;
  CEdit m_EditHeight;
  CComboBox m_ComboDocs;
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedButtonRefresh();
  afx_msg void OnCbnSelchangeComboDocuments();
  CStatic m_StaticTestBed;
  afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
  virtual void OnOK();
  virtual void OnCancel();
public:
  afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
  afx_msg void OnSize(UINT nType, int cx, int cy);
};
