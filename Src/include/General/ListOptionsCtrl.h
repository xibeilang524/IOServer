/*
Module : ListptionsCtrl.h
Purpose: Defines the interface for an MFC class to implement a list options control 
         similiar to properties control which appears in Visual Basic 6
Created: PJN / 04-12-2001

Copyright (c) 2001 - 2006 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////

#ifndef __LISTOPTIONSCTRL_H__
#define __LISTOPTIONSCTRL_H__

#define DWORD_PTR DWORD
/////////////////////////////// Includes //////////////////////////////////////

#ifndef __AFXDTCTL_H__
#pragma message("To avoid this message please put afxdtctl.h in your PCH")
#include <afxdtctl.h>
#endif

TRUNK_NAMESPACE_BEGIN

/////////////////////////////// Classes ///////////////////////////////////////

//forward declaration
class CListOptionsCtrl;
class CListOptionsBrowseButton;

//Class which represents a combo box used by the list options class
class _GENERAL_CLASS CListOptionsCombo : public CComboBox
{
public:
//Constructors / Destructors
	CListOptionsCombo();
	virtual ~CListOptionsCombo() {};

public:
//Misc methods
  void SetButtonBuddy(CListOptionsBrowseButton* pButtonCtrl) { m_pButtonCtrl = pButtonCtrl; };
  void SetListBuddy(CListOptionsCtrl* pListCtrl) { m_pListCtrl = pListCtrl; };
  void SetListIndex(int nIndex) { m_nListCtrlIndex = nIndex; };

//Virtual methods
  virtual DWORD GetWindowStyle();
  virtual int   GetDropDownHeight();
  virtual BOOL  IsRelatedWnd(CWnd* pChild);
  virtual void  OnShowNewValue() {};

protected:
	//{{AFX_VIRTUAL(CListOptionsCombo)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsCombo)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsCombo)

//Member variables
  CListOptionsCtrl* m_pListCtrl;
  CListOptionsBrowseButton* m_pButtonCtrl;
  int m_nListCtrlIndex;
  BOOL m_bDoNotDestroyUponLoseFocus;
  friend class CListOptionsCtrl;
};

//Class which represents a combo box which allows a Font Name to be specified
class _GENERAL_CLASS CListOptionsFontNameCombo : public CListOptionsCombo
{
public:
//Constructors / Destructors
	virtual ~CListOptionsFontNameCombo() {};

protected:
	//{{AFX_VIRTUAL(CListOptionsFontNameCombo)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsFontNameCombo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsFontNameCombo)

//Misc Methods
  virtual DWORD GetWindowStyle();
  int EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm, DWORD dwType);
  static int CALLBACK _EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData);
};

//Class which represents a combo box which allows a True / False value to be specified
class _GENERAL_CLASS CListOptionsBooleanCombo : public CListOptionsCombo
{
public:
//Constructors / Destructors
	virtual ~CListOptionsBooleanCombo() {};

protected:
	//{{AFX_VIRTUAL(CListOptionsBooleanCombo)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsBooleanCombo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsBooleanCombo)
};

//ÎÄ±¾¶ÔÆë
class _GENERAL_CLASS CListOptionsAlignCombo : public CListOptionsCombo
{
public:
//Constructors / Destructors
	virtual ~CListOptionsAlignCombo() {};

protected:
	//{{AFX_VIRTUAL(CListOptionsAlignCombo)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsAlignCombo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsAlignCombo)
};

//forward declaration
class CListOptionsBrowseButton;

//Class which represents an edit box used by the list options class
class _GENERAL_CLASS CListOptionsEdit : public CEdit
{
public:
//Constructors / Destructors
	CListOptionsEdit();
	virtual ~CListOptionsEdit() {};

public:
//Misc methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) { m_pListCtrl = pListCtrl; };
  void SetButtonBuddy(CListOptionsBrowseButton* pButtonCtrl) { m_pButtonCtrl = pButtonCtrl; };
  void SetListIndex(int nIndex) { m_nListCtrlIndex = nIndex; };

//Virtual methods
  virtual DWORD   GetWindowStyle();
  virtual int     GetHeight(int nItemHeight);
  virtual void    BrowseForFolder(const CString& sInitialFolder);
  virtual void    BrowseForFile(const CString& sInitialFile);
  virtual CString GetBrowseForFolderCaption();
  virtual CString GetBrowseForFileCaption();
  virtual CString GetFileExtensionFilter();
  virtual void    OnShowNewValue() {};

protected:
	//{{AFX_VIRTUAL(CListOptionsEdit)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

  static int CALLBACK SHBrowseSetSelProc(HWND hWnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsEdit)

//Member variables
  CListOptionsCtrl* m_pListCtrl;
  CListOptionsBrowseButton* m_pButtonCtrl;
  int m_nListCtrlIndex;
  BOOL m_bDoNotDestroyUponLoseFocus;
  
  friend class CListOptionsCtrl;
  friend class CListOptionsBrowseButton;
};

//Class which represents the spin control which can be used in association with an edit box by the list options class
class _GENERAL_CLASS CListOptionsSpinCtrl : public CSpinButtonCtrl
{
public:
//Constructors / Destructors
	CListOptionsSpinCtrl();
	virtual ~CListOptionsSpinCtrl() {};

public:
//Misc methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl);
  void SetEditBuddy(CListOptionsEdit* pEdit);
  void SetListIndex(int nIndex) { m_nListCtrlIndex = nIndex; };
  virtual DWORD GetWindowStyle();
  virtual void GetDefaultRange(int &lower, int& upper);

protected:
	//{{AFX_VIRTUAL(CListOptionsSpinCtrl)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsSpinCtrl)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsSpinCtrl)

//Member variables
  CListOptionsCtrl* m_pListCtrl;
  int m_nListCtrlIndex;
  CListOptionsEdit* m_pEdit;
  friend class CListOptionsCtrl;
};

//Class which represents the browse button which can be used in association with an edit box by the list options class
class _GENERAL_CLASS CListOptionsBrowseButton : public CButton
{
public:
//Constructors / Destructors
	CListOptionsBrowseButton();
	virtual ~CListOptionsBrowseButton() {};

public:
//Misc methods
  void            SetListBuddy(CListOptionsCtrl* pListCtrl);
  void            SetListIndex(int nIndex) { m_nListCtrlIndex = nIndex; };
  void            SetEditBuddy(CListOptionsEdit* pEdit);
  void            SetComboBuddy(CListOptionsCombo* pCombo);
  virtual DWORD   GetWindowStyle();
  virtual int     GetWidth();
  virtual CString GetCaption();
  COLORREF        GetColor() const { return m_Color; };
  void            SetColor(COLORREF color);
  void            GetFontItem(LOGFONT* pLogFont);
  void            SetFontItem(const LOGFONT* pLogFont);

//Virtual methods
  virtual void    BrowseForColor();
  virtual void    BrowseForFont();
  virtual void    BrowseForOpaque();
  virtual void    OnShowNewValue() {};

protected:
	//{{AFX_VIRTUAL(CListOptionsBrowseButton)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsBrowseButton)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsBrowseButton)

//Member variables
  COLORREF m_Color;
  LOGFONT m_Font;
  CListOptionsCtrl* m_pListCtrl;
  CListOptionsEdit* m_pEdit;
  CListOptionsCombo* m_pCombo;
  int m_nListCtrlIndex;
  friend class CListOptionsCtrl;
};

//Class which is used for browsing for filenames
class _GENERAL_CLASS CListOptionsFileDialog : public CFileDialog
{
public:
//Constructors / Destructors
	CListOptionsFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL,
                       	 DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);

protected:
	DECLARE_DYNAMIC(CListOptionsFileDialog)

  virtual void OnInitDone();

	//{{AFX_MSG(CListOptionsFileDialog)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//Class which represents a date / time control used by the list options class
class _GENERAL_CLASS CListOptionsDateCtrl : public CDateTimeCtrl
{
public:
//Constructors / Destructors
	CListOptionsDateCtrl();
	virtual ~CListOptionsDateCtrl() {};

//Methods
  virtual CString GetDisplayText(const SYSTEMTIME& st);

//Misc methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) { m_pListCtrl = pListCtrl; };
  void SetListIndex(int nIndex) { m_nListCtrlIndex = nIndex; };
  void GetDateTime(SYSTEMTIME& st) const { CopyMemory(&st, &m_SystemTime, sizeof(SYSTEMTIME)); };
  void SetDateTime(const SYSTEMTIME& st) { CopyMemory(&m_SystemTime, &st, sizeof(SYSTEMTIME)); };

//Virtual methods
  virtual DWORD GetWindowStyle();
  virtual BOOL IsRelatedWnd(CWnd* pChild);
  virtual void OnShowNewValue() {};

protected:
	//{{AFX_VIRTUAL(CListOptionsDateCtrl)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsDateCtrl)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsDateCtrl)

//Member variables
  CListOptionsCtrl* m_pListCtrl;
  int m_nListCtrlIndex;
  BOOL m_bDoNotDestroyUponLoseFocus;
  friend class CListOptionsCtrl;
  SYSTEMTIME m_SystemTime;
};

//Class which represents a time control used by the list options class
class _GENERAL_CLASS CListOptionsTimeCtrl : public CListOptionsDateCtrl
{
public:
//Constructors / Destructors
	virtual ~CListOptionsTimeCtrl() {};

//methods
  virtual CString GetDisplayText(const SYSTEMTIME& st);

protected:
  virtual DWORD GetWindowStyle();

	//{{AFX_VIRTUAL(CListOptionsTimeCtrl)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsTimeCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

  DECLARE_DYNCREATE(CListOptionsTimeCtrl)
};

//Class which represents IP Address control used by the list options class
class _GENERAL_CLASS CListOptionsIPAddressCtrl : public CIPAddressCtrl
{
public:
//Constructors / Destructors
	CListOptionsIPAddressCtrl();
	virtual ~CListOptionsIPAddressCtrl() {};

//methods
  virtual CString GetDisplayText(DWORD dwAddress);

//Misc methods
  void  SetListBuddy(CListOptionsCtrl* pListCtrl) { m_pListCtrl = pListCtrl; };
  void  SetListIndex(int nIndex) { m_nListCtrlIndex = nIndex; };
  DWORD GetIPAddress() const { return m_dwAddress; };
  void  SetIPAddress(DWORD dwAddress) { m_dwAddress = dwAddress; };

//Virtual methods
  virtual DWORD GetWindowStyle();
  virtual BOOL IsRelatedWnd(CWnd* pChild);
  virtual void OnShowNewValue() {};

protected:
	//{{AFX_VIRTUAL(CListOptionsIPAddressCtrl)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsIPAddressCtrl)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsIPAddressCtrl)

//Member variables
  CListOptionsCtrl* m_pListCtrl;
  int m_nListCtrlIndex;
  BOOL m_bDoNotDestroyUponLoseFocus;
  friend class CListOptionsCtrl;
  DWORD m_dwAddress;
};

//Class which will be the header control (subclassed) for the List Options control
class _GENERAL_CLASS CListOptionsHeaderCtrl : public CHeaderCtrl
{
public:
//Constructors / Destructors
	CListOptionsHeaderCtrl();
	virtual ~CListOptionsHeaderCtrl() {};

//Methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) { m_pListCtrl = pListCtrl; };

protected:
	//{{AFX_VIRTUAL(CListOptionsHeaderCtrl)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsHeaderCtrl)
	//}}AFX_MSG
  afx_msg BOOL OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

  CListOptionsCtrl* m_pListCtrl;
};

//Class which is stored in the list options item data 
class _GENERAL_CLASS CListOptionsItemData
{
public:
//Enums
  enum ControlType
  {
    Unknown,
    OpaqueBrowser,
    Spin,
    FileBrowser,
    FolderBrowser,
    ColorBrowser,
    FontBrowser,
    ComboBox,
    EditBox,
    DateTimeCtrl,
    IPAddressCtrl,
  };

//Data
  CRuntimeClass* m_pRuntimeClass1;
  CRuntimeClass* m_pRuntimeClass2;
  ControlType    m_Type;
  DWORD_PTR      m_dwItemData;
  COLORREF       m_Color;
  LOGFONT        m_Font;
  SYSTEMTIME     m_DateTime;
  BOOL           m_bDrawColorForIcon;
  DWORD          m_dwIPAddress;
  BOOL           m_bPromptBoldText;
  BOOL           m_bPromptItalicText;
  BOOL           m_bPromptUnderlineText;
  BOOL           m_bValueBoldText;
  BOOL           m_bValueItalicText;
  BOOL           m_bValueUnderlineText;
  COLORREF       m_PromptTextColor;
  COLORREF       m_PromptBackgroundColor;
  COLORREF       m_ValueTextColor;
  COLORREF       m_ValueBackgroundColor;

//Methods
  CListOptionsItemData() 
  {
    m_Type = Unknown;
    m_pRuntimeClass1 = NULL;
    m_pRuntimeClass2 = NULL;
    m_dwItemData = (DWORD_PTR) 0;
    m_Color = RGB(255, 0, 0);
    ZeroMemory(&m_Font, sizeof(LOGFONT));
    ZeroMemory(&m_DateTime, sizeof(SYSTEMTIME));
    m_bDrawColorForIcon = TRUE;
    m_dwIPAddress = 0;
    m_bPromptBoldText = FALSE;
    m_bPromptItalicText = FALSE;
    m_bPromptUnderlineText = FALSE;
    m_bValueBoldText = FALSE;
    m_bValueItalicText = FALSE;
    m_bValueUnderlineText = FALSE;
    m_PromptTextColor = GetSysColor(COLOR_WINDOWTEXT);
    m_PromptBackgroundColor = GetSysColor(COLOR_WINDOW);
    m_ValueTextColor = m_PromptTextColor;
    m_ValueBackgroundColor = m_PromptBackgroundColor;
  }
};

//The actual list options control class
class _GENERAL_CLASS CListOptionsCtrl : public CListCtrl
{
public:
//Constructors / Destructors
  CListOptionsCtrl();

//Misc
  void      SetAutoSelect(BOOL bAutoSelect) { m_bAutoSelect = bAutoSelect; };
  BOOL      GetAutoSelect() const { return m_bAutoSelect; };
  DWORD_PTR GetUserItemData(int nIndex);
  void      RequestChildReposition();

//Validation methods
  BOOL      IsBoolean(int nIndex);
  BOOL      IsEditBox(int nIndex);
  BOOL      IsFileItem(int nIndex);
  BOOL      IsFolderItem(int nIndex);
  BOOL      IsColorItem(int nIndex);
  BOOL      IsFontItem(int nIndex);
  BOOL      IsOpaqueItem(int nIndex);

//Adding a boolean combo box to an item
  BOOL      AddBoolean(int nIndex, DWORD_PTR dwItemData = 0);
  BOOL      SetBoolean(int nIndex, BOOL bValue);
  BOOL      GetBoolean(int nIndex, BOOL& bValue);

//Adding a combo box to an item
  BOOL      AddComboBox(int nIndex, CRuntimeClass* pRuntimeClass, DWORD_PTR dwItemData = 0);
  CString   GetComboText(int nIndex);
  void      SetComboText(int nIndex, const CString& sComboText);

//Adding an edit box (and a spin control) to an item
  BOOL      AddEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, DWORD_PTR dwItemData = 0);
  BOOL      AddEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassSpinCtrl, DWORD_PTR dwItemData = 0);
  CString   GetEditText(int nIndex);
  void      SetEditText(int nIndex, const CString& sEditText);

//Adding a file / Folder edit box (and a browse button) to an item
  BOOL      AddFileEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0);
  CString   GetFileEditText(int nIndex);
  void      SetFileEditText(int nIndex, const CString& sEditText);

  BOOL      AddFolderEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0);
  CString   GetFolderEditText(int nIndex);
  void      SetFolderEditText(int nIndex, const CString& sEditText);

//Adding a Color selector to an item
  BOOL      AddColorSelector(int nIndex, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0, BOOL bDrawColorForIcon = TRUE);  
  COLORREF  GetColor(int nIndex);
  void      SetColor(int nIndex, COLORREF color);

//Adding a Opaque selector to an item
  BOOL      AddOpaque(int nIndex, CRuntimeClass* pRuntimeClass1, CRuntimeClass* pRuntimeClass2, DWORD_PTR dwItemData = 0);  
  DWORD_PTR GetOpaque(int nIndex);
  void      SetOpaque(int nIndex, DWORD_PTR dwItemData);

//Adding a font name selector to an item
  BOOL      AddFontSelector(int nIndex, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0);  
  void      GetFontItem(int nIndex, LOGFONT* pLogFont);
  void      SetFontItem(int nIndex, const LOGFONT* pLogFont);

//Adding a Date Time  selector to an item
  BOOL      AddDateTime(int nIndex, CRuntimeClass* pRuntimeClassDateTime, DWORD_PTR dwItemData = 0);  
  void      GetDateTime(int nIndex, SYSTEMTIME& st);
  void      SetDateTime(int nIndex, const SYSTEMTIME& st);

//Adding an IP Address selector to an item
  BOOL      AddIPAddress(int nIndex, CRuntimeClass* pRuntimeClassIPAddress, DWORD_PTR dwItemData = 0);  
  DWORD     GetIPAddress(int nIndex);
  void      SetIPAddress(int nIndex, DWORD dwAddress);
/*
  void SetObj(LPVOID pView,LPVOID pPic,LPVOID pDrawObj,LPVOID pObjList){
	  m_pView=pView;
	  m_pPic=pPic;
	  m_pDrawObj=pDrawObj;
	  m_pObjList = pObjList;
  };//hmi
  */
  void DeleteChild(){DestroyOldChildControl();};
protected:
//Variables
  CListOptionsCombo*         m_pCombo;
  CListOptionsEdit*          m_pEdit;
  CListOptionsSpinCtrl*      m_pSpin;
  CListOptionsBrowseButton*  m_pButton;
  CListOptionsDateCtrl*      m_pDateTime;
  CListOptionsIPAddressCtrl* m_pIPAddress;
  int                        m_nControlItem;
  BOOL                       m_bAutoSelect;
  CListOptionsHeaderCtrl     m_ctrlHeader;
  CFont                      m_Font;
  CFont                      m_BoldFont;
  CFont                      m_ItalicFont;
  CFont                      m_UnderlineFont;
  CFont                      m_ItalicBoldFont;
  CFont                      m_ItalicUnderlineFont;
  CFont                      m_ItalicUnderlineBoldFont;

 // LPVOID	m_pPic,m_pDrawObj,m_pView,m_pObjList;

//Methods
  virtual void DestroyOldChildControl();
  virtual void CreateNewChildControl(int nindex);
  virtual void CreateSpinCtrl(CRuntimeClass* pRuntimeClassSpinCtrl, CRect rItem, CRect rPrimaryControl);
  virtual void CreateBrowseButton(CRuntimeClass* pRuntimeClassBrowseButton, CRect rItem, int nColumnWidth);
  virtual void UpdateListControlValueFromChildControl(int nIndex){};
  virtual void HandleChildControlLosingFocus();
  int          GetSelectedItem();
  void         SetSelected(int nIndex, BOOL bSelect);

	//{{AFX_VIRTUAL(CListOptionsCtrl)
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CListOptionsCtrl)
	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg BOOL OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg LRESULT OnSetFocusToChild(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnRepositionChild(WPARAM wParam, LPARAM lParam);
  afx_msg BOOL OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);


  DECLARE_DYNAMIC(CListOptionsCtrl)

	DECLARE_MESSAGE_MAP()

  friend class CListOptionsEdit;
  friend class CListOptionsStatic;
  friend class CListOptionsCombo;
  friend class CListOptionsSpinCtrl;
  friend class CListOptionsBrowseButton;
  friend class CListOptionsHeaderCtrl;
  friend class CListOptionsDateCtrl;
  friend class CListOptionsIPAddressCtrl;
};

//Dialog Data exchange support
_GENERAL_API(void) DDX_ListEdit(CDataExchange* pDX, int nIDC, int nIndex, CString& sText);
_GENERAL_API(void) DDX_ListEdit(CDataExchange* pDX, int nIDC, int nIndex, int& nValue);
_GENERAL_API(void) DDX_ListCombo(CDataExchange* pDX, int nIDC, int nIndex, CString& sText);
_GENERAL_API(void) DDX_ListFileEdit(CDataExchange* pDX, int nIDC, int nIndex, CString& sText);
_GENERAL_API(void) DDX_ListFolderEdit(CDataExchange* pDX, int nIDC, int nIndex, CString& sText);
_GENERAL_API(void) DDX_ListColor(CDataExchange* pDX, int nIDC, int nIndex, COLORREF& color);
_GENERAL_API(void) DDX_ListFont(CDataExchange* pDX, int nIDC, int nIndex, LOGFONT* pLogFont);
_GENERAL_API(void) DDX_ListBoolean(CDataExchange* pDX, int nIDC, int nIndex, BOOL& bValue);
_GENERAL_API(void) DDX_ListDateTime(CDataExchange* pDX, int nIDC, int nIndex, SYSTEMTIME& st);
_GENERAL_API(void) DDX_ListIPAddress(CDataExchange* pDX, int nIDC, int nIndex, DWORD& dwAddress);

TRUNK_NAMESPACE_END

#endif //end of __LISTOPTIONSCTRL_H__
