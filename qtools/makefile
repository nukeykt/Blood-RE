
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

CPPFLAGS = /4r /s /orti /oe=3 /i=$(INC_DIR);..\helix32
CFLAGS = /i=$(INC_DIR)
AFLAGS = -ml


.h: $(INC_DIR)
.cpp: $(SRC_DIR)
.c: $(SRC_DIR)
.asm: $(SRC_DIR)
.obj: $(OBJ_DIR)

OBJS = &
qheap.obj &
resource.obj &
iob.obj &
crc32.obj &
trig.obj &
getopt.obj &
error.obj &
debug4g.obj &
textio.obj &
pqueue.obj &
key.obj &
inifile.obj &
mouse.obj &
misc.obj &
gfx.obj

all : qtools.lib .symbolic
    @%null

qtools.lib: $(OBJS)
    %make qtools.lnk
	wlib $^@ /n /b @qtools.lnk

qtools.lnk: makefile
    %create $^@
    for %i in ($(OBJS)) do %append $^@ -+$(OBJ_DIR)\%i


.cpp.obj:
    wpp386 $(CPPFLAGS) $[* /fo=$(OBJ_DIR)\$^&

.asm.obj:
    tasm $(AFLAGS) $[*.asm $(OBJ_DIR)\$^&