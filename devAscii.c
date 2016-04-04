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
#include <sys/ioctl.h>
#include <stdlib.h>
#ifndef linux
#include <libc.h>
#endif

#include "piclib.h"
#include "picloader.h"
#include "devAscii.h"

#ifdef LOWRES
#define CHAR_WIDTH 2
#define CHAR_HEIGHT 2

static int cdens[256][CHAR_WIDTH][CHAR_HEIGHT] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x12, 0x1c, 0x00, 0x12,
  0x00, 0x47, 0x39, 0x39, 0x2f, 0x39, 0x1c, 0x4b, 0x41, 0x39, 0x2a,
  0x39, 0x1c, 0x47, 0x2a, 0x1c, 0x25, 0x0e, 0x00, 0x12, 0x00, 0x1c,
  0x1c, 0x1c, 0x1c, 0x0e, 0x0e, 0x25, 0x25, 0x39, 0x00, 0x55, 0x08,
  0x1c, 0x00, 0x2f, 0x12, 0x00, 0x00, 0x00, 0x12, 0x1c, 0x00, 0x1c,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x1c, 0x2f, 0x08, 0x55, 0x2a,
  0x4b, 0x25, 0x1c, 0x1c, 0x2f, 0x2f, 0x1c, 0x39, 0x39, 0x1c, 0x1c,
  0x1c, 0x41, 0x25, 0x39, 0x00, 0x4b, 0x1c, 0x64, 0x1c, 0x39, 0x25,
  0x47, 0x2a, 0x2f, 0x25, 0x2a, 0x00, 0x41, 0x1c, 0x47, 0x2a, 0x41,
  0x25, 0x47, 0x0e, 0x4b, 0x1c, 0x00, 0x00, 0x08, 0x08, 0x00, 0x0e,
  0x08, 0x08, 0x1c, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c,
  0x1c, 0x1c, 0x08, 0x2a, 0x00, 0x39, 0x12, 0x39, 0x2a, 0x55, 0x41,
  0x1c, 0x39, 0x2f, 0x2f, 0x64, 0x39, 0x41, 0x25, 0x47, 0x2a, 0x1c,
  0x1c, 0x55, 0x39, 0x2f, 0x1c, 0x64, 0x39, 0x2f, 0x1c, 0x64, 0x2a,
  0x2f, 0x00, 0x47, 0x2a, 0x2f, 0x25, 0x55, 0x2a, 0x41, 0x1c, 0x0e,
  0x0e, 0x39, 0x25, 0x00, 0x2a, 0x2f, 0x25, 0x64, 0x2a, 0x1c, 0x1c,
  0x47, 0x39, 0x00, 0x1c, 0x64, 0x2a, 0x4b, 0x1c, 0x64, 0x2a, 0x41,
  0x2f, 0x47, 0x2a, 0x39, 0x25, 0x64, 0x2a, 0x41, 0x00, 0x47, 0x2a,
  0x39, 0x39, 0x64, 0x2a, 0x41, 0x1c, 0x39, 0x1c, 0x39, 0x25, 0x1c,
  0x00, 0x41, 0x1c, 0x47, 0x2a, 0x2f, 0x25, 0x47, 0x0e, 0x2f, 0x1c,
  0x47, 0x2a, 0x4b, 0x1c, 0x2a, 0x2a, 0x2f, 0x1c, 0x39, 0x00, 0x2f,
  0x1c, 0x1c, 0x39, 0x41, 0x1c, 0x47, 0x47, 0x12, 0x12, 0x1c, 0x00,
  0x1c, 0x1c, 0x0e, 0x0e, 0x39, 0x39, 0x1c, 0x00, 0x1c, 0x00, 0x00,
  0x1c, 0x00, 0x1c, 0x00, 0x00, 0x12, 0x00, 0x2a, 0x2a, 0x39, 0x2f,
  0x55, 0x39, 0x25, 0x25, 0x2a, 0x2a, 0x1c, 0x1c, 0x2a, 0x2a, 0x41,
  0x2f, 0x39, 0x2a, 0x39, 0x1c, 0x0e, 0x00, 0x4b, 0x1c, 0x2a, 0x39,
  0x2f, 0x4b, 0x55, 0x2a, 0x25, 0x1c, 0x1c, 0x1c, 0x25, 0x2f, 0x00,
  0x0e, 0x39, 0x39, 0x55, 0x2a, 0x12, 0x1c, 0x0e, 0x1c, 0x2f, 0x2f,
  0x39, 0x2a, 0x2f, 0x39, 0x39, 0x2a, 0x25, 0x1c, 0x2a, 0x2a, 0x25,
  0x25, 0x39, 0x55, 0x25, 0x25, 0x2a, 0x2a, 0x2f, 0x41, 0x39, 0x39,
  0x1c, 0x08, 0x2a, 0x1c, 0x2f, 0x25, 0x55, 0x1c, 0x12, 0x1c, 0x2a,
  0x2a, 0x1c, 0x2f, 0x2a, 0x0e, 0x1c, 0x1c, 0x2a, 0x2a, 0x25, 0x25,
  0x1c, 0x1c, 0x1c, 0x1c, 0x2a, 0x2a, 0x1c, 0x1c, 0x1c, 0x39, 0x2f,
  0x1c, 0x0e, 0x00, 0x25, 0x2f, 0x00, 0x00, 0x2f, 0x2f, 0x0e, 0x0e,
  0x25, 0x25, 0x1c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static int bitmap[40][100];
static int bw = 5, bh = 10;
static int cw = 6, ch = 13;
static int lc = 96;
static unsigned char *dithered_img;

static inline int closest_match(int x, int y, int columns)
{
  int i, j, d, e, f;
  int k, l = 0;
  int abs();


  for (d = i = 0; i < CHAR_WIDTH; i++)
    for (j = 0; j < CHAR_HEIGHT; j++)
      d += dithered_img[(y * CHAR_HEIGHT + j) * columns * CHAR_WIDTH + (x * CHAR_WIDTH + i)];

  e = 99999;
  for (k = 0; k < lc; k++) {
    for (f = 0, i = 0; i < CHAR_WIDTH; i++)
      for (j = 0; j < CHAR_HEIGHT; j++)
	f += cdens[k][i][j];

    if (abs(d - f) < abs(d - e)) {
      e = f;
      l = k;
    }
  }

  return l + 32;
}

static inline void get_char_size(int *xo, int *yo, int *wi, int *he)
{
  int i, j;
  int l, t, w, h;


  l = bw; t = bh;
  w = -1; h = -1;
  
  for (i = 0; i < bw; i++)
    for (j = 0; j < bh; j++)
      if (!bitmap[i][j]) {
	if (i < l)
	  l = i;
	if (i > w)
	  w = i;
	if (j < t)
	  t = j;
	if (j > h)
	  h = j;
      }

  *xo = l; *yo = t;
  *wi = (w - l) + 1; *he = (h - t) + 1;

  return;
}

static inline void get_cdens(char *file, int lastChar)
{
  image charTiff;
  int i, j;
  int i2, j2;
  int c, d, ld = 0;
  float bw3, bh3;
  int x, y, xs, ys;
  int w, h;
  int xos, yos, aw, ah;
  int p, q, r, s;
  int hd = 0;
  int rows = 3, columns = 32;
  double offby;
  int err;

  void exit();


  if (err = load_pic(file, &charTiff)) {
    fprintf(stderr, "Cannot load file %s\nError: %s\n", file, picErrorStr(err));
    exit(1);
  }

  lc = lastChar - 31;
  rows = lc / 32 + (lc % 32 > 0);
  
  xs = charTiff.width / columns;
  ys = charTiff.height / rows;

  cw = (int) xs;
  ch = (int) ys;

  xos = xs; yos = ys;
  aw = -1; ah = -1;
  bw = xs; bh = ys;
  for (y = 0; y < charTiff.height; y += ys)
    for (x = 0; x < charTiff.width; x += xs) {
      for (i = 0; i < bh; i++)
	for (j = 0; j < bw; j++) {
	  bitmap[j][i] = getval(x + j, y + i, MONO, &charTiff);
	}
      get_char_size(&p, &q, &r, &s);
      if (p < xos)
	xos = p;
      if (q < yos)
	yos = q;
      if (r > aw)
	aw = r;
      if (s > ah)
	ah = s;
    }

  c = 0;
  bw = aw; bh = ah;
  bw3 = (float) bw / (float) CHAR_WIDTH;
  bh3 = (float) bh / (float) CHAR_HEIGHT;
  for (y = yos; y < charTiff.height; y += ys) {
    for (x = xos; x < charTiff.width; x += xs) {
      for (i = 0; i < bh; i++) {
	for (j = 0; j < bw; j++) {
	  bitmap[j][i] = getval(x + j, y + i, MONO, &charTiff);
	}
      }
      for (i = CHAR_WIDTH - 1; i >= 0; i--) {
	for (j = CHAR_HEIGHT - 1; j >= 0; j--) {
	  d = 0;
	  for (i2 = (int) (bw3 * i), w = 0, h = 0; i2 < (int) (bw3 * (i + 1)); i2++, w++)
	    for (j2 = (int) (bh3 * j), h = 0; j2 < (int) (bh3 * (j + 1)); j2++, h++)
	      d += MV - bitmap[i2][j2];

	  cdens[c][i][j] = ld;
	  if (h * w)
	    ld = cdens[c][i][j] = d / (h * w);

	  if (ld > hd)
	    hd = ld;
	}
      }
      c++;
    }
  }

  offby = (double) MV / (double) hd;

  for (c = 0; c < lc; c++)
    for (i = 0; i < CHAR_WIDTH; i++)
      for (j = 0; j < CHAR_HEIGHT; j++)
	cdens[c][i][j] = cdens[c][i][j] * offby;

  return;
}

static inline void ascii_dither(image *aPic, int columns, int rows)
{
  double x, y, xs, ys;
  int ccol, crow;
  int i, j;
  int d, w, h;
  int ld = 0;
  static int remap[256] = {
    94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94, 
    94,  94,  94,  94,  94,  93,  92,  91,  90,  90,  90,  89,  89,  89, 100, 
    100,  88,  88,  88, 101, 101,  87,  87,  87,  86,  86,  86,  86,  85,  85, 
    85, 103, 103,  84,  84,  84, 104, 104,  83,  83,  83, 105, 105, 105,  82, 
    82,  82,  82, 106, 106, 106, 129, 129,  81, 107, 107, 132, 127, 127, 108, 
    108, 108, 126,  80,  80,  80, 134, 134, 109, 125, 125, 125, 135, 135, 135, 
    124, 110, 110, 110, 123, 123, 123, 111, 111, 111, 111, 122, 122, 112, 112, 
    112, 113, 113, 113, 138, 138, 114, 114, 114, 114, 120, 139, 139, 119, 119, 
    139, 140, 140, 140, 140, 140, 140, 140, 141, 141, 141, 141, 141, 141, 141, 
    141, 142, 142, 142, 142, 142, 142, 142, 143, 157, 157, 157, 157, 156, 160, 
    160, 161, 155, 155, 154, 155, 163, 164, 164, 164, 165, 165, 165, 165, 165, 
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 250, 250, 250, 251, 251, 
    251, 252, 252, 252, 252, 253, 253, 253, 254, 254, 254, 254, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    255, 
  };
  
  void *malloc();
  int abs();


  dithered_img = malloc(columns * rows * CHAR_WIDTH * CHAR_HEIGHT);

  xs = ((double) aPic->width) / ((double) columns * CHAR_WIDTH);
  ys = ((double) aPic->height) / ((double) rows * CHAR_HEIGHT);

  for (crow = 0, y = 0.0; crow < rows * CHAR_HEIGHT; crow++, y += ys) {
    for (ccol = 0, x = 0.0; ccol < columns * CHAR_WIDTH; ccol++, x += xs) {
      bw = (int) (x + xs) - (int) (x);
      bh = (int) (y + ys) - (int) (y);
      bw = (bw < 1) ? 1 : bw;
      bh = (bh < 1) ? 1 : bh;
      for (i = 0; i < bh; i++) {
	for (j = 0; j < bw; j++) {
	  bitmap[j][i] = MV;
	  if (x + j < aPic->width && y + i < aPic->height) {
	    bitmap[j][i] = getval(x + j, y + i, MONO, aPic);
	    bitmap[j][i] = remap[bitmap[j][i]];
	  }
	}
      }

      d = 0;
      for (i = 0, w = 0, h = 0; i < bw; i++, w++)
	for (j = 0, h = 0; j < bh; j++, h++)
	  d += MV - bitmap[i][j];
	  
      dithered_img[crow * columns * CHAR_WIDTH + ccol] = ld;
      
      if (h * w)
	dithered_img[crow * columns * CHAR_WIDTH + ccol] = ld = d / (h * w);
    }
  }

  for (crow = 0; crow < rows * CHAR_HEIGHT; crow++)
    for (ccol = 0; ccol < columns * CHAR_WIDTH; ccol++) {
      for (d = MV, i = 0; i < lc; i++) {
	j = dithered_img[crow * columns * CHAR_WIDTH + ccol] - cdens[i][ccol % CHAR_WIDTH][crow % CHAR_HEIGHT];
	if (abs(j) < abs(d))
	  d = j;
      }
      if (ccol+1 < columns * CHAR_WIDTH)
	dithered_img[crow*columns*CHAR_WIDTH+ccol+1]=CLIP(dithered_img[crow*columns*CHAR_WIDTH+ccol+1]+(d*7)/16,0,MV);
      if (crow+1 < rows * CHAR_HEIGHT && ccol-1 < columns * CHAR_WIDTH)
	dithered_img[(crow+1)*columns*CHAR_WIDTH+ccol-1]=CLIP(dithered_img[(crow+1)*columns*CHAR_WIDTH+ccol-1]+(d*5)/16,0,MV);
      if (crow+1 < rows * CHAR_HEIGHT && ccol+0 < columns * CHAR_WIDTH)
	dithered_img[(crow+1)*columns*CHAR_WIDTH+ccol]=CLIP(dithered_img[(crow+1)*columns*CHAR_WIDTH+ccol]+(d*1)/16,0,MV);
      if (crow+1 < rows * CHAR_HEIGHT && ccol+1 < columns * CHAR_WIDTH)
	dithered_img[(crow+1)*columns*CHAR_WIDTH+ccol+1]=CLIP(dithered_img[(crow+1)*columns*CHAR_WIDTH+ccol+1]+(d*3)/16,0,MV);
    }
    
  return;
}
#else
#define BL	5
#define CP	(BL * columns)
#define RP	(BL * rows)
#define WR	((double) BL / cw)
#define HR	((double) BL / ch)

image charImage = {
    1, 160, 15, 20,
    (unsigned char *) "\377\377\377\377\377\377\377\000\377\377\377\000\377\000\377\377\000\377\000"
    "\377\377\000\000\000\377\377\000\377\377\000\377\377\000\377\377\377\377\000"
    "\377\377\377\377\000\377\377\377\377\000\377\377\377\377\000\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\000\377\000\000\000\377\377\377\000\377\377\377\000\000\000\377"
    "\000\000\000\000\377\377\377\000\000\377\000\000\000\000\000\377\000\000\000"
    "\377\000\000\000\000\000\377\000\000\000\377\377\000\000\000\000\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\000\377\377\377\377\377\377\377\000"
    "\377\377\377\377\000\000\000\377\377\377\377\377\377\377\377\000\377\377\377"
    "\000\377\000\377\000\000\000\000\000\000\377\000\377\377\000\377\377\000\377"
    "\377\000\377\000\377\377\377\000\377\377\377\000\377\377\377\377\377\377\000"
    "\377\377\000\000\000\377\377\377\000\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\000\377\000\377\377\000\000\377\000"
    "\000\377\377\000\377\377\377\000\000\377\377\377\000\377\000\377\000\377\000"
    "\377\377\377\377\000\377\377\377\377\377\377\377\377\000\000\377\377\377\000"
    "\000\377\377\377\000\377\377\000\377\377\377\377\000\377\377\377\377\000\377"
    "\377\000\000\000\000\000\377\377\000\377\377\000\377\377\377\000\377\377\377"
    "\377\377\377\377\000\377\377\377\377\377\377\377\377\000\000\000\377\377\000"
    "\000\000\377\377\377\000\377\377\377\377\000\377\377\377\377\377\377\377\377"
    "\000\377\377\377\377\377\377\000\377\377\377\000\377\377\377\000\000\000\377"
    "\377\377\377\377\377\000\000\000\000\000\377\377\377\377\377\377\377\000\377"
    "\377\000\377\000\377\000\000\377\000\377\377\377\377\377\000\377\377\377\000"
    "\000\377\000\377\377\000\377\000\000\000\000\377\000\000\000\000\377\377\377"
    "\377\000\377\377\000\000\000\377\377\000\000\000\000\377\377\377\377\377\377"
    "\377\377\377\377\377\000\377\377\377\377\377\377\377\377\377\377\377\000\377"
    "\377\377\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\000\000\000\000\000\377\377\000\377\000\377\000\377\377\000\377\000\377"
    "\000\377\377\377\377\377\377\377\000\377\377\377\377\377\377\000\377\377\000"
    "\000\000\377\377\377\000\377\377\377\377\000\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\000\377\377\377\000\000\377\377\000\377\377\000\377\377"
    "\377\377\000\377\377\000\377\377\377\000\000\000\000\000\000\377\377\377\377"
    "\000\000\377\377\377\000\377\377\000\377\377\000\377\377\377\000\377\377\377"
    "\000\377\377\377\000\377\377\377\377\000\377\377\377\377\000\377\377\000\000"
    "\000\000\000\377\377\000\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\000\377\377\377\377\377\377\377\377\000\377\000\377\377\000\000\000\377"
    "\000\377\377\000\377\377\000\000\377\000\377\377\377\377\377\377\377\000\377"
    "\377\377\377\000\377\377\377\377\000\377\377\377\377\377\377\377\377\000\377"
    "\377\377\377\377\377\377\377\377\377\000\377\377\000\377\377\377\377\377\000"
    "\000\000\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377"
    "\377\377\000\377\000\000\000\000\377\377\000\000\000\377\377\377\000\377\377"
    "\377\000\000\000\377\377\377\000\377\377\377\377\377\377\377\377\000\377\377"
    "\377\377\377\377\000\377\377\377\377\377\377\377\000\377\377\377\377\377\000"
    "\377\377\377\000\000\000\377\377\377\000\377\377\000\000\000\000\377\377\000"
    "\000\000\377\000\000\000\000\377\000\000\000\000\000\000\000\000\000\000\377"
    "\000\000\000\377\000\377\377\377\000\000\000\000\000\000\377\377\377\377\000"
    "\000\377\377\377\000\000\377\377\377\377\000\377\377\377\000\000\377\377\377"
    "\000\000\000\000\000\377\000\000\000\000\377\377\000\000\000\377\000\000\000"
    "\000\377\377\000\000\000\000\000\000\000\000\000\000\377\377\377\000\000\377"
    "\377\377\000\000\377\377\377\000\000\377\377\377\000\000\377\377\377\000\000"
    "\000\000\000\000\377\000\000\000\377\000\377\377\377\377\377\000\000\000\377"
    "\377\377\377\377\377\377\377\377\377\377\000\377\000\377\000\377\000\377\000"
    "\377\000\377\377\377\000\000\377\377\377\000\000\377\377\377\000\000\377\377"
    "\377\377\000\377\377\377\377\000\377\377\377\377\000\377\377\377\000\377\377"
    "\000\377\377\377\377\377\377\000\000\377\377\000\377\000\377\377\377\377\000"
    "\000\377\000\000\000\000\377\377\000\000\377\377\377\000\000\377\377\377\000"
    "\000\377\377\377\000\000\377\377\377\000\000\377\377\377\377\377\377\000\377"
    "\377\000\377\377\377\000\000\377\377\377\000\000\377\377\377\000\377\000\377"
    "\000\377\000\377\377\377\000\377\377\377\000\377\377\000\377\377\377\377\000"
    "\377\377\377\377\377\377\000\377\377\377\000\377\377\377\377\377\377\377\000"
    "\377\000\000\000\000\377\377\377\000\000\000\000\000\000\000\377\377\377\377"
    "\000\377\377\377\000\000\000\000\000\377\000\000\000\000\377\000\377\000\000"
    "\000\000\000\000\000\000\377\377\000\377\377\377\377\377\377\000\000\000\000"
    "\377\377\000\377\377\377\377\000\377\000\377\000\000\377\000\377\000\000\377"
    "\377\377\000\000\000\000\000\377\000\377\377\377\000\000\000\000\000\377\377"
    "\000\000\000\377\377\377\000\377\377\000\377\377\377\000\000\377\377\377\000"
    "\000\377\000\377\000\377\377\000\377\377\377\000\377\000\377\377\377\000\377"
    "\377\377\000\377\377\377\377\377\000\377\377\377\377\377\000\377\377\000\377"
    "\000\377\377\377\377\377\377\000\377\377\377\377\000\000\000\000\000\000\377"
    "\377\377\000\000\377\377\377\000\000\377\377\377\000\000\377\377\377\377\000"
    "\377\377\377\377\000\377\377\377\000\000\377\377\377\000\377\377\000\377\377"
    "\000\377\377\377\000\000\377\377\000\377\000\377\377\377\377\000\377\377\377"
    "\000\000\377\377\000\000\000\377\377\377\000\000\377\377\377\377\000\377\377"
    "\000\000\000\377\377\000\377\377\377\377\000\000\377\377\000\377\377\000\377"
    "\377\377\000\377\000\377\000\377\000\000\377\000\000\377\000\377\000\377\377"
    "\377\000\377\377\377\000\377\377\377\377\000\377\377\377\377\377\377\000\377"
    "\377\377\377\000\377\377\377\377\377\377\377\377\377\377\377\377\000\000\000"
    "\000\000\377\377\377\000\000\000\000\000\377\377\000\000\000\377\000\000\000"
    "\000\377\000\000\000\000\000\000\377\377\377\377\377\000\000\000\377\000\377"
    "\377\377\000\000\000\000\000\000\377\000\000\000\377\000\377\377\377\000\000"
    "\000\000\000\000\000\377\377\377\000\000\377\377\377\000\377\000\000\000\377"
    "\000\377\377\377\377\377\000\000\000\000\000\377\377\377\000\000\000\000\000"
    "\377\377\377\000\377\377\377\000\000\000\377\377\377\000\377\377\000\377\377"
    "\377\000\000\377\377\377\000\377\377\000\377\377\000\000\000\000\000\377\000"
    "\000\000\377\377\377\377\377\000\377\000\000\000\377\377\377\377\377\377\000"
    "\000\000\000\000\377\000\377\377\377\377\377\377\377\377\000\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\000\377\377\377\377\377\377\377\000\377"
    "\377\377\377\377\377\377\000\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\000\377\377\377\377\377\000\000\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\000\000\377\377\377\000\377\377\377\000\000"
    "\377\377\377\000\377\377\000\377\377\377\377\377\377\377\000\377\377\377\377"
    "\377\377\377\000\377\377\377\377\377\377\377\377\377\377\377\377\377\000\377"
    "\377\377\377\377\377\000\377\000\377\377\377\377\377\377\000\377\377\377\377"
    "\377\377\000\377\377\377\377\000\377\377\000\377\377\377\377\377\377\000\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\000\377\377"
    "\377\377\000\377\377\377\377\000\377\377\000\377\000\000\377\377\377\377\377"
    "\377\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\377\000\000"
    "\000\000\377\000\000\000\000\377\000\000\000\377\377\000\377\377\377\377\000"
    "\000\000\377\000\000\000\000\377\377\377\377\377\377\377\377\000\377\377\000"
    "\000\377\377\000\377\377\000\377\377\000\000\000\000\377\000\000\000\000\377"
    "\377\000\000\000\377\377\000\000\000\000\000\000\000\000\377\000\377\000\000"
    "\377\000\000\000\000\000\000\000\000\000\000\000\377\377\377\000\000\377\377"
    "\377\000\000\377\377\377\000\000\377\377\377\000\000\377\377\377\000\000\000"
    "\000\000\000\377\000\000\377\377\377\377\000\377\377\377\377\000\000\377\377"
    "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\000\377\377\377\000"
    "\000\377\377\377\000\000\377\377\377\377\000\377\377\377\000\000\377\377\377"
    "\000\000\000\000\377\377\000\377\377\377\000\000\377\377\377\000\377\377\000"
    "\377\377\377\377\000\377\377\000\000\000\000\377\377\377\000\377\377\000\377"
    "\000\377\000\000\377\377\377\000\000\377\377\377\000\000\377\377\377\000\000"
    "\377\377\377\000\000\000\377\377\000\377\000\000\000\377\377\377\000\377\377"
    "\000\377\377\377\000\377\000\377\000\377\000\377\000\377\000\377\000\000\000"
    "\377\000\377\377\377\000\377\000\377\000\377\377\377\000\377\377\377\377\000"
    "\377\377\377\377\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
    "\377\377\377\377\000\000\000\000\000\000\000\000\377\000\000\000\000\000\377"
    "\000\000\000\000\377\000\000\000\377\377\000\377\377\377\377\000\000\000\377"
    "\000\377\377\377\000\377\377\000\377\377\377\377\000\377\377\000\377\377\377"
    "\000\377\000\000\000\377\000\377\377\377\000\000\377\377\377\000\377\000\000"
    "\000\377\377\000\000\000\000\000\000\000\000\377\000\377\377\377\377\000\000"
    "\000\000\000\377\377\000\377\377\377\000\000\000\000\377\377\000\377\377\000"
    "\000\000\000\377\000\377\377\377\000\377\000\000\000\000\000\000\000\000\000"
    "\377\377\000\000\377\377\377\000\377\377\377\000\000\377\377\377\377\377\377"
    "\377\377\377\377\377\377",
  };
#endif

int infoAscii(deviceCap *cap)
{
  cap->info.maxWidth = NOMAX;
  cap->info.maxHeight = NOMAX;
  cap->info.maxBitsPerPixel = 8;
  cap->info.maxBitsPerColor = 8;
  cap->info.colorSpace = MONOMASK;
  cap->info.grayRamp = NULL;

  cap->canScale = YES;
  cap->canSmooth = YES;
  cap->canRotate = NO;
  cap->canDither = YES;
  cap->dontScale = YES;
#ifndef LOWRES
  cap->info.maxBitsPerPixel = 1;
  cap->info.maxBitsPerColor = 1;
  cap->canDither = YES;
  cap->dontScale = YES;
#endif

  return PICERR_NOERR;
}

int dispAscii(image *aPic, deviceCmd *cmd)
{
#ifdef LOWRES
  int i, j;
  int columns = -1, rows = -1;
  int gc;
  int width, height;
  struct winsize wsize;
#else
  int i, j, x, y;
  int c, xo, yo;
  int lv, lc, d;
  struct winsize wsize;
  int columns = -1, rows = -1;
  double xs2, ys2;
  int cw = 6, ch = 11;

  extern void handleDithering();
#endif


  if (!aPic) /* End of pictures signal. I don't care */
    return PICERR_NOERR;

#ifdef LOWRES
  ioctl(0, TIOCGWINSZ, &wsize);

  if (wsize.ws_xpixel)
    cw = wsize.ws_xpixel / wsize.ws_col;
  if (wsize.ws_ypixel)
    ch = wsize.ws_ypixel / wsize.ws_row;
  
  if (cmd->charFile)
    get_cdens(cmd->charFile, cmd->lastChar);
  else
    for (gc = 0; gc < lc; gc++)
      for (i = 0; i < CHAR_WIDTH; i++)
	for (j = 0; j < CHAR_HEIGHT; j++)
	  cdens[gc][i][j] = cdens[gc][i][j] * (255.0 / 100.0);

  if (aPic->photoInterp & COLORMASK)
    makeMono(aPic);

  if (cmd->charWidth)
    cw = cmd->charWidth;
  if (cmd->charHeight)
    ch = cmd->charHeight;

  if (cmd->columns)
    columns = cmd->columns;
  if (cmd->rows)
    rows = cmd->rows;

  if (columns < 0 || rows < 0) {
    if (columns < 0)
      columns = wsize.ws_col - 1;
    if (rows < 0)
      rows = wsize.ws_row - 1;
  }

  if (cmd->scaleToFit) {
    width = aPic->width * cmd->xscale;
    height = aPic->height * cmd->yscale;
    if (!cmd->rows) {
      rows = ((double) columns / ((double) width / (double) cw)) * ((double) height / (double) ch);
    }
    else {
      columns = ((double) rows / ((double) height / (double) ch)) * ((double) width / (double) cw);
      if (cmd->columns && columns > cmd->columns) {
	columns = cmd->columns;
	rows = ((double) columns / ((double) width / (double) cw)) * ((double) height / (double) ch);
      }
    }
  }

  ascii_dither(aPic, columns, rows);

  for (j = 0; j < rows; j++) {
    if (cmd->leadChar)
      fprintf(stdout, "%c", cmd->leadChar);
    for (i = 0; i < columns; i++) {
      gc = closest_match(i, j, columns);
      fprintf(stdout, "%c", gc);
    }
    fprintf(stdout, "\n");    
  }
#else
  ioctl(0, TIOCGWINSZ, &wsize);

  if (wsize.ws_xpixel)
    cw = wsize.ws_xpixel / wsize.ws_col;
  if (wsize.ws_ypixel)
    ch = wsize.ws_ypixel / wsize.ws_row;
  
  if (cmd->charWidth)
    cw = cmd->charWidth;
  if (cmd->charHeight)
    ch = cmd->charHeight;

  if (cmd->columns)
    columns = cmd->columns;
  if (cmd->rows)
    rows = cmd->rows;

  if (columns < 0)
    columns = wsize.ws_col - 1;
  if (rows < 0)
    rows = wsize.ws_row - 1;

  xs2 = cmd->xscale;
  ys2 = cmd->yscale;
  if (cmd->scaleToFit) {
    if (!cmd->rows && cmd->columns) {
      xs2 = (double) CP / ((double) aPic->width * cmd->xscale * WR) * cmd->xscale;
      ys2 = ((double) CP / ((double) aPic->width * cmd->xscale * WR)) * cmd->yscale;
    }
    else if (cmd->rows) {
      xs2 = ((double) RP / ((double) aPic->height * cmd->yscale * HR)) * cmd->xscale;
      ys2 = (double) RP / ((double) aPic->height * cmd->yscale * HR) * cmd->yscale;
      if (cmd->columns && ((aPic->width * xs2 * WR) / BL) > cmd->columns) {
	xs2 = (double) CP / ((double) aPic->width * cmd->xscale * WR) * cmd->xscale;
	ys2 = ((double) CP / ((double) aPic->width * cmd->xscale * WR)) * cmd->yscale;
      }
    }
  }
  else {
    if (cmd->columns)
      xs2 = (double) CP / (double) aPic->width;
    if (cmd->rows)
      ys2 = (double) RP / (double) aPic->height;
  }

  makeMono(aPic);
  scaleImage(xs2 * WR, ys2 * HR, 0, aPic);
  if (cmd->ditherMode)
    handleDithering(cmd->ditherMode, aPic, 1, NULL);
  negateImage(aPic);
  
  for (i = 0; i < aPic->height; i += BL) {
    for (j = 0; j < aPic->width; j += BL) {
      for (c = lc = lv = 0; c < 96; c++) {
	xo = (c % 32) * BL;
	yo = (c / 32) * BL;
	for (x = d = 0; x < BL; x++)
	  for (y = 0; y < BL; y++) {
	    if (getval(xo + x, yo + y, MONO, &charImage) != getval(j + x, i + y, MONO, aPic))
	      d++;
	  }
	if (d > lv) {
	  lc = c;
	  lv = d;
	}
      }
      printf("%c", lc + ' ');
    }
    printf("\n");
  }
  
#endif

  return PICERR_NOERR;
}
