/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_events.cpp
   This is the event module.

   EVENTS: Half-Life SDK has a good documentation about 
   how events are called and executed. I overlooked the
   document and thought that event scripts had to be 
   written in order for the client and the server to 
   execute the client side prediction. This is not the
   case, only the event file has to be there, so that
   both server and client can precache it and give 
   it a unique 10-bit number. That number is the 
   reference index of events traveling between the 
   client and the DLL. Just like Windows messages, 
   events can come empty handed, or bring in up to
   four parameters, which help break down the message
   even further.
   This file deals with client-side weapon control 
   through events sent by the server DLL.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
\***************************************************/


#include "whichdll.h"
#include "sdk_ev.h"
#include "sdk_w_matrix.h"


#define I_AM_CLIENT
#define MP5_MODULE
#define TMP_MODULE
#define MAC10_MODULE
#define UMP45_MODULE
#define AK47_MODULE
#define SG552_MODULE
#define AUG_MODULE
#define M3_MODULE
#define XM1014_MODULE
#define SG550_MODULE
#define DEAGLE_MODULE
#define GLOCK_MODULE
#define B92D_MODULE
#define M16_MODULE


#include "sdk_w_global.h"

void EV_GetDefaultShellInfo2(float *origin, 
                             float *velocity, 
                             float *ShellVelocity, 
                             float *ShellOrigin, 
                             float *forward, 
                             float *right, 
                             float *up, 
                             const float *start);

int weapon_zoom = 0;

extern vec3_t ev_punchangle;
extern int cam_thirdperson;

float EV_HLDM_PlayTextureSound( int idx, pmtrace_t *ptr, float *vecSrc, float *vecEnd, int iBulletType );
void EV_HLDM_FireBullet( int idx, float *forward, float *right, float *up, float *vecSrc, float *vecDirShooting, float flDistance, int iBulletType, int penetration = 0);

void EV_HLDM_FireBullet( int idx, float *forward, float *right, float *up, float *vecSrc, float *vecDirShooting, float flDistance, int iBulletType, int penetration)
{
   int throughwall = 0;
   int fence = 0, fences = 0;
	pmtrace_t tr;
	vec3_t vecRight, vecUp;

	VectorCopy( right, vecRight );
	VectorCopy( up, vecUp );

	vec3_t vecDir, vecEnd, vecTemp;
	vecDir[0] = vecDirShooting[0];
	vecDir[1] = vecDirShooting[1];
	vecDir[2] = vecDirShooting[2];
	vecEnd[0] = vecSrc[0] + flDistance * vecDir[0];
	vecEnd[1] = vecSrc[1] + flDistance * vecDir[1];
	vecEnd[2] = vecSrc[2] + flDistance * vecDir[2];

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

retrace:
   fence = 0;
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );


	// do damage, paint decals
   //if (tr.pHit->v.takedamage)
	if ( tr.fraction != 1.0 )
		{
		switch(iBulletType)
			{
			default:
			case BULLET_PLAYER_9MM:		
					EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
					fence = EV_HLDM_DecalGunshot( &tr, iBulletType, vecSrc, vecEnd );
				break;
			case BULLET_PLAYER_MP5:		
					EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
					fence = EV_HLDM_DecalGunshot( &tr, iBulletType, vecSrc, vecEnd );
				break;
			case BULLET_PLAYER_BUCKSHOT:
					fence = EV_HLDM_DecalGunshot( &tr, iBulletType, vecSrc, vecEnd );
				break;
			case BULLET_PLAYER_357:
					EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
					fence = EV_HLDM_DecalGunshot( &tr, iBulletType, vecSrc, vecEnd );
				break;
			}
      if (fence && fences < 4)       
         {
         fences++;
        	vecSrc[0] = tr.endpos[0] + vecDir[0] * P_FENCE_WIDTH;
        	vecSrc[1] = tr.endpos[1] + vecDir[1] * P_FENCE_WIDTH;
        	vecSrc[2] = tr.endpos[2] + vecDir[2] * P_FENCE_WIDTH;
         if (gEngfuncs.PM_PointContents( vecSrc, NULL ) != CONTENT_SOLID)
            goto retrace;
         }
      if (penetration > 0 && throughwall < 2)
         {
         throughwall++;
         if (throughwall == 1)
            {
            VectorCopy(vecEnd, vecTemp);
            VectorCopy(vecSrc, vecEnd);
        	   vecSrc[0] = tr.endpos[0] + vecDir[0] * penetration;
        	   vecSrc[1] = tr.endpos[1] + vecDir[1] * penetration;
        	   vecSrc[2] = tr.endpos[2] + vecDir[2] * penetration;
            if (gEngfuncs.PM_PointContents( vecSrc, NULL ) != CONTENT_SOLID)
               goto retrace;
            }
         else
            {
            VectorCopy(vecTemp, vecEnd);
        	   vecSrc[0] = tr.endpos[0] + vecDir[0] * penetration;
        	   vecSrc[1] = tr.endpos[1] + vecDir[1] * penetration;
        	   vecSrc[2] = tr.endpos[2] + vecDir[2] * penetration;
            goto retrace;
            }
         }
		}
   gEngfuncs.pEventAPI->EV_PopPMStates();

}
/*
   MP5 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireMP5NAVY( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {14, 17, 6}, 
               p_start_d[] = {22, 18, 6};
   float pitch, yaw;
	int idx, shell;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL);

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_MP5NAVY].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_MP5NAVY].recoil_yaw, w_stat[WEAPON_MP5NAVY].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_MP5_SHOOT1 + gEngfuncs.pfnRandomLong(0,2), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

   		gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, MP5_SHOOT1, MP5_VOLUME, 
         MP5_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
         /*
   switch( rand() & 1 )
	   {
	   case 0:
   		gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, MP5_SHOOT1, MP5_VOLUME, 
         MP5_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
		   break;
	   default:
   		gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, MP5_SHOOT2, MP5_VOLUME, 
         MP5_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
		break;
	   }
      */

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_MP5NAVY].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_MP5NAVY].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}

/*
   TMP Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireTMP( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {22, 24, 0}, 
               p_start_d[] = {26, 24, 0};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_TMP].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_TMP].recoil_yaw, w_stat[WEAPON_TMP].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_TMP_SHOOT1 + gEngfuncs.pfnRandomLong(0,2), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }

	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 
	switch( rand() & 1 )
	   {
	   case 0:
   		gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, TMP_SHOOT1, TMP_VOLUME, 
         TMP_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
		   break;
	   default:
   		gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, TMP_SHOOT2, TMP_VOLUME, 
         TMP_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
		break;
	   }

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_TMP].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_TMP].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}

/*
   MAC10 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireMAC10( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {20, 24, 0}, 
               p_start_d[] = {24, 24, 0};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_MAC10].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_MAC10].recoil_yaw, w_stat[WEAPON_MAC10].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_MAC10_SHOOT1 + gEngfuncs.pfnRandomLong(0,2), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

   gEngfuncs.pEventAPI->EV_PlaySound
   (idx, origin, CHAN_WEAPON, MAC10_SHOOT1, MAC10_VOLUME, 
   MAC10_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_MAC10].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_MAC10].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}

/*
   UMP45 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireUMP45( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {14, 17, 6}, 
               p_start_d[] = {22, 18, 6};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_UMP45].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_UMP45].recoil_yaw, w_stat[WEAPON_UMP45].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_UMP45_SHOOT1 + gEngfuncs.pfnRandomLong(0,2), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

   gEngfuncs.pEventAPI->EV_PlaySound
   (idx, origin, CHAN_WEAPON, UMP45_SHOOT1, UMP45_VOLUME, 
   UMP45_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_UMP45].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_UMP45].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}


/****************************************\
   RIFLES:
   Functions with wall penetration code.
\****************************************/
/*
   AK47 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireAK47( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {10, 6, 6}, 
               p_start_d[] = {27, 25, 4};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL2);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_AK47].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_AK47].recoil_yaw, w_stat[WEAPON_AK47].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_AK47_SHOOT1 + gEngfuncs.pfnRandomLong(0,2), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

   switch (rand() & 1)
      {
      case 0:
         gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, AK47_SHOOT1, AK47_VOLUME, 
         AK47_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
         break;
      default:
         gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, AK47_SHOOT2, AK47_VOLUME, 
         AK47_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
         break;
      }

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_AK47].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_AK47].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}

/*
   SG552 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireSG552( event_args_t *args )
{
   const float v_start[] =   {20, 18, 8}, 
               v_start_d[] = {20, 17, 8}, 
               p_start[] =   {20, 17, 6}, 
               p_start_d[] = {30, 17, 6};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL2);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_SG552].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_SG552].recoil_yaw, w_stat[WEAPON_SG552].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_SG552_SHOOT1 + gEngfuncs.pfnRandomLong(0,2), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

   switch (rand() & 1)
      {
      case 0:
         gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, SG552_SHOOT1, SG552_VOLUME, 
         SG552_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
         break;
      default:
         gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, SG552_SHOOT2, SG552_VOLUME, 
         SG552_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
         break;
      }

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_SG552].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_SG552].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}

/*
   AUG Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireAUG( event_args_t *args )
{
   const float v_start[] =   {20, 18-3, 8}, 
               v_start_d[] = {20, 14-3, 8}, 
               p_start[] =   {20, 17, 6}, 
               p_start_d[] = {28, 17, 6};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_AUG].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_AUG].recoil_yaw, w_stat[WEAPON_AUG].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_AUG_SHOOT1 + gEngfuncs.pfnRandomLong(0,2), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 
   gEngfuncs.pEventAPI->EV_PlaySound
   (idx, origin, CHAN_WEAPON, AUG_SHOOT1, AUG_VOLUME, 
   AUG_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_AUG].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_AUG].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}


/*
   M3 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireM3( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {20, 17, 6}, 
               p_start_d[] = {30, 17, 6};
	int idx, shell;
	vec3_t origin, angles, velocity, old_angles;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL4);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->angles, old_angles );  
	VectorCopy( args->velocity, velocity );

   AngleVectors( angles, forward, right, up );
	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_M3_SHOOT1 + gEngfuncs.pfnRandomLong(0,1), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 
   gEngfuncs.pEventAPI->EV_PlaySound
   (idx, origin, CHAN_WEAPON, M3_SHOOT1, M3_VOLUME, 
   M3_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

   for (int i = 0; i < 6; i++)
      {
      angles[0] = old_angles[0] + gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_M3].recoil_pitch, w_stat[WEAPON_M3].recoil_pitch);
      angles[1] = old_angles[1] + gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_M3].recoil_yaw, w_stat[WEAPON_M3].recoil_yaw);
	   AngleVectors( angles, forward, right, up );

	   EV_GetGunPosition( args, vecSrc, origin );
	   VectorCopy( forward, vecAiming );

      if ((rand () & 3) == 1)
         {
	      EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                            vecAiming, w_stat[WEAPON_M3].range, 
                            BULLET_PLAYER_MP5, w_stat[WEAPON_M3].penetration);
         }
      else
         {
	      EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                            vecAiming, w_stat[WEAPON_M3].range, 
                            BULLET_PLAYER_MP5, w_stat[WEAPON_M3].penetration);
         }
      }

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_M3].recoil_pitch, w_stat[WEAPON_M3].recoil_pitch);
      ev_punchangle[1] += gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_M3].recoil_yaw, w_stat[WEAPON_M3].recoil_yaw);
      }
}

/*
   XM1014 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireXM1014( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {14, 12, 6}, 
               p_start_d[] = {18, 18, 6};
	int idx, shell;
	vec3_t origin, angles, velocity, old_angles;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL4);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->angles, old_angles );  
	VectorCopy( args->velocity, velocity );
   AngleVectors( angles, forward, right, up );
	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_XM1014_SHOOT1 + gEngfuncs.pfnRandomLong(0,1), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 
   gEngfuncs.pEventAPI->EV_PlaySound
   (idx, origin, CHAN_WEAPON, XM1014_SHOOT1, XM1014_VOLUME, 
   XM1014_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

   for (int i = 0; i < 6; i++)
      {
      angles[0] = old_angles[0] + gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_XM1014].recoil_pitch, w_stat[WEAPON_XM1014].recoil_pitch);
      angles[1] = old_angles[1] + gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_XM1014].recoil_yaw, w_stat[WEAPON_XM1014].recoil_yaw);
	   AngleVectors( angles, forward, right, up );

	   EV_GetGunPosition( args, vecSrc, origin );
	   VectorCopy( forward, vecAiming );

      if ((rand () & 3) == 1)
         {
	      EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                            vecAiming, w_stat[WEAPON_XM1014].range, 
                            BULLET_PLAYER_MP5, w_stat[WEAPON_XM1014].penetration);
         }
      else
         {
	      EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                            vecAiming, w_stat[WEAPON_XM1014].range, 
                            BULLET_PLAYER_MP5, w_stat[WEAPON_XM1014].penetration);
         }
      }

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_XM1014].recoil_pitch, w_stat[WEAPON_XM1014].recoil_pitch);
      ev_punchangle[1] += gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_XM1014].recoil_yaw, w_stat[WEAPON_XM1014].recoil_yaw);
      }
}

/*
   SG550 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireSG550( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {20, 17, 6}, 
               p_start_d[] = {30, 17, 6};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL3);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_SG550].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_SG550].recoil_yaw, w_stat[WEAPON_SG550].recoil_yaw);
   if (args->ducking)
         { pitch *= .4f; yaw *= .4f;}
   if (args->iparam1 == 1)
         { pitch *= .5f; yaw *= .5f;}
   else if (args->iparam1 == 2)
         { pitch *= .2f; yaw *= .2f;}
   else 
         { pitch *= 2.0f; yaw *= 2.0f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_SG550_SHOOT1 + gEngfuncs.pfnRandomLong(0,1), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 
   gEngfuncs.pEventAPI->EV_PlaySound
   (idx, origin, CHAN_WEAPON, SG550_SHOOT1, SG550_VOLUME, 
   SG550_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_SG550].range,
                      BULLET_PLAYER_MP5, w_stat[WEAPON_SG550].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}


/****************************************\
   PISTOLS:
\****************************************/
/****************************************\
   PISTOLS:
\****************************************/
/*
   Desert Eagle Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireDEAGLE( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {22, 24, 0}, 
               p_start_d[] = {26, 24, 0};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_DEAGLE].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_DEAGLE].recoil_yaw, w_stat[WEAPON_DEAGLE].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
      if (args->iparam1 > 0)
		   gEngfuncs.pEventAPI->EV_WeaponAnimation(A_DEAGLE_SHOOT1 + gEngfuncs.pfnRandomLong(0,1), 2 );
      else
		   gEngfuncs.pEventAPI->EV_WeaponAnimation(A_DEAGLE_SHOOT_EMPTY, 2 );
	   }
   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

   switch (rand() & 1)
      {
      case 0:
         gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, DEAGLE_SHOOT1, DEAGLE_VOLUME, 
         DEAGLE_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
         break;
      default:
         gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, DEAGLE_SHOOT2, DEAGLE_VOLUME, 
         DEAGLE_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
         break;
      }

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_DEAGLE].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_DEAGLE].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}


/*
   Glock 18 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireGLOCK18( event_args_t *args )
{
	int idx, shell;
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {22, 24, 0}, 
               p_start_d[] = {26, 24, 0};
   float pitch, yaw;
   //const float s_start[] = {25, -4, 3}, s_start_duck[] = {28, -4, 3};
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_GLOCK18].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_GLOCK18].recoil_yaw, w_stat[WEAPON_GLOCK18].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
      if (args->iparam1 > 0)
		   gEngfuncs.pEventAPI->EV_WeaponAnimation(A_GLOCK_SHOOT3, 2 );
      else
		   gEngfuncs.pEventAPI->EV_WeaponAnimation(A_GLOCK_SHOOT_EMPTY, 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }

   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

   gEngfuncs.pEventAPI->EV_PlaySound
   (idx, origin, CHAN_WEAPON, GLOCK_SHOOT1, GLOCK_VOLUME, 
   GLOCK_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_GLOCK18].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_GLOCK18].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}

/*
   Berreta 92D Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireB92D( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {22, 24, 0}, 
               p_start_d[] = {26, 24, 0};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_92D].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_92D].recoil_yaw, w_stat[WEAPON_92D].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
      if (args->iparam1 > 0)
		   gEngfuncs.pEventAPI->EV_WeaponAnimation(A_B92D_SHOOT1 + gEngfuncs.pfnRandomLong(0,1), 2 );
      else
		   gEngfuncs.pEventAPI->EV_WeaponAnimation(A_B92D_SHOOT_EMPTY, 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL );

   gEngfuncs.pEventAPI->EV_PlaySound
   (idx, origin, CHAN_WEAPON, B92D_SHOOT1, B92D_VOLUME, 
   B92D_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_92D].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_92D].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}

/*
   M16 Firing event:
   This function only handles the first person animation 
   and decals.
*/
void EV_FireM16( event_args_t *args )
{
   const float v_start[] =   {20, 17-3, 8}, 
               v_start_d[] = {20, 13-3, 8},
               p_start[] =   {20, 17, 6}, 
               p_start_d[] = {30, 17, 6};
	int idx, shell;
   float pitch, yaw;
	vec3_t origin, angles, velocity;
	vec3_t ShellVelocity, ShellOrigin;
	vec3_t vecSrc, vecAiming, right, forward, up;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex (SHELL_MODEL2);

   VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
   pitch = -gEngfuncs.pfnRandomFloat(0, w_stat[WEAPON_M16].recoil_pitch);
   yaw = gEngfuncs.pfnRandomFloat(-w_stat[WEAPON_M16].recoil_yaw, w_stat[WEAPON_M16].recoil_yaw);
   if (args->ducking)
         { pitch *= .7f; yaw *= .7f;}
   angles[0] += pitch; angles[1] += yaw;
	AngleVectors( angles, forward, right, up );

	if (EV_IsLocal(idx) && !cam_thirdperson)
	   {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(A_M16_SHOOT1 + gEngfuncs.pfnRandomLong(0,2), 2 );
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start_d);
      else
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, v_start);
	   }
   else
      {  
      if (args->ducking)
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start_d);
      else 
         EV_GetDefaultShellInfo2(origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, p_start);
      }
   EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

   switch (rand() & 1)
      {
      case 0:
         gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, M16_SHOOT1, M16_VOLUME, 
         M16_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
         break;
      default:
         gEngfuncs.pEventAPI->EV_PlaySound
         (idx, origin, CHAN_WEAPON, M16_SHOOT2, M16_VOLUME, 
         M16_ATTN, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
         break;
      }

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_HLDM_FireBullet(idx, forward, right, up, vecSrc, 
                      vecAiming, w_stat[WEAPON_M16].range, 
                      BULLET_PLAYER_MP5, w_stat[WEAPON_M16].penetration);

	if ( EV_IsLocal( idx ) )
      {
      ev_punchangle[0] += pitch;
      ev_punchangle[1] += yaw;
      }
}
