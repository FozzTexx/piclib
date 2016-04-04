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
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#ifdef linux
#include <sys/types.h>
#include <unistd.h>
#else
#include <libc.h>
#endif
#include "piclib.h"
#include "devXerox.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define BLACK 0
#define WHITE 1

#define lgetpxl(x,y,col,pic) (getval(x, y, col, pic) > 127)

void set_color(int col);
static inline void print_image(image *CLayer, image *MLayer, image *YLayer, image *KLayer);


static inline void myputc(int dev, char c)
{
  int i = 0;


  while (!(i & TIOCM_CD)) {
    for (i = 0; i < 3072; i++);
    ioctl(dev, TIOCMGET, &i);
  }

  write(dev, &c, 1);

  return;
}

static inline void myputs(int dev, char *s)
{
  int i;


  for (i = 0; s[i]; i++)
    myputc(dev, s[i]);

  return;
}

static inline void myputline(int dev, char color, unsigned char *rowbytes, int wpl)
{
  int i, j;
  char buf[256];
  
  
  for (i = wpl - 1; !rowbytes[i] && i >= 0; i--);
  sprintf(buf,  "\033g%c%03i,", color, i + 1);
  myputs(dev, buf);
  
  for (j = 0; j <= i; j++)
    myputc(dev, rowbytes[j]);

  return;
}
	    
static inline void print_image(image *CLayer, image *MLayer, image *YLayer, image *KLayer)
{
  int printdev;
  int col_on;
  int i, j, k;
  int byte, byteWidth;
#ifndef linux
  struct sgttyb ttyprm;
#endif
  static char colors[] = "<=>?456789:;0123";
  unsigned char *rowbytes;
  int wpl = (CLayer->width + 7) / 8;
  

  printdev = open("/dev/cub", O_WRONLY);
#ifndef linux
  ioctl(printdev, TIOCGETP, &ttyprm);
  ttyprm.sg_ispeed = B9600;
  ttyprm.sg_ospeed = B9600;
  ttyprm.sg_flags = 3;
  ioctl(printdev, TIOCSETP, &ttyprm);
  j = 512;
  ioctl(printdev, TIOCLBIS, &j);
  j = 65023;
  ioctl(printdev, TIOCLBIC, &j);
#endif

  byteWidth = wpl * 8;

  rowbytes = malloc(wpl);
  
  myputs(printdev, "\033\015P\033we\033\\");
  
  for (j = 0; j < CLayer->height; j += 4) {
    fprintf(stderr, "\010\010\010\010%4i", j);
    col_on = 2;
    for (k = j; k < j + 4; k++) {
	for (byte = 0, i = 0; i < byteWidth; i++) {
	  rowbytes[i / 8] = 0;
	  byte <<= 1;
	  byte |= lgetpxl(i, k, MONO, YLayer);
	  if (i % 8 == 7) {
	    rowbytes[i / 8] = byte;
	    byte = 0;
	  }
	}
	myputline(printdev, colors[col_on * 4 + (k % 4)], rowbytes, wpl);
      }
    
/*    myputc(printdev, 27);
    myputc(printdev, 'k');
    myputc(printdev, '0'); */
    
    col_on = 1;
    for (k = j; k < j + 4; k++) {
	for (byte = 0, i = 0; i < byteWidth; i++) {
	  rowbytes[i / 8] = 0;
	  byte <<= 1;
	  byte |= lgetpxl(i, k, MONO, MLayer);
	  if (i % 8 == 7) {
	    rowbytes[i / 8] = byte;
	    byte = 0;
	  }
	}
	myputline(printdev, colors[col_on * 4 + (k % 4)], rowbytes, wpl);
      }

/*    myputc(printdev, 27);
    myputc(printdev, 'k');
    myputc(printdev, '0'); */
    
    col_on = 0;
    for (k = j; k < j + 4; k++) {
	for (byte = 0, i = 0; i < byteWidth; i++) {
	  rowbytes[i / 8] = 0;
	  byte <<= 1;
	  byte |= lgetpxl(i, k, MONO, CLayer);
	  if (i % 8 == 7) {
	    rowbytes[i / 8] = byte;
	    byte = 0;
	  }
	}
	myputline(printdev, colors[col_on * 4 + (k % 4)], rowbytes, wpl);
      }

/*    myputc(printdev, 27);
    myputc(printdev, 'k');
    myputc(printdev, '0'); */
    
    col_on = 3;
    for (k = j; k < j + 4; k++) {
	for (byte = 0, i = 0; i < byteWidth; i++) {
	  rowbytes[i / 8] = 0;
	  byte <<= 1;
	  byte |= lgetpxl(i, k, MONO, KLayer);
	  if (i % 8 == 7) {
	    rowbytes[i / 8] = byte;
	    byte = 0;
	  }
	}
	myputline(printdev, colors[col_on * 4 + (k % 4)], rowbytes, wpl);
      }

    myputc(printdev, 27);
    myputc(printdev, 'k');
    myputc(printdev, '2');

    myputs(printdev, "\033wB");
    
    myputc(printdev, 27);
    myputc(printdev, 'k');
    myputc(printdev, '1');
  }

  close(printdev);
  
  return;
}

#define XEROX_SCALE	(120.0 / 72.0)

int infoXerox(deviceCap *cap)
{
  cap->info.maxWidth = NOMAX; /* FIXME */
  cap->info.maxHeight = NOMAX; /* FIXME */
  cap->info.maxBitsPerPixel = 24;
  cap->info.maxBitsPerColor = 8;
  cap->info.colorSpace = COLORMASK;
  cap->info.grayRamp = NULL;

  cap->canScale = NO;
  cap->canSmooth = NO;
  cap->canRotate = NO;
  cap->canDither = NO;
  cap->dontScale = NO;

  return PICERR_NOERR;
}

int dispXerox(image *aPic, deviceCmd *cmd)
{
  image CLayer, MLayer, YLayer, KLayer;
  int i, j, c, m, y, k;
  static int mapping[] = {
      0,   1,   2,   3,  23,  32,  45,  51,  54,  57,  59,  61,  63,  65,  67,  68,  70,
     71,  72,  73,  74,  75,  76,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,
     89,  90,  91,  94,  97, 100, 102, 104, 105, 107, 108, 110, 111, 112, 113, 114, 116,
    117, 118, 119, 120, 121, 122, 123, 123, 124, 125, 126, 127, 127, 128, 129, 129, 130,
    130, 131, 132, 132, 133, 134, 134, 135, 136, 136, 137, 138, 139, 140, 141, 141, 142,
    143, 144, 144, 145, 146, 146, 147, 148, 148, 149, 149, 150, 150, 151, 151, 152, 152,
    153, 153, 154, 154, 155, 155, 156, 157, 157, 158, 158, 159, 159, 160, 160, 161, 161,
    162, 162, 163, 163, 164, 164, 165, 165, 166, 166, 167, 167, 168, 168, 169, 169, 170,
    170, 171, 171, 172, 173, 173, 174, 175, 175, 176, 177, 178, 178, 179, 179, 180, 181,
    181, 182, 183, 183, 184, 184, 185, 186, 186, 187, 187, 188, 188, 189, 190, 190, 191,
    192, 192, 193, 193, 194, 195, 195, 196, 196, 197, 197, 198, 198, 199, 199, 200, 200,
    201, 201, 202, 202, 203, 203, 204, 204, 205, 205, 206, 207, 207, 208, 208, 209, 210,
    210, 211, 211, 212, 213, 214, 214, 215, 215, 216, 217, 217, 218, 219, 219, 220, 221,
    222, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237,
    238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
    255,
  };


  if (!aPic)
    return PICERR_NOERR;
  
  for (i = 0; i < 256; i++)
    mapping[i] = 255 - mapping[i];
  for (i = 0; i < 256 / 2; i++) {
    j = mapping[i];
    mapping[i] = mapping[255 - i];
    mapping[255 - i] = j;
  }
  
  fprintf(stderr, "Scaling...");
  scaleImage(XEROX_SCALE, XEROX_SCALE, NO, aPic);
  fprintf(stderr, "...done\n");
  
  if (aPic->photoInterp & COLORMASK) {
    fprintf(stderr, "Separating...");
    allocateImage(&CLayer, MONOMASK, aPic->width, aPic->height);
    allocateImage(&MLayer, MONOMASK, aPic->width, aPic->height);
    allocateImage(&YLayer, MONOMASK, aPic->width, aPic->height);
    allocateImage(&KLayer, MONOMASK, aPic->width, aPic->height);
    fprintf(stderr, " Line: 0000");
    for (j = 0; j < aPic->height; j++) {
      fprintf(stderr, "\010\010\010\010%4i", j);
      for (i = 0; i < aPic->width; i++) {
	c = 255 - getval(i, j, RED, aPic);
	m = 255 - getval(i, j, GREEN, aPic);
	y = 255 - getval(i, j, BLUE, aPic);
/*	if (c == m && c == y) {
	  putval(i, j, 0, MONO, &CLayer);
	  putval(i, j, 0, MONO, &MLayer);
	  putval(i, j, 0, MONO, &YLayer);
	  putval(i, j, c, MONO, &KLayer);
	}
	else {
	  putval(i, j, (c * 90) / 100, MONO, &CLayer);
	  putval(i, j, (m * 90) / 100, MONO, &MLayer);
	  putval(i, j, y, MONO, &YLayer);
	  putval(i, j, 0, MONO, &KLayer);
	} */

	/* I found that separating black from every pixel gives a much
	   sharper image. The colors aren't as smooth, but it does
	   look better. */
	k = c;
	if (m < k)
	  k = m;
	if (y < k)
	  k = y;
	c -= k;
	m -= k;
	y -= k;
	putval(i, j, c, MONO, &CLayer);
	putval(i, j, m, MONO, &MLayer);
	putval(i, j, y, MONO, &YLayer);
	putval(i, j, k, MONO, &KLayer);
      }
    }
    fprintf(stderr, "...done\nDithering...");
    ditherImage(&CLayer, mapping, 1);
    fprintf(stderr, "C");
    ditherImage(&MLayer, mapping, 1);
    fprintf(stderr, "M");
    ditherImage(&YLayer, mapping, 1);
    fprintf(stderr, "Y");
    ditherImage(&KLayer, mapping, 1);
    fprintf(stderr, "K...done\nPrinting... Line: 0000");
    print_image(&CLayer, &MLayer, &YLayer, &KLayer);
    fprintf(stderr, "...done\n");
  }
  else {
  }

  return PICERR_NOERR;
}
