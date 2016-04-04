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
#include <sys/malloc.h>
#include <signal.h>
#include <strings.h>
#include <fozzlib/getargs.h>

#include "piclib.h"
#include "picloader.h"


char *strip_extension(char *filename);

void main(int argc, char *argv[])
{
  double xscale = 1, yscale = 1, fitscale = 1;
  double scale1, scale2;
  double scaleWidth, scaleHeight;
  image myPic;
  int i, j;
  int left = -1, right = -1, top = -1, bottom = -1;
  int scaletofit = NO;
  int autoCrop = NO;
  int rotate = NO;
  int count;
  char buf[256];
  FILE *epsFile;
  int err = 0;
  int negative = NO;
  
  double atof();
  void system();
  void exit();

  
  count = getargs(argc, argv, "nbxdydsbabLiRiTiBi",
		   &negative, &xscale, &yscale, &scaletofit,
		   &autoCrop,
		   &left, &right, &top, &bottom);

  if (count < 0 || count == argc) {
    fprintf(stderr,
	     "Usage: %s [-options] filename\n"
	     "  options:\n"
	     "\tn: draw image as negative\n"
	     "\tx, y: specify scale factor in that direction\n"
	     "\ts: scale image to fit in 4\" by 2.5\" rect\n"
	     "\ta: auto crop solid plotting areas\n"
	     "\tL, R, T, B: specify where to crop picture\n"
	     , *argv);
    exit(1);
  }

  for (; count < argc; count++) {
    if (err = load_pic(argv[count], &myPic)) {
      fprintf(stderr, "Cannot load file %s\nError: %s\n", argv[count], picErrorStr(err));
      continue;
    }

    if ((myPic.photoInterp & (COLORMASK | MONOMASK)) != COLORMASK &&
	(myPic.photoInterp & (COLORMASK | MONOMASK)) != MONOMASK) {
      fprintf(stderr, "Not a valid file\n");
      exit(1);
    }

    if (autoCrop) {
      autoCropImage(&myPic);
    }
    else if (left >= 0 || right >= 0 || top >= 0 || bottom >= 0) {
      if (left < 0)
	left = 0;
      if (right < 0)
	right = myPic.width - 1;
      if (top < 0)
	top = 0;
      if (bottom < 0)
	bottom = myPic.height - 1;
      cropImage(left, top, right, bottom, &myPic);
    }

    scaleWidth = myPic.width * xscale;
    scaleHeight = myPic.height * yscale;

    if (scaletofit) {
      scale1 = 288.0 / scaleWidth;
      if (scaleHeight * scale1 > 180)
	scale1 = 180.0 / (double) scaleHeight;
    
      scale2 = 288.0 / (double) scaleHeight;
      if (scaleWidth * scale2 > 180)
	scale2 = 180.0 / (double) scaleWidth;
    
      i = 51840 - ((int) (scaleWidth * scale1) * (scaleHeight * scale1));
      j = 51840 - ((int) (scaleWidth * scale2) * (scaleHeight * scale2));
    
      if (j < i) {
	rotate = YES;
	fitscale = scale2;
      }
      else {
	fitscale = scale1;
	rotate = NO;
      }
    }

    sprintf(buf, "%s.eps", strip_extension(argv[count]));
    if (!(epsFile = fopen(buf, "w"))) {
      fprintf(stderr, "Cannot open %s for output\n", buf);
      exit(1);
    }
    
    fprintf(stderr, "Writing %s...", buf);

    if (myPic.photoInterp & COLORMASK)
      makeMono(&myPic);

    fprintf(epsFile, "%%!PS-Adobe-2.0 EPSF-2.0\n");
    fprintf(epsFile, "%%%%Creator:pictoeps\n");
    if (!rotate) 
      fprintf(epsFile, "%%%%BoundingBox: %f %f %f %f\n\n", 0.0, 0.0, (double) scaleWidth * fitscale, (double) scaleHeight * fitscale);
    else
      fprintf(epsFile, "%%%%BoundingBox: %f %f %f %f\n\n", 0.0, 0.0, (double) scaleHeight * fitscale, (double) scaleWidth * fitscale);
    
    fprintf(epsFile, "currentscreen 3 1 roll pop pop 60 90 3 -1 roll setscreen\n");
    fprintf(epsFile, "/picstr %d string def\n", myPic.bpl);
    fprintf(epsFile, "/screen {%d %d %d [%d 0 0 -%d 0 %d]\n", myPic.width, myPic.height, myPic.bpp, myPic.width, myPic.height, myPic.height);
    fprintf(epsFile, "   {currentfile picstr readhexstring pop} image} def\n");
    if (rotate)
      fprintf(epsFile, "%f 0 translate 90.0 rotate\n", (double) scaleHeight * fitscale);
    fprintf(epsFile, "%f %f scale screen\n", (double) scaleWidth * fitscale, (double) scaleHeight * fitscale);
  
    for (i = 0; i < myPic.height; i++) {
      for (j = 0; j < myPic.bpl; j++) {
	if (j % 40 == 0)
	  fprintf(epsFile, "\n");
	fprintf(epsFile, "%02X", myPic.data[i * myPic.bpl + j]);
      }
      fprintf(epsFile, "\n");
    }

    fprintf(stderr, "\n");
    
    free(myPic.data);
    
    fclose(epsFile);
  }

  exit(0);
}

char *strip_extension(char *filename)
{
  static char buf[256];
  int i;


  for (i = strlen(filename) - 1; filename[i] != '.' && filename[i] != '/' && i >= 0; i--);

  if (filename[i] == '.') {
    strncpy(buf, filename, i);
    buf[i] = 0;
  }
  else
    strcpy(buf, filename);

  return buf;
}
