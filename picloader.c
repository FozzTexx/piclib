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
#include <string.h>

#include "piclib.h"
#include "picloader.h"

#if defined(NS_TARGET_MAJOR) && (NS_TARGET_MAJOR == 3)
#import "fmttiff.h"
#else
#include "fmtlibtiff.h"
#endif
#include "fmtgif.h"
#include "fmtIIgs.h"
#include "fmtast.h"
#include "fmthp150.h"
#include "fmtclip.h"
#include "fmtppm.h"
#include "fmtgrob.h"
#include "fmtjpeg.h"
#include "fmtppp.h"
#include "fmtSNES.h"
#include "fmtpng.h"

typedef struct filetypeidrec {
  char *IDBytes;
  int IDOffset;
  int FileSize;
  int (*loadFunc)();
  int (*infoFunc)();
} fileTypeID;

fileTypeID knownFileTypes[] = {
  {"MAIN", 5, 0, load_apf, info_apf},
  {"GIF", 0, 0, load_gif, info_gif},
  {"II*\000", 0, 0, load_tiff, info_tiff},
  {"MM\000*", 0, 0, load_tiff, info_tiff},
  {"", 0, 32768, load_gsbin, info_gsbin},
  {"\17\377\377\377\377\377\377\377\377\377", 34, 0, load_pplus, info_pplus},
  {"", 0, 96000, load_ast, info_ast},
  {"", 0, 24968, load_hp150, info_hp150},
  {"4\022", 0, 0, load_clip, info_clip},
  {"P6\n", 0, 0, load_ppm, info_ppm},
  {"P5\n", 0, 0, load_pgm, info_pgm},
  {"%%HP: ", 0, 0, load_grob, info_grob},
  {"HPHP48", 0, 0, load_grob, info_grob},
  {"\377\330\377", 0, 0, load_jpeg, info_jpeg},
  {"NOVA", 0, 0, load_ppp, info_ppp},
  {"SNS", 0, 0, load_snes, info_snes},
  {"\211PNG\r\n\032\n", 0, 0, load_png, info_png},
  {"", 0, 0, NULL, NULL},
};

int load_pic(const char *filename, image *aPic)
{
  FILE *picFile;
  char buf[512];
  int err;
  int i, j;
  int nuftoid;
  struct stat statbuf;

  
  for (i = 0, nuftoid = 0; knownFileTypes[i].loadFunc; i++) {
    j = strlen(knownFileTypes[i].IDBytes) + knownFileTypes[i].IDOffset;
    if (j > nuftoid)
      nuftoid = j;
  }
  
  if (!(picFile = fopen(filename, "r")))
    return PICERR_CANTOPEN;
  fread(buf, 1, nuftoid, picFile);
  fclose(picFile);

  stat(filename, &statbuf);

  for (i = 0; knownFileTypes[i].loadFunc; i++)
    if ((!knownFileTypes[i].IDBytes[0] && knownFileTypes[i].FileSize == statbuf.st_size) ||
	(knownFileTypes[i].IDBytes[0] && !strncmp(&buf[knownFileTypes[i].IDOffset], knownFileTypes[i].IDBytes, strlen(knownFileTypes[i].IDBytes)))) {
      err = knownFileTypes[i].loadFunc(filename, aPic);
      return err;
    }

  return PICERR_FORMATNOTSUPPORTED;
}

int getImageInfo(const char *filename, imageInfo *info)
{
  FILE *picFile;
  char buf[512];
  int err;
  int i, j;
  int nuftoid;
  struct stat statbuf;

  
  for (i = 0, nuftoid = 0; knownFileTypes[i].loadFunc; i++) {
    j = strlen(knownFileTypes[i].IDBytes) + knownFileTypes[i].IDOffset;
    if (j > nuftoid)
      nuftoid = j;
  }
  
  if (!(picFile = fopen(filename, "r")))
    return PICERR_CANTOPEN;
  fread(buf, 1, nuftoid, picFile);
  fclose(picFile);

  stat(filename, &statbuf);

  for (i = 0; knownFileTypes[i].loadFunc; i++)
    if ((!knownFileTypes[i].IDBytes[0] && knownFileTypes[i].FileSize == statbuf.st_size) ||
	(knownFileTypes[i].IDBytes[0] && !strncmp(&buf[knownFileTypes[i].IDOffset], knownFileTypes[i].IDBytes, strlen(knownFileTypes[i].IDBytes)))) {
      err = knownFileTypes[i].infoFunc(filename, info, NULL);
      return err;
    }

  return PICERR_FORMATNOTSUPPORTED;
}  
