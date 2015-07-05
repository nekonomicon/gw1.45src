/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_m16.cpp 
   This is the M16 rifle module.

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

#define M16_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_m16, M16 );

void M16::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(M16_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_m16");
	SET_MODEL(ENT(pev), M16_WEAPON_W);
	m_iId = WEAPON_M16;
	m_iDefaultAmmo = w_stat[WEAPON_M16].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}

void M16::Precache( void )
{
	PRECACHE_MODEL(M16_WEAPON_V);
	PRECACHE_MODEL(M16_WEAPON_W);
	PRECACHE_MODEL(M16_WEAPON_P);
   PRECACHE_MODEL(SHELL_MODEL2);

	PRECACHE_SOUND(M16_BOLTPULL);
	PRECACHE_SOUND(M16_CLIPIN);
	PRECACHE_SOUND(M16_CLIPOUT);
	PRECACHE_SOUND(M16_CLIPIN);
	PRECACHE_SOUND(M16_CLIPOUT);
	PRECACHE_SOUND(M16_SHOOT1);
	PRECACHE_SOUND(M16_SHOOT2);
   PRECACHE_SOUND(M16_EMPTY);
   m_usM16 = PRECACHE_EVENT( 1, M16_EV_SCRIPT);
}

int M16::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_556nato";
	p->iMaxAmmo1 = w_stat[WEAPON_M16].max_carry;
	p->iMaxClip = w_stat[WEAPON_M16].clip;
	p->iSlot = M16_SLOT;
	p->iPosition = M16_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_M16;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

int M16::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_M16;
		return TRUE;
	   }
	return FALSE;
}

BOOL M16::Deploy( )
{
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_M16].speed;
	return DefaultDeploy( M16_WEAPON_V, M16_WEAPON_P, A_M16_DRAW, "carbine" );
}

void M16::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, M16_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, M16_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
   PLAYBACK_EVENT( 1, m_pPlayer->edict(), m_usM16 );
   FireSharpBullet(m_pPlayer, pev, WEAPON_M16);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usM16, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_M16].rate;
}

#ifndef LOCAL_WEAPON   
void M16::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + M16_RELOAD_TIME;

	// clip is full, no need to reload
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_M16].clip)
		return;

	DefaultReload( w_stat[WEAPON_M16].clip, A_M16_RELOAD, M16_RELOAD_TIME);
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + M16_RELOAD_TIME;
}

void M16::WeaponIdle( void )
{
	SendWeaponAnim( A_M16_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif
