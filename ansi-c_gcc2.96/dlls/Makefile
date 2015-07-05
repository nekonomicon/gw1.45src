#
# Half-Life ProSDK 2.0 hl_i386.so Makefile for i386 Linux
#
# April 2000 by Leon Hartwig (jehannum@planethalflife.com)
#

# May 2015 customized by Swamp Dog for GangWars Mod
DLLNAME=mp

ARCH=i386

#make sure this is the correct compiler for your system
CC=gcc

DLL_SRCDIR=.
WPN_SHARED_SRCDIR=./wpn_shared
PM_SHARED_SRCDIR=../pm_shared

DLL_OBJDIR=$(DLL_SRCDIR)/obj
WPN_SHARED_OBJDIR=$(WPN_SHARED_SRCDIR)/obj
PM_SHARED_OBJDIR=$(PM_SHARED_SRCDIR)/obj

## old setup
## BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp

## new setup based on Natural Selection makefile -incomplete - see visual c++ 6 settings
## BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -DLINUX -DVALVE_DLL -DQUIVER -DVOXEL -DQUAKE2 -DDEDICATED -DSWDS -D_STLP_USE_GLIBC -DUSE_OLDAUTH 

## try all settings
## BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -DLINUX -DVALVE_DLL -DQUIVER -DVOXEL -DQUAKE2 -DDEDICATED -DSWDS -D_STLP_USE_GLIBC -DUSE_OLDAUTH 

## doesn't build sdk_c_logic.cpp:
BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -DLINUX -DVALVE_DLL -DQUIVER -DVOXEL -DQUAKE2
## may not need -DAVH_SERVER , may not need -DSWDS ? , may not need -D_STLP_USE_GLIBC ? , may not need -DUSE_OLDAUTH ?


## safe optimization
## CFLAGS=$(BASE_CFLAGS) -w -m486 -O1

## full optimization
CFLAGS=$(BASE_CFLAGS) -w -m486 -O2 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2

## use these when debugging 
## CFLAGS=$(BASE_CFLAGS) -g

INCLUDEDIRS=-I. -I../engine -I../common -I../pm_shared

LDFLAGS=/usr/lib/gcc-lib/i386-redhat-linux/2.96/libstdc++.a

SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) $(INCLUDEDIRS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################

$(DLL_OBJDIR)/%.o: $(DLL_SRCDIR)/%.cpp
	$(DO_CC)

$(WPN_SHARED_OBJDIR)/%.o: $(WPN_SHARED_SRCDIR)/%.cpp
	$(DO_CC)

$(PM_SHARED_OBJDIR)/%.o: $(PM_SHARED_SRCDIR)/%.c
	$(DO_CC)

## Removed apache.o, sdk_w_usp.o, skill.o, player_sec.o -
## not needed - renamed "sdk_c_waitlist.cpp" swampdog July 1 2015
## Need safe optimization, debugging, and oldsetup basecflags
## Now trying with full optimization, libstdc++.a, basecflags -DLINUX

OBJ = \
	$(DLL_OBJDIR)/animating.o \
	$(DLL_OBJDIR)/animation.o \
	$(DLL_OBJDIR)/bmodels.o \
	$(DLL_OBJDIR)/buttons.o \
	$(DLL_OBJDIR)/cbase.o \
	$(DLL_OBJDIR)/client.o \
	$(DLL_OBJDIR)/combat.o \
	$(DLL_OBJDIR)/doors.o \
	$(DLL_OBJDIR)/effects.o \
	$(DLL_OBJDIR)/explode.o \
	$(DLL_OBJDIR)/func_break.o \
	$(DLL_OBJDIR)/func_tank.o \
	$(DLL_OBJDIR)/game.o \
	$(DLL_OBJDIR)/gamerules.o \
	$(DLL_OBJDIR)/ggrenade.o \
	$(DLL_OBJDIR)/globals.o \
	$(DLL_OBJDIR)/h_ai.o \
	$(DLL_OBJDIR)/h_battery.o \
	$(DLL_OBJDIR)/h_export.o \
	$(DLL_OBJDIR)/healthkit.o \
	$(DLL_OBJDIR)/items.o \
	$(DLL_OBJDIR)/lights.o \
	$(DLL_OBJDIR)/maprules.o \
	$(DLL_OBJDIR)/monsters.o \
	$(DLL_OBJDIR)/monsterstate.o \
	$(DLL_OBJDIR)/multiplay_gamerules.o \
	$(DLL_OBJDIR)/pathcorner.o \
	$(DLL_OBJDIR)/plane.o \
	$(DLL_OBJDIR)/plats.o \
	$(DLL_OBJDIR)/player.o \
	$(DLL_OBJDIR)/sdk_c_logic.o \
	$(DLL_OBJDIR)/sdk_c_server.o \
	$(DLL_OBJDIR)/sdk_c_stats.o \
	$(DLL_OBJDIR)/sdk_c_waitlist.o \
	$(DLL_OBJDIR)/sdk_e_menu.o \
	$(DLL_OBJDIR)/sdk_u_main.o \
	$(DLL_OBJDIR)/sdk_u_observer.o \
	$(DLL_OBJDIR)/sdk_w_ak47.o \
	$(DLL_OBJDIR)/sdk_w_aug.o \
	$(DLL_OBJDIR)/sdk_w_deagle.o \
	$(DLL_OBJDIR)/sdk_w_glock.o \
	$(DLL_OBJDIR)/sdk_w_hegrenade.o \
	$(DLL_OBJDIR)/sdk_w_knife.o \
	$(DLL_OBJDIR)/sdk_w_m3.o \
	$(DLL_OBJDIR)/sdk_w_m16.o \
	$(DLL_OBJDIR)/sdk_w_mac10.o \
	$(DLL_OBJDIR)/sdk_w_matrix.o \
	$(DLL_OBJDIR)/sdk_w_mp5.o \
	$(DLL_OBJDIR)/sdk_w_p228.o \
	$(DLL_OBJDIR)/sdk_w_sg550.o \
	$(DLL_OBJDIR)/sdk_w_sg552.o \
	$(DLL_OBJDIR)/sdk_w_tmp.o \
	$(DLL_OBJDIR)/sdk_w_ump45.o \
	$(DLL_OBJDIR)/sdk_w_xm1014.o \
	$(DLL_OBJDIR)/singleplay_gamerules.o \
	$(DLL_OBJDIR)/sound.o \
	$(DLL_OBJDIR)/soundent.o \
	$(DLL_OBJDIR)/spectator.o \
	$(DLL_OBJDIR)/subs.o \
	$(DLL_OBJDIR)/teamplay_gamerules.o \
	$(DLL_OBJDIR)/triggers.o \
	$(DLL_OBJDIR)/util.o \
	$(DLL_OBJDIR)/weapons.o \
	$(DLL_OBJDIR)/world.o \
	$(WPN_SHARED_OBJDIR)/hl_wpn_glock.o \
	$(PM_SHARED_OBJDIR)/pm_debug.o \
	$(PM_SHARED_OBJDIR)/pm_math.o \
	$(PM_SHARED_OBJDIR)/pm_shared.o

$(DLLNAME)_$(ARCH).$(SHLIBEXT) : neat $(OBJ)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) $(LDFLAGS) -o $@ $(OBJ)

neat:
	-mkdir $(DLL_OBJDIR)
	-mkdir $(WPN_SHARED_OBJDIR)
	-mkdir $(PM_SHARED_OBJDIR)
clean:
	-rm -f $(OBJ)
	-rm -f $(DLLNAME)_$(ARCH).$(SHLIBEXT)
spotless: clean
	-rm -r $(DLL_OBJDIR)
	-rm -r $(WPN_SHARED_OBJDIR)
	-rm -r $(PM_SHARED_OBJDIR)

