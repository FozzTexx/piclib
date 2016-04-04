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
#include <stdlib.h>
#include "piclib.h"
#include "fmtclip.h"

static int blocksleft;
static int curlen;
static int spec;
static int bytesleft;

static int rd(FILE *file, unsigned char *b, int len)
{
  int i;
  int c;

  
  for (i = 0; i < len; i++) {
    if ((c = getc(file)) == EOF)
      return 0;
    b[i] = (c & 255);
  }
  
  return 1;
}

static int brd(FILE *file, unsigned char *b, int len)
{
  unsigned char buf[5];
  
 
  if (curlen <= 0) {
    if (blocksleft == 0)
      return 0; //PICERR_BADFORMAT;
 
    if (!rd(file, buf, 5))
      return 0; //PICERR_BADFORMAT;
 
    curlen = buf[0] + buf[1] * 256 - 5;
    /* size of buffer needed to store next block of pixels? */
    /* bufsize = buf[2] + buf[3] * 256; */
    spec = buf[4];
    blocksleft--;
  }
 
  /* this should be done right and allow for overlapping reads! :-( */
 
  if (len > curlen)
    return 0; //PICERR_BADFORMAT;
 
  if (!rd(file, b, len))
    return 0; //PICERR_BADFORMAT;
	
  curlen -= len;
	
  return 1;
}
 
int load_clip(char *filename, image *aPic)
{
  unsigned char buf[32];
  unsigned char cmap[256*3];
  int hascmap;
  int i, j, r, c, maxc=0;
  int width, height, bpp;
  unsigned char* img;
  unsigned char* p;
  FILE *file;


  curlen = 0;
  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  if (!rd(file, buf, 17))
    return PICERR_BADFORMAT;
 
  if (buf[15] != 0)
    if (!rd(file, buf + 17, 2))
      return PICERR_BADFORMAT;
	
  if (buf[0] != 0x34 || buf[1] != 0x12)
    return PICERR_BADFORMAT;
 
  if (buf[12] != 'L' && buf[12] != 'C')
    fprintf(stderr, "I may not do the right thing...\n");
	
  hascmap = buf[13] & 4;
 
  width = buf[2] + buf[3] * 256;
  height = buf[4] + buf[5] * 256;
  bpp = buf[10];
  /* I'm not real sure about the bits-per-pixel, above */
 
  /* allocation equation depends on 8 | width (| = divides) */
  if (!(img = malloc(bytesleft = width * height * bpp / 8)))
    return PICERR_CANTALLOC;

  p = img;
  
  if (hascmap) {
    if (!rd(file, cmap, 256 * 3))
      return PICERR_BADFORMAT;
	
    for (i = 0; i < 256 * 3; i++)
      cmap[i] *= 4;
  }
 
  if (!rd(file, buf, 2))
    return PICERR_BADFORMAT;
 
  blocksleft = buf[0] + buf[1] * 256;
 
  while (brd(file, buf, 1)) {
    c = buf[0];
    if (c == spec) {
      brd(file, buf, 1);
      r = buf[0];
      if (r == 0) {
	brd(file, buf, 3);
	r = buf[1] * 256 + buf[0];
	c = buf[2];
      }
      else {
	brd(file, buf, 1);
	c = buf[0];
      }
    }
    else {
      r = 1;
    }

    while (r-- > 0) {
      if (--bytesleft < 0) {
	fprintf(stderr, "file too long, ignoring extra\n");
	goto dump;
      }
      *p++ = c;
    }
    if (c>maxc) maxc=c;
    if (bytesleft == 0)
      break;
  }
  if (!hascmap) {
    for (i = 0; i < 255 * 3; i++)
      cmap[i] = (255.0/maxc)*(i/3);
  }
 
 dump:
  for (i = 0; i < height / 2; i++)
    for (j = 0; j < width; j++) {
      c = img[i * width + j];
      img[i * width + j] = img[((height - 1) - i) * width + j];
      img[((height - 1) - i) * width + j] = c;
    }
 
  if (bpp == 8) {
    constructImage(aPic, COLORMASK | PALETTEMASK, PLANAR, width, height, bpp, img, cmap);
  }
  else {
    constructImage(aPic, COLORMASK, PLANAR, width, height, bpp, img, NULL);
  }
	
  free(img);
  
  return PICERR_NOERR;
}

int info_clip(char *filename, imageInfo *info, formatInfo *fInfo)
{
  if (!filename) { /* Just want longname and extension */
    fInfo->type = "PornoGraphicAnimitionFrame";
    fInfo->extension = "clp";
    fInfo->maxWidth = NOMAX;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = 8;
    fInfo->maxBitsPerColor = 8;
    fInfo->colorSpace = COLORMASK | PALETTEMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }

  return PICERR_NOERR;
}

