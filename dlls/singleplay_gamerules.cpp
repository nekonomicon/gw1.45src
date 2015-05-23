/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// teamplay_gamerules.cpp
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"items.h"

extern DLL_GLOBAL CGameRules	*g_pGameRules;
extern DLL_GLOBAL BOOL	g_fGameOver;
extern int gmsgDeathMsg;	// client dll messages
extern int gmsgScoreInfo;
extern int gmsgMOTD;

CHalfLifeRules::CHalfLifeRules( void ){}
int CHalfLifeRules::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled ){return 1;}
void CHalfLifeRules::Think ( void ){}
BOOL CHalfLifeRules::IsMultiplayer( void ){return TRUE;}
BOOL CHalfLifeRules::IsDeathmatch ( void ){return FALSE;}
BOOL CHalfLifeRules::IsCoOp( void ){return FALSE;}
void CHalfLifeRules::PlayerSpawn( CBasePlayer *pPlayer ){}
void CHalfLifeRules::PlayerThink( CBasePlayer *pPlayer ){}
void CHalfLifeRules::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor ){}
void CHalfLifeRules::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor ){}
void CHalfLifeRules::PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon ){}
BOOL CHalfLifeRules::CanHaveItem( CBasePlayer *pPlayer, CItem *pItem ){return TRUE;}
void CHalfLifeRules::PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem ){}
void CHalfLifeRules::PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount ){}
void CHalfLifeRules::CheckRoundEnd(){}
void CHalfLifeRules::CheckDeadRoundEnd(){}
BOOL CHalfLifeRules::GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon ) {return FALSE;}
BOOL CHalfLifeRules::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] ){return TRUE;}
void CHalfLifeRules::InitHUD( CBasePlayer *pl ){}
void CHalfLifeRules::ClientDisconnected( edict_t *pClient ){}
float CHalfLifeRules::FlHealthChargerRechargeTime( void ){return 0;}
float CHalfLifeRules::FlPlayerFallDamage( CBasePlayer *pPlayer )
{
	pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
	return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
}
BOOL CHalfLifeRules::FShouldSwitchWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
	if ( !pPlayer->m_pActiveItem ) return TRUE;
	if ( !pPlayer->m_pActiveItem->CanHolster() ) return FALSE;
	return TRUE;
}

