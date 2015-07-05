/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_u_main.cpp
   This is the main utility module.

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
#include	"sdk_e_menu.h"
#include "sdk_c_stats.h"
#include	"sdk_w_armory.h"

#define MONEY_CAP    12000
#define ARMORY_RANGE 256

#define TEAM_EAST_GROUP_MODEL_1  "east1"
#define TEAM_EAST_GROUP_MODEL_2  "east2"
#define TEAM_EAST_GROUP_MODEL_3  "east1"
#define TEAM_EAST_GROUP_MODEL_4  "east2"

#define TEAM_WEST_GROUP_MODEL_1  "west1"
#define TEAM_WEST_GROUP_MODEL_2  "west2"
#define TEAM_WEST_GROUP_MODEL_3  "west1"
#define TEAM_WEST_GROUP_MODEL_4  "west2"


extern int gmsgTitle;
extern int gmsgMoney;
extern int gmsgArmory;
extern int gmsgTime;
extern int gmsgTeamInfo;
extern int gmsgCLsHealth;
extern int gmsgNewTeam;
extern int gmsgGetTeams;
extern int gmsgCLAdvert;
extern int gmsgCLStat;
extern int gmsgCLID;
extern int gmsgNANA;


extern int gsmgPlayTheme;
struct team_data team[TEAM_MAX];
int IsNearArmory (CBasePlayer* pPlayer);
void FreshRound();

void UTIL_BlastSound(char *filename)
{
   GW_EMIT_SOUND(ENT(0), CHAN_STREAM, filename, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 93 + RANDOM_LONG(0,0xF));
   /*
   int i;
   char *nn;
   CBasePlayer *pPlayer;
   for (i = 1; i  <= gpGlobals->maxClients; i++)
      {
      pPlayer = (CBasePlayer *) UTIL_PlayerByIndex( i );
 		if (!pPlayer) continue;
 		if (FNullEnt(pPlayer->edict())) continue;
      nn = (char *)STRING(pPlayer->pev->netname);
      if (nn && nn[0])
         {
         GW_EMIT_SOUND(pPlayer->edict(), CHAN_STREAM, filename, RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 93 + RANDOM_LONG(0,0xF));
         return;
         }
      }
   */
}

void UTIL_PlayTheme(int theme)
{
   int i;
   char *nn;
   CBasePlayer *pPlayer;
   for (i = 1; i  <= gpGlobals->maxClients; i++)
      {
      pPlayer = (CBasePlayer *) UTIL_PlayerByIndex( i );
 		if (!pPlayer) continue;
 		if (FNullEnt(pPlayer->edict())) continue;
      nn = (char *)STRING(pPlayer->pev->netname);
      if (nn && nn[0])
         {
	      MESSAGE_BEGIN(MSG_ONE, gsmgPlayTheme, NULL, pPlayer->pev);
            WRITE_BYTE(theme);
	      MESSAGE_END();
         }
      }
}
void UpdateTeamMembers()
{
   char *nn;
   team[TEAM_NONE].members = team[TEAM_EAST].members 
                           = team[TEAM_WEST].members 
                           = 0;
   team[TEAM_NONE].alive = team[TEAM_EAST].alive 
                         = team[TEAM_WEST].alive 
                         = 0;

   CBaseEntity* pPlayer = NULL;
	CBasePlayer* player;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (!pPlayer->pev)
         continue;

      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      nn = (char *)STRING(pPlayer->pev->netname);
      if (!nn)
         {
         pPlayer->pev->flags = FL_DORMANT;
         continue;
         }

      if (!nn[0])
         {
         pPlayer->pev->flags = FL_DORMANT;
         continue;
         }

	   player = (CBasePlayer*)pPlayer;
      if (player->m_iTeam == TEAM_EAST)
         {
         team[TEAM_EAST].members++;
         if (player->pev->health > 0 && player->pev->deadflag == DEAD_NO)
            team[TEAM_EAST].alive++;
         }
      else if (player->m_iTeam == TEAM_WEST)
         {
         team[TEAM_WEST].members++;
         if (player->pev->health > 0 && player->pev->deadflag == DEAD_NO)
            team[TEAM_WEST].alive++;
         }
	   }  
}

BOOL IsSpawnPointValid( CBaseEntity *pPlayer, CBaseEntity *pSpot );
edict_t *EntSelectTeamSpawnPoint( CBaseEntity *pPlayer )
{
	CBaseEntity *pSpot, *loopSpot = NULL;
   CBasePlayer *pp = (CBasePlayer *) pPlayer;
	edict_t		*player;
	player = pPlayer->edict();

   switch (pp->m_iTeam)
      {
      case TEAM_EAST:
		   pSpot = team[TEAM_EAST].last_spawn;
         loopSpot = pSpot;
         while ((pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch")) != loopSpot)
            {
            if (!pSpot)
               continue;
            if (!pSpot->pev)
               continue;
				//if (!IsSpawnPointValid(pPlayer, pSpot))
               //continue;
            if (pSpot->pev->origin == Vector(0, 0, 0))
               continue;
            team[TEAM_EAST].last_spawn = pSpot;
            goto foundSpot;
				}
         break;
      default:
		   pSpot = team[TEAM_WEST].last_spawn;
         loopSpot = pSpot;
         while ((pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_start")) != loopSpot)
            {
            if (!pSpot)
               continue;
				//if (!IsSpawnPointValid(pPlayer, pSpot))
               //continue;
            if (!pSpot->pev)
               continue;
            if (pSpot->pev->origin == Vector( 0, 0, 0 ))
               continue;
            team[TEAM_WEST].last_spawn = pSpot;
            goto foundSpot;
				}
         break;
      }

	if (!pSpot)
	   {
		ALERT(at_error, "PutClientInServer: no info_player_start on level");
		return INDEXENT(0);
	   }

foundSpot:
	if (pSpot)
	   {
      return pSpot->edict();
      /*
		CBaseEntity *ent = NULL;
      if (pp->m_iTeam && (pp->pev->solid & SOLID_SLIDEBOX))
		   while ( (ent = UTIL_FindEntityInSphere( ent, pSpot->pev->origin, 32 )) != NULL )
		      {
			   if ( ent->IsPlayer() && !(ent->edict() == player) )
               ent->Killed(ent->pev, GIB_NEVER);
		      }
      char x[256];
      sprintf(x, "x: %i\n", pSpot->entindex());
      ClientPrint(((CBasePlayer *)pPlayer)->pev, HUD_PRINTCENTER, x);
      */
	   }
   return INDEXENT(0);
	
}


void BuyDropWeapon(CBasePlayer *player, int primary)
   {
   if (primary == 1)
      {
      player->DropPlayerItem("weapon_ak47");
      player->DropPlayerItem("weapon_m16");
      player->DropPlayerItem("weapon_sg552");
      player->DropPlayerItem("weapon_aug");

      player->DropPlayerItem("weapon_sg550");

      player->DropPlayerItem("weapon_m3");
      player->DropPlayerItem("weapon_xm1014");

      player->DropPlayerItem("weapon_mac10");
      player->DropPlayerItem("weapon_tmp");
      player->DropPlayerItem("weapon_mp5navy");
      player->DropPlayerItem("weapon_ump45");
      player->m_iMaxSpeed = 300;
      player->m_iPrimary = 0;
      }
   else if (primary == 0)
      {
      player->DropPlayerItem("weapon_1911");
      player->DropPlayerItem("weapon_b93r");
      player->DropPlayerItem("weapon_92d");
      //player->DropPlayerItem("weapon_");
      player->m_iSecondary = 0;
      }      
}

CBasePlayerItem *GetWeaponByWeight(CBasePlayer *pPlayer, int weight)
{
	CBasePlayerItem *pCheck;
	int i;
	pCheck = NULL;
  
	for (i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	   {
		pCheck = pPlayer->m_rgpPlayerItems[ i ];
		while ( pCheck )
		   {
         if (pCheck->iWeight() == weight)
            return pCheck;
			pCheck = pCheck->m_pNext;
			}
		}
   return NULL;
}


void GiveTeam(int team_index, int money, int points, int winner_money, int winner_points)
{
   CBaseEntity* pPlayer = NULL;
	CBasePlayer* player;
   
   // Pay Taxes (collective money), take home stays the same.
   // The collective money will be used by the Round Winner:
   team[team_index].money += money / 4;

   // Winner gets a little moola cashoolla:
   if (team[team_index].winner)
   if (team[team_index].winner->IsPlayer())
      {
      team[team_index].winner->m_iMoney += winner_money;
      team[team_index].winner->m_iPoints += winner_points;
      }

   // Give money to the whole team:

	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

      if (!pPlayer->pev)
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      player = GetClassPtr ((CBasePlayer*)pPlayer->pev);
          
      if (player->m_iTeam != team_index)
         continue;
      // Exceeding money goes to the team collective:
      player->m_iMoney += money;
      player->m_iPoints += points;
      if (player->m_iMoney > aw.maxmoney)
         {
         team[team_index].money += (player->m_iMoney - aw.maxmoney);
         player->m_iMoney = aw.maxmoney;
         }
      MESSAGE_BEGIN(MSG_ONE, gmsgMoney, NULL, pPlayer->edict());
   	   WRITE_SHORT( player->m_iMoney );
      MESSAGE_END();
	   }  
   // No more winner:
   team[team_index].winner = NULL;
}

void GiveDeadTeam(int team_index, int money, int points)
{
   CBaseEntity* pPlayer = NULL;
	CBasePlayer* player;
   
   // Pay Taxes (collective money), take home stays the same.
   // The collective money will be used by the Round Winner:
   team[team_index].money += money / 4;

   // No more winner:
   team[team_index].winner = NULL;

   // Give money to the dead players of team only:
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

      if (!pPlayer->pev)
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      player = GetClassPtr ((CBasePlayer*)pPlayer->pev);

      if (player->m_iTeam != team_index)
         continue;

      if (player->pev->deadflag == DEAD_NO)
         continue;

      // Exceeding money goes to the team collective:
      player->m_iMoney += money;
      player->m_iPoints += points;
      if (player->m_iMoney > aw.maxmoney)
         {
         team[team_index].money += (player->m_iMoney - aw.maxmoney);
         player->m_iMoney = aw.maxmoney;
         }
      MESSAGE_BEGIN(MSG_ONE, gmsgMoney, NULL, pPlayer->edict());
   	   WRITE_SHORT( player->m_iMoney );
      MESSAGE_END();
	   }  
}

void SendTitle(char *print_string, CBasePlayer *me)
{
      MESSAGE_BEGIN(MSG_ONE, gmsgTitle, NULL, me->edict());
	      WRITE_BYTE( me->m_iTeam);
	      WRITE_STRING( print_string );
      MESSAGE_END();
}

void SendTitle(char *print_string, int team)
{
   CBaseEntity* pPlayer = NULL;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (!pPlayer->pev)
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      MESSAGE_BEGIN(MSG_ONE, gmsgTitle, NULL, pPlayer->edict());
	      WRITE_BYTE( team);
	      WRITE_STRING( print_string );
      MESSAGE_END();
      }
}

void SendTitleTeamOnly(char *print_string, int team, CBasePlayer *me)
{
   CBaseEntity* pPlayer = NULL;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (!pPlayer->pev)
         continue;

      if (pPlayer == me)
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      if (((CBasePlayer *)pPlayer)->m_iTeam != team)
         continue;

      MESSAGE_BEGIN(MSG_ONE, gmsgTitle, NULL, pPlayer->edict());
	      WRITE_BYTE( team);
	      WRITE_STRING( print_string );
      MESSAGE_END();
      }
}


void SendTeamTitle(char *print_string, int team, CBasePlayer *me)
{
   CBaseEntity* pPlayer = NULL;
   CBasePlayer* player = NULL;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (!pPlayer->pev)
         continue;

      if (pPlayer == me)
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      if (pPlayer->pev->deadflag > DEAD_NO)
         continue;

      player = (CBasePlayer*) pPlayer;
      if (player->m_iTeam != team)
         continue;

      ClientPrint(pPlayer->pev, HUD_PRINTCENTER, print_string);
      }
}

void UpdateArmoryIcon(CBasePlayer* pPlayer)
{
   int x;
   if (pPlayer->pev->deadflag > DEAD_NO)
      return;
   if (map_stat.g_fRoundArmoryCloseTime < gpGlobals->time && !pPlayer->m_iNANA)
      x = 0;
   else
      x = IsNearArmory (pPlayer);
   char outbyte = x & 0xFF;
   if (pPlayer->m_iNearArmory != x)
      {
      pPlayer->m_iNearArmory = x;
      MESSAGE_BEGIN(MSG_ONE, gmsgArmory, NULL, pPlayer->pev);
         WRITE_BYTE( outbyte);
      MESSAGE_END();
      }
}

void UpdateTimeIcon(CBasePlayer* pPlayer)
{
   float x, outtime;
   if (map_stat.g_fRoundEndTime)
      {
      x = map_stat.g_fRoundEndTime - gpGlobals->time;
      outtime = x * 10.f;
      MESSAGE_BEGIN(MSG_ONE, gmsgTime, NULL, pPlayer->pev);
         WRITE_SHORT(outtime);
         x = map_stat.g_fRoundArmoryCloseTime - gpGlobals->time;
         outtime = x * 10.f;
         WRITE_SHORT(outtime);
      MESSAGE_END();
      }
}

int IsNearArmory (CBasePlayer* pPlayer)
{
	CBaseEntity* pSpot = NULL;
	if (pPlayer->m_iTeam == TEAM_EAST)
	   {
		pSpot = UTIL_FindEntityByClassname(NULL, "info_player_deathmatch");
		while (pSpot != NULL)
		   {	
			if (((pSpot->pev->origin - pPlayer->pev->origin).Length()) < ARMORY_RANGE)
				return TRUE;
				
			pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch");
		   }
	   }
	else if (pPlayer->m_iTeam == TEAM_WEST)
	   {
		pSpot = UTIL_FindEntityByClassname(NULL, "info_player_start");
		while (pSpot != NULL)
		   {	
			if (((pSpot->pev->origin - pPlayer->pev->origin).Length()) < ARMORY_RANGE)
				return TRUE;
				
			pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_start");
		   }
	   }

   return FALSE;
}

int CountPlayerAmmo(CBasePlayer* pPlayer, char *xweapon)
{
   int ammotype = pPlayer->GetAmmoIndex(xweapon);
   if (ammotype > 0)
      return pPlayer->m_rgAmmo[ammotype];
   return -1;
}

void SDK_ChangeTeam(CBasePlayer* pPlayer)
{
   int clientIndex = pPlayer->entindex();
   
   //memset(pPlayer->m_szTeamName, 0, TEAM_NAME_LENGTH);
   pPlayer->m_szTeamName[0] = pPlayer->m_szModelName[0] = 0;
   switch (pPlayer->m_iTeam)
      {
      case TEAM_EAST:
         strcpy(pPlayer->m_szTeamName, "East");
         switch (pPlayer->m_iGroup)
            {
            case 1:
               strcpy(pPlayer->m_szModelName, TEAM_EAST_GROUP_MODEL_1);
               break;
            case 2:
               strcpy(pPlayer->m_szModelName, TEAM_EAST_GROUP_MODEL_2);
               break;
            case 3:
               strcpy(pPlayer->m_szModelName, TEAM_EAST_GROUP_MODEL_3);
               break;
            case 4:
               strcpy(pPlayer->m_szModelName, TEAM_EAST_GROUP_MODEL_4);
               break;
            default:
               return;
            }
         break;
      case TEAM_WEST:
         strcpy(pPlayer->m_szTeamName, "West");
         switch (pPlayer->m_iGroup)
            {
            case 1:
               strcpy(pPlayer->m_szModelName, TEAM_WEST_GROUP_MODEL_1);
               break;
            case 2:
               strcpy(pPlayer->m_szModelName, TEAM_WEST_GROUP_MODEL_2);
               break;
            case 3:
               strcpy(pPlayer->m_szModelName, TEAM_WEST_GROUP_MODEL_3);
               break;
            case 4:
               strcpy(pPlayer->m_szModelName, TEAM_WEST_GROUP_MODEL_4);
               break;
            default:
               return;
            }
         break;
      default:
         return;
      }
   if (pPlayer->m_iTeam == TEAM_EAST) 
      switch (pPlayer->m_iGroup)
         {
         case 1:
         default:
            CLIENT_COMMAND (pPlayer->edict(), "model east1\n");
            g_engfuncs.pfnSetClientKeyValue( clientIndex,g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "east1" );
            break;
         case 2:
            CLIENT_COMMAND (pPlayer->edict(), "model east2\n");
            g_engfuncs.pfnSetClientKeyValue( clientIndex,g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "east2" );
            break;
         case 3:
            CLIENT_COMMAND (pPlayer->edict(), "model east3\n");
            g_engfuncs.pfnSetClientKeyValue( clientIndex,g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "east3" );
            break;
         case 4:
            CLIENT_COMMAND (pPlayer->edict(), "model east4\n");
            g_engfuncs.pfnSetClientKeyValue( clientIndex,g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "east4" );
            break;
         }
   else if (pPlayer->m_iTeam == TEAM_WEST)
      switch (pPlayer->m_iGroup)
         {
         case 1:
         default:
            CLIENT_COMMAND (pPlayer->edict(), "model west1\n");
            g_engfuncs.pfnSetClientKeyValue( clientIndex,g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "west1" );
            break;
         case 2:
            CLIENT_COMMAND (pPlayer->edict(), "model west2\n");
            g_engfuncs.pfnSetClientKeyValue( clientIndex,g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "west2" );
            break;
         case 3:
            CLIENT_COMMAND (pPlayer->edict(), "model west3\n");
            g_engfuncs.pfnSetClientKeyValue( clientIndex,g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "west3" );
            break;
         case 4:
            CLIENT_COMMAND (pPlayer->edict(), "model west4\n");
            g_engfuncs.pfnSetClientKeyValue( clientIndex,g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", "west4" );
            break;
         }
   //g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->m_szModelName );
   /*g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->m_szTeamName );*/
   /*
   MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( clientIndex );
		WRITE_STRING( pPlayer->m_szTeamName );
	MESSAGE_END();
   */
   MESSAGE_BEGIN(MSG_ALL, gmsgNewTeam);
		WRITE_BYTE( clientIndex );
		WRITE_BYTE( pPlayer->m_iTeam);
   MESSAGE_END();

   return;
}

/*
   This one needs a little explanation:
   The string is a concatenation of client indices + healths with the first
   datum being the number of clients packed. The healths are either <1 - 101>
   or -1 for dead. 0 health = 1, since a 0 would terminate the string. -2 is
   alive.
*/
void SendClientsInfo(CBasePlayer* me, int team_index)
{
   CBaseEntity* pPlayer = NULL;
 	CBasePlayer* player;
   int clientIndex,       
       num_players = 0, 
       rank;
   unsigned char clientHealth;
   char *nn;

	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      nn = (char *)STRING(pPlayer->pev->netname);
      if (nn && nn[0])
         num_players++;
      }
   MESSAGE_BEGIN(MSG_ONE, gmsgCLsHealth, NULL, me->edict());
      WRITE_SHORT(map_stat.g_iWins[TEAM_EAST]);
      WRITE_SHORT(map_stat.g_iWins[TEAM_WEST]);
      WRITE_BYTE(num_players);
   pPlayer = NULL;
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      nn = (char *)STRING(pPlayer->pev->netname);
      if (nn && nn[0])
         {
 	      player = (CBasePlayer*)pPlayer;
         clientIndex = player->entindex();
         if (player->pev->deadflag > DEAD_NO)
            clientHealth = CLS_DEAD;
         else if (player->m_iNANA)
            clientHealth = CLS_BAG;
         else if (me->pev->deadflag > DEAD_NO || !team_index || player->m_iTeam == team_index)
            clientHealth = player->pev->health;
         else 
            clientHealth = CLS_NONE;
         WRITE_BYTE(clientIndex);
         WRITE_BYTE(clientHealth);
         if (player->m_iJuice < 0) player->m_iJuice = 0;
         if (player->m_iJuice > 0)
            rank = GetBestScoreRank(player);
         else 
            rank = 0;
         if (rank > 0)
            {
            WRITE_SHORT(-player->m_iJuice);
            WRITE_BYTE(rank);
            }
         else
            WRITE_SHORT(player->m_iJuice);
         }
	   }  
   MESSAGE_END();
}

/*
   This one needs a little explanation:
   The string is a concatenation of client indices + healths with the first
   datum being the number of clients packed. The healths are either <1 - 101>
   or -1 for dead. 0 health = 1, since a 0 would terminate the string. -2 is
   alive.
*/
void GetTeamInfo(CBasePlayer* me)
{
   CBaseEntity* pPlayer = NULL;
 	CBasePlayer* player;
   char *nn;
   int clientIndex, clientTeam, num_players = 0;

	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      nn = (char *)STRING(pPlayer->pev->netname);
      if (nn && nn[0])
         num_players++;
      }
   MESSAGE_BEGIN(MSG_ONE, gmsgGetTeams, NULL, me->edict());
      WRITE_BYTE(num_players);
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

		if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

      nn = (char *)STRING(pPlayer->pev->netname);
      if (nn && nn[0])
         {
 	      player = (CBasePlayer*)pPlayer;
         clientIndex = player->entindex();
         clientTeam = player->m_iTeam;
         WRITE_BYTE(clientIndex);
         WRITE_BYTE(clientTeam);
         }
	   }  
   MESSAGE_END();
}

extern float map_bag_position[5][3];
extern int map_bags;
void DropNANA(int team)
{
   //int bag_number;
	edict_t	*pent;
   //ALERT (at_console, "bag number %i!\n", map_bags);
   if (!map_bags)
      return;
   //bag_number = rand() % map_bags;
	int istr = MAKE_STRING("item_backpack");
	pent = CREATE_NAMED_ENTITY(istr);
	if ( FNullEnt( pent ) )
		return;
   //float *x = &map_bag_position[bag_number][0];
   CItemBackPack *bb = (CItemBackPack *)CBaseEntity::Instance(pent);
   if (!bb)
      return;
   bb->m_iTeam = team;
   float *x = map_bag_position[team - 1];
	VARS(pent)->origin[0] = x[0];
	VARS(pent)->origin[1] = x[1];
	VARS(pent)->origin[2] = x[2];
	pent->v.spawnflags |= SF_NORESPAWN;
	DispatchSpawn( pent );
}

edict_t *EntGetTeamSpawnPoint( int mteam )
{
	CBaseEntity *pSpot, *loopSpot = NULL;

   switch (mteam)
      {
      case TEAM_EAST:
		   pSpot = team[TEAM_EAST].last_spawn;
         loopSpot = pSpot;
         while ((pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_deathmatch")) != loopSpot)
            {
            if (!pSpot)
               continue;
            if (!pSpot->pev)
               continue;
            if (pSpot->pev->origin == Vector(0, 0, 0))
               continue;
            team[TEAM_EAST].last_spawn = pSpot;
            goto foundSpot;
				}
         break;
      default:
		   pSpot = team[TEAM_WEST].last_spawn;
         loopSpot = pSpot;
         while ((pSpot = UTIL_FindEntityByClassname(pSpot, "info_player_start")) != loopSpot)
            {
            if (!pSpot)
               continue;
            if (!pSpot->pev)
               continue;
            if (pSpot->pev->origin == Vector( 0, 0, 0 ))
               continue;
            team[TEAM_WEST].last_spawn = pSpot;
            goto foundSpot;
				}
         break;
      }

	if (!pSpot)
	   {
		ALERT(at_error, "PutClientInServer: no info_player_start on level");
		return INDEXENT(0);
	   }

foundSpot:
	if (pSpot)
	   {
      return pSpot->edict();
	   }
   return INDEXENT(0);
	
}
void ShowSpawns(int team)
{
   return;
   //int bag_number;
   int index = 0;
	edict_t	*pent, *spotent = NULL, *spotstart = NULL;  
	int istr = MAKE_STRING("item_backpack");

   do
      {
      spotent = EntGetTeamSpawnPoint(team);
      if (!spotstart) spotstart = spotent;
      else if (spotstart == spotent) 
         {
         ALERT(at_error, "Number of Spawns for Team %i = %i", team, index);
         return;
         }
      index++;
	   pent = CREATE_NAMED_ENTITY(istr);
      
	   if ( FNullEnt( pent ) )
		   return;
      CItemBackPack *bb = (CItemBackPack *)CBaseEntity::Instance(pent);
      if (!bb)
         return;
      bb->m_iTeam = team;
	   VARS(pent)->origin[0] = VARS(spotent)->origin[0];
	   VARS(pent)->origin[1] = VARS(spotent)->origin[1];
	   VARS(pent)->origin[2] = VARS(spotent)->origin[2];
	   pent->v.spawnflags |= SF_NORESPAWN;
	   DispatchSpawn( pent );
      } while (1);
   
}

void DropPlayerNANA(CBasePlayer *pPlayer)
{
   //int bag_number;
	edict_t	*pent;
	int istr = MAKE_STRING("item_backpack");
   if (!pPlayer || !pPlayer->pev)
      return;
   if (pPlayer->m_iTeam != TEAM_EAST &&  pPlayer->m_iTeam != TEAM_WEST)
      return;
   if (!map_bags)
      return;
   pPlayer->pev->body = 0;
	pent = CREATE_NAMED_ENTITY(istr);
	if ( FNullEnt( pent ) )
		return;

   SendBossStatus(0, pPlayer);

   //float *x = &map_bag_position[bag_number][0];
   CItemBackPack *bb = (CItemBackPack *)CBaseEntity::Instance(pent);
   if (bb)
      {
      bb->m_iTeam = pPlayer->m_iTeam;
      
      if (pPlayer->pev->flags & FL_DORMANT)
         {
         float *x = map_bag_position[pPlayer->m_iTeam - 1];
	      VARS(pent)->origin[0] = x[0];
	      VARS(pent)->origin[1] = x[1];
	      VARS(pent)->origin[2] = x[2]+5;
         }
      else
         {
	      VARS(pent)->origin[0] = pPlayer->pev->origin[0];
	      VARS(pent)->origin[1] = pPlayer->pev->origin[1];
	      VARS(pent)->origin[2] = pPlayer->pev->origin[2]+5;
         }
	   pent->v.spawnflags |= SF_NORESPAWN;
	   DispatchSpawn( pent );
      }

   char xx[128];
   sprintf (xx, "^_ ^r Your boss %s has died\nGo grab the bag!", STRING(pPlayer->pev->netname));
   SendTitleTeamOnly(xx, pPlayer->m_iTeam, pPlayer);
   //SendTeamTitle(xx, pPlayer->m_iTeam, pPlayer);
}
extern advertisement_t advert[32];
void SendAdvert(CBasePlayer* me)
{
   MESSAGE_BEGIN(MSG_ONE, gmsgCLAdvert, NULL, me->edict());
      WRITE_BYTE(me->stat.s0_index);
      WRITE_STRING(advert[me->stat.s0_index].data);
   MESSAGE_END();
}


void SendStats(CBasePlayer* me)
{
   if (me->stat.s1_index > 18) me->stat.s1_index = 1;
   switch (me->stat.s1_index)
      {
      case 1:
         S1_Message_1(me);
         break;
      case 2:
         S1_Message_2(me);
         break;
      case 3:
         S1_Message_3(me);
         break;
      case 4:
         S1_Message_4(me);
         break;
      case 5:
         S1_Message_5(me);
         break;
      case 6:
         S1_Message_6(me);
         break;
      case 7:
         S1_Message_7(me);
         break;
      case 8:
         S1_Message_8(me);
         break;
      case 9:
         S1_Message_9(me);
         break;
      case 10:
         S1_Message_10(me);
         break;
      case 11:
         S1_Message_11(me);
         break;
      case 12:
         S1_Message_12(me);
         break;
      case 13:
         S1_Message_13(me);
         break;
      case 14:
         S1_Message_14(me);
         break;
      case 15:
         S1_Message_15(me);
         break;
      case 16:
         S1_Message_16(me);
         break;
      case 17:
         S1_Message_17(me);
         break;
      case 18:
         S1_Message_18(me);
         break;
      };
}

CBasePlayer *FindPlayerForward(CBasePlayer *pMe)
{
	TraceResult tr;

   Vector anglesAim = pMe->pev->v_angle + pMe->pev->punchangle;
	UTIL_MakeVectors(anglesAim);
   
	Vector vecSrc = pMe->EyePosition() - gpGlobals->v_up * 2;
   Vector vecEnd = vecSrc + gpGlobals->v_forward * 2048;

	//UTIL_MakeVectors(pMe->pev->v_angle);
	//UTIL_TraceLine(pMe->pev->origin + pMe->pev->view_ofs,pMe->pev->origin + pMe->pev->view_ofs + gpGlobals->v_forward * 2048, dont_ignore_monsters, pMe->edict(), &tr );

   UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, pMe->edict(), &tr);
	if ( tr.flFraction != 1.0 && !FNullEnt( tr.pHit) && tr.pHit->v.takedamage)
	   {
		CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
      if (pHit && pHit->IsPlayer() )
         {
         if (pHit->pev && !(pHit->pev->flags & FL_DORMANT) && pHit->pev->deadflag == DEAD_NO)
            {            
		      return (CBasePlayer *)pHit;
            }
         }
	   }
	return NULL;
}

void SendId(CBasePlayer* me)
{
   CBasePlayer *pID;
   //ALERT( at_console, "in View\n" );
	pID = FindPlayerForward(me);
	if (pID)
	   {
      MESSAGE_BEGIN(MSG_ONE, gmsgCLID, NULL, me->edict());
         WRITE_BYTE(pID->entindex());
         if (me->m_iTeam == pID->m_iTeam)
            WRITE_BYTE(pID->pev->health);
         else
            WRITE_BYTE(120);
         if (pID->m_iJuice < 0) pID->m_iJuice = 0;
         WRITE_SHORT(pID->m_iJuice);
      MESSAGE_END();
      me->m_ilast_ID = 1;
      me->m_flNextID = gpGlobals->time + 2;
	   }
   else
      {
      if (me->m_ilast_ID)
         {
         MESSAGE_BEGIN(MSG_ONE, gmsgCLID, NULL, me->edict());
            WRITE_BYTE(0xFF);
         MESSAGE_END();
         me->m_ilast_ID = 0;
         }
      me->m_flNextID = gpGlobals->time + .2;
      }
   //ALERT( at_console, "out of View\n" );
}

void SwitchPlayerTeam(CBasePlayer *pPlayer, int teamc)
{
	switch (teamc)
		{
		case 1:
         pPlayer->m_iPlayerStatus = 0;
         if (pPlayer->m_iTeam)
            {
            if (pPlayer->pev->deadflag == DEAD_NO && !(pPlayer->m_iClientFlag & CF_JUST_CONNECTED))
               {
               pPlayer->m_iDeaths -= 1;
		         pPlayer->pev->frags += 1;
               pPlayer->Killed(pPlayer->pev, GIB_NEVER);
               }
            }
         UpdateTeamMembers();
         if (team[TEAM_EAST].members < 16)
            {
            pPlayer->m_iTeam = TEAM_EAST;
            UTIL_LogPrintf( "%s joined EAST COAST\n", STRING(pPlayer->pev->netname));
            strcpy(pPlayer->m_szTeamName, "EAST");
            UpdateTeamMembers();
            DisplayGroupMenu(pPlayer, TEAM_EAST);
            }
         else
            {
            pPlayer->m_iTeam = TEAM_WEST;
            UTIL_LogPrintf( "%s joined WEST COAST\n", STRING(pPlayer->pev->netname));
            strcpy(pPlayer->m_szTeamName, "WEST");
            UpdateTeamMembers();
            DisplayGroupMenu(pPlayer, TEAM_WEST);
            }
         break;
      case 2:
         pPlayer->m_iPlayerStatus = 0;
         if (pPlayer->m_iTeam)
            {
            if (pPlayer->pev->deadflag == DEAD_NO && !(pPlayer->m_iClientFlag & CF_JUST_CONNECTED))
               {
               pPlayer->m_iDeaths -= 1;
		         pPlayer->pev->frags += 1;
               pPlayer->Killed(pPlayer->pev, GIB_NEVER);
               }
            }
         UpdateTeamMembers();
         if (team[TEAM_WEST].members < 16)
            {
            pPlayer->m_iTeam = TEAM_WEST;
            UTIL_LogPrintf( "%s joined WEST COAST\n", STRING(pPlayer->pev->netname));
            strcpy(pPlayer->m_szTeamName, "WEST");
            UpdateTeamMembers();
            DisplayGroupMenu(pPlayer, TEAM_WEST);
            }
         else
            {
            pPlayer->m_iTeam = TEAM_EAST;
            UTIL_LogPrintf( "%s joined EAST COAST\n", STRING(pPlayer->pev->netname));
            strcpy(pPlayer->m_szTeamName, "EAST");
            UpdateTeamMembers();
            DisplayGroupMenu(pPlayer, TEAM_EAST);
            }
         break;
      case 3:
         /*
         pPlayer->m_iPlayerStatus = 0;
         UpdateTeamMembers();
         if (team[TEAM_EAST].members < team[TEAM_WEST].members)
            myteam = TEAM_EAST;
         if (pPlayer->m_iTeam)
            {
            if (pPlayer->pev->deadflag == DEAD_NO && !(pPlayer->m_iClientFlag & CF_JUST_CONNECTED))
               {
               pPlayer->m_iDeaths -= 1;
		         pPlayer->pev->frags += 1;
               pPlayer->Killed(pPlayer->pev, GIB_NEVER);
               }
            }
         pPlayer->m_iTeam = myteam;
         if (myteam == TEAM_EAST)
            {
            strcpy(pPlayer->m_szTeamName, "EAST");
            UpdateTeamMembers();
            DisplayGroupMenu(pPlayer, TEAM_EAST);
            }
         else
            {
            strcpy(pPlayer->m_szTeamName, "WEST");
            UpdateTeamMembers();
            DisplayGroupMenu(pPlayer, TEAM_WEST);
            }
         */
         break;
      case 5:
         pPlayer->m_iClientFlag &= ~CF_JUST_CONNECTED;
         break;
		}
}

void SwitchPlayerGroup(CBasePlayer *pPlayer, int group)
{
   pPlayer->m_iPlayerStatus = 0;
   pPlayer->m_iMenu = 0;
   pPlayer->m_iGroup = group;
   SDK_ChangeTeam(pPlayer);
   if (pPlayer->m_iTeam == TEAM_EAST)
      UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "%s has joined East Coast\n", STRING(pPlayer->pev->netname)));
   else if (pPlayer->m_iTeam == TEAM_WEST)
      UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "%s has joined West Coast\n", STRING(pPlayer->pev->netname)));
   if (pPlayer->pev->deadflag == DEAD_NO && !(pPlayer->m_iClientFlag & CF_JUST_CONNECTED))
      {
      pPlayer->m_iDeaths -= 1;
		pPlayer->pev->frags += 1;
      pPlayer->Killed(pPlayer->pev, GIB_NEVER);
      }
   else
      {
      if (pPlayer->m_iClientFlag & CF_JUST_CONNECTED)
         {
         pPlayer->m_iClientFlag &= ~CF_JUST_CONNECTED;
         if (map_stat.g_fRoundEndRespawnTime > gpGlobals->time)
            {
            pPlayer->Spawn();
            return;
            }
         }
      if (team[pPlayer->m_iTeam].alive < 1)
         {
         pPlayer->m_iClientFlag &= ~CF_JUST_CONNECTED;
         FreshRound();
         }      
      }
}

