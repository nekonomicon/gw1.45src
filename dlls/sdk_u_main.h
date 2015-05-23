/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_u_main.h 
   This is the team definition header.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#ifndef UTIL_MAIN_H
#define UTIL_MAIN_H


   #define TEAM_MAX           4
   #define TEAM_NONE          0
   #define TEAM_EAST          1
   #define TEAM_WEST          2

   #define EAST_WINS_MONEY       1100 //2400
   #define EAST_WINS_POINTS      1
   #define EAST_WINNER_MONEY     0 //800
   #define EAST_WINNER_POINTS    1
   #define EAST_LOSES_MONEY      900 //1400
   #define EAST_LOSES_POINTS     0
   #define EAST_OBJECTIVE_MONEY  0 //2900
   #define EAST_OBJECTIVE_POINTS 1
   #define EAST_DRAW_MONEY       500 //11100
   #define EAST_DRAW_POINTS      0

   #define WEST_WINS_MONEY       1100 //2400
   #define WEST_WINS_POINTS      1
   #define WEST_WINNER_MONEY     0 //800
   #define WEST_WINNER_POINTS    1
   #define WEST_LOSES_MONEY      900 //1400
   #define WEST_LOSES_POINTS     0
   #define WEST_OBJECTIVE_MONEY  0 //2900
   #define WEST_OBJECTIVE_POINTS 1
   #define WEST_DRAW_MONEY       500 //11100
   #define WEST_DRAW_POINTS      0

   struct team_data
      {
      char name[64];
      int money;
      int members;
      int wins;
      int loses;
      int draws;
      int alive;
      CBaseEntity *last_spawn;
      CBasePlayer *winner;
      CBasePlayer *candidate;

      int win_money;
      int win_points;
      int winner_money;
      int winner_points;
      int lose_money;
      int lose_points;
      int objective_money;
      int objective_points;
      int draw_money;
      int draw_points;
      };

   extern struct team_data team[TEAM_MAX];

   CBasePlayerItem *GetWeaponByWeight(int weight);
   void GiveTeam(int team_index, int money, int points, int winner_money, int winner_points);
   void GiveDeadTeam(int team_index, int money, int points);
   CBasePlayer *GetClientWithWonID(unsigned int WonID);
   CBasePlayer *GetClientWithSteamID(char *SteamID);
   void SendBossStatus(int boss, CBasePlayer *pPlayer);

#endif
