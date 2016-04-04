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

#include "fmtast.h"
#include <stdio.h>
#include <stdlib.h>

int load_ast(char *filename, image *aPic)
{
  byte *data;
  FILE *file;


  file = fopen(filename, "r");

  data = malloc(96000);
  fread(data, 1, 96000, file);

  fclose(file);

  constructImage(aPic, COLORMASK, PLANAR, 320, 200, 4, data, NULL);
  free(data);

  return PICERR_NOERR;
}

int save_ast(char *filename, image *aPic)
{
  FILE *file;
  image myPic;
  int i, j, k;


  copyImage(&myPic, aPic);
  ditherImage(&myPic, NULL, 4);

  file = fopen(filename, "w");
  
  for (j = 0; j < 200; j++)
    for (i = 0; i < 320; i += 2) {
      k = (getval(i, j, RED, &myPic) / 17 << 4) | (getval(i + 1, j, RED, &myPic) / 17);
      fputc(k, file);
    }

  for (j = 0; j < 200; j++)
    for (i = 0; i < 320; i += 2) {
      k = (getval(i, j, GREEN, &myPic) / 17 << 4) | (getval(i + 1, j, GREEN, &myPic) / 17);
      fputc(k, file);
    }
  
  for (j = 0; j < 200; j++)
    for (i = 0; i < 320; i += 2) {
      k = (getval(i, j, BLUE, &myPic) / 17 << 4) | (getval(i + 1, j, BLUE, &myPic) / 17);
      fputc(k, file);
    }

  fclose(file);
  freeImage(&myPic);

  return PICERR_NOERR;
}

int info_ast(char *filename, imageInfo *info, formatInfo *fInfo)
{
  if (!filename) { /* Just want longname and extension */
    fInfo->type = "AST VisionEffects Color Data File";
    fInfo->extension = "ast";
    fInfo->maxWidth = 320;
    fInfo->maxHeight = 200;
    fInfo->maxBitsPerPixel = 12;
    fInfo->maxBitsPerColor = 4;
    fInfo->colorSpace = COLORMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }

  info->type = "AST VisionEffects Color Data File";
  info->extension = "ast";
  info->photoInterp = COLORMASK;
  info->planarConfig = PLANAR;
  info->width = 320;
  info->height = 200;
  info->bitsPerPixel = 4;
  
  return PICERR_NOERR;
}
