#ifndef SDK_C_GLOBAL_H
#define SDK_C_GLOBAL_H

#include <time.h>

// void DropPrimaryWeapon(CBasePlayer * player);
void RestartRound();

#define MAX_MAP_VOTES   10

typedef struct best_score_s
   {
   char name[64];
   int  juice;
   time_t rawtime;
   unsigned int wonid;
   char SteamID[128];
   } best_score_t;

typedef struct level_s
   {
   float g_fRoundStartTime;
   float g_fRoundEndTime;
   float g_fRoundEndRespawnTime;
   float g_fRoundCriticalTime;
   float g_fRoundArmoryCloseTime;
   float m_fRestartRoundTime;
   int   g_iRounds;
   int   g_iKills;
   int   g_iShots;
   int   g_iWins[2];
   int   g_iFlags;
   int   g_iRoundTime;
   FILE  *logfile;
   best_score_t bs[10];
   } level_t;

typedef struct map_vote_s
   {
   int   votes;
   float ratio;
   char  name[64];   
   } map_vote_t;

void FitBestScore(CBasePlayer *me);
int GetBestScoreRank(CBasePlayer *me);
void LoadBestScore();
void SaveBestScore();
void SendVoteMaps(CBasePlayer *pPlayer, int num_players);
int CompileMapVotes();

extern level_t map_stat;
extern map_vote_t map_vote[MAX_MAP_VOTES];
extern int map_vote_count;
#endif