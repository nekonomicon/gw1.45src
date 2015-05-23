/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_c_stats.cpp
   This is the server side client statistics module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "player.h"
#include "spectator.h"
#include "client.h"
#include "soundent.h"
#include "gamerules.h"
#include "weapons.h"
#include "shake.h" 
#include <string.h>
#include "func_break.h"
#include "doors.h"
#include "sdk_u_main.h"
#include "sdk_c_global.h"
#include "sdk_c_stats.h"

extern int gmsgCLStat;

// Highest shooting percentage
void S1_Message_1(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   int i;
   float stat_accuracy = -1.0f, f;
   char *nn;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->stat.Hits > 0 && player->stat.Shots)
         {
         f = (float) player->stat.Hits / (float)player->stat.Shots;
         if (f > stat_accuracy)
            {
            splayer = player;
            stat_accuracy = f;
            }
         }
      }
   f = stat_accuracy * 100;
   i = f;
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SHOOTING_HIGH);
         WRITE_BYTE(splayer->entindex());
         WRITE_BYTE(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SHOOTING_HIGH);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}

// Lowest shooting percentage
void S1_Message_2(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i;
   float stat_accuracy = 2.0f, f;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      f = 0;
      if (player->stat.Hits > 0 && player->stat.Shots)
         {
         f = (float) player->stat.Hits / (float)player->stat.Shots;
         if (f < stat_accuracy)
            {
            splayer = player;
            stat_accuracy = f;
            }
         }
      }
   f = stat_accuracy * 100;
   i = f;
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SHOOTING_LOW);
         WRITE_BYTE(splayer->entindex());
         WRITE_BYTE(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SHOOTING_LOW);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Most shots
void S1_Message_3(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->stat.Shots > i)
         {
         splayer = player;
         i = player->stat.Shots;
         }
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_MOST_SHOTS);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_MOST_SHOTS);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Least shots
void S1_Message_4(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0xFFFF;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->stat.Shots < i)
         {
         splayer = player;
         i = player->stat.Shots;
         }
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_LEAST_SHOTS);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_LEAST_SHOTS);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest pistol kills
void S1_Message_5(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0, p = 0; 
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->stat.Kills[WEAPON_92D] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_92D]; p = WEAPON_92D;}
      if (player->stat.Kills[WEAPON_GLOCK18] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_GLOCK18]; p = WEAPON_GLOCK18;}
      if (player->stat.Kills[WEAPON_DEAGLE] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_DEAGLE]; p = WEAPON_DEAGLE;}
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_PISTOL_KILLS);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
         WRITE_BYTE(p);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_PISTOL_KILLS);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest shotgun kills
void S1_Message_6(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0, p = 0; 
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->stat.Kills[WEAPON_M3] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_M3]; p = WEAPON_M3;}
      if (player->stat.Kills[WEAPON_XM1014] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_XM1014]; p = WEAPON_XM1014;}
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SHOTGUN_KILLS);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
         WRITE_BYTE(p);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SHOTGUN_KILLS);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest sub kills
void S1_Message_7(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0, p = 0; 
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->stat.Kills[WEAPON_TMP] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_TMP]; p = WEAPON_TMP;}
      if (player->stat.Kills[WEAPON_MAC10] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_MAC10]; p = WEAPON_MAC10;}
      if (player->stat.Kills[WEAPON_UMP45] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_UMP45]; p = WEAPON_UMP45;}
      if (player->stat.Kills[WEAPON_MP5NAVY] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_MP5NAVY]; p = WEAPON_MP5NAVY;}
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SUB_KILLS);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
         WRITE_BYTE(p);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SUB_KILLS);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest rifle kills
void S1_Message_8(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0, p = 0; 
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->stat.Kills[WEAPON_AK47] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_AK47]; p = WEAPON_AK47;}
      if (player->stat.Kills[WEAPON_SG552] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_SG552]; p = WEAPON_SG552;}
      if (player->stat.Kills[WEAPON_M16] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_M16]; p = WEAPON_M16;}
      if (player->stat.Kills[WEAPON_AUG] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_AUG]; p = WEAPON_AUG;}
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_RIFLE_KILLS);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
         WRITE_BYTE(p);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_RIFLE_KILLS);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest precision rifle kills   
void S1_Message_9(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0, p = 0; 
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->stat.Kills[WEAPON_SG550] > i)
         { splayer = player; i = player->stat.Kills[WEAPON_SG550]; p = WEAPON_SG550;}
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_PRECISION_KILLS);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
         WRITE_BYTE(p);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_PRECISION_KILLS);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}

// Highest specialty weapon kills
void S1_Message_10(CBasePlayer* me)
{
}

// Highest kills
void S1_Message_11(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0; 
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->pev->frags > i)
         {
         i = player->pev->frags;
         splayer = player;
         }
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_KILLS);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_KILLS);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest kill ratio
void S1_Message_12(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   float f, k, l = 0;
   int i;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      k = player->pev->frags;
      f = player->m_iDeaths;
      if (k > 0 && f > 0)
         {        
         k /= f;
         if (k > l)
            {
            l = k;
            splayer = player;
            }
         }
      }
   l *= 100;
   i = l;
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_KILL_RATIO);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_KILL_RATIO);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest round kills
void S1_Message_13(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (player->stat.MaxRoundKills > i)
         {
         splayer = player;
         i = player->stat.MaxRoundKills;
         }
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_ROUND_KILLS);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_ROUND_KILLS);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest life expectancy
void S1_Message_14(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   float f, k, l = 0;
   int i;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      k = player->stat.Rounds;
      if (k > 0)
         {
         f = player->stat.MaxRoundLives;
         f /= k;
         if (f > l)
            {
            l = f;
            splayer = player;
            }
         }
      }
   l *= 100;
   i = l;
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_LIFE_EXPECTANCY);
         WRITE_BYTE(splayer->entindex());
         WRITE_BYTE(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_LIFE_EXPECTANCY);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest suicides
void S1_Message_15(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (i < player->stat.Suicides)
         {
         i = player->stat.Suicides;
         splayer = player;
         }
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SUICIDES);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_SUICIDES);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Highest body count per minute
void S1_Message_16(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   float f, k, l = 0;
   int i;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      k = player->pev->frags;
      player->stat.TimeOn = gpGlobals->time - player->stat.TimeOn;
      f = player->stat.TimeOn/60.0f;
      if (k > 0  && f >= 1)
         {        
         k /= f;
         if (k > l)
            {
            l = k;
            splayer = player;
            }
         }
      }
   l *= 100;
   i = l;
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_BODY_COUNT);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_BODY_COUNT);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Best offensive record
void S1_Message_17(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (i < player->stat.Offense)
         {
         i = player->stat.Offense;
         splayer = player;
         }
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_OFFENSE);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_OFFENSE);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}
// Worst defensive record
void S1_Message_18(CBasePlayer* me)
{
   CBaseEntity *pPlayer = NULL;
 	CBasePlayer *player, *splayer = NULL;  
   char *nn;
   int i = 0;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict())) continue;
		if (!pPlayer->IsPlayer()) continue;
      if (pPlayer->pev->flags & FL_DORMANT) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn)
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (!nn[0])
         { pPlayer->pev->flags = FL_DORMANT; continue;}
      if (i < player->stat.Defense)
         {
         i = player->stat.Defense;
         splayer = player;
         }
      }
   if (splayer)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_DEFENSE);
         WRITE_BYTE(splayer->entindex());
         WRITE_SHORT(i);
      MESSAGE_END();
      }
   else
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLStat, NULL, me->edict());
         WRITE_BYTE(MSG_STAT_DEFENSE);
         WRITE_BYTE(0xFF);
      MESSAGE_END();
      }
}

int GetBestScoreRank(CBasePlayer *me)
{
   int i;
   for (i = 0; i < 10; i++)
      {
      // FIXME x:
      //if (me->m_iWONID == map_stat.bs[i].wonid)
         //return i + 1;
      if (!strcmp(me->m_iSteamID, map_stat.bs[i].SteamID))
         return i + 1;
      }
   return -1;
}

void FitBestScore(CBasePlayer *me)
{
   int i, j, r;
   if (me->m_iJuice <= 0)
      return;
   if (me->m_iJuice < map_stat.bs[9].juice)
      return;
   for (i = 0; i < 10; i++)
      {
      // FIXME x:
      if (/*map_stat.bs[i].wonid == me->m_iWONID ||*/
         !strcmp(map_stat.bs[i].SteamID, me->m_iSteamID))
         {
         if (map_stat.bs[i].juice <= me->m_iJuice)
            {
            strcpy(map_stat.bs[i].name, (char *)STRING(me->pev->netname));
            map_stat.bs[i].juice = me->m_iJuice;
            time ( &map_stat.bs[i].rawtime );
            map_stat.bs[i].wonid = me->m_iWONID;
            strcpy(map_stat.bs[i].SteamID, me->m_iSteamID);
            }
         return;
         }    
      if (me->m_iJuice > map_stat.bs[i].juice)
         {
         r = 9;
         for (j = 9; j > i; j--)
            // FIXME x:
            if (/*map_stat.bs[j].wonid == me->m_iWONID ||*/
               !strcmp(map_stat.bs[j].SteamID, me->m_iSteamID))

               {r = j; break;}
         for (j = r; j > i; j--)
            map_stat.bs[j] = map_stat.bs[j-1];
         strcpy(map_stat.bs[i].name, (char *)STRING(me->pev->netname));
         map_stat.bs[i].juice = me->m_iJuice;
         time ( &map_stat.bs[i].rawtime );
         map_stat.bs[i].wonid = me->m_iWONID;
         strcpy(map_stat.bs[i].SteamID, me->m_iSteamID);
         return;
         }
      }
}

void LoadBestScore()
{
   FILE *fp;
   int i;
   fp = fopen("gangwars/topscores.dat", "rb");
   if (!fp) return;
   for (i = 0; i < 10; i++)
      fread(&map_stat.bs[i], sizeof(best_score_t), 1, fp);
   fclose(fp);
}

void SaveBestScore()
{
   FILE *fp;
   int i;
   fp = fopen("gangwars/topscores.dat", "wb");
   if (!fp) return;
   for (i = 0; i < 10; i++)
      fwrite(&map_stat.bs[i], sizeof(best_score_t), 1, fp);
   fclose(fp);
}

