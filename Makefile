PRODUCT= libpiclib.a
# If PRODUCT= line is missing, be sure to insert one
# $(PRODUCT).c will be automatically compiled, so it
# doesn't need to be inserted below
NS33FMTS= fmttiff.c
CFILES= fmtIIgs.c fmtgif.c fmtast.c fmthp150.c fmtppm.c fmtclip.c	\
	fmtgrob.c fmtjpeg.c fmtppp.c fmtSNES.c fmtpng.c fmtlibtiff.c	\
	fmtIIhires.c \
	picloader.c picsaver.c piclib.c
#CFLAGS= -O3 -g -Wall -I$(HOME)/Unix/$(OSTYPE)/include
CFLAGS= -g -Wall -I$(HOME)/Unix/$(OSTYPE)/include
MAKEFILEDIR=/usr/local/Makefiles
MAKEFILE=lib.make
NS33LIBS= -lNeXT_s -lsys_s
LINUXLIBS= -lm
OTHER_LIBS= -lpiclib -ljpeg -lpng -ltiff $(LINUXLIBS) -lfozzlib

-include Makefile.preamble

include $(MAKEFILEDIR)/$(MAKEFILE)

-include Makefile.postamble

-include Makefile.dependencies
