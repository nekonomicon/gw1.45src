/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_p228.cpp 
   This is the P228 pistol module.

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

#define B92D_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_92d, B92D );

void B92D::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(B92D_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_92d");
	SET_MODEL(ENT(pev), B92D_WEAPON_W);
	m_iId = WEAPON_92D;
	m_iDefaultAmmo = w_stat[WEAPON_92D].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}


void B92D::Precache( void )
{
	PRECACHE_MODEL(B92D_WEAPON_V);
	PRECACHE_MODEL(B92D_WEAPON_W);
	PRECACHE_MODEL(B92D_WEAPON_P);

	PRECACHE_SOUND(B92D_CLIPIN);
	PRECACHE_SOUND(B92D_CLIPOUT);
	PRECACHE_SOUND(B92D_SLIDEBACK);
	PRECACHE_SOUND(B92D_SLIDERELEASE);
	PRECACHE_SOUND(B92D_SHOOT1);
   PRECACHE_SOUND(B92D_EMPTY);
   m_usB92D = PRECACHE_EVENT( 1, B92D_EV_SCRIPT);
}

int B92D::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_357sig";
	p->iMaxAmmo1 = w_stat[WEAPON_92D].max_carry;
	p->iMaxClip = w_stat[WEAPON_92D].clip;
	p->iSlot = B92D_SLOT;
	p->iPosition = B92D_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_92D;
	p->iWeight = WEIGHT_SECONDARY;
	return 1;
}

int B92D::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iSecondary = WEAPON_92D;
		return TRUE;
	   }
	return FALSE;
}

BOOL B92D::Deploy( )
{
   if (m_pPlayer->m_iPrimary)
      m_pPlayer->m_iMaxSpeed = (300 + w_stat[m_pPlayer->m_iPrimary].speed) / 2;
   return DefaultDeploy(B92D_WEAPON_V, B92D_WEAPON_P, A_B92D_DRAW, "onehanded");
}

void B92D::PrimaryAttack()
{
   if (m_bTriggerReleased)
      m_bTriggerReleased = false;
   else
      return;

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, B92D_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, B92D_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
   PLAYBACK_EVENT( 1, m_pPlayer->edict(), m_usB92D );
   FireFlatBullet(m_pPlayer, pev, WEAPON_92D);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usB92D, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, m_iClip, 0, 0, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_92D].rate;
}

#ifndef LOCAL_WEAPON
void B92D::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + B92D_RELOAD_TIME;

	// clip is full, no need to reload
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_92D].clip)
		return;
	DefaultReload( w_stat[WEAPON_92D].clip, A_B92D_RELOAD, B92D_RELOAD_TIME);
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + B92D_RELOAD_TIME;
}

void B92D::WeaponIdle( void )
{
	SendWeaponAnim( A_B92D_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif