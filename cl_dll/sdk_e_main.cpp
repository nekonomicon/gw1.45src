/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_e_main.cpp 
   This is the smoke module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include <memory.h>

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_types.h"
#include "studio_event.h" // def. of mstudioevent_t
#include "r_efx.h"
#include "event_api.h"
#include "pm_defs.h"
#include "pmtrace.h"	

#define DLLEXPORT __declspec( dllexport )

extern vec3_t v_origin;
extern CHud gHUD;

#define TENT_SMOKE_LIFE  24.0f
#define TENT_SMOKE_DIE   6.0f

void SmokeEntCallback ( struct tempent_s *ent, float frametime, float currenttime )
{
	int i;
   float timeleft = ent->die - gEngfuncs.GetClientTime();

	for ( i = 0; i < 3; i++ )
	   {
		ent->entity.curstate.origin[ i ] += ent->entity.baseline.origin[ i ] * frametime;
	   }

   if (ent->entity.curstate.frame < 15)
      ent->entity.curstate.frame++;

   if (timeleft < TENT_SMOKE_DIE)
      {
      ent->entity.curstate.renderamt = (float) (timeleft/TENT_SMOKE_DIE) * 255.0f;
      }
   if (ent->entity.curstate.scale < 10)
      ent->entity.curstate.scale += 0.03f;
}

void SmokeEnts(vec3_t src_origin)
{
	static float lasttime;
	float curtime;
	
	curtime = gEngfuncs.GetClientTime();

	//if ( ( curtime - lasttime ) < 10.0 )
		//return;

	lasttime = curtime;

	TEMPENTITY *p;
	int i, j;
	struct model_s *mod;
	vec3_t origin;
	int index;

   //gHUD.m_SayText.SayTextPrint( "Here", 4 );
	mod = gEngfuncs.CL_LoadModel( "sprites/black_smoke3.spr", &index );
	//mod = gEngfuncs.CL_LoadModel( "sprites/smokepuff.spr", &index );
	//mod = gEngfuncs.CL_LoadModel( "sprites/steam1.spr", &index );
   /*
   if (!mod)
      {
      gHUD.m_SayText.SayTextPrint( "Problem", 4, -1, 0 );
      return;
      }
   */
   
   
	for ( i = 0; i < 5; i++ )
	   {
		for ( j = 0; j < 3; j++ )
			origin[ j ] = src_origin[ j ];

		p = gEngfuncs.pEfxAPI->CL_TentEntAllocCustom( (float *)&origin, mod, 1, SmokeEntCallback );
		if ( !p )
			break;

      p->entity.curstate.renderfx = kRenderFxSmoke;
      p->entity.curstate.rendermode = kRenderTransAlpha;
      p->entity.curstate.renderamt = 255;

      //p->entity.curstate.animtime = gEngfuncs.GetClientTime();
      p->entity.curstate.scale = 1;
   	p->entity.curstate.origin[ 0 ] = origin[ 0 ];
		p->entity.baseline.origin[ 0 ] = gEngfuncs.pfnRandomFloat( -10, 10 );
   	p->entity.curstate.origin[ 1 ] = origin[ 1 ];
		p->entity.baseline.origin[ 1 ] = gEngfuncs.pfnRandomFloat( -10, 10 );
   	p->entity.curstate.origin[ 2 ] = origin[ 2 ];
		p->entity.baseline.origin[ 2 ] = gEngfuncs.pfnRandomFloat( 5, 17 );

		// p->die is set to current time so all you have to do is add an additional time to it
		p->die += TENT_SMOKE_LIFE;
	   }
}

