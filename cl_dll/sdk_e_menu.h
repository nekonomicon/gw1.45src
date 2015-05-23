#ifndef SDK_MENU_H
#define SDK_MENU_H

#define EAST_RED        226
#define EAST_GREEN      172
#define EAST_BLUE       86

#define WEST_RED        89
#define WEST_GREEN      153
#define WEST_BLUE       171

#define TRUE_RED        216
#define TRUE_GREEN      201
#define TRUE_BLUE       198

#define FALSE_RED       130
#define FALSE_GREEN     110 
#define FALSE_BLUE      106

#define SCORE_FG_RED    TRUE_RED
#define SCORE_FG_GREEN  TRUE_GREEN
#define SCORE_FG_BLUE   TRUE_BLUE

#define SCORE_BG_RED    FALSE_RED/2
#define SCORE_BG_GREEN  FALSE_GREEN/2
#define SCORE_BG_BLUE   FALSE_BLUE/2

#define CLS_DEAD        0x81
#define CLS_BAG         0x82
#define CLS_NONE        0x83

#define D_MEDIUM_TEAMMATE     0x80
#define D_MEDIUM_UNKNOWN      0x81

#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7

enum
{
      MSG_STAT_SHOOTING_HIGH = 1, // Highest shooting percentage
      MSG_STAT_SHOOTING_LOW,      // Lowest shooting percentage
      MSG_STAT_MOST_SHOTS,        // Most shots
      MSG_STAT_LEAST_SHOTS,       // Least shots
      MSG_STAT_PISTOL_KILLS,      // Highest pistol kills
      MSG_STAT_SHOTGUN_KILLS,     // Highest shotgun kills
      MSG_STAT_SUB_KILLS,         // Highest sub kills
      MSG_STAT_RIFLE_KILLS,       // Highest rifle kills
      MSG_STAT_PRECISION_KILLS,   // Highest precision rifle kills   
      MSG_STAT_SPECIALTY_KILLS,   // Highest specialty weapon kills
      MSG_STAT_KILLS,             // Highest kills
      MSG_STAT_KILL_RATIO,        // Highest kill ratio
      MSG_STAT_ROUND_KILLS,       // Highest round kills
      MSG_STAT_LIFE_EXPECTANCY,   // Highest life expectancy
      MSG_STAT_SUICIDES,          // Highest suicides
      MSG_STAT_BODY_COUNT,        // Highest body count per minute
      MSG_STAT_OFFENSE,           // Best offensive record
      MSG_STAT_DEFENSE            // Worst defensive record
};

typedef struct 
{
   char data[256];
   int length;
   int index;
   int x;
}  scroller_t;

typedef struct map_vote_s
{
   int   votes;
   float ratio;
   char name[64];
}  map_vote_t;

typedef struct
   {
   int pistols;
   int shotguns;
   int subs;
   int autos;
   int rifles;
   int specialty;
   int items;
   int upgradeprice;
   } avail_weapons_t;

extern avail_weapons_t aw;

extern scroller_t s_top, s_bottom;
void DisplayTeamMenu(char *message, short *validbuttons, int team, int thugs1, int thugs2);
void DisplayGroupMenu(char *message, short *validbuttons, int team);
void DisplayBuyWeaponMenu(char *message, short *validbuttons, int team);
void DisplayBuyPistolMenu(char *message, short *validbuttons, int team);
void DisplayBuyShotgunMenu(char *message, short *validbuttons, int team);
void DisplayBuySubMenu(char *message, short *validbuttons, int team);
void DisplayBuyAutoMenu(char *message, short *validbuttons, int team);
void DisplayAudio1Menu(char *message, short *validbuttons, int team);
void DisplayAudio2Menu(char *message, short *validbuttons, int team);

#define MAX_MAP_VOTES 10
extern int  num_players;
extern map_vote_t map_vote[MAX_MAP_VOTES];
#endif