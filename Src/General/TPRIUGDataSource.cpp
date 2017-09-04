#include "StdAfx.h"
#include "GeneralAPI.h"
#include "General\TPRIUGDataSource.h"
#include "General\TPRIUGGrid.h"

TRUNK_NAMESPACE_BEGIN

CTPRIUGDataSource::CTPRIUGDataSource(void)
{
	m_array.SetGrown(1024 * 4);
}

CTPRIUGDataSource::~CTPRIUGDataSource(void)
{
	m_array.DeleteObjects();
}

CTPRIUGDataSource::CTPRIUGDataSource(const CTPRIUGDataSource& object) {
	// TODO: Insert code to initialize members 
}

CTPRIUGDataSource CTPRIUGDataSource::operator=(const CTPRIUGDataSource& object){
	// TODO: Insert code to copy members 
	return *this;
}

bool operator==(const CTPRIUGDataSource& object1, const CTPRIUGDataSource& object2) {
	// TODO: Insert code to perform comparison
	return true;
}

bool operator!=(const CTPRIUGDataSource& object1, const CTPRIUGDataSource& object2) {
	if(object1 == object2)
		return false;
	else
		return true;
}

/////////////////////////////////////////////////////////////////////
// CUGDataSource 重载函数 - 因为这是一个只读的数据源，所以只需要重载几个基本的就可以了

long CTPRIUGDataSource::GetNumRows()
{	
	return (long)m_array.GetSize();
}

int CTPRIUGDataSource::GetNumCols()
{
	return (int)m_arrHeadSize.GetCount();
}

/***********************************************************************
	GetCell()
	// this is the heart of any datasource.  Given a col and row, fill
	// the cell with data for display...
 ***********************************************************************/
int	CTPRIUGDataSource::GetCell(int col, long row, CUGCell *cell)
{
	if (col > GetNumCols() - 1)
		return UG_NA;

	if(row > (long)m_array.GetSize() - 1)
		return UG_NA;

	// 行和列号不能小于-1
	if(-1 > col || -1 > row)
		return UG_NA;

	// 不需要处理左上角[-1,-1]格
	if(-1 == col && -1 == row)
		return UG_NA;
	
	// 左边-1列的头，不显示任何东东
	if(-1 == col)	{
		return UG_NA;
	}
	
	// 这是标题头
	if (-1 == row) 
	{
		cell->SetText( GetHeadText( col ) );
		CTPRIUGGrid *pGrid = (CTPRIUGGrid*)m_ctrl;
		if (pGrid->m_iSortCol == col) {
			cell->SetCellType( pGrid->m_iArrowIndex );
			cell->SetCellTypeEx(pGrid->m_bSortedAscending ? UGCT_SORTARROWUP : UGCT_SORTARROWDOWN);
		}
		return UG_SUCCESS;
	}

	return UG_SUCCESS;
}

/***********************************************************************
	FindNext()
	// this is a very generic FindNext routine that almost any 
	// datasource can use - the idea is to start from the 
	// current row and return the location of a match in the col & row
	// along with UG_SUCCESS if found.

	// this implementation does not 'wrap' - reporting 'not found' when
	// top or bottom of ds is reached.  The implementation in CUGMem
	// (the memory based default ds for UG) will continue to the top
	// or bottom)

	// When you call FindDialog in your derived grid class,
	// CUGCtrl::ProcessFindDialog() will call FindNext in the active
	// datasource...
 ***********************************************************************/
int CTPRIUGDataSource::FindNext(CString *string, int *col, long *row, int flags) 
{
	int  retval = UG_SUCCESS;

	long nStartRow = m_ctrl->GetCurrentRow();
	long nSearchRow = nStartRow;

	int  nMaxCol = GetNumCols()-1;

	int  nStartColumn = *col;
	int  nSearchColumn = *col;
	CUGCell cell;
	CString strCell;


	while(1) {

		// positioning...
		if(UG_FIND_ALLCOLUMNS&flags) {
			if(UG_FIND_UP&flags) {
				nSearchColumn--;
				if(nSearchColumn < 0) {
					nSearchColumn = nMaxCol;
					nSearchRow--;
				}
			}
			else {
				nSearchColumn++;
				if(nSearchColumn > nMaxCol) {
					nSearchColumn = 0;
					nSearchRow++;
				}
			}
		}
		else {
		
			if(UG_FIND_UP&flags)
				nSearchRow--;
			else
				nSearchRow++;

		}

		if(nSearchRow < 0) {		// don't search headings...
			// reset to last position
			
			retval = UG_NA;
			break;
		}
		
		if(GetCell(nSearchColumn, nSearchRow, &cell) != UG_SUCCESS) { 
			retval = UG_NA;			// no more data.
			break;
		}

		cell.GetText(&strCell);		// get the text.

		if(UG_FIND_CASEINSENSITIVE&flags) {
			// make both upper case
			string->MakeUpper();
			strCell.MakeUpper();
		}

		if(UG_FIND_PARTIAL&flags) {
			if(strCell.Find(*string) != -1) {		// compare partial
				*col = nSearchColumn;
				*row = nSearchRow;
				retval = UG_SUCCESS;
				break;
			}
		}
		else {
			if(strCell == *string) {					// match whole cell
				*col = nSearchColumn;
				*row = nSearchRow;
				retval = UG_SUCCESS;
				break;
			}
		}
	}		
	
	return retval;

}

/***************************************************
RefreshGrid
	Purpose of this function is to make sure that the grid
	gets fresh data from the database.

	This is simply done by calling FetchNewRows function looking
	for current top row.  Also I determine here if rows were
	added or deleted, so the grid will show all fetchd rows.
Params:
	<none>
Returns:
	UG_SUCCESS on success, this function will never fail.
***************************************************/
int CTPRIUGDataSource::RefreshGrid( void )
{
	long nRow = GetNumRows();
	if (m_ctrl->GetNumberRows() != nRow)
		m_ctrl->SetNumberRows(nRow);

	// update grid's view
	CRect rect;
	m_ctrl->GetRangeRect( m_ctrl->GetLeftCol(),
						  m_ctrl->GetTopRow(),
						  m_ctrl->GetRightCol(),
						  m_ctrl->GetBottomRow(),
						  rect );
	m_ctrl->m_CUGGrid->m_drawHint.ClearHints();
	m_ctrl->m_CUGGrid->m_drawHint.AddHint( m_ctrl->GetLeftCol(),
						  m_ctrl->GetTopRow(),
						  m_ctrl->GetRightCol(),
						  m_ctrl->GetBottomRow());
	m_ctrl->m_CUGGrid->PaintDrawHintsNow( rect );
	m_ctrl->m_CUGGrid->m_drawHint.ClearHints();
	m_ctrl->m_CUGSideHdg->Update();

	return UG_SUCCESS;
}

int CTPRIUGDataSource::SortBy(int col,int flags)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(flags);
	
	m_array.SetSortType(col, (flags == UG_SORT_ASCENDING));
	m_array.Qsort();
	
	return UG_SUCCESS;
}

TRUNK_NAMESPACE_END