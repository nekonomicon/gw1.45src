/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_c_server.cpp
   This is the server utilites for clients module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"game.h"
#include	"items.h"
#include	"sdk_u_main.h"
#include	"sdk_c_global.h"
#include	"sdk_c_waitlist.h"
#include	"sdk_e_menu.h"

void SendTitle(char *, int);
void AccessPlayer (int idx, int value)
{
   int k;
	CBaseEntity* pPlayer = NULL;
	CBasePlayer* player;
   switch (idx)
      {
      case GW_SET_LICENSE:
         if (value < 0) value = 0;
         else if (value > 8) value = 8;
	      while ((pPlayer = UTIL_FindEntityByClassname (pPlayer, "player")) != NULL)
	         {
            if (FNullEnt(pPlayer->edict())) continue;
            if (!pPlayer->pev) continue;
		      if (!pPlayer->IsPlayer()) continue;	
            if (pPlayer->pev->flags & FL_DORMANT) continue;
            player = GetClassPtr ((CBasePlayer*)pPlayer->pev);
            player->m_iOldLicense = player->m_iLicense;
            player->m_iLicense = value;
            aw.baselicense = value;
	         }
         SendTitle("Upgrade selected. Players can upgrade freely!", 2);
         break;
      case GW_SMOKE_EAST:
	      while ((pPlayer = UTIL_FindEntityByClassname (pPlayer, "player")) != NULL)
	         {
            if (FNullEnt(pPlayer->edict())) continue;
            if (!pPlayer->pev) continue;
		      if (!pPlayer->IsPlayer()) continue;	
            if (pPlayer->pev->flags & FL_DORMANT) continue;
            player = GetClassPtr ((CBasePlayer*)pPlayer->pev);
            if (player->m_iTeam == TEAM_EAST)
               if (player->pev->deadflag == DEAD_NO)
                  CLIENT_COMMAND(player->edict(), "kill\n");
	         }
         SendTitle("EAST TEAM HAS BEEN SMOKED!", 2);
         break;
      case GW_SMOKE_WEST:
	      while ((pPlayer = UTIL_FindEntityByClassname (pPlayer, "player")) != NULL)
	         {
            if (FNullEnt(pPlayer->edict())) continue;
            if (!pPlayer->pev) continue;
		      if (!pPlayer->IsPlayer()) continue;	
            if (pPlayer->pev->flags & FL_DORMANT) continue;
            player = GetClassPtr ((CBasePlayer*)pPlayer->pev);
            if (player->m_iTeam == TEAM_WEST)
               if (player->pev->deadflag == DEAD_NO)
                  CLIENT_COMMAND(player->edict(), "kill\n");
	         }
         SendTitle("WEST TEAM HAS BEEN SMOKED!", 1);
         break;
      case GW_RESET_LICENSE:
	      while ((pPlayer = UTIL_FindEntityByClassname (pPlayer, "player")) != NULL)
	         {
            if (FNullEnt(pPlayer->edict())) continue;
            if (!pPlayer->pev) continue;
		      if (!pPlayer->IsPlayer()) continue;	
            if (pPlayer->pev->flags & FL_DORMANT) continue;
            player = GetClassPtr ((CBasePlayer*)pPlayer->pev);
            player->m_iLicense = player->m_iOldLicense;
            aw.baselicense = old_aw.baselicense;
	         }
         SendTitle("Upgrade reset. Restoring old player licenses.", 1);
         break;
      case GW_RESET_SCORES:
	      while ((pPlayer = UTIL_FindEntityByClassname (pPlayer, "player")) != NULL)
	         {
            if (FNullEnt(pPlayer->edict())) continue;
            if (!pPlayer->pev) continue;
		      if (!pPlayer->IsPlayer()) continue;	
            if (pPlayer->pev->flags & FL_DORMANT) continue;
            player = GetClassPtr ((CBasePlayer*)pPlayer->pev);
            memset(&player->stat, 0, sizeof(pstat_t));
            player->pev->frags = 0;
            player->m_iDeaths = 0;
	         }
         team[TEAM_EAST].wins = team[TEAM_WEST].wins = 0;
         map_stat.g_iWins[0] = map_stat.g_iWins[1] = map_stat.g_iShots = map_stat.g_iRounds = 
         map_stat.g_iKills = 0;
         SendTitle("All scores are reset", 2);
         break;
      case GW_SET_WEAPONS:
         aw.pistols = 15;
         aw.shotguns = 3;
         aw.subs = 31;
         aw.autos = 31;
         SendTitle("All weapons are available", 2);
         break;
      case GW_RESET_WEAPONS:
         aw.pistols = old_aw.pistols;
         aw.shotguns = old_aw.shotguns;
         aw.subs = old_aw.subs;
         aw.autos = old_aw.autos;
         SendTitle("Old weapon setting restored", 1);
         break;
      case GW_SET_DAMAGE:
         aw.damage = value;
         for (k = 0; k < MAX_WEAPONS; k++)
            {
            w_stat[k].damage1 *= aw.damage;
            w_stat[k].damage2 *= aw.damage;
            }
         SendTitle("New damage configuration loaded", 1);
         break;
      case GW_RESET_DAMAGE:
         aw.damage = old_aw.damage;
         for (k = 0; k < MAX_WEAPONS; k++)
            {
            w_stat[k].damage1 *= aw.damage;
            w_stat[k].damage2 *= aw.damage;
            }
         SendTitle("Old damage configuration restored", 1);
         break;
      default:
         break;
      }
}

void TagPlayer (CBasePlayer *current, int idx, int tagnumber, int write_on)
{
	int i = 0;
   char output[256] = {0};
	CBaseEntity* pPlayer = NULL;
	CBasePlayer* player;
	while ((pPlayer = UTIL_FindEntityByClassname (pPlayer, "player")) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;
		if (pPlayer->IsPlayer()	&& pPlayer->pev->flags != FL_DORMANT)
		   {
			player = GetClassPtr ((CBasePlayer*)pPlayer->pev);
         if (player->entindex() == idx)
            {
            if (tagnumber == TAG_KILL)
               {
               if (player->pev->deadflag == DEAD_NO)
                  player->Killed(player->pev, GIB_NEVER);
               }
            else
               {
               if (write_on)
                  {
                  player->m_iTagged |= tagnumber;
                  sprintf (output, "[%04i] : %s is tagged with %i\n", pPlayer->entindex(), STRING(player->pev->netname), tagnumber);
                  }
               else
                  {
                  player->m_iTagged &= ~tagnumber;
                  sprintf (output, "[%04i] : %s is untagged with %i\n", pPlayer->entindex(), STRING(player->pev->netname), tagnumber);
                  }
			      ClientPrint(current->pev, HUD_PRINTCONSOLE, output);
               }
            return;
            }
		   }
	   }
}

void ListPlayers (CBasePlayer *current)
{
	int i = 0;
   char output[120] = {0};
	CBaseEntity* pPlayer = NULL;
	CBasePlayer* player;
	while ((pPlayer = UTIL_FindEntityByClassname (pPlayer, "player")) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;
		if (pPlayer->IsPlayer()	&& pPlayer->pev->flags != FL_DORMANT)
		   {
			player = GetClassPtr ((CBasePlayer*)pPlayer->pev);
         sprintf (output, "[%04i] : %s\n", pPlayer->entindex(), STRING(player->pev->netname));
			ClientPrint(current->pev, HUD_PRINTCONSOLE, output);
		   }
	   }
}

void ComputeJuice(CBasePlayer *current)
{
   if (!current)
      return;
   current->m_iJuice = current->pev->frags * 3
                     - current->m_iDeaths * 1
                     + current->stat.Offense * 8
                     + current->stat.LastKills * 5
                     - current->stat.Defense * 6
                     + current->stat.MaxRoundKills * 20
                     + current->stat.MaxRoundLives * 2;
}