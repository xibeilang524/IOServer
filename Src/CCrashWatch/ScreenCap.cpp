#include "stdafx.h"
#include "ScreenCap.h"
#include "Utility.h"

// Disable warning C4611: interaction between '_setjmp' and C++ object destruction is non-portable
#pragma warning(disable:4611)

// This function is used for monitor enumeration
BOOL CALLBACK EnumMonitorsProc(HMONITOR hMonitor, HDC /*hdcMonitor*/, LPRECT lprcMonitor, LPARAM dwData)
{	  
  CScreenCapture* psc = (CScreenCapture*)dwData;

	MONITORINFOEX mi;
	HDC hDC = NULL;  
  HDC hCompatDC = NULL;
  HBITMAP hBitmap = NULL;
  BITMAPINFO bmi;
  int nWidth = 0;
  int nHeight = 0;
  int nRowWidth = 0;
  LPBYTE pRowBits = NULL;
  CString sFileName;
  MonitorInfo monitor_info;
    
  // Get monitor rect size
  nWidth = lprcMonitor->right - lprcMonitor->left;
	nHeight = lprcMonitor->bottom - lprcMonitor->top;
	
  // Get monitor info
  mi.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &mi);
      
	// Get the device context for this monitor
	hDC = CreateDC(_T("DISPLAY"), mi.szDevice, NULL, NULL); 
	if(hDC==NULL)
    goto cleanup;

  hCompatDC = CreateCompatibleDC(hDC);
  if(hCompatDC==NULL)
    goto cleanup;

  hBitmap = CreateCompatibleBitmap(hDC, nWidth, nHeight);
  if(hBitmap==NULL)
    goto cleanup;

  SelectObject(hCompatDC, hBitmap);

  int i;
  for(i=0; i<(int)psc->m_arcCapture.size(); i++)
  {
    CRect rc = psc->m_arcCapture[i];
    CRect rcIntersect;
    if(IntersectRect(&rcIntersect, lprcMonitor, &rc))
    {
      BOOL bBitBlt = BitBlt(hCompatDC, rc.left-lprcMonitor->left, rc.top-lprcMonitor->top, 
        rc.Width(), rc.Height(), hDC, rc.left-lprcMonitor->left, rc.top-lprcMonitor->top, SRCCOPY|CAPTUREBLT);
      if(!bBitBlt)
        goto cleanup;
    }
  }
  
  // Draw mouse cursor.
  if(PtInRect(lprcMonitor, psc->m_ptCursorPos))
	{						
		if(psc->m_CursorInfo.flags == CURSOR_SHOWING)
		{
      ICONINFO IconInfo;
		  GetIconInfo((HICON)psc->m_CursorInfo.hCursor, &IconInfo);
      int x = psc->m_ptCursorPos.x - lprcMonitor->left - IconInfo.xHotspot;
      int y = psc->m_ptCursorPos.y - lprcMonitor->top  - IconInfo.yHotspot;
			DrawIcon(hCompatDC, x, y, (HICON)psc->m_CursorInfo.hCursor);
      DeleteObject(IconInfo.hbmMask);
			DeleteObject(IconInfo.hbmColor);
		}				
	}

  /* Write screenshot bitmap to an image file. */

	// Init JPG writer
	sFileName.Format(_T("%s\\screenshot%d.jpg"), psc->m_sSaveDirName, psc->m_nIdStartFrom++);
	BOOL bInit = psc->JpegInit(nWidth, nHeight, psc->m_bGrayscale, psc->m_nJpegQuality, sFileName);
	if(!bInit)
	  goto cleanup;

  // We will get bitmap bits row by row
  nRowWidth = nWidth*3 + 10;
  pRowBits = new BYTE[nRowWidth];
  if(pRowBits==NULL)
    goto cleanup;
    
  memset(&bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
  bmi.bmiHeader.biWidth = nWidth;
  bmi.bmiHeader.biHeight = -nHeight;
  bmi.bmiHeader.biBitCount = 24;
  bmi.bmiHeader.biPlanes = 1;  

  //int i;
  for(i=nHeight-1; i>=0; i--)
  {    
    int nFetched = GetDIBits(hCompatDC, hBitmap, i, 1, pRowBits, &bmi, DIB_RGB_COLORS);
    if(nFetched!=1)
      break;

      BOOL bWrite = psc->JpegWriteRow(pRowBits, nRowWidth, psc->m_bGrayscale);
      if(!bWrite)
        goto cleanup; 
  }
  
   psc->JpegFinalize();
  
  psc->m_out_file_list.push_back(sFileName);
  
  monitor_info.m_rcMonitor = mi.rcMonitor;
  monitor_info.m_sDeviceID = mi.szDevice;
  monitor_info.m_sFileName = sFileName;
  psc->m_monitor_list.push_back(monitor_info);
  
cleanup:

  // Clean up
  if(hDC)
    DeleteDC(hDC);

  if(hCompatDC)
    DeleteDC(hCompatDC);

  if(hBitmap)
    DeleteObject(hBitmap);

  if(pRowBits)
    delete [] pRowBits;

  // Next monitor
	return TRUE;
}

CScreenCapture::CScreenCapture()
{
  m_fp = NULL;
  m_nIdStartFrom = 0;
}

CScreenCapture::~CScreenCapture()
{
}

BOOL CScreenCapture::CaptureScreenRect(
  std::vector<CRect> arcCapture,  
  CString sSaveDirName,   
  int nIdStartFrom, 
  int nJpegQuality,
  BOOL bGrayscale,
  std::vector<MonitorInfo>& monitor_list,
  std::vector<CString>& out_file_list)
{	
  // Init output variables
  monitor_list.clear();
  out_file_list.clear();

  // Set internal variables
  m_nIdStartFrom = nIdStartFrom;
  m_sSaveDirName = sSaveDirName;
  m_nJpegQuality = nJpegQuality;
  m_bGrayscale = bGrayscale;
  m_arcCapture = arcCapture;
  m_out_file_list.clear();
  m_monitor_list.clear();

  // Get cursor information
  GetCursorPos(&m_ptCursorPos);
  m_CursorInfo.cbSize = sizeof(CURSORINFO);
  GetCursorInfo(&m_CursorInfo);

  // Perform actual capture task inside of EnumMonitorsProc
	EnumDisplayMonitors(NULL, NULL, EnumMonitorsProc, (LPARAM)this);	

  // Return
  out_file_list = m_out_file_list;
  monitor_list = m_monitor_list;
	return TRUE;
}

// Gets rectangle of the virtual screen
void CScreenCapture::GetScreenRect(LPRECT rcScreen)
{
	int nWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int nHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	rcScreen->left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	rcScreen->top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	rcScreen->right = rcScreen->left + nWidth;
	rcScreen->bottom = rcScreen->top + nHeight;
}

BOOL CScreenCapture::JpegInit(int nWidth, int nHeight, BOOL bGrayscale, int nQuality, CString sFileName)
{   
  /* Step 1: allocate and initialize JPEG compression object */

  m_cinfo.err = jpeg_std_error(&m_jerr);  
  jpeg_create_compress(&m_cinfo);

  /* Step 2: specify data destination (eg, a file) */

#if _MSC_VER < 1400
  m_fp = _tfopen(sFileName, _T("wb"));
#else
  _tfopen_s(&m_fp, sFileName, _T("wb"));
#endif
  if(m_fp==NULL)
    return FALSE;

  jpeg_stdio_dest(&m_cinfo, m_fp);

  /* Step 3: set parameters for compression */

  m_cinfo.image_width = nWidth; 	/* image width and height, in pixels */
  m_cinfo.image_height = nHeight;
  m_cinfo.input_components = bGrayscale?1:3;		/* # of color components per pixel */
  m_cinfo.in_color_space = bGrayscale?JCS_GRAYSCALE:JCS_RGB; 	/* colorspace of input image */
  jpeg_set_defaults(&m_cinfo);
  jpeg_set_quality(&m_cinfo, nQuality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  jpeg_start_compress(&m_cinfo, TRUE);

  return TRUE;
}

BOOL CScreenCapture::JpegWriteRow(LPBYTE pRow, int nRowLen, BOOL bGrayscale)
{
  // Convert RGB to BGR
  LPBYTE pRow2 = NULL;
  int i;
  if(bGrayscale)
  {
    pRow2 = new BYTE[nRowLen/3];
    for(i=0; i<nRowLen/3; i++)
      pRow2[i] = (pRow[i*3+0]+pRow[i*3+1]+pRow[i*3+2])/3;
  }
  else
  {
    pRow2 = new BYTE[nRowLen];
    for(i=0; i<nRowLen/3; i++)
    {    
      pRow2[i*3+0] = pRow[i*3+2];
      pRow2[i*3+1] = pRow[i*3+1];
      pRow2[i*3+2] = pRow[i*3+0];
    }
  }

  JSAMPROW row_pointer[1];
  row_pointer[0] = (JSAMPROW)pRow2;
  (void) jpeg_write_scanlines(&m_cinfo, row_pointer, 1);

  delete [] pRow2;

  return TRUE;
}

BOOL CScreenCapture::JpegFinalize()
{
  /* Step 6: Finish compression */
  jpeg_finish_compress(&m_cinfo);

  /* After finish_compress, we can close the output file. */
  if(m_fp!=NULL)
    fclose(m_fp);
  m_fp = NULL;

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&m_cinfo);

  return TRUE;
}

struct FindWindowData
{
  HANDLE hProcess;                     // Handle to the process
  BOOL bAllProcessWindows;             // If TRUE, finds all process windows, else only the main one
  std::vector<WindowInfo>* paWindows;  // Output array of window handles
};

// Windows 2000²»Ö§³ÖGetProcessId()
#define ProcessBasicInformation 0

typedef struct
{
	DWORD ExitStatus;
	DWORD PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}PROCESS_BASIC_INFORMATION;

typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);

DWORD MyGetProcessID(HANDLE hProcess)
{
	PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
		GetModuleHandle("ntdll"),
		"NtQueryInformationProcess"
		);

	if (!NtQueryInformationProcess)
		return 0;

	PROCESS_BASIC_INFORMATION pbi;

	ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));
	if (NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL) == 0)
		return (DWORD)pbi.UniqueProcessId;
	else
		return 0;
}

BOOL CALLBACK EnumWndProc(HWND hWnd, LPARAM lParam)
{
  FindWindowData* pFWD = (FindWindowData*)lParam;

  // Get process ID
  //DWORD dwMyProcessId = GetProcessId(pFWD->hProcess);
  DWORD dwMyProcessId = MyGetProcessID(pFWD->hProcess);

  if(IsWindowVisible(hWnd)) // Get only wisible windows
  {
    // Determine the process ID of the current window
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &dwProcessId);

    // Compare window process ID to our process ID
    if(dwProcessId == dwMyProcessId)
    {     
      DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
      if((dwStyle&WS_CHILD)==0) // Get only non-child windows
      {
        DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
        WindowInfo wi;
        TCHAR szTitle[1024];
        GetWindowText(hWnd, szTitle, 1024);            
        wi.m_sTitle = szTitle;
        GetWindowRect(hWnd, &wi.m_rcWnd);
        wi.dwStyle = dwStyle;
        wi.dwExStyle = dwExStyle;
        pFWD->paWindows->push_back(wi);
        
        return TRUE;          
      }
    }
  }
   
  return TRUE;
}

BOOL CScreenCapture::FindWindows(HANDLE hProcess, BOOL bAllProcessWindows, std::vector<WindowInfo>* paWindows)
{
  FindWindowData fwd;
  fwd.bAllProcessWindows = bAllProcessWindows;
  fwd.hProcess = hProcess;
  fwd.paWindows = paWindows;
  EnumWindows(EnumWndProc, (LPARAM)&fwd);

  if(!bAllProcessWindows) 
  {    
    // Find only the main window
    // The main window should have caption, system menu and WS_EX_APPWINDOW style.  
    WindowInfo MainWnd;
    BOOL bMainWndFound = FALSE;

    size_t i;
    for(i=0; i<paWindows->size(); i++)
    {
      if(((*paWindows)[i].dwExStyle&WS_EX_APPWINDOW) && 
         ((*paWindows)[i].dwStyle&WS_CAPTION) && 
         ((*paWindows)[i].dwStyle&WS_SYSMENU))
      {      
        // Match!
        bMainWndFound = TRUE;
        MainWnd = (*paWindows)[i];
        break;
      }
    }

    if(!bMainWndFound && paWindows->size()>0)
    {
      MainWnd = (*paWindows)[0];
      bMainWndFound = TRUE;
    }

    if(bMainWndFound)
    {
      paWindows->clear();
      paWindows->push_back(MainWnd);
    }
  }

  return TRUE;
}





