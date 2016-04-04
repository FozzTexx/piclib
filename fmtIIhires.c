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

#include "fmtIIhires.h"
#include <stdio.h>
#include <stdlib.h>

#define WIDTH	280
#define HEIGHT	192

int load_IIhires(char *filename, image *aPic)
{
  return 0;
}

int save_IIhires(char *filename, image *aPic)
{
  unsigned char *buf;
  int x, y, mx, my;
  int r, g, c1, c2;
  int offset, shift, mask;
  FILE *file;


  buf = calloc(1, 8192);
  mx = aPic->width;
  if (mx > WIDTH)
    mx = WIDTH;
  my = aPic->height;
  if (my > HEIGHT)
    my = HEIGHT;

  for (y = 0; y < my; y++) {
    for (x = 0; x < mx; x++) {
      r = getval(x, y, RED, aPic);
      switch (r) {
      case 221:
	c1 = 1;
	break;
      case 34:
	c1 = 5;
	break;
      case 255:
	g = getval(x, y, GREEN, aPic);
	if (g == 102)
	  c1 = 6;
	else
	  c1 = 3;
	break;
      case 0:
	g = getval(x, y, GREEN, aPic);
	if (g == 221)
	  c1 = 2;
	else
	  c1 = 0;
	break;

      default:
	c1 = 0;
      }

      offset = (y % 8) * 1024;
      offset += ((y / 8) % 8) * 128;
      offset += (y / 64) * 40;
      offset += (x / 14) * 2;
      c2 = buf[offset] & 0x7f;
      c2 |= (buf[offset + 1] & 0x7f) << 7;

      shift = (x % 14) & 0xfe;
      mask = 7 ^ (1 - (x & 1) + 1);
      c1 &= mask;
      c2 |= (c1 & 3) << shift;
      
      buf[offset] = (c2 & 0x7f) | (buf[offset] & 128);
      buf[offset + 1] = ((c2 >> 7) & 0x7f) | (buf[offset + 1] & 128);

      if (c1 & 4) {
	if (shift > 6)
	  buf[offset + 1] |= 128;
	else
	  buf[offset] |= 128;
      }
    }
  }

  file = fopen(filename, "w");
  fwrite(buf, 1, 8192, file);
  fclose(file);

  return 0;
}

int info_IIhires(char *filename, imageInfo *info, formatInfo *fInfo)
{
  if (!filename) {
    fInfo->type = "Apple II hires (HGR) screen save";
    fInfo->extension = "hgr";
    fInfo->maxWidth = 280;
    fInfo->maxHeight = 192;
    fInfo->maxBitsPerPixel = 4;
    fInfo->maxBitsPerColor = 8;
    fInfo->colorSpace = COLORMASK | PALETTEMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }

  info->type = "Apple II hires (HGR) screen save";
  info->extension = "hgr";

  return PICERR_NOERR;
}
