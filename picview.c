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
#include <strings.h>
#include <sys/param.h>
#include <stdlib.h>

#include <fozzlib/getargs.h>

#include "piclib.h"
#include "picloader.h"

#include "device.h"
#include "devHP.h"
#include "devXerox.h"
#if defined(NS_TARGET_MAJOR) && (NS_TARGET_MAJOR == 3)
#include "devNeXT.h"
#endif
#include "devAscii.h"

#define ARGSTOGET \
  "nbHbxdydsbmbXiYiabLiRiTiBidcosfswihiCclibiDdSbKdeirdEiMb", \
  &negative, &keephidden, &xscale, &yscale, &scaletofit, &drawMono, \
  &xdim, &ydim, &acrop, &left, &right, &top, &bottom, &ditherMode, \
  &buf, &charFile, &mycw, &mych, &leadChar, &lastChar, &numBits, &darken, \
  &blend, &contrast, &enhance, &rotate, &enqack, &maxfit

typedef struct outDev {
  char *name;
  int (*infoFunc)();
  int (*dispFunc)();
} outDev;

outDev devices[] = {
  {"HPTerm", infoHP, dispHP},
#if defined(NS_TARGET_MAJOR) && (NS_TARGET_MAJOR == 3)
  {"NeXT", infoNeXT, dispNeXT},
#endif
  {"Xerox", infoXerox, dispXerox},
  {"ascii", infoAscii, dispAscii},
  {"thermal", infoHPthermal, dispHPthermal},
  {NULL, NULL, NULL},
};

void handleDithering(int ditherMode, image *aPic, int bits, int *grayRamp);

void showUsage(char *argv)
{
  fprintf(stderr,
	  "Usage: %s [-options] filename ...\n"
	  "  options:\n"
	  "\tn: draw image as negative\n"
	  "\tH: keep image hidden while drawing\n"
	  "\ts: scale image to fill up screen\n"
	  "\tm: display image as monochrome\n"
	  "\ta: auto crop non plotting areas\n"
	  "\tx, y: specify scale factor in that direction\n"
	  "\tX, Y: specify dimension of picture in that direction\n"
	  "\tL, R, T, B: specify where to crop picture\n"
	  "\td: select dither method: (D)ither, (T)hreshold, or(H)alftone\n"
	  "\to: specify output mode (NeXT, HPTerm, Xerox, ascii, thermal)\n"
	  "\tf: select pic file of character images\n"
	  "\tl: specify last character in character images\n"
	  "\tw, h: specify width and height of characters in pixels\n"
	  "\tC: set character to print at beginning of every line\n"
	  "\tb: number of bits to dither to\n"
	  "\tD: how much to brighten image\n"
	  "\tK: contrast level\n"
	  "\tS: blend pixels together when scaling\n"
	  "\te: contrast for enhance image\n"
	  "\tr: number of degrees to rotate\n"
	  "\tE: how many bytes before enqack\n"
	  "\tM: scale down if larger than screen\n"
	  , argv);
  exit(1);
}

int main(int argc, char *argv[])
{
  double xscale = 1, yscale = 1;
  double xs2 = 1, ys2 = 1;
  image myPic;
  int left = -1, right = -1, top = -1, bottom = -1;
  int keephidden = NO;
  int scaletofit = NO;
  int drawMono = NO;
  int acrop = NO;
  int ditherMode = 0;
  int outputDev = 0;
  int xdim = 0, ydim = 0;
  int xd, yd;
  int count;
  char *buf = "";
  static char *charFile = NULL;
  int err = 0;
  int mycw = 0, mych = 0, leadChar = 0, lastChar = 127;
  int negative = NO;
  int numBits = 0;
  double darken = 0;
  double contrast = 0;
  double rotate = 0;
  int blend = NO;
  int enhance = NO;
  int enqack = 0;
  int maxfit = 0;
  deviceCmd dCmd;
  deviceCap dCap;
  int i;
  

  count = getargs(argc, argv, ARGSTOGET);
  
  if (count < 0 || count == argc) {
    if (count < 0 && -count != '-')
      fprintf(stderr, "Bad flag: %c\n", -count);
    showUsage(*argv);
  }

  /* This is only settable once */
  for (i = 0; devices[i].name; i++) {
    if (!strcasecmp(buf, devices[i].name)) {
      outputDev = i;
      break;
    }
  }

  devices[outputDev].infoFunc(&dCap);
  
  for (; count < argc; count++) {
    if (argv[count][0] == '-') {
      err = getargs(argc - count + 1, &argv[count - 1], ARGSTOGET);
      if (err < 0) {
	if (err < 0 && -err != '-')
	  fprintf(stderr, "Bad flag: %c\n", -err);
	showUsage(*argv);
      }
      count += err - 1;
    }
    
    if ((err = load_pic(argv[count], &myPic))) {
      fprintf(stderr, "Cannot view file %s\nError: %s\n", argv[count], picErrorStr(err));
      continue;
//      exit(1);
    }
    
    if (drawMono || (dCap.info.colorSpace & MONOMASK))
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

    if (acrop)
      autoCropImage(&myPic, 40);

    if (!dCap.canRotate && rotate)
      rotateImage(&myPic, (rotate * -3.141592) / 180.0);

    xd = xdim;
    yd = ydim;
    xs2 = xscale;
    ys2 = yscale;
    if (!dCap.dontScale) {
      if (!scaletofit && !(maxfit && (myPic.width * xscale > dCap.info.maxWidth || myPic.height * yscale > dCap.info.maxHeight))) {
	if (xd)
	  xs2 = (double) xd / (double) myPic.width;
	if (yd)
	  ys2 = (double) yd / (double) myPic.height;
      }
      else {
	if (!xd && !yd && dCap.info.maxWidth && dCap.info.maxHeight) {
	  xd = dCap.info.maxWidth;
	  yd = dCap.info.maxHeight;
	}
	if (!yd && xd) {
	  xs2 = (double) xd / ((double) myPic.width * xscale) * xscale;
	  ys2 = ((double) xd / ((double) myPic.width * xscale)) * yscale;
	}
	else if (yd) {
	  xs2 = ((double) yd / ((double) myPic.height * yscale)) * xscale;
	  ys2 = (double) yd / ((double) myPic.height * yscale) * yscale;
	  if (xd && (myPic.width * xs2) > xd) {
	    xs2 = (double) xd / ((double) myPic.width * xscale) * xscale;
	    ys2 = ((double) xd / ((double) myPic.width * xscale)) * yscale;
	  }
	}
      }
      if (!dCap.canScale || (blend && !dCap.canSmooth) || (ditherMode && !dCap.canDither)) {
	scaleImage(xs2, ys2, blend, &myPic);
	xs2 = ys2 = 1; /* Scaled it here */
      }
    }
    
    if (darken || contrast)
      adjustImage(&myPic, contrast, darken);
    if (negative)
      negateImage(&myPic);

    if (ditherMode && !dCap.canDither)
      handleDithering(ditherMode, &myPic, numBits && numBits <= dCap.info.maxBitsPerColor ? numBits : dCap.info.maxBitsPerColor, dCap.info.grayRamp);

    dCmd.keepHidden = keephidden;
    dCmd.ditherMode = ditherMode;
    dCmd.enqack = enqack;
    dCmd.charFile = charFile;
    dCmd.leadChar = leadChar;
    dCmd.lastChar = lastChar;
    dCmd.charWidth = mycw;
    dCmd.charHeight = mych;
    dCmd.columns = xdim;
    dCmd.rows = ydim;
    dCmd.scaleToFit = scaletofit;
    dCmd.xscale = xs2;
    dCmd.yscale = ys2;
    
    devices[outputDev].dispFunc(&myPic, &dCmd);

    freeImage(&myPic);
  }

  /* Signal no more pictures so appropiate routine can loop if necessary */

  devices[outputDev].dispFunc(NULL, &dCmd);
  
  exit(0);
  return 0;
}

void handleDithering(int ditherMode, image *aPic, int bits, int *grayRamp)
{
  switch(ditherMode) {
  case 'H':
  case 'h':
    halftoneImage(aPic, grayRamp, bits);
    break;

  case 'T':
  case 't':
    thresholdImage(aPic, grayRamp, bits);
    break;

  case 'D':
  case 'd':
    ditherImage(aPic, grayRamp, bits);
    break;
  }

  return;
}

