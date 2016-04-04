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
#include <stdlib.h>
#include <string.h>
#include "fmtppm.h"

int load_ppm(char *filename, image *aPic)
{
  FILE *file;
  char buf[256];
  int width, height, depth;
  int i;
  byte *data;
  

  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fgets(buf, sizeof(buf), file);
  if (strcmp(buf, "P6\n"))
    return PICERR_BADFORMAT;

  fscanf(file, "%i %i\n%i\n", &width, &height, &depth);

  for (i = 1; (1 << i) < depth; i++);
  
  data = malloc(width * height * 3);
  fread(data, width * height * 3, 1, file);
  fclose(file);
  constructImage(aPic, COLORMASK, MESHED, width, height, i, data, NULL);
  free(data);
  
  return PICERR_NOERR;
}

int save_ppm(char *filename, image *aPic)
{
  FILE *file;


  if (!(file = fopen(filename, "w")))
    return PICERR_CANTOPEN;

  fprintf(file, "P6\n%i %i\n%i\n", aPic->width, aPic->height, 255);
  fwrite(aPic->data, aPic->width * aPic->height * 3, 1, file);
  fclose(file);

  return PICERR_NOERR;
}

int info_ppm(char *filename, imageInfo *info, formatInfo *fInfo)
{
  int width, height, depth;
  FILE *file;
  char buf[256];
  int i;

  
  if (!filename) {
    fInfo->type = "Jef Poskanzer's Portable Pixmaps (binary)";
    fInfo->extension = "ppm";
    fInfo->maxWidth = NOMAX;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = 24;
    fInfo->maxBitsPerColor = 8;
    fInfo->colorSpace = COLORMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }
  
  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fgets(buf, sizeof(buf), file);
  if (strcmp(buf, "P6\n"))
    return PICERR_BADFORMAT;

  fscanf(file, "%i %i\n%i\n", &width, &height, &depth);
  fclose(file);
  for (i = 1; (1 << i) < depth; i++);
  
  info->type = "Jef Poskanzer's Portable Pixmaps (binary)";
  info->extension = "ppm";
  info->photoInterp = COLORMASK;
  info->planarConfig = MESHED;
  info->width = width;
  info->height = height;
  info->bitsPerPixel = i;

  return PICERR_NOERR;
}

int load_pgm(char *filename, image *aPic)
{
  FILE *file;
  char buf[256];
  int width, height, depth;
  int i;
  byte *data;
  

  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fgets(buf, sizeof(buf), file);
  if (strcmp(buf, "P5\n"))
    return PICERR_BADFORMAT;

  fscanf(file, "%i %i\n%i\n", &width, &height, &depth);

  for (i = 1; (1 << i) < depth; i++);
  
  data = malloc(width * height);
  fread(data, width * height, 1, file);
  fclose(file);
  constructImage(aPic, MONOMASK, PLANAR, width, height, i, data, NULL);
  free(data);
  
  return PICERR_NOERR;
}

int save_pgm(char *filename, image *aPic)
{
  FILE *file;


  if (!(file = fopen(filename, "w")))
    return PICERR_CANTOPEN;

  fprintf(file, "P5\n%i %i\n%i\n", aPic->width, aPic->height, 255);
  fwrite(aPic->data, aPic->width * aPic->height, 1, file);
  fclose(file);

  return PICERR_NOERR;
}

int info_pgm(char *filename, imageInfo *info, formatInfo *fInfo)
{
  int width, height, depth;
  FILE *file;
  char buf[256];
  int i;

  
  if (!filename) {
    fInfo->type = "Jef Poskanzer's Portable Graymaps (binary)";
    fInfo->extension = "pgm";
    fInfo->maxWidth = NOMAX;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = 8;
    fInfo->maxBitsPerColor = 8;
    fInfo->colorSpace = MONOMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }
  
  if (!(file = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fgets(buf, sizeof(buf), file);
  if (strcmp(buf, "P5\n"))
    return PICERR_BADFORMAT;

  fscanf(file, "%i %i\n%i\n", &width, &height, &depth);
  fclose(file);
  for (i = 1; (1 << i) < depth; i++);
  
  info->type = "Jef Poskanzer's Portable Graymaps (binary)";
  info->extension = "pgm";
  info->photoInterp = MONOMASK;
  info->planarConfig = PLANAR;
  info->width = width;
  info->height = height;
  info->bitsPerPixel = i;

  return PICERR_NOERR;
}
