#ifndef HELPER_INCLUDED
#define HELPER_INCLUDED

#include <windows.h>

#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) \
                                 | ((Color << 16) & 0xff0000)

//-------------------------------------------------------------------------------
// ReplaceColor
//
// Author    : Dimitri Rochette drochette@ltezone.net
// Specials Thanks to Joe Woodbury for his comments and code corrections
//
// Includes  : Only <windows.h>
//
// hBmp	     : Source Bitmap
// cOldColor : Color to replace in hBmp
// cNewColor : Color used for replacement
// hBmpDC    : DC of hBmp ( default NULL ) could be NULL if hBmp is not selected
//
// Retcode   : HBITMAP of the modified bitmap or NULL for errors
//
//-------------------------------------------------------------------------------
HBITMAP ReplaceColor(HBITMAP hBmp,COLORREF cOldColor,COLORREF cNewColor,HDC hBmpDC)
{
    HBITMAP RetBmp=NULL;
    if (hBmp)
    {
        HDC BufferDC=CreateCompatibleDC(NULL);	// DC for Source Bitmap
	if (BufferDC)
	{
	    HBITMAP hTmpBitmap = (HBITMAP) NULL;
	    if (hBmpDC)
	        if (hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
		{
		    hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
		    SelectObject(hBmpDC, hTmpBitmap);
		}

            HGDIOBJ PreviousBufferObject=SelectObject(BufferDC,hBmp);
	    // here BufferDC contains the bitmap

	    HDC DirectDC=CreateCompatibleDC(NULL); // DC for working
	    if (DirectDC)
	    {
		// Get bitmap size
		BITMAP bm;
		GetObject(hBmp, sizeof(bm), &bm);

		// create a BITMAPINFO with minimal initilisation
                // for the CreateDIBSection
		BITMAPINFO RGB32BitsBITMAPINFO;
		ZeroMemory(&RGB32BitsBITMAPINFO,sizeof(BITMAPINFO));
		RGB32BitsBITMAPINFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		RGB32BitsBITMAPINFO.bmiHeader.biWidth=bm.bmWidth;
		RGB32BitsBITMAPINFO.bmiHeader.biHeight=bm.bmHeight;
		RGB32BitsBITMAPINFO.bmiHeader.biPlanes=1;
		RGB32BitsBITMAPINFO.bmiHeader.biBitCount=32;

                // pointer used for direct Bitmap pixels access
		UINT * ptPixels;

		HBITMAP DirectBitmap = CreateDIBSection(DirectDC,
                                       (BITMAPINFO *)&RGB32BitsBITMAPINFO,
                                       DIB_RGB_COLORS,
                                       (void **)&ptPixels,
                                       NULL, 0);
		if (DirectBitmap)
		{
		    // here DirectBitmap!=NULL so ptPixels!=NULL no need to test
		    HGDIOBJ PreviousObject=SelectObject(DirectDC, DirectBitmap);
		    BitBlt(DirectDC,0,0,
                           bm.bmWidth,bm.bmHeight,
                           BufferDC,0,0,SRCCOPY);

		    // here the DirectDC contains the bitmap

		    // Convert COLORREF to RGB (Invert RED and BLUE)
		    cOldColor=COLORREF2RGB(cOldColor);
		    cNewColor=COLORREF2RGB(cNewColor);

		    // After all the inits we can do the job : Replace Color
		    for (int i=((bm.bmWidth*bm.bmHeight)-1);i>=0;i--)
		    {
			if (ptPixels[i]==cOldColor) ptPixels[i]=cNewColor;
		    }
		    // little clean up
		    // Don't delete the result of SelectObject because it's
                    // our modified bitmap (DirectBitmap)
		    SelectObject(DirectDC,PreviousObject);

		    // finish
		    RetBmp=DirectBitmap;
		}
		// clean up
		DeleteDC(DirectDC);
	    }
	    if (hTmpBitmap)
	    {
		SelectObject(hBmpDC, hBmp);
		DeleteObject(hTmpBitmap);
	    }
	    SelectObject(BufferDC,PreviousBufferObject);
	    // BufferDC is now useless
	    DeleteDC(BufferDC);
	}
    }
    return RetBmp;
}


HBITMAP Create24BPPDIBSection(HDC hDC, int iWidth, int iHeight)
{
    BITMAPINFO bmi;
    HBITMAP hbm;
    LPBYTE pBits;

    // Initialize header to 0s.
    ZeroMemory(&bmi, sizeof(bmi));

    // Fill out the fields you care about.
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iWidth;
    bmi.bmiHeader.biHeight = iHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Create the surface.
    hbm = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS,(void **)&pBits, NULL, 0);

    return(hbm);
}


BOOL BitmapsCompatible(LPBITMAP lpbm1, LPBITMAP lpbm2)
{
    if (lpbm1->bmBitsPixel != lpbm2->bmBitsPixel)
		return FALSE;
    if (lpbm1->bmPlanes    != lpbm2->bmPlanes)
		return FALSE;
    if (lpbm1->bmWidth     != lpbm2->bmWidth)
		return FALSE;
    if (lpbm1->bmHeight    != lpbm2->bmHeight)
		return FALSE;
    return TRUE;
}

BOOL BlendImages(HBITMAP hbmSrc1, HBITMAP hbmSrc2, HBITMAP hbmDst, DWORD dwWeight1)
{
    BITMAP bmSrc1, bmSrc2, bmDst;
    RGBTRIPLE *lprgbSrc1, *lprgbSrc2, *lprgbDst;
    DWORD dwWidthBytes, dwWeight2;
    int x, y;

    // Only values between 0 and 255 are valid.
    if (dwWeight1 > 255) return FALSE;

    // Get weighting value for second source image.
    dwWeight2 = 255-dwWeight1;

    // Get information about the surfaces you were passed.
    if (!GetObject(hbmSrc1, sizeof(BITMAP), &bmSrc1)) return FALSE;
    if (!GetObject(hbmSrc2, sizeof(BITMAP), &bmSrc2)) return FALSE;
    if (!GetObject(hbmDst,  sizeof(BITMAP), &bmDst))  return FALSE;

    // Make sure you have data that meets your requirements.
    if (!BitmapsCompatible(&bmSrc1, &bmSrc2))
		return FALSE;
    if (!BitmapsCompatible(&bmSrc1, &bmDst))
		return FALSE;
    if (bmSrc1.bmBitsPixel != 24)
		return FALSE;
    if (bmSrc1.bmPlanes != 1)
		return FALSE;
    if (!bmSrc1.bmBits || !bmSrc2.bmBits || !bmDst.bmBits)
		return FALSE;

    dwWidthBytes = bmDst.bmWidthBytes;

    // Initialize the surface pointers.
    lprgbSrc1 = (RGBTRIPLE *)bmSrc1.bmBits;
    lprgbSrc2 = (RGBTRIPLE *)bmSrc2.bmBits;
    lprgbDst  = (RGBTRIPLE *)bmDst.bmBits;

    for (y=0; y<bmDst.bmHeight; y++) {
        for (x=0; x<bmDst.bmWidth; x++) {
            lprgbDst[x].rgbtRed   = (BYTE)((((DWORD)lprgbSrc1[x].rgbtRed * dwWeight1) +
                                            ((DWORD)lprgbSrc2[x].rgbtRed * dwWeight2)) >> 8);
            lprgbDst[x].rgbtGreen = (BYTE)((((DWORD)lprgbSrc1[x].rgbtGreen * dwWeight1) +
                                            ((DWORD)lprgbSrc2[x].rgbtGreen * dwWeight2)) >> 8);
            lprgbDst[x].rgbtBlue  = (BYTE)((((DWORD)lprgbSrc1[x].rgbtBlue * dwWeight1) +
                                            ((DWORD)lprgbSrc2[x].rgbtBlue * dwWeight2)) >> 8);
        }

        // Move to next scan line.
        lprgbSrc1 = (RGBTRIPLE *)((LPBYTE)lprgbSrc1 + dwWidthBytes);
        lprgbSrc2 = (RGBTRIPLE *)((LPBYTE)lprgbSrc2 + dwWidthBytes);
        lprgbDst  = (RGBTRIPLE *)((LPBYTE)lprgbDst  + dwWidthBytes);
    }

    return TRUE;
}

BOOL DoAlphaBlend(
  HDC hdcDest,                 // Handle to destination DC.
  int nXOriginDest,            // X-coord of upper-left corner.
  int nYOriginDest,            // Y-coord of upper-left corner.
  int nWidthDest,              // Destination width.
  int nHeightDest,             // Destination height.
  HDC hdcSrc,                  // Handle to source DC.
  int nXOriginSrc,             // X-coord of upper-left corner.
  int nYOriginSrc,             // Y-coord of upper-left corner.
  int nWidthSrc,               // Source width.
  int nHeightSrc,              // Source height.
  DWORD dwSourceWeight)        // Source weighting (between 0 and 255).
{
    HDC      hdcSrc1 = NULL;
    HDC      hdcSrc2 = NULL;
    HDC      hdcDst  = NULL;
    HBITMAP  hbmSrc1 = NULL;
    HBITMAP  hbmSrc2 = NULL;
    HBITMAP  hbmDst  = NULL;
    BOOL     bReturn;

    // Create surfaces for sources and destination images.
    hbmSrc1 = Create24BPPDIBSection(hdcDest, nWidthDest,nHeightDest);
    if (!hbmSrc1) goto HANDLEERROR;

    hbmSrc2 = Create24BPPDIBSection(hdcDest, nWidthDest,nHeightDest);
    if (!hbmSrc2) goto HANDLEERROR;

    hbmDst  = Create24BPPDIBSection(hdcDest, nWidthDest,nHeightDest);
    if (!hbmDst) goto HANDLEERROR;


    // Create HDCs to hold our surfaces.
    hdcSrc1 = CreateCompatibleDC(hdcDest);
    if (!hdcSrc1) goto HANDLEERROR;

    hdcSrc2 = CreateCompatibleDC(hdcDest);
    if (!hdcSrc2) goto HANDLEERROR;

    hdcDst  = CreateCompatibleDC(hdcDest);
    if (!hdcDst) goto HANDLEERROR;


    // Prepare the surfaces for drawing.
    SelectObject(hdcSrc1, hbmSrc1);
    SelectObject(hdcSrc2, hbmSrc2);
    SelectObject(hdcDst,  hbmDst);
    SetStretchBltMode(hdcSrc1, COLORONCOLOR);
    SetStretchBltMode(hdcSrc2, COLORONCOLOR);


    // Capture a copy of the source area.
    if (!StretchBlt(hdcSrc1, 0,0,nWidthDest,nHeightDest,
                    hdcSrc,  nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc,
                    SRCCOPY))
         goto HANDLEERROR;

    // Capture a copy of the destination area.
    if (!StretchBlt(hdcSrc2, 0,0,nWidthDest,nHeightDest,
                    hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
                    SRCCOPY))
         goto HANDLEERROR;


    // Blend the two source areas to create the destination image.
    bReturn = BlendImages(hbmSrc1, hbmSrc2, hbmDst, dwSourceWeight);


    // Clean up objects you do not need any longer.
    // You cannot delete an object that's selected into an
    // HDC so delete the HDC first.
    DeleteDC(hdcSrc1);
    DeleteDC(hdcSrc2);
    DeleteObject(hbmSrc1);
    DeleteObject(hbmSrc2);


    // Display the blended (destination) image to the target HDC.
    if (bReturn) {
        BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
               hdcDst, 0,0, SRCCOPY);
    }


    // Clean up the rest of the objects you created.
    DeleteDC(hdcDst);
    DeleteObject(hbmDst);

    return bReturn;

HANDLEERROR:

    if (hdcSrc1) DeleteDC(hdcSrc1);
    if (hdcSrc2) DeleteDC(hdcSrc2);
    if (hdcDst)  DeleteDC(hdcDst);
    if (hbmSrc1) DeleteObject(hbmSrc1);
    if (hbmSrc2) DeleteObject(hbmSrc2);
    if (hbmDst)  DeleteObject(hbmDst);
    MessageBox(NULL, "ERROR BLENDING", "DEBUG Message", 0);

    return FALSE;
}


#endif
