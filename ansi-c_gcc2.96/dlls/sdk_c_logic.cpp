/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_c_logic.cpp
   This is the server/game logic module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/


// #include <string.h>
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "player.h"
// #include "trains.h"
#include "spectator.h"
#include "client.h"
#include "soundent.h"
#include "gamerules.h"
#include "weapons.h"
// #include "decals.h"
// added monsters.h, trains.h, shake.h in engine dir, 
// #include "monsters.h"
// #include "shake.h" 
#include <string.h>
#include "func_break.h"
#include "doors.h"
#include "sdk_u_main.h"
#include "sdk_e_menu.h"
#include "sdk_c_global.h"
#include "sdk_c_waitlist.h"
// added classes header
// #include "sdk_w_classes.h"

// adding declarations/definitions? july 2015 - already in weapons.h
// BOOL CBasePlayerItem::CBasePlayerItem ( void ){}
// void CBasePlayerItem::ItemInfoArray ( void ){}

extern int gsmgVoteMaps;

void SendTitle(char *);
void DropNANA(int team);
void ShowSpawns(int team);
level_t map_stat;
// BOOL CBasePlayerItem(
// BOOL CBasePlayer::CBasePlayerItem(
CBasePlayer *last_player_killer;
// int CBasePlayerItem::CBasePlayerItem ( void ) {};
// int CBasePlayerItem::ItemInfoArray (void ) {};
// ItemInfo CBasePlayerItem::ItemInfoArray[MAX_WEAPONS];
// AmmoInfo CBasePlayerItem::AmmoInfoArray[MAX_AMMO_SLOTS];
// LINK_ENTITY_TO_CLASS( player, CBasePlayer )

// void CBasePlayerItem::ItemInfoArray( void )
// {
// }

// int CBasePlayer::CBasePlayerItem( void )
// {
// }

void DropPrimaryWeapon (CBasePlayer *player)
{
   if (player->m_pActiveItem)
	// edit: added "CBasePlayer::" to "ItemInfoArray" fixing many errors about "undefined reference"
      switch (player->m_pActiveItem->ItemInfoArray[player->m_pActiveItem->m_iId].iWeight)
         {
         case (WEIGHT_PRIMARY):
            {
            player->DropPlayerItem("");
            player->m_iMaxSpeed = 300;
            player->m_iPrimary = 0;
            break;
            }
         case (WEIGHT_SECONDARY):
            {
            // char *p = (char*) STRING(player->m_pActiveItem->pev->classname);     
            player->DropPlayerItem("");
            break;
            }
         }
}

void DropNANA3(CBasePlayer * player)
{
   //if (player->m_iNANA)
      //DropNANA(player->m_iTeam);
      //player->DropPlayerItem("item_backpack");
}

void RemoveGuns (void)
{
	CBaseEntity* temp_del = NULL;
	while ((temp_del = UTIL_FindEntityByClassname(temp_del, "weaponbox")) != NULL)
      ((CWeaponBox*)temp_del)->Kill();     
	while ((temp_del = UTIL_FindEntityByClassname(temp_del, "item_backpack")) != NULL)
      UTIL_Remove(temp_del);
}

void CleanUpMap (void)
{
	CBaseEntity* reflush = NULL;
	CBaseEntity* temp_del = NULL;

	reflush = UTIL_FindEntityByClassname(NULL, "func_breakable");
	while (reflush != NULL)
	   {
		((CBreakable *) reflush)->Flush();
		reflush = UTIL_FindEntityByClassname(reflush, "func_breakable");
	   }

	reflush = UTIL_FindEntityByClassname(NULL, "func_door");
	while (reflush != NULL)
	   {
		((CBaseDoor *) reflush)->Flush();
		reflush = UTIL_FindEntityByClassname(reflush, "func_door");
	   }

   reflush = UTIL_FindEntityByClassname(NULL, "func_door_rotating");
	while (reflush != NULL)
	   {
		((CBaseDoor *) reflush)->Flush();
		reflush = UTIL_FindEntityByClassname(reflush, "func_door_rotating");
	   }

   reflush = UTIL_FindEntityByClassname(NULL, "ambient_generic");
	while (reflush != NULL)
	   {
		((CAmbientGeneric *) reflush)->Flush();
		reflush = UTIL_FindEntityByClassname(reflush, "ambient_generic");
	   }

/*	torestart = UTIL_FindEntityByClassname(NULL, "func_train");
	while (torestart != NULL)
	{
		ALERT (at_console, "Restarting a func_train\n");
		torestart->Restart();
		torestart = UTIL_FindEntityByClassname(torestart, "func_train");
	} */

   RemoveGuns ();
}


