/* DrawHTML()
 * Drop-in replacement for DrawText() supporting a tiny subset of HTML.
 */
#ifndef __DRAW_HTML_H__
#define __DRAW_HTML_H__

TRUNK_NAMESPACE_BEGIN

_GENERAL_API(int) DrawHTML(
                       HDC     hdc,        // handle of device context
                       LPCTSTR lpString,   // address of string to draw
                       int     nCount,     // string length, in characters
                       LPRECT  lpRect,     // address of structure with formatting dimensions
                       UINT    uFormat     // text-drawing flags
                      );

TRUNK_NAMESPACE_END

#endif // end of __DRAW_HTML_H__