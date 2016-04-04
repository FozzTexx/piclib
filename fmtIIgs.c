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

#include "fmtIIgs.h"
#include <stdio.h>
#include <stdlib.h>


void swap(int *a, int *b)
{
  int c;

  c = *a; *a = *b; *b = c;

  return;
}

void extend_line(unsigned char *line)
{
  int i, j;


  for (i = 319; i >= 0; i--) {
    j = i * 2;
    line[j + 1] = line[j] = line[i];
  }
}

int reduce_pals(int pal_use[], unsigned char *pals, int limit)
{
  int i, j, k, l, size;


  for (i = 0; i < limit; i++)
    if (pal_use[i] > 0)
      pal_use[i] = i;

  size = limit;

  for (i = 0; i < limit - 1; i++) {
    if (pal_use[i] == i) {
      k = i * 3;
      for (j = i + 1; j < limit; j++) {
	l = j * 3;
	if ((pal_use[j] != -1) && (pals[k] == pals[l]) && (pals[k + 1] == pals[l + 1]) && (pals[k + 2] == pals[l + 2])) {
	  size--;
	  pal_use[j] = i;
	}
      }
    }
    else {
      if (pal_use[i] == -1)
	size--;
    }
  }

  if (size != limit)
    for (k = limit, i = limit - 1; i >= 0; i--)
      if (pal_use[i] != i) {
	for (j = i + 1; j < limit; j++) {
	  if (j < k) {
	    l = (j - 1) * 3;
	    pals[l] = pals[l + 3];
	    pals[l + 1] = pals[l + 4];
	    pals[l + 2] = pals[l + 5];
	  }
	  if (pal_use[j] > i)
	    pal_use[j]--;
	}
	k--;
      }
  
  return size;
}

static inline int apf_make_line(int scb, int width, int mastermode, unsigned char *from_line, unsigned char *to_line)
{
  int i, lw, bpp, ppb;
  int j, k;


  lw = width * (1 + (mastermode == 0x80));
  bpp = (4 * width) / lw;
  ppb = 8 / bpp;
  j = ppb - 1;
  k = (1 << bpp) - 1;

  for (i = 0; i < lw; i++) {
    to_line[i] = (from_line[i / ppb] >> ((j - i % ppb) * bpp)) & k;
    if (mastermode == 0x80)
      to_line[i] += (i % ppb) * 4;
  }

  if (scb & 0x20)
    for (i = 0; i < lw; i++) {
      if (!to_line[i])
	to_line[i] = ppb;
      else
	ppb = to_line[i];
    }

  return lw;
}

static inline int apf_unpack(FILE *in, int *wh, int *ht, byte **d, byte *palette)
{
  int mastermode, width, height;
  int bytew;
  int numpalettes;
  char name[255];
  byte *data;
  int *scanlinedir;
  int i, j, k, l, m, n;
  int b[4];
  int type, val;


#if 0
  length = fgetc(in) + (fgetc(in) << 8) + (fgetc(in) << 16) + (fgetc(in) << 24);
#else
  fgetc(in);
  fgetc(in);
  fgetc(in);
  fgetc(in);
#endif
  
  i = fgetc(in);
  for (j = 0; j < i; j++)
    name[j] = fgetc(in);
  name[j] = 0;

  if (name[0] != 'M' || name[1] != 'A' || name[2] != 'I' || name[3] != 'N' || name[4] != 0)
    return -PICERR_BADFORMAT;
  
  mastermode = fgetc(in) + ((fgetc(in) << 8) & 0x80);
  width = fgetc(in) + (fgetc(in) << 8);
  
  numpalettes = fgetc(in) + (fgetc(in) << 8);
  for (i = 0; i < numpalettes * 32; i++)
    palette[i] = fgetc(in);
  
  height = fgetc(in) + (fgetc(in) << 8);

  scanlinedir = (int *) malloc(height * sizeof(int));
  for (i = 0; i < height; i++)
    scanlinedir[i] = fgetc(in) + (fgetc(in) << 8) + (fgetc(in) << 16) + (fgetc(in) << 24);

  i = 2 + 2 * (mastermode == 0x80);
  bytew = width / i + (width % i > 0);
  *d = data = (unsigned char *) malloc(height * bytew + height);
  for (i = 0; i < height; i++) {
    m = 0;
    k = scanlinedir[i] & 0xffff;
    for (j = 0; j < k; j++) {
      l = fgetc(in);
      type = (l >> 6) & 0x3;
      val = l & 0x3f;
      switch (type) {
      case 0:
	for (l = 0; l <= val; l++, j++, m++)
	  data[m + i * bytew] = fgetc(in);
	break;

      case 1:
	n = fgetc(in);
	j++;
	for (l = 0; l <= val; l++, m++)
	  data[m + i * bytew] = n;
	break;

      case 2:
	for (l = 0; l < 4; l++, j++)
	  b[l] = fgetc(in);
	for (l = 0; l <= val; l++)
	  for (n = 0; n < 4; n++, m++)
	    data[m + i * bytew] = b[n];
	break;

      case 3:
	b[0] = fgetc(in);
	j++;
	for (l = 0; l <= val; l++)
	  for (n = 0; n < 4; n++, m++)
	    data[m + i * bytew] = b[0];
	break;
      }
    }
  }

  *wh = width;
  *ht = height;

  for (i = 0; i < height; i++)
    data[i + height * bytew] = (scanlinedir[i] >> 16) & 0xff;
  free(scanlinedir);

  return mastermode;
}

int load_apf(char *filename, image *aPic)
{
  FILE *apffile;
  unsigned char *p;
  unsigned int n;
  int i, j, k, l, m;
  int scb_use[16], scb_order[16];
  int *scbs;
  int pal_use[256];
  int iw, ih, nc, bw;
  int mastermode;
  byte *palette;
  byte *data;
  

  if (!(apffile = fopen(filename, "r")))
    return PICERR_CANTOPEN;
  
  if (!(palette = malloc(768)))
    return PICERR_CANTALLOC;
  
  mastermode = apf_unpack(apffile, &iw, &ih, &data, palette);
  if (mastermode < 0)
    return -mastermode;
  
  fclose(apffile);

  /* Convert palette to full bytes */

  for (i = 0; i < 16; i++) {
    scb_order[i] = i;
    scb_use[i] = 0;
  }

  i = 2 + 2 * (mastermode == 0x80);
  bw = (iw / i) + (iw % i > 0);

  scbs = (int *) malloc(ih * sizeof(int));
  
  for (i = 0; i < ih; i++) {
    scbs[i] = data[i + ih * bw];
    (scb_use[scbs[i] & 0xf])++;
  }

  for (i = 0; i < 15; i++)
    for (j = i + 1; j < 16; j++)
      if (scb_use[i] < scb_use[j]) {
	swap(&scb_use[i], &scb_use[j]);
	swap(&scb_order[i], &scb_order[j]);
      }

  for (nc = 0, i = 0; i < 16; i++) {
    if (scb_use[i])
      nc += 16;
  }

  p = palette;
  if ((palette = malloc(768)) == NULL)
    return PICERR_CANTALLOC;

  for (i = 0; i < 16; i++) {
    k = i * 48; l = scb_order[i] * 32;
    for (j = 0; j < 16; j++) {
      m = k + j * 3; n = l + j * 2;
      palette[m] = ((p[n + 1] & 0xf) * 255) / 15;
      palette[m + 1] = (((p[n] >> 4) & 0xf) * 255) / 15;
      palette[m + 2] = ((p[n] & 0xf) * 255) / 15;
    }
  }

  free(p);

  for (i = 0; i < 16; i++)
    scb_use[scb_order[i]] = i;
  
  /* Convert pixels in image to full bytes */

  p = data;
    
  if ((data = malloc(iw * ih)) == NULL)
    return PICERR_CANTALLOC;
    
  for (i = (ih - 1) * bw; i >=0; i -= bw) {
    j = apf_make_line(scbs[i / bw], iw, mastermode, &p[i], &data[(i / bw) * iw]);
    if (j < iw)
      extend_line(&data[(i / bw) * iw]);
  }

  free(p);

  for (i = 0; i < nc; i++)
    pal_use[i] = -1;
    
  for (i = 0; i < iw * ih; i++) {
    data[i] += scb_use[scbs[i / iw] & 0xf] * 16;
    pal_use[data[i]] = 1;
  }

  if ((j = reduce_pals(pal_use, palette, nc)) < nc) {
    nc = j;
    for (i = 0; i < iw * ih; i++)
      data[i] = pal_use[data[i]];
  }

  free(scbs);

  constructImage(aPic, COLORMASK | PALETTEMASK, PLANAR, iw, ih, 8, data, palette);
  free(data);
  free(palette);

  return PICERR_NOERR;
}

int info_apf(char *filename, imageInfo *info, formatInfo *fInfo)
{
  FILE *in;
  int i, j;
  int numpalettes;
  int width, height;
  char name[255];
  

  if (!filename) { /* Just want format info */
    fInfo->type = "Apple IIgs Super Hi-Res Picture File";
    fInfo->extension = "apf";
    fInfo->maxWidth = 640;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = 4; /* Sorta. 4 per scan line. */
    fInfo->maxBitsPerColor = 4;
    fInfo->colorSpace = COLORMASK | PALETTEMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }
  
  if (!(in = fopen(filename, "r")))
    return PICERR_CANTOPEN;

#if 0
  length = fgetc(in) + (fgetc(in) << 8) + (fgetc(in) << 16) + (fgetc(in) << 24);
#else
  fgetc(in);
  fgetc(in);
  fgetc(in);
  fgetc(in);
#endif
  
  i = fgetc(in);
  for (j = 0; j < i; j++)
    name[j] = fgetc(in);
  name[j] = 0;

  if (name[0] != 'M' || name[1] != 'A' || name[2] != 'I' || name[3] != 'N' || name[4] != 0)
    return PICERR_BADFORMAT;

#if 0
  mastermode = fgetc(in) + ((fgetc(in) << 8) & 0x80);
#else
  fgetc(in);
  fgetc(in);
#endif
  
  width = fgetc(in) + (fgetc(in) << 8);
  
  numpalettes = fgetc(in) + (fgetc(in) << 8);
  for (i = 0; i < numpalettes * 32; i++)
    fgetc(in);
  
  height = fgetc(in) + (fgetc(in) << 8);

  fclose(in);
  
  info->type = "Apple IIgs Super Hi-Res Picture File";
  info->extension = "apf";
  info->photoInterp = COLORMASK | PALETTEMASK;
  info->width = width;
  info->height = height;
  info->bitsPerColor = 12;
  info->numColors = 16 * numpalettes;

  return PICERR_NOERR;
}

static inline int gsbin_make_line(int scb, unsigned char *from_line, unsigned char *to_line)
{
  int i, lw, bpp, ppb;
  int j, k;


  lw = 320 + 320 * ((scb & 0x80) >> 7);
  bpp = (8 * 160) / lw;
  ppb = lw / 160;
  j = ppb - 1;
  k = (1 << bpp) - 1;
  
  for (i = 0; i < lw; i++) {
    to_line[i] = (from_line[i / ppb] >> ((j - i % ppb) * bpp)) & k;
    if (lw == 640)
      to_line[i] += (i % ppb) * 4;
  }

  if (scb & 0x20)
    for (i = 0; i < lw; i++) {
      if (!to_line[i])
	to_line[i] = ppb;
      else
	ppb = to_line[i];
    }

  return lw;
}

int load_gsbin(char *filename, image *aPic)
{
  FILE *imagef;
  unsigned long size;
  unsigned char *p;
  void strmfe();
  unsigned int n;
  int i, j, k, l, m;
  int scb_use[16], scb_order[16];
  int scbs[200];
  int pal_use[256];
  int iw, ih, nc;
  unsigned char *gsimage;
  byte *palette, *data;

  
  size = 32768;

  if (!(imagef = fopen(filename, "r")))
    return PICERR_CANTOPEN;
      
  if (!(gsimage = malloc(size)))
    return PICERR_CANTALLOC;
      
  p = gsimage;
  do {				/* get one 64K block at a gulp */
    if (size > 0xff00)
      n = 0xff00;
    else
      n = (unsigned int) size;
    fread(p, n, 1, imagef);
    size -= (long)n;
  } while (size > 0);
      
  fclose(imagef);

  if (!(palette = malloc(768)))
    return PICERR_CANTALLOC;

  /* Convert palette to full bytes */

  for (i = 0; i < 16; i++) {
    scb_order[i] = i;
    scb_use[i] = 0;
  }

  iw = 320; ih = 200;
    
  for (i = 0; i < 200; i++) {
    scbs[i] = gsimage[i + 0x7D00];
    scb_use[scbs[i] & 0xf]++;
    if (scbs[i] & 0x80)
      iw = 640;
  }

  for (i = 0; i < 15; i++)
    for (j = i + 1; j < 16; j++)
      if (scb_use[i] < scb_use[j]) {
	swap(&scb_use[i], &scb_use[j]);
	swap(&scb_order[i], &scb_order[j]);
      }

  for (nc = 0, i = 0; i < 16; i++) {
    if (scb_use[i])
      nc += 16;
  }

  p = &gsimage[0x7E00];

  for (i = 0; i < 16; i++) {
    k = i * 48; l = scb_order[i] * 32;
    for (j = 0; j < 16; j++) {
      m = k + j * 3; n = l + j * 2;
      palette[m] = ((p[n + 1] & 0xf) * 255) / 15;
      palette[m + 1] = (((p[n] >> 4) & 0xf) * 255) / 15;
      palette[m + 2] = ((p[n] & 0xf) * 255) / 15;
    }
  }

  for (i = 0; i < 16; i++)
    scb_use[scb_order[i]] = i;
  
  /* Convert pixels in image to full bytes */

  if (!(data = malloc(iw * ih)))
    return PICERR_CANTALLOC;
    
  for (i = 31840; i >=0; i -= 160) {
    j = gsbin_make_line(scbs[i / 160], &gsimage[i], &data[(i / 160) * iw]);
    if (j < iw)
      extend_line(&data[(i / 160) * iw]);
  }

  for (i = 0; i < nc; i++)
    pal_use[i] = -1;
    
  for (i = 0; i < iw * ih; i++) {
    data[i] += scb_use[scbs[i / iw] & 0xf] * 16;
    pal_use[data[i]] = 1;
  }

  if ((j = reduce_pals(pal_use, palette, nc)) < nc) {
    nc = j;
    for (i = 0; i < iw * ih; i++)
      data[i] = pal_use[data[i]];
  }

  free(gsimage);

  constructImage(aPic, COLORMASK | PALETTEMASK, PLANAR, iw, ih, 8, data, palette);
  free(data);
  free(palette);

  return PICERR_NOERR;
}

int info_gsbin(char *filename, imageInfo *info, formatInfo *fInfo)
{
  if (!filename) {		/* Just want longname and extension */
    fInfo->type = "Apple IIGS Super Hi-Res Graphics Screen Image";
    fInfo->extension = "gsbin";    
    fInfo->maxWidth = 640;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = 4; /* Sorta. 4 per scan line. */
    fInfo->maxBitsPerColor = 4;
    fInfo->colorSpace = COLORMASK | PALETTEMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }

  info->type = "Apple IIGS Super Hi-Res Graphics Screen Image";
  info->extension = "gsbin";
  
  return PICERR_NOERR;
}

static inline int pplus_make_line(int scb, int width, int mastermode, unsigned char *from_line, unsigned char *to_line)
{
  int i, lw, bpp, ppb;
  int j, k;


  lw = width * (1 + (mastermode == 0x80));
  bpp = (4 * width) / lw;
  ppb = 8 / bpp;
  j = ppb - 1;
  k = (1 << bpp) - 1;

  for (i = 0; i < lw; i++) {
    to_line[i] = (from_line[i / ppb] >> ((j - i % ppb) * bpp)) & k;
    if (mastermode == 0x80)
      to_line[i] += (i % ppb) * 4;
  }

  if (scb & 0x20)
    for (i = 0; i < lw; i++) {
      if (!to_line[i])
	to_line[i] = ppb;
      else
	ppb = to_line[i];
    }

  return lw;
}

static inline int pplus_unpack(FILE *in, int *wh, int *ht, byte **d, byte *palette)
{
  int mastermode, width, height;
  int i, l, m, n;
  int b[4];
  int type, val;
  byte *data;
  

  width = 320;
  height = 396;
  
  for (i = 0; i < 32; i++)
    palette[i] = fgetc(in);

  for (i = 0; i < 514; i++)
    fgetc(in);

  mastermode = 0;
  
  *d = data = (unsigned char *) malloc(height * 160);
  m = 0;
  while ((l = fgetc(in)) != EOF) {
    type = (l >> 6) & 0x3;
    val = l & 0x3f;
    switch (type) {
    case 0:
      for (l = 0; l <= val; l++, m++)
	data[m] = fgetc(in);
      break;
      
    case 1:
      n = fgetc(in);
      for (l = 0; l <= val; l++, m++)
	data[m] = n;
      break;
      
    case 2:
      for (l = 0; l < 4; l++)
	b[l] = fgetc(in);
      for (l = 0; l <= val; l++)
	for (n = 0; n < 4; n++, m++)
	  data[m] = b[n];
      break;
      
    case 3:
      b[0] = fgetc(in);
      for (l = 0; l <= val; l++)
	for (n = 0; n < 4; n++, m++)
	  data[m] = b[0];
      break;
    }
  }

  *wh = width;
  *ht = height;
  
  return mastermode;
}

int load_pplus(char *filename, image *aPic)
{
  FILE *pplusfile;
  unsigned char *p;
  int i;
  int iw, ih;
  int mastermode;
  byte *data, *palette;
  

  if (!(pplusfile = fopen(filename, "r")))
    return PICERR_CANTOPEN;
  
  if (!(palette = malloc(64)))
    return PICERR_CANTALLOC;
  
  mastermode = pplus_unpack(pplusfile, &iw, &ih, &data, palette);
  if (mastermode < 0)
    return -mastermode;
  
  fclose(pplusfile);

  /* Convert palette to full bytes */

  p = palette;
  if ((palette = malloc(96)) == NULL)
    return PICERR_CANTALLOC;

  for (i = 0; i < 16; i++) {
    palette[i * 3] = ((p[i * 2 + 1] & 0xf) * 255) / 15;
    palette[i * 3 + 1] = (((p[i * 2] >> 4) & 0xf) * 255) / 15;
    palette[i * 3 + 2] = ((p[i * 2] & 0xf) * 255) / 15;
  }

  free(p);

  constructImage(aPic, COLORMASK | PALETTEMASK, PLANAR, iw, ih, 4, data, palette);
  free(data);
  free(palette);
  
  return PICERR_NOERR;
}

int info_pplus(char *filename, imageInfo *info, formatInfo *fInfo)
{
  if (!filename) {
    fInfo->maxWidth = 320;
    fInfo->maxHeight = 396;
    fInfo->maxBitsPerPixel = 4; /* Sorta. 4 per scan line. */
    fInfo->maxBitsPerColor = 4;
    fInfo->colorSpace = COLORMASK | PALETTEMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }
  
  info->type = "Paintworks Packed Super Hi-Res Picture File";
  info->extension = "pwf";
  info->photoInterp = COLORMASK | PALETTEMASK;
  info->width = 320;
  info->height = 396;
  info->bitsPerColor = 12;
  info->numColors = 16;

  return PICERR_NOERR;
}
