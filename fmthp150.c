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

#include "fmthp150.h"
#include <stdio.h>
#include <stdlib.h>

int load_hp150(char *filename, image *aPic)
{
  byte *data;
  FILE *file;
  int i, j, k, l;
  

  file = fopen(filename, "r");

  data = malloc(24960);
  fread(data, 1, 7, file); /* Eat junk written by GWBASIC */
  fread(data, 1, 24960, file);

  fclose(file);

  for (i = 0; i < 24960; i++) {
    for (j = 0, k = 0, l = data[i]; j < 8; j++) {
      k <<= 1;
      k |= l & 1;
      l >>= 1;
    }
    data[i] = k;
  }
  
  constructImage(aPic, MONOMASK, PLANAR, 512, 390, 1, data, NULL);
  free(data);

  return PICERR_NOERR;
}

int save_hp150(char *filename, image *aPic)
{
  FILE *file;
  image myPic;
  int i, j, k;
  byte *data;


  copyImage(&myPic, aPic);
  makeMono(&myPic);
  ditherImage(&myPic, NULL, 1);

  data = malloc(24960);
  
  for (i = 0; i < 24960; i++) {
    k = 0;
    if ((i % 64) * 8 + 7 < myPic.width && (i / 64) < myPic.height) {
      for (j = 0, k = 0; j < 8; j++) {
	k <<= 1;
	k |= getval((i % 64) * 8 + (7 - j), i / 64, MONO, &myPic) > 127;
      }
    }
    data[i] = k;
  }

  file = fopen(filename, "w");
  fprintf(file, "%c%c%c%c%c%c%c", 0xfd, 0x00, 0xc0, 0x00, 0x00, 0x80, 0x61);
  fwrite(data, 1, 24960, file);
  fprintf(file, "%c", 0x1a);
  fclose(file);

  free(data);
  freeImage(&myPic);

  return PICERR_NOERR;
}

int info_hp150(char *filename, imageInfo *info, formatInfo *fInfo)
{
  if (!filename) { /* Just want longname and extension */
    fInfo->type = "HP 150 GWBASIC Binary Screen Image";
    fInfo->extension = "hp150";
    fInfo->maxWidth = 512;
    fInfo->maxHeight = 390;
    fInfo->maxBitsPerPixel = 1;
    fInfo->maxBitsPerColor = 1;
    fInfo->colorSpace = MONOMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }
  
  info->type = "HP 150 GWBASIC Binary Screen Image";
  info->extension = "hp150";
  info->photoInterp = MONOMASK;
  info->planarConfig = PLANAR;
  info->width = 512;
  info->height = 390;
  info->bitsPerPixel = 1;
  
  return PICERR_NOERR;
}
