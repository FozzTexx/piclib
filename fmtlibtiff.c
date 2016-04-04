/* Copyright 1990-2016 by Chris Osborn <fozztexx@fozztexx.com>
 * http://insentricity.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "fmtlibtiff.h"
#include <tiffio.h>

int load_tiff(char *filename, image *aPic)
{
  TIFF *tif;
  uint32 width, height, npixels;
  int succ;
  uint32 *raster;


  if (!(tif = TIFFOpen(filename, "r")))
    return PICERR_CANTOPEN;
  
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
  npixels = width * height;
  raster = _TIFFmalloc(npixels * sizeof(uint32));
  if ((succ = TIFFReadRGBAImageOriented(tif, width, height, raster, 0,
					ORIENTATION_TOPLEFT)))
    constructImage(aPic, COLORMASK | ALPHAMASK, MESHED, width, height, 8,
		   (byte *) raster, NULL);
  
  _TIFFfree(raster);
  TIFFClose(tif);

  return succ ? PICERR_NOERR : PICERR_BADFORMAT;
}

int save_tiff(char *filename, image *aPic)
{
  return PICERR_NOERR;
}

int info_tiff(char *filename, imageInfo *info, formatInfo *fInfo)
{
  TIFF *tif;
  uint32 width, height;
  

  if (!filename) { /* Just want longname and extension */
    fInfo->type = "Aldus/Microsoft Tagged Image File Format";
    fInfo->extension = "tiff";
    fInfo->maxWidth = NOMAX;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = NOMAX;
    fInfo->maxBitsPerColor = 8;
    fInfo->colorSpace = COLORMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }

  if (!(tif = TIFFOpen(filename, "r")))
    return PICERR_CANTOPEN;

  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
  TIFFClose(tif);

  info->type = "Aldus/Microsoft Tagged Image File Format";
  info->extension = "tiff";
  info->photoInterp = COLORMASK | ALPHAMASK;
  info->planarConfig = MESHED;
  info->width = width;
  info->height = height;
  info->bitsPerPixel = 8;

  return PICERR_NOERR;
}
