#ifndef __KIMI_TPRIUGDGRID_H__
#define __KIMI_TPRIUGDGRID_H__

//#include "UGCtrl.h"
//#include "UGCTsarw.h"

#include "TPRIUGDataSource.h"

_KIMI_NAMESPACE__BEGIN

#define CWM_GRID_DBCLICKED (WM_USER + 100)

class TPRICOMMON_CLASS CTPRIUGGrid : public CUGCtrl
{
public:
	friend CTPRIUGDataSource;

	CTPRIUGGrid(void);
	virtual ~CTPRIUGGrid(void);

	CSortObArray &GetArray() { return m_pDataSource->m_array; }

	BOOL HasSelected();
	int RefreshGrid( void );
	void DrawCached();
	void AddToCache( int col, long row );
	int RedrawCell_WCache( int col, long row );
	void UpdateSort();
	void RedrawScrollBar();
protected:
	int m_nRedrawDelay;
	// draw hints list
	UGDrwHintVList *m_head;
	CFont m_fntText;	

	CTPRIUGDataSource	*m_pDataSource;
	BOOL m_bSortedAscending;
	int m_iArrowIndex;
	CUGSortArrowType m_sortArrow;
	int m_iSortCol;

	virtual BOOL CanSorted(int iCol, long lRow) { return TRUE; }
private:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTPRIUGGrid)
	//}}AFX_VIRTUAL


	//{{AFX_MSG(CTPRIUGGrid)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnSetup();	
	virtual int  OnCanSizeCol(int col);
	virtual int	 OnCanSizeRow(long row);
	virtual int  OnCanSizeTopHdg();
	virtual int  OnCanSizeSideHdg();
	virtual void OnTH_LClicked( int iCol, long lRow, int iUpdn, RECT* pRect, POINT* pPoint, BOOL bProcessed = 0 );
	virtual int OnSortEvaluate( CUGCell* pCell1, CUGCell* pCell2, int iFlags );
	//GetCellIndirect notification
	virtual void OnGetCell(int col,long row,CUGCell *cell);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
};

_KIMI_NAMESPACE__END

#endif // end of __KIMI_TPRIUGDGRID_H__