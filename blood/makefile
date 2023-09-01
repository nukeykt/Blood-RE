SRC_DIR = src
OBJ_DIR = obj

#CPPFLAGS = /5r /d3 /i=..\qtools\include;..\helix32;..\audiolib;..\mact;..\smacker
CPPFLAGS = /5r /d1 /s /orti /oe=3 /i=..\qtools\include;..\helix32;..\audiolib;..\mact;..\smacker
TENFLAGS = /4r /d1 /s /orti /oe=3 /i=..\qtools\include;..\audiolib;..\mact;..\ten\incl
CFLAGS = /5r /d2 /orti
AFLAGS = -ml

.h: $(SRC_DIR)
.cpp: $(SRC_DIR)
.c: $(SRC_DIR)
.asm: $(SRC_DIR)
.obj: $(OBJ_DIR)

OBJS = &
globals.obj &
blood.obj &
screen.obj &
view.obj &
db.obj &
gameutil.obj &
fire.obj &
fframe.obj &
sectorfx.obj &
map2d.obj &
satimer.obj &
usrhooks.obj &
endgame.obj &
levels.obj &
sound.obj &
sfx.obj &
controls.obj &
actor.obj &
player.obj &
replace.obj &
tile.obj &
triggers.obj &
eventq.obj &
qav.obj &
weapon.obj &
gui.obj &
seq.obj &
mirrors.obj &
dude.obj &
warp.obj &
weather.obj &
credits.obj &
menu.obj &
messages.obj &
ai.obj &
aibat.obj &
aibeast.obj &
aiboneel.obj &
aiburn.obj &
aicaleb.obj &
aicerber.obj &
aicult.obj &
aigarg.obj &
aigilbst.obj &
aighost.obj &
aihand.obj &
aihound.obj &
aiinnoc.obj &
aipod.obj &
airat.obj &
aispid.obj &
aitchern.obj &
aizomba.obj &
aizombf.obj &
fx.obj &
callback.obj &
loadsave.obj &
config.obj &
network.obj &
demo.obj &
gib.obj &
asound.obj &
gamemenu.obj &
cdrom.obj &
smkplay.obj &
mpath1.obj &
mpath2.obj &
mpath3.obj &
heat1.obj &
heat2.obj &
heat3.obj &
engage.obj &
choke.obj

all : blood.exe .symbolic
	@%null

blood.exe: $(OBJS) tenblood.obj
	%make blood.lnk
	wlink @blood.lnk

blood.lnk: makefile
	%create $^@
	%append $^@ opt map
	%append $^@ opt stack=65536
	%append $^@ opt static
	%append $^@ debug all
	%append $^@ name blood
	%append $^@ library ..\qtools\qtools.lib
	%append $^@ library ..\helix32\helix.lib
	%append $^@ library ..\build\engine.lib
	%append $^@ library ..\mact\mact386.lib
	%append $^@ library ..\audiolib\audio_wf.lib
	%append $^@ library ..\smacker\smack.lib
	%append $^@ library ..\ten\lib\sdkboxr.lib
	%append $^@ system dos4g
    for %i in ($(OBJS)) do %append $^@ file $(OBJ_DIR)\%i
	%append $^@ file ..\helix32\hgd13.obj
	%append $^@ file ..\helix32\hgdx.obj
	%append $^@ file ..\build\mmulti.obj
	%append $^@ file $(OBJ_DIR)\tenblood.obj


.cpp.obj:
    wpp386 $(CPPFLAGS) $[* /fo=$(OBJ_DIR)\$^&

.c.obj:
    wcc386 $(CFLAGS) $[* /fo=$(OBJ_DIR)\$^&

.asm.obj:
    tasm $(AFLAGS) $[*.asm $(OBJ_DIR)\$^&

tenblood.obj: $(SRC_DIR)\tenblood.cpp
    wpp386 $(TENFLAGS) $(SRC_DIR)\tenblood /fo=$(OBJ_DIR)\tenblood.obj
