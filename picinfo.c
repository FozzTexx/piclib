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

#include <fozzlib/getargs.h>

#include "piclib.h"
#include "picloader.h"

int main(int argc, char *argv[])
{
  int count;
  imageInfo info;
  int err;
  static char *interps[] = {
    "Nothing",
    "Monochrome",
    "Color",
    "Color/Monochrome",
    "CMYK",
    "CMYK/Monochrome",
    "CMYK Color",
    "CMYK Color/Monochrome",
    "Palette",
    "Palette/Monochrome",
    "Palette Color",
    "Palette Color/Monochrome",
    "CMYK Palette Color",
    "Palette/CMYK/Monochrome",
    "Palette CMYK Color",
    "Palette CMYK Color/Monochrome",
  };
  

  count = getargs(argc, argv, "");

  if (count < 0 || count == argc) {
    fprintf(stderr,
	     "Usage: %s filename ...\n"
	     , *argv);
    exit(1);
  }

  for (; count < argc; count++) {
    if ((err = getImageInfo(argv[count], &info))) {
      fprintf(stderr, "Cannot get info on file %s\nError: %s\n", argv[count], picErrorStr(err));
      exit(1);
    }

    fprintf(stdout, "%s:\n", argv[count]);
    fprintf(stdout, "  File Type: %s\n", info.type);
    fprintf(stdout, "  Photometric Interpretation: %s\n",
	    interps[info.photoInterp & ~ALPHAMASK]);
    if (!(info.photoInterp & PALETTEMASK) && (info.photoInterp & COLORMASK))
      fprintf(stdout, "  Planar Config: %s\n", info.planarConfig == PLANAR ? "Planar" : "Meshed");
    fprintf(stdout, "  Width: %i\n", info.width);
    fprintf(stdout, "  Height: %i\n", info.height);

    if (info.photoInterp & PALETTEMASK) {
      fprintf(stdout, "  Bits Per Color: %i\n", info.bitsPerColor);
      fprintf(stdout, "  Number of Colors: %i\n", info.numColors);
    }
    else {
      fprintf(stdout, "  Bits Per Pixel: %i\n", info.bitsPerPixel);
    }
  }

  exit(0);
}
    
