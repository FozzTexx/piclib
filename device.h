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

#ifndef _DEVICE_H
#define _DEVICE_H

#include "piclib.h"

typedef struct _deviceCmd {
  int keepHidden;
  int ditherMode;
  int enqack;
  char *charFile;
  int leadChar;
  int lastChar;
  int charWidth;
  int charHeight;
  int columns;
  int rows;
  int scaleToFit;
  int smooth;
  double xscale, yscale;
  double rotate;
} deviceCmd;

typedef struct _deviceCap {
  formatInfo info;
  int canScale;
  int canSmooth;
  int canRotate;
  int canDither;
  int dontScale;
} deviceCap;

#endif /* _DEVICE_H */