void ComputeJuice(CBasePlayer *current);
void RestartRound()
{
   CBaseEntity* pPlayer = NULL;
   CBasePlayer* player;
   int kicked;
   char *nn;
   map_stat.g_fRoundStartTime = gpGlobals->time;
   map_stat.g_fRoundEndTime = gpGlobals->time + map_stat.g_iRoundTime;
   /* FIXME */
   map_stat.g_fRoundEndRespawnTime = gpGlobals->time + 10;
   map_stat.g_fRoundArmoryCloseTime = gpGlobals->time + 90;

   CleanUpMap();
   wl[0].FlushWaitList();
   wl[TEAM_EAST].FlushWaitList();
   wl[TEAM_WEST].FlushWaitList();
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      nn = (char *)STRING(pPlayer->pev->netname);
      if (!nn)
         continue;

      if (!nn[0])
         continue;

      player = (CBasePlayer*)pPlayer;
      //player->m_iClientFlag &= ~CF_JUST_CONNECTED;
      
      if (player == last_player_killer)
         {
         player->stat.LastKills++;
         player->m_iNextMoney += aw.lastkillmoney;
         player->m_iJuice += AWARD_JUICE_EXTERMINATOR;
         }
      if (player->stat.RoundKills > player->stat.MaxRoundKills)
         {
         player->stat.MaxRoundKills = player->stat.RoundKills;
         }
      if (!player->m_iKilled)
         {
         player->stat.MaxRoundLives++;
         player->m_iNextMoney += aw.killmoney;
         }
      player->stat.Rounds++;
      player->stat.RoundKills = 0;
      player->m_iKilled = 0;
      //ComputeJuice(player);
      player->m_iMoney += player->m_iNextMoney;
      FitBestScore(player);
      kicked = 0;
      player->stat.ping_rounds++;
      if (player->stat.ping_rounds >= CVAR_GET_FLOAT( "gw_ping_rounds" ))
         {
         kicked = player->HighPingCheck();
         player->stat.ping_rounds = 0;
         }
      if (kicked)
         continue;
      pPlayer->Spawn();
	   }  
   DropNANA(TEAM_EAST);
   DropNANA(TEAM_WEST);
   ShowSpawns(TEAM_EAST);
   ShowSpawns(TEAM_WEST);
   SaveBestScore();
   map_stat.g_iRounds++;
   UTIL_LogPrintf( "Round %i Restarted\n", map_stat.g_iRounds);
}

void FreshRound()
{
   CBaseEntity* pPlayer = NULL;
   CBasePlayer* player;
   char *nn;
   map_stat.g_fRoundStartTime = gpGlobals->time;
   map_stat.g_fRoundEndTime = gpGlobals->time + map_stat.g_iRoundTime; 
   /* FIXME */
   map_stat.g_fRoundEndRespawnTime = gpGlobals->time + 10;
   map_stat.g_fRoundArmoryCloseTime = gpGlobals->time + 90;

   CleanUpMap();
   wl[0].FlushWaitList();
   wl[TEAM_EAST].FlushWaitList();
   wl[TEAM_WEST].FlushWaitList();
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;
		if (!pPlayer->IsPlayer())
         continue;
      if (pPlayer->pev->flags & FL_DORMANT)
         continue;
      nn = (char *)STRING(pPlayer->pev->netname);
      if (!nn)
         continue;

      if (!nn[0])
         continue;

      //GetClassPtr ((CBasePlayer*)pPlayer->pev)->m_iClientFlag &= ~CF_JUST_CONNECTED;
      player = (CBasePlayer*)pPlayer;
      player->stat.RoundKills = 0;
      player->m_iKilled = 0;
      FitBestScore(player);
      pPlayer->Spawn();
	   }  
   DropNANA(TEAM_EAST);
   DropNANA(TEAM_WEST);
   ShowSpawns(TEAM_EAST);
   ShowSpawns(TEAM_WEST);
   SaveBestScore();
   UTIL_LogPrintf( "Fresh Round %i Restarted\n", map_stat.g_iRounds);
}


int CompileMapVotes()
{
   CBaseEntity* pPlayer = NULL;
   CBasePlayer* player;
   char *nn;
   int i;
   int num_players = 0;
   float ff;
   
   if (!CVAR_GET_FLOAT( "gw_mapvote" ))
      return 0;

   for (i = 0; i < map_vote_count; i++)
      {
      map_vote[i].ratio = 0;
      map_vote[i].votes = 0;
      }

	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      nn = (char *)STRING(pPlayer->pev->netname);
      if (!nn)
         continue;

      if (!nn[0])
         continue;

      player = (CBasePlayer*)pPlayer;
      //player->m_iClientFlag &= ~CF_JUST_CONNECTED;
      num_players++;
      if (player->m_iVoteMap >=0 && player->m_iVoteMap < map_vote_count)
         map_vote[player->m_iVoteMap].votes++; 
      }
   if (num_players)
      {
      ff = num_players;
      for (i = 0; i < map_vote_count; i++)
         {
         map_vote[i].ratio = map_vote[i].votes;
         map_vote[i].ratio /= ff;
         if (map_vote[i].ratio > CVAR_GET_FLOAT( "gw_mapvote_ratio" ))
            {
            if (IS_MAP_VALID(map_vote[i].name))
               {
               CHANGE_LEVEL(map_vote[i].name , NULL );
               /*
               char xxx[64];
               sprintf(xxx, "Map changed to %s with %.2f votes\n", , map_vote[i].ratio);
               UTIL_SayTextAll( xxx, UTIL_FindEntityByClassname ( NULL, "player" ));
               */
               }
            }
         }
      }
   return num_players;
}

void SendVoteMaps(CBasePlayer *pPlayer, int num_players)
{
   int k;
   if (!map_vote_count)
      return;
   if (!CVAR_GET_FLOAT( "gw_mapvote" ))
      {
      ClientPrint( pPlayer->pev, HUD_PRINTCENTER, "Map voting is disabled!");
      return;
      }

   num_players *= CVAR_GET_FLOAT( "gw_mapvote_ratio" );
	MESSAGE_BEGIN(MSG_ONE, gsmgVoteMaps, NULL, pPlayer->pev);
      WRITE_BYTE(num_players);
      WRITE_BYTE(map_vote_count);
      for (k = 0; k < map_vote_count; k++)
         {
         WRITE_STRING(map_vote[k].name);
         WRITE_BYTE(map_vote[k].votes);
         }
	MESSAGE_END();  
}