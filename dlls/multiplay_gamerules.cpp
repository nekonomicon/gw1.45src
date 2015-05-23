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
#include	"game.h"
#include	"items.h"
#include	"sdk_u_main.h"
#include	"sdk_e_menu.h"
#include	"sdk_c_global.h"
#include	"sdk_w_armory.h"
#include	"sdk_c_waitlist.h"
#include <time.h>

// Hit Group standards
#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7

WaitList wl[4];

extern DLL_GLOBAL CGameRules	*g_pGameRules;
extern DLL_GLOBAL BOOL	g_fGameOver;
extern int gmsgDeathMsg;	// client dll messages
extern int gmsgScoreInfo;
extern int gmsgMOTD;

extern int gmsgSpectator;
extern int gmsgNANA;

void UpdateTeamMembers();

#define ITEM_RESPAWN_TIME	30
#define WEAPON_RESPAWN_TIME	20
#define AMMO_RESPAWN_TIME	20

extern CBasePlayer *last_player_killer;

void UTIL_PlayTheme(int theme);
void UTIL_BlastSound(char *filename);
void SendTitle(char *print_string, CBasePlayer *me);
//*********************************************************
// Rules for the half-life multiplayer game.
//*********************************************************

CHalfLifeMultiplay :: CHalfLifeMultiplay()
{
	//RefreshSkillData();
	m_flIntermissionEndTime = 0;
	map_stat.m_fRestartRoundTime = 0;
	
	// 11/8/98
	// Modified by YWB:  Server .cfg file is now a cvar, so that 
	//  server ops can run multiple game servers, with different server .cfg files,
	//  from a single installed directory.
	// Mapcyclefile is already a cvar.

	// 3/31/99
	// Added lservercfg file cvar, since listen and dedicated servers should not
	// share a single config file. (sjb)
	if ( IS_DEDICATED_SERVER() )
	   {
		// dedicated server
		char *servercfgfile = (char *)CVAR_GET_STRING( "servercfgfile" );

		if ( servercfgfile && servercfgfile[0] )
		   {
			char szCommand[256];
			
			ALERT( at_console, "Executing dedicated server config file\n" );
			sprintf( szCommand, "exec %s\n", servercfgfile );
			SERVER_COMMAND( szCommand );
		   }
	   }
	else
	   {
		// listen server
		char *lservercfgfile = (char *)CVAR_GET_STRING( "lservercfgfile" );

		if ( lservercfgfile && lservercfgfile[0] )
		   {
			char szCommand[256];
			
			ALERT( at_console, "Executing listen server config file\n" );
			sprintf( szCommand, "exec %s\n", lservercfgfile );
			SERVER_COMMAND( szCommand );
		   }
	   }
}

//=========================================================
//=========================================================
void CHalfLifeMultiplay::RefreshSkillData( void )
{
}

// longest the intermission can last, in seconds
#define MAX_INTERMISSION_TIME		8

extern cvar_t timeleft, fragsleft;
//=========================================================
//=========================================================
void CHalfLifeMultiplay :: Think ( void )
{
	if ( g_fGameOver )   // someone else quit the game already
	   {
		if ( m_flIntermissionEndTime < gpGlobals->time )
		   {
			if ( m_iEndIntermissionButtonHit  // check that someone has pressed a key, or the max intermission time is over
				|| ((m_flIntermissionEndTime + MAX_INTERMISSION_TIME) < gpGlobals->time) ) 
				ChangeLevel(); // intermission is over
		   }
		return;
	   }
}


//=========================================================
//=========================================================
void DropPlayerNANA(CBasePlayer *pPlayer);
void SendTitle(char *, int);
BOOL CHalfLifeMultiplay::IsMultiplayer( void ) { return TRUE; }
BOOL CHalfLifeMultiplay::IsDeathmatch( void ) { return TRUE; }
int CHalfLifeMultiplay :: IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled ) { return 1; }
BOOL CHalfLifeMultiplay::IsCoOp( void ) { return gpGlobals->coop; }
BOOL CHalfLifeMultiplay :: ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] ){ return TRUE; }
BOOL CHalfLifeMultiplay::CanHaveItem( CBasePlayer *pPlayer, CItem *pItem ){ return TRUE; }
void CHalfLifeMultiplay::PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem ){}
void CHalfLifeMultiplay::PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount ){}
float CHalfLifeMultiplay::FlHealthChargerRechargeTime( void ) { return 60; }
float CHalfLifeMultiplay::FlHEVChargerRechargeTime( void ) { return 30; }
BOOL CHalfLifeMultiplay::FShouldSwitchWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
	if ( !pWeapon->CanDeploy() ) return FALSE;
	if ( !pPlayer->m_pActiveItem ) return TRUE;
	if ( !pPlayer->m_pActiveItem->CanHolster() ) return FALSE;
	if ( pWeapon->iWeight() > pPlayer->m_pActiveItem->iWeight() ) return TRUE;
	return FALSE;
}

BOOL CHalfLifeMultiplay :: GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon )
{

	CBasePlayerItem *pCheck;
	CBasePlayerItem *pBest;
	int iBestWeight;
	int i;

	iBestWeight = -1;// no weapon lower than -1 can be autoswitched to
	pBest = NULL;

	if ( !pCurrentWeapon->CanHolster() )
	   {
		// can't put this gun away right now, so can't switch.
		return FALSE;
	   }

	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	   {
		pCheck = pPlayer->m_rgpPlayerItems[ i ];

		while ( pCheck )
		   {
			if ( pCheck->iWeight() > -1 && pCheck->iWeight() == pCurrentWeapon->iWeight() && pCheck != pCurrentWeapon )
			   {
				// this weapon is from the same category. 
				if ( pCheck->CanDeploy() )
				   {
					if ( pPlayer->SwitchWeapon( pCheck ) )
					   {
						return TRUE;
					   }
				   }
			   }
			else if ( pCheck->iWeight() > iBestWeight && pCheck != pCurrentWeapon )// don't reselect the weapon we're trying to get rid of
			   {
				//ALERT ( at_console, "Considering %s\n", STRING( pCheck->pev->classname ) );
				// we keep updating the 'best' weapon just in case we can't find a weapon of the same weight
				// that the player was using. This will end up leaving the player with his heaviest-weighted 
				// weapon. 
				if ( pCheck->CanDeploy() )
				   {
					// if this weapon is useable, flag it as the best
					iBestWeight = pCheck->iWeight();
					pBest = pCheck;
				   }
			   }

			pCheck = pCheck->m_pNext;
		   }
	   }

	// if we make it here, we've checked all the weapons and found no useable 
	// weapon in the same catagory as the current weapon. 
	
	// if pBest is null, we didn't find ANYTHING. Shouldn't be possible- should always 
	// at least get the crowbar, but ya never know.
	if ( !pBest )
	   {
		return FALSE;
	   }

	pPlayer->SwitchWeapon( pBest );

	return TRUE;
}

extern int gmsgSayText;
extern int gmsgGameMode;

void CHalfLifeMultiplay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 0 );  // game mode none
	MESSAGE_END();
}

void CHalfLifeMultiplay :: InitHUD( CBasePlayer *pl )
{
	// notify other clients of player joining the game
	//UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "%s has connected\n", 
		//( pl->pev->netname && STRING(pl->pev->netname)[0] != 0 ) ? STRING(pl->pev->netname) : "unconnected" ) );

   pl->SendJustConnected();

	UTIL_LogPrintf( "\"%s<%i>\" has entered\n",  STRING( pl->pev->netname ), GETPLAYERUSERID( pl->edict() ) );

	UpdateGameMode( pl );

	// sending just one score makes the hud scoreboard active;  otherwise
	// it is just disabled for single play
	MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pl->edict() );
		WRITE_BYTE( ENTINDEX(pl->edict()) );
		WRITE_SHORT( 0 );
		WRITE_SHORT( 0 );
	MESSAGE_END();

	// loop through all active players and send their score info to the new client
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	   {
		// FIXME:  Probably don't need to cast this just to read m_iDeaths
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( i );

		if ( plr && plr->pev)
		   {
			MESSAGE_BEGIN( MSG_ONE, gmsgScoreInfo, NULL, pl->edict() );
				WRITE_BYTE( i );	// client number
				WRITE_SHORT( plr->pev->frags );
				WRITE_SHORT( plr->m_iDeaths );
			MESSAGE_END();

         MESSAGE_BEGIN( MSG_ONE, gmsgSpectator, NULL, pl->edict() );  
		      WRITE_BYTE( i );
		      WRITE_BYTE( (plr->pev->iuser1 != 0) );
	      MESSAGE_END();
		   }
	   }

	if ( g_fGameOver )
	   {
		MESSAGE_BEGIN( MSG_ONE, SVC_INTERMISSION, NULL, pl->edict() );
		MESSAGE_END();
	   }
}

void CHalfLifeMultiplay :: ClientDisconnected( edict_t *pClient )
{
	if ( pClient )
	   {
		CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );
		if ( pPlayer )
		   {
			// Remove Nana
			if (pPlayer->m_iNANA)
            {
            pPlayer->m_iNANA = 0;            
            pPlayer->pev->flags = FL_DORMANT;
            DropPlayerNANA(pPlayer);
            }

         pPlayer->pev->frags = 0;
         pPlayer->m_iDeaths = 0;
         pPlayer->pev->deadflag = DEAD_DEAD;
         pPlayer->m_iPoints = 0;
         pPlayer->m_iJuice = 0;
         pPlayer->m_iTeam = 0;
         pPlayer->m_iMenu = 0;
         pPlayer->m_iGroup = 0;
         pPlayer->m_iMoney = aw.basemoney;
         pPlayer->m_iLicense = aw.baselicense;
         pPlayer->m_iOldLicense = aw.baselicense;
         pPlayer->m_iKilled = 0;
         pPlayer->m_iVoteMap= -1;
         memset(&pPlayer->stat, 0, sizeof (pPlayer->stat));
         pPlayer->m_iClientFlag = 0;
         pPlayer->m_iTagged = 0;
         pPlayer->m_iAdmin = 0;
         pPlayer->m_iPrimary = 0;
         pPlayer->m_iSecondary = 0;

         //pPlayer->Killed(pPlayer->pev, GIB_NEVER);
         //pPlayer->PackDeadPlayerItems();

			//FireTargets( "game_playerleave", pPlayer, pPlayer, USE_TOGGLE, 0 );
			UTIL_LogPrintf( "\"%s<%i>\" disconnected\n",  STRING( pPlayer->pev->netname ), GETPLAYERUSERID( pPlayer->edict() ) );

			pPlayer->RemoveAllItems( TRUE );// destroy all of the players weapons and items
         // Tell all clients this player isn't a spectator anymore
	      MESSAGE_BEGIN( MSG_ALL, gmsgSpectator );  
		      WRITE_BYTE( ENTINDEX(pClient) );
		      WRITE_BYTE( 0 );
	      MESSAGE_END();

         /*
	      CBasePlayer *client = NULL;
	      while ( ((client = (CBasePlayer*)UTIL_FindEntityByClassname( client, "player" )) != NULL) && (!FNullEnt(client->edict())) ) 
	         {
		      if ( !client->pev )
			      continue;
		      if ( client == pPlayer )
			      continue;

		      // If a spectator was chasing this player, move him/her onto the next player
		      if ( client->m_hObserverTarget == pPlayer )
		         {
			      int iMode = client->pev->iuser1;
			      client->pev->iuser1 = 0;
			      client->m_hObserverTarget = NULL;
			      client->Observer_SetMode( iMode );
		         }
	         }
         */
         }
	   }
   g_pGameRules->CheckDeadRoundEnd();		   
}

float CHalfLifeMultiplay :: FlPlayerFallDamage( CBasePlayer *pPlayer )
{
	pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
   return pPlayer->m_flFallVelocity * (DAMAGE_FOR_FALL_SPEED * 2);
} 

BOOL CHalfLifeMultiplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	return TRUE;
}

void CHalfLifeMultiplay :: PlayerThink( CBasePlayer *pPlayer )
{
	// check for button presses
	if ( pPlayer->m_afButtonPressed & ( IN_DUCK | IN_ATTACK | IN_ATTACK2 | IN_USE | IN_JUMP ) )
		m_iEndIntermissionButtonHit = TRUE;

	// clear attack/use commands from player
	pPlayer->m_afButtonPressed = 0;
	pPlayer->pev->button = 0;
	pPlayer->m_afButtonReleased = 0;
}

void CHalfLifeMultiplay :: PlayerSpawn( CBasePlayer *pPlayer )
{
	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
   
	if ( pPlayer->pev->deadflag > DEAD_NO)
	   {
      pPlayer->pev->deadflag = DEAD_NO;
      pPlayer->GiveNamedItem( "weapon_knife" );
      if (aw.freebies == 0)
         {
		   pPlayer->GiveNamedItem( "weapon_b93r" );
		   pPlayer->GiveAmmo(w_stat[WEAPON_GLOCK18].clip, "ammo_9mm", w_stat[WEAPON_GLOCK18].max_carry);
         }
      else
         {
         if (aw.freebies & 1)
            {
            switch (rand() & 3)
               {
               case 0:
                  pPlayer->GiveNamedItem( "weapon_1911" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_DEAGLE].clip, "ammo_50ae", w_stat[WEAPON_DEAGLE].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_DEAGLE].clip, "ammo_50ae", w_stat[WEAPON_DEAGLE].max_carry);
                  break;
               case 1:
                  pPlayer->GiveNamedItem( "weapon_92d" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_92D].clip, "ammo_357sig", w_stat[WEAPON_92D].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_92D].clip, "ammo_357sig", w_stat[WEAPON_92D].max_carry);
                  break;
               default:
		            pPlayer->GiveNamedItem( "weapon_b93r" );
		            pPlayer->GiveAmmo(w_stat[WEAPON_GLOCK18].clip, "ammo_9mm", w_stat[WEAPON_GLOCK18].max_carry);
		            pPlayer->GiveAmmo(w_stat[WEAPON_GLOCK18].clip, "ammo_9mm", w_stat[WEAPON_GLOCK18].max_carry);
                  break;
               }
            }
         if (aw.freebies & 2)
            {
            switch (rand() & 15)
               {
               case 0:
                  pPlayer->GiveNamedItem( "weapon_mac10" );
	               pPlayer->GiveAmmo(w_stat[WEAPON_MAC10].clip, "ammo_9mm", w_stat[WEAPON_MAC10].max_carry);
	               pPlayer->GiveAmmo(w_stat[WEAPON_MAC10].clip, "ammo_9mm", w_stat[WEAPON_MAC10].max_carry);
                  break;
               case 1:
               case 2:
                  pPlayer->GiveNamedItem( "weapon_tmp" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_TMP].clip, "ammo_9mm", w_stat[WEAPON_TMP].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_TMP].clip, "ammo_9mm", w_stat[WEAPON_TMP].max_carry);
                  break;
               case 3:
               case 4:
                  pPlayer->GiveNamedItem( "weapon_mp5navy" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_MP5NAVY].clip, "ammo_9mm", w_stat[WEAPON_MP5NAVY].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_MP5NAVY].clip, "ammo_9mm", w_stat[WEAPON_MP5NAVY].max_carry);
                  break;
               case 5:
                  pPlayer->GiveNamedItem( "weapon_ump45" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_UMP45].clip, "ammo_9mm", w_stat[WEAPON_UMP45].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_UMP45].clip, "ammo_9mm", w_stat[WEAPON_UMP45].max_carry);
                  break;
               case 6:
               case 7:
                  pPlayer->GiveNamedItem( "weapon_m3" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_UMP45].clip, "ammo_9mm", w_stat[WEAPON_UMP45].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_UMP45].clip, "ammo_9mm", w_stat[WEAPON_UMP45].max_carry);
                  break;
               case 8:
                  pPlayer->GiveNamedItem( "weapon_xm1014" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_XM1014].clip, "ammo_buckshot", w_stat[WEAPON_XM1014].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_XM1014].clip, "ammo_buckshot", w_stat[WEAPON_XM1014].max_carry);
                  break;
               case 9:
               case 10:
                  pPlayer->GiveNamedItem( "weapon_m16" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_M16].clip, "ammo_556nato", w_stat[WEAPON_M16].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_M16].clip, "ammo_556nato", w_stat[WEAPON_M16].max_carry);
                  break;
               case 11:
                  pPlayer->GiveNamedItem( "weapon_sg552" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_SG552].clip, "ammo_556nato", w_stat[WEAPON_SG552].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_SG552].clip, "ammo_556nato", w_stat[WEAPON_SG552].max_carry);
                  break;
               case 12:
               case 13:
                  pPlayer->GiveNamedItem( "weapon_ak47" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_AK47].clip, "ammo_762nato", w_stat[WEAPON_AK47].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_AK47].clip, "ammo_762nato", w_stat[WEAPON_AK47].max_carry);
                  break;
               case 14:
                  pPlayer->GiveNamedItem( "weapon_sg550" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_SG550].clip, "ammo_762nato", w_stat[WEAPON_SG550].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_SG550].clip, "ammo_762nato", w_stat[WEAPON_SG550].max_carry);
                  break;
               case 15:
                  pPlayer->GiveNamedItem( "weapon_aug" );
                  pPlayer->GiveAmmo(w_stat[WEAPON_AUG].clip, "ammo_762nato", w_stat[WEAPON_AUG].max_carry);
                  pPlayer->GiveAmmo(w_stat[WEAPON_AUG].clip, "ammo_762nato", w_stat[WEAPON_AUG].max_carry);
                  break;
               }
            }
         }
	   }
}


//=========================================================
// PlayerKilled - someone/something killed this player
//=========================================================
extern int gmsgCLSpawn;
void CHalfLifeMultiplay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
   if (!pKiller)
      return;

	DeathNotice(pVictim, pKiller, pInflictor);

	pVictim->m_iDeaths += 1;
   pVictim->m_iKilled = 1;

	//FireTargets( "game_playerdie", pVictim, pVictim, USE_TOGGLE, 0 );
	CBasePlayer *peKiller = NULL;

	if (pVictim->pev == pKiller )  
	   {
      pVictim->stat.Suicides++;
		//pKiller->frags -= 1;
	   }

	CBaseEntity *ktmp = CBaseEntity::Instance( pKiller );
	if (ktmp && ktmp->IsPlayer())
	   {
		peKiller = (CBasePlayer*)ktmp;
		// if a player dies in a deathmatch game and the killer is a client, award the killer some points
		pKiller->frags += IPointsForKill( peKiller, pVictim );	
		//FireTargets( "game_playerkill", ktmp, ktmp, USE_TOGGLE, 0 );
	   }
	else
	   {  // killed by the world
		//pVictim->pev->frags -= 1;
      pVictim->stat.Suicides++;
	   }

   if (pVictim->pev->frags < 0)
      pVictim->pev->frags = 0;

	// update the scores
	// killed scores
	MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
		WRITE_BYTE( ENTINDEX(pVictim->edict()) );
		WRITE_SHORT( pVictim->pev->frags );
		WRITE_SHORT( pVictim->m_iDeaths );
	MESSAGE_END();

   // Dead player will join the wait list
   if (pVictim->m_iTeam == TEAM_EAST)
      wl[TEAM_EAST].AddToWaitList(pVictim);
   else if (pVictim->m_iTeam == TEAM_WEST)
      wl[TEAM_WEST].AddToWaitList(pVictim);
}

//=========================================================
// Deathnotice. 
//=========================================================
void CHalfLifeMultiplay::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	// Work out what killed the player, and send a message to all clients about it
   CBaseEntity *Killer;
   CBasePlayer *PK = NULL;

	const char *killer_weapon_name = "world";
   int deathmedium = 0;  
	int killer_index = 0;

   if (!pKiller)
      return;
	
	Killer = CBaseEntity::Instance( pKiller );

   if (!Killer)
      return;

	if (pKiller->flags & FL_CLIENT)
	   {
		killer_index = ENTINDEX(ENT(pKiller));	
      PK = (CBasePlayer *) Killer;
	   }
	if ( pevInflictor )
      killer_weapon_name = STRING( pevInflictor->classname );

	// strip the monster_* or weapon_* from the inflictor's classname
   deathmedium = D_MEDIUM_UNKNOWN;
   if (!killer_weapon_name || !killer_weapon_name[0])
      return;
	else if (!strcmp( killer_weapon_name, "weapon_knife"))
      {
      if (PK)
         PK->stat.KnifeKills++;
      deathmedium = WEAPON_KNIFE;
      }
	else if (!strcmp( killer_weapon_name, "weapon_hegrenade"))
      {
      deathmedium = WEAPON_HEGRENADE;
      }
	else if (!strncmp( killer_weapon_name, "weapon_", 7 ))
      {
	   if (!FNullEnt( pevInflictor ))
	      {
		   CBasePlayerWeapon *pInflictor = (CBasePlayerWeapon *) CBaseEntity::Instance( pevInflictor );
		   if (pInflictor)
            {
            deathmedium = pInflictor->m_iId;
            if (PK)
               {
               if (deathmedium > 0 && deathmedium < 48)
                  {
                  if (!PK->m_iKilled)
                     PK->stat.RoundKills++;
                  PK->stat.Kills[deathmedium]++;
                  pVictim->stat.Deaths[deathmedium]++;
                  }
               }
            }
	      }
      }
   if (pVictim->m_LastHitGroup == HITGROUP_HEAD)
      if (PK) PK->stat.HeadShots++;

	MESSAGE_BEGIN( MSG_ALL, gmsgDeathMsg );
		WRITE_BYTE( killer_index );						// the killer
		WRITE_BYTE( ENTINDEX(pVictim->edict()) );		// the victim
      WRITE_BYTE( deathmedium);		               // medium
      WRITE_BYTE( pVictim->m_LastHitGroup );		   // spot
	MESSAGE_END();

	if ( pVictim->pev == pKiller )  
	   { 		
      UTIL_LogPrintf( "%s killed self\n",  STRING( pVictim->pev->netname ));
	   }
   else if ((pKiller->flags & FL_CLIENT) && killer_weapon_name && killer_weapon_name[0])
      {
      UTIL_LogPrintf( "%s killed %s with %s\n",  
               STRING( pKiller->netname ), 
               STRING( pVictim->pev->netname ), 
               killer_weapon_name);
      }

	return; // just remove for now
}

//=========================================================
// PlayerGotWeapon - player has grabbed a weapon that was
// sitting in the world
//=========================================================
void CHalfLifeMultiplay :: PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
}

//=========================================================
// CanHaveWeapon - returns FALSE if the player is not allowed
// to pick up this weapon
//=========================================================
BOOL CHalfLifeMultiplay::CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pItem )
{
	return CGameRules::CanHavePlayerItem( pPlayer, pItem );
}

edict_t *CHalfLifeMultiplay::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	edict_t *pentSpawnSpot = CGameRules::GetPlayerSpawnSpot( pPlayer );
	return pentSpawnSpot;
}


BOOL CHalfLifeMultiplay :: PlayFootstepSounds( CBasePlayer *pl, float fvol )
{
	if ( pl->IsOnLadder())
		return TRUE;  // only make step sounds in multiplayer if the player is moving fast enough

   if (pl->pev->velocity.Length2D() > 180)
      return TRUE;
	return FALSE;
}

BOOL CHalfLifeMultiplay :: FAllowFlashlight( void ) 
{ 
	return CVAR_GET_FLOAT( "mp_flashlight" ) != 0; 
}

//=========================================================
//======== CHalfLifeMultiplay private functions ===========
#define INTERMISSION_TIME		6

void CHalfLifeMultiplay :: GoToIntermission( void )
{
	if ( g_fGameOver )
		return;  // intermission has already been triggered, so ignore.

	MESSAGE_BEGIN(MSG_ALL, SVC_INTERMISSION);
	MESSAGE_END();

	m_flIntermissionEndTime = gpGlobals->time + INTERMISSION_TIME;
	g_fGameOver = TRUE;
	m_iEndIntermissionButtonHit = FALSE;
}

#define MAX_RULE_BUFFER 1024

typedef struct mapcycle_item_s
{
	struct mapcycle_item_s *next;

	char mapname[ 32 ];
	int  minplayers, maxplayers;
	char rulebuffer[ MAX_RULE_BUFFER ];
} mapcycle_item_t;

typedef struct mapcycle_s
{
	struct mapcycle_item_s *items;
	struct mapcycle_item_s *next_item;
} mapcycle_t;

/*
==============
DestroyMapCycle

Clean up memory used by mapcycle when switching it
==============
*/
void DestroyMapCycle( mapcycle_t *cycle )
{
	mapcycle_item_t *p, *n, *start;
	p = cycle->items;
	if ( p )
	{
		start = p;
		p = p->next;
		while ( p != start )
		{
			n = p->next;
			delete p;
			p = n;
		}
		
		delete cycle->items;
	}
	cycle->items = NULL;
	cycle->next_item = NULL;
}

static char com_token[ 1500 ];

/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse (char *data)
{
	int             c;
	int             len;
	
	len = 0;
	com_token[0] = 0;
	
	if (!data)
		return NULL;
		
// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;
		data++;
	}
	
// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}
	

// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

// parse single characters
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}

// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
			break;
	} while (c>32);
	
	com_token[len] = 0;
	return data;
}

/*
==============
COM_TokenWaiting

Returns 1 if additional data is waiting to be processed on this line
==============
*/
int COM_TokenWaiting( char *buffer )
{
	char *p;

	p = buffer;
	while ( *p && *p!='\n')
	{
		if ( !isspace( *p ) || isalnum( *p ) )
			return 1;

		p++;
	}

	return 0;
}

/*
==============
ReloadMapCycleFile


Parses mapcycle.txt file into mapcycle_t structure
==============
*/
int ReloadMapCycleFile( char *filename, mapcycle_t *cycle )
{
	char szBuffer[ MAX_RULE_BUFFER ];
	char szMap[ 32 ];
	int length;
	char *pFileList;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( filename, &length );
	int hasbuffer;
	mapcycle_item_s *item, *newlist = NULL, *next;

	if ( pFileList && length )
	{
		// the first map name in the file becomes the default
		while ( 1 )
		{
			hasbuffer = 0;
			memset( szBuffer, 0, MAX_RULE_BUFFER );

			pFileList = COM_Parse( pFileList );
			if ( strlen( com_token ) <= 0 )
				break;

			strcpy( szMap, com_token );

			// Any more tokens on this line?
			if ( COM_TokenWaiting( pFileList ) )
			{
				pFileList = COM_Parse( pFileList );
				if ( strlen( com_token ) > 0 )
				{
					hasbuffer = 1;
					strcpy( szBuffer, com_token );
				}
			}

			// Check map
			if ( IS_MAP_VALID( szMap ) )
			{
				// Create entry
				char *s;

				item = new mapcycle_item_s;

				strcpy( item->mapname, szMap );

				item->minplayers = 0;
				item->maxplayers = 0;

				memset( item->rulebuffer, 0, MAX_RULE_BUFFER );

				if ( hasbuffer )
				{
					s = g_engfuncs.pfnInfoKeyValue( szBuffer, "minplayers" );
					if ( s && s[0] )
					{
						item->minplayers = atoi( s );
						item->minplayers = max( item->minplayers, 0 );
						item->minplayers = min( item->minplayers, gpGlobals->maxClients );
					}
					s = g_engfuncs.pfnInfoKeyValue( szBuffer, "maxplayers" );
					if ( s && s[0] )
					{
						item->maxplayers = atoi( s );
						item->maxplayers = max( item->maxplayers, 0 );
						item->maxplayers = min( item->maxplayers, gpGlobals->maxClients );
					}

					// Remove keys
					//
					g_engfuncs.pfnInfo_RemoveKey( szBuffer, "minplayers" );
					g_engfuncs.pfnInfo_RemoveKey( szBuffer, "maxplayers" );

					strcpy( item->rulebuffer, szBuffer );
				}

				item->next = cycle->items;
				cycle->items = item;
			}
			else
			{
				ALERT( at_console, "Skipping %s from mapcycle, not a valid map\n", szMap );
			}


		}

		FREE_FILE( aFileList );
	}

	// Fixup circular list pointer
	item = cycle->items;

	// Reverse it to get original order
	while ( item )
	{
		next = item->next;
		item->next = newlist;
		newlist = item;
		item = next;
	}
	cycle->items = newlist;
	item = cycle->items;

	// Didn't parse anything
	if ( !item )
	{
		return 0;
	}

	while ( item->next )
	{
		item = item->next;
	}
	item->next = cycle->items;
	
	cycle->next_item = item->next;

	return 1;
}

/*
==============
CountPlayers

Determine the current # of active players on the server for map cycling logic
==============
*/
int CountPlayers( void )
{
	int	num = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );

		if ( pEnt )
		{
			num = num + 1;
		}
	}

	return num;
}

/*
==============
ExtractCommandString

Parse commands/key value pairs to issue right after map xxx command is issued on server
 level transition
==============
*/
void ExtractCommandString( char *s, char *szCommand )
{
	// Now make rules happen
	char	pkey[512];
	char	value[512];	// use two buffers so compares
								// work without stomping on each other
	char	*o;
	
	if ( *s == '\\' )
		s++;

	while (1)
	{
		o = pkey;
		while ( *s != '\\' )
		{
			if ( !*s )
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;

		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		strcat( szCommand, pkey );
		if ( strlen( value ) > 0 )
		{
			strcat( szCommand, " " );
			strcat( szCommand, value );
		}
		strcat( szCommand, "\n" );

		if (!*s)
			return;
		s++;
	}
}

/*
==============
ChangeLevel

Server is changing to a new level, check mapcycle.txt for map name and setup info
==============
*/
void CHalfLifeMultiplay :: ChangeLevel( void )
{
	static char szPreviousMapCycleFile[ 256 ];
	static mapcycle_t mapcycle;


	char szNextMap[32];
	char szFirstMapInList[32];
	char szCommands[ 1500 ];
	char szRules[ 1500 ];
	int minplayers = 0, maxplayers = 0;
	strcpy( szFirstMapInList, "gw_hive" );  // the absolute default level is hldm1

	int	curplayers;
	BOOL do_cycle = TRUE;

	// find the map to change to
	char *mapcfile = (char*)CVAR_GET_STRING( "mapcyclefile" );
	ASSERT( mapcfile != NULL );

	szCommands[ 0 ] = '\0';
	szRules[ 0 ] = '\0';

	curplayers = CountPlayers();

   if (aw.nextmap[0]) 
      {
      if (IS_MAP_VALID(aw.nextmap))
         {
         strcpy(szNextMap, aw.nextmap);
         goto IHaveMap;
         }
      }

	// Has the map cycle filename changed?
	if ( stricmp( mapcfile, szPreviousMapCycleFile ) )
	   {
		strcpy( szPreviousMapCycleFile, mapcfile );

		DestroyMapCycle( &mapcycle );

		if ( !ReloadMapCycleFile( mapcfile, &mapcycle ) || ( !mapcycle.items ) )
		   {
			ALERT( at_console, "Unable to load map cycle file %s\n", mapcfile );
			do_cycle = FALSE;
		   }
	   }

	if ( do_cycle && mapcycle.items )
	   {
		BOOL keeplooking = FALSE;
		BOOL found = FALSE;
		mapcycle_item_s *item;

		// Assume current map
		strcpy( szNextMap, STRING(gpGlobals->mapname) );
		strcpy( szFirstMapInList, STRING(gpGlobals->mapname) );

		// Traverse list
		for ( item = mapcycle.next_item; item->next != mapcycle.next_item; item = item->next )
		   {
			keeplooking = FALSE;

			ASSERT( item != NULL );

			if ( item->minplayers != 0 )
			   {
				if ( curplayers >= item->minplayers )
				   {
					found = TRUE;
					minplayers = item->minplayers;
				   }
				else
				   {
					keeplooking = TRUE;
				   }
			   }

			if ( item->maxplayers != 0 )
			   {
				if ( curplayers <= item->maxplayers )
				   {
					found = TRUE;
					maxplayers = item->maxplayers;
				   }
				else
				   {
					keeplooking = TRUE;
				   }
			   }

			if ( keeplooking )
				continue;

			found = TRUE;
			break;
		   }

		if ( !found )
		   {
			item = mapcycle.next_item;
		   }			
		
		// Increment next item pointer
		mapcycle.next_item = item->next;

		// Perform logic on current item
		strcpy( szNextMap, item->mapname );

		ExtractCommandString( item->rulebuffer, szCommands );
		strcpy( szRules, item->rulebuffer );
	   }

	if ( !IS_MAP_VALID(szNextMap) )
	   {
		strcpy( szNextMap, szFirstMapInList );
	   }


	ALERT( at_console, "CHANGE LEVEL: %s\n", szNextMap );
	if ( minplayers || maxplayers )
	   {
		ALERT( at_console, "PLAYER COUNT:  min %i max %i current %i\n", minplayers, maxplayers, curplayers );
	   }
	if ( strlen( szRules ) > 0 )
	   {
		ALERT( at_console, "RULES:  %s\n", szRules );
	   }
	
IHaveMap:

	g_fGameOver = TRUE;

	CHANGE_LEVEL( szNextMap, NULL );
	if ( strlen( szCommands ) > 0 )
	   {
		SERVER_COMMAND( szCommands );
	   }
}

#define MAX_MOTD_CHUNK	  60
#define MAX_MOTD_LENGTH   (MAX_MOTD_CHUNK * 4)


void SendTitle(char *print_string, int team);
#define ROUND_RESTART_TIME 7
void CHalfLifeMultiplay::CheckRoundEnd()
{
   // In case somebody dies once the round is over. Official CS
   // has double win problems:
   if (map_stat.m_fRestartRoundTime)
      return;

   UpdateTeamMembers();

   if (map_stat.g_fRoundEndTime < gpGlobals->time)
      {
      // Round draw: suckers camped out? Give money + points only
      // to dead peeps:
      // If it's an EAST objective round and EAST did nothing about it, 
      // screw them
      // If it's a WEST objective round and WEST did nothing about it,
      // screw them too
      SendTitle("Round Draw", 0);
      
      if (aw.quiet)
         UTIL_BlastSound("ambience/winbuzz.wav");
      else
         UTIL_PlayTheme(1);

      GiveDeadTeam(TEAM_EAST, team[TEAM_EAST].draw_money, team[TEAM_EAST].draw_points);
      GiveDeadTeam(TEAM_WEST, team[TEAM_WEST].draw_money, team[TEAM_WEST].draw_points);
      map_stat.m_fRestartRoundTime = gpGlobals->time + ROUND_RESTART_TIME;
      return;
      }
}
void CleanUpMap();
void CHalfLifeMultiplay::CheckDeadRoundEnd()
{
   // In case somebody dies once the round is over. Official CS
   // has double win problems:
   if (map_stat.m_fRestartRoundTime)
      return;

   UpdateTeamMembers();

   // Only one player on server? If the player dies, do not give money:
   if (team[TEAM_EAST].alive <= 0 && team[TEAM_EAST].members)
      {
      if (team[TEAM_WEST].alive <= 0 && team[TEAM_WEST].members)
         {
         SendTitle("Round Draw", 0);
         GiveDeadTeam(TEAM_EAST, team[TEAM_EAST].draw_money, team[TEAM_EAST].draw_points);
         GiveDeadTeam(TEAM_WEST, team[TEAM_WEST].draw_money, team[TEAM_WEST].draw_points);
         map_stat.m_fRestartRoundTime = gpGlobals->time + ROUND_RESTART_TIME;
         return;
         }
      // East loses:
      if (team[TEAM_WEST].alive == team[TEAM_WEST].members)
         SendTitle("PERFECT SCORE!\nWest coast wins", TEAM_WEST);
      else
         SendTitle("West coast wins", TEAM_WEST);

      if (aw.quiet)
         UTIL_BlastSound("ambience/winbuzz.wav");
      else
         UTIL_PlayTheme(3);

      GiveTeam(TEAM_EAST, team[TEAM_EAST].lose_money, team[TEAM_EAST].lose_points, 0, 0);
      GiveTeam(TEAM_WEST, team[TEAM_WEST].win_money, team[TEAM_WEST].win_points,
               team[TEAM_WEST].winner_money, team[TEAM_WEST].winner_points);
      map_stat.m_fRestartRoundTime = gpGlobals->time + ROUND_RESTART_TIME;
      map_stat.g_iWins[TEAM_WEST]++;
      return;
      }
   if (team[TEAM_WEST].alive <= 0 && team[TEAM_WEST].members)
      {
      // West loses (ya baby):
      if (team[TEAM_EAST].alive == team[TEAM_EAST].members)
         SendTitle("PERFECT SCORE!\nEast coast wins", TEAM_EAST);
      else
         SendTitle("East coast wins", TEAM_EAST);

      if (aw.quiet)
         UTIL_BlastSound("ambience/winbuzz.wav");
      else
         UTIL_PlayTheme(2);

      GiveTeam(TEAM_WEST, team[TEAM_WEST].lose_money, team[TEAM_WEST].lose_points, 0, 0);
      GiveTeam(TEAM_EAST, team[TEAM_EAST].win_money, team[TEAM_EAST].win_points,
               team[TEAM_EAST].winner_money, team[TEAM_EAST].winner_points);
      map_stat.m_fRestartRoundTime = gpGlobals->time + ROUND_RESTART_TIME;
      map_stat.g_iWins[TEAM_EAST]++;
      return;
      }
}

// NANA Pack
extern int gmsgRadar;
LINK_ENTITY_TO_CLASS( item_backpack, CItemBackPack );

void CItemBackPack::Spawn( void )
{ 
   Precache( );
   
   if (m_iTeam == TEAM_EAST)
      SET_MODEL(ENT(pev), "models/w_bageast.mdl");
   else
      SET_MODEL(ENT(pev), "models/w_bagwest.mdl");

   CItem::Spawn( );
   pev->nextthink = gpGlobals->time + 4;
   m_flNextResawnSpot = gpGlobals->time + aw.respawntime;
}

void CItemBackPack::Precache( void )
{
   PRECACHE_MODEL ("models/w_bageast.mdl");
   PRECACHE_MODEL ("models/w_bagwest.mdl");
   PRECACHE_SOUND ("items/kevlar.wav");
}

BOOL CItemBackPack::MyTouch( CBasePlayer *pPlayer )
{
   if (!pPlayer->m_iTeam) return FALSE;
   if (pPlayer->pev->deadflag > DEAD_NO) return FALSE;
   if (pPlayer->m_iTeam != m_iTeam) return FALSE;
   if (pPlayer->pev->health <= 0) return FALSE;
	pPlayer->m_iNANA = 1;
   pPlayer->m_flNextNANABlip = gpGlobals->time + 4;
	pPlayer->pev->body = 1;
   if (aw.grenades)
      pPlayer->GiveNamedItem( "weapon_hegrenade" );
	EMIT_SOUND( ENT(pPlayer->pev), CHAN_VOICE, "items/kevlar.wav", 1, ATTN_NORM);         
   char xx[128];
   sprintf (xx, "^_ ^g %s is your new boss\nProtect him!", STRING(pPlayer->pev->netname), pPlayer);
   //SendTeamTitle(xx, m_iTeam, pPlayer);
   SendTitleTeamOnly(xx, m_iTeam, pPlayer);
   sprintf (xx, "^_ ^g You are the new boss\nProtect your gang!");
   //ClientPrint(pPlayer->pev, HUD_PRINTCENTER, xx);
   SendTitle(xx, pPlayer);
   UTIL_LogPrintf( "%s became boss\n", STRING(pPlayer->pev->netname) );

   SendBossStatus(1, pPlayer);

	return TRUE;
}
extern float map_bag_position[5][3];
void CItemBackPack::Think( void ) 
{ 
   CBaseEntity *pPlayer = NULL;
   Vector v_other, v_radar;
   //float distance;
   int index, sent = 0;
   if (m_flNextResawnSpot && m_flNextResawnSpot < gpGlobals->time)
      {
      float *x = NULL;
      m_flNextResawnSpot = 0;
      if (m_iTeam == 1)
         x = map_bag_position[0];
      else
         x = map_bag_position[1];
      if (pev->origin[0] != x[0] && pev->origin[1] != x[1])
         {
	      pev->origin[0] = x[0];
	      pev->origin[1] = x[1];
	      pev->origin[2] = x[2];
         SET_ORIGIN(ENT(pev), pev->origin );
         pev->movetype = MOVETYPE_TOSS;
	      pev->solid = SOLID_TRIGGER;
	      if (DROP_TO_FLOOR(ENT(pev)) == 0)
	         {
		      ALERT(at_error, "Item %s fell out of level at %f,%f,%f", STRING( pev->classname ), pev->origin.x, pev->origin.y, pev->origin.z);
		      UTIL_Remove( this );
		      return;
	         }
         }
      }

   for (index = 1; index <= gpGlobals->maxClients; index++)
      {
      pPlayer = UTIL_PlayerByIndex( index );
      if (!pPlayer) continue;
      if (!pPlayer->pev) continue;
      if (FNullEnt(pPlayer->edict())) continue;
      if (pPlayer->pev->deadflag > DEAD_NO) continue;
      v_other = pev->origin - pPlayer->pev->origin;
      //distance = v_other.Length();
      ////if (distance < 2000)
         //{
         sent = 1;
         MESSAGE_BEGIN( MSG_ONE, gmsgRadar, NULL, pPlayer->pev);
         if (((CBasePlayer *)pPlayer)->m_iTeam == m_iTeam)
            WRITE_BYTE(1); // Send Green
         else 
            WRITE_BYTE(2); // Send Red
         //WRITE_BYTE(m_iTeam);                // turn on radar
         WRITE_COORD(pev->origin.x);
         WRITE_COORD(pev->origin.y);
         WRITE_COORD(pev->origin.z);
         MESSAGE_END();
         //}
      }
   if (!sent) pev->nextthink = gpGlobals->time + 2;
   else pev->nextthink = gpGlobals->time + 4;
}
