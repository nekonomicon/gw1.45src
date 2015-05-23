#ifndef SDK_C_STATS_H
#define SDK_C_STATS_H

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

void S1_Message_1(CBasePlayer* me);
void S1_Message_2(CBasePlayer* me);
void S1_Message_3(CBasePlayer* me);
void S1_Message_4(CBasePlayer* me);
void S1_Message_5(CBasePlayer* me);
void S1_Message_6(CBasePlayer* me);
void S1_Message_7(CBasePlayer* me);
void S1_Message_8(CBasePlayer* me);
void S1_Message_9(CBasePlayer* me);
void S1_Message_10(CBasePlayer* me);
void S1_Message_11(CBasePlayer* me);
void S1_Message_12(CBasePlayer* me);
void S1_Message_13(CBasePlayer* me);
void S1_Message_14(CBasePlayer* me);
void S1_Message_15(CBasePlayer* me);
void S1_Message_16(CBasePlayer* me);
void S1_Message_17(CBasePlayer* me);
void S1_Message_18(CBasePlayer* me);

#endif