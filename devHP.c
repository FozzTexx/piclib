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
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#ifndef linux
#include <libc.h>
#endif

#include "devHP.h"

#undef RANDOMIZE

#define TERMSCREEN stdout

#define lgetpxl(x,y,col,pic) (getval(x, y, col, pic) > 127)

#define ESC "\033"
#define HP_clearscreen()	tprintf(ESC "*daC");
#define HP_setcolor(x)		tprintf(ESC "*m5a%dX", x)
#define HP_line(x1,y1,x2,y2)	tprintf(ESC "*pa%d %d %d %dZ", x1, 389 - (y1), x2, 389 - (y2))
#define HP_rect(x1,y1,x2,y2)	({int _x1 = x1, _y1 = y1, _x2 = x2, _y2 = y2; _x1 == _x2 ? \
				    HP_line(_x1, _y1, _x2, _y2) : \
				      tprintf(ESC "*m%d %d %d %dE", _x1, 389 - _y2, _x2, 389 - _y1);})
#define HP_defpat(x)		tprintf(ESC "*m%d %d %d %d %d %d %d %d D", x[7], x[6], x[5], x[4], x[3], x[2], x[1], x[0])

#define drawn(x, y, w, d) ({int _x = x, _y = y, _w = w; (d[_x / 8 + _y * _w] >> (7 - _x % 8)) & 1;})

static int doenqack = 0;
static int thermal = NO;
#ifndef linux
struct sgttyb old_tty;
#endif

static inline void enqack()
{
  int c, i;
  char buf[64];


  fputc(5, TERMSCREEN);
    
  c = fgetc(stdin);
  i = 0;
  while (c != 6) {
    buf[i] = c;
    i++;
    if (i == 4 && buf[0] == 27 && buf[1] == '*' && buf[2] == 'd')
      if (buf[3] == 'C' || buf[3] == 'D' || buf[3] == 'E' || buf[3] == 'F' || buf[3] == 'K' || buf[3] == 'L') {
	fprintf(TERMSCREEN, ESC "*d%c", buf[3]);
	break;
      } 
    c = fgetc(stdin);
  }
  
  return;
}

int tprintf(char *format, ...)
{
  char buf[256];
  va_list arg;
  int i;
  static int nb = 0;
  

  va_start(arg, format);
  vsprintf(buf, format, arg);

  for (i = 0; buf[i]; i++) {
    fputc(buf[i], TERMSCREEN);
    if (doenqack && (++nb == doenqack)) {
      nb = 0;
      enqack();
    }
  }

  return i;
}

int HP_iscolorterm()
{
#ifndef linux
  char termtype[20];
  struct sgttyb newstate, oldstate;
#endif
  int color = 0;


#ifndef linux
  if (isatty(fileno(TERMSCREEN))) {
    ioctl(fileno(TERMSCREEN), TIOCGETP, &oldstate);
    newstate = oldstate;
    newstate.sg_flags = CBREAK | CRMOD;
    ioctl(fileno(TERMSCREEN), TIOCSETP, &newstate);
    
    printf(ESC "*s^\021");
    fgets(termtype, 20, stdin);
    if (!strncmp("2627", termtype, 4))
      color = 1;

    ioctl(fileno(TERMSCREEN), TIOCSETP, &oldstate);
  }
  else
    color = 1;
#endif

  return color;
}
    
static inline int rect_at(int x1, int y1, int *x2, int *y2, int col, image *aPic, int sw, int sh)
{
  int i2, j2, j3;
  int bx, by;
  int ph = (sh / 8) * 8 + 8 * (sh % 8 > 0);
  int pw = (sw / 8) * 8 + 8 * (sw % 8 > 0);


  for (j3 = y1; j3 < ph; j3++)
    if (lgetpxl(x1, j3, col, aPic) !=
	lgetpxl(x1, y1 + ((j3 - y1) % 8), col, aPic))
      break;
  j3--;
  by = j3;
  bx = x1;
  for (i2 = x1 + 1; i2 < pw; i2++) {
    for (j2 = y1; j2 <= j3; j2++)
      if (lgetpxl(i2, j2, col, aPic) !=
	  lgetpxl(x1 + ((i2 - x1) % 8), y1 + ((j2 - y1) % 8), col, aPic))
	break;
    j2--;
    if ((i2 - x1 + 1) * (j2 - y1 + 1) >= (bx - x1 + 1) * (by - y1 + 1))
      by = j2, bx = i2;
    j3 = j2;
  }
  i2 = bx;
  j3 = by;
  if ((i2 - x1 + 1) * (j3 - y1 + 1) < 64) {
    i2 = x1 + 7;
    j3 = y1 + 7;
  }
  *x2 = i2;
  *y2 = j3;

  return(i2 - x1 + 1) * (j3 - y1 + 1);
}
	
static inline void clear_pixels(int x1, int y1, int x2, int y2, int col,
			  unsigned char *displayed, image *aPic)
{
  int i, j, k;
  int wpl = (aPic->width + 7) / 8;
  

  for (i = x1; i <= x2 && i < aPic->width; i++)
    for (j = y1; j <= y2 && j < aPic->height; j++) {
      k = !lgetpxl(i, j, col, aPic) |
	lgetpxl(x1 + (i - x1) % 8, y1 + (j - y1) % 8, col, aPic);
      displayed[i / 8 + j * wpl] |= k << (7 - i % 8);
    }
}

static inline void define_pattern
  (int x, int y, int col, image *aPic, int xd, int yd, unsigned char *patbuf)
{
  int i, j, k;
  int pb[8];


  for (i = 0; i < 8; i++)
    pb[i] = 0;

  for (i = y; i < y + 8; i++)
    for (j = x; j < x + 8; j++) {
      pb[i - y] <<= 1;
      if (j < aPic->width && i < aPic->height) 
	pb[i - y] |= lgetpxl(j, i, col, aPic);
    }

  /* Pattern is relative to 0,0 on the graphics screen,
     not 0,0 on the rectangle we are about to fill,
     so we may need to roll the pattern */
  
  if ((yd + 2) % 8)
    for (j = 0; j < (yd + 2) % 8; j++) {
      k = pb[7];
      for (i = 7; i > 0; i--)
	pb[i] = pb[i-1];
      pb[0] = k;
    }

  if (xd % 8)
    for (j = 0; j < xd % 8; j++)
      for (k = 0; k < 8; k++)
	pb[k] = ((pb[k] >> 1) | ((pb[k] << 7) & 0x80)) & 0xff;

  for (i = 0; i < 8; i++)
    patbuf[i] = pb[i];

  return;
}

static inline void int_hand() /* cleanup on ^C */
{
  fflush(TERMSCREEN);
#ifndef linux
  ioctl(fileno(TERMSCREEN), TIOCSETP, &old_tty);
#endif
  exit(1);
}

static inline int output_to_hpterm(image *aPic, int keephidden)
{
  int col_on;
  int i, j, i2, j3;
  int k, l, m, n;
  unsigned char *displayed;
  int uh, uw;
  int first, last;
  static int osx = 512, osy = 390, hil = 0;
  int rl;
  int rh, rw;
  unsigned char *pat1, *pat2;
  unsigned char *pbuf;
  int ofs, lfs;
  int offx, offy;
  int wpl = (aPic->width + 7) / 8;
  

  uw = aPic->width;
  uh = aPic->height;
  if (uw > 512)
    uw = 512;
  if (uh > 390)
    uh = 390;

  if (osx + uw > 512) {
    osx = 0;
    osy += hil;
    hil = 0;
  }
  if (osy + uh > 390) {
    osy = 0;
    osx = 0;
    hil = 0;
    HP_clearscreen();
    tprintf(ESC "H" ESC "J" ESC "&a0c23Y");
  }

  if (keephidden)
    tprintf(ESC "*dD");
  
  tprintf(ESC "*m255 255 255 255 255 255 255 255d2g3B");

  if (!(displayed = (unsigned char *) malloc(wpl * aPic->height)))
    return PICERR_CANTALLOC;

  if (aPic->photoInterp & COLORMASK) {
    first = RED;
    last = BLUE;
  }
  else
    first = last = MONO;

  rw = (uw + 7) / 8;
  rh = (uh + 7) / 8;
  if (!(pbuf = malloc(rw * rh * 8)))
    return PICERR_CANTALLOC;
  
  for (col_on = first; col_on <= last; col_on ++) {
    HP_setcolor(col_on != MONO ? 1 << col_on : 7);
    for (i = 0; i < wpl; i++)
      for (j = 0; j < uh; j++)
	displayed[i + j * wpl] = 0;

    for (i = 0; i < rw; i++)
      for (j = 0; j < rh; j++)
	define_pattern(i * 8, j * 8, col_on, aPic,
			i * 8 + osx, j * 8 + osy, &pbuf[i * 8 + j * rw * 8]);

    ofs = 64;
    lfs = 0;
    offx = offy = 0;
    i = ofs * -8;
    j = 0;
    for (rl = rw * rh; rl;) {
      do {
#ifdef RANDOMIZE
	i = (random() % rw) * 8;
	j = (random() % rh) * 8;
#else
	i += ofs * 8;
	if (i > uw) {
	  j += ofs * 8;
	  i = (ofs == lfs ? (ofs * 8) / 2 : 0) + offx;
	  if (j > uh) {
	    if (lfs == ofs) {
	      ofs /= 2;
	      if (ofs == 1) {
		ofs = 64;
		lfs = 64;
		if (offx == offy)
		  offx -= 8;
		else if (offx < offy)
		  offx += 8, offy -= 8;
		else if (offx > offy)
		  offx -= 8;
	      }
	    }
	    else
	      lfs = ofs;
	    j = ofs == lfs ? (ofs * 8) / 2 : 0 + offy;
	    i = (ofs == lfs ? (ofs * 8) / 2 : 0) + offx;
	  }
	}
#endif
      } while (drawn(i, j, wpl, displayed));

      pat1 = &pbuf[(i / 8) * 8 + (j / 8) * rw * 8];
      HP_defpat(pat1);
	
      for (k = 0; k < uw; k += 8)
	for (l = 0; l < uh; l += 8) {
	  pat2 = &pbuf[(k / 8) * 8 + (l / 8) * rw * 8];
	  if (!drawn(k, l, wpl, displayed) && !memcmp(pat1, pat2, 8)) {
	    rect_at(k, l, &i2, &j3, col_on, aPic, uw, uh);
	    if ((((i2 + 1) / 8) * 8 - 1) - k <= 0 ||
		(((j3 + 1) / 8) * 8 - 1) - l <= 0)
	      i2 = k + 7, j3 = l + 7;
	    HP_rect(k + osx, l + osy, i2 + osx, j3 + osy);
	    i2 = ((i2 + 1) / 8) * 8 - 1;
	    j3 = ((j3 + 1) / 8) * 8 - 1;
	    for (m = k; m < i2; m += 8)
	      for (n = l; n < j3; n += 8) 
		rl -= !drawn(m, n, wpl, displayed);
	    clear_pixels(k, l, i2, j3, col_on, displayed, aPic);
	  }
	}
    }
  }

  free(pbuf);
  free(displayed);  

  osx += uw;
  if (uh > hil)
    hil = uh;

  fflush(TERMSCREEN);
  
  return PICERR_NOERR;
}

int dispHP(image *aPic, deviceCmd *devCmd)
{
#ifndef linux
  struct sgttyb newstate;
#endif
  

  if (!aPic) /* No more pictures, just return */
    return PICERR_NOERR;
  
  doenqack = devCmd->enqack;

  setbuf(TERMSCREEN, NULL);
  
  if (doenqack) {
    signal(SIGINT, (void *) &int_hand);

#ifndef linux
    ioctl(fileno(TERMSCREEN), TIOCGETP, &old_tty);

    newstate = old_tty;
    newstate.sg_flags = CBREAK;
    ioctl(fileno(TERMSCREEN), TIOCSETP, &newstate);
#endif
  }
  
  output_to_hpterm(aPic, devCmd->keepHidden);

#ifndef linux
  if (doenqack)
    ioctl(fileno(TERMSCREEN), TIOCSETP, &old_tty);
#endif
  
  return PICERR_NOERR;
}

