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

#include <dpsclient/dpsclient.h>
#include <appkit/graphics.h>
#include <libc.h>
#include "piclib.h"
#include "devNeXT.h"
#include "devNXWin.h"

static DPSContext myctxt = NULL;
static int screenWidth = -1, screenHeight = -1;

int getevent();

void printjunk(DPSContext ctxt, const char *buf, long unsigned int count)
{
  unsigned char c;

  
  while (count--) {
    c = *buf++;
    if (c < ' ' && c != '\n') {putchar('^'); c ^= 0x40;}
    putchar(c);
  }
  putchar('\n');
}

void myerrorproc(DPSContext ctxt, DPSErrorCode errorCode, long unsigned int arg1, long unsigned int arg2)
{
  printf("Error %d",errorCode);
  printjunk(ctxt,(char *)arg1,(long unsigned)arg2);
}

int infoNeXT(deviceCap *cap)
{
  int min, max, iscolor;
  

  if (!myctxt)
    myctxt = DPSCreateContext(0, 0, printjunk, myerrorproc);
  
  getDepth(&min, &max, &iscolor, &screenWidth, &screenHeight);

  if (!max) {
    iscolor = YES;
    max = 8;
    min = 0;
  }
  
  cap->info.maxWidth = screenWidth;
  cap->info.maxHeight = screenHeight;
  cap->info.maxBitsPerPixel = iscolor ? max * 3 : max;
  cap->info.maxBitsPerColor = max;
  cap->info.colorSpace = iscolor ? COLORMASK : MONOMASK;
  cap->info.grayRamp = NULL;

  cap->canScale = YES;
  cap->canSmooth = NO;
  cap->canRotate = NO;
  cap->canDither = NO;
  cap->dontScale = NO;

  return PICERR_NOERR;
}

int dispNeXT(image *aPic, deviceCmd *cmd)
{
  static int openWins = 0;
  static lx = 10, ly = 10, by = 0;
  static int window = 0;
  int lwin = 0;
  NXRect aRect;
  int spp;
  int config;
  int min, max, iscolor;
  

  if (screenWidth < 0 || screenHeight < 0)
    getDepth(&min, &max, &iscolor, &screenWidth, &screenHeight);
    
  if (!aPic) { /* No more pictures. Loop for dragging */
    while (openWins)
      if (lwin = getevent()) {
	DPSPrintf(myctxt,"%d termwindow nulldevice\n", lwin);
	DPSFlushContext(myctxt);
	openWins--;
      }
    
    if (myctxt)
      DPSDestroyContext(myctxt);

    return PICERR_NOERR;
  }

  if (!myctxt) {
    myctxt = DPSCreateContext(0, 0, printjunk, myerrorproc);
    DPSPrintf(myctxt, "currentcontext setactiveapp\n");
  }

  if ((lx + (aPic->width * cmd->xscale) + 2) > (screenWidth - 72)) {
    lx = 10;
    ly += by + 23;
    if (ly + (aPic->height * cmd->yscale) + 24 > (screenHeight - 10))
      ly = 10;
    by = 0;
  }

  lwin = window;
  initWindow(lx, ly, (aPic->width * cmd->xscale), (aPic->height * cmd->yscale), lwin, !cmd->keepHidden, &window);

  aRect.origin.x = 0;
  aRect.origin.y = 0;
  aRect.size.width = (int) (aPic->width * cmd->xscale);
  aRect.size.height = (int) (aPic->height * cmd->yscale);
  spp = aPic->photoInterp & COLORMASK ? 3 : 1;
  config = aPic->photoInterp & COLORMASK ? NX_MESHED : NX_PLANAR;
  
  NXDrawBitmap(&aRect, aPic->width, aPic->height,
	       8, spp, spp * 8, spp * aPic->width, config == NX_PLANAR, NO,
	       aPic->photoInterp, &aPic->data);
  
  DPSFlushContext(myctxt);
  if (cmd->keepHidden && lwin) {
    if (!cmd->enqack) {
      for (;;)
	if (getevent())
	  break;
    }
    else
      sleep(cmd->enqack);
  }
  
  showWindow(window, lwin);
  DPSFlushContext(myctxt);

  lx += (aPic->width * cmd->xscale) + 1;
  if ((aPic->height * cmd->yscale) > by)
    by = (aPic->height * cmd->yscale);

  openWins++;

  if (cmd->keepHidden) {
    if (lwin) {
      DPSFlushContext(myctxt);
      DPSPrintf(myctxt, "%d termwindow nulldevice\n", lwin);
      openWins--;
      DPSStartWaitCursorTimer();
      DPSFlushContext(myctxt);
    }

    lx = ly = 10;
  }

  return PICERR_NOERR;
}

int getevent()
{
  NXEvent event;


  DPSFlushContext(myctxt);
  DPSGetEvent(myctxt,&event,-1,NX_FOREVER,0);
  if (event.type == NX_RMOUSEDOWN) {
    return event.window;
  }
  else if (event.type == NX_LMOUSEDOWN) {
    if (event.flags & NX_COMMANDMASK)
      DPSPrintf(myctxt, "-1 0 %d orderwindow\n",
		event.window);
    else
      DPSPrintf(myctxt,
		"1 0 %d orderwindow\n"
		"{%d stilldown not {exit} if\n"
		"deskWin currentmouse\n"
		"deskWin currentwindowbounds pop pop\n"
		"3 -1 roll add 3 1 roll add exch\n"
		"%g sub exch %g sub exch\n"
		"%d movewindow} loop\n",
		event.window, event.data.mouse.eventNum,
		event.location.y, event.location.x,
		event.window);
  }

  return 0;
}
