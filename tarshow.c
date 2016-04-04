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
#include <fozzlib/getargs.h>
#include <ctype.h>
#include <sys/fcntl.h>
#include <libc.h>
#include <sys/types.h>
#include <dev/scsireg.h>
#include <sys/mtio.h>

extern void exit();

#define TBLOCK 512
#define NAMSIZ 100
#define BFACTOR 20

#define BSIZE (TBLOCK * BFACTOR)

typedef union hblock {
  char dummy[TBLOCK];
  struct header {
    char name[NAMSIZ];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char linkflag;
    char linkname[NAMSIZ];
  } dbuf;
} tar_block;

char bbuf[BSIZE];
int bp = BSIZE;
int cblock = 0;

int bl_read(int d, tar_block *block);
int bl_seek(int d, int offset);

void main(int argc, char *argv[])
{
  int count;
  int d, d2;
  tar_block block;
  int i, j, size;
  char name[256];
  char buf[256];
  int list = 0;
  char *skipto = NULL;
  int rpos = 0;
  

  count = getargs(argc, argv, "lbssri", &list, &skipto, &rpos);

  if (count < 0 || (argc - count) < 1) {
    if (count < 0 && -count != '-')
      fprintf(stderr, "Bad flag: %c\n", -count);
    fprintf(stderr, "Usage: %s filename\n"
	     , *argv);
    exit(1);
  }

  if (!(d = open(argv[count], O_RDONLY))) {
    fprintf(stderr, "Can't open %s\n", argv[count]);
    exit(1);
  }

  if (rpos)
    bl_seek(d, rpos);
  
  while (bl_read(d, &block)) {
    for (i = size = 0; i < 12; i++)
      if (isdigit(block.dbuf.size[i])) {
	size *= 8;
	size += block.dbuf.size[i] - '0';
      }
    fprintf(stderr, "%9i %7i %s\n", size, cblock - 1, block.dbuf.name);
    
    i = (size + (TBLOCK - 1)) / TBLOCK;
    if (list || (skipto && strcmp(block.dbuf.name, skipto))) {
      bl_seek(d, i);
    }
    else {
      skipto = NULL;
      strcpy(name, "/tmp/fileXXXXXX");
      d2 = mkstemp(name);
      for (j = 0; j < i; j++) {
	bl_read(d, &block);
	write(d2, &block, TBLOCK);
      }
      close(d2);
      sprintf(buf, "picview -o NeXT -M %s", name);
      system(buf);
      unlink(name);
    }
  }
  
  exit(0);
}

int bl_read(int d, tar_block *block)
{
  int len;
  
  
  if (bp >= BSIZE) {
    if ((len = read(d, bbuf, BSIZE)) <= 0)
      return 0;
    bp = 0;
  }

  memcpy(block, &bbuf[bp], TBLOCK);
  bp += TBLOCK;

  cblock++;
  
  return TBLOCK;
}

int bl_seek(int d, int offset)
{
  int i;
  tar_block block;
  struct modesel_parms parms;
  int bl;
  struct mtop op;


  if (!cblock && ioctl(d, MTIOCMODSEN, &parms) >= 0) { /* First seek, use record position commands */
    bl = (BSIZE - bp) / TBLOCK;
    i = offset < bl ? offset : bl;
    bp += i * TBLOCK;
    cblock += i;
    offset -= i;
    if (offset) {
      i = offset / BFACTOR;
      if (i) {
	op.mt_op = MTFSR;
	op.mt_count = i;
	ioctl(d, MTIOCTOP, &op);
	offset -= i * BFACTOR;
	cblock += i * BFACTOR;
      }
    }
  }

  for (i = 0; i < offset; i++)
    bl_read(d, &block);
  
  return 1;
}