#define THERMAL_SCALE (89.0 / 72.0)

int dispHPthermal(image *aPic, deviceCmd *devCmd)
{
  if (!aPic)
    return PICERR_NOERR;
  
  negateImage(aPic);
  makeMono(aPic);
  
  thermal = YES;

  scaleImage(THERMAL_SCALE, THERMAL_SCALE, NO, aPic);
  
  return dispHP(aPic, devCmd);
}

int infoHPthermal(deviceCap *cap)
{
  static int thermalmap[] = {
    0,   1,   2,   3, 112, 115, 118, 120, 123, 124, 126, 127, 128, 129, 130,
    131, 132, 133, 133, 134, 135, 135, 136, 136, 137, 137, 138, 138, 139, 139,
    140, 140, 140, 141, 141, 141, 142, 142, 142, 143, 143, 143, 143, 144, 144,
    144, 144, 145, 145, 145, 146, 146, 146, 147, 147, 147, 148, 148, 148, 149,
    149, 149, 150, 150, 150, 151, 151, 151, 151, 152, 152, 152, 153, 153, 153,
    154, 154, 154, 155, 155, 155, 155, 156, 156, 156, 156, 157, 157, 157, 157,
    158, 158, 158, 158, 159, 159, 159, 159, 160, 160, 160, 160, 160, 161, 161,
    161, 161, 162, 162, 162, 162, 163, 163, 163, 163, 164, 164, 164, 165, 165,
    165, 165, 166, 166, 166, 167, 167, 167, 168, 168, 168, 169, 169, 169, 170,
    170, 170, 171, 171, 171, 172, 172, 172, 173, 173, 173, 173, 174, 174, 174,
    175, 175, 175, 175, 176, 176, 176, 177, 177, 177, 178, 178, 178, 178, 179,
    179, 179, 180, 180, 180, 181, 181, 181, 182, 182, 182, 183, 183, 184, 184,
    184, 185, 185, 186, 186, 186, 186, 187, 188, 189, 188, 189, 189, 190, 190,
    190, 191, 191, 191, 192, 192, 193, 193, 193, 194, 194, 195, 195, 196, 196,
    197, 197, 198, 198, 199, 200, 200, 201, 202, 203, 203, 204, 205, 205, 206,
    207, 207, 208, 209, 209, 210, 211, 212, 213, 213, 214, 215, 217, 218, 219,
    220, 222, 223, 225, 227, 228, 230, 232, 234, 236, 250, 240, 252, 253, 254,
    255,
  };


  cap->info.maxWidth = 512 / THERMAL_SCALE;
  cap->info.maxHeight = 390 / THERMAL_SCALE;
  cap->info.maxBitsPerPixel = 1;
  cap->info.maxBitsPerColor = 1;
  cap->info.colorSpace = MONOMASK;
  cap->info.grayRamp = thermalmap;

  cap->canScale = NO;
  cap->canSmooth = NO;
  cap->canRotate = NO;
  cap->canDither = NO;
  cap->dontScale = NO;

  return PICERR_NOERR;
}

int infoHP(deviceCap *cap)
{
  cap->info.maxWidth = 512;
  cap->info.maxHeight = 390;
  cap->info.maxBitsPerPixel = 1;
  cap->info.maxBitsPerColor = 1;

  cap->info.grayRamp = NULL;

  if (HP_iscolorterm())
    cap->info.colorSpace = COLORMASK;
  else
    cap->info.colorSpace = MONOMASK;      

  cap->canScale = NO;
  cap->canSmooth = NO;
  cap->canRotate = NO;
  cap->canDither = NO;
  cap->dontScale = NO;

  return PICERR_NOERR;
}
