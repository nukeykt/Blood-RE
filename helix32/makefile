CPPFLAGS = /4r /d2 /otexan
AFLAGS = -ml -m2


OBJS = &
hkernel.obj &
palette.obj &
vmode.obj &
vgt.obj

DRIVEROBJS = &
hgd13.obj &
hgdx.obj

all : helix.lib $(DRIVEROBJS) .symbolic
	@%null

helix.lib: $(OBJS)
    %make helix.lnk
	wlib $^@ /n /b @helix.lnk

helix.lnk: makefile
    %create $^@
    for %i in ($(OBJS)) do %append $^@ -+%i


.cpp.obj:
    wpp386 $(CPPFLAGS) $[*

.asm.obj:
    tasm $(AFLAGS) $[*
