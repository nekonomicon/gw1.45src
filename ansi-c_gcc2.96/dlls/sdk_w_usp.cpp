/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_usp.cpp 
   This is the USP Pistol module.

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
// adding strings
#include <strings.h>
#define USP_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_usp, USP );

void USP::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(USP_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_usp");
	SET_MODEL(ENT(pev), USP_WEAPON_W);
	m_iId = WEAPON_USP;
	m_iDefaultAmmo = w_stat[WEAPON_USP].clip;
   m_iWeaponStatus = 0;
   silenced = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	// adding "BOOL"? 
	FallInit();
}


void USP::Precache( void )
{
	PRECACHE_MODEL(USP_WEAPON_V);
	PRECACHE_MODEL(USP_WEAPON_W);
	PRECACHE_MODEL(USP_WEAPON_P);

   PRECACHE_SOUND(USP_CLIPIN);
   PRECACHE_SOUND(USP_CLIPOUT);
   PRECACHE_SOUND(USP_SILENCE);
   PRECACHE_SOUND(USP_UNSILENCE);
   PRECACHE_SOUND(USP_USHOOT1);
   PRECACHE_SOUND(USP_SHOOT1);
   PRECACHE_SOUND(USP_SHOOT2);
   PRECACHE_SOUND(USP_SLIDEBACK);
   PRECACHE_SOUND(USP_SLIDERELEASE);
   PRECACHE_SOUND(USP_EMPTY);   

   m_usUSP = PRECACHE_EVENT( 1, USP_EV_SCRIPT);
}

int USP::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_45acp";
	p->iMaxAmmo1 = w_stat[WEAPON_USP].max_carry;
	p->iMaxClip = w_stat[WEAPON_USP].clip;
	p->iSlot = USP_SLOT;
	p->iPosition = USP_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_USP;
	p->iWeight = WEIGHT_SECONDARY;
	return 1;
}

int USP::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( silenced );
		MESSAGE_END();
		return TRUE;
	   }
	return FALSE;
}

BOOL USP::Deploy( )
{
   int x;
   PLAYBACK_EVENT2( 0, m_pPlayer->edict(), m_usUSP, silenced, M_UPDATE);
   if (!silenced) x = DefaultDeploy(USP_WEAPON_V, USP_WEAPON_P, A_USP_UDRAW, "onehanded");
   else x = DefaultDeploy(USP_WEAPON_V, USP_WEAPON_P, A_USP_DRAW, "onehanded");
   return x;
}

void USP::PrimaryAttack()
{
   if (m_bTriggerReleased) m_bTriggerReleased = false;
   else return;

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, USP_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, USP_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
   if (!silenced)
      {
	   m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	   m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
      }
   else
      {
	   m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	   m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
      }
   PLAYBACK_EVENT1( 1, m_pPlayer->edict(), m_usUSP, silenced);
   FireFlatBullet(m_pPlayer, pev, WEAPON_USP);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usUSP, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, silenced, 0, m_iClip, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_USP].rate;
}

#ifndef LOCAL_WEAPON
void USP::SecondaryAttack()
{
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.6;
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.6;
   m_flNextReload = UTIL_WeaponTimeBase() + 2.6;
   if (!silenced)
      silenced = 1;
   else
      silenced = 0;
   PLAYBACK_EVENT2( 0, m_pPlayer->edict(), m_usUSP, silenced, M_SILENCE);
   return;
}


void USP::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   if (m_flNextSecondaryAttack > UTIL_WeaponTimeBase()) return;
   m_flNextSecondaryAttack = m_flNextReload = UTIL_WeaponTimeBase() + USP_RELOAD_TIME;

	// clip is full, no need to reload
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_USP].clip)
		return;
   if (!silenced) DefaultReload( w_stat[WEAPON_USP].clip, A_USP_URELOAD, USP_RELOAD_TIME );
   else DefaultReload( w_stat[WEAPON_USP].clip, A_USP_RELOAD, USP_RELOAD_TIME );
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + USP_RELOAD_TIME;
   m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + USP_RELOAD_TIME + .6;
}

void USP::WeaponIdle( void )
{
   if (!silenced) SendWeaponAnim( A_USP_UIDLE );
   else SendWeaponAnim( A_USP_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif