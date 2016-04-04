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

#include "fmtpng.h"
#include <png.h>
#include <stdlib.h>

int load_png(char *filename, image *aPic)
{
  FILE *file;
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep *row_pointers;
  int y;
  unsigned char *data;
  int num_palette;
  png_colorp palette = NULL;
  png_uint_32 height, width, rowbytes;
  png_byte bit_depth, color_type;


  file = fopen(filename, "r");
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info_ptr = png_create_info_struct(png_ptr);

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr,
			    (png_infopp)NULL);
    return PICERR_BADFORMAT;
  }

  png_init_io(png_ptr, file);
  png_read_info(png_ptr, info_ptr);

  png_set_interlace_handling(png_ptr);
  png_read_update_info(png_ptr, info_ptr);

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr,
			    (png_infopp)NULL);
    return PICERR_BADFORMAT;
  }

  /* Stuff that used to be in a handy struct */
  height = png_get_image_height(png_ptr, info_ptr);
  width = png_get_image_width(png_ptr, info_ptr);
  rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);
  
  if (bit_depth > 8) { /* FIXME - deal with 16bit pixel images! */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return PICERR_BADFORMAT;
  }

  row_pointers = malloc(sizeof(png_bytep) * height);
  data = malloc(rowbytes * height);
  for (y = 0; y < height; y++)
    row_pointers[y] = data + rowbytes * y;
  png_read_image(png_ptr, row_pointers);
  fclose(file);

  {
    int p_interp;
    int p_config;
    int bpp;

    
    p_interp = 0;
    if (color_type & PNG_COLOR_MASK_COLOR ||
	color_type & PNG_COLOR_MASK_PALETTE)
      p_interp |= COLORMASK;
    else
      p_interp |= MONOMASK;
    if (color_type & PNG_COLOR_MASK_PALETTE) {
      p_interp |= PALETTEMASK;
      png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
    }
    if (color_type & PNG_COLOR_MASK_ALPHA)
      p_interp |= ALPHAMASK;
  
    p_config = MESHED;
    bpp = bit_depth;

    constructImage(aPic, p_interp, p_config, width, height, bpp, data, (byte *) palette);
  }
  
  free(data);
  free(row_pointers);
  
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  
  return PICERR_NOERR;
}

int save_png(char *filename, image *aPic)
{
  FILE *file;
  png_structp png_ptr;
  png_infop info_ptr;
#if 0
  png_colorp palette;
#endif
  byte *data, *pal;
  int err, bits, png_ctype;
  png_bytep *row_pointers;
  int samples, bpl;
  int i;


  if (!(file = fopen(filename, "w")))
    return PICERR_CANTOPEN;

  if (!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))) {
    fclose(file);
    return PICERR_CANTALLOC;
  }

  if (!(info_ptr = png_create_info_struct(png_ptr))) {
    fclose(file);
    png_destroy_write_struct(&png_ptr, NULL);
    return PICERR_CANTALLOC;
  }

#if 0
  if (setjmp(png_jmpbuf(png_ptr))) {
    fclose(file);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return PICERR_IOERROR;
  }
#endif

  png_init_io(png_ptr, file);

  /* FIXME - try to support palette based PNG */
  if ((err = unconstructImage(aPic, aPic->photoInterp, MESHED, 8, &bits, &data, &pal))) {
    fclose(file);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return err;
  }

  if (aPic->photoInterp & COLORMASK) {
    if (aPic->photoInterp & PALETTEMASK)
      png_ctype = PNG_COLOR_TYPE_PALETTE;
    else if (aPic->photoInterp & ALPHAMASK)
      png_ctype = PNG_COLOR_TYPE_RGB_ALPHA;
    else
      png_ctype = PNG_COLOR_TYPE_RGB;
  }
  else {
    if (aPic->photoInterp & ALPHAMASK)
      png_ctype = PNG_COLOR_TYPE_GRAY_ALPHA;
    else
      png_ctype = PNG_COLOR_TYPE_GRAY;
  }
  
  png_set_IHDR(png_ptr, info_ptr, aPic->width, aPic->height, bits, png_ctype,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

#if 0 /* FIXME - palette support */
   /* set the palette if there is one.  REQUIRED for indexed-color images */
   palette = (png_colorp) png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));
   /* ... set palette colors ... */
   png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
   /* You must not free palette here, because png_set_PLTE only makes a link to
      the palette that you malloced.  Wait until you are about to destroy
      the png structure. */
#endif

#if 0 /* FIXME - what the heck is this crap? */
   /* optional significant bit chunk */
   /* if we are dealing with a grayscale image then */
   sig_bit.gray = true_bit_depth;
   /* otherwise, if we are dealing with a color image then */
   sig_bit.red = true_red_bit_depth;
   sig_bit.green = true_green_bit_depth;
   sig_bit.blue = true_blue_bit_depth;
   /* if the image has an alpha channel then */
   sig_bit.alpha = true_alpha_bit_depth;
   png_set_sBIT(png_ptr, info_ptr, sig_bit);
#endif

   png_write_info(png_ptr, info_ptr);
   
   row_pointers = malloc(aPic->height * sizeof(png_bytep));
   samples = 1;
   if (aPic->photoInterp & COLORMASK && !(aPic->photoInterp & PALETTEMASK))
     samples = 3;
   if (aPic->photoInterp & ALPHAMASK)
     samples++;
   bpl = (aPic->width * samples * bits + 7) / 8;
   for (i = 0; i < aPic->height; i++)
     row_pointers[i] = data + i * bpl;

   png_write_image(png_ptr, row_pointers);
   png_write_end(png_ptr, info_ptr);

#if 0
   /* If you png_malloced a palette, free it here (don't free info_ptr->palette,
      as recommended in versions 1.0.5m and earlier of this example; if
      libpng mallocs info_ptr->palette, libpng will free it).  If you
      allocated it with malloc() instead of png_malloc(), use free() instead
      of png_free(). */
   png_free(png_ptr, palette);
   palette=NULL;
#endif

   png_destroy_write_struct(&png_ptr, &info_ptr);
   free(row_pointers);
   free(data);
   if (pal)
     free(pal);
   
   fclose(file);

   return PICERR_NOERR;
}

int info_png(char *filename, imageInfo *info, formatInfo *fInfo)
{
  FILE *file;
  png_structp png_ptr;
  png_infop info_ptr;
  png_uint_32 height, width;
  png_byte bit_depth, color_type;

  
  if (!filename) { /* Just want longname and extension */
    fInfo->type = "Portable Network Graphics Image";
    fInfo->extension = "png";
    fInfo->maxWidth = NOMAX;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = NOMAX;
    fInfo->maxBitsPerColor = 16;
    fInfo->colorSpace = COLORMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }

  info->type = "Portable Network Graphics Image";
  info->extension = "png";

  if ((file = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return PICERR_CANTOPEN;
  }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info_ptr = png_create_info_struct(png_ptr);

  /* FIXME - setjmp */

  png_init_io(png_ptr, file);
  png_read_info(png_ptr, info_ptr);

  /* Stuff that used to be in a handy struct */
  height = png_get_image_height(png_ptr, info_ptr);
  width = png_get_image_width(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);

  info->photoInterp = 0;
  if (color_type & PNG_COLOR_MASK_COLOR ||
      color_type & PNG_COLOR_MASK_PALETTE)
    info->photoInterp |= COLORMASK;
  else
    info->photoInterp |= MONOMASK;
  if (color_type & PNG_COLOR_MASK_PALETTE)
    info->photoInterp |= PALETTEMASK;
  if (color_type & PNG_COLOR_MASK_ALPHA)
    info->photoInterp |= ALPHAMASK;
  
  info->planarConfig = MESHED;
  info->width = width;
  info->height = height;
  info->bitsPerPixel = bit_depth;
  info->bitsPerColor = info->bitsPerPixel *
    ((info->photoInterp & COLORMASK ? 3 : 1) +
     (info->photoInterp & ALPHAMASK ? 1 : 0));

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  fclose(file);
  
  return PICERR_NOERR;
}
