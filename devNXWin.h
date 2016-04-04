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

/* devNXWin.h generated from devNXWin.psw
   by unix pswrap V1.009  Wed Apr 19 17:50:24 PDT 1989
 */

#ifndef DEVNXWIN_H
#define DEVNXWIN_H

extern void getDepth(int *min, int *max, int *iscolor, int *width, int *height);

extern void initWindow(int x, int y, int width, int height, int lwin, int show, int *windownum);

extern void compositeImageMono(int width, int height);

extern void compositeImageColor(int width, int height);

extern void showWindow(int win1, int win2);

#endif DEVNXWIN_H
