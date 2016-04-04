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
#include <libc.h>
#include "piclib.h"
#include "picsaver.h"
#include "getargs.h"


void main(int argc, char *argv[])
{
  FILE *gfile;
  int count;
  picRec graymap;
  int i, j, k, x, y;


  count = getargs(argc, argv, "");

  if (count < 0 || count == argc) {
    fprintf(stderr, "Usage: %s filename\n");
    exit(1);
  }

  allocate_image(&graymap, MONOMASK, 320, 320, 8);

  if (!(gfile = fopen(argv[count], "r"))) {
    fprintf(stderr, "Cannot open %s\n", argv[count]);
    exit(1);
  }
  
  for (i = 0; i < 160; i++)
    for (j = 0; j < 160; j++) {
      fscanf(gfile, "%i,", &k);
      fprintf(stdout, "%i, ", k);
      k *= 255.0 / 100.0;
      for (x = 0; x < 10; x++)
	for (y = 0; y < 10; y++)
	  put_val(j * 2 + x, i * 2 + y, k, MONO, &graymap);
      fscanf(gfile, "%i,", &k);
      fprintf(stdout, "%i, ", k);
      k *= 255.0 / 100.0;
      for (x = 10; x < 20; x++)
	for (y = 0; y < 10; y++)
	  put_val(j * 2 + x, i * 2 + y, k, MONO, &graymap);
      fscanf(gfile, "%i,", &k);
      fprintf(stdout, "%i, ", k);
      k *= 255.0 / 100.0;
      for (x = 0; x < 10; x++)
	for (y = 10; y < 20; y++)
	  put_val(j * 2 + x, i * 2 + y, k, MONO, &graymap);
      fscanf(gfile, "%i,", &k);
      fprintf(stdout, "%i, ", k);
      k *= 255.0 / 100.0;
      for (x = 10; x < 20; x++)
	for (y = 10; y < 20; y++)
	  put_val(j * 2 + x, i * 2 + y, k, MONO, &graymap);
    }

  save_pic("ascii.graymap", "gif", &graymap);
  
  exit(0);
}
      
