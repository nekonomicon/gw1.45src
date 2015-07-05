/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_tmp.cpp 
   This is the TMP sub module.

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

#define TMP_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_tmp, TMP );

void TMP::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(TMP_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_tmp");
	SET_MODEL(ENT(pev), TMP_WEAPON_W);
	m_iId = WEAPON_TMP;
	m_iDefaultAmmo = w_stat[WEAPON_TMP].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}

void TMP::Precache( void )
{
	PRECACHE_MODEL(TMP_WEAPON_V);
	PRECACHE_MODEL(TMP_WEAPON_W);
	PRECACHE_MODEL(TMP_WEAPON_P);

	PRECACHE_SOUND(TMP_CLIPIN);
	PRECACHE_SOUND(TMP_CLIPOUT);
	PRECACHE_SOUND(TMP_SHOOT1);
	PRECACHE_SOUND(TMP_SHOOT2);
   PRECACHE_SOUND(TMP_EMPTY);
   m_usTMP = PRECACHE_EVENT( 1, TMP_EV_SCRIPT);
}

int TMP::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_9mm";
	p->iMaxAmmo1 = w_stat[WEAPON_TMP].max_carry;
	p->iMaxClip = w_stat[WEAPON_TMP].clip;
	p->iSlot = TMP_SLOT;
	p->iPosition = TMP_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_TMP;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

int TMP::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_TMP;
		return TRUE;
	   }
	return FALSE;
}

BOOL TMP::Deploy( )
{
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_TMP].speed;
	return DefaultDeploy( TMP_WEAPON_V, TMP_WEAPON_P, A_TMP_DRAW, "onehanded" );
}

void TMP::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, TMP_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, TMP_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }
  
   m_iClip--;
   m_pPlayer->stat.Shots++;

#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
   PLAYBACK_EVENT( 1, m_pPlayer->edict(), m_usTMP );
   FireFlatBullet(m_pPlayer, pev, WEAPON_TMP);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usTMP, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_TMP].rate;
}

#ifndef LOCAL_WEAPON   
void TMP::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + 2.2;
   
	// clip is full, no need to reload
   if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_TMP].clip)
		return;
	DefaultReload( w_stat[WEAPON_TMP].clip, A_TMP_RELOAD, TMP_RELOAD_TIME );
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + TMP_RELOAD_TIME;
}

void TMP::WeaponIdle( void )
{
	SendWeaponAnim( A_TMP_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif