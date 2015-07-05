#ifndef SDK_MENU_H
#define SDK_MENU_H

void ShowMenu (CBasePlayer *pPlayer, int bitsValidSlots, int nDisplayTime, BOOL fNeedMore, char *pszText);
void DisplayTeamMenu(CBasePlayer *pPlayer);
void DisplayGroupMenu(CBasePlayer *pPlayer, int team);
void DisplayBuyWeaponMenu(CBasePlayer *pPlayer, int team);
void DisplayBuyPistolMenu(CBasePlayer *pPlayer, int team);
void DisplayBuyShotgunMenu(CBasePlayer *pPlayer, int team);
void DisplayBuySubMenu(CBasePlayer *pPlayer, int team);
void DisplayBuyAutoMenu(CBasePlayer *pPlayer, int team);
void DisplayBuyRifleMenu(CBasePlayer *pPlayer, int team);
void DisplayBuySpecialtyGunMenu(CBasePlayer *pPlayer, int team);
void DisplayBuyItemMenu(CBasePlayer *pPlayer, int team);
void DisplayAudioMenu1(CBasePlayer *pPlayer, int team);
void DisplayAudioMenu2(CBasePlayer *pPlayer, int team);

int BuyWeapon(CBasePlayer *pPlayer, int weapon_id, int price, char *weapon_name, int );
int BuyPrimaryAmmo(CBasePlayer* pPlayer);
int BuySecondaryAmmo(CBasePlayer* pPlayer);
void SendTitleTeamOnly(char *print_string, int team, CBasePlayer *me);
void SendTeamTitle(char *, int, CBasePlayer* pPlayer);

#define CLS_DEAD     0x81
#define CLS_BAG      0x82
#define CLS_NONE     0x83

#define D_MEDIUM_TEAMMATE     0x80
#define D_MEDIUM_UNKNOWN      0x81

#define AWARD_CASH_FRAG                250
#define AWARD_CASH_KILLEDLEADER        450
#define AWARD_CASH_RESPAWNED           300
#define AWARD_CASH_SURVIVOR            200
#define AWARD_CASH_EXTERMINATOR        300

#define AWARD_JUICE_FRAG               3
#define AWARD_JUICE_DEATH              -1
#define AWARD_JUICE_KILLEDLEADER       8
#define AWARD_JUICE_RESPAWNED          6
#define AWARD_JUICE_KILLEDASLEADER     -5
#define AWARD_JUICE_EXTERMINATOR       5
#define AWARD_JUICE_SURVIVOR           2


#define VOICE_IDX_AIGHT          0
#define VOICE_IDX_FUCKDAT        1
#define VOICE_IDX_CHECKDISOUT    2
#define VOICE_IDX_FOLLOWME       3
#define VOICE_IDX_PUTO           4
#define VOICE_IDX_WASSUP         5
#define VOICE_IDX_NOTHING        6
#define VOICE_IDX_WAZA1          7
#define VOICE_IDX_WAZA2          8
#define VOICE_IDX_WAWA3          9
#define VOICE_IDX_WAZA4         10

#define ADMIN_TORTURE_ROUNDSTART 1
#define ADMIN_TORTURE_FREEZE     2
#define ADMIN_TORTURE_GAG        4

#define MAX_ADMINS               48

typedef struct
   {
   unsigned int WONID;
   char password[32];
   unsigned int level;
   char SteamID[64];
   } admin_database_t;

typedef struct
   {
   int pistols;
   int shotguns;
   int subs;
   int autos;
   int items;
   int baselicense;
   int basemoney;
   int upgradeprice;
   int grenades;
   float damage;
   float headshot;
   float grenade;
   float timelimit;
   char nextmap[64];
   int anti_cheats;
   int respawns;
   int respawn_min;
   int respawntime;
   int winmoney;
   int losemoney;
   int respawnmoney;
   int killmoney;
   int lastkillmoney;
   int drawmoney;
   int maxmoney;
   int freebies;
   int quiet;
   float recoil;
   float speed;
   float kickback;
   float fadeout;
   } avail_weapons_t;

extern avail_weapons_t aw, old_aw;
extern admin_database_t adb[MAX_ADMINS];
extern int num_admins;
extern char admin_reject_message[512];

#define TAG_KILL        1
#define TAG_FOLLOW      2
#define TAG_HURT        4
#define TAG_DONTSHOOT   8

#define GW_SET_LICENSE     1
#define GW_RESET_LICENSE   2
#define GW_SET_WEAPONS     3
#define GW_RESET_WEAPONS   4
#define GW_RESET_SCORES    5
#define GW_SET_DAMAGE      6
#define GW_RESET_DAMAGE    7
#define GW_SMOKE_EAST      8
#define GW_SMOKE_WEST      9

#define GW_AC_BUNNYHOP     1



#endif