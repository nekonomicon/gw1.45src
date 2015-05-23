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
// shared event functions
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"

#include "r_efx.h"

#include "eventscripts.h"
#include "event_api.h"

/*
=================
GetEntity

Return's the requested cl_entity_t
=================
*/
struct cl_entity_s *GetEntity( int idx )
{
	return gEngfuncs.GetEntityByIndex( idx );
}

/*
=================
GetViewEntity

Return's the current weapon/view model
=================
*/
struct cl_entity_s *GetViewEntity( void )
{
	return gEngfuncs.GetViewModel();
}

/*
=================
EV_CreateTracer

Creates a tracer effect
=================
*/
void EV_CreateTracer( float *start, float *end )
{
	gEngfuncs.pEfxAPI->R_TracerEffect( start, end );
}

/*
=================
EV_IsPlayer

Is the entity's index in the player range?
=================
*/
qboolean EV_IsPlayer( int idx )
{
	if ( idx >= 1 && idx <= gEngfuncs.GetMaxClients() )
		return true;

	return false;
}

/*
=================
EV_IsLocal

Is the entity == the local player
=================
*/
qboolean EV_IsLocal( int idx )
{
	return gEngfuncs.pEventAPI->EV_IsLocal( idx - 1 ) ? true : false;
}

/*
=================
EV_GetGunPosition

Figure out the height of the gun
=================
*/
void EV_GetGunPosition( event_args_t *args, float *pos, float *origin )
{
	int idx;
	vec3_t view_ofs;

	idx = args->entindex;
	VectorClear( view_ofs );
	view_ofs[2] = DEFAULT_VIEWHEIGHT;

	if ( EV_IsPlayer( idx ) )
	   {
		if ( EV_IsLocal( idx ) )
		   {
			// Grab predicted result for local player
			gEngfuncs.pEventAPI->EV_LocalPlayerViewheight( view_ofs );
		   }
		else if ( args->ducking == 1 )
		   {
			view_ofs[2] = VEC_DUCK_VIEW;
		   }
	   }
	VectorAdd( origin, view_ofs, pos );
}

/*
=================
EV_EjectBrass

Bullet shell casings
=================
*/
void EV_EjectBrass( float *origin, float *velocity, float rotation, int model, int soundtype )
{
	vec3_t endpos;
	VectorClear( endpos );
	endpos[1] = rotation;
	gEngfuncs.pEfxAPI->R_TempModel( origin, velocity, endpos, 2.5, model, soundtype );
}

/*
=================
EV_GetDefaultShellInfo

Determine where to eject shells from
=================
*/
typedef enum
	{
	at_notice,
	at_console,		// same as at_notice, but forces a ConPrintf, not a message box
	at_aiconsole,	// same as at_console, but only shown if developer level is 2!
	at_warning,
	at_error,
	at_logged		// Server print to console ( only in multiplayer games ).
	} ALERT_TYPE;
void AlertMessage( ALERT_TYPE atype, char *szFmt, ... );
extern int cam_thirdperson;
void EV_GetDefaultShellInfo2(float *origin, 
                             float *velocity, 
                             float *ShellVelocity, 
                             float *ShellOrigin, 
                             float *forward, 
                             float *right, 
                             float *up, 
                             const float *start)
{
	int i;
	vec3_t s_scale;
	float fR, fU;
   VectorCopy(start, s_scale);
	fR = -gEngfuncs.pfnRandomFloat( 0, 50 );
	fU = gEngfuncs.pfnRandomFloat( 10, 75 );
   //fU = 0;//-10;
   //fR = 0;//10;
	for ( i = 0; i < 3; i++ )
	   {
		ShellVelocity[i] = velocity[i] + right[i] * fR + up[i] * fU; //+ forward[i] * 25;
		ShellOrigin[i]   = origin[i] 
                       + forward[i] * s_scale[0]
                       + up[i] * s_scale[1]
                       + right[i] * s_scale[2];
	   }
}

void EV_GetDefaultShellInfo( event_args_t *args, float *origin, float *velocity, float *ShellVelocity, float *ShellOrigin, float *forward, float *right, float *up, float forwardScale, float upScale, float rightScale )
{
	int i;
	vec3_t view_ofs;
	float fR, fU;

	int idx;

	idx = args->entindex;

	VectorClear( view_ofs );
	view_ofs[2] = DEFAULT_VIEWHEIGHT;

   /*
	if ( EV_IsPlayer( idx ) )
	   {
		if ( )
			gEngfuncs.pEventAPI->EV_LocalPlayerViewheight( view_ofs );
		else if ( args->ducking == 1 )
			view_ofs[2] = VEC_DUCK_VIEW;
	   }
   */
   /*
   gEngfuncs.pEventAPI->EV_LocalPlayerViewheight( view_ofs );
   char x[32];
   sprintf (x, "%.2f", view_ofs[2]);
   gEngfuncs.pfnCenterPrint(x);
   view_ofs[2] = 16;
   */
   if (cam_thirdperson || !EV_IsPlayer(idx) || !EV_IsLocal(idx))
      {
      if ( args->ducking == 1 ) view_ofs[2] = VEC_DUCK_VIEW;
      }
   else if ( args->ducking == 1 ) view_ofs[2] = 12;
      

	fR = -gEngfuncs.pfnRandomFloat( 20, 120 );
	fU = gEngfuncs.pfnRandomFloat( 10, 75 );

	for ( i = 0; i < 3; i++ )
	   {
		ShellVelocity[i] = velocity[i] + right[i] * fR + up[i] * fU; //+ forward[i] * 25;
		ShellOrigin[i]   = origin[i] + view_ofs[i] + up[i] * upScale + forward[i] * forwardScale + right[i] * -rightScale;
	   }
}

/*
=================
EV_MuzzleFlash

Flag weapon/view model for muzzle flash
=================
*/
void EV_MuzzleFlash( void )
{
   return;
	// Add muzzle flash to current weapon model
	cl_entity_t *ent = GetViewEntity();
	if ( !ent )
	{
		return;
	}

	// Or in the muzzle flash
	ent->curstate.effects |= EF_MUZZLEFLASH;
}