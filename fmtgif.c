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

#include "piclib.h"
#include "fmtgif.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFERSIZE 255
#define MAXMAX 4096

#define STRUCTSIZE 32
#define HASHFIRST 0
#define HASHNEXT 2
#define HASHCHAR 4

typedef struct {
  short hashfirst;
  short hashnext;
  unsigned char hashchar;
} HASHTABLE;


typedef struct codestruct {
  struct codestruct *prefix;
  unsigned char first, suffix;
} codetype;

static int clear, eoi;

static int datasize, codesize, codemask;
static codetype *codetable;
static int global, globalbits;
static unsigned char globalmap[256][3];

static unsigned char *outputdata;
static HASHTABLE *hash;
static int code;
static int flag;
static int bitoffset, handle, imagerem, buflen;
static int firstfree, prefixcode, freecode, maxcode, nbits, thisbyte;


static void outcode(codetype *p, unsigned char **fill)
{
  if (p->prefix)
    outcode(p->prefix, fill);

  *(*fill)++ = p->suffix;
}

static inline int process(int code, unsigned char **fill)
{
  static int avail, oldcode;
  codetype *p;

  if (code == clear) {
    codesize = datasize + 1;
    codemask = (1 << codesize) - 1;
    avail = clear + 2;
    oldcode = -1;
  }
  else if (code < avail) {
    outcode(&codetable[code], fill);
    if (oldcode != -1) {
      p = &codetable[avail++];
      p->prefix = &codetable[oldcode];
      p->first = p->prefix->first;
      p->suffix = codetable[code].first;

      if ((avail & codemask) == 0 && avail < 4096) {
	codesize++;
	codemask += avail;
      }
    }

    oldcode = code;
  }
  else if (code == avail && oldcode != -1) {
    p = &codetable[avail++];
    p->prefix = &codetable[oldcode];
    p->first = p->prefix->first;
    p->suffix = p->first;

    outcode(p, fill);

    if ((avail & codemask) == 0 && avail < 4096) {
      codesize++;
      codemask += avail;
    }
    oldcode = code;
  }
  else {
    fprintf(stderr, "Illegal code in raster data. Continuing\n");
    return PICERR_BADFORMAT;
  }

  return 0;
}
  
static inline int readGifRaster(int width, int height, unsigned char *raster, FILE *giffile)
{
  unsigned char *fill = raster;
  unsigned char buf[255];
  int bits = 0;
  int count, datum = 0;
  unsigned char *ch;
  int code, err, byte;


  datasize = getc(giffile);
  clear = 1 << datasize;
  eoi = clear + 1;
  codesize = datasize + 1;
  codemask = (1 << codesize) - 1;
  if (!(codetable = (codetype *) malloc(4096 * sizeof(codetype))))
    return PICERR_CANTALLOC;

  for (code = 0; code < clear; code++) {
    codetable[code].prefix = (codetype *) 0;
    codetable[code].first = code;
    codetable[code].suffix = code;
  }

  for (count = getc(giffile), byte = 1; count > 0; count = getc(giffile), byte++) {
    fread(buf, 1, count, giffile);
    for (ch = buf; count-- > 0; ch++) {
      datum += *ch << bits;
      bits += 8;
      byte++;
      while (bits >= codesize) {
	code = datum & codemask;
	datum >>= codesize;
	bits -= codesize;
	if (code == eoi)
	  goto exitloop;         /* This kludge put in because some GIF files aren't standard */

	if ((err = process(code, &fill))) {
	  fprintf(stderr, "File Position: %o %o %o %o %i\n",
		   (unsigned int) *ch, (unsigned int) *(ch + 1), (unsigned int) *(ch + 2), (unsigned int) *(ch + 3),
		   count);
	  return err;
	}
      }
    }
  }

 exitloop:
  if (fill != raster + width * height) {
    fprintf(stderr, "Raster has the wrong size. Continuing\n");
    return PICERR_BADFORMAT;
  }

  free(codetable);

  return 0;
}

static inline int readGifImage(FILE *giffile, image *aPic)
{
  unsigned char buf[9];
  int top, width, height;
  int local, interleaved;
  int localbits;
  int row;
  int *interleavetable = NULL;
  unsigned char *rasterfix = NULL;
  unsigned char *cmap = NULL, *raster = NULL;
  unsigned char colormap[256][3];
  int i, cbits;
  int err;


  fread(buf, 1, 9, giffile);
  top = buf[2] + (buf[3] << 8);
  width = buf[4] + (buf[5] << 8);
  height = buf[6] + (buf[7] << 8);
  local = buf[8] & 0x80;
  interleaved = buf[8] & 0x40;

  if (local) {
    localbits = (buf[8] & 0x07) + 1;
    fread(colormap, 3, 1 << localbits, giffile);
    cmap = (unsigned char *) colormap;
    cbits = 1 << localbits;
  }
  else if (global) {
    cmap = (unsigned char *) globalmap;
    cbits = 1 << globalbits;
  }
  else {
    fprintf(stderr, "No color map present.\n");
    return PICERR_BADFORMAT;
  }

  if (!(raster = malloc(width * height)))
    return PICERR_CANTALLOC;

  err = readGifRaster(width, height, raster, giffile);

  if (interleaved) {
    if (!(interleavetable = malloc(height * sizeof(int))))
      return PICERR_CANTALLOC;
    if (!(rasterfix = malloc(width * height)))
      return PICERR_CANTALLOC;

    row = 0;
    for (i = top; i < top + height; i += 8)
      interleavetable[i] = row++;
    for (i = top + 4; i < top + height; i += 8)
      interleavetable[i] = row++;
    for (i = top + 2; i < top + height; i += 4)
      interleavetable[i] = row++;
    for (i = top + 1; i < top + height; i += 2)
      interleavetable[i] = row++;

    for (row = top; row < top + height; row++)
      for (i = 0; i < width; i++)
	rasterfix[i + row * width] = raster[i + interleavetable[row] * width];

    free(raster);
    raster = rasterfix;
    free(interleavetable);
  }

  constructImage(aPic, COLORMASK | PALETTEMASK, PLANAR, width, height, 8, raster, cmap);
  free(raster);
  
  for (i = 0, row = 0; i < cbits; i++)
    if (cmap[i * 3] == cmap[i * 3 + 1] && cmap[i * 3] == cmap[i * 3 + 2])
      row++;

  if (row == cbits)
    makeMono(aPic);
  
  return err;
}

static inline void readGifExtension(FILE *giffile)
{
  unsigned char count;
  char buf[255];

  code = getc(giffile);
  while ((count = getc(giffile)))
    fread(buf, 1, count, giffile);
}

int load_gif(char *filename, image *aPic)
{
  FILE *giffile;
  unsigned char buf[7];
  int ch;
  int quit = NO;
  int err;
  

  if (!(giffile = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fread(buf, 1, 6, giffile);
  if (strncmp((char *) buf, "GIF", 3))
    return PICERR_BADFORMAT;
  if (strncmp((char *) &buf[3], "87a", 3) && strncmp((char *) &buf[3], "89a", 3))
    return PICERR_FORMATNOTSUPPORTED;

  fread(buf, 1, 7, giffile);

  global = buf[4] & 0x80;

  if (global) {
    globalbits = (buf[4] & 0x07) + 1;
    fread(globalmap, 3, 1 << globalbits, giffile);
  }

  do {
    ch = getc(giffile);
    switch (ch) {
    case '\0':
      break;            /* This kludge for non-standard files */

    case ',':
      err = readGifImage(giffile, aPic);
      /* I don't want to go on, but I would like to see the picture */
      if (err)
	return 0;
      break;

    case ';':
      quit = YES;
      break;

    case '!':
      readGifExtension(giffile);
      break;

    default:
      fprintf(stderr, "Illegal GIF block type. Continuing\n");
      return 0; //PICERR_BADFORMAT;
    }
  } while (!quit);

  fclose(giffile);
  
  return 0;
}

static inline void write_word(FILE *file, int word)
{
  fprintf(file, "%c%c", word & 0xFF, (word >> 8) & 0xFF);

  return;
}

static inline void init_table(int bpp)
{
  int j;
  

  nbits = bpp+ 1;
  maxcode = clear * 2;

  for (j = 0; j <= clear; j++) {
    hash[j].hashfirst = -1;
    hash[j].hashnext = -1;
    hash[j].hashchar = -1;
  }

  freecode = firstfree;
  
  return;
}

static inline void flush(FILE *gif_file)
{
  int i;

  
  fprintf(gif_file, "%c", buflen);

  for (i = 0; i < buflen; i++)
    fprintf(gif_file, "%c", outputdata[i]);

  return;
}

static inline void write_code(FILE *gif_file, int code)
{
  int j;
  

  buflen = bitoffset / 8;
  j = bitoffset % 8;
  bitoffset += nbits;
  
  if (buflen >= BUFFERSIZE - 4) {
    flush(gif_file);
    bitoffset = nbits + j;
    outputdata[0] = outputdata[buflen];
    buflen = 0;
  }
  
  if (j) {
    code <<= j;
    code |= outputdata[buflen];
  }
  
  outputdata[buflen] = code & 0xFF;
  outputdata[buflen + 1] = (code >> 8) & 0xFF;
  outputdata[buflen + 2] = (code >> 16) & 0xFF;
  buflen += 3;

  return;
}

static inline int gnext_byte(int *cont, int width, int height, byte *data)
{
  *cont = 1;
  imagerem++;
  if (imagerem <= width * height)
    return data[imagerem - 1];
  else {
    *cont = 0;
    return 0;
  }
}

static inline int lookup_code(int start, int *rcode)
{
  int ocode;

  
  code = start;
  flag = 0;

  if (hash[code].hashfirst == -1)
    return 0;

  flag++;
  
  code = hash[code].hashfirst;
  
  do {
    if (hash[code].hashchar == thisbyte) {
      *rcode = code;
      return 1;
    }
    ocode = code;
    code = hash[code].hashnext;
  } while (code != -1);

  code = ocode;
  
  return 0;
}
  
static inline void add_code()
{
  if (flag) {
    hash[code].hashnext = freecode;
  }
  else {
    hash[code].hashfirst = freecode;
  }

  if (freecode == MAXMAX) {
    freecode++;
    return;
  }

  hash[freecode].hashfirst = -1;
  hash[freecode].hashnext = -1;
  hash[freecode].hashchar = thisbyte;
  
  freecode++;

  return;
}

static inline int compress(FILE *gif_file, int width, int height, int bpp, byte *data)
{
  int cont;

  
  hash = (HASHTABLE *) malloc(sizeof(HASHTABLE) * MAXMAX);
  outputdata = (unsigned char *) malloc(BUFFERSIZE);
  
  init_table(bpp);
  write_code(gif_file, clear);

  thisbyte = gnext_byte(&cont, width, height, data);
  while (cont) {
    prefixcode = thisbyte;
    thisbyte = gnext_byte(&cont, width, height, data);

    if (thisbyte >= clear) {
      fprintf(stderr, "Error on code: %i\n", thisbyte);
      return PICERR_BADFORMAT;
    }

    if (cont) {
      if (!lookup_code(prefixcode, &thisbyte)) {
	add_code();
	write_code(gif_file, prefixcode);

	if (freecode > maxcode) {
	  if (nbits >= 12) {
	    write_code(gif_file, clear);
	    init_table(bpp);
	  }
	  else {
	    nbits++;
	    maxcode <<= 1;
	  }
	}
      }
    }
  }

  write_code(gif_file, prefixcode);
  write_code(gif_file, eoi);
  if (bitoffset) {
    buflen = (bitoffset / 8) + (bitoffset % 8 > 0);
    flush(gif_file);
  }

  free(hash);
  free(outputdata);

  return PICERR_NOERR;
}

int save_gif(char *filename, image *aPic)
{
  int i = 0;
  int bits;
  int err;
  byte *data, *palette;
  FILE *gif_file;


  if ((err = unconstructImage(aPic, aPic->photoInterp | PALETTEMASK, PLANAR, 0,
			      &bits, &data, &palette)))
    return err;
  if (bits < 2)
    bits = 2;
  
  clear = eoi = firstfree = prefixcode = 0;
  freecode = maxcode = nbits = thisbyte = bitoffset = handle = 0;
  imagerem = 0;

  clear = 1 << bits;
  eoi = clear + 1;
  firstfree = eoi + 1;
    
  if (!(gif_file = fopen(filename, "w")))
    return PICERR_CANTOPEN;

  /* Write GIF header */
  fprintf(gif_file, "GIF87a");
  write_word(gif_file, aPic->width);
  write_word(gif_file, aPic->height);
  fprintf(gif_file, "%c", (bits - 1) | 0x90);
  fprintf(gif_file, "%c", 0);
  fprintf(gif_file, "%c", 0);
  i = (1 << bits) * 3;
  fwrite(palette, 1, i, gif_file);
  fprintf(gif_file, ",");
  write_word(gif_file, 0);
  write_word(gif_file, 0);
  write_word(gif_file, aPic->width);
  write_word(gif_file, aPic->height);
  fprintf(gif_file, "%c%c", bits - 1, bits);
  
  /* Write GIF data */
  if ((err = compress(gif_file, aPic->width, aPic->height, bits, data)))
    return err;

  fprintf(gif_file, "%c;", 0);
  
  fclose(gif_file);

  free(data);
  free(palette);
  
  return 0;
}

static inline int readGifInfo(FILE *giffile, imageInfo *info)
{
  unsigned char buf[9];
  int width, height;
  int local;
  int localbits;
  int cbits;


  fread(buf, 1, 9, giffile);
  width = buf[4] + (buf[5] << 8);
  height = buf[6] + (buf[7] << 8);
  local = buf[8] & 0x80;

  if (local) {
    localbits = (buf[8] & 0x07) + 1;
    cbits = localbits;
  }
  else if (global) {
    cbits = globalbits;
  }
  else {
    fprintf(stderr, "No color map present.\n");
    return PICERR_BADFORMAT;
  }

  info->width = width;
  info->height = height;
  info->bitsPerColor = 24;
  info->numColors = 1 << cbits;

  return PICERR_NOERR;
}

int info_gif(char *filename, imageInfo *info, formatInfo *fInfo)
{
  FILE *giffile;
  unsigned char buf[7];
  int ch;
  int quit = NO;
  int err;
  

  if (!filename) { /* Just want longname and extension */
    fInfo->type = "Compuserve Graphics Interchange Format";
    fInfo->extension = "gif";
    fInfo->maxWidth = NOMAX;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = 8;
    fInfo->maxBitsPerColor = 8;
    fInfo->colorSpace = COLORMASK | PALETTEMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }
  
  if (!(giffile = fopen(filename, "r")))
    return PICERR_CANTOPEN;

  fread(buf, 1, 6, giffile);
  if (strncmp((char *) buf, "GIF", 3))
    return PICERR_BADFORMAT;
  if (strncmp((char *) &buf[3], "87a", 3) && strncmp((char *) &buf[3], "89a", 3))
    return PICERR_FORMATNOTSUPPORTED;

  fread(buf, 1, 7, giffile);

  global = buf[4] & 0x80;

  if (global) {
    globalbits = (buf[4] & 0x07) + 1;
    fread(globalmap, 3, 1 << globalbits, giffile);
  }

  do {
    ch = getc(giffile);
    switch (ch) {
    case '\0':
      break;            /* This kludge for non-standard files */

    case ',':
      err = readGifInfo(giffile, info);
      if (err)
	return err;
      quit = YES;
      break;

    case ';':
      quit = YES;
      break;

    case '!':
      readGifExtension(giffile);
      break;

    default:
      fprintf(stderr, "Illegal GIF block type. Continuing\n");
      return 0;
    }
  } while (!quit);

  fclose(giffile);

  info->type = "Compuserve Graphics Interchange Format";
  info->extension = "gif";
  info->photoInterp = COLORMASK | PALETTEMASK;
    
  return PICERR_NOERR;
}
