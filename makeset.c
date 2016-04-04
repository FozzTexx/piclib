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


main(int argc, char *argv[])
{
  picRec myPic;
  int i, j, k, l;
  int left, right, top, bottom;
  int l2, r2, t2, b2;


  load_pic("/fozztexx/pics/ibm.set", &myPic);
  
  for (i = 0, left = myPic.width, top = myPic.height; i < myPic.width; i++)
    for (j = 0; j < top; j++) {
      if (myPic.photoInterp & COLORMASK)
	k = get_pixel(i, j, RED, &myPic) || get_pixel(i, j, GREEN, &myPic) || get_pixel(i, j, BLUE, &myPic);
      else
	k = get_pixel(i, j, MONO, &myPic);
      if (k) {
	if (i < left)
	  left = i;
	if (j < top)
	  top = j;
      }
    }
  for (i = myPic.width - 1, right = 0, bottom = 0; i >= 0; i--)
    for (j = myPic.height - 1; j >= bottom; j--) {
      if (myPic.photoInterp & COLORMASK)
	k = get_pixel(i, j, RED, &myPic) || get_pixel(i, j, GREEN, &myPic) || get_pixel(i, j, BLUE, &myPic);
      else
	k = get_pixel(i, j, MONO, &myPic);
      if (k) {
	if (i > right)
	  right = i;
	if (j > bottom)
	  bottom = j;
      }
    }
  l2 = left;
  t2 = top;
  r2 = right;
  b2 = bottom;
    
  for (i = l2, left = r2 + 1, top = b2 + 1; i < r2 + 1; i++)
    for (j = t2; j < top; j++) {
      if (myPic.photoInterp & COLORMASK)
	k = get_pixel(i, j, RED, &myPic) && get_pixel(i, j, GREEN, &myPic) && get_pixel(i, j, BLUE, &myPic);
      else
	k = get_pixel(i, j, MONO, &myPic);
      if (!k) {
	if (i < left)
	  left = i;
	if (j < top)
	  top = j;
      }
    }
  for (i = r2, right = l2, bottom = t2; i >= l2; i--)
    for (j = b2; j >= bottom; j--) {
      if (myPic.photoInterp & COLORMASK)
	k = get_pixel(i, j, RED, &myPic) && get_pixel(i, j, GREEN, &myPic) && get_pixel(i, j, BLUE, &myPic);
      else
	k = get_pixel(i, j, MONO, &myPic);
      if (!k) {
	if (i > right)
	  right = i;
	if (j > bottom)
	  bottom = j;
      }
    }

  crop_image(left, top, right, bottom, &myPic);

  fprintf(stdout, "%%!\n");
  fprintf(stdout, "%%%%BoundingBox: 0 0 %i %i\n", 8 * 32, 14 * 8);
  fprintf(stdout, "/picstr 1 string def\n");
  fprintf(stdout, "/screen {gsave translate 8 14 scale 8 14 4 [8 0 0 -14 0 14] {currentfile picstr readhexstring pop} image grestore} def\n");

  for (left = 0, top = 7, i = 0; i < myPic.height - 15; i += 15)
    for (j = 0; j < myPic.width - 9; j += 9)
      if (!(i == 15 && j >= 72)) {
	left++;
	if (left >= 32)
	  left = 0, top--;

	fprintf(stdout, "%i %i screen\n", left * 8, top * 14);
      
	for (k = i+1; k < i+15; k++)
	  for (l = j+1; l < j+9; l++)
	    fprintf(stdout, "%x", get_pixel(l, k, MONO, &myPic) * 15);
	fprintf(stdout, "\n");
      
      }
      
  exit(0);
}

