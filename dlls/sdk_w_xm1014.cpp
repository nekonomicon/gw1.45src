/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_xm1014.cpp 
   This is the XM-1014 shotgun module.

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

#define XM1014_MODULE
#include "sdk_w_armory.h"
#include "sdk_w_global.h"
#include "sdk_w_classes.h"

LINK_ENTITY_TO_CLASS( weapon_xm1014, XM1014 );

void XM1014::Spawn( )
{
	Precache( );
   dropmodel = MAKE_STRING(XM1014_WEAPON_W);
	pev->classname = MAKE_STRING("weapon_xm1014");
	SET_MODEL(ENT(pev), XM1014_WEAPON_W);
	m_iId = WEAPON_XM1014;
	m_iDefaultAmmo = w_stat[WEAPON_XM1014].clip;
   m_iWeaponStatus = 0;
   m_flNextReload = UTIL_WeaponTimeBase();
	FallInit();
}

void XM1014::Precache( void )
{
	PRECACHE_MODEL(XM1014_WEAPON_V);
	PRECACHE_MODEL(XM1014_WEAPON_W);
	PRECACHE_MODEL(XM1014_WEAPON_P);

	PRECACHE_SOUND(XM1014_RELOAD);
	PRECACHE_SOUND(XM1014_DEPLOY);
	PRECACHE_SOUND(XM1014_SHOOT1);
   PRECACHE_SOUND(XM1014_EMPTY);

   m_usXM1014 = PRECACHE_EVENT( 1, XM1014_EV_SCRIPT );
}

int XM1014::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ammo_buckshot";
	p->iMaxAmmo1 = w_stat[WEAPON_XM1014].max_carry;
	p->iMaxClip = w_stat[WEAPON_XM1014].clip;
	p->iSlot = XM1014_SLOT;
	p->iPosition = XM1014_POSITION;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_XM1014;
	p->iWeight = WEIGHT_PRIMARY;
	return 1;
}

int XM1014::AddToPlayer( CBasePlayer *pPlayer )
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( 0 );
		MESSAGE_END();
      pPlayer->m_iPrimary = WEAPON_XM1014;
		return TRUE;
	   }
	return FALSE;
}

BOOL XM1014::Deploy( )
{
   //EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, XM1014_DEPLOY, 0.8, ATTN_NORM);
   m_pPlayer->m_iMaxSpeed = w_stat[WEAPON_XM1014].speed;
	return DefaultDeploy( XM1014_WEAPON_V, XM1014_WEAPON_P, A_XM1014_DRAW, "m249" );
}

void XM1014::PrimaryAttack()
{
   int throughwall = 0;
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, XM1014_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }

   // clip is empty
	if (m_iClip <= 0)
	   {
      EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, XM1014_EMPTY, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1;
		return;
	   }
  
   if (m_iWeaponStatus)
      {
      m_iWeaponStatus = 0;
      SendWeaponAnim( A_XM1014_RELOAD2 );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + .4;
      m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
      m_flNextReload = UTIL_WeaponTimeBase() + .6;
      return;
      }
   
   m_iClip--;
   m_pPlayer->stat.Shots++;

#ifndef LOCAL_WEAPON   
   TraceResult tr;
   int wallrandom;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
   m_pPlayer->pev->punchangle.x += RANDOM_FLOAT(-w_stat[WEAPON_XM1014].recoil_pitch, w_stat[WEAPON_XM1014].recoil_pitch) * 3;
   m_pPlayer->pev->punchangle.y += RANDOM_FLOAT(-w_stat[WEAPON_XM1014].recoil_yaw, w_stat[WEAPON_XM1014].recoil_yaw) * 3;
   PLAYBACK_EVENT( 0, m_pPlayer->edict(), m_usXM1014 );
   m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
   Vector vecSrc, vecDir, vecEnd, offset;   
   offset.z = 0;
   for (int i = 0; i < 6; i++)
      {
      throughwall = 0;
      offset.x = RANDOM_FLOAT(-w_stat[WEAPON_XM1014].recoil_pitch, w_stat[WEAPON_XM1014].recoil_pitch) * 2;
      offset.y = RANDOM_FLOAT(-w_stat[WEAPON_XM1014].recoil_yaw, w_stat[WEAPON_XM1014].recoil_yaw) * 2;
   
	   Vector anglesAim = m_pPlayer->pev->v_angle + offset;
	   UTIL_MakeVectors( anglesAim );
	   vecSrc = m_pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
	   vecDir = gpGlobals->v_forward;
      vecEnd = vecSrc + vecDir * w_stat[WEAPON_XM1014].range;

retrace:

      UTIL_TraceLine(vecSrc, vecEnd, IGNORE_DIM, m_pPlayer->edict(), &tr);

      if ( tr.flFraction != 1.0 && !FNullEnt( tr.pHit) )      
         {
	      if ( tr.pHit->v.takedamage )
	         {
		      ClearMultiDamage( );
            if (!throughwall)
               {
               CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
               if (!pEntity) 
                  UTIL_LogPrintf("Crashed @ 013\n");
               else
                  {
	               pEntity->TraceAttack
                  (m_pPlayer->pev, w_stat[WEAPON_XM1014].damage1, vecDir, &tr, DAMAGE_BNG); 
		            ApplyMultiDamage( pev, m_pPlayer->pev );
                  vecSrc = tr.vecEndPos + vecDir * P_BODY_WIDTH;
                  throughwall++;
                  goto retrace;
                  }
               }
            else
               {
               CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
               if (!pEntity) 
                  UTIL_LogPrintf("Crashed @ 014\n");
               else
                  {
	               pEntity->TraceAttack
                  (m_pPlayer->pev, w_stat[WEAPON_XM1014].damage2, vecDir, &tr, DAMAGE_BNG); 
		            ApplyMultiDamage( pev, m_pPlayer->pev );
                  }
               }
            }
	      else
	         {
            if (tr.flFraction < 1)
               {
               wallrandom = rand() & 3;
               if (!throughwall && wallrandom == 1)
                  {
                  vecSrc = tr.vecEndPos + vecDir * w_stat[WEAPON_XM1014].penetration;
                  throughwall++;
                  if (UTIL_PointContents(vecSrc) != CONTENT_SOLID)
                  goto retrace;
                  }
               }
            }
         }
      }
#else
   HUD_PlaybackEvent(0, m_pPlayer->edict(), m_usXM1014, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#endif
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + w_stat[WEAPON_XM1014].rate;
   m_flNextReload = UTIL_WeaponTimeBase() + w_stat[WEAPON_XM1014].rate;
}

#ifndef LOCAL_WEAPON
void XM1014::Reload( void )
{
   // already reloading, don't reload
   if (m_flNextReload < UTIL_WeaponTimeBase()) return;
   m_flNextReload = UTIL_WeaponTimeBase() + XM1014_RELOAD_TIME;

	// clip is full or no bullets left, no need to reload
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= w_stat[WEAPON_XM1014].clip)
		return;
   m_iWeaponStatus = w_stat[WEAPON_XM1014].clip - m_iClip + 1;
   SendWeaponAnim( A_XM1014_RELOAD1 );
   m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + .4;
   m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + .4;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + .4;
}

void XM1014::WeaponIdle( void )
{
   if (m_iWeaponStatus)
      {  
      m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + .4;
      if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) m_iWeaponStatus = 1;
      if (m_iWeaponStatus != 1)
         {
         EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, XM1014_RELOAD, 0.8, ATTN_NORM);
         SendWeaponAnim( A_XM1014_INSERT );
         m_iClip++;
         m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
         m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + .4;
         }
      else
         {
         SendWeaponAnim( A_XM1014_RELOAD2 );
         m_flNextReload = UTIL_WeaponTimeBase() + .4;
         m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
         }
      m_iWeaponStatus--;
      return;
      }

   SendWeaponAnim( A_XM1014_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8192;
}
#endif