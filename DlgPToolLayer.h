#if !defined(AFX_DLGPTOOLLAYER_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_)
#define AFX_DLGPTOOLLAYER_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolFarben.h : Header-Datei
//

#include "resource.h"

#include <afxole.h>

#include <Interface/INotifyMember.h>

#include <GR/GRTypes.h>



class DocumentInfo;
class CLayer;
class CDlgPToolLayer;

class CDragMaskTarget : public COleDropTarget
{
  public:

    CDlgPToolLayer    *m_pToolLayer;

    virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
    virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
    virtual BOOL OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
    /*
    virtual void OnDragLeave( CWnd* pWnd );
    
    */

};


class CDlgPToolLayer : public CDialog, public INotifyMember<GR::u32>
{
  // Konstruktion
  public:

    enum eConstants
    {
      LAYER_ACT_ICON_X = 2,
      LAYER_ACT_ICON_Y = 2,
      LAYER_VIS_ICON_X = 20,
      LAYER_VIS_ICON_Y = 2,
      LAYER_MSK_ICON_X = 38,
      LAYER_MSK_ICON_Y = 2,
      LAYER_TSP_ICON_X = 56,
      LAYER_TSP_ICON_Y = 2,
      LAYER_NAME_X     = 74,
      LAYER_NAME_Y     = 2,
      LAYER_NAME_W     = 60,
      LAYER_NAME_H     = 16,
      LAYER_ALPHA_X    = 140,
      LAYER_ALPHA_Y    = 2,
      LAYER_ALPHA_W    = 56,
      LAYER_ALPHA_H    = 16,
      LAYER_ITEM_HEIGHT = 20,
    };

    CDlgPToolLayer(CWnd* pParent = NULL);   // Standardkonstruktor

    CDragMaskTarget ddMaskTarget;

    DocumentInfo   *m_pDocInfo;
    BOOL            bClipping;
    CLayer          *pClipLayer;

    POINT           m_ptButtonDown;

    HICON           iconInactiveLayer,
                    iconActiveLayer,
                    iconVisibleLayer,
                    iconActiveMask,
                    iconInactiveMask,
                    iconColorKeyLayer;

    HICON           m_hIconNewLayer,
                    m_hIconMergeVisiblem_vectLayers,
                    m_hIconMergeAllm_vectLayers,
                    m_hIconCreateLayerMask;


    void UpdateControls( void );

    void                DoPopup( CLayer* pLayer, int iLayerIndex );


    void LBAlphaRegler( int iX, RECT& rc );


    virtual void    OnNotify( const GR::u32& NotifyMessage, INotifyMember<GR::u32>* pFromMember );

// Dialogfelddaten
	//{{AFX_DATA(CDlgPToolLayer)
	enum { IDD = IDD_PTOOL_LAYER};
	CButton	m_ButtonMergeVisible;
	CButton	m_ButtonCreateMask;
	CButton	m_ButtonMergeAll;
	CButton	m_ButtonNewLayer;
	CDragListBox	m_Listm_vectLayers;
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA



// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgPToolLayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

  // Implementierung
  protected:


    LRESULT             WindowProc(UINT message, WPARAM wParam, LPARAM lParam );

    void                SetActiveLayer( size_t iLayerIndex );

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgPToolLayer)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonToolLayerNewLayer();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnButtonToolLayerMergeAll();
	afx_msg void OnButtonToolLayerMerge();
	afx_msg void OnButtonToolLayerCreateMask();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
  virtual void OnOK();
  virtual void OnCancel();
public:
  afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGPTOOLLAYER_H__9F96B784_16FD_11D6_AC02_00104B6795C0__INCLUDED_
