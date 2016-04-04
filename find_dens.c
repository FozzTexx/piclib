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
#include <fozzlib/getargs.h>
#include <libc.h>
#include "piclib.h"
#include "picloader.h"

#define BLOCK_WIDTH 20
#define BLOCK_OFFSET 0

/*
cc -o find_dens find_dens.c ../getargs.o -L. -lpiclib -lNeXT_s
*/

void main(int argc, char *argv[])
{
  image myPic;
  int count;
  int i, j;
  int x, y;
  int density;
  int mapping[256];
  int map2[256];
  int col;
  int min;
  double offby;
  
  
  count = getargs(argc, argv, "");

  if (count < 0 || count == argc) {
    fprintf(stderr, "Usage: %s filename\n"
	     , *argv);
    exit(1);
  }

  load_pic(argv[count], &myPic);

  scaleImage(320.0 / myPic.width, 320.0 / myPic.height, 0, &myPic);
  
  for (y = 0; y < 320; y += BLOCK_WIDTH)
    for (x = 320 - BLOCK_WIDTH; x >= 0; x -= BLOCK_WIDTH) {
      density = 0;
      for (i = 0; i < BLOCK_WIDTH - BLOCK_OFFSET * 2; i++)
	for (j = 0; j < BLOCK_WIDTH - BLOCK_OFFSET * 2; j++)
	  density += getval(i + x + BLOCK_OFFSET, j + y + BLOCK_OFFSET, MONO, &myPic);
      density /= i * j;
      i = 255 - ((15 - (x / 20)) + (y / 20) * 16);
      mapping[i] = density;
    }

  for (i = 0; i < 256; i++) {
    for (x = 0, y = 0, j = i - 5; j < i + 6; j++, y++) {
      if (j < 0) 
	density = 0;
      else if (j > 255)
	density = 255;
      else
	density = mapping[j];
      x += density;
    }
    map2[i] = x / y;
  }

  for (i = 0; i < 256; i++)
    mapping[i] = map2[i];

  for (i = 0; i < 256; i++) {
    for (x = 0, y = 0, j = i - 5; j < i + 6; j++, y++) {
      if (j < 0) 
	density = 0;
      else if (j > 255)
	density = 255;
      else
	density = mapping[j];
      x += density;
    }
    map2[i] = x / y;
  }

  for (i = 0; i < 256; i++)
    mapping[i] = map2[i];

  min = mapping[0];
  offby = 255.0 / (mapping[255] - min);
  for (i = 0; i < 256; i++)
    mapping[i] = (mapping[i] - min) * offby;
  
  for (col = 0, i = 0; i < 256; i++) {
    if (abs(i - mapping[i]) > 3) {
      for (j = 0, x = 255, y = 255; j < 256; j++)
	if (abs(i - mapping[j]) < x) {
	  x = abs(i - mapping[j]);
	  y = j;
	}
      map2[i] = y;
    }
    else
      map2[i] = i;
    fprintf(stdout, "%3i, ", map2[i]);
    col += 5;
    if (col > 70) {
      fprintf(stdout, "\n");
      col = 0;
    }
  }

  fprintf(stdout, "\n");
  
  exit(0);
}
