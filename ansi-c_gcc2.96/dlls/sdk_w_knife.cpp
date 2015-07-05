/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_knife.cpp 
   This is the knife module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"


#define	CROWBAR_BODYHIT_VOLUME 128
#define	CROWBAR_WALLHIT_VOLUME 512

#define  KNIFE_DAMAGE_LENGTH    64



enum knife_e {
	KNIFE_IDLE = 0,
	KNIFE_SLASH1,
	KNIFE_SLASH2,
	KNIFE_DRAW,
	KNIFE_STAB,
	KNIFE_STABMISS,
	KNIFE_MIDSLASH1,
	KNIFE_MIDSLASH2
};

#define WEAPON_V     "models/weapons/v_knife_r.mdl"
#define WEAPON_W     "models/weapons/w_knife.mdl"
#define WEAPON_P     "models/weapons/p_knife.mdl"

#define WEAPON_DEPLOY   "weapons/knife_deploy1.wav"
#define WEAPON_HIT1     "weapons/knife_hit1.wav"
#define WEAPON_HIT2     "weapons/knife_hit2.wav"
#define WEAPON_HIT3     "weapons/knife_hit3.wav"
#define WEAPON_HIT4     "weapons/knife_hit4.wav"
#define WEAPON_HITWALL  "weapons/knife_hitwall1.wav"
#define WEAPON_SLASH1   "weapons/knife_slash1.wav"
#define WEAPON_SLASH2   "weapons/knife_slash2.wav"
#define WEAPON_STAB     "weapons/knife_stab.wav"



class KNIFE : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 1; }
	void EXPORT SwingAgain( void );
	void EXPORT Smack( void );
	int GetItemInfo(ItemInfo *p);
   void Slash();
   void Stab();

	void PrimaryAttack( void );
   void SecondaryAttack();
	int Swing( int fFirst );
	BOOL Deploy( void );
	//void Holster( int skiplocal = 0 );
	int m_iSwing;
   float m_fFirstSlice;
   float m_fLastSlice;
   int m_iCurrentSlice;
	TraceResult m_trHit;
};
LINK_ENTITY_TO_CLASS( weapon_knife, KNIFE );

void KNIFE::Spawn( )
{
   pev->classname = MAKE_STRING("weapon_knife");
	Precache( );
   dropmodel = MAKE_STRING(WEAPON_W);
	m_iId = WEAPON_KNIFE;
	SET_MODEL(ENT(pev), WEAPON_W);
	m_iClip = -1;

   m_fFirstSlice = 0;
   m_fLastSlice = 0;
   m_iCurrentSlice = 0;

	FallInit();// get ready to fall down.
}


void KNIFE::Precache( void )
{
	PRECACHE_MODEL(WEAPON_V);
	PRECACHE_MODEL(WEAPON_W);
	PRECACHE_MODEL(WEAPON_P);

	PRECACHE_SOUND(WEAPON_DEPLOY);
	PRECACHE_SOUND(WEAPON_HIT1);
	PRECACHE_SOUND(WEAPON_HIT2);
	PRECACHE_SOUND(WEAPON_HIT3);
	PRECACHE_SOUND(WEAPON_HIT4);
	PRECACHE_SOUND(WEAPON_HITWALL);
	PRECACHE_SOUND(WEAPON_SLASH1);
	PRECACHE_SOUND(WEAPON_SLASH2);
	PRECACHE_SOUND(WEAPON_STAB);
}

int KNIFE::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 2;
	p->iPosition = 1;
	p->iId = WEAPON_KNIFE;
	p->iWeight = WEIGHT_MELEE;
	return 1;
}

BOOL KNIFE::Deploy( )
{
   if (m_pPlayer->m_iPrimary)
      m_pPlayer->m_iMaxSpeed = (300 + w_stat[m_pPlayer->m_iPrimary].speed) / 2;
   //EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_DEPLOY, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
	return DefaultDeploy( WEAPON_V, WEAPON_P, KNIFE_DRAW, "knife" );
}


void KNIFE::PrimaryAttack()
{
   if (m_bTriggerReleased)
      m_bTriggerReleased = false;
   else
      return;

   Slash();
   m_flNextPrimaryAttack = gpGlobals->time + .3;
   if (gpGlobals->time < m_fLastSlice)
      {
      if (gpGlobals->time + 3 > m_fLastSlice)
         {
         m_iCurrentSlice++;
         if (m_iCurrentSlice > 4)
            {
            switch (rand() & 3)
               {
               case 0:
                  EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_VOICE, "player/knife1.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
                  break;
               case 1:
                  EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_VOICE, "player/knife2.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
                  break;
               default:
                  EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_VOICE, "player/knife3.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
                  break;
               }
            m_iCurrentSlice = 0;
            m_fLastSlice = gpGlobals->time + 8;
            }
         }
      }     
   if (m_fLastSlice - gpGlobals->time < 3)
      m_fLastSlice = gpGlobals->time + 1.0f;
}

void KNIFE::Slash()
{
#ifndef LOCAL_WEAPON
   TraceResult tr;
   //PLAYBACK_EVENT( 1, m_pPlayer->edict(), m_usKNIFE);
	Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );
	Vector vecSrc = m_pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
	Vector vecDir = gpGlobals->v_forward;
 
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
   UTIL_TraceLine(vecSrc, vecSrc + vecDir * KNIFE_DAMAGE_LENGTH, dont_ignore_monsters, m_pPlayer->edict(), &tr);
   /*
   if ( tr.flFraction != 1.0 && !FNullEnt( tr.pHit) )
      {
      if ( tr.pHit->v.takedamage )
	      {
	      ClearMultiDamage( );
	      CBaseEntity::Instance(tr.pHit)->TraceAttack(m_pPlayer->pev, 40, vecDir, &tr, DMG_BULLET | DMG_NEVERGIB ); 
	      ApplyMultiDamage( pev, m_pPlayer->pev );
         }
      }
   */
#else
   /*HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usKNIFE, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );*/
#endif
	if ( tr.flFraction >= 1.0 )
      {
      switch (rand () & 3)
         {
         case 0:
		      SendWeaponAnim( KNIFE_SLASH1 );
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_SLASH1, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
            break;
         case 1:
		      SendWeaponAnim( KNIFE_SLASH2 );
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_SLASH2, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
            break;
         case 2:
		      SendWeaponAnim( KNIFE_MIDSLASH2 );
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_SLASH1, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
            break;
         default:
		      SendWeaponAnim( KNIFE_MIDSLASH2 );
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_SLASH2, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
            break;
         } 
      }
   else
      {
      if ( !FNullEnt( tr.pHit) && tr.pHit->v.takedamage )
	      {
	      ClearMultiDamage( );
         CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
         if (!pEntity) 
            {
            UTIL_LogPrintf("Crashed @ 007\n");
            return;
            }
	      pEntity->TraceAttack(m_pPlayer->pev, 70, vecDir, &tr, DMG_BULLET | DMG_NEVERGIB ); 
	      ApplyMultiDamage( pev, m_pPlayer->pev );
         switch (rand () & 3)
            {
            case 0:
		         SendWeaponAnim( KNIFE_SLASH1 );
               EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_HIT1, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
               break;
            case 1:
		         SendWeaponAnim( KNIFE_SLASH2 );
               EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_HIT2, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
               break;
            case 2:
		         SendWeaponAnim( KNIFE_MIDSLASH1 );
               EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_HIT3, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
               break;
            default:
		         SendWeaponAnim( KNIFE_MIDSLASH2 );
               EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_HIT4, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
               break;
            }
	      }
      else
	      {
         switch (rand () & 1)
            {
            case 1:
		         SendWeaponAnim( KNIFE_SLASH1 );
               break;
            default:
		         SendWeaponAnim( KNIFE_SLASH2 );
               break;
            }
         EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_HITWALL, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
         DecalGunshot( &tr, BULLET_PLAYER_CROWBAR );
	      }
      }
}


void KNIFE::SecondaryAttack()
{
   m_flNextSecondaryAttack = gpGlobals->time + 1.0;
   if (m_bTriggerReleased)
      m_bTriggerReleased = false;
   else
      return;
   Stab();
}

void KNIFE::Stab()
{
	TraceResult tr;
	Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );
	Vector vecSrc = m_pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
	Vector vecDir = gpGlobals->v_forward;
   m_pPlayer->pev->punchangle.x += 4;
   m_pPlayer->pev->punchangle.y += RANDOM_FLOAT(-.4, .4);
 
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
   UTIL_TraceLine(vecSrc, vecSrc + vecDir * KNIFE_DAMAGE_LENGTH, dont_ignore_monsters, m_pPlayer->edict(), &tr);
	if ( tr.flFraction >= 1.0 )
      {
      SendWeaponAnim( KNIFE_STABMISS );
      switch (rand () & 1)
         {
         case 1:
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_SLASH1, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
            break;
         default:
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_SLASH2, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
            break;
         } 
      }
   else
      {
      if ( tr.flFraction != 1.0)      
         {
         if ( !FNullEnt( tr.pHit) && tr.pHit->v.takedamage )
	         {
	         Vector myForward, hisForward;
            float fromBehind;
	         UTIL_MakeVectors( m_pPlayer->pev->angles );
            myForward = gpGlobals->v_forward;
	         UTIL_MakeVectors(tr.pHit->v.angles );
            hisForward = gpGlobals->v_forward;
	         ClearMultiDamage( );
            fromBehind = DotProduct( myForward, hisForward );
            if (fromBehind > .5) 
               {
               CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
               if (!pEntity) 
                  {
                  UTIL_LogPrintf("Crashed @ 008\n");
                  return;
                  }
	            pEntity->TraceAttack(m_pPlayer->pev, 140, vecDir, &tr, DMG_BULLET | DMG_NEVERGIB ); 
               }
            else if (fromBehind > .1) 
               {
               CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
               if (!pEntity) 
                  {
                  UTIL_LogPrintf("Crashed @ 009\n");
                  return;
                  }
	            pEntity->TraceAttack(m_pPlayer->pev, 90, vecDir, &tr, DMG_BULLET | DMG_NEVERGIB ); 
               }
            else
               {
               CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
               if (!pEntity) 
                  {
                  UTIL_LogPrintf("Crashed @ 010\n");
                  return;
                  }
	            pEntity->TraceAttack(m_pPlayer->pev, 60, vecDir, &tr, DMG_BULLET | DMG_NEVERGIB ); 
               }
	         ApplyMultiDamage( pev, m_pPlayer->pev );
            SendWeaponAnim( KNIFE_STAB );
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_HIT1, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
	         }
         else
	         {
            SendWeaponAnim( KNIFE_STABMISS );
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, WEAPON_HITWALL, .5, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));
            DecalGunshot( &tr, BULLET_PLAYER_CROWBAR );
	         }
         }
      }
}

void FindHullIntersection( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity )
{
	int			i, j, k;
	float		distance;
	float		*minmaxs[2] = {mins, maxs};
	TraceResult tmpTrace;
	Vector		vecHullEnd = tr.vecEndPos;
	Vector		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace );
	if ( tmpTrace.flFraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace );
				if ( tmpTrace.flFraction < 1.0 )
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

void KNIFE::Smack( )
{
	DecalGunshot( &m_trHit, BULLET_PLAYER_CROWBAR );
}
void KNIFE::SwingAgain( void )
{
	Swing( 0 );
}


int KNIFE::Swing( int fFirst )
{
	int fDidHit = FALSE;

	TraceResult tr;

	UTIL_MakeVectors (m_pPlayer->pev->v_angle);
	Vector vecSrc	= m_pPlayer->GetGunPosition( );
	Vector vecEnd	= vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	if ( tr.flFraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( !pHit || pHit->IsBSPModel() )
				FindHullIntersection( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict() );
			vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}

	if ( tr.flFraction >= 1.0 )
	{
		if (fFirst)
		{
			// miss
			switch( (m_iSwing++) % 3 )
			{
			case 0:
				SendWeaponAnim( KNIFE_MIDSLASH1 ); break;
			case 1:
				SendWeaponAnim( KNIFE_MIDSLASH2 ); break;
			case 2:
				SendWeaponAnim( KNIFE_STABMISS ); break;
			}
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
			// play wiff or swish sound
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/cbar_miss1.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0,0xF));

			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		}
	}
	else
	{
		// hit
		fDidHit = TRUE;

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
      if (!pEntity) 
         {
         UTIL_LogPrintf("Crashed @ 011\n");
         return FALSE;
         }
		switch( ((m_iSwing++) % 2) + 1 )
		{
		case 0:
			SendWeaponAnim( KNIFE_STAB ); break;
		case 1:
			SendWeaponAnim( KNIFE_SLASH1 ); break;
		case 2:
			SendWeaponAnim( KNIFE_SLASH2 ); break;
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		ClearMultiDamage( );
		if ( (m_flNextPrimaryAttack + 1 < UTIL_WeaponTimeBase()) || g_pGameRules->IsMultiplayer() )
		{
			// first swing does full damage
			pEntity->TraceAttack(m_pPlayer->pev, 80, gpGlobals->v_forward, &tr, DMG_CLUB ); 
		}
		else
		{
			// subsequent swings do half
			pEntity->TraceAttack(m_pPlayer->pev, 40, gpGlobals->v_forward, &tr, DMG_CLUB ); 
		}	
		ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25;

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				// play thwack or smack sound
				switch( RANDOM_LONG(0,2) )
				{
				case 0:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/cbar_hitbod1.wav", 1, ATTN_NORM); break;
				case 1:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/cbar_hitbod2.wav", 1, ATTN_NORM); break;
				case 2:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/cbar_hitbod3.wav", 1, ATTN_NORM); break;
				}
				m_pPlayer->m_iWeaponVolume = CROWBAR_BODYHIT_VOLUME;
				if (!pEntity->IsAlive() )
					return TRUE;
				else
					flVol = 0.1;

				fHitWorld = FALSE;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		if (fHitWorld)
		{
			float fvolbar = TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd-vecSrc)*2, BULLET_PLAYER_CROWBAR);

			if ( g_pGameRules->IsMultiplayer() )
			{
				// override the volume here, cause we don't play texture sounds in multiplayer, 
				// and fvolbar is going to be 0 from the above call.

				fvolbar = 1;
			}

			// also play crowbar strike
			switch( RANDOM_LONG(0,1) )
			{
			case 0:
				//UTIL_EmitAmbientSound(ENT(0), ptr->vecEndPos, "weapons/cbar_hit1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3));
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hit1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3)); 
				break;
			case 1:
				//UTIL_EmitAmbientSound(ENT(0), ptr->vecEndPos, "weapons/cbar_hit2.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3));
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/cbar_hit2.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0,3)); 
				break;
			}
		}

		// delay the decal a bit
		m_trHit = tr;
	// added "&KNIFE::"
		SetThink( &KNIFE::Smack );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.2;

		m_pPlayer->m_iWeaponVolume = flVol * CROWBAR_WALLHIT_VOLUME;
	}
	return fDidHit;
}
