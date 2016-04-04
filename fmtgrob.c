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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "fmtgrob.h"

#define tohex(x) ({int _x = x; _x + '0' + 7 * (_x > 9);})
  
int rbits(int val)
{
  int i, j;


  for (i = j = 0; i < 4; i++) {
    j <<= 1;
    j |= val & 1;
    val >>= 1;
  }

  return j;
}

int load_grob(char *filename, image *aPic)
{
  FILE *file;
  unsigned char buf[40];
  byte *data = NULL;
  int width, height;
  int cwid;
  int i, j;


  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fread(buf, 1, 6, file);
  if (!strncmp((char *) buf, "%%HP: ", 6)) {
    fread(buf, 1, 19, file);
    fscanf(file, "%i %i ", &width, &height);
    cwid = ((width + 7) / 8) * 8;
    data = malloc(cwid * height / 8);
    
    j = 0;
    while(j < (cwid * height / 4)) {
      do {
	i = fgetc(file);
      } while (i != EOF && !isxdigit(i));
      
      if (i == EOF)
	break;
      
      i -= '0' + 7 * (i >= 'A') + 32 * (i >= 'a');
      i = rbits(~i);
      if (j & 1)
	data[j / 2] |= i & 0xf;
      else
	data[j / 2] = (i & 0xf) << 4;
      j++;
    }
  }
  else if (!strncmp((char *) buf, "HPHP48", 6)) {
    fread(buf, 1, 5, file);
    fread(buf, 1, 4, file);
    width = buf[1] << 8 | buf[0];
    height = buf[3] << 8 | buf[2];
    fread(buf, 1, 3, file);
    data = malloc(width * height);
    fread(data, 1, width * height, file);

    for (i = 0; i < width * height; i++) {
      j = ~data[i];
      data[i] = (j & 1) << 7 | (j & 2) << 5 | (j & 4) << 3 | (j & 8) << 1 | (j & 16) >> 1 | (j & 32) >> 3 | (j & 64) >> 5 | (j & 128) >> 7;
    }
  }
  
  fclose(file);

  if (!data)
    return PICERR_BADFORMAT;
  
  constructImage(aPic, MONOMASK, PLANAR, width, height, 1, data, NULL);
  free(data);
  
  return PICERR_NOERR;
}

int save_grob(char *filename, image *aPic)
{
  FILE *file;
  image myPic;
  int i, j, k;
  int bpl;
  byte *data;
  int bits;

  
  copyImage(&myPic, aPic);
  makeMono(&myPic);
  ditherImage(&myPic, NULL, 1);

  unconstructImage(&myPic, MONOMASK, PLANAR, 0, &bits, &data, NULL);
  bpl = (myPic.width + 7) / 8;

  if (!(file = fopen(filename, "w")))
    return PICERR_CANTOPEN;
  
  fprintf(file, "%%%%HP: T(3)A(D)F(.);\n\rGROB");
  fprintf(file, " %i %i ", aPic->width, aPic->height);
  
  for (i = 0; i < bpl * myPic.height; i++) {
    j = data[i] & 0xf;
    k = (data[i] >> 4) & 0xf;
    fprintf(file, "%c%c", tohex(rbits(~k)), tohex(rbits(~j)));
  }

  fprintf(file, "\r\n");
  fclose (file);

  free(data);
  freeImage(&myPic);

  return PICERR_NOERR;
}

int info_grob(char *filename, imageInfo *info, formatInfo *fInfo)
{
  FILE *file;
  char buf[40];
  int width, height;


  if (!filename) { /* Just want longname and extension */
    fInfo->type = "HP48 Graphic Image";
    fInfo->extension = "gro";
    fInfo->maxWidth = NOMAX;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = 1;
    fInfo->maxBitsPerColor = 1;
    fInfo->colorSpace = MONOMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }
  
  info->type = "HP48 Graphic Image";
  info->extension = "gro";
  info->photoInterp = MONOMASK;
  info->planarConfig = PLANAR;

  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fread(buf, 1, 25, file);

  fscanf(file, "%i %i ", &width, &height);

  fclose(file);

  info->width = width;
  info->height = height;
  info->bitsPerPixel = 1;
  
  return PICERR_NOERR;
}
