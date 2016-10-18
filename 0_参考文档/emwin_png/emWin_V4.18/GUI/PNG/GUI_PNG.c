/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2004  SEGGER Microcontroller Systeme GmbH        *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

***** emWin - Graphical user interface for embedded applications *****
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_PNG.c
Purpose     : Implementation of GUI_PNG... functions
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>
#include <stdlib.h>

#include "png.h"

#include "GUI_Private.h"
#include "GUI_PNG_Private.h"

#if (GUI_VERSION <= 41800)
  int GUI_PNG_Draw      (const void * pFileData, int DataSize, int x0, int y0);
  int GUI_PNG_DrawEx    (GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
  int GUI_PNG_GetXSize  (const void * pFileData, int FileSize);
  int GUI_PNG_GetXSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p);
  int GUI_PNG_GetYSize  (const void * pFileData, int FileSize);
  int GUI_PNG_GetYSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p);
#endif

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_PNG__GetData
*
* Parameters:
*   p           - Pointer to application defined data.
*   NumBytesReq - Number of bytes requested.
*   ppData      - Pointer to data pointer. This pointer should be set to
*                 a valid location.
*
* Return value:
*   Number of data bytes available.
*/
static int GUI_PNG__GetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  GUI_PNG_PARAM * pParam;
  U8 * pData;

  pData = (U8 *)*ppData;
  pParam = (GUI_PNG_PARAM *)p;
  memcpy(pData, (const void *)(pParam->pFileData + Off), NumBytesReq);
  return NumBytesReq;
}

/*********************************************************************
*
*       _png_cexcept_error
*
* Purpose:
*   Called by PNG library if an error occurs.
*
* Parameter:
*   png_ptr - Context pointer
*   msg     - Error message
*/
static void _png_cexcept_error(png_structp png_ptr, png_const_charp msg) {
  GUI_USE_PARA(png_ptr);
  GUI_USE_PARA(msg);
  while (1); /* Stop on error */
}

/*********************************************************************
*
*       _png_read_data
*/
static void PNGAPI _png_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
  GUI_PNG_CONTEXT * pContext;
  pContext = (GUI_PNG_CONTEXT *)png_ptr->io_ptr;
  if ((png_size_t)pContext->pfGetData(pContext->pParam, (const U8 **)&data, length, pContext->Off) != length) {
    _png_cexcept_error(png_ptr, "Error reading data!");
  }
  pContext->Off += length;
}

/*********************************************************************
*
*       _CompactPixelIndexArray
*/
static void _CompactPixelIndexArray(LCD_PIXELINDEX * pBuffer, int NumPixels, int BitsPerPixel) {
  int BytesPerPixel;
  if (BitsPerPixel <= 8) {
    BytesPerPixel = 1;
  } else if (BitsPerPixel <= 16) {
    BytesPerPixel = 2;
  } else {
    BytesPerPixel = 4;
  }
  if (BytesPerPixel < GUI_BYTESPERPIXEL) {
    switch (GUI_BYTESPERPIXEL) {
    case 4:
      switch (BytesPerPixel) {
      case 2: { /* Convert from U32 to U16 */
          U16 * pDest;
          pDest = (U16 *)pBuffer;
          do {
            *pDest++ = *pBuffer++;
          } while (--NumPixels);
        }
        break;
      case 1: { /* Convert from U32 to U8 */
          U8 * pDest;
          pDest = (U8 *)pBuffer;
          do {
            *pDest++ = *pBuffer++;
          } while (--NumPixels);
        }
        break;
      }
      break;
    case 2: {   /* Convert from U16 to U8 */
        U8 * pDest;
        pDest = (U8 *)pBuffer;
        do {
          *pDest++ = *pBuffer++;
        } while (--NumPixels);
      }
      break;
    }
  }
}

/*********************************************************************
*
*       _ExpandPixelIndexArray
*/
static void _ExpandPixelIndexArray(void * pBuffer, int NumPixels, int BitsPerPixel) {
  int BytesPerPixel;
  U32 * pDest;

  pDest = (U32 *)pBuffer;
  if (BitsPerPixel <= 8) {
    BytesPerPixel = 1;
  } else if (BitsPerPixel <= 16) {
    BytesPerPixel = 2;
  } else {
    BytesPerPixel = 4;
  }
  if (BytesPerPixel < GUI_BYTESPERPIXEL) {
    switch (GUI_BYTESPERPIXEL) {
    case 4:
      switch (BytesPerPixel) {
      case 2: { /* Convert from U16 to U32 */
          U16 * pSrc;
          pSrc = (U16 *)pBuffer;
          pSrc  += NumPixels;
          pDest += NumPixels;
          do {
            *--pDest = *--pSrc;
          } while (--NumPixels);
        }
        break;
      case 1: { /* Convert from U8 to U32 */
          U8 * pSrc;
          pSrc = (U8 *)pBuffer;
          pSrc  += NumPixels;
          pDest += NumPixels;
          do {
            *--pDest = *--pSrc;
          } while (--NumPixels);
        }
        break;
      }
      break;
    case 2: {   /* Convert from U8 to U16 */
        U8 * pSrc;
        pSrc = (U8 *)pBuffer;
        pSrc  += NumPixels;
        pDest += NumPixels;
        do {
          *--pDest = *--pSrc;
        } while (--NumPixels);
      }
      break;
    }
  }
}

/*********************************************************************
*
*       _GetImageHeader
*/
static int _GetImageHeader(png_structp * ppng_ptr, png_infop * pinfo_ptr, GUI_PNG_CONTEXT * pContext, U32 * pWidth, U32 * pHeight, int * pBitDepth, int * pColorType) {
  U8 acHeader[8];
  png_structp png_ptr  = NULL;
  png_infop   info_ptr = NULL;

  png_ptr  = *ppng_ptr;
  info_ptr = *pinfo_ptr;
  //
  // Read-struct creation
  //
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr)_png_cexcept_error, (png_error_ptr)NULL);
  if (png_ptr == NULL) {
    return 1;
  }
  //
  // Info-struct creation
  //
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 1;
  }
  //
  // Set read function
  //
  png_set_read_fn(png_ptr, (png_voidp)pContext, _png_read_data);
  //
  // Check file header
  //
  _png_read_data(png_ptr, acHeader, 8);
  if (png_sig_cmp(acHeader, 0, 8) != 0) {
    return 1;
  }
  png_set_sig_bytes(png_ptr, 8);
  //
  // Read all PNG info up to image data
  //
  png_read_info(png_ptr, info_ptr);
  //
  // Get width, height, bit-depth and color-type
  //
  png_get_IHDR(png_ptr, info_ptr, pWidth, pHeight, pBitDepth, pColorType, NULL, NULL, NULL);
  *ppng_ptr  = png_ptr;
  *pinfo_ptr = info_ptr;
  return 0;
}

/*********************************************************************
*
*       _Draw
*/
static int _Draw(int x0, int y0, GUI_PNG_CONTEXT * pContext) {
  png_structp png_ptr  = NULL;
  png_infop   info_ptr = NULL;
  U32 Width, Height;
  int BitDepth, ColorType;
  png_color bkgColor = {127, 127, 127};
  png_color * pBkgColor; 
  U32 RowBytes;
  U32 Channels;
  double Gamma;
  U8 * pImageData;
  U8 * pImageDataOld;
  U8 ** ppRowPointers = NULL;
  png_color_16 * pBackground;
  int BitsPerPixel, BytesPerPixel, HasAlpha, HasTrans;
  unsigned i, x, y;
  int BkPixelIndex;
  LCD_PIXELINDEX * pBkGnd;
  U32 * pColor;
  U32 * pWrite;
  tLCDDEV_Index2Color * pfIndex2Color;
  tLCDDEV_Color2Index * pfColor2Index;
  GUI_HMEM hBkGnd;
  GUI_HMEM hColor;
  
  //
  // Get image header
  //
  if (_GetImageHeader(&png_ptr, &info_ptr, pContext, &Width, &Height, &BitDepth, &ColorType)) {
    return 1;
  }
  //
  // Expand images of all color-type and bit-depth to 3x8 bit RGB images, and
  // let the library process things like alpha, transparency, background
  //
  if (BitDepth == 16) {
    png_set_strip_16(png_ptr);
  }
  if (ColorType == PNG_COLOR_TYPE_PALETTE) {
    png_set_expand(png_ptr);
  }
  if (BitDepth < 8) {
    png_set_expand(png_ptr);
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_expand(png_ptr);
  }
  if (ColorType == PNG_COLOR_TYPE_GRAY || ColorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png_ptr);
  }
  //
  // Set the background color to draw transparent and alpha images over.
  //
  pBkgColor = &bkgColor;
  if (png_get_bKGD(png_ptr, info_ptr, &pBackground)) {
    png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
    pBkgColor->red   = (png_byte) pBackground->red;
    pBkgColor->green = (png_byte) pBackground->green;
    pBkgColor->blue  = (png_byte) pBackground->blue;
  } else {
    pBkgColor = NULL;
  }
  //
  // If required set gamma conversion
  //
  if (png_get_gAMA(png_ptr, info_ptr, &Gamma)) {
    png_set_gamma(png_ptr, (double)2.2, Gamma);
  }
  //
  // After the transformations have been registered update info_ptr data
  //
  png_read_update_info(png_ptr, info_ptr);
  //
  // Get again width, height and the new bit-depth and color-type
  //
  png_get_IHDR(png_ptr, info_ptr, &Width, &Height, &BitDepth, &ColorType, NULL, NULL, NULL);
  //
  // Row_bytes is the width x number of channels
  //
  RowBytes = png_get_rowbytes(png_ptr, info_ptr);
  Channels = png_get_channels(png_ptr, info_ptr);
  //
  // Now we can allocate memory to store the image
  //
  if ((pImageData = (png_byte *)malloc(RowBytes * Height * sizeof(png_byte))) == NULL) {
    png_error(png_ptr, "Out of memory");
  }
  //
  // And allocate memory for an array of row-pointers
  //
  if ((ppRowPointers = (png_bytepp)malloc(Height * sizeof(png_bytep))) == NULL) {
    png_error(png_ptr, "Out of memory");
  }
  //
  // Set the individual row-pointers to point at the correct offsets
  //
  for (i = 0; i < Height; i++) {
    ppRowPointers[i] = pImageData + i * RowBytes;
  }
  //
  // Read the whole image
  //
  png_read_image(png_ptr, ppRowPointers);
  //
  // Cleanup memory except image data
  //
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  free(ppRowPointers);
  pImageDataOld = pImageData;
  {
    //
    // Allocate line buffer(s)
    //
    hColor = GUI_ALLOC_AllocZero(Width * sizeof(U32));
    hBkGnd = GUI_ALLOC_AllocZero(Width * sizeof(LCD_PIXELINDEX));
    if (hColor && hBkGnd) {
      #if (GUI_WINSUPPORT)
        GUI_RECT Rect;
      #endif

      pColor = (U32 *)GUI_LOCK_H(hColor);
      pBkGnd = (LCD_PIXELINDEX *)GUI_LOCK_H(hBkGnd);
      BitsPerPixel = LCD_GetBitsPerPixel();
      if (BitsPerPixel <= 8) {
        BytesPerPixel = 1;
      } else if (BitsPerPixel <= 16) {
        BytesPerPixel = 2;
      } else {
        BytesPerPixel = 4;
      }
      //
      // Get function pointer(s)
      //
      pfIndex2Color = GUI_GetpfIndex2ColorEx(GUI_Context.SelLayer);
      pfColor2Index = GUI_GetpfColor2IndexEx(GUI_Context.SelLayer);
      //
      // Iterate over window manager rectangles
      //
      #if (GUI_WINSUPPORT)
        WM_ADDORG(x0,y0);
        Rect.x1 = (Rect.x0 = x0) + Width - 1;
        Rect.y1 = (Rect.y0 = y0) + Height - 1;
        WM_ITERATE_START(&Rect) {
      #endif
      //
      // Iterate over all lines
      //
      for (y = 0; y < Height; y++) {
        pWrite = pColor;
        HasAlpha = HasTrans = 0;
        //
        // Read one line of pixel data
        //
        for (x = 0; x < Width; x++) {
          U8 r, g, b, a;
          U32 Color;
          r = *pImageData++;
          g = *pImageData++;
          b = *pImageData++;
          if (Channels == 4) { // If alpha channel exist...
            a = 255 - *pImageData++;
            if (a < 255) {
              HasAlpha = 1;
            } else if (a == 255) {
              HasTrans = 1;
            }
            Color = r + ((U16)g << 8) + ((U32)b << 16) + ((U32)a << 24);
          } else {
            Color = r + ((U16)g << 8) + ((U32)b << 16);
          }
          *pWrite++ = Color;
        }
        //
        // Read background if transparency or alpha exist
        //
        if (HasAlpha || HasTrans) {
          LCD_ReadRect(x0, y0 + y, x0 + Width - 1, y0 + y, pBkGnd, GUI_Context.pDeviceAPI);
          _ExpandPixelIndexArray(pBkGnd, Width, BitsPerPixel);
        }
        if (HasAlpha) {
          //
          // Mix with background
          //
          for (x = 0; x < Width; x++) {
            U32 BkColor, DataColor, Color;
            U8 Alpha;
            Alpha = *(pColor + x) >> 24;
            if (Alpha) {
              BkPixelIndex = *(pBkGnd + x);
              BkColor = pfIndex2Color(BkPixelIndex);
              DataColor = *(pColor + x) & 0xFFFFFF;
              Color = LCD_MixColors256(DataColor, BkColor, 255 - Alpha);
              *(pBkGnd + x) = pfColor2Index(Color);
            } else {
              Color = *(pColor + x) & 0xFFFFFF;
              *(pBkGnd + x) = pfColor2Index(Color);
            }
          }
        } else {
          //
          // Store data
          //
          for (x = 0; x < Width; x++) {
            U32 Color;
            U8 Alpha;
            Alpha = *(pColor + x) >> 24;
            if (Alpha == 0) {
              Color = *(pColor + x);
              *(pBkGnd + x) = pfColor2Index(Color);
            }
          }
        }
        //
        // Draw line of pixels
        //
        _CompactPixelIndexArray(pBkGnd, Width, BitsPerPixel);
        LCD_DrawBitmap(x0, y0 + y, Width, 1, 1, 1, BytesPerPixel * 8, 0, (U8 *)pBkGnd, NULL);
      }
      pImageData = pImageDataOld;
      #if (GUI_WINSUPPORT)
        } WM_ITERATE_END();
      #endif
      //
      // Unlock pointers
      //
      GUI_UNLOCK_H(pColor);
      GUI_UNLOCK_H(pBkGnd);
    }
    GUI_ALLOC_Free(hColor);
    GUI_ALLOC_Free(hBkGnd);
  }
  //
  // Cleanup image data
  //
  free(pImageData);
  return 0;
}

/*********************************************************************
*
*       _GetSize
*/
static int _GetSize(GUI_GET_DATA_FUNC * pfGetData, void * p, U32 * pxSize, U32 * pySize) {
  png_structp png_ptr  = NULL;
  png_infop   info_ptr = NULL;
  int BitDepth, ColorType;
  GUI_PNG_CONTEXT Context = {0};

  Context.pfGetData = pfGetData;
  Context.pParam    = p;
  //
  // Get image header
  //
  if (_GetImageHeader(&png_ptr, &info_ptr, &Context, pxSize, pySize, &BitDepth, &ColorType)) {
    return 1;
  }
  //
  // Cleanup memory
  //
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_PNG_GetXSizeEx
*/
int GUI_PNG_GetXSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p) {
  U32 Width, Height;
  if (_GetSize(pfGetData, p, &Width, &Height)) {
    return 0;
  }
  return (int)Width;
}

/*********************************************************************
*
*       GUI_PNG_GetXSize
*/
int GUI_PNG_GetXSize(const void * pFileData, int FileSize) {
  GUI_PNG_PARAM Param = {0};
  Param.pFileData = (const U8 *)pFileData;
  Param.FileSize  = FileSize;
  return GUI_PNG_GetXSizeEx(GUI_PNG__GetData, &Param);
}

/*********************************************************************
*
*       GUI_PNG_GetYSizeEx
*/
int GUI_PNG_GetYSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p) {
  U32 Width, Height;
  if (_GetSize(pfGetData, p, &Width, &Height)) {
    return 0;
  }
  return (int)Height;
}

/*********************************************************************
*
*       GUI_PNG_GetYSize
*/
int GUI_PNG_GetYSize(const void * pFileData, int FileSize) {
  GUI_PNG_PARAM Param = {0};
  Param.pFileData = (const U8 *)pFileData;
  Param.FileSize  = FileSize;
  return GUI_PNG_GetYSizeEx(GUI_PNG__GetData, &Param);
}

/*********************************************************************
*
*       GUI_PNG_DrawEx
*/
int  GUI_PNG_DrawEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0) {
  GUI_PNG_CONTEXT Context = {0};
  int r;

  GUI_LOCK();
  Context.pfGetData = pfGetData;
  Context.pParam    = p;
  r = _Draw(x0, y0, &Context);
  GUI_UNLOCK();
  return r;
}

/*********************************************************************
*
*       GUI_PNG_Draw
*/
int GUI_PNG_Draw(const void * pFileData, int FileSize, int x0, int y0) {
  GUI_PNG_PARAM Param = {0};
  int r;

  GUI_LOCK();
  Param.pFileData = (const U8 *)pFileData;
  Param.FileSize  = FileSize;
  r = GUI_PNG_DrawEx(GUI_PNG__GetData, &Param, x0, y0);
  GUI_UNLOCK();
  return r;
}

/*************************** End of file ****************************/
