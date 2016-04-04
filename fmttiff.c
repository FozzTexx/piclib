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

#include <appkit/tiff.h>
#include <streams/streams.h>
#include <sys/file.h>
#include <stdio.h>

#include "piclib.h"
#include "fmttiff.h"

int colorspacetomask(int space)
{
  switch (space) {
  case NX_OneIsBlackColorSpace:
  case NX_OneIsWhiteColorSpace:
    return MONOMASK;

  case NX_RGBColorSpace:
    return COLORMASK;

  case NX_CMYKColorSpace:
    return COLORMASK;

  case 6:
    return COLORMASK | ALPHAMASK;

  default:
    fprintf(stderr, "Don't know colorspace %i\n", space);
  }

  return -1;
}

int load_tiff(char *filename, image *aPic)
{
  NXTIFFInfo tiff_info;
  NXStream *stream = 0;
  byte *data;
  int fd;
  int i, j;
  byte *p, *q;

  int open();
  void close();
  void free();
  
  
  if (((fd = open(filename, O_RDONLY)) != -1) && (stream = NXOpenFile(fd, NX_READONLY))) {
    data = NXReadTIFF(0, stream, &tiff_info, NULL);
    NXClose(stream);
    close(fd);
  }
  else {
    if (stream)
      NXClose(stream);
    if (fd != -1)
      close(fd);
    return PICERR_CANTOPEN;
  }

  if (!data)
    return tiff_info.error;

  constructImage(aPic, colorspacetomask(tiff_info.image.photoInterp), tiff_info.image.planarConfig,
		  tiff_info.image.width, tiff_info.image.height,
		  tiff_info.image.bitsPerSample, data, NULL);
  
  if (tiff_info.image.photoInterp == NX_CMYKColorSpace && tiff_info.image.samplesPerPixel == 4) {
    for (p = data, q = aPic->data, i = 0, j = tiff_info.image.width * tiff_info.image.height * 4; i < j; p += 4, q += 3, i += 4) {
      q[0] = ((MV - p[0]) * (MV - p[3])) / 255;
      q[1] = ((MV - p[1]) * (MV - p[3])) / 255;
      q[2] = ((MV - p[2]) * (MV - p[3])) / 255;
    }
  }
  
  free(data);

  return PICERR_NOERR;
}

int save_tiff(char *filename, image *aPic)
{
  NXStream *stream = 0;
  NXImageInfo picInfo;
  int fd;
  int bits;
  byte *data, *palette;
  int err;
  
  int open();
  void close();
  void free();


  picInfo.width = aPic->width;
  picInfo.height = aPic->height;
  picInfo.samplesPerPixel = 1 + (aPic->photoInterp & COLORMASK);
  picInfo.planarConfig = aPic->photoInterp == COLORMASK ? MESHED : PLANAR;
  picInfo.photoInterp = aPic->photoInterp;
	
  if (err = unconstructImage(aPic, picInfo.photoInterp, picInfo.planarConfig, &bits, &data, &palette))
    return err;
  
  picInfo.bitsPerSample = bits;
  
  if (((fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644)) != -1) && (stream = NXOpenFile(fd, NX_WRITEONLY))) {
    NXWriteTIFF(stream, &picInfo, data);
    NXClose(stream);
    close(fd);
  }
  else {
    if (stream)
      NXClose(stream);
    if (fd != -1)
      close(fd);
    return PICERR_CANTOPEN;
  }

  free(data);
  
  return PICERR_NOERR;
}

int info_tiff(char *filename, imageInfo *info, formatInfo *fInfo)
{
  NXTIFFInfo tiff_info;
  NXStream *stream = 0;
  int err;
  int fd;

  int open();
  void close();
  void free();
  

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
    
  if (((fd = open(filename, O_RDONLY)) != -1) && (stream = NXOpenFile(fd, NX_READONLY))) {
    err = NXGetTIFFInfo(0, stream, &tiff_info);
    NXClose(stream);
    close(fd);
  }
  else {
    if (stream)
      NXClose(stream);
    if (fd != -1)
      close(fd);
    return PICERR_CANTOPEN;
  }

  if (!err)
    return tiff_info.error;

  info->type = "Aldus/Microsoft Tagged Image File Format";
  info->extension = "tiff";
  info->photoInterp = colorspacetomask(tiff_info.image.photoInterp);
  info->planarConfig = tiff_info.image.planarConfig;
  info->width = tiff_info.image.width;
  info->height = tiff_info.image.height;
  info->bitsPerPixel = tiff_info.image.bitsPerSample;

  return PICERR_NOERR;
}  
