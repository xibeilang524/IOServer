#ifndef __SCREENCAP_H__
#define __SCREENCAP_H__

#include "jpeglib.h"

// Window information
struct WindowInfo
{
  CString m_sTitle; // Window title
  CRect m_rcWnd;    // Window rect
  DWORD dwStyle;
  DWORD dwExStyle;
};

// Monitor info
struct MonitorInfo
{
  CString m_sDeviceID; // Device ID
  CRect m_rcMonitor;   // Monitor rectangle in screen coordinates
  CString m_sFileName; // Image file name corresponding to this monitor
};

// Desktop screen shot info
struct ScreenshotInfo
{
  ScreenshotInfo()
  {
    m_bValid = FALSE;
  }

  BOOL m_bValid;
  CRect m_rcVirtualScreen;
  std::vector<MonitorInfo> m_aMonitors; // The list of monitors.
  std::vector<WindowInfo> m_aWindows; // The list of windows.
};

// Desktop screenshot capture
class CScreenCapture
{
public:

  // Constructor
  CScreenCapture();
  ~CScreenCapture();

  // Returns virtual screen rectangle
  void GetScreenRect(LPRECT rcScreen);

  // Returns an array of visible windows for the specified process or 
  // the main window of the process.
  BOOL FindWindows(HANDLE hProcess, BOOL bAllProcessWindows, 
    std::vector<WindowInfo>* paWindows);

  // Captures the specified screen area and returns the list of image files
  BOOL CaptureScreenRect(
    std::vector<CRect> arcCapture, 
    CString sSaveDirName, 
    int nIdStartFrom, 
    int nJpegQuality,
    BOOL bGrayscale,
    std::vector<MonitorInfo>& monitor_list,
    std::vector<CString>& out_file_list);

  /* JPEG management functions */

  BOOL JpegInit(int nWidth, int nHeight, BOOL bGrayscale, int nQuality, CString sFileName);
  BOOL JpegWriteRow(LPBYTE pRow, int nRowLen, BOOL bGrayscale);
  BOOL JpegFinalize();

  /* Member variables. */

  CPoint m_ptCursorPos;                 // Current mouse cursor pos
  std::vector<CRect> m_arcCapture;      // Array of capture rectangles
  CURSORINFO m_CursorInfo;              // Cursor info  
  int m_nIdStartFrom;                   // An ID for the current screenshot image 
  CString m_sSaveDirName;               // Directory name to save screenshots to
  int m_nJpegQuality;                   // Jpeg quality
  BOOL m_bGrayscale;                    // Create grayscale image or not
  FILE* m_fp;                           // Handle to the file
  struct jpeg_compress_struct m_cinfo;  // libjpeg stuff
  struct jpeg_error_mgr m_jerr;         // libjpeg stuff
  std::vector<MonitorInfo> m_monitor_list; // The list of monitor devices
  std::vector<CString> m_out_file_list; // The list of output image files
};

#endif //__SCREENCAP_H__


