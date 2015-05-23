/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_hegrenade.cpp 
   This is the High Explosive grenade module.

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

#define HEGRENADE_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_hegrenade, HEGRENADE );

void HEGRENADE::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(HE_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_hegrenade");
	SET_MODEL(ENT(pev), HE_WEAPON_W);
	m_iId = WEAPON_HEGRENADE;
	m_iDefaultAmmo = 1;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase() + 1;
	FallInit();
}


void HEGRENADE::Precache( void )
{
	PRECACHE_MODEL(HE_WEAPON_V);
	PRECACHE_MODEL(HE_WEAPON_W);
	PRECACHE_MODEL(HE_WEAPON_P);

	PRECACHE_SOUND(HEGRENADE_SHOOT1);
	PRECACHE_SOUND(HEGRENADE_SHOOT2);
	PRECACHE_SOUND(HEGRENADE_BOUNCE);
	PRECACHE_SOUND(HEGRENADE_PINPULL);
}

int HEGRENADE::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "HEGrenade";
	p->iMaxAmmo1 = HEGRENADE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = HEGRENADE_SLOT;
	p->iPosition = HEGRENADE_POSITION;
	p->iId = m_iId = WEAPON_HEGRENADE;
	p->iWeight = WEIGHT_PROJECTILE;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	return 1;
}


BOOL HEGRENADE::Deploy( )
{
	m_flReleaseThrow = -1;
   if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) return 0;
	return DefaultDeploy( HE_WEAPON_V, HE_WEAPON_P, A_HEGRENADE_DRAW, "grenade" );
}

BOOL HEGRENADE::CanHolster( void )
{
	// can only holster hand grenades when not primed!
   if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) return 0;
	return ( m_flStartThrow == 0);
}

void HEGRENADE::Holster( )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + HEGRENADE_ATTACK1_TIME;
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	   {
	   }
	else
	   {
		m_pPlayer->pev->weapons &= ~(1<<WEAPON_HEGRENADE);
		SetThink( DestroyItem );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	   }
}

void HEGRENADE::PrimaryAttack()
{
   if (m_flStartThrow)
      {
      m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + .1f;
      return;
      }
	if (!m_flStartThrow && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
	   {
		m_flStartThrow = UTIL_WeaponTimeBase() + .1f;
		m_flReleaseThrow = 0;
		SendWeaponAnim( A_HEGRENADE_PINPULL );
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + HEGRENADE_ATTACK1_TIME;
	   }
}


void HEGRENADE::WeaponIdle( void )
{
	if (m_flReleaseThrow == 0)
		m_flReleaseThrow = UTIL_WeaponTimeBase() + .1f;

	if (m_flStartThrow)
	   {
		Vector throw_angle = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if (throw_angle.x < 0)
			throw_angle.x = -10 + throw_angle.x * (80 / 90.0);
		else
			throw_angle.x = -10 + throw_angle.x * (100 / 90.0);

		float flVel = (90 - throw_angle.x) * 6;
		if (flVel > 750) flVel = 750;

		UTIL_MakeVectors( throw_angle );
		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16 + gpGlobals->v_right * 12;
		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;

		float time = m_flStartThrow - UTIL_WeaponTimeBase() + 3.0;
		if (time < 0) time = 0;

		CGrenade::ShootTimed( m_pPlayer->pev, vecSrc, vecThrow,  2); //m_pPlayer->m_iTeam ); 

		SendWeaponAnim( A_HEGRENADE_THROW );

      m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_flStartThrow = 0;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		if ( !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
		   {
         //RetireWeapon();
         // Last grenade, remove from player inventory
         //m_pPlayer->pev->weapons &= ~(1<<m_iId);
			m_flTimeWeaponIdle = m_flNextSecondaryAttack 
                            = m_flNextPrimaryAttack 
                            = UTIL_WeaponTimeBase() + HEGRENADE_ATTACK1_TIME;
		   }
		return;
	   }
	else if (m_flReleaseThrow > 0)
	   {
		m_flStartThrow = 0;
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			SendWeaponAnim( A_HEGRENADE_DRAW );
		else
		   {
			RetireWeapon();
			return;
		   }
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
		m_flReleaseThrow = -1;
		return;
	   }

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	   {
      m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
		SendWeaponAnim( A_HEGRENADE_IDLE );
	   }
}