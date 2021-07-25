TARGET = robot
OBJS = robot.c pg.c font.c 

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lpspdisplay -lc -lm

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = robotfindskitten
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
