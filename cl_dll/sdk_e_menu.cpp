/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_e_menu.cpp 
   This is the player selection menus (weapons, ammo) module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"parsemsg.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"game.h"
#include "sdk_u_main.h"
#include "sdk_e_menu.h"
#include "sdk_w_armory.h"

#define CANCEL_BIT      (1<<9)

enum 
{ 
	MAX_PLAYERS = 64,
	//MAX_TEAMS = 64,
	MAX_TEAM_NAME = 32   ,
};

scroller_t s_top, s_bottom;
avail_weapons_t aw;
 
struct extra_player_info_t 
{
	short frags;
	short deaths;
	short playerclass;
	short teamnumber;
   char  indexnumber;
   short health;
	char teamname[MAX_TEAM_NAME];
};

typedef struct
{
	char *name;
	short ping;
	byte thisplayer;  // TRUE if this is the calling player

  // stuff that's unused at the moment,  but should be done
	byte spectator;
	byte packetloss;

	char *model;
	short topcolor;
	short bottomcolor;

} hud_player_info_t;

extern extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];
extern hud_player_info_t	 g_PlayerInfoList[MAX_PLAYERS+1];

int TeamPlayers(int iTeam);

void DisplayTeamMenu(char *message, short *validbuttons, int team, int thugs1, int thugs2)
{
   if (thugs1 == 1 && thugs2 == 1)
      {
      sprintf(message, "^86* Select a gang:\n\n"
                       "^e 1: East Coast (%i thug)\n"
                       "^w 2: West Coast (%i thug)\n"
                       "^t 3: Team balance\n\n\n"
                       "5: Spectate\n",
                       thugs1, thugs2);
      }
   if (thugs1 != 1 && thugs2 == 1)
      {
      sprintf(message, "^86* Select a gang:\n\n"
                       "^e 1: East Coast (%i thugs)\n"
                       "^w 2: West Coast (%i thug)\n"
                       "^t 3: Team balance\n\n\n"
                       "5: Spectate\n",
                       thugs1, thugs2);
      }
   if (thugs1 == 1 && thugs2 != 1)
      {
      sprintf(message, "^86* Select a gang:\n\n"
                       "^e 1: East Coast (%i thug)\n"
                       "^w 2: West Coast (%i thugs)\n"
                       "^t 3: Team balance\n\n\n"
                       "5: Spectate\n",
                       thugs1, thugs2);
      }
   else
      {
      sprintf(message, "^86* Select a gang:\n\n"
                       "^e 1: East Coast (%i thug)\n"
                       "^w 2: West Coast (%i thug)\n"
                       "^t 3: Team balance\n\n\n"
                       "5: Spectate\n",
                       thugs1, thugs2);
      }
   *validbuttons = CANCEL_BIT | 7 | 16;
}

void DisplayGroupMenu(char *message, short *validbuttons, int team)
{
   if (team == TEAM_EAST)
      {
      sprintf(message, "^9** Select a group:\n\n"
                       "^e 1: Siahz\tWashington, DC\n"
                       "2: Nolia\tNew Orleans, LA\n"
                       "3: Latin Kings\tChicago, IL\n"
                       "4: Spades\tPhiladelphia, PA\n");
      *validbuttons = CANCEL_BIT | 15;
      }
   else
      {
      sprintf(message, "^**9 Select a group:\n\n"
                       "^w 1: Denver Lane\tLos Angeles, CA\n"
                       "2: Bloods\tLas Vegas, NV\n"
                       "3: 18th Street\tLos Angeles, CA\n"
                       "4: Golden Kings\tReno, NV\n");
      *validbuttons = CANCEL_BIT | 15;
      }
}

void DisplayBuyWeaponMenu(char *message, short *validbuttons, int team)
{
   sprintf(message, "^*9* Buy Weapon:\n\n"
                    "^t 1: Pistols\n"
                    "2: Shotguns\n"
                    "3: Sub Machine Guns\n"
                    "4: Auto Rifles\n");
   *validbuttons = CANCEL_BIT | 15;
}

void DisplayBuyPistolMenu(char *message, short *validbuttons, int team)
{
   char *sz = message;
   short b_action = aw.pistols;
   sprintf(sz, "^*9* Buy Pistol:\n\n");
   sz = message + strlen(message);
   if (b_action & 1) sprintf(sz, "^t 1: %s\t$%d\n", w_stat[WEAPON_GLOCK18].name, w_stat[WEAPON_GLOCK18].price);
   else sprintf(sz, "^f 1: %s\t$%d\n", w_stat[WEAPON_GLOCK18].name, w_stat[WEAPON_GLOCK18].price);
   sz = message + strlen(message);
   if (b_action & 2) sprintf(sz, "^t 2: %s\t$%d\n", w_stat[WEAPON_92D].name, w_stat[WEAPON_92D].price);
   else sprintf(sz, "^f 2: %s\t$%d\n", w_stat[WEAPON_92D].name, w_stat[WEAPON_92D].price);
   sz = message + strlen(message);
   if (b_action & 4) sprintf(sz, "^t 3: %s\t$%d\n", w_stat[WEAPON_DEAGLE].name, w_stat[WEAPON_DEAGLE].price);
   else sprintf(sz, "^f 3: %s\t$%d\n", w_stat[WEAPON_DEAGLE].name, w_stat[WEAPON_DEAGLE].price);
   *validbuttons = CANCEL_BIT | b_action;
}

void DisplayBuyShotgunMenu(char *message, short *validbuttons, int team)
{
   char *sz = message;
   short b_action = aw.shotguns;
   sprintf(sz, "^*9* Buy Shotgun:\n\n");
   sz = message + strlen(message);
   if (b_action & 1) sprintf(sz, "^t 1: %s\t$%d\n", w_stat[WEAPON_M3].name, w_stat[WEAPON_M3].price);
   else sprintf(sz, "^f 1: %s\t$%d\n", w_stat[WEAPON_M3].name, w_stat[WEAPON_M3].price);
   sz = message + strlen(message);
   if (b_action & 2) sprintf(sz, "^t 2: %s\t$%d\n", w_stat[WEAPON_XM1014].name, w_stat[WEAPON_XM1014].price);
   else sprintf(sz, "^f 2: %s\t$%d\n", w_stat[WEAPON_XM1014].name, w_stat[WEAPON_XM1014].price);
   *validbuttons = CANCEL_BIT | b_action;
}

void DisplayBuySubMenu(char *message, short *validbuttons, int team)
{
   char *sz = message;
   short b_action = aw.subs;
   sprintf(sz, "^*9* Buy Sub Machinegun:\n\n");
   sz = message + strlen(message);
   if (b_action & 1) sprintf(sz, "^t 1: %s\t$%d\n", w_stat[WEAPON_MAC10].name, w_stat[WEAPON_MAC10].price);
   else sprintf(sz, "^f 1: %s\t$%d\n", w_stat[WEAPON_MAC10].name, w_stat[WEAPON_MAC10].price);
   sz = message + strlen(message);
   if (b_action & 2) sprintf(sz, "^t 2: %s\t$%d\n", w_stat[WEAPON_TMP].name, w_stat[WEAPON_TMP].price);
   else sprintf(sz, "^f 2: %s\t$%d\n", w_stat[WEAPON_TMP].name, w_stat[WEAPON_TMP].price);
   sz = message + strlen(message);
   if (b_action & 4) sprintf(sz, "^t 3: %s\t$%d\n", w_stat[WEAPON_UMP45].name, w_stat[WEAPON_UMP45].price);
   else sprintf(sz, "^f 3: %s\t$%d\n", w_stat[WEAPON_UMP45].name, w_stat[WEAPON_UMP45].price);
   sz = message + strlen(message);
   if (b_action & 8) sprintf(sz, "^t 4: %s\t$%d\n", w_stat[WEAPON_MP5NAVY].name, w_stat[WEAPON_MP5NAVY].price);
   else sprintf(sz, "^f 4: %s\t$%d\n", w_stat[WEAPON_MP5NAVY].name, w_stat[WEAPON_MP5NAVY].price);
   *validbuttons = CANCEL_BIT | b_action;
}

void DisplayBuyAutoMenu(char *message, short *validbuttons, int team)
{
   char *sz = message;
   short b_action = aw.autos;
   sprintf(sz, "^*9* Buy Auto Rifle:\n\n");
   sz = message + strlen(message);
   if (b_action & 1) sprintf(sz, "^t 1: %s\t$%d\n", w_stat[WEAPON_AK47].name, w_stat[WEAPON_AK47].price);
   else sprintf(sz, "^f 1: %s\t$%d\n", w_stat[WEAPON_AK47].name, w_stat[WEAPON_AK47].price);
   sz = message + strlen(message);
   if (b_action & 2) sprintf(sz, "^t 2: %s\t$%d\n", w_stat[WEAPON_M16].name, w_stat[WEAPON_M16].price);
   else sprintf(sz, "^f 2: %s\t$%d\n", w_stat[WEAPON_M16].name, w_stat[WEAPON_M16].price);
   sz = message + strlen(message);
   if (b_action & 4) sprintf(sz, "^t 3: %s\t$%d\n", w_stat[WEAPON_SG552].name, w_stat[WEAPON_SG552].price);
   else sprintf(sz, "^f 3: %s\t$%d\n", w_stat[WEAPON_SG552].name, w_stat[WEAPON_SG552].price);
   sz = message + strlen(message);
   if (b_action & 8) sprintf(sz, "^t 4: %s\t$%d\n", w_stat[WEAPON_AUG].name, w_stat[WEAPON_AUG].price);
   else sprintf(sz, "^f 4: %s\t$%d\n", w_stat[WEAPON_AUG].name, w_stat[WEAPON_AUG].price);
   sz = message + strlen(message);
   if (b_action & 16) sprintf(sz, "^t 5: %s\t$%d\n", w_stat[WEAPON_SG550].name, w_stat[WEAPON_SG550].price);
   else sprintf(sz, "^f 5: %s\t$%d\n", w_stat[WEAPON_SG550].name, w_stat[WEAPON_SG550].price);
   *validbuttons = CANCEL_BIT | b_action;
}

void DisplayAudio1Menu(char *message, short *validbuttons, int team)
{
   char *sz = message;
   sprintf(message, "^*9* Holler:\n\n"
                    "^t 1: Aight!\n"
                    "2: Fuck Dat!\n"
                    "3: Check Dis Out!\n"
                    "4: Follow Me!\n"
                    "5: Stupid Puto!\n");
   *validbuttons = CANCEL_BIT | 31;
}

void DisplayAudio2Menu(char *message, short *validbuttons, int team)
{
   char *sz = message;
   sprintf(message, "^*9* Wassup:\n\n"
                    "^t 1: Wassup B?\n"
                    "2: Nuttin' Dawg!\n"
                    "3: Wazaaaa!\n"
                    "4: Wuzaaa!\n"
                    "5: Huzaaa!\n"
                    "6: Aaaaaa!\n");
   *validbuttons = CANCEL_BIT | 63;
}


int TeamPlayers(int iTeam)
{
   int i, count = 0;
   for (i = 0; i < MAX_PLAYERS; i++)
      {
      if (g_PlayerExtraInfo[i].teamnumber == iTeam)
			count++;
      }
   return count;
}

char *GetCurrentStat(int messagetype)
{
   static char xout[256];
   int clientIndex = READ_BYTE();
   int i, j;
   xout[0] = 0;
   if (clientIndex != 0xFF)
      {
      switch (messagetype)
         {
         case MSG_STAT_SHOOTING_HIGH:     // Highest shooting percentage
            i = READ_BYTE();
            sprintf(xout, "%s has the best shooting percentage of %i%%", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_SHOOTING_LOW:      // Lowest shooting percentage
            i = READ_BYTE();
            sprintf(xout, "%s has the worst shooting percentage of %i%%", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_MOST_SHOTS:        // Most shots
            i = READ_WORD();
            sprintf(xout, "%s has the most shots fired of %i rounds", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_LEAST_SHOTS:       // Least shots
            i = READ_WORD();
            sprintf(xout, "%s has the least shots fired of %i", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_PISTOL_KILLS:      // Highest pistol kills
            i = READ_WORD();
            j = READ_BYTE();
            sprintf(xout, "%s has the most pistol kills of %i with a %s", g_PlayerInfoList[clientIndex].name, i, w_stat[j].name);
            break;
         case MSG_STAT_SHOTGUN_KILLS:     // Highest shotgun kills
            i = READ_WORD();
            j = READ_BYTE();
            sprintf(xout, "%s has the most shotgun kills of %i with a %s", g_PlayerInfoList[clientIndex].name, i, w_stat[j].name);
            break;
         case MSG_STAT_SUB_KILLS:         // Highest sub kills
            i = READ_WORD();
            j = READ_BYTE();
            sprintf(xout, "%s has the most sub machinegun kills of %i with a %s", g_PlayerInfoList[clientIndex].name, i, w_stat[j].name);
            break;
         case MSG_STAT_RIFLE_KILLS:       // Highest rifle kills
            i = READ_WORD();
            j = READ_BYTE();
            sprintf(xout, "%s has the most rifle kills of %i with a %s", g_PlayerInfoList[clientIndex].name, i, w_stat[j].name);
            break;
         case MSG_STAT_PRECISION_KILLS:   // Highest precision rifle kills   
            i = READ_WORD();
            j = READ_BYTE();
            sprintf(xout, "%s has the most precision rifle kills of %i with a %s", g_PlayerInfoList[clientIndex].name, i, w_stat[j].name);
            break;
         case MSG_STAT_SPECIALTY_KILLS:   // Highest specialty weapon kills
            i = READ_WORD();
            j = READ_BYTE();
            sprintf(xout, "%s has the most specialty rifle kills of %i with a %s", g_PlayerInfoList[clientIndex].name, i, w_stat[j].name);
            break;
         case MSG_STAT_KILLS:             // Highest kills
            i = READ_WORD();
            sprintf(xout, "%s has the most kills of %i ", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_KILL_RATIO:        // Highest kill ratio
            i = READ_WORD();
            sprintf(xout, "%s has the highest kill to death percentage of %i%%", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_ROUND_KILLS:       // Highest round kills
            i = READ_WORD();
            sprintf(xout, "%s has the highest killes per round of %i", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_LIFE_EXPECTANCY:   // Highest life expectancy
            i = READ_BYTE();
            sprintf(xout, "%s has the highest life expectancy percentage of %i%%", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_SUICIDES:          // Highest suicides
            i = READ_WORD();
            sprintf(xout, "%s Has the most suicides with %i deaths", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_BODY_COUNT:        // Highest body count per minute
            i = READ_WORD();
            sprintf(xout, "%s has the highest kills per minute percentage of %i%%", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_OFFENSE:           // Best offensive record
            i = READ_WORD();
            sprintf(xout, "%s has the best offensive record with %i kills", g_PlayerInfoList[clientIndex].name, i);
            break;
         case MSG_STAT_DEFENSE:           // Worst defensive record
            i = READ_WORD();
            sprintf(xout, "%s has the worst offensive record of %i deaths", g_PlayerInfoList[clientIndex].name, i);
            break;
         default:
            sprintf(xout, "** %s **", READ_STRING());
            break;
         }
      }
   else
      {
      switch (messagetype)
         {
         case MSG_STAT_SHOOTING_HIGH:     // Highest shooting percentage
            strcpy(xout, "There are no available stats for best shooting percentage category");
            break;
         case MSG_STAT_SHOOTING_LOW:      // Lowest shooting percentage
            strcpy(xout, "There are no available stats for worst shooting percentage category");
            break;
         case MSG_STAT_MOST_SHOTS:        // Most shots
            strcpy(xout, "There are no available stats for most shots category");
            break;
         case MSG_STAT_LEAST_SHOTS:       // Least shots
            strcpy(xout, "There are no available stats for least shots category");
            break;
         case MSG_STAT_PISTOL_KILLS:      // Highest pistol kills
            strcpy(xout, "There are no available stats for most pistol kills category");
            break;
         case MSG_STAT_SHOTGUN_KILLS:     // Highest shotgun kills
            strcpy(xout, "There are no available stats for shotgun kills category");
            break;
         case MSG_STAT_SUB_KILLS:         // Highest sub kills
            strcpy(xout, "There are no available stats for sub machinegun kills category");
            break;
         case MSG_STAT_RIFLE_KILLS:       // Highest rifle kills
            strcpy(xout, "There are no available stats for rifle kills category");
            break;
         case MSG_STAT_PRECISION_KILLS:   // Highest precision rifle kills   
            strcpy(xout, "There are no available stats for precision rifle kills category");
            break;
         case MSG_STAT_SPECIALTY_KILLS:   // Highest specialty weapon kills
            strcpy(xout, "There are no available stats for specialty rifle kills category");
            break;
         case MSG_STAT_KILLS:             // Highest kills
            strcpy(xout, "There are no available stats for most kills category");
            break;
         case MSG_STAT_KILL_RATIO:        // Highest kill ratio
            strcpy(xout, "There are no available stats for best kill ratio category");
            break;
         case MSG_STAT_ROUND_KILLS:       // Highest round kills
            strcpy(xout, "There are no available stats for most round kills category");
            break;
         case MSG_STAT_LIFE_EXPECTANCY:   // Highest life expectancy
            strcpy(xout, "There are no available stats for best life expectency category");
            break;
         case MSG_STAT_SUICIDES:          // Highest suicides
            strcpy(xout, "There are no available stats for suicides category");
            break;
         case MSG_STAT_BODY_COUNT:        // Highest body count per minute
            strcpy(xout, "There are no available stats for best kill percentage category");
            break;
         case MSG_STAT_OFFENSE:           // Best offensive record
            strcpy(xout, "There are no available stats for best offense category");
            break;
         case MSG_STAT_DEFENSE:           // Worst defensive record
            strcpy(xout, "There are no available stats for worst defense category");
            break;
         default:
            sprintf(xout, "** %s **", READ_STRING());
            break;
         }
      }
   return xout;
}

