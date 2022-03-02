TARGET = pspnetcat
OBJS   = main.o utils.o

INCDIR   = 
CFLAGS   = -G0 -Wall -O2 -g -Wno-unused-function
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS  = $(CFLAGS)
LIBS     = -lpspgu -lpspnet -lpspdisplay \
           -lpspgum -lz -lm

EXTRA_TARGETS   = EBOOT.PBP
PSP_EBOOT_TITLE = 	ELDEN RING

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak