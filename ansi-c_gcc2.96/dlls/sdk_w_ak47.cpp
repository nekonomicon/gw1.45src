/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_ak47.cpp
   This is the AK-47 rifle module.

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

#define AK47_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_ak47, AK47 );

void AK47::Spawn( )
{
	Precache( );
	pev->classname = MAKE_STRING("weapon_ak47");
   dropmodel = MAKE_STRING(AK47_WEAPON_W);
	SET_MODEL(ENT(pev), AK47_WEAPON_W);
	m_iId = WEAPON_AK47;
	m_iDefaultAmmo = w_stat[WEAPON_AK47].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}

void AK47::Precache( void )
{
	PRECACHE_MODEL(AK47_WEAPON_V);
	PRECACHE_MODEL(AK47_WEAPON_W);
	PRECACHE_MODEL(AK47_WEAPON_P);
   PRECACHE_MODEL(SHELL_MODEL2);

	PRECACHE_SOUND(AK47_BOLTPULL);
	PRECACHE_SOUND(AK47_CLIPIN);
	PRECACHE_SOUND(AK47_CLIPOUT);
	PRECACHE_SOUND(AK47_CLIPIN);
	PRECACHE_SOUND(AK47_CLIPOUT);
	PRECACHE_SOUND(AK47_SHOOT1);
	PRECACHE_SOUND(AK47_SHOOT2);
   PRECACHE_SOUND(AK47_EMPTY);
   m_usAK47 = PRECACHE_EVENT( 1, AK47_EV_SCRIPT);
}

int AK47::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_762nato";
	p->iMaxAmmo1 = w_stat[WEAPON_AK47].max_carry;
	p->iMaxClip = w_stat[WEAPON_AK47].clip;
	p->iSlot = AK47_SLOT;
	p->iPosition = AK47_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_AK47;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

int AK47::AddToPlayer( CBasePlayer *pPlayer )
{   
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_AK47;
		return TRUE;
	   }
	return FALSE;
}


BOOL AK47::Deploy( )
{
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_AK47].speed;
	return DefaultDeploy( AK47_WEAPON_V, AK47_WEAPON_P, A_AK47_DRAW, "ak47" );
}

void AK47::PrimaryAttack()
{

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, AK47_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, AK47_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
   PLAYBACK_EVENT( 1, m_pPlayer->edict(), m_usAK47 );
   FireSharpBullet(m_pPlayer, pev, WEAPON_AK47);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usAK47, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_AK47].rate;
}

#ifndef LOCAL_WEAPON   
void AK47::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + AK47_RELOAD_TIME;

	// clip is full, no need to reload
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_AK47].clip)
		return;

	DefaultReload( w_stat[WEAPON_AK47].clip, A_AK47_RELOAD, AK47_RELOAD_TIME);
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + AK47_RELOAD_TIME;
}

void AK47::WeaponIdle( void )
{
	SendWeaponAnim( A_AK47_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif
