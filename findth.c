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
#include "piclib.h"
#include "picloader.h"
#include "getargs.h"

void main(int argc, char *argv[])
{
  image myPic;
  int count;
  int i, j, k;
  int x, y;
  int lx, ly;
  int threshold[8][8];
  
  int abs();
  void exit();

  
  count = getargs(argc, argv, "");

  if (count < 0 || count == argc) {
    fprintf(stderr, "Usage: %s filename\n"
	     , *argv);
    exit(1);
  }

  load_pic(argv[count], &myPic);

  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      threshold[i][j] = 0;

  for (k = 0, ly = lx = -1; k < 86; k++) {
    x = k % 16;
    y = k / 16;
    if (lx >= 0 && ly >= 0) {
      for (i = 0; i < 8; i++)
	for (j = 0; j < 8; j++) {
	  if (getval(lx * 8 + i, 127 - (ly * 8 + j), MONO, &myPic) != getval(x * 8 + i, 127 - (y * 8 + j), MONO, &myPic))
	    threshold[i][j] = x + y * 16;
	}
    }
    lx = x;
    ly = y;
  }

  for (i = 7; i >= 0; i--) {
    for (j = 0; j < 8; j++)
      printf("%3i, ", threshold[j][i] * 3);
    printf("\n");
  }
  
  exit(0);
}