CBasePlayer *GetPrevClient(int c_index)
{
   int i;
   char *nn;
   CBasePlayer *player;
   CBaseEntity *pPlayer;
   i = c_index - 1;
   while (i > 0)
      {
      pPlayer = UTIL_PlayerByIndex( i );
 		if (pPlayer && !FNullEnt(pPlayer->edict()))
         {
         player = (CBasePlayer*)pPlayer;
         nn = (char *)STRING(player->pev->netname);
         if (nn && nn[0])
            return player;
         }		
      i--;
      }
   i = gpGlobals->maxClients;
   while (i > 0)
      {
      pPlayer = UTIL_PlayerByIndex( i );
 		if (pPlayer && !FNullEnt(pPlayer->edict()))
         {
         player = (CBasePlayer*)pPlayer;
         nn = (char *)STRING(player->pev->netname);
         if (nn && nn[0])
            return player;
         }		
      i--;
      }
   return NULL;
}

CBasePlayer *GetNextClient(int c_index)
{
   int i;
   char *nn;
   CBasePlayer *player;
   CBaseEntity *pPlayer;
   i = c_index + 1;
   while (i <= gpGlobals->maxClients)
      {
      pPlayer = UTIL_PlayerByIndex( i );
 		if (pPlayer && !FNullEnt(pPlayer->edict()))
         {
         player = (CBasePlayer*)pPlayer;
         nn = (char *)STRING(player->pev->netname);
         if (nn && nn[0])
            return player;
         }		
      i++;
      }
   i = 1;
   while (i <= gpGlobals->maxClients)
      {
      pPlayer = UTIL_PlayerByIndex( i );
 		if (pPlayer && !FNullEnt(pPlayer->edict()))
         {
         player = (CBasePlayer*)pPlayer;
         nn = (char *)STRING(player->pev->netname);
         if (nn && nn[0])
            return player;
         }		
      i++;
      }
   return NULL;
}

