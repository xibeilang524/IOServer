// DlgItemAttr.cpp : implementation file
//

#include "stdafx.h"
#include "DlgItemAttr.h"
#include "..\sdk\drvdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgItemAttr dialog


CDlgItemAttr::CDlgItemAttr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgItemAttr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgItemAttr)
	m_nDataType = 1;
	m_szDesc = _T("");
	m_szNote = _T("");
	m_szTagname = _T("");
	m_szUnit = _T("");
	m_szID = _T("");
	//}}AFX_DATA_INIT

	m_bAdd = FALSE;
}


void CDlgItemAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgItemAttr)
	DDX_Control(pDX, IDC_COMBO_DATATYPE, m_ctrlDataType);
	DDX_CBIndex(pDX, IDC_COMBO_DATATYPE, m_nDataType);
	DDX_Text(pDX, IDC_EDIT_DESC, m_szDesc);
	DDV_MaxChars(pDX, m_szDesc, 79);
	DDX_Text(pDX, IDC_EDIT_NOTE, m_szNote);
	DDV_MaxChars(pDX, m_szNote, 79);
	DDX_Text(pDX, IDC_EDIT_TAGNAME, m_szTagname);
	DDV_MaxChars(pDX, m_szTagname, 79);
	DDX_Text(pDX, IDC_EDIT_UNIT, m_szUnit);
	DDV_MaxChars(pDX, m_szUnit, 15);
	DDX_Text(pDX, IDC_EDIT_ID, m_szID);
	//}}AFX_DATA_MAP
}

WORD CDlgItemAttr::GetDataType()
{
	WORD wtype;
	switch(m_nDataType)
	{
	case 0:
		wtype = DRT_DIGITAL;
		break;
	case 1:
		wtype = DRT_INT32;
		break;
	case 2:
		wtype = DRT_INT64;
		break;
	case 3:
		wtype = DRT_FLOAT32;
		break;
	case 4:
		wtype = DRT_FLOAT64;
		break;
	case 5:
		wtype = DRT_STRING;
		break;
	case 6:
		wtype = DRT_BLOB;
		break;
	default:
		wtype = DRT_VOID;
		break;;
	}
	return wtype;
}

void CDlgItemAttr::SetDataType(WORD wtype)
{
	switch(wtype)
	{
	case DRT_DIGITAL:
		m_nDataType = 0;
		break;
	case DRT_INT32:
		m_nDataType = 1;
		break;
	case DRT_INT64:
		m_nDataType = 2;
		break;
	case DRT_FLOAT32:
		m_nDataType = 3;
		break;
	case DRT_FLOAT64:
		m_nDataType = 4;
		break;
	case DRT_STRING:
		m_nDataType = 5;
		break;
	case DRT_BLOB:
		m_nDataType = 6;
		break;
	default:
		m_nDataType = 0;
		break;;
	}
}

BEGIN_MESSAGE_MAP(CDlgItemAttr, CDialog)
	//{{AFX_MSG_MAP(CDlgItemAttr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDlgItemAttr message handlers

BOOL CDlgItemAttr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_bAdd)
		SetWindowText("添加数据项");
	else
	{
		CString szt;
		szt.Format("%s<%s>","修改数据项", m_szTagname);//修改数据项
		SetWindowText(szt);
		GetDlgItem(IDC_EDIT_TAGNAME)->EnableWindow(FALSE);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgItemAttr::OnOK() 
{
	// TODO: Add extra validation here
	//	this->UpdateData(TRUE);
	if(!UpdateData(TRUE))
		return;
	if(m_szTagname.IsEmpty())
	{
		AfxMessageBox("标签名不能为空!");
		return;
	}
	
	CDialog::OnOK();
}
