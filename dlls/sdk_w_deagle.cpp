/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_deagle.cpp
   This is the Desert Eagle pistol module.

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

#define DEAGLE_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_1911, DEAGLE );

void DEAGLE::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(DEAGLE_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_1911");
	SET_MODEL(ENT(pev), DEAGLE_WEAPON_W);
	m_iId = WEAPON_DEAGLE;
	m_iDefaultAmmo = w_stat[WEAPON_DEAGLE].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}


void DEAGLE::Precache( void )
{
	PRECACHE_MODEL(DEAGLE_WEAPON_V);
	PRECACHE_MODEL(DEAGLE_WEAPON_W);
	PRECACHE_MODEL(DEAGLE_WEAPON_P);

	PRECACHE_SOUND(DEAGLE_CLIPIN);
	PRECACHE_SOUND(DEAGLE_CLIPOUT);
	PRECACHE_SOUND(DEAGLE_SHOOT1);
   PRECACHE_SOUND(DEAGLE_SHOOT2);
   PRECACHE_SOUND(DEAGLE_DEPLOY);
   PRECACHE_SOUND(DEAGLE_EMPTY);
   m_usDEAGLE = PRECACHE_EVENT( 1, DEAGLE_EV_SCRIPT);
}

int DEAGLE::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_50ae";
	p->iMaxAmmo1 = w_stat[WEAPON_DEAGLE].max_carry;
	p->iMaxClip = w_stat[WEAPON_DEAGLE].clip;
	p->iSlot = DEAGLE_SLOT;
	p->iPosition = DEAGLE_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_DEAGLE;
	p->iWeight = WEIGHT_SECONDARY;
	return 1;
}

int DEAGLE::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iSecondary = WEAPON_DEAGLE;
      return TRUE;
	   }
	return FALSE;
}

BOOL DEAGLE::Deploy( )
{
   if (m_pPlayer->m_iPrimary)
      m_pPlayer->m_iMaxSpeed = (300 + w_stat[m_pPlayer->m_iPrimary].speed) / 2;
   return DefaultDeploy(DEAGLE_WEAPON_V, DEAGLE_WEAPON_P, A_DEAGLE_DRAW, "onehanded");
}

void DEAGLE::PrimaryAttack()
{
   if (m_bTriggerReleased) m_bTriggerReleased = false;
   else return;

	if (m_pPlayer->pev->waterlevel == 3)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, DEAGLE_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, DEAGLE_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
   PLAYBACK_EVENT(1, m_pPlayer->edict(), m_usDEAGLE);
   FireSharpBullet(m_pPlayer, pev, WEAPON_DEAGLE);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usDEAGLE, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, m_iClip, 0, 0, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_DEAGLE].rate;
}

#ifndef LOCAL_WEAPON
void DEAGLE::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + DEAGLE_RELOAD_TIME;

	// clip is full, no need to reload
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_DEAGLE].clip)
		return;
	DefaultReload( w_stat[WEAPON_DEAGLE].clip, A_DEAGLE_RELOAD, DEAGLE_RELOAD_TIME);
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + DEAGLE_RELOAD_TIME;
}

void DEAGLE::WeaponIdle( void )
{
	SendWeaponAnim( A_DEAGLE_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif