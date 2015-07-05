/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_SG552.cpp 
   This is the SG-552 rifle module.

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

#define SG552_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

#ifndef LOCAL_WEAPON
   extern int gmsgSetScope;
#else
   extern int weapon_zoom;
#endif

LINK_ENTITY_TO_CLASS( weapon_sg552, SG552 );

void SG552::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(SG552_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_sg552");
	SET_MODEL(ENT(pev), SG552_WEAPON_W);
	m_iId = WEAPON_SG552;
	m_iDefaultAmmo = w_stat[WEAPON_SG552].clip;
   m_iWeaponStatus = 0;
   m_fInZoom = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}

void SG552::Precache( void )
{
	PRECACHE_MODEL(SG552_WEAPON_V);
	PRECACHE_MODEL(SG552_WEAPON_W);
	PRECACHE_MODEL(SG552_WEAPON_P);

	PRECACHE_SOUND(SG552_BOLTPULL);
	PRECACHE_SOUND(SG552_CLIPIN);
	PRECACHE_SOUND(SG552_CLIPOUT);
	PRECACHE_SOUND(SG552_SHOOT1);
	PRECACHE_SOUND(SG552_SHOOT2);
	PRECACHE_SOUND(SG552_ZOOM);
   PRECACHE_SOUND(SG552_EMPTY);
   m_usSG552 = PRECACHE_EVENT( 1, SG552_EV_SCRIPT);
}

int SG552::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_556nato";
	p->iMaxAmmo1 = w_stat[WEAPON_SG552].max_carry;
	p->iMaxClip = w_stat[WEAPON_SG552].clip;
	p->iSlot = SG552_SLOT;
	p->iPosition = SG552_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SG552;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

int SG552::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_SG552;
		return TRUE;
	   }
	return FALSE;
}

BOOL SG552::Deploy( )
{
   m_fInZoom = 0; // gun is deployed out of zoom
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_SG552].speed;
   //EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, SG552_BOLTPULL, 0.8, ATTN_NORM);
	return DefaultDeploy( SG552_WEAPON_V, SG552_WEAPON_P, A_SG552_DRAW, "carbine" );
}

void SG552::PrimaryAttack()
{
   // don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, SG552_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, SG552_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
   PLAYBACK_EVENT1( 1, m_pPlayer->edict(), m_usSG552, m_fInZoom );
   FireSharpBullet(m_pPlayer, pev, WEAPON_SG552);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usSG552, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, weapon_zoom, 0, 0, 0 );
#endif 
   if (m_fInZoom) m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_SG552].rate2;
   else m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_SG552].rate;
}

#ifndef LOCAL_WEAPON   
void SG552::SecondaryAttack( void )
{
   if (!m_fInZoom)
      {
      if (m_pPlayer->pev->velocity.Length() > 140) return;
      if (!(m_pPlayer->pev->flags & FL_ONGROUND)) return;
      m_fInZoom = 1;
      }
	else m_fInZoom = 0;
   MESSAGE_BEGIN(MSG_ONE, gmsgSetScope, NULL, m_pPlayer->pev);
   WRITE_BYTE(m_iId);
   WRITE_BYTE(m_fInZoom);
   MESSAGE_END();
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + .3; 
}

void SG552::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + SG552_RELOAD_TIME;

	// clip is full, no need to reload
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_SG552].clip)
		return;
   if ( m_fInZoom )
	   {
		m_fInZoom = 0;
      MESSAGE_BEGIN(MSG_ONE, gmsgSetScope, NULL, m_pPlayer->pev);
      WRITE_BYTE(m_iId);
      WRITE_BYTE(m_fInZoom);
      MESSAGE_END();
	   }
	DefaultReload( w_stat[WEAPON_SG552].clip, A_SG552_RELOAD, SG552_RELOAD_TIME);
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + SG552_RELOAD_TIME;
}

void SG552::WeaponIdle( void )
{
	SendWeaponAnim( A_SG552_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif