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
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <fozzlib/getargs.h>

#include "piclib.h"
#include "picloader.h"
#include "picsaver.h"


static void handle_dithering(int ditherMode, image *aPic, int outType, int numBits);
char *change_extension(char *name, char *from, char *to, int insert);

int main(int argc, char *argv[])
{
  image myPic;
  imageInfo picInfo;
  int count;
  char *outputType = NULL;
  int err;
  int aCrop = NO;
  int left = -1, right = -1, top = -1, bottom = -1;
  double xscale = 1, yscale = 1;
  double xs2, ys2;
  int xdim = 0, ydim = 0;
  int ditherMode = ' ';
  int mMono = NO;
  int outType = NO;
  int scaletofit = NO;
  int numBits = 1;
  char *outFile = NULL;
  char *type = NULL, *ext = NULL;
  char *p;
  int nv;
  double darken = 0;
  double contrast = 0;
  double rotate = 0;
  struct stat sbuf;
  int blend = NO;
  static char filename[MAXPATHLEN + 1] = "";
  int negative = NO;
  int enhance = NO;
  formatInfo fInfo;
  

  count = getargs(argc, argv, "nbosabLiRiTiBixdydXiYidcmbpbsbbiDdSbKdfseird",
		   &negative, &outputType, &aCrop, &left, &right, &top,
		   &bottom, &xscale, &yscale, &xdim, &ydim, &ditherMode,
		   &mMono, &outType, &scaletofit, &numBits, &darken,
		   &blend, &contrast, &outFile, &enhance, &rotate
		   );

  if (count < 0 || count == argc) {
    if (count < 0 && -count != '-')
      fprintf(stderr, "Bad flag: %c\n", -count);
    fprintf(stderr, "Usage: %s [-options] filename\n"
	     "  options:\n"
	     "\tn: make negative\n"
	     "\to: string indicating output format\n"
	     "\ta: auto crop non plotting areas\n"
	     "\tL, R, T, B: specify where to crop picture\n"
	     "\tx, y: specify scale factor in that direction\n"
	     "\tX, Y: specify dimension of picture in that direction\n"
	     "\td: select dither method: (D)ither, (T)hreshold, or (H)alftone\n"
	     "\tm: convert image to monochrome\n"
	     "\tp: dither image for output to printer\n"
	     "\ts: scale image to fill up specified screen\n"
	     "\tb: number of bits to dither to\n"
	     "\tD: how much to brighten image\n"
	     "\tK: contrast level\n"
	     "\tS: blend pixels together when scaling\n"
	     "\tf: file name to save to\n"
	     "\te: contrast for enhance image\n"
	     "\tr: number of degrees to rotate\n"
	     , *argv);
    exit(1);
  }

  for (; count < argc; count++) {
    if ((err = load_pic(argv[count], &myPic))) {
      fprintf(stderr, "Cannot load file %s\nError: %s\n", argv[count], picErrorStr(err));
      exit(1);
    }
    
    if (!getImageInfo(argv[count], &picInfo))
      type = picInfo.extension;

    if (!outputType) 
      outputType =  type;

    if (!infoForFormat(outputType, &fInfo)) {
      ext = fInfo.extension;
      if (scaletofit && !xdim && !ydim && fInfo.maxWidth && fInfo.maxHeight) {
	xdim = fInfo.maxWidth;
	ydim = fInfo.maxHeight;
      }
    }
    
    if (mMono)
      makeMono(&myPic);
    
    if (left >= 0 || right >= 0 || top >= 0 || bottom >= 0) {
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
    
    if (aCrop)
      autoCropImage(&myPic, 40);

    if (rotate)
      rotateImage(&myPic, (rotate * -3.141592) / 180.0);
    
    xs2 = xscale;
    ys2 = yscale;
    if (scaletofit) {
      if (!ydim && xdim) {
	xs2 = (double) xdim / ((double) myPic.width * xscale) * xscale;
	ys2 = ((double) xdim / ((double) myPic.width * xscale)) * yscale;
      }
      else if (ydim) {
	xs2 = ((double) ydim / ((double) myPic.height * yscale)) * xscale;
	ys2 = (double) ydim / ((double) myPic.height * yscale) * yscale;
	if (xdim && (myPic.width * xs2) > xdim) {
	  xs2 = (double) xdim / ((double) myPic.width * xscale) * xscale;
	  ys2 = ((double) xdim / ((double) myPic.width * xscale)) * yscale;
	}
      }
    }
    else {
      if (xdim)
	xs2 = (double) xdim / (double) myPic.width;
      if (ydim)
	ys2 = (double) ydim / (double) myPic.height;
    }
    xscale = xs2;
    yscale = ys2;

    scaleImage(xscale, yscale, blend, &myPic);

    if (darken || contrast)
      adjustImage(&myPic, contrast, darken);
    if (enhance) {
      makeMono(&myPic);
      enhanceImage(&myPic, enhance);
    }
    
    handle_dithering(ditherMode, &myPic, outType, numBits);
    if (negative)
      negateImage(&myPic);

    if (outFile) {
      strcpy(filename, outFile);
      if (!stat(filename, &sbuf) && (sbuf.st_mode & S_IFDIR)) {
	if (filename[strlen(filename) - 1] != '/')
	  strcat(filename, "/");
	if ((p = strrchr(argv[count], '/')))
	  strcat(filename, p + 1);
	else
	  strcat(filename, argv[count]);
      }
    }
    else
      strcpy(filename, argv[count]);
    
    p = change_extension(filename, type, ext, 0);
    nv = 2;
    while (!stat(p, &sbuf)) {
      p = change_extension(filename, type, ext, nv);
      nv++;
    }

    if ((err = save_pic(p, outputType, &myPic))) {
      fprintf(stderr, "Cannot save file %s\nError: %s\n", p, picErrorStr(err));
      exit(1);
    }

    fprintf(stderr, "Saved %s as %s\n", argv[count], p);

    freeImage(&myPic);
  }

  exit(0);
  return 0;
}

static void handle_dithering(int ditherMode, image *aPic, int outType, int numBits)
{
  static int mapping[] = {
      0,   1,   2,   3,  73,  79,  86,  88,  90,  93, 107, 119, 121, 123, 125, 136, 137,
    138, 139, 141, 152, 153, 153, 154, 155, 155, 156, 157, 158, 159, 162, 165, 166, 167,
    168, 169, 169, 170, 171, 172, 173, 174, 176, 179, 180, 181, 182, 182, 183, 183, 184,
    184, 185, 185, 186, 186, 187, 187, 188, 189, 189, 190, 194, 196, 197, 197, 198, 198,
    199, 199, 200, 200, 200, 201, 201, 201, 202, 202, 202, 203, 203, 203, 204, 204, 204,
    205, 205, 206, 206, 207, 207, 208, 209, 209, 210, 211, 211, 212, 212, 213, 213, 213,
    214, 214, 214, 215, 215, 215, 216, 216, 216, 217, 217, 217, 217, 218, 218, 218, 218,
    219, 219, 219, 219, 220, 220, 220, 221, 221, 221, 221, 222, 222, 222, 223, 223, 223,
    224, 224, 224, 225, 225, 225, 226, 226, 226, 227, 227, 227, 227, 228, 228, 228, 229,
    229, 229, 229, 229, 230, 230, 230, 230, 231, 231, 231, 231, 231, 232, 232, 232, 232,
    232, 233, 233, 233, 233, 233, 233, 234, 234, 234, 234, 234, 235, 235, 235, 235, 235,
    235, 236, 236, 236, 236, 236, 237, 237, 237, 237, 237, 238, 238, 238, 238, 238, 239,
    239, 239, 239, 239, 240, 240, 240, 240, 241, 241, 241, 241, 242, 242, 242, 242, 243,
    243, 243, 244, 244, 244, 244, 245, 245, 245, 245, 246, 246, 246, 247, 247, 247, 248,
    248, 248, 249, 249, 249, 250, 250, 250, 251, 251, 251, 252, 252, 253, 253, 254, 255,
    255,
  };


  switch(ditherMode) {
  case 'H':
  case 'h':
    if (outType)
      halftoneImage(aPic, mapping, numBits);
    else
      halftoneImage(aPic, NULL, numBits);
    break;

  case 'T':
  case 't':
    if (outType)
      thresholdImage(aPic, mapping, numBits);
    else
      thresholdImage(aPic, NULL, numBits);
    break;

  case 'D':
  case 'd':
    if (outType)
      ditherImage(aPic, mapping, numBits);
    else
      ditherImage(aPic, NULL, numBits);
    break;

  default:
    break;
  }

  return;
}

char *change_extension(char *name, char *from, char *to, int insert)
{
  static char buf[MAXPATHLEN];
  char buf2[10];
  int i, j;


  strcpy(buf, name);

  i = strlen(buf);
  if (from && to) {
    j = strlen(to);
    if (!(strcmp(&buf[i - j], to)) && buf[i - j - 1] == '.')
      buf[i - j - 1] = 0;
    else {
      j = strlen(from);
      if (!(strcmp(&buf[i - j], from)) && buf[i - j - 1] == '.')
	buf[i - j - 1] = 0;
    }

    i = strlen(buf);
    if (insert) {
      if (buf[i - 4] == '.' && atoi(&buf[i - 3]))
	buf[i - 4] = 0;

      sprintf(buf2, ".%03i", insert);
      strcat(buf, buf2);
    }

    strcat(buf, ".");
    strcat(buf, to);
  }
  else if (to) {
    j = strlen(to);
    if (!(strcmp(&buf[i - j], to)) && buf[i - j - 1] == '.')
      buf[i - j - 1] = 0;
    
    i = strlen(buf);
    if (insert) {
      if (buf[i - 4] == '.' && atoi(&buf[i - 3]))
	buf[i - 4] = 0;

      sprintf(buf2, ".%03i", insert);
      strcat(buf, buf2);
    }

    strcat(buf, ".");
    strcat(buf, to);
  }
  else if (insert) {
    i = strlen(buf);
    if (buf[i - 4] == '.' && atoi(&buf[i - 3]))
      buf[i - 4] = 0;

    sprintf(buf2, ".%03i", insert);
    strcat(buf, buf2);
  }
  
  return buf;
}