int IsValidPlayer(CBasePlayer *me)
{
   int i;
   char *nn;
   CBasePlayer *player;
   CBaseEntity *pPlayer;
   i = 1;
   while (i <= gpGlobals->maxClients)
      {
      pPlayer = UTIL_PlayerByIndex( i );
      i++;
 		if (!pPlayer) continue; 
      if (pPlayer != me) continue;
      if (FNullEnt(pPlayer->edict())) continue;
      player = (CBasePlayer*)pPlayer;
      nn = (char *)STRING(player->pev->netname);
      if (!nn) continue;
      if (!nn[0]) continue;
      return TRUE;
      }
   return FALSE;
}

CBasePlayer *GetClientWithWonID(unsigned int WonID)
{
   int i = 1;
   char *nn;
   CBasePlayer *player;
   CBaseEntity *pPlayer;
   while (i <= gpGlobals->maxClients)
      {
      pPlayer = UTIL_PlayerByIndex( i );
 		if (pPlayer && !FNullEnt(pPlayer->edict()))
         {
         player = (CBasePlayer*)pPlayer;
         if (player->m_iWONID == WonID)
            {
            nn = (char *)STRING(player->pev->netname);
            if (nn && nn[0])
               {
               return player;
               }
            }
         }		
      i++;
      }
   return NULL;
}

CBasePlayer *GetClientWithSteamID(char *SteamID)
{
   int i = 1;
   char *nn;
   CBasePlayer *player;
   CBaseEntity *pPlayer;
   while (i <= gpGlobals->maxClients)
      {
      pPlayer = UTIL_PlayerByIndex( i );
 		if (pPlayer && !FNullEnt(pPlayer->edict()))
         {
         player = (CBasePlayer*)pPlayer;
         if (!strcmp(player->m_iSteamID, SteamID))
            {
            nn = (char *)STRING(player->pev->netname);
            if (nn && nn[0])
               {
               return player;
               }
            }
         }		
      i++;
      }
   return NULL;
}

void SendBossStatus(int boss, CBasePlayer *pPlayer)
{
   int index = 1;
   CBasePlayer *client;
   if (boss)
      {
      for (index = 1; index <= gpGlobals->maxClients; index++)
         {
         client = (CBasePlayer *) UTIL_PlayerByIndex( index );
         if (!client) continue;
         if (client->m_iTeam != pPlayer->m_iTeam) continue;
         if (!client->pev) continue;
         if (FNullEnt(client->edict())) continue;
         if (client->pev->deadflag > DEAD_NO) continue;
         MESSAGE_BEGIN( MSG_ONE, gmsgNANA, NULL, client->pev);
            WRITE_BYTE(boss);
            WRITE_BYTE(pPlayer->entindex());
         MESSAGE_END();
         }
      return;
      }
   for (index = 1; index <= gpGlobals->maxClients; index++)
      {
      client = (CBasePlayer *) UTIL_PlayerByIndex( index );
      if (!client) continue;
      if (client->m_iTeam != pPlayer->m_iTeam) continue;
      if (!client->pev) continue;
      if (FNullEnt(client->edict())) continue;
      if (client->pev->deadflag > DEAD_NO) continue;
      MESSAGE_BEGIN( MSG_ONE, gmsgNANA, NULL, client->pev);
         WRITE_BYTE(0);
         WRITE_BYTE(0);
      MESSAGE_END();
      }
}