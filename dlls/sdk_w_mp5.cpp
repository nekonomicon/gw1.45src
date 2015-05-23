/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_mp5.cpp 
   This is the MP5 sub module.

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

#define MP5_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_mp5navy, MP5 );

void MP5::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(MP5_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_mp5navy");
	SET_MODEL(ENT(pev), MP5_WEAPON_W);
	m_iId = WEAPON_MP5NAVY;
	m_iDefaultAmmo = w_stat[WEAPON_MP5NAVY].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}

void MP5::Precache( void )
{
	PRECACHE_MODEL(MP5_WEAPON_V);
	PRECACHE_MODEL(MP5_WEAPON_W);
	PRECACHE_MODEL(MP5_WEAPON_P);
   PRECACHE_MODEL(SHELL_MODEL);

	PRECACHE_SOUND(MP5_SLIDEBACK);
	PRECACHE_SOUND(MP5_CLIPIN);
	PRECACHE_SOUND(MP5_CLIPOUT);
	PRECACHE_SOUND(MP5_SHOOT1);
	//PRECACHE_SOUND(MP5_SHOOT2);
   PRECACHE_SOUND(MP5_EMPTY);
   m_usMP5 = PRECACHE_EVENT( 1, MP5_EV_SCRIPT);
}

int MP5::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_9mm";
	p->iMaxAmmo1 = w_stat[WEAPON_MP5NAVY].max_carry;
	p->iMaxClip = w_stat[WEAPON_MP5NAVY].clip;
	p->iSlot = MP5_SLOT;
	p->iPosition = MP5_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_MP5NAVY;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

int MP5::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_MP5NAVY;
		return TRUE;
	   }
	return FALSE;
}

BOOL MP5::Deploy( )
{
   //EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, MP5_SLIDEBACK, 0.8, ATTN_NORM);
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_MP5NAVY].speed;
	return DefaultDeploy( MP5_WEAPON_V, MP5_WEAPON_P, A_MP5_DRAW, "mp5" );
}

void MP5::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, MP5_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, MP5_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   m_iClip--;
   m_pPlayer->stat.Shots++;

#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
   PLAYBACK_EVENT( 1, m_pPlayer->edict(), m_usMP5 );
   FireFlatBullet(m_pPlayer, pev, WEAPON_MP5NAVY);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usMP5, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_MP5NAVY].rate;
}

#ifndef LOCAL_WEAPON   
void MP5::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + MP5_RELOAD_TIME;

	// clip is full, no need to reload
   if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_MP5NAVY].clip)
		return;
	DefaultReload( w_stat[WEAPON_MP5NAVY].clip, A_MP5_RELOAD, MP5_RELOAD_TIME );
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + MP5_RELOAD_TIME;
}

void MP5::WeaponIdle( void )
{
	SendWeaponAnim( A_MP5_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif
