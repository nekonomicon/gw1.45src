/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_ump45.cpp 
   This is the UMP-45 sub module.

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

#define UMP45_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_ump45, UMP45 );

void UMP45::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(UMP45_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_ump45");
	SET_MODEL(ENT(pev), UMP45_WEAPON_W);
	m_iId = WEAPON_UMP45;
	m_iDefaultAmmo = w_stat[WEAPON_UMP45].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}


void UMP45::Precache( void )
{
	PRECACHE_MODEL(UMP45_WEAPON_V);
	PRECACHE_MODEL(UMP45_WEAPON_W);
	PRECACHE_MODEL(UMP45_WEAPON_P);

	PRECACHE_SOUND(UMP45_BOLTSLAP);
	PRECACHE_SOUND(UMP45_CLIPIN);
	PRECACHE_SOUND(UMP45_CLIPOUT);
	PRECACHE_SOUND(UMP45_SHOOT1);
   PRECACHE_SOUND(UMP45_EMPTY);
   m_usUMP45 = PRECACHE_EVENT( 1, UMP45_EV_SCRIPT);
}

int UMP45::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_9mm";
	p->iMaxAmmo1 = w_stat[WEAPON_UMP45].max_carry;
	p->iMaxClip = w_stat[WEAPON_UMP45].clip;
	p->iSlot = UMP45_SLOT;
	p->iPosition = UMP45_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_UMP45;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

int UMP45::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_UMP45;
		return TRUE;
	   }
	return FALSE;
}

BOOL UMP45::Deploy( )
{
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_UMP45].speed;
   EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, UMP45_BOLTSLAP, 0.8, ATTN_NORM);
	return DefaultDeploy( UMP45_WEAPON_V, UMP45_WEAPON_P, A_UMP45_DRAW, "carbine" );
}

void UMP45::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, UMP45_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, UMP45_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }
 
   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
   PLAYBACK_EVENT( 1, m_pPlayer->edict(), m_usUMP45 );
   FireFlatBullet(m_pPlayer, pev, WEAPON_UMP45);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usUMP45, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#endif  
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_UMP45].rate;
}

#ifndef LOCAL_WEAPON   
void UMP45::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + UMP45_RELOAD_TIME;

	// clip is full, no need to reload
   if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_UMP45].clip)
		return;
	DefaultReload( w_stat[WEAPON_UMP45].clip, A_UMP45_RELOAD, UMP45_RELOAD_TIME );
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + UMP45_RELOAD_TIME;
}

void UMP45::WeaponIdle( void )
{
	SendWeaponAnim( A_UMP45_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif