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
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "entity_types.h"
#include "usercmd.h"
#include "pm_defs.h"
#include "pm_materials.h"

#include "eventscripts.h"
#include "ev_hldm.h"

#include "r_efx.h"
#include "event_api.h"
#include "event_args.h"
#include "in_defs.h"

#include <string.h>

static int tracerCount[ 32 ];

extern "C" char PM_FindTextureType( char *name );

void V_PunchAxis( int axis, float punch );

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

#define VECTOR_CONE_1DEGREES Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES Vector( 0.06105, 0.06105, 0.06105 )	
#define VECTOR_CONE_8DEGREES Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES Vector( 0.17365, 0.17365, 0.17365 )

// play a strike sound based on the texture that was hit by the attack traceline.  VecSrc/VecEnd are the
// original traceline endpoints used by the attacker, iBulletType is the type of bullet that hit the texture.
// returns volume of strike instrument (crowbar) to play
enum
{ 
   TEX_EFF_ELSE = 2,
   TEX_EFF_METAL,
   TEX_EFF_NONE
};
void VectorAngles( const float *forward, float *angles );
void EV_EFFECT_WALLPUFF(pmtrace_t *pTrace,int iSprite);
void EV_WALLPUFF(pmtrace_t *pTrace,char chTextureType)
{
   int iEffectType=NULL;
   int iSpriteIdx=NULL;
   iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/wall_puff1.spr" );
   /*
   switch(chTextureType)
      {
      case CHAR_TEX_CONCRETE:
      case CHAR_TEX_TILE:
         switch(rand() & 3)
            {
            case 0:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/wall_puff1.spr" );
            break;
            case 1:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/wallpuff1.spr" );
            break;
            case 2:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/wallpuff2.spr" );
            break;
            default:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/wallpuff3.spr" );
            }
         iEffectType=TEX_EFF_ELSE;
         break;
      case CHAR_TEX_DIRT:
         switch(rand() & 3)
            {
            case 0:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/dirtpuff.spr" );
            break;
            case 1:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/woodpuff1.spr" );
            break;
            case 2:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/woodpuff2.spr" );
            break;
            default:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/woodpuff3.spr" );
            }
         iEffectType=TEX_EFF_ELSE;
         break;
      case CHAR_TEX_WOOD:
      case CHAR_TEX_GRATE:
         switch(rand() & 3)
            {
            case 0:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/dirtpuff.spr" );
               break;
            case 1:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/dirtpuff1.spr" );
               break;
            case 2:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/dirtpuff2.spr" );
               break;
            default:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/dirtpuff3.spr" );
            }
         iEffectType=TEX_EFF_ELSE;
         break;
      case CHAR_TEX_FLESH:
         switch(rand() & 3)
            {
            case 0:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/bloodpuff.spr" );
            break;
            case 1:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/bloodpuff1.spr" );
            break;
            case 2:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/bloodpuff2.spr" );
            break;
            default:
               iSpriteIdx =gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/bloodpuff3.spr" );
            }
         iEffectType=TEX_EFF_ELSE;
         break;
      case CHAR_TEX_SLOSH:
      case CHAR_TEX_GLASS:
         iEffectType=TEX_EFF_NONE;
         break;
      case CHAR_TEX_METAL:
      case CHAR_TEX_VENT:
      case CHAR_TEX_COMPUTER:
         iEffectType=TEX_EFF_METAL;
         break;
      }

   switch(iEffectType)
      {
      case TEX_EFF_METAL:
         gEngfuncs.pEfxAPI->R_SparkStreaks(pTrace->endpos,15,-140,140);
         break;
      case TEX_EFF_ELSE:
         EV_EFFECT_WALLPUFF(pTrace, iSpriteIdx);
         break;
      case TEX_EFF_NONE:
         break;
      }
   */
   EV_EFFECT_WALLPUFF(pTrace, iSpriteIdx);
}

void EV_EFFECT_WALLPUFF(pmtrace_t *pTrace,int iSprite)
{
   Vector angles, forward, right, up; 
   VectorAngles( pTrace->plane.normal, angles ); 
   AngleVectors( angles, forward,up, right ); 
   forward.z = forward.z * -1; 

   TEMPENTITY *pWallpuff = NULL;
   pWallpuff = gEngfuncs.pEfxAPI->R_TempSprite( pTrace->endpos + pTrace->plane.normal * 3, 
                                                forward * gEngfuncs.pfnRandomFloat(10, 30) 
                                                + right * gEngfuncs.pfnRandomFloat(-12, 12) 
                                                + up * gEngfuncs.pfnRandomFloat(-12, 12), 
                                                0.5, iSprite, kRenderTransAdd, 
                                                0, 50, 25, FTENT_SPRANIMATE);
   if (pWallpuff) 
      {
	   pWallpuff->entity.curstate.framerate = 25; 
	   pWallpuff->entity.curstate.renderamt = 70;
      }
}



float EV_HLDM_PlayTextureSound( int idx, pmtrace_t *ptr, float *vecSrc, float *vecEnd, int iBulletType )
{
	// hit the world, try to play sound based on texture material type
	char chTextureType = CHAR_TEX_CONCRETE;
	float fvol;
	float fvolbar;
	char *rgsz[4];
	int cnt;
	float fattn = ATTN_NORM;
	int entity;
	char *pTextureName;
	char texname[ 64 ];
	char szbuffer[ 64 ];

	entity = gEngfuncs.pEventAPI->EV_IndexFromTrace( ptr );

	// FIXME check if playtexture sounds movevar is set
	//

	chTextureType = 0;

	// Player
	if ( entity >= 1 && entity <= gEngfuncs.GetMaxClients() )
	{
		// hit body
		chTextureType = CHAR_TEX_FLESH;
	}
	else if ( entity == 0 )
	{
		// get texture from entity or world (world is ent(0))
		pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture( ptr->ent, vecSrc, vecEnd );
		
		if ( pTextureName )
		{
			strcpy( texname, pTextureName );
			pTextureName = texname;

			// strip leading '-0' or '+0~' or '{' or '!'
			if (*pTextureName == '-' || *pTextureName == '+')
			{
				pTextureName += 2;
			}

			if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
			{
				pTextureName++;
			}
			
			// '}}'
			strcpy( szbuffer, pTextureName );
			szbuffer[ CBTEXTURENAMEMAX - 1 ] = 0;
				
			// get texture type
			chTextureType = PM_FindTextureType( szbuffer );	
		}
	}
	
	switch (chTextureType)
	{
	default:
	case CHAR_TEX_CONCRETE: fvol = 0.9;	fvolbar = 0.6;
		rgsz[0] = "player/pl_step1.wav";
		rgsz[1] = "player/pl_step2.wav";
		rgsz[2] = "player/pl_step3.wav";
		rgsz[3] = "player/pl_step4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_METAL: fvol = 0.9; fvolbar = 0.3;
		rgsz[0] = "player/pl_metal1.wav";
		rgsz[1] = "player/pl_metal2.wav";
		rgsz[2] = "player/pl_metal3.wav";
		rgsz[3] = "player/pl_metal4.wav";
		cnt = 2;
		break;
	case CHAR_TEX_DIRT:	fvol = 0.9; fvolbar = 0.1;
		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		rgsz[3] = "player/pl_dirt3.wav";
		cnt = 4;
		break;
	case CHAR_TEX_VENT:	fvol = 0.5; fvolbar = 0.3;
		rgsz[0] = "player/pl_duct1.wav";
		rgsz[1] = "player/pl_duct1.wav";
		cnt = 2;
		break;
	case CHAR_TEX_GRATE: fvol = 0.9; fvolbar = 0.5;
		rgsz[0] = "player/pl_grate1.wav";
		rgsz[1] = "player/pl_grate2.wav";
		rgsz[2] = "player/pl_grate3.wav";
		rgsz[3] = "player/pl_grate4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_TILE:	fvol = 0.8; fvolbar = 0.2;
		rgsz[0] = "player/pl_tile1.wav";
		rgsz[1] = "player/pl_tile3.wav";
		rgsz[2] = "player/pl_tile2.wav";
		rgsz[3] = "player/pl_tile4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_SLOSH: fvol = 0.9; fvolbar = 0.0;
		rgsz[0] = "player/pl_slosh1.wav";
		rgsz[1] = "player/pl_slosh3.wav";
		rgsz[2] = "player/pl_slosh2.wav";
		rgsz[3] = "player/pl_slosh4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_WOOD: fvol = 0.9; fvolbar = 0.2;
		rgsz[0] = "debris/wood1.wav";
		rgsz[1] = "debris/wood2.wav";
		rgsz[2] = "debris/wood3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_GLASS:
	case CHAR_TEX_COMPUTER:
		fvol = 0.8; fvolbar = 0.2;
		rgsz[0] = "debris/glass1.wav";
		rgsz[1] = "debris/glass2.wav";
		rgsz[2] = "debris/glass3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_FLESH:
		if (iBulletType == BULLET_PLAYER_CROWBAR)
			return 0.0; // crowbar already makes this sound
		fvol = 1.0;	fvolbar = 0.2;
		rgsz[0] = "weapons/bullet_hit1.wav";
		rgsz[1] = "weapons/bullet_hit2.wav";
		fattn = 1.0;
		cnt = 2;
		break;
	}

   //EV_WALLPUFF(ptr, chTextureType);
	// play material hit sound
	gEngfuncs.pEventAPI->EV_PlaySound( 0, ptr->endpos, CHAN_STATIC, rgsz[gEngfuncs.pfnRandomLong(0,cnt-1)], fvol, fattn, 0, 96 + gEngfuncs.pfnRandomLong(0,0xf) );
	return fvolbar;
}

char *EV_HLDM_DamageDecal( physent_t *pe )
{
	static char decalname[ 32 ];
	int idx;

	if ( pe->classnumber == 1 )
	   {
		idx = gEngfuncs.pfnRandomLong( 0, 2 );
		sprintf( decalname, "{break%i", idx + 1 );
   	}
	else if ( pe->rendermode != kRenderNormal )
	   {
		sprintf( decalname, "{bproof1" );
   	}
	else
	   {
		idx = gEngfuncs.pfnRandomLong( 0, 4 );
		sprintf( decalname, "{shot%i", idx + 1 );
	   }
	return decalname;
}

void EV_HLDM_GunshotDecalTrace( pmtrace_t *pTrace, char *decalName )
{
	int iRand;
	physent_t *pe;

	gEngfuncs.pEfxAPI->R_BulletImpactParticles( pTrace->endpos );

	iRand = gEngfuncs.pfnRandomLong(0,0x7FFF);
	if ( iRand < (0x7fff/2) )// not every bullet makes a sound.
	   {
		switch( iRand % 5)
		   {
		   case 0:	gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
		   case 1:	gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
		   case 2:	gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
		   case 3:	gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric4.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
		   case 4:	gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric5.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
		   }
	   }

	pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );

	// Only decal brush models such as the world etc.
	if (  decalName && decalName[0] && pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
	   {
		if ( CVAR_GET_FLOAT( "r_decals" ) )
		   {
         
			gEngfuncs.pEfxAPI->R_DecalShoot( 
				gEngfuncs.pEfxAPI->Draw_DecalIndex( gEngfuncs.pEfxAPI->Draw_DecalIndexFromName( decalName ) ), 
				gEngfuncs.pEventAPI->EV_IndexFromTrace( pTrace ), 0, pTrace->endpos, 0 );

         if (decalName[1] == 's' && !(rand() & 3))
            {
            int modelindex;
		      char *spritename = "sprites/wall_puff1.spr"; //name of your sprite
		   
		      modelindex = gEngfuncs.pEventAPI->EV_FindModelIndex ( spritename );
   	   
		      vec3_t velocity;
		      VectorCopy( pTrace->endpos, velocity);
		      velocity = velocity.Normalize() * 16; 
 
		      vec3_t origin;
		      VectorMA (pTrace->endpos, .5, pTrace->plane.normal, origin);
		      VectorScale (pTrace->plane.normal, 8, velocity); //don't clip in the wall
		   
		      velocity[2] += 1; //make the wall puff drift upwards
		      gEngfuncs.pEfxAPI->R_TempSprite (origin , velocity , .4, modelindex, kRenderTransAdd , 0, 0.07, gEngfuncs.pfnRandomFloat( .1, .6 ), FTENT_SPRANIMATE | FTENT_FADEOUT);
            }
		   }
	   }
}

int EV_HLDM_DecalGunshot( pmtrace_t *pTrace, int iBulletType, float *vecSrc, float *vecEnd )
{
   //static int smoke_puff = 0;
   //if (!smoke_puff)
      //smoke_puff = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/smokepuff.spr" );
	physent_t *pe;


	pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );
	if ( pe && pe->solid == SOLID_BSP )
	   {
      char *pTextureName;
		pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture( pTrace->ent, vecSrc, vecEnd );		
		if ( pTextureName )
		   {
			if (*pTextureName == '{' || *pTextureName == '!')
				return 1;
         }
      EV_HLDM_GunshotDecalTrace( pTrace, EV_HLDM_DamageDecal( pe ) );
	   }
   return 0;
}

int EV_HLDM_CheckTracer( int idx, float *vecSrc, float *end, float *forward, float *right, int iBulletType, int iTracerFreq, int *tracerCount )
{
	int tracer = 0;
	int i;
	qboolean player = idx >= 1 && idx <= gEngfuncs.GetMaxClients() ? true : false;

	if ( iTracerFreq != 0 && ( (*tracerCount)++ % iTracerFreq) == 0 )
	{
		vec3_t vecTracerSrc;

		if ( player )
		{
			vec3_t offset( 0, 0, -4 );

			// adjust tracer position for player
			for ( i = 0; i < 3; i++ )
			{
				vecTracerSrc[ i ] = vecSrc[ i ] + offset[ i ] + right[ i ] * 2 + forward[ i ] * 16;
			}
		}
		else
		{
			VectorCopy( vecSrc, vecTracerSrc );
		}
		
		if ( iTracerFreq != 1 )		// guns that always trace also always decal
			tracer = 1;

		switch( iBulletType )
		{
		case BULLET_PLAYER_MP5:
		case BULLET_MONSTER_MP5:
		case BULLET_MONSTER_9MM:
		case BULLET_MONSTER_12MM:
		default:
			EV_CreateTracer( vecTracerSrc, end );
			break;
		}
	}

	return tracer;
}

/*
================
FireBullets

Go to the trouble of combining multiple pellets into a single damage call.
================
*/
void EV_HLDM_FireBullets( int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount )
{

}

void EV_FireGlock1( event_args_t *args )
{
}

void EV_FireGlock2( event_args_t *args )
{
}

void EV_FireShotGunDouble( event_args_t *args )
{
}

void EV_FireShotGunSingle( event_args_t *args )
{
}
void EV_FirePython( event_args_t *args )
{
}

#define SND_CHANGE_PITCH	(1<<7)		// duplicated in protocol.h change sound pitch

void EV_SpinGauss( event_args_t *args )
{
}

/*
==============================
EV_StopPreviousGauss

==============================
*/
void EV_StopPreviousGauss( int idx )
{
}

void EV_FireGauss( event_args_t *args )
{
}

void EV_TrainPitchAdjust( event_args_t *args )
{
	int idx;
	vec3_t origin;

	unsigned short us_params;
	int noise;
	float m_flVolume;
	int pitch;
	int stop;
	
	char sz[ 256 ];

	idx = args->entindex;
	
	VectorCopy( args->origin, origin );

	us_params = (unsigned short)args->iparam1;
	stop	  = args->bparam1;

	m_flVolume	= (float)(us_params & 0x003f)/40.0;
	noise		= (int)(((us_params) >> 12 ) & 0x0007);
	pitch		= (int)( 10.0 * (float)( ( us_params >> 6 ) & 0x003f ) );

	switch ( noise )
	{
	case 1: strcpy( sz, "plats/ttrain1.wav"); break;
	case 2: strcpy( sz, "plats/ttrain2.wav"); break;
	case 3: strcpy( sz, "plats/ttrain3.wav"); break; 
	case 4: strcpy( sz, "plats/ttrain4.wav"); break;
	case 5: strcpy( sz, "plats/ttrain6.wav"); break;
	case 6: strcpy( sz, "plats/ttrain7.wav"); break;
	default:
		// no sound
		strcpy( sz, "" );
		return;
	}

	if ( stop )
	{
		gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, sz );
	}
	else
	{
		gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_STATIC, sz, m_flVolume, ATTN_NORM, SND_CHANGE_PITCH, pitch );
	}
}

int EV_TFC_IsAllyTeam( int iTeam1, int iTeam2 )
{
	return 0;
}

