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

#ifndef _PICLIB_H
#define _PICLIB_H
#ifndef linux
#ifndef MAX
#define MAX(A,B) ((A) > (B) ? (A) : (B))
#endif
#ifndef MIN
#define MIN(A,B) ((A) < (B) ? (A) : (B))
#endif
#else /* linux */
#include <sys/param.h>
#endif
#define CLIP(A,C,B) (MAX(MIN(A,B),C))

#define MV 255

#ifndef NO
#define NO	0
#endif
#ifndef YES
#define YES	1
#endif

#define RED	0
#define GREEN	1
#define BLUE	2
#define ALPHA	3
#define MONO	4

#define MONOMASK	1
#define COLORMASK	2
#define CMYKMASK	4
#define PALETTEMASK	8
#define ALPHAMASK      16

#define MESHED 1
#define PLANAR 2

typedef enum {
  PICERR_NOERR = 0,
  PICERR_BADFORMAT,
  PICERR_IMAGENOTFOUND,
  PICERR_CANTALLOC,
  PICERR_FORMATNOTSUPPORTED,
  PICERR_IOERROR,
  PICERR_COMPRESSIONNOTSUPPORTED,
  PICERR_CANTAPPEND,
  PICERR_CANTOPEN,
} PICERRS;

#define NOMAX	0

typedef unsigned char byte;

typedef struct _image {
  int photoInterp;
  int width, height;
  int samplesPerPixel;
  unsigned char *data;
} image;

typedef struct _imageInfo {
  char *type;			/* Long name of file format */
  char *extension;		/* Common file extension */
  int photoInterp;
  int planarConfig;
  int width, height;
  int bitsPerPixel;		/* Bits Per Pixel in data */
  int bitsPerColor;		/* Bits Per Color plane */
  int numColors;		/* Total number of colors in palette */
} imageInfo;

typedef struct _formatInfo {
  char *type;
  char *extension;
  int maxWidth, maxHeight;
  int maxBitsPerPixel;
  int maxBitsPerColor;
  int colorSpace;		/* Mask of best color space */
  int *grayRamp;
} formatInfo;

static inline int getval(int x, int y, int p, image *a)
{
  if (p > a->samplesPerPixel)
    p = 0;
  if (x >= 0 && y >= 0 && x < a->width && y < a->height)
    return a->data[(x + y * a->width) * a->samplesPerPixel + p];
  return 0;
}

static inline void putval(int x, int y, int v, int p, image *a)
{
  if (p > a->samplesPerPixel)
    p = 0;
  if (x >= 0 && y >= 0 && x < a->width && y < a->height)
    a->data[(x + y * a->width) * a->samplesPerPixel + p] = v;
  return;
}

int ditherImage(image *aPic, int *mapping, int bits);
int thresholdImage(image *aPic, int *mapping, int bits);
int halftoneImage(image *aPic, int *mapping, int bits);
void negateImage(image *aPic);
void scaleImage(double xpercent, double ypercent, int blend, image *aPic);
void resizeImage(unsigned int width, unsigned int height, int blend, image *aPic);
void rotateImage(image *aPic, double radians);
void makeMono(image *aPic);
void cropImage(int x1, int y1, int x2, int y2, image *aPic);
void autoCropImage(image *aPic, int margin);
void adjustImage(image *aPic, double contrast, int brightness);
void enhanceImage(image *aPic, int contrast);
void allocateImage(image *aPic, int photoInterp, int width, int height);
int copyImage(image *dest, image *source);
void freeImage(image *aPic);

int constructImage(image *aPic, int photoInterp, int planarConfig, int width, int height, int bpp, byte *data, byte *palette);
int unconstructImage(image *aPic, int photoInterp, int planarConfig, int minbpp,
		     int *bpp, byte **data, byte **palette);

char *picErrorStr(int err);

#endif /* _PICLIB_H */
