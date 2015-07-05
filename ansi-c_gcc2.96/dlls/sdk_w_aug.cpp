/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_aug.cpp
   This is the AUG rifle module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

#define AUG_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

#ifndef LOCAL_WEAPON
   extern int gmsgSetScope;
#else
   extern int weapon_zoom;
#endif

#define WEAPON_SLAP     "sound/weapons/knife_slash2.wav"

LINK_ENTITY_TO_CLASS( weapon_aug, AUG );

void AUG::Spawn( )
{
	Precache( );
	pev->classname = MAKE_STRING("weapon_aug");
   dropmodel = MAKE_STRING(AUG_WEAPON_W);
	SET_MODEL(ENT(pev), AUG_WEAPON_W);
	m_iId = WEAPON_AUG;
	m_iDefaultAmmo = w_stat[WEAPON_AUG].clip;
   m_iWeaponStatus = 0;
   m_fInZoom = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}

void AUG::Precache( void )
{
	PRECACHE_MODEL(AUG_WEAPON_V);
	PRECACHE_MODEL(AUG_WEAPON_W);
	PRECACHE_MODEL(AUG_WEAPON_P);

	PRECACHE_SOUND(AUG_BOLTPULL);
	PRECACHE_SOUND(AUG_BOLTSLAP);
	PRECACHE_SOUND(AUG_CLIPIN);
	PRECACHE_SOUND(AUG_CLIPOUT);
	PRECACHE_SOUND(AUG_SHOOT1);
	PRECACHE_SOUND(AUG_FOREARM);
	PRECACHE_SOUND(AUG_ZOOM);
   PRECACHE_SOUND(AUG_EMPTY);
   m_usAUG = PRECACHE_EVENT( 1, AUG_EV_SCRIPT);
}

int AUG::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_762nato";
	p->iMaxAmmo1 = w_stat[WEAPON_AUG].max_carry;
	p->iMaxClip = w_stat[WEAPON_AUG].clip;
	p->iSlot = AUG_SLOT;
	p->iPosition = AUG_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_AUG;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

int AUG::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_AUG;
		return TRUE;
	   }
	return FALSE;
}

BOOL AUG::Deploy( )
{
   m_fInZoom = 0;
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_AUG].speed;
   return DefaultDeploy( AUG_WEAPON_V, AUG_WEAPON_P, A_AUG_DRAW, "carbine" );
}

void AUG::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, AUG_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }
	if (m_iClip <= 0)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, AUG_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }
  
   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
   TraceResult tr;
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
   PLAYBACK_EVENT1( 1, m_pPlayer->edict(), m_usAUG, m_fInZoom );
   FireSharpBullet(m_pPlayer, pev, WEAPON_AUG);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usAUG, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, weapon_zoom, 0, 0, 0 );
#endif
   if (m_fInZoom) m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_AUG].rate2;
   else m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_AUG].rate;
}

#ifndef LOCAL_WEAPON
void AUG::SecondaryAttack( void )
{
   if (!m_fInZoom)
      {
      if (m_pPlayer->pev->velocity.Length() > 140) return;
      if (!(m_pPlayer->pev->flags & FL_ONGROUND)) return;
      m_fInZoom = 1;
      }
	else m_fInZoom = 0;
   MESSAGE_BEGIN(MSG_ONE, gmsgSetScope, NULL, m_pPlayer->pev);
   WRITE_BYTE(m_iId);
   WRITE_BYTE(m_fInZoom);
   MESSAGE_END();

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + .3; 
}

void AUG::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + AUG_RELOAD_TIME;
	
   // clip is full, no need to reload
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_AUG].clip)
		return;

	if ( m_fInZoom )
	   {
		m_fInZoom = 0;
      MESSAGE_BEGIN(MSG_ONE, gmsgSetScope, NULL, m_pPlayer->pev);
      WRITE_BYTE(m_iId);
      WRITE_BYTE(m_fInZoom);
      MESSAGE_END();
	   }

	DefaultReload(w_stat[WEAPON_AUG].clip, A_AUG_RELOAD, AUG_RELOAD_TIME);
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + AUG_RELOAD_TIME;
}

void AUG::WeaponIdle( void )
{
	SendWeaponAnim( A_AUG_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif
