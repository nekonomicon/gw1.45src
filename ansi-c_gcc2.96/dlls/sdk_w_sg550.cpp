/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_sg550.cpp 
   This is the SG-550 sniper rifle module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"

#define SG550_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

#ifndef LOCAL_WEAPON
   extern int gmsgSetScope;
#else
   extern int weapon_zoom;
#endif

#define BOLT_AIR_VELOCITY	2000
#define BOLT_WATER_VELOCITY	1000

LINK_ENTITY_TO_CLASS( weapon_sg550, SG550 );

void SG550::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(SG550_WEAPON_W);
   pev->classname = MAKE_STRING("weapon_sg550");
	SET_MODEL(ENT(pev), SG550_WEAPON_W);
	m_iId = WEAPON_SG550;
	m_iDefaultAmmo = w_stat[WEAPON_SG550].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase() + 2;
	FallInit();
}

int SG550::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_SG550;
		return TRUE;
	   }
	return FALSE;
}

void SG550::Precache( void )
{
	PRECACHE_MODEL(SG550_WEAPON_V);
	PRECACHE_MODEL(SG550_WEAPON_W);
	PRECACHE_MODEL(SG550_WEAPON_P);

   PRECACHE_SOUND(SG550_CLIPIN);
   PRECACHE_SOUND(SG550_CLIPOUT);
   PRECACHE_SOUND(SG550_DEPLOY);
   PRECACHE_SOUND(SG550_ZOOM);
   PRECACHE_SOUND(SG550_SHOOT1);
   PRECACHE_SOUND(SG550_EMPTY);
   m_usSG550 = PRECACHE_EVENT( 1, SG550_EV_SCRIPT);
}

int SG550::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_762nato";
	p->iMaxAmmo1 = w_stat[WEAPON_SG550].max_carry;
	p->iMaxClip = w_stat[WEAPON_SG550].clip;
	p->iSlot = SG550_SLOT;
	p->iPosition = SG550_POSITION;
	p->iId = WEAPON_SG550;
	p->iFlags = 0;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

BOOL SG550::Deploy( )
{
   m_fInZoom = 0;
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_SG550].speed;
   EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, SG550_DEPLOY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
   return DefaultDeploy( SG550_WEAPON_V, SG550_WEAPON_P, A_SG550_DRAW, "rifle" );
}

void SG550::PrimaryAttack( void )
{
   // clip is empty
	if (m_iClip <= 0)
	   {
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, SG550_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_SG550].rate2;
		return;
	   }

   m_iClip--;
   m_pPlayer->stat.Shots++;
	
#ifndef LOCAL_WEAPON   
   TraceResult tr;
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
   PLAYBACK_EVENT1(1, m_pPlayer->edict(), m_usSG550, m_fInZoom );
   FireStrongBullet(m_pPlayer, pev, WEAPON_SG550, m_fInZoom);
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usSG550, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, weapon_zoom, 0, 0, 0 );
#endif 
   if (m_fInZoom) m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_SG550].rate2;
   else m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_SG550].rate;
}


#ifndef LOCAL_WEAPON
void SG550::SecondaryAttack()
{
   if (!m_fInZoom)
      {
      if (m_pPlayer->pev->velocity.Length() > 140) return;
      if (!(m_pPlayer->pev->flags & FL_ONGROUND)) return;
      m_fInZoom = 1;
      }
	else if (m_fInZoom == 2) m_fInZoom = 0;
	else 
      {
      if (m_pPlayer->pev->velocity.Length() > 5) return;
      m_fInZoom = 2;
      }
   MESSAGE_BEGIN(MSG_ONE, gmsgSetScope, NULL, m_pPlayer->pev);
   WRITE_BYTE(m_iId);
   WRITE_BYTE(m_fInZoom);
   MESSAGE_END();
   EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, SG550_ZOOM, RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + .3; 
}

void SG550::Reload( void )
{
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + SG550_RELOAD_TIME;

   if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_SG550].clip)
      return;
	if ( m_fInZoom )
	   {
		m_fInZoom = 0;
      MESSAGE_BEGIN(MSG_ONE, gmsgSetScope, NULL, m_pPlayer->pev);
      WRITE_BYTE(m_iId);
      WRITE_BYTE(m_fInZoom);
      MESSAGE_END();
	   }
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + SG550_RELOAD_TIME;
	DefaultReload( w_stat[WEAPON_SG550].clip, A_SG550_RELOAD, SG550_RELOAD_TIME );
}

void SG550::WeaponIdle( void )
{
   SendWeaponAnim( A_SG550_IDLE1 );
   m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif