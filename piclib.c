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
#include <math.h>
#include <string.h>
#include <stdlib.h>

static inline int readval(int x, int y, int plane, image *aPic)
{
  if (x >= 0 && y >= 0 && x < aPic->width && y < aPic->height) {
    if ((aPic->photoInterp & COLORMASK) && plane == MONO) 
      return(getval(x, y, RED, aPic) * 77 +
	     getval(x, y, GREEN, aPic) * 150 +
	     getval(x, y, BLUE, aPic) * 29) / 256;
    else
      return getval(x, y, plane, aPic);
  }

  return 0;
}

int ditherImage(image *aPic, int *mapping, int bits)
{
  image newPic;
  int col, first, last;
  int i, j, k, n;
  int scl = 255 / ((1 << bits) - 1);
  int *ditherTable;
  int err;
  int dir = 1;
  
  void exit();
  void *malloc();
  void free();


  allocateImage(&newPic, aPic->photoInterp & (MONOMASK | COLORMASK | ALPHAMASK),
		aPic->width, aPic->height);
  
  n = aPic->width + 2;
  if (!(ditherTable = (int *) malloc(n * 2 * sizeof(int))))
    return PICERR_CANTALLOC;

  first = 0;
  last = aPic->samplesPerPixel - 1;

  for (col = first; col <= last; col++) {
    for (i = 0; i < aPic->width; i++) {
      if (mapping)
	ditherTable[i + 1 + n] = mapping[getval(i, 1, col, aPic)];
      else
	ditherTable[i + 1 + n] = getval(i, 1, col, aPic);
    }
    
    for (j = 0; j < aPic->height; j++, dir = -dir) {
      for (i = 0; i < aPic->width; i++) {
	ditherTable[i + 1] = ditherTable[i + 1 + n];
	if (mapping)
	  ditherTable[i + 1 + n] = mapping[getval(i, j + 1, col, aPic)];
	else
	  ditherTable[i + 1 + n] = getval(i, j + 1, col, aPic);
      }
      for (i = dir < 0 ? aPic->width : 1;
	   dir < 0 ? i > 0 : i <= aPic->width; i += dir) {
	k = ditherTable[i];
	ditherTable[i] = ((ditherTable[i] + (scl / 2)) / scl) * scl;
	err = k - ditherTable[i];
	if (err) {
	  ditherTable[i + dir] = MIN(ditherTable[i + dir]+(err*7)/16, 255);
	  ditherTable[i-dir+n] = MIN(ditherTable[i-dir+n]+(err*5)/16, 255);
	  ditherTable[i  +  n] = MIN(ditherTable[i  +  n]+(err*1)/16, 255);
	  ditherTable[i+dir+n] = MIN(ditherTable[i+dir+n]+(err*3)/16, 255);
	}
      }
      for (i = 0; i < aPic->width; i++)
	putval(i, j, ditherTable[i + 1], col, &newPic);
    }
  }

  free(ditherTable);
  freeImage(aPic);
  copyImage(aPic, &newPic);
  freeImage(&newPic);

  return 0;
}

int thresholdImage(image *aPic, int *mapping, int bits)
{
  image newPic;
  int len;
  int i;
  int scl = 255 / ((1 << bits) - 1);
  byte *dt, *dt2;
  
  void exit();
  void *malloc();
  void free();


  allocateImage(&newPic, aPic->photoInterp & (MONOMASK | COLORMASK | ALPHAMASK)
		, aPic->width, aPic->height);
  
  len = aPic->width * aPic->height * aPic->samplesPerPixel;

  for (i = 0, dt = newPic.data, dt2 = aPic->data; i < len; i++, dt++, dt2++)
    *dt = (((mapping ? mapping[*dt2] : *dt2) + (scl / 2)) / scl) * scl;
  
  freeImage(aPic);
  copyImage(aPic, &newPic);
  freeImage(&newPic);

  return 0;
}

int halftoneImage(image *aPic, int *mapping, int bits)
{
  image newPic;
  int col, first, last;
  int i, j, t;
  int scl = 255 / ((1 << bits) - 1);
  int *ditherTable;
  int thresholds[] = {
    6, 195,  54, 243,  18, 207,  66, 255,
    132,  69, 180, 117, 144,  81, 192, 129,
    36, 228,  21, 213,  48, 240,  33, 225,
    165, 102, 147,  84, 177, 114, 159,  96,
    12, 204,  60, 252,   9, 201,  57, 249,
    141,  78, 189, 126, 135,  72, 183, 120,
    45, 237,  30, 219,  42, 231,  24, 216,
    171, 108, 156,  93, 168, 105, 153,  90,
  };

  void *malloc();
  void free();


  allocateImage(&newPic, aPic->photoInterp & (MONOMASK | COLORMASK | ALPHAMASK),
		aPic->width, aPic->height);

  if (!(ditherTable = (int *) malloc(aPic->width * sizeof(int))))
    return PICERR_CANTALLOC;

  first = 0;
  last = aPic->samplesPerPixel - 1;

  for (col = first; col <= last; col++)
    for (j = 0; j < aPic->height; j++) {
      for (i = 0; i < aPic->width; i++)
	if (mapping)
	  ditherTable[i] = mapping[getval(i, j, col, aPic)];
	else
	  ditherTable[i] = getval(i, j, col, aPic);
      
      for (i = 0; i < aPic->width; i++) {
	t = (thresholds[i % 8 + (j % 8) * 8] / (255 / scl)) * ((ditherTable[i] - 1) / scl + 1);
	ditherTable[i] = ((ditherTable[i] + ((t - 1) / scl + 1) * scl - t) / scl) * scl;
	ditherTable[i] = MIN(ditherTable[i], 255);
	ditherTable[i] = MAX(ditherTable[i], 0);
      }
      
      for (i = 0; i < aPic->width; i++)
	putval(i, j, ditherTable[i], col, &newPic);
    }


  free(ditherTable);
  freeImage(aPic);
  copyImage(aPic, &newPic);
  freeImage(&newPic);

  return 0;
}

void negateImage(image *aPic)
{
  int i, len;
  byte *dt;


  len = aPic->width * aPic->height * (aPic->photoInterp & COLORMASK ? 3 : 1);

  for (i = 0, dt = aPic->data; i < len; i++, dt++)
    *dt = MV - *dt;

  return;
}

void scaleImage(double xpercent, double ypercent, int blend, image *aPic)
{
  if (xpercent == 1.0 && ypercent == 1.0)
    return;

  resizeImage(aPic->width * xpercent, aPic->height * ypercent, blend, aPic);
  return;
}

void resizeImage(unsigned int width, unsigned int height, int blend, image *aPic)
{
  image newPic;
  int col;
  int i, j, k, l;
  int x, y;
  double xpercent, ypercent;


  allocateImage(&newPic, aPic->photoInterp, width, height);
  xpercent = (double) newPic.width / aPic->width;
  ypercent = (double) newPic.height / aPic->height;

  for (col = 0; col < aPic->samplesPerPixel; col++)
    for (i = 0; i < newPic.width; i++)
      for (j = 0; j < newPic.height; j++) {
	if (blend) {
	  for (l = k = 0, x = i / xpercent; x < (i + 1) / xpercent; x++)
	    for (y = j / ypercent; y < (j + 1) / ypercent; y++, l++)
	      k += getval(x, y, col, aPic);
	  k /= l;
	  putval(i, j, k, col, &newPic);
	}
	else
	  putval(i, j, getval(i / xpercent, j / ypercent, col, aPic), col, &newPic);
      }

  freeImage(aPic);
  copyImage(aPic, &newPic);
  freeImage(&newPic);

  return;
}

void rotateImage(image *aPic, double radians)
{
  double ndeg = acos(0);
  double nh, nw;
  double u, r, l, d;
  int i, j, k;
  image newPic;
  double u2, r2, l2, d2;
  double ox, oy;
  int first, last;


  if (!radians)
    return;
  
  nh = fabs(d = aPic->width * sin(radians)) + fabs(u = aPic->height * sin(radians + ndeg));
  nw = fabs(r = aPic->width * cos(radians)) + fabs(l = aPic->height * cos(radians + ndeg));

  ox = oy = 0;
  if (r < 0)
    ox -= r;
  if (l < 0)
    ox -= l;
  if (d < 0)
    oy -= d;
  if (u < 0)
    oy -= u;
  
  allocateImage(&newPic, aPic->photoInterp, nw, nh);

  first = 0;
  last = aPic->samplesPerPixel - 1;

  for (i = 0; i < newPic.width; i++) {
    for (j = 0; j < newPic.height; j++) {
      d2 = (i - ox) * sin(-radians);
      u2 = ((newPic.height - j) - oy) * sin(-radians + ndeg);
      r2 = (i - ox) * cos(-radians);
      l2 = ((newPic.height - j) - oy) * cos(-radians + ndeg);
      for (k = first; k <= last; k++) {
	putval(i, j, getval(l2 + r2, aPic->height - (d2 + u2), k, aPic), k, &newPic);
      }
    }
  }
  
  freeImage(aPic);
  copyImage(aPic, &newPic);
  freeImage(&newPic);
  
  return;
}

void makeMono(image *aPic)
{
  int i;
  image newPic;
  int len;
  byte *dt, *dt2;
  int alpha = 0, ns;
  

  if (aPic->photoInterp == MONOMASK)
    return;

  allocateImage(&newPic, MONOMASK | (aPic->photoInterp & ALPHAMASK),
		aPic->width, aPic->height);

  ns = 3;
  if (aPic->photoInterp & ALPHAMASK) {
    ns++;
    alpha = 1;
  }
  
  len = aPic->width * aPic->height * ns;
  for (i = 0, dt = newPic.data, dt2 = aPic->data; i < len;
       i += ns, dt += 1 + alpha, dt2 += ns) {
    *dt = (*(dt2 + 0) * 77 + *(dt2 + 1) * 150 + *(dt2 + 2) * 29) / 256;
    if (alpha)
      *(dt+1) = *(dt + ns);
  }

  freeImage(aPic);
  copyImage(aPic, &newPic);
  freeImage(&newPic);

  return;
}

void adjustImage(image *aPic, double contrast, int brightness)
{
  int i;
  int len;
  byte *dt;
  

  if (!contrast && !brightness)
    return;
  
  contrast += 1.0; /* 0 level should do nothing */
  
  len = aPic->width * aPic->height * (aPic->photoInterp & COLORMASK ? 3 : 1);

  for (i = 0, dt = aPic->data; i < len; i++, dt++)
    *dt = CLIP((*dt - 127) * contrast + 127 + brightness, 0, 255);
  
  return;
}

void enhanceImage(image *aPic, int contrast)
{
  int i, j, k, l;


  for (i = 0; i < aPic->width; i++)
    for (j = 0; j < aPic->height; j++)
      for (k = -1; k < 2; k++)
	for (l = -1; l < 2; l++)
	  if ((k + i >= 0 && k + i < aPic->width && l + j >= 0 && l + j < aPic->height) && !(i + k == i && j + l == j))
	    if (abs(getval(i + k, j + l, MONO, aPic) - getval(i, j, MONO, aPic)) < contrast) {
	      putval(i, j, (getval(i + k, j + l, MONO, aPic) + getval(i, j, MONO, aPic)) / 2, MONO, aPic);
//	      putval(i + k, j + l, (getval(i + k, j + l, MONO, aPic) + getval(i, j, MONO, aPic)) / 2, MONO, aPic);
	    }
  
  return;
}

void cropImage(int x1, int y1, int x2, int y2, image *aPic)
{
  image newPic;
  int i, j, k;


  if (x1 == 0 && y1 == 0 && x2 >= aPic->width - 1 && y2 >= aPic->height - 1)
    return;

  allocateImage(&newPic, aPic->photoInterp, x2 - x1 + 1, y2 - y1 + 1);

  i = (x2 - x1 + 1) * aPic->samplesPerPixel;
  j = aPic->width * aPic->samplesPerPixel;

  for (k = y1; k <= y2; k++)
    memcpy(&(newPic.data[(k - y1) * i]),
	   &(aPic->data[k * j + x1 * aPic->samplesPerPixel]), i);

  freeImage(aPic);
  copyImage(aPic, &newPic);
  freeImage(&newPic);
  
  return;
}

void autoCropImage(image *aPic, int margin)
{
  int i, j, k;
  int left, right, top, bottom;
  int l2 = 0, r2 = 0, t2 = 0, b2 = 0;
  int c1, c2;
  int plane = MONO;
  

  left = top = 0;
  right = aPic->width - 1;
  bottom = aPic->height - 1;

  if (aPic->photoInterp & ALPHAMASK)
    plane = ALPHA;
  
  while ((l2 != left || r2 != right || t2 != top || b2 != bottom) &&
	 (left < right && top < bottom)) {
    l2 = left;
    t2 = top;
    r2 = right;
    b2 = bottom;

    c1 = readval(l2, t2, plane, aPic);
    c2 = readval(r2, b2, plane, aPic);
    
    for (i = l2, left = r2 + 1, top = b2 + 1; i < r2 + 1; i++)
      for (j = t2; j < top; j++) {
	k = readval(i, j, plane, aPic);
	if (k - margin > c1 || k + margin < c1) {
	  if (i < left)
	    left = i;
	  if (j < top)
	    top = j;
	}
      }
    for (i = r2, right = l2, bottom = t2; i >= l2; i--)
      for (j = b2; j >= bottom; j--) {
	k = readval(i, j, plane, aPic);
	if (k - margin > c2 || k + margin < c2) {
	  if (i > right)
	    right = i;
	  if (j > bottom)
	    bottom = j;
	}
      }
  }

  if (left < right && top < bottom)
    cropImage(left, top, right, bottom, aPic);

  return;
}

char *picErrorStr(int err)
{
  int i;
  char *errors[] = {
    "File is damaged",
    "No image found",
    "Cannot allocate memory",
    "Unsupported format",
    "IO Error",
    "Unsupported compression",
    "Can't append",
    "Cannot open file",
    NULL,
  };


  for (i = 0; errors[i]; i++);
  if (err - 1 < i)
    return errors[err - 1];

  return "Unknown Error";
}

void allocateImage(image *aPic, int photoInterp, int width, int height)
{
  int samples;

  
  if (photoInterp & COLORMASK)
    samples = 3;
  else
    samples = 1;
  if (photoInterp & ALPHAMASK)
    samples++;
  
  aPic->photoInterp = photoInterp;
  aPic->width = width;
  aPic->height = height;
  aPic->samplesPerPixel = samples;
  aPic->data = calloc(height * width, samples);

  return;
}

int copyImage(image *dest, image *source)
{
  int i;
  
  void *malloc();


  allocateImage(dest, source->photoInterp, source->width, source->height);
  dest->photoInterp = source->photoInterp;
  dest->width = source->width;
  dest->height = source->height;
  dest->samplesPerPixel = source->samplesPerPixel;

  i = source->width * source->height * source->samplesPerPixel;
  memcpy(dest->data, source->data, i);

  return PICERR_NOERR;
}
  
void freeImage(image *aPic)
{
  void free();


  if (aPic->data) 
    free(aPic->data);
  aPic->data = NULL;

  return;
}

/* Code for converting various image formats into image record */
typedef struct pictureRecord {
  int photoInterp;
  int planarConfig;
  int width, height;
  int bpp;
  int ppb, bpl;
  int mv;
  byte *data;
  byte *palette;
} picRec;

static inline int get_val(int x, int y, int col, picRec *aPic)
{
  int l, pv = 0;
  int i, cm;


  cm = 1;
  if (aPic->photoInterp & COLORMASK)
    cm = 3;
  if (aPic->photoInterp & ALPHAMASK)
    cm++;

  if (col == MONO)
    col = 0;
  i = ((aPic->ppb - 1) - (x * cm + col) % aPic->ppb) * aPic->bpp;
  
  if (aPic->photoInterp & PALETTEMASK) {
    l = aPic->data[x / aPic->ppb + y * aPic->bpl];
    pv = (l >> i) & aPic->mv;
    return aPic->palette[pv * cm + col];
  }
  if (aPic->planarConfig == MESHED) {
    l = aPic->data[(x * cm + col) / aPic->ppb + y * aPic->bpl];
    return (l >> i) & aPic->mv;
  }
  else {
    l = aPic->data[x / aPic->ppb + (y + aPic->height * col) * aPic->bpl];
    return (l >> i) & aPic->mv;
  }

  return pv;
}

int constructImage(image *aPic, int photoInterp, int planarConfig, int width, int height,
		   int bpp, byte *data, byte *palette)
{
  int i, j, k, l;
  int first, last;
  picRec newpic;
  byte *dt;
  int samples = 1;
  

  if (planarConfig == MESHED) {
    if (photoInterp & COLORMASK && !(photoInterp & PALETTEMASK))
      samples = 3;
    if (photoInterp & ALPHAMASK)
      samples++;
  }
  
  newpic.photoInterp = photoInterp;
  newpic.planarConfig = planarConfig;
  newpic.width = width;
  newpic.height = height;
  newpic.bpp = bpp;
  newpic.ppb = 8 / bpp;
  newpic.bpl = (width * samples * bpp + 7) / 8;
  newpic.mv = (1 << bpp) - 1;
  newpic.data = data;
  newpic.palette = palette;
  
  allocateImage(aPic, photoInterp & (COLORMASK | MONOMASK | ALPHAMASK), width, height);

  if (bpp == 8 && ((photoInterp == COLORMASK && planarConfig == MESHED) ||
		   photoInterp == MONOMASK)) {
    i = width * height * (photoInterp & COLORMASK ? 3 : 1);
    memcpy(aPic->data, data, i);
    return PICERR_NOERR;
  }

  if (bpp == 8 && (photoInterp & PALETTEMASK)) {
    for (i = 0, j = width * height, dt = aPic->data; i < j; i++, dt +=3, data++) {
      *(dt + 0) = palette[*data * 3 + 0];
      *(dt + 1) = palette[*data * 3 + 1];
      *(dt + 2) = palette[*data * 3 + 2];
    }
    return PICERR_NOERR;
  }

  first = 0;
  last = aPic->samplesPerPixel - 1;

  if (photoInterp & PALETTEMASK)
    bpp = 8;
  l = 255 / ((1 << bpp) - 1);
  
  for (i = 0; i < aPic->height; i++)
    for (j = 0; j < aPic->width; j++) 
      for (k = first; k <= last; k++)
	putval(j, i, get_val(j, i, k, &newpic) * l, k, aPic);

  return PICERR_NOERR;
}

/* stuff for unconverting from image type */
static inline void put_val(int x, int y, int val, int col, picRec *aPic)
{
  int l, i, cm;


  cm = 1;
  if (aPic->photoInterp & COLORMASK)
    cm = 3;
  if (aPic->photoInterp & ALPHAMASK)
    cm++;

  if (col == MONO)
    col = 0;
  i = ((aPic->ppb - 1) - (x * cm + col) % aPic->ppb) * aPic->bpp;
  val = (val & aPic->mv) << i;
  
  if (aPic->planarConfig == MESHED) {
    l = aPic->data[(x * cm + col) / aPic->ppb + y * aPic->bpl];
    l &= ~(aPic->mv << i);
    l |= val;
    aPic->data[(x * cm + col) / aPic->ppb + y * aPic->bpl] = l;
  }
  else {
    l = aPic->data[x / aPic->ppb + (y + aPic->height * col) * aPic->bpl];
    l &= ~(aPic->mv << i);
    l |= val;
    aPic->data[x / aPic->ppb + (y + aPic->height * col) * aPic->bpl] = l;
  }
  
  return;
}

int putColorInPalette(int r, int g, int b, byte *palette, int *nv)
{
  int i;


  for (i = 0; i < (*nv); i++)
    if (palette[i * 3 + 0] == r && palette[i * 3 + 1] == g && palette[i * 3 + 2] == b)
      return i;

  if ((*nv) < 256) {
    palette[(*nv) * 3 + 0] = r;
    palette[(*nv) * 3 + 1] = g;
    palette[(*nv) * 3 + 2] = b;
    (*nv)++;
    return(*nv) - 1;
  }

  return -1;
}

int unconstructImage(image *aPic, int photoInterp, int planarConfig, int minbpp,
		     int *bpp, byte **data, byte **palette)
{
  int i, j, k, l;
  int first, last;
  int bits, scl;
  picRec newpic;
  int samples = 1;

  void *malloc();
  

  if (planarConfig == MESHED) {
    if (photoInterp & COLORMASK && !(photoInterp & PALETTEMASK))
      samples = 3;
    if (photoInterp & ALPHAMASK)
      samples++;
  }
  
  if (photoInterp & PALETTEMASK) {
    newpic.photoInterp = MONOMASK;
    newpic.planarConfig = PLANAR;
    newpic.width = aPic->width;
    newpic.height = aPic->height;
    newpic.bpp = 8;
    newpic.ppb = 8 / newpic.bpp;
    newpic.bpl = (newpic.width * samples * newpic.bpp + 7) / 8;
    newpic.mv = (1 << newpic.bpp) - 1;
  
    newpic.data = malloc(aPic->width * aPic->height);
    newpic.palette = malloc(256 * 3);

    bits = 0;
    if (aPic->photoInterp & MONOMASK) {
      for (i = 0; i < aPic->width; i++)
	for (j = 0; j < aPic->height; j++) {
	  k = getval(i, j, MONO, aPic);
	  if ((l = putColorInPalette(k, k, k, newpic.palette, &bits)) < 0)
	    return PICERR_COMPRESSIONNOTSUPPORTED; /* Too many colors */
	  put_val(i, j, l, MONO, &newpic);
	}
    }
    else {
      for (i = 0; i < aPic->width; i++)
	for (j = 0; j < aPic->height; j++) {
	  if ((l = putColorInPalette(getval(i, j, RED, aPic), getval(i, j, GREEN, aPic), getval(i, j, BLUE, aPic), newpic.palette, &bits)) < 0)
	    return PICERR_COMPRESSIONNOTSUPPORTED; /* Too many colors */
	  put_val(i, j, l, MONO, &newpic);
	}
    }

    *bpp = 1;
    while (bits > (1 << (*bpp)))
      *bpp += 1;
    if (minbpp && *bpp < minbpp)
      *bpp = minbpp;
  }
  else {
    first = 0;
    last = aPic->samplesPerPixel - 1;

    bits = 1;
    scl = 255 / ((1 << bits) - 1);
    for (i = 0; i < aPic->width; i++)
      for (j = 0; j < aPic->height; j++)
	for (k = first; k <= last; k++) {
	  l = getval(i, j, k, aPic);
	  while (l != (l / scl) * scl) {
	    bits++;
	    scl = 255 / ((1 << bits) - 1);
	  }
	}
  
    newpic.photoInterp = photoInterp;
    newpic.planarConfig = planarConfig;
    newpic.width = aPic->width;
    newpic.height = aPic->height;
    if (minbpp && bits < minbpp)
      bits = minbpp;
    *bpp = newpic.bpp = bits;
    newpic.ppb = 8 / newpic.bpp;
    newpic.bpl = (newpic.width * samples * newpic.bpp + 7) / 8;
    newpic.mv = (1 << newpic.bpp) - 1;
  
    newpic.palette = NULL;

    newpic.data = malloc(aPic->height * newpic.bpl);
  
    if (newpic.bpp == 8 && ((photoInterp == COLORMASK && planarConfig == MESHED) ||
		      photoInterp == MONOMASK)) {
      i = aPic->width * aPic->height * (photoInterp & COLORMASK ? 3 : 1);
      memcpy(newpic.data, aPic->data, i);
    }
    else {
      l = 255 / ((1 << newpic.bpp) - 1);
      
      for (i = 0; i < aPic->height; i++)
	for (j = 0; j < aPic->width; j++) 
	  for (k = first; k <= last; k++)
	    put_val(j, i, getval(j, i, k, aPic) / l, k, &newpic);
    }
  }

  if (data)
    *data = newpic.data;
  if (palette)
    *palette = newpic.palette;
  
  return PICERR_NOERR;
}  
