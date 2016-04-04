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

#include "fmtjpeg.h"
#include <sys/param.h>
#include <stdio.h>
#include <setjmp.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;
static struct my_error_mgr jerr;

static void my_error_exit(j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

static void my_handler(int dunno)
{
  longjmp(jerr.setjmp_buffer, 1);
}

int load_jpeg(char *filename, image *aPic)
{
  struct jpeg_decompress_struct cinfo;
  FILE *file;
  JSAMPROW jpeg_row;
  JSAMPARRAY jpeg_data;
  unsigned char *data;
  int i;
#ifdef linux
  void *handler;
#endif


  memset(aPic, 0, sizeof(image));
  
  if ((file = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return PICERR_CANTOPEN;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    return PICERR_BADFORMAT;
  }

#ifdef linux
  handler = signal(SIGSEGV, my_handler);
#endif
  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  data = malloc(cinfo.output_width * cinfo.output_height *
		     cinfo.output_components);
  jpeg_row = malloc(cinfo.output_width * cinfo.output_components);
  jpeg_data = &jpeg_row;
  for (i = 0; cinfo.output_scanline < cinfo.output_height; i++) {
    jpeg_read_scanlines(&cinfo, jpeg_data,
			1);
    memcpy(&data[i * cinfo.output_width * cinfo.output_components],
	   jpeg_row, cinfo.output_width * cinfo.output_components);
  }
  
  constructImage(aPic, cinfo.out_color_space == JCS_GRAYSCALE ?
		 MONOMASK : COLORMASK, MESHED, cinfo.output_width,
		 cinfo.output_height, cinfo.data_precision, data,
		 NULL);
  
  free(jpeg_row);
  free(data);
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(file);
#ifdef linux
  signal(SIGSEGV, handler);
#endif

  return PICERR_NOERR;
}

int save_jpeg(char *filename, image *aPic)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *outfile;
  JSAMPROW row_pointer[1];
  int row_stride;
  

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  if (!(outfile = fopen(filename, "w"))) {
    fprintf(stderr, "Can't open %s\n", filename);
    return PICERR_CANTOPEN;
  }
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = aPic->width;
  cinfo.image_height = aPic->height;
  cinfo.input_components = aPic->photoInterp & MONOMASK ? 1 : 3;
  cinfo.in_color_space = aPic->photoInterp & MONOMASK ? JCS_GRAYSCALE : JCS_RGB;
  row_stride = cinfo.image_width * cinfo.input_components;

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 100, TRUE);
  jpeg_simple_progression(&cinfo);
  jpeg_start_compress(&cinfo, TRUE);

  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &aPic->data[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);
  
  return PICERR_NOERR;
}

int info_jpeg(char *filename, imageInfo *info, formatInfo *fInfo)
{
  FILE *file;
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;
#ifdef linux
  void *handler;
#endif

  
  if (!filename) { /* Just want longname and extension */
    fInfo->type = "Joint Photographic Expert Group Image";
    fInfo->extension = "jpg";
    fInfo->maxWidth = NOMAX;
    fInfo->maxHeight = NOMAX;
    fInfo->maxBitsPerPixel = NOMAX;
    fInfo->maxBitsPerColor = 8;
    fInfo->colorSpace = COLORMASK;
    fInfo->grayRamp = NULL;
    return PICERR_NOERR;
  }

  info->type = "Joint Photographic Expert Group Image";
  info->extension = "jpg";

  if ((file = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return PICERR_CANTOPEN;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    return PICERR_BADFORMAT;
  }

#ifdef linux
  handler = signal(SIGSEGV, my_handler);
#endif
  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);
  
  info->photoInterp = cinfo.out_color_space == JCS_GRAYSCALE ? MONOMASK : COLORMASK;
  info->planarConfig = MESHED;
  info->width = cinfo.image_width;
  info->height = cinfo.image_height;
  info->bitsPerColor = cinfo.out_color_space == JCS_GRAYSCALE ? 8 : 24;
  info->bitsPerPixel = 8;

  jpeg_destroy((j_common_ptr) &cinfo);
  fclose(file);
#ifdef linux
  signal(SIGSEGV, handler);
#endif
  
  return PICERR_NOERR;
}

