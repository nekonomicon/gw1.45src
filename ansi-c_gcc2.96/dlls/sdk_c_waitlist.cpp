/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_c_waitlist.cpp
   This is the client resurection waitlist module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"sdk_u_main.h"
#include	"sdk_e_menu.h"
#include	"sdk_c_waitlist.h"

extern WaitList wl[4];
void SendTitle(char *print_string, int team);
void SendTitle(char *print_string, CBasePlayer *me);
int IsValidPlayer(CBasePlayer *me);


WaitList::WaitList()
{
   q = NULL;
   q = new DeadQueue_t[32];
   FlushWaitList();
}

WaitList::~WaitList()
{
   FlushWaitList();
   if (q) delete [] q;
   q = NULL;
}

void WaitList::FlushWaitList()
{
   int i;
   for (i = 0; i < 32; i++)
      {
      q[i].player = NULL;
      q[i].used = 0;
      }
   index = 0;
}

void WaitList::DisplayList(CBasePlayer *me)
{
   CBasePlayer *client;
   int i, order = 0;
   char xx[256];
   for (i = 0; i < 25; i++)
      {
      if (q[0].used == 0xDEADC0DE)
         {  
         client = q[i].player;
         if (!IsValidPlayer(client)) continue;
         if (client->m_iTeam != me->m_iTeam) continue;
         if (client->pev->deadflag == DEAD_NO) continue;
         if (client->pev->flags & FL_DORMANT) continue;
         order++;
         if (client == me)
            {
            sprintf(xx, "^_ ^n QUEUE POSITION: %i", order);
            SendTitle(xx, me);
            return;
            }
         }
      }
   SendTitle("^_ ^n You are not in the queue", me);
}

void WaitList::AddToWaitList(CBasePlayer *p)
{

   if (index < 0) index = 0;
   if (index < 16)
      {
      q[index].player = p;
      q[index].used = 0xDEADC0DE;
      index++;
      }
}

CBasePlayer *WaitList::NextOnWaitList(int team)
{
   CBasePlayer *client = NULL;
   int i = 0;
   while (i < 16)
      {
      if (q[0].used == 0xDEADC0DE)
         {  
         client = q[0].player;
         if (IsValidPlayer(client))
            {
            if (client->pev->deadflag > DEAD_NO && !(client->pev->flags & FL_DORMANT))
               {
               ShiftDown();
               return client;
               }
            }
         }
      ShiftDown();
      i++;
      }
   return NULL;
}

void WaitList::ShiftDown()
{
   int i; 
   for (i = 0; i < 25; i++) q[i] = q[i+1];
   for (; i < 32; i++)
      {q[i].player = NULL; q[i].used = 0;}
   index--;
   if (index < 0) index = 0;
}


extern int gmsgCLSpawn;
extern CBasePlayer *last_player_killer;
void RespawnDeadPlayers( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
   int i;
   if (!pKiller)
      return;


	CBasePlayer *peKiller = NULL;
	CBaseEntity *ep = CBaseEntity::Instance( pKiller );
   if (!ep)
      {
      UTIL_LogPrintf("Crashed @ 004\n");
      return;
      }
   CBasePlayer *PK = (CBasePlayer*)ep;

   if (!IsValidPlayer(PK)) return;

   if (PK != pVictim)
      {
      last_player_killer = PK;
      PK->m_iNextMoney += aw.killmoney;
      PK->m_iJuice += AWARD_JUICE_FRAG;
      pVictim->m_iJuice += AWARD_JUICE_DEATH;
		// let the killer paint another decal as soon as he'd like.
		PK->m_flNextDecalTime = gpGlobals->time;
      // NANA holder will respawn his teammate
      if (PK->m_iTeam != pVictim->m_iTeam)
         {
         PK->stat.Offense++;
         pVictim->stat.Defense++;
         if (pVictim->m_iNANA)
            {
            PK->m_iNextMoney += aw.killmoney * 3;
            PK->m_iJuice += AWARD_JUICE_KILLEDLEADER;
            pVictim->m_iJuice += AWARD_JUICE_KILLEDASLEADER;
            }
         if (PK->m_iNANA)
            {
            PK->m_iNextMoney += aw.killmoney + aw.respawnmoney;
            PK->m_iJuice += AWARD_JUICE_RESPAWNED;
            for (i = 0; i < aw.respawns; i++)
               {
               if (PK->m_iTeam == TEAM_EAST)
                  peKiller = wl[TEAM_EAST].NextOnWaitList(TEAM_EAST);
               else if (PK->m_iTeam == TEAM_WEST)
                  peKiller = wl[TEAM_WEST].NextOnWaitList(TEAM_WEST);
               else 
                  peKiller = NULL;
               if (peKiller)
                  {
                  peKiller->Spawn();
                  MESSAGE_BEGIN(MSG_ALL, gmsgCLSpawn);
		               WRITE_BYTE( peKiller->entindex());
		               WRITE_BYTE( PK->entindex());
		               WRITE_BYTE( peKiller->m_iTeam);
                  MESSAGE_END();
                  UTIL_LogPrintf( "%s was respawned by %s\n", STRING(peKiller->pev->netname), STRING(PK->pev->netname));
                  }
               }
            }  
         }
      }
}
