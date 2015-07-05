/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_mac10.cpp 
   This is the MAC10 sub module.

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

#define MAC10_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_mac10, MAC10 );

void MAC10::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(MAC10_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_mac10");
	SET_MODEL(ENT(pev), MAC10_WEAPON_W);
	m_iId = WEAPON_MAC10;
	m_iDefaultAmmo = w_stat[WEAPON_MAC10].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}

void MAC10::Precache( void )
{
	PRECACHE_MODEL(MAC10_WEAPON_V);
	PRECACHE_MODEL(MAC10_WEAPON_W);
	PRECACHE_MODEL(MAC10_WEAPON_P);

	PRECACHE_SOUND(MAC10_BOLTPULL);
	PRECACHE_SOUND(MAC10_CLIPIN);
	PRECACHE_SOUND(MAC10_CLIPOUT);
	PRECACHE_SOUND(MAC10_SHOOT1);
   PRECACHE_SOUND(MAC10_EMPTY);
   m_usMAC10 = PRECACHE_EVENT( 1, MAC10_EV_SCRIPT);
}

int MAC10::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_9mm";
	p->iMaxAmmo1 = w_stat[WEAPON_MAC10].max_carry;
	p->iMaxClip = w_stat[WEAPON_MAC10].clip;
	p->iSlot = MAC10_SLOT;
	p->iPosition = MAC10_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_MAC10;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

int MAC10::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_MAC10;
		return TRUE;
	   }
	return FALSE;
}

BOOL MAC10::Deploy( )
{
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_MAC10].speed;
   EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, MAC10_BOLTPULL, 0.8, ATTN_NORM);
	return DefaultDeploy( MAC10_WEAPON_V, MAC10_WEAPON_P, A_MAC10_DRAW, "onehanded" );
}

void MAC10::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, MAC10_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, MAC10_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
      m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }
  
   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
   PLAYBACK_EVENT( 1, m_pPlayer->edict(), m_usMAC10 );
   FireFlatBullet(m_pPlayer, pev, WEAPON_MAC10);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usMAC10, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_MAC10].rate;
}

#ifndef LOCAL_WEAPON   
void MAC10::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + MAC10_RELOAD_TIME;

	// clip is full, no need to reload
   if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_MAC10].clip)
		return;
	DefaultReload( w_stat[WEAPON_MAC10].clip, A_MAC10_RELOAD, MAC10_RELOAD_TIME );
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + MAC10_RELOAD_TIME;
}

void MAC10::WeaponIdle( void )
{
	SendWeaponAnim( A_MAC10_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif