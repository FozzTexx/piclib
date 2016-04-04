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

#include "piclib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>


int load_ppp(char *filename, image *aPic)
{
  FILE *file;
  byte *data, *palette;
  unsigned char buf[MAXPATHLEN + 1];
  char *p;
  int width, height;
  int i, c;
  int bp;
  

  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fread(buf, 8, 1, file);
  if (strncmp((char *) buf, "NOVA", 4))
    return PICERR_BADFORMAT;

  width = buf[4] | buf[5] << 8;
  height = buf[6] | buf[7] << 8;

  data = malloc(width * height);
  bp = 0;

  while ((c = getc(file)) != EOF) {
    switch (c) {
    case 255:
      fread(buf, 3, 1, file);
      c = buf[0];
      i = buf[1] | buf[2] << 8;
      for (; i; i--, bp++)
	data[bp] = c;
      break;
      
    default:
      data[bp++] = c;
      break;
    }
  }

  fclose(file);

  strcpy((char *) buf, filename);
  p = strrchr((char *) buf, '.');
  strcpy(p, ".pal");
  if (!(file = fopen((char *) buf, "r")))
    return PICERR_CANTOPEN;
  palette = malloc(768);
  fread(palette, 768, 1, file);
  fclose(file);
  for (i = 0; i < 768; i++) {
    palette[i] <<= 2;
  }
  
  constructImage(aPic, COLORMASK | PALETTEMASK, PLANAR, width, height, 8, data, palette);

  free(data);
  free(palette);

  return PICERR_NOERR;
}

int info_ppp(char *filename, imageInfo *info, formatInfo *fInfo)
{
  int width, height;
  FILE *file;
  unsigned char buf[256];

  
  if (!filename) {
    fInfo->type = "NOVA ppp format";
    fInfo->extension = "ppp";
    fInfo->maxWidth = NOMAX;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = 8;
    fInfo->maxBitsPerColor = 8;
    fInfo->colorSpace = MONOMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }
  
  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fgets((char *) buf, 8, file);
  fread(buf, 8, 1, file);
  if (strncmp((char *) buf, "NOVA", 4))
    return PICERR_BADFORMAT;

  width = buf[4] | buf[5] << 8;
  height = buf[6] | buf[7] << 8;

  info->type = "NOVA ppp format";
  info->extension = "ppp";
  info->photoInterp = MONOMASK;
  info->planarConfig = PLANAR;
  info->width = width;
  info->height = height;
  info->bitsPerPixel = 8;

  return PICERR_NOERR;
}
