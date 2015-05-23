/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "../hud.h"
#include "../cl_util.h"
#include "event_api.h"

extern "C"
{
// HLDM
void EV_FireGlock1( struct event_args_s *args  );
void EV_FireGlock2( struct event_args_s *args  );
void EV_FireShotGunSingle( struct event_args_s *args  );
void EV_FireShotGunDouble( struct event_args_s *args  );
void EV_FireMP5( struct event_args_s *args  );
void EV_FirePython( struct event_args_s *args  );
void EV_FireGauss( struct event_args_s *args  );
void EV_SpinGauss( struct event_args_s *args  );
void EV_TrainPitchAdjust( struct event_args_s *args );
}


extern "C"
{
void EV_FireMP5NAVY( struct event_args_s *args  );
void EV_FireTMP( struct event_args_s *args  );
void EV_FireMAC10( struct event_args_s *args  );
void EV_FireUMP45( struct event_args_s *args  );
void EV_FireAK47( struct event_args_s *args  );
void EV_FireSG552( struct event_args_s *args  );
void EV_FireAUG( struct event_args_s *args  );
void EV_FireM3( struct event_args_s *args  );
void EV_FireXM1014( struct event_args_s *args  );
void EV_FireSG550( struct event_args_s *args );
void EV_FireDEAGLE( struct event_args_s *args );
void EV_FireGLOCK18( struct event_args_s *args );
void EV_FireB92D( struct event_args_s *args );
void EV_FireM16( struct event_args_s *args  );
}

/*
======================
Game_HookEvents

Associate script file name with callback functions.  Callback's must be extern "C" so
 the engine doesn't get confused about name mangling stuff.  Note that the format is
 always the same.  Of course, a clever mod team could actually embed parameters, behavior
 into the actual .sc files and create a .sc file parser and hook their functionality through
 that.. i.e., a scripting system.

That was what we were going to do, but we ran out of time...oh well.
======================
*/
void Game_HookEvents( void )
{
   /*
	gEngfuncs.pfnHookEvent( "events/glock1.sc",					EV_FireGlock1 );
	gEngfuncs.pfnHookEvent( "events/glock2.sc",					EV_FireGlock2 );
	gEngfuncs.pfnHookEvent( "events/shotgun1.sc",				EV_FireShotGunSingle );
	gEngfuncs.pfnHookEvent( "events/shotgun2.sc",				EV_FireShotGunDouble );
	gEngfuncs.pfnHookEvent( "events/mp5.sc",					   EV_FireMP5 );
	gEngfuncs.pfnHookEvent( "events/python.sc",					EV_FirePython );
	gEngfuncs.pfnHookEvent( "events/gauss.sc",					EV_FireGauss );
	gEngfuncs.pfnHookEvent( "events/gaussspin.sc",				EV_SpinGauss );
	gEngfuncs.pfnHookEvent( "events/train.sc",					EV_TrainPitchAdjust );
   */
   gEngfuncs.pfnHookEvent( "events/mp5n.sc",	   EV_FireMP5NAVY );
   gEngfuncs.pfnHookEvent( "events/tmp.sc",	   EV_FireTMP );
   gEngfuncs.pfnHookEvent( "events/mac10.sc",	EV_FireMAC10 );
   gEngfuncs.pfnHookEvent( "events/ump45.sc",	EV_FireUMP45 );
   gEngfuncs.pfnHookEvent( "events/ak47.sc",	   EV_FireAK47 );
   gEngfuncs.pfnHookEvent( "events/sg552.sc",	EV_FireSG552 );
   gEngfuncs.pfnHookEvent( "events/aug.sc",	   EV_FireAUG );
   gEngfuncs.pfnHookEvent( "events/m3.sc",	   EV_FireM3 );
   gEngfuncs.pfnHookEvent( "events/xm1014.sc",	EV_FireXM1014 );
   gEngfuncs.pfnHookEvent( "events/sg550.sc",	EV_FireSG550 );
   gEngfuncs.pfnHookEvent( "events/magnum.sc",	EV_FireDEAGLE );
   gEngfuncs.pfnHookEvent( "events/b93r.sc",	   EV_FireGLOCK18 );
   gEngfuncs.pfnHookEvent( "events/92f.sc",	   EV_FireB92D );
   gEngfuncs.pfnHookEvent( "events/m16.sc",	   EV_FireM16 );
}