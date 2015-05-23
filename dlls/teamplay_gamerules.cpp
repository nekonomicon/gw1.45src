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
#include	"teamplay_gamerules.h"
#include	"game.h"
#include	"sdk_e_menu.h"
#include	"sdk_u_main.h"
#include	"sdk_c_global.h"
#include	"sdk_w_armory.h"
#include	"shake.h"

extern DLL_GLOBAL BOOL		g_fGameOver;
#define STR_ARMORY_CLOSED "^_ ^n The armory is closed"



void SendTitle(char *print_string, CBasePlayer *me);
int IsNearArmory (CBasePlayer* pPlayer);
int CountPlayerAmmo(CBasePlayer* pPlayer, char *xweapon);
void UpdateTeamMembers();
void SDK_ChangeTeam(CBasePlayer* pPlayer);
void RestartRound();
void SendClientsInfo(CBasePlayer* me, int team_index);
void SendStats(CBasePlayer* me);
void SendAdvert(CBasePlayer* me);
void SwitchPlayerTeam(CBasePlayer *pPlayer, int team);
void SwitchPlayerGroup(CBasePlayer *pPlayer, int group);
void ComputeJuice(CBasePlayer *current);
void CopyToBodyQue(entvars_t *pev) ;
void FreshRound();
void ListPlayers (CBasePlayer *current);

extern int gmsgMoney;
extern int advert_count, gmsgCLSpawn;
extern cvar_t timeleft, fragsleft;

CHalfLifeTeamplay::CHalfLifeTeamplay()
{
	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;
}


void CHalfLifeTeamplay::Think ( void )
{
	if ( g_fGameOver )
	   {
		CHalfLifeMultiplay::Think();
		return;
	   }
	float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60;
	float time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);
	if (flTimeLimit != 0 && gpGlobals->time >= flTimeLimit)
	   {
		GoToIntermission();
		return;
	   }
   if (map_stat.g_fRoundEndTime && map_stat.g_fRoundEndTime < gpGlobals->time)
      {
      CheckRoundEnd();
      map_stat.g_fRoundEndTime = 0;
      }
   if (map_stat.m_fRestartRoundTime && map_stat.m_fRestartRoundTime < gpGlobals->time)
      {
      map_stat.m_fRestartRoundTime = 0;
      RestartRound();
      }
}

//=========================================================
// ClientCommand
// the user has typed a command which is unrecognized by everything else;
// this check to see if the gamerules knows anything about the command
//=========================================================
bool CHalfLifeTeamplay::GWClientCommand(CBasePlayer *pPlayer, const char *pcmd)
{
   char *rcon_password;
   rcon_password = (char*)CVAR_GET_STRING( "gw_pass0" );
   //char *rcon_password = (char *)CVAR_GET_STRING("rcon_password");
   
   if (rcon_password && rcon_password[0])
      {
      if (!strcmp(pcmd, rcon_password))
         {
         ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, admin_reject_message);
         //pPlayer->m_iAdmin = 1;
         //ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "Authenticated\nLevel 0\n");
         return TRUE;
         }
      }   
   
   rcon_password = (char*)CVAR_GET_STRING( "gw_pass1" );
   if (rcon_password && rcon_password[0])
      {
      if (!strcmp(pcmd, rcon_password))
         {
         ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, admin_reject_message);
         //pPlayer->m_iAdmin = 2;
         //ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "Authenticated\nLevel 1\n");
         return TRUE;
         }
      }   

   rcon_password = (char*)CVAR_GET_STRING( "gw_pass2" );
   if (rcon_password && rcon_password[0])
      {
      if (!strcmp(pcmd, rcon_password))
         {
         ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, admin_reject_message);
         //pPlayer->m_iAdmin = 3;
         //ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "Authenticated\nLevel 2\n");
         return TRUE;
         }
      }   
   
   rcon_password = (char*)CVAR_GET_STRING( "gw_pass3" );
   if (rcon_password && rcon_password[0])
      {
      if (!strcmp(pcmd, rcon_password))
         {
         ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, admin_reject_message);
         //pPlayer->m_iAdmin = 4;
         //ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "Authenticated\nLevel 3\n");
         return TRUE;
         }
      }   

   /*
   if (!strcmp(CMD_ARGV(0), "gw_admins"))
      {
      char xxx[256];
      int i;
      sprintf(xxx, "Registered admins: %i\n", num_admins);
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, xxx);
      for (i = 0; i < num_admins; i++)
         {
         sprintf(xxx, "WONID: %u PASS: %s LEVEL: %i\n", adb[i].WONID, adb[i].password, adb[i].level);
         ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, xxx);
         }
      return TRUE;
      }
   */

   if (!strcmp(CMD_ARGV(0), "gw_pass"))
      {
      char temp_pass[256];
      char xxx[256];
		if (CMD_ARGC() < 2) return TRUE;
      strcpy(temp_pass, CMD_ARGV(1));
      temp_pass[32] = 0;
      pPlayer->m_iWONID = g_engfuncs.pfnGetPlayerWONId(pPlayer->edict());
      strcpy(pPlayer->m_iSteamID, g_engfuncs.pfnGetPlayerAuthId(pPlayer->edict()));
      for (int i = 0; i < num_admins; i++)
         {
         // FIXME x:
         if (/*pPlayer->m_iWONID == adb[i].WONID*/
            !strcmp(pPlayer->m_iSteamID, adb[i].SteamID))
            {
            if (!strcmp(temp_pass, adb[i].password))
               {
               pPlayer->m_iAdmin = adb[i].level;
               sprintf(xxx, "Authenticated\nLevel %i\n", pPlayer->m_iAdmin);
               ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, xxx);
               return true;
               }
            }
         }
      //sprintf(xxx, "Failed authentication for WONID: %u with password %s\n", pPlayer->m_iWONID, temp_pass);
      sprintf(xxx, "Failed authentication for SteamID: %s with password %s\n", pPlayer->m_iSteamID, temp_pass);
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, xxx);
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, admin_reject_message);
      return true;
      }
   return false;
}

void UTIL_ScreenFade( CBaseEntity *pEntity, const Vector &color, float fadeTime, float fadeHold, int alpha, int flags );
BOOL CHalfLifeTeamplay :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
   // !!FIXME!!
   /*
   if (!strcmp(pcmd, "copybody"))
      {
      CopyToBodyQue(pPlayer->pev);
      return TRUE;
      }
   */
   /*
   if (!strcmp(pcmd, "punch"))
      {
      float flDamage = 10;
      UTIL_ScreenFade( pPlayer, Vector(255,0,0), .2, 0, 64, FFADE_IN | FFADE_MODULATE);
	   pPlayer->pev->punchangle.x = RANDOM_FLOAT(-flDamage, flDamage);
	   pPlayer->pev->punchangle.y = RANDOM_FLOAT(-flDamage, flDamage);
      return TRUE;
      }
   */

   if (!strcmp(pcmd, "listplayers"))
      {
      ListPlayers(pPlayer);
      return TRUE;
      }

   if (!strcmp(pcmd, "chooseteam"))
      {
      DisplayTeamMenu(pPlayer);
      return TRUE;
      }

   if (!strcmp(pcmd, "timeleft"))
      {
      char output[128];
      int minutes;
      int seconds;
	   float time_remaining, flTimeLimit;
      flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60;	   
      time_remaining = map_stat.g_fRoundEndTime - gpGlobals->time;
      seconds = time_remaining;
      seconds = seconds % 60;
      minutes = time_remaining - seconds;
      minutes /= 60;
      sprintf (output, "Timeleft on round %i is %i:%02i\n", 
               map_stat.g_iRounds, minutes, seconds);
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, output);
	   time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);
      if (time_remaining)
         {         
         seconds = time_remaining;
         seconds = seconds % 60;
         minutes = time_remaining - seconds;
         minutes /= 60;
         sprintf (output, "Timeleft on '%s' is %i:%02i\n", 
                 (char *)STRING(gpGlobals->mapname), 
                 minutes, seconds);
         ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, output);
         if (aw.nextmap[0])
            {
            sprintf (output, "Next map is %s\n", aw.nextmap);
            ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, output);
            }
         ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "\n");
         }
      else
         ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "*** This map has no time limit ***\n");
      return TRUE;
      }

   if (!strcmp(pcmd, "pos"))
      {
      char output[128];
      sprintf (output, "x:%.03f y:%.03f z:%.03f\n", pPlayer->pev->origin[0], pPlayer->pev->origin[1], pPlayer->pev->origin[2]);
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, output);
      return TRUE;
      }

   if (!strcmp(pcmd, "SteamID"))
      {
      char output[128];
      sprintf (output, "Your Steam ID = %s\n", pPlayer->m_iSteamID);
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, output);
      return TRUE;
      }

   if (!strcmp(pcmd, "gw_edicts"))
      {
      char xxx[256];
      sprintf(xxx, "Edicts %i\n", NUMBER_OF_ENTITIES());
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, xxx);
      return TRUE;
      }

   if (!strcmp(pcmd, "stl"))
      {
      pPlayer->m_iStealth = 1;
      return TRUE;
      }

   if (!strcmp(pcmd, "CLsHealth"))
      {
      SendClientsInfo(pPlayer, pPlayer->m_iTeam);
      return TRUE;
      }

   if (!strcmp(pcmd, "juice"))
      {
      char output[128];
      sprintf (output, "Your current juice level is: %i\n", pPlayer->m_iJuice);
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, output);
      return TRUE;
      }

   /* FIXME */
   /* WARNING - THIS IS A DEBUG CHEAT - REMOVE!!!!*/
   /*
   if (!strcmp(pcmd, "addjuice"))
      {
      pPlayer->m_iJuice += 10;
      char output[128];
      sprintf (output, "Your WON id: %i\n", pPlayer->m_iWONID);
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, output);
      return TRUE;
      }
   */

   if (!strcmp(pcmd, "top10"))
      {
      struct tm * timeinfo;
      char output[256];
      char sdate[256];
      int i;
      sprintf (output, "Top scores on this server are:\n");
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "------------------------------\n");
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "DATE | SteamID | NAME | SCORE \n");
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "------------------------------\n");
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, output);
      for (i = 0; i < 10; i++)
         {
         if (map_stat.bs[i].name[0])
            {
            timeinfo = localtime ( &map_stat.bs[i].rawtime );
            strcpy(sdate, asctime(timeinfo));
            sdate[24] = 0;
            sprintf(output, "%s - %s - %s - %d\n", sdate,  map_stat.bs[i].SteamID, map_stat.bs[i].name, map_stat.bs[i].juice);
            }
         else
            {
            break;
            //sprintf(output, "%s - %6i - %s\n", "free", map_stat.bs[i].juice, "none");
            }

         ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, output);
         }
      ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "----------------------------\n");
      return TRUE;
      }

   if (!strcmp(pcmd, "s0"))
      {
      if (pPlayer->stat.s0_lasttime < gpGlobals->time)
         {
         SendAdvert(pPlayer);
         pPlayer->stat.s0_index++;
         if (pPlayer->stat.s0_index >= advert_count)
            pPlayer->stat.s0_index = 0;
         pPlayer->stat.s0_lasttime = gpGlobals->time + 5;
         }
      return TRUE;
      }

   if (!strcmp(pcmd, "s1"))
      {
      /*
      if (pPlayer->stat.s1_lasttime < gpGlobals->time)
         {
         pPlayer->stat.s1_index++;
         SendStats(pPlayer);
         pPlayer->stat.s0_lasttime = gpGlobals->time + 5;
         }
      */
      return TRUE;
      }

   if (pPlayer->pev->deadflag == DEAD_NO)
      {          
      if (!strcmp(pcmd, "buy"))
         {
         if (IsNearArmory (pPlayer))
            {
            if (map_stat.g_fRoundArmoryCloseTime < gpGlobals->time && !pPlayer->m_iNANA)
               SendTitle(STR_ARMORY_CLOSED, pPlayer);
            else DisplayBuyWeaponMenu(pPlayer, pPlayer->m_iTeam);
            }
         return TRUE;
         }
      if (!strcmp(pcmd, "buyammo1"))
         {
         if (IsNearArmory (pPlayer))
            {
            if (map_stat.g_fRoundArmoryCloseTime < gpGlobals->time && !pPlayer->m_iNANA && !pPlayer->m_iKilled)
               SendTitle(STR_ARMORY_CLOSED, pPlayer);
            else BuyPrimaryAmmo(pPlayer);
            }
         return TRUE;
         }
      if (!strcmp(pcmd, "buyammo2"))
         {
         if (IsNearArmory (pPlayer))
            {
            if (map_stat.g_fRoundArmoryCloseTime < gpGlobals->time && !pPlayer->m_iNANA && !pPlayer->m_iKilled)
               SendTitle(STR_ARMORY_CLOSED, pPlayer);
            else BuySecondaryAmmo(pPlayer);
            }
         return TRUE;
         }      
      if (!strcmp(pcmd, "shout" ) )
         {
         if (CMD_ARGC() > 1) 
            {
            pPlayer->Speak(atoi(CMD_ARGV(1)));
            }
         return TRUE;
         }
      if (!strcmp(pcmd, "holler1" ) )
         {
         DisplayAudioMenu1(pPlayer, pPlayer->m_iTeam);
         return TRUE;
         }
      if (!strcmp(pcmd, "holler2" ) )
         {
         DisplayAudioMenu2(pPlayer, pPlayer->m_iTeam);
         return TRUE;
         }
      }


   if ( FStrEq( pcmd, "menuselect" ) )
	   {
		if ( CMD_ARGC() < 2 )
			return TRUE;

		int slot = atoi( CMD_ARGV(1) );
      switch (pPlayer->m_iPlayerStatus)
         {
         case STATUS_PLAYER_CHOOSETEAM:
            pPlayer->m_iPlayerStatus = 0;
            SwitchPlayerTeam(pPlayer, slot);
            break;
         case STATUS_PLAYER_CHOOSEGROUP:           
            if (slot < 1 || slot > 4)
               break;
            SwitchPlayerGroup(pPlayer, slot);
            break;
         case STATUS_PLAYER_BUYWEAPONMENU:
            pPlayer->m_iPlayerStatus = 0;
            if (map_stat.g_fRoundArmoryCloseTime < gpGlobals->time && !pPlayer->m_iNANA)
               SendTitle(STR_ARMORY_CLOSED, pPlayer);
            else if (IsNearArmory (pPlayer) &&
               pPlayer->pev->deadflag == DEAD_NO)
               {
               switch (slot)
                  {
                  case 1:            
                     DisplayBuyPistolMenu(pPlayer, pPlayer->m_iTeam);
                     break;
                  case 2:
                     DisplayBuyShotgunMenu(pPlayer, pPlayer->m_iTeam);
                     break;
                  case 3:
                     DisplayBuySubMenu(pPlayer, pPlayer->m_iTeam);
                     break;
                  case 4:
                     DisplayBuyAutoMenu(pPlayer, pPlayer->m_iTeam);
                     break;
                  }
               }
            break;
         case STATUS_PLAYER_BUYPISTOLMENU:
            pPlayer->m_iPlayerStatus = 0;
            if (map_stat.g_fRoundArmoryCloseTime < gpGlobals->time && !pPlayer->m_iNANA)
               SendTitle(STR_ARMORY_CLOSED, pPlayer);
            else if (IsNearArmory (pPlayer) &&
               pPlayer->pev->deadflag == DEAD_NO)
               {
               switch (slot)
                  {
                  case 1:
                     if (aw.pistols & 1)
                        BuyWeapon(pPlayer, WEAPON_GLOCK18, w_stat[WEAPON_GLOCK18].price, "weapon_b93r", 0);
                     break;
                  case 2:
                     if (aw.pistols & 2)
                        BuyWeapon(pPlayer, WEAPON_GLOCK18, w_stat[WEAPON_92D].price, "weapon_92d", 0);
                     break;
                  case 3:            
                     if (aw.pistols & 4)
                        BuyWeapon(pPlayer, WEAPON_DEAGLE, w_stat[WEAPON_DEAGLE].price, "weapon_1911", 0);
                     break;
                  }
               }
            break;
         case STATUS_PLAYER_BUYSHOTGUNMENU:
            pPlayer->m_iPlayerStatus = 0;
            if (map_stat.g_fRoundArmoryCloseTime < gpGlobals->time && !pPlayer->m_iNANA)
               SendTitle(STR_ARMORY_CLOSED, pPlayer);
            else if (IsNearArmory (pPlayer) &&
               pPlayer->pev->deadflag == DEAD_NO)
               {
               switch (slot)
                  {
                  case 1:            
                     if (aw.shotguns & 1)
                        BuyWeapon(pPlayer, WEAPON_M3, w_stat[WEAPON_M3].price, "weapon_m3", 1);
                     break;
                  case 2:            
                     if (aw.shotguns & 2)
                        BuyWeapon(pPlayer, WEAPON_XM1014, w_stat[WEAPON_XM1014].price, "weapon_xm1014", 1);
                     break;
                  }
               }
            break;
         case STATUS_PLAYER_BUYSUBMENU:
            pPlayer->m_iPlayerStatus = 0;
            if (map_stat.g_fRoundArmoryCloseTime < gpGlobals->time && !pPlayer->m_iNANA)
               SendTitle(STR_ARMORY_CLOSED, pPlayer);
            else if (IsNearArmory (pPlayer) &&
               pPlayer->pev->deadflag == DEAD_NO)
               {
               switch (slot)
                  {
                  case 1:            
                     if (aw.subs & 1)
                        BuyWeapon(pPlayer, WEAPON_MAC10, w_stat[WEAPON_MAC10].price, "weapon_mac10", 1);
                     break;
                  case 2:            
                     if (aw.subs & 2)
                        BuyWeapon(pPlayer, WEAPON_TMP, w_stat[WEAPON_TMP].price, "weapon_tmp", 1);
                     break;
                  case 3:            
                     if (aw.subs & 4)                        
                        BuyWeapon(pPlayer, WEAPON_UMP45, w_stat[WEAPON_UMP45].price, "weapon_ump45", 1);
                     break;
                  case 4:            
                     if (aw.subs & 8)
                        BuyWeapon(pPlayer, WEAPON_MP5NAVY, w_stat[WEAPON_MP5NAVY].price, "weapon_mp5navy", 1);
                     break;
                  }
               }
            break;
         case STATUS_PLAYER_BUYAUTOMENU:
            pPlayer->m_iPlayerStatus = 0;
            if (map_stat.g_fRoundArmoryCloseTime < gpGlobals->time && !pPlayer->m_iNANA)
               SendTitle(STR_ARMORY_CLOSED, pPlayer);
            else if (IsNearArmory (pPlayer) &&
               pPlayer->pev->deadflag == DEAD_NO)
               {
               switch (slot)
                  {
                  case 1:            
                     if (aw.autos & 1)
                        BuyWeapon(pPlayer, WEAPON_AK47, w_stat[WEAPON_AK47].price, "weapon_ak47", 1);
                     break;
                  case 2:            
                     if (aw.autos & 2)
                        BuyWeapon(pPlayer, WEAPON_M16, w_stat[WEAPON_M16].price, "weapon_m16", 1);
                     break;
                  case 3:            
                     if (aw.autos & 4)
                        BuyWeapon(pPlayer, WEAPON_SG552, w_stat[WEAPON_SG552].price, "weapon_sg552", 1);
                     break;
                  case 4:            
                     if (aw.autos & 8)
                        BuyWeapon(pPlayer, WEAPON_AUG, w_stat[WEAPON_AUG].price, "weapon_aug", 1);
                     break;
                  case 5:            
                     if (aw.autos & 16)
                        BuyWeapon(pPlayer, WEAPON_SG550, w_stat[WEAPON_SG550].price, "weapon_sg550", 1);
                     break;
                  }
               }
            break;
         case STATUS_PLAYER_AUDIOMENU1:
            pPlayer->m_iPlayerStatus = 0;
            if (pPlayer->pev->deadflag == DEAD_NO)
               {
               switch (slot)
                  {
                  case 1:
                     pPlayer->Speak(0);
                     break;
                  case 2:            
                     pPlayer->Speak(1);
                     break;
                  case 3:            
                     pPlayer->Speak(2);
                     break;
                  case 4:            
                     pPlayer->Speak(3);
                     break;
                  case 5:
                     pPlayer->Speak(4);
                     break;
                  }
               }
            break;
         case STATUS_PLAYER_AUDIOMENU2:
            pPlayer->m_iPlayerStatus = 0;
            if (pPlayer->pev->deadflag == DEAD_NO)
               {
               switch (slot)
                  {
                  case 1:
                     pPlayer->Speak(5);
                     break;
                  case 2:            
                     pPlayer->Speak(6);
                     break;
                  case 3:            
                     pPlayer->Speak(7);
                     break;
                  case 4:            
                     pPlayer->Speak(8);
                     break;
                  case 5:            
                     pPlayer->Speak(9);
                     break;
                  case 6:            
                     pPlayer->Speak(10);
                     break;
                  }
               }
            break;
         }
		return TRUE;
	   }

	return GWClientCommand(pPlayer, pcmd);
}

extern int gmsgGameMode;
extern int gmsgSayText;
extern int gmsgTeamInfo;


void CHalfLifeTeamplay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 1 );  // game mode teamplay
	MESSAGE_END();
}

//=========================================================
// InitHUD
//=========================================================
void CHalfLifeTeamplay::InitHUD( CBasePlayer *pPlayer )
{
	CHalfLifeMultiplay::InitHUD( pPlayer );
}

//=========================================================
// ClientUserInfoChanged
//=========================================================
void CHalfLifeTeamplay::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
   return;
}

extern int gmsgDeathMsg;

//=========================================================
// Deathnotice. 
//=========================================================
void CHalfLifeTeamplay::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	if ( m_DisableDeathMessages )
		return;
	CHalfLifeMultiplay::DeathNotice( pVictim, pKiller, pevInflictor );
}

//=========================================================
//=========================================================
void CHalfLifeTeamplay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
   CHalfLifeMultiplay::PlayerKilled( pVictim, pKiller, pInflictor );
}


//=========================================================
// IsTeamplay
//=========================================================
BOOL CHalfLifeTeamplay::IsTeamplay( void )
{
	return TRUE;
}

BOOL CHalfLifeTeamplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
   if (!pAttacker)
      return TRUE;
   if (!pAttacker->IsPlayer())
      return TRUE;
   if (pPlayer->m_iTeam != ((CBasePlayer *)pAttacker)->m_iTeam)
      return TRUE;
   if (pPlayer == pAttacker)
      return TRUE;
   return FALSE;
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 1;

   if (pAttacker->m_iTeam == pKilled->m_iTeam)
      return -1;

   return 1;
}

