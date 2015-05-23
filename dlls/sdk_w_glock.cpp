/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_glock.cpp
   This is the Glock 18 pistol module.

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

#define GLOCK_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_b93r, GLOCK );

void GLOCK::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(GLOCK_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_b93r");
	SET_MODEL(ENT(pev), GLOCK_WEAPON_W);
	m_iId = WEAPON_GLOCK18;
	m_iDefaultAmmo = w_stat[WEAPON_GLOCK18].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}


void GLOCK::Precache( void )
{
	PRECACHE_MODEL(GLOCK_WEAPON_V);
	PRECACHE_MODEL(GLOCK_WEAPON_W);
	PRECACHE_MODEL(GLOCK_WEAPON_P);

   PRECACHE_MODEL("sprites/wall_puff1.spr");

	PRECACHE_SOUND(GLOCK_CLIPIN);
	PRECACHE_SOUND(GLOCK_CLIPOUT);
	PRECACHE_SOUND(GLOCK_SHOOT1);
   PRECACHE_SOUND(GLOCK_SLIDEBACK);
   PRECACHE_SOUND(GLOCK_EMPTY);
   m_usGLOCK = PRECACHE_EVENT( 1, GLOCK_EV_SCRIPT);
}

int GLOCK::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_9mm";
	p->iMaxAmmo1 = w_stat[WEAPON_GLOCK18].max_carry;
	p->iMaxClip = w_stat[WEAPON_GLOCK18].clip;
	p->iSlot = GLOCK_SLOT;
	p->iPosition = GLOCK_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_GLOCK18;
	p->iWeight = WEIGHT_SECONDARY;
	return 1;
}

int GLOCK::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iSecondary = WEAPON_GLOCK18;
		return TRUE;
	   }
	return FALSE;
}

BOOL GLOCK::Deploy( )
{
   int x;
   if (m_pPlayer->m_iPrimary)
      m_pPlayer->m_iMaxSpeed = (300 + w_stat[m_pPlayer->m_iPrimary].speed) / 2;
   if (rand() & 1) x = DefaultDeploy(GLOCK_WEAPON_V, GLOCK_WEAPON_P, A_GLOCK_DRAW1, "onehanded");
   else x = DefaultDeploy(GLOCK_WEAPON_V, GLOCK_WEAPON_P, A_GLOCK_DRAW2, "onehanded");


   return x;
}

void GLOCK::PrimaryAttack()
{
   if (m_bTriggerReleased) m_bTriggerReleased = false;
   else return;

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, GLOCK_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, GLOCK_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
   PLAYBACK_EVENT( 1, m_pPlayer->edict(), m_usGLOCK );
   FireFlatBullet(m_pPlayer, pev, WEAPON_GLOCK18);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usGLOCK, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, m_iClip, 0, 0, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_GLOCK18].rate;
}

#ifndef LOCAL_WEAPON
void GLOCK::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + GLOCK_RELOAD_TIME;

	// clip is full, no need to reload
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_GLOCK18].clip)
		return;  
   DefaultReload( w_stat[WEAPON_GLOCK18].clip, A_GLOCK_RELOAD1, GLOCK_RELOAD_TIME);
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + GLOCK_RELOAD_TIME;
}

void GLOCK::WeaponIdle( void )
{
	SendWeaponAnim( A_GLOCK_IDLE3 );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif