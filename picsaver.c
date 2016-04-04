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
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>

#include "piclib.h"
#include "picsaver.h"

#if defined(NS_TARGET_MAJOR) && (NS_TARGET_MAJOR == 3)
#import "fmttiff.h"
#else
#include "fmtlibtiff.h"
#endif
#include "fmtgif.h"
#include "fmtIIgs.h"
#include "fmtast.h"
#include "fmthp150.h"
#include "fmtppm.h"
#include "fmtgrob.h"
#include "fmtSNES.h"
#include "fmtjpeg.h"
#include "fmtpng.h"
#include "fmtIIhires.h"

typedef struct filetypeidrec {
  int (*saveFunc)();
  int (*infoFunc)();
} fileTypeID;

fileTypeID saveFileTypes[] = {
  {save_gif, info_gif},
  {save_tiff, info_tiff},
  {save_ast, info_ast},
  {save_hp150, info_hp150},
  {save_ppm, info_ppm},
  {save_pgm, info_pgm},
  {save_grob, info_grob},
  {save_snes, info_snes},
  {save_jpeg, info_jpeg},
  {save_png, info_png},
  {save_IIhires, info_IIhires},
  {NULL, NULL},
};

int save_pic(const char *filename, const char *format, image *aPic)
{
  int err;
  int i;
  formatInfo info;
  

  for (i = 0; saveFileTypes[i].saveFunc; i++)
    if (!saveFileTypes[i].infoFunc(NULL, NULL, &info))
      if (!strcasecmp(info.extension, format) || !strcasecmp(info.type, format)) {
	err = saveFileTypes[i].saveFunc(filename, aPic);
	return err;
      }

  return PICERR_FORMATNOTSUPPORTED;
}

int infoForFormat(const char *format, formatInfo *info)
{
  int i;
  

  for (i = 0; saveFileTypes[i].saveFunc; i++)
    if (!saveFileTypes[i].infoFunc(NULL, NULL, info))
      if (!strcasecmp(info->extension, format) || !strcasecmp(info->type, format))
	return PICERR_NOERR;

  return PICERR_FORMATNOTSUPPORTED;
}
