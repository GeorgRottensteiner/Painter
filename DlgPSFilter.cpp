#include "stdafx.h"
#include "painter.h"
#include "DlgPSFilter.h"
#include "PhotoshopFilterHandler.h"

#include <String/Convert.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgPSFilter::CDlgPSFilter(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPSFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPSFilter)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgPSFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPSFilter)
	DDX_Control(pDX, IDC_TREE_PS_FILTER, m_TreeFilter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPSFilter, CDialog)
	//{{AFX_MSG_MAP(CDlgPSFilter)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PS_FILTER, OnDblclkTreePsFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




BOOL CDlgPSFilter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  std::list<CPhotoShopFilterEntry>::iterator  it( pSettings->m_listPhotoShopFilter.begin() );
  while ( it != pSettings->m_listPhotoShopFilter.end() )
  {
    CPhotoShopFilterEntry* pFE = &(*it);

    HTREEITEM   hItemParent = NULL;

    HTREEITEM   hItemTemp = m_TreeFilter.GetRootItem();

    while ( hItemTemp != NULL )
    {
      CString   cstrGnu = m_TreeFilter.GetItemText( hItemTemp );

      if ( cstrGnu.Compare( GR::Convert::ToUTF16( pFE->m_strCategory ).c_str() ) == 0 )
      {
        hItemParent = hItemTemp;
        break;
      }

      hItemTemp = m_TreeFilter.GetNextSiblingItem( hItemTemp );
    }

    if ( hItemParent == NULL )
    {
      hItemParent = m_TreeFilter.InsertItem( GR::Convert::ToUTF16( pFE->m_strCategory ).c_str() );
    }

    HTREEITEM   hItemFilter = m_TreeFilter.InsertItem( GR::Convert::ToUTF16( pFE->m_strDescription ).c_str(), hItemParent );

    m_TreeFilter.SetItemData( hItemFilter, (DWORD_PTR)pFE );

    it++;
  }
	return TRUE;
}



void CDlgPSFilter::OnDblclkTreePsFilter( NMHDR* pNMHDR, LRESULT* pResult )
{
  HTREEITEM   hItem = m_TreeFilter.GetSelectedItem();

  CPhotoShopFilterEntry*    pFilter = (CPhotoShopFilterEntry*)m_TreeFilter.GetItemData( hItem );

  if ( pFilter )
  {
    PhotoShopFilterHandler.OpenPhotoShopFilter( *pSettings->m_pActiveViewInfo, pFilter->m_FileName );
  }
	
	*pResult = 0;
}
