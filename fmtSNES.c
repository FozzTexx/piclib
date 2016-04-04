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

#include "fmtSNES.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fliptile(int len, char *buf, int x, int y);

int load_snes(char *filename, image *aPic)
{
  FILE *file;
  byte buf[3];
  int nc, nt;
  byte *data, *palette;
  byte *map, *tiles, *cols;
  int i, j, k, x, y;
  

  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fread(buf, 1, 3, file);

  if (strncmp((char *) buf, "SNS", 3)) {
    fclose(file);
    return PICERR_BADFORMAT;
  }

  fread(buf, 1, 3, file);

  nc = buf[0] + 1;
  nt = buf[1] | buf[2] << 8;

  map = malloc(960 * 2);
  cols = malloc(nc * 2);
  tiles = malloc(nt * 64);

  fread(map, 1, 1920, file);
  fread(cols, 1, nc * 2, file);
  fread(tiles, 1, nt * 64, file);
  fclose(file);

  data = malloc(256 * 240);
  palette = malloc(256 * 3);

  for (i = 0; i < nc; i++) {
    k = cols[i * 2] | cols[i * 2 + 1] << 8;
    palette[i * 3 + 0] = ((k & 0x1f) * 255) / 31;
    palette[i * 3 + 1] = (((k >> 5) & 0x1f) * 255) / 31;
    palette[i * 3 + 2] = (((k >> 10) & 0x1f) * 255) / 31;
  }

  memset(data, 0, 256 * 240);
  
  for (i = 0; i < 240; i += 8)
    for (j = 0; j < 256; j += 8) {
      k = map[((i / 8) * 32 + (j / 8)) * 2] | map[((i / 8) * 32 + (j / 8)) * 2 + 1] << 8;
      for (y = 0; y < 8; y++)
	for (x = 0; x < 8; x++) {
	  data[(i + y) * 256 + j + x] |= ((tiles[(k & 0x3ff) * 64 + 0 * 16 + y*2] >> (7-x)) & 1) << 0;
	  data[(i + y) * 256 + j + x] |= ((tiles[(k & 0x3ff) * 64 + 0 * 16 + y*2+1] >> (7-x)) & 1) << 1;
	  data[(i + y) * 256 + j + x] |= ((tiles[(k & 0x3ff) * 64 + 1 * 16 + y*2] >> (7-x)) & 1) << 2;
	  data[(i + y) * 256 + j + x] |= ((tiles[(k & 0x3ff) * 64 + 1 * 16 + y*2+1] >> (7-x)) & 1) << 3;
	  data[(i + y) * 256 + j + x] |= ((tiles[(k & 0x3ff) * 64 + 2 * 16 + y*2] >> (7-x)) & 1) << 4;
	  data[(i + y) * 256 + j + x] |= ((tiles[(k & 0x3ff) * 64 + 2 * 16 + y*2+1] >> (7-x)) & 1) << 5;
	  data[(i + y) * 256 + j + x] |= ((tiles[(k & 0x3ff) * 64 + 3 * 16 + y*2] >> (7-x)) & 1) << 6;
	  data[(i + y) * 256 + j + x] |= ((tiles[(k & 0x3ff) * 64 + 3 * 16 + y*2+1] >> (7-x)) & 1) << 7;
	}
    }
  
  constructImage(aPic, COLORMASK | PALETTEMASK, PLANAR, 256, 240, 8, data, palette);

  free(map);
  free(cols);
  free(tiles);
  free(data);
  free(palette);
  
  return 0;
}

int save_snes(char *filename, image *aPic)
{
  int err;
  int bits;
  byte *data, *palette;
  FILE *file;
  int nc, nt;
  int i, j, k, l;
  byte *tiles;
  byte *map;
  int w, h, x, y;
  byte *p, *q;
  

  if (!(file = fopen(filename, "w")))
    return PICERR_CANTOPEN;

  if ((err = unconstructImage(aPic, aPic->photoInterp | PALETTEMASK, PLANAR, 0,
			      &bits, &data, &palette))) {
    fclose(file);
    return err;
  }

  nc = 1 << bits;
  fprintf(file, "SNS%c", nc - 1);

  map = malloc(960 * 2);
  tiles = malloc(960 * 64);

  for (i = 0; i < 960; i++)
    map[i * 2 + 0] = i & 0xff, map[i * 2 + 1] = (i >> 8) & 0xff;

  memset(tiles, 0, 960 * 64);
  w = aPic->width < 256 ? aPic->width : 256;
  h = aPic->height < 240 ? aPic->height : 240;

  for (i = 0; i < h; i += 8)
    for (j = 0; j < w; j += 8) {
      k = (i / 8) * 32 + j / 8;
      for (y = 0; y < 8 && i + y < h; y++)
	for (x = 0; x < 8 && j + x < w; x++) {
	  tiles[k * 64 + 0*16 + y*2] |= ((data[(i + y) * aPic->width + j + x] >> 0) & 1) << (7-x);
	  tiles[k * 64 + 0*16 + y*2+1] |= ((data[(i + y) * aPic->width + j + x] >> 1) & 1) << (7-x);
	  tiles[k * 64 + 1*16 + y*2] |= ((data[(i + y) * aPic->width + j + x] >> 2) & 1) << (7-x);
	  tiles[k * 64 + 1*16 + y*2+1] |= ((data[(i + y) * aPic->width + j + x] >> 3) & 1) << (7-x);
	  tiles[k * 64 + 2*16 + y*2] |= ((data[(i + y) * aPic->width + j + x] >> 4) & 1) << (7-x);
	  tiles[k * 64 + 2*16 + y*2+1] |= ((data[(i + y) * aPic->width + j + x] >> 5) & 1) << (7-x);
	  tiles[k * 64 + 3*16 + y*2] |= ((data[(i + y) * aPic->width + j + x] >> 6) & 1) << (7-x);
	  tiles[k * 64 + 3*16 + y*2+1] |= ((data[(i + y) * aPic->width + j + x] >> 7) & 1) << (7-x);
	}
    }

  nt = 960;

  for (i = 0; i < nt-1; i++) {
    p = &tiles[i * 64];
    for (j = i+1; j < nt; j++) {
      q = &tiles[j * 64];
      if (!memcmp(p, q, 64)) {
	memmove(q, q+64, (nt - j - 1) * 64);
	nt--;
	for (k = 0; k < 960; k++) {
	  l = map[k*2] | map[k*2+1] << 8;
	  if (l == j)
	    map[k*2] = i & 0xff, map[k*2+1] = (i >> 8) & 0xff;
	  else if (l > j)
	    map[k*2] = (l-1) & 0xff, map[k*2+1] = ((l-1) >> 8) & 0xff;
	}
	j--;
      }
    }
  }
  
  fprintf(file, "%c%c", nt & 0xff, (nt >> 8) & 0xff);
  fwrite(map, 1, 1920, file);
  
  for (i = 0; i < nc; i++) {
    j = ((palette[i * 3 + 0] * 31) / 255) | ((palette[i * 3 + 1] * 31) / 255) << 5 | ((palette[i * 3 + 2] * 31) / 255) << 10;
    fprintf(file, "%c%c", j & 0xff, (j >> 8) & 0xff);
  }
  
  fwrite(tiles, 1, nt * 64, file);
  
  fclose(file);

  free(data);
  free(palette);
  free(tiles);
  free(map);
  
  return 0;
}

int info_snes(char *filename, imageInfo *info, formatInfo *fInfo)
{
  if (!filename) { /* Just want longname and extension */
    fInfo->type = "Chris Osborn's Super Nintendo Graphics Format";
    fInfo->extension = "sngf";
    fInfo->maxWidth = 256;
    fInfo->maxHeight = 240;
    fInfo->maxBitsPerPixel = 8;
    fInfo->maxBitsPerColor = 5;
    fInfo->colorSpace = COLORMASK | PALETTEMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }

  info->type = "Chris Osborn's Super Nintendo Graphics Format";
  info->extension = "sngf";
  
  return PICERR_NOERR;
}

void fliptile(int len, char *buf, int x, int y)
{
  int i, j, k, l;
  char buf2[16];
  
  
  if (x)
    for (i = 0; i < len * 8; i++) {
      for (j = k = 0, l = buf[i]; j < 8; j++, k <<= 1, l >>= 1)
	k |= l & 1;
      buf[i] = k;
    }

  if (y)
    for (i = 0; i < 4; i++) {
      memcpy(buf2, &buf[i * len], len);
      memcpy(&buf[i * len], &buf[(7 - i) * len], len);
      memcpy(&buf[(7 - i) * len], buf2, len);
    }
  
  return;
}
