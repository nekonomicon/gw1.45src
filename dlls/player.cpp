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
/*

===== player.cpp ========================================================

  functions dealing with the player

*/

#include "extdll.h"
#include "util.h"

#include "cbase.h"
#include "player.h"
#include "trains.h"
#include "weapons.h"
#include "soundent.h"
#include "monsters.h"
#include "../engine/shake.h"
#include "decals.h"
#include "gamerules.h"
#include "sdk_e_menu.h"
#include "sdk_u_main.h"
#include "sdk_c_global.h"

#define SG550_MODULE
#define AUG_MODULE
#define SG552_MODULE
#include "sdk_w_classes.h"

// #define DUCKFIX

void UpdateArmoryIcon(CBasePlayer* pPlayer);
void UpdateTimeIcon(CBasePlayer* pPlayer);

extern DLL_GLOBAL ULONG		g_ulModelIndexPlayer;
extern DLL_GLOBAL BOOL		g_fGameOver;
extern DLL_GLOBAL	BOOL	g_fDrawLines;
int gEvilImpulse101;
extern DLL_GLOBAL int		g_iSkillLevel, gDisplayTitle;
BOOL gInitHUD = TRUE;

extern void CopyToBodyQue(entvars_t* pev);
extern void respawn(entvars_t *pev, BOOL fCopyCorpse);
extern Vector VecBModelOrigin(entvars_t *pevBModel );
extern edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer );
int MapTextureTypeStepType(char chTextureType);

#define	PLAYER_WALLJUMP_SPEED 300 // how fast we can spring off walls
#define PLAYER_LONGJUMP_SPEED 350 // how fast we longjump

#define TRAIN_ACTIVE	0x80 
#define TRAIN_NEW		0xc0
#define TRAIN_OFF		0x00
#define TRAIN_NEUTRAL	0x01
#define TRAIN_SLOW		0x02
#define TRAIN_MEDIUM	0x03
#define TRAIN_FAST		0x04 
#define TRAIN_BACK		0x05

#define	FLASH_DRAIN_TIME	 1.2 //100 units/3 minutes
#define	FLASH_CHARGE_TIME	 0.2 // 100 units/20 seconds  (seconds per unit)


void SendId(CBasePlayer* me);

//#define PLAYER_MAX_SAFE_FALL_DIST	20// falling any farther than this many feet will inflict damage
//#define	PLAYER_FATAL_FALL_DIST		60// 100% damage inflicted if player falls this many feet
//#define	DAMAGE_PER_UNIT_FALLEN		(float)( 100 ) / ( ( PLAYER_FATAL_FALL_DIST - PLAYER_MAX_SAFE_FALL_DIST ) * 12 )
//#define MAX_SAFE_FALL_UNITS			( PLAYER_MAX_SAFE_FALL_DIST * 12 )

int gmsgShake = 0;
int gmsgFade = 0;
int gmsgSelAmmo = 0;
int gmsgFlashlight = 0;
int gmsgFlashBattery = 0;
int gmsgResetHUD = 0;
int gmsgInitHUD = 0;
int gmsgShowGameTitle = 0;
int gmsgCurWeapon = 0;
int gmsgHealth = 0;
int gmsgDamage = 0;
int gmsgBattery = 0;
int gmsgTrain = 0;
int gmsgLogo = 0;
int gmsgWeaponList = 0;
int gmsgAmmoX = 0;
int gmsgHudText = 0;
int gmsgDeathMsg = 0;
int gmsgScoreInfo = 0;
int gmsgTeamInfo = 0;
int gmsgTeamScore = 0;
int gmsgGameMode = 0;
int gmsgMOTD = 0;
int gmsgAmmoPickup = 0;
int gmsgWeapPickup = 0;
int gmsgItemPickup = 0;
int gmsgHideWeapon = 0;
int gmsgSetCurWeap = 0;
int gmsgSayText = 0;
int gmsgTextMsg = 0;
int gmsgSetFOV = 0;
int gmsgShowMenu = 0;
int gmsgCSOSMenu = 0;
int gmsgSetScope = 0;
int gmsgSmokeWindow = 0;
int gmsgSpectator = 0;
int gmsgTitle = 0;
int gmsgNANA = 0;
int gmsgRadar = 0;
int gmsgMoney = 0;
int gmsgArmory = 0;
int gmsgTime = 0;
int gmsgCLsHealth = 0;
int gmsgNewTeam = 0;
int gmsgGetTeams = 0;
int gmsgCLAdvert= 0;
int gmsgCLStat = 0;
int gmsgCLSpawn = 0;
int advert_count = 0;
int gmsgCLID = 0;
int gmsgSilencer = 0;
int gmsgVGUIMenu = 0;
int gsmgSendStats = 0;
int gsmgSendSteamID = 0;
int gsmgSendTop32 = 0;
int gsmgAdminMenu = 0;
int gsmgPlayBeat = 0;
int gsmgPlayTheme = 0;
int gsmgServerVars = 0;
int gsmgVoteMaps = 0;
int gsmgCLUpdate = 0;
int gsmgPlayMp3 = 0;

int ReadAdFile(char *filename);
void SendTitle(char *print_string, CBasePlayer *me);

void LinkUserMessages( void )
{
	// Already taken care of?
	if ( gmsgSelAmmo )
	{
		return;
	}

	gmsgSelAmmo = REG_USER_MSG("SelAmmo", sizeof(SelAmmo));
	gmsgCurWeapon = REG_USER_MSG("CurWeapon", 3);
	gmsgFlashlight = REG_USER_MSG("Flashlight", 2);
	gmsgFlashBattery = REG_USER_MSG("FlashBat", 1);
	gmsgHealth = REG_USER_MSG( "Health", 1 );
	gmsgDamage = REG_USER_MSG( "Damage", 12 );
	gmsgBattery = REG_USER_MSG( "Battery", 2);
	gmsgTrain = REG_USER_MSG( "Train", 1);
	gmsgHudText = REG_USER_MSG( "HudText", -1 );
	gmsgSayText = REG_USER_MSG( "SayText", -1 );
	gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 );
	gmsgWeaponList = REG_USER_MSG("WeaponList", -1);
	gmsgResetHUD = REG_USER_MSG("ResetHUD", 1);		// called every respawn
	gmsgInitHUD = REG_USER_MSG("InitHUD", 0 );		// called every time a new player joins the server
	gmsgShowGameTitle = REG_USER_MSG("GameTitle", 1);
	gmsgDeathMsg = REG_USER_MSG( "DeathMsg", 4 );
	gmsgScoreInfo = REG_USER_MSG( "ScoreInfo", 5 );
	gmsgTeamInfo = REG_USER_MSG( "TeamInfo", -1 );  // sets the name of a player's team
	gmsgTeamScore = REG_USER_MSG( "TeamScore", -1 );  // sets the score of a team on the scoreboard
	gmsgGameMode = REG_USER_MSG( "GameMode", 1 );
	gmsgMOTD = REG_USER_MSG( "MOTD", -1 );
	gmsgAmmoPickup = REG_USER_MSG( "AmmoPickup", 2 );
	gmsgWeapPickup = REG_USER_MSG( "WeapPickup", 2 );
	gmsgItemPickup = REG_USER_MSG( "ItemPickup", -1 );
	gmsgHideWeapon = REG_USER_MSG( "HideWeapon", 1 );
	gmsgSetFOV = REG_USER_MSG( "SetFOV", 1 );
	gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 );
	gmsgCSOSMenu = REG_USER_MSG( "CSOSMenu", -1 );
	gmsgShake = REG_USER_MSG("ScreenShake", sizeof(ScreenShake));
	gmsgFade = REG_USER_MSG("ScreenFade", sizeof(ScreenFade));
	gmsgAmmoX = REG_USER_MSG("AmmoX", 2);
   gmsgSpectator = REG_USER_MSG( "Spectator", 2 );
	gmsgSetScope = REG_USER_MSG("SetScope", 2);
	gmsgSmokeWindow = REG_USER_MSG("SmokeWindow", 6);
	gmsgTitle = REG_USER_MSG("Title", -1);
	gmsgNANA = REG_USER_MSG("NANA", 2);
   gmsgRadar = REG_USER_MSG("Radar", -1);
	gmsgMoney = REG_USER_MSG("Money", 2);
	gmsgArmory = REG_USER_MSG("Armory", 1);
	gmsgTime = REG_USER_MSG("Time", 4);
	gmsgCLsHealth = REG_USER_MSG("CLsHealth", -1);
	gmsgNewTeam = REG_USER_MSG("NewTeam", 2);
	gmsgGetTeams = REG_USER_MSG("GetTeams", -1);
	gmsgCLAdvert = REG_USER_MSG("CLAdvert", -1);
	gmsgCLStat = REG_USER_MSG("CLStat", -1);
	gmsgCLSpawn = REG_USER_MSG("CLSpawn", 3);
	gmsgCLID = REG_USER_MSG("CLID", -1);
	gmsgSilencer = REG_USER_MSG("Silencer", 2);
   gmsgVGUIMenu = REG_USER_MSG("VGUIMenu", 1);
   gsmgSendStats = REG_USER_MSG("SendStats", sizeof(sendstat_t) - 64 - 4);
   gsmgSendSteamID = REG_USER_MSG("SendSteamID", -1);
   gsmgSendTop32 = REG_USER_MSG("SendTop32", -1);
   gsmgAdminMenu = REG_USER_MSG("AdminMenu", 2);
   gsmgPlayBeat = REG_USER_MSG("PlayBeat", 1);
   gsmgPlayTheme = REG_USER_MSG("PlayTheme", 1);
   gsmgServerVars = REG_USER_MSG("ServerVars", -1);
   gsmgVoteMaps = REG_USER_MSG("VoteMaps", -1);
   gsmgCLUpdate = REG_USER_MSG("CLUpdate", 1);
   advert_count = ReadAdFile("gangwars/ad.txt");
   gsmgPlayMp3 = REG_USER_MSG("PlayMp3", -1);
}
LINK_ENTITY_TO_CLASS( player, CBasePlayer );

void CBasePlayer :: Speak(int soundindex)
{
   switch (soundindex)
      {
      case VOICE_IDX_AIGHT:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/aight.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_FUCKDAT:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/fuckdat.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_CHECKDISOUT:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/checkdisout.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_FOLLOWME:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/follow.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_PUTO:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/puto.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_WASSUP:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/wassup.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_NOTHING:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/nuttin.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_WAZA1:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/waza1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_WAZA2:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/waza2.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_WAWA3:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/waza3.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case VOICE_IDX_WAZA4:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "voices/waza4.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_STATIC, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      }
}

void CBasePlayer :: Pain( void )
{
   if (m_LastHitGroup == HITGROUP_HEAD)
      {
      EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "player/bhit_head.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));         
      return;
      }
   switch (rand() & 3)
      {
      case 0:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "player/bhit_flesh-1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      case 1:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "player/bhit_flesh-2.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      default:
         EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "player/bhit_flesh-3.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
         break;
      }
}

Vector VecVelocityForDamage(float flDamage)
{
	Vector vec(RANDOM_FLOAT(-100,100), RANDOM_FLOAT(-100,100), RANDOM_FLOAT(200,300));

	if (flDamage > -50)
		vec = vec * 0.7;
	else if (flDamage > -200)
		vec = vec * 2;
	else
		vec = vec * 10;
	
	return vec;
}

int TrainSpeed(int iSpeed, int iMax)
{
	float fSpeed, fMax;
	int iRet = 0;

	fMax = (float)iMax;
	fSpeed = iSpeed;

	fSpeed = fSpeed/fMax;

	if (iSpeed < 0)
		iRet = TRAIN_BACK;
	else if (iSpeed == 0)
		iRet = TRAIN_NEUTRAL;
	else if (fSpeed < 0.33)
		iRet = TRAIN_SLOW;
	else if (fSpeed < 0.66)
		iRet = TRAIN_MEDIUM;
	else
		iRet = TRAIN_FAST;

	return iRet;
}

void CBasePlayer :: DeathSound( void )
{
	// water death sounds
	if (pev->waterlevel == 3)
	   {
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/h2odeath.wav", 1, ATTN_NONE);
		return;
	   }

	// temporarily using pain sounds for death sounds
   switch (rand() & 3)
	   {
	   case 0: 
		   EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/die1.wav", 1, ATTN_NORM);
		   break;
	   case 1: 
		   EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/die2.wav", 1, ATTN_NORM);
		   break;
	   case 2: 
		   EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/die3.wav", 1, ATTN_NORM);
		   break;
	   case 3: 
		   EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/die4.wav", 1, ATTN_NORM);
		   break;
	   }
}

// override takehealth
// bitsDamageType indicates type of damage healed. 
int CBasePlayer :: TakeHealth( float flHealth, int bitsDamageType )
{
	return CBaseMonster :: TakeHealth (flHealth, bitsDamageType);
}

Vector CBasePlayer :: GetGunPosition( )
{
	Vector origin;
	origin = pev->origin + pev->view_ofs;
	return origin;
}

//=========================================================
// TraceAttack
//=========================================================
void CBasePlayer :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
   CBaseEntity *xe = NULL;
   CBasePlayer *xp = NULL;
   if (!pev || !ptr)
      return;
	if ( pev->takedamage )
	   {
      m_LastHitGroup = ptr->iHitgroup;
		switch ( ptr->iHitgroup )
		   {
		   case HITGROUP_GENERIC:
			   break;
		   case HITGROUP_HEAD:
			   flDamage *= aw.headshot;
			   pev->punchangle.x = flDamage * -0.3;
			   if (pev->punchangle.x < -12) pev->punchangle.x = -12;
			   pev->punchangle.z = flDamage * RANDOM_FLOAT(-1,1);
			   if (pev->punchangle.z < -9) pev->punchangle.z = -9;
			   else if (pev->punchangle.z > 9) pev->punchangle.z = 9;
			   break;
		   case HITGROUP_CHEST:
			   //flDamage = 3;
			   pev->punchangle.x = flDamage * -0.1;
			   if (pev->punchangle.x < -4)
				   pev->punchangle.x = -4;
			   break;
		   case HITGROUP_STOMACH:
			   flDamage *= .6f;
			   pev->punchangle.x = flDamage * 0.1;
			   if (pev->punchangle.x > 4)
				   pev->punchangle.x = 4;
			   break;
		   case HITGROUP_LEFTARM:
		   case HITGROUP_RIGHTARM:
			   flDamage *= .4f;
			   break;
		   case HITGROUP_LEFTLEG:
		   case HITGROUP_RIGHTLEG:
			   flDamage *= .2f;
			   break;
		   default:
			   break;
		   }

      xe = CBaseEntity::Instance( pevAttacker );
      if (xe && xe->IsPlayer())
         {
         xp = (CBasePlayer *) xe;
         if (xp == this || xp->m_iTeam == m_iTeam || (pev->flags & FL_GODMODE))
            return;
         }
      SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);
		TraceBleed( flDamage, vecDir, ptr, bitsDamageType );
      AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
      }
}

/*
	Take some damage.  
	NOTE: each call to TakeDamage with bitsDamageType set to a time-based damage
	type will cause the damage time countdown to be reset.  Thus the ongoing effects of poison, radiation
	etc are implemented with subsequent calls to TakeDamage using DMG_GENERIC.
*/
int CBasePlayer :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	// have suit diagnose the problem - ie: report damage type
	int bitsDamage = bitsDamageType;
	int fTookDamage;
	float flHealthPrev = pev->health;
   int i;

   if (!pevInflictor || !pevAttacker)
      return 0;

	if (bitsDamageType & DMG_BLAST)
      flDamage *= aw.grenade;

	if (!IsAlive())
		return 0;

   if (m_iJumpCount > 2)
      flDamage *= 3;
	
	CBaseEntity *pAttacker = CBaseEntity::Instance(pevAttacker);

   if (pAttacker && pAttacker->IsPlayer())
      {
      CBasePlayer *pPlayer = (CBasePlayer *)pAttacker;
      if (m_iTeam == pPlayer->m_iTeam) return 0;
      pPlayer->stat.Hits++;
      }

	// keep track of amount of damage last sustained
	m_lastDamageAmount = flDamage;

	// this cast to INT is critical!!! If a player ends up with 0.5 health, the engine will get that
	// as an int (zero) and think the player is dead! (this will incite a clientside screentilt, etc)
	fTookDamage = CBaseMonster::TakeDamage(pevInflictor, pevAttacker, (int)flDamage, bitsDamageType);

	// reset damage time countdown for each type of time based damage player just sustained

   for (i = 0; i < CDMG_TIMEBASED; i++)
		if (bitsDamageType & (DMG_PARALYZE << i))
		   m_rgbTimeBasedDamage[i] = 0;


   if (flDamage > 10)
      Pain();

   flDamage /= 6;
   if (flDamage > aw.kickback) flDamage = aw.kickback;   

	pev->punchangle.x = RANDOM_FLOAT(-flDamage, flDamage);
   pev->punchangle.y = RANDOM_FLOAT(-flDamage, flDamage);

   flDamage *= 12.8f;

   UTIL_ScreenFade( this, Vector(255,0,0), .4, 0, flDamage, FFADE_IN | FFADE_MODULATE);

	return fTookDamage;
}

void DropPrimaryWeapon(CBasePlayer * player);
void CBasePlayer::PackDeadPlayerItems( void )
{
   DropPrimaryWeapon(this);
   m_iPrimary = 0;
   m_iSecondary = 0;
   RemoveAllItems( TRUE );
}

void CBasePlayer::RemoveAllItems( BOOL removeSuit )
{
	int i;
	if (m_pActiveItem)
	   {
		m_pActiveItem->Holster( );
		m_pActiveItem = NULL;
	   }

	m_pLastItem = NULL;

	CBasePlayerItem *pPendingItem;
	for (i = 0; i < MAX_ITEM_TYPES; i++)
	   {
		m_pActiveItem = m_rgpPlayerItems[i];
		while (m_pActiveItem)
		   {
			pPendingItem = m_pActiveItem->m_pNext;         
			m_pActiveItem->Drop( );
			m_pActiveItem = pPendingItem;
		   }
		m_rgpPlayerItems[i] = NULL;
	   }
	m_pActiveItem = NULL;

	pev->viewmodel		= 0;
	pev->weaponmodel	= 0;
	
	if ( removeSuit )
		pev->weapons = 0;
	else
		pev->weapons &= ~WEAPON_ALLWEAPONS;

	for ( i = 0; i < MAX_AMMO_SLOTS;i++)
		m_rgAmmo[i] = 0;

	UpdateClientData();
	// send Selected Weapon Message to our client
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pev );
		WRITE_BYTE(0);
		WRITE_BYTE(0);
		WRITE_BYTE(0);
	MESSAGE_END();
}

/*
 * GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
 *
 * ENTITY_METHOD(PlayerDie)
 */
entvars_t *g_pevLastInflictor;  // Set in combat.cpp.  Used to pass the damage inflictor for death messages.
								// Better solution:  Add as parameter to all Killed() functions.
void DropPlayerNANA(CBasePlayer *);
void RespawnDeadPlayers( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
void CBasePlayer::Killed( entvars_t *pevAttacker, int iGib )
{
	CSound *pSound;

	g_pGameRules->PlayerKilled( this, pevAttacker, g_pevLastInflictor );

   if ( m_pTank != NULL )
	   {
		m_pTank->Use( this, this, USE_OFF, 0 );
		m_pTank = NULL;
	   }

	// this client isn't going to be thinking for a while, so reset the sound until they respawn
	pSound = CSoundEnt::SoundPointerForIndex( CSoundEnt::ClientSoundIndex( edict() ) );
	if ( pSound )
		pSound->Reset();

   SetAnimation( PLAYER_DIE );
   pev->flags &= ~FL_DUCKING;
	
	pev->modelindex = g_ulModelIndexPlayer;    // don't use eyes

	pev->deadflag		= DEAD_DYING;
   pev->solid			= SOLID_NOT;
   pev->takedamage   = DAMAGE_NO;
	pev->movetype		= MOVETYPE_TOSS;
	ClearBits(pev->flags, FL_ONGROUND);
	if (pev->velocity.z < 10)
		pev->velocity.z += RANDOM_FLOAT(0,300);

	// clear out the suit message cache so we don't keep chattering
	//SetSuitUpdate(NULL, FALSE, 0);

	// send "health" update message to zero
	m_iClientHealth = 0;
	MESSAGE_BEGIN( MSG_ONE, gmsgHealth, NULL, pev );
		WRITE_BYTE( m_iClientHealth );
	MESSAGE_END();

	// Tell Ammo Hud that the player is dead
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pev );
		WRITE_BYTE(0);
		WRITE_BYTE(0XFF);
		WRITE_BYTE(0xFF);
	MESSAGE_END();

	// reset FOV
	m_iFOV = m_iClientFOV = 0;

	MESSAGE_BEGIN( MSG_ONE, gmsgSetFOV, NULL, pev );
		WRITE_BYTE(0);
	MESSAGE_END();


	// UNDONE: Put this in, but add FFADE_PERMANENT and make fade time 8.8 instead of 4.12
   if (aw.fadeout)
      {
      //aw.fadeout*4
	   UTIL_ScreenFade(this, Vector(0,0,0), .3, 400, 255, FFADE_OUT/* | FFADE_MODULATE*/ );
      }

	DeathSound();
	
	pev->angles.x = 0;
	pev->angles.z = 0;
	pev->angles.y = 0;
   m_fDeadTime = gpGlobals->time + 2;

	SetThink(PlayerDeathThink);
	pev->nextthink = gpGlobals->time + 0.1;

   if (m_iNANA)
      {
		m_iNANA = 0;
		pev->body = 0;
      DropPlayerNANA(this);
      //GiveNamedItem( "item_backpack" );
      SendBossStatus(0, this);
      UTIL_LogPrintf( "%s died as boss\n", STRING(pev->netname) );
      }  

   RespawnDeadPlayers( this, pevAttacker, g_pevLastInflictor );

   if (!(m_iClientFlag & CF_JUST_CONNECTED))
      g_pGameRules->CheckDeadRoundEnd();
}

// Set the activity based on an event or current state
void CBasePlayer::SetAnimation( PLAYER_ANIM playerAnim )
{
	int animDesired;
	float speed;
	char szAnim[64], *szSeq = NULL;

	speed = pev->velocity.Length2D();

	if (pev->flags & FL_FROZEN)
	   {
		speed = 0;
		playerAnim = PLAYER_IDLE;
	   }

	switch (playerAnim) 
	   {
	   case PLAYER_JUMP:
		   m_IdealActivity = ACT_WALK;//ACT_HOP;
		   break;
	   
	   case PLAYER_SUPERJUMP:
		   m_IdealActivity = ACT_WALK; //ACT_LEAP;
		   break;
	   
	   case PLAYER_DIE:
		   m_IdealActivity = ACT_DIESIMPLE;
		   break;

      case PLAYER_RELOAD:
         m_IdealActivity = ACT_RELOAD;
         break;

	   case PLAYER_ATTACK1:	
		   switch( m_Activity )
		      {
		      case ACT_HOVER:
		      case ACT_SWIM:
		      ////case ACT_HOP:
		      case ACT_LEAP:
		      case ACT_DIESIMPLE:
			      m_IdealActivity = m_Activity;
			      break;
		      default:
			      m_IdealActivity = ACT_RANGE_ATTACK1;
			      break;
		      }
		   break;
	   case PLAYER_IDLE:
	   case PLAYER_WALK:
		   if ( !FBitSet( pev->flags, FL_ONGROUND ) && (m_Activity == ACT_HOP || m_Activity == ACT_LEAP) )	// Still jumping
		      {
			   m_IdealActivity = m_Activity;
		      }
		   else if ( pev->waterlevel > 1 )
		      {
			   if ( speed == 0 )
				   m_IdealActivity = ACT_HOVER;
			   else
				   m_IdealActivity = ACT_SWIM;
		      }
		   else
		      {
			   m_IdealActivity = ACT_WALK;
		      }
		   break;
	   }

	switch (m_IdealActivity)
	   {
	   case ACT_HOVER:
	   case ACT_LEAP:
	   case ACT_SWIM:
	   //case ACT_HOP:
	   default:
		   if ( m_Activity == m_IdealActivity)
			   return;
		   m_Activity = m_IdealActivity;

		   animDesired = LookupActivity( m_Activity );
		   // Already using the desired animation?
		   if (pev->sequence == animDesired)
			   return;

		   pev->gaitsequence = 0;
		   pev->sequence		= animDesired;
		   pev->frame			= 0;
		   ResetSequenceInfo( );
		   return;

      case ACT_RELOAD:
		   if ( FBitSet( pev->flags, FL_DUCKING ) )	// crouching
		      strcpy( szAnim, "crouch_reload_" );
         else
            strcpy( szAnim, "ref_reload_" );
		   strcat( szAnim, m_szAnimExtention );
		   animDesired = LookupSequence( szAnim );
		   if (animDesired == -1)
			   animDesired = 0;
		   if ( pev->sequence != animDesired || !m_fSequenceLoops )
			   pev->frame = 0;
		   if (!m_fSequenceLoops)
 			   pev->effects |= EF_NOINTERP;
	      m_Activity = m_IdealActivity;
	      pev->sequence = animDesired;
		   ResetSequenceInfo( );
		   break;

      case ACT_DIESIMPLE:
         /* FLIFLA: HARD CODED in client */
         /* Email: Topaz@gangwars.net
            for more info */
         if ( FBitSet( pev->flags, FL_DUCKING ) )
            szSeq = "crouch_die";
         else
            {  
            switch (m_LastHitGroup)
               {
               case HITGROUP_HEAD: szSeq = "head"; break;
               case HITGROUP_CHEST:
                  switch  (rand() & 3) 
                     { case 0: szSeq = "death1"; break; 
                       case 1: szSeq = "death2"; break; 
                       default: szSeq = "death3"; break;}
                  break;
		         case HITGROUP_STOMACH:
                  if (rand() & 1) szSeq = "gutshot"; else szSeq = "back";
                  break;
		         case HITGROUP_LEFTARM: szSeq = "left"; break;
		         case HITGROUP_RIGHTARM: szSeq = "right"; break;
		         case HITGROUP_LEFTLEG: szSeq = "left"; break;
		         case HITGROUP_RIGHTLEG: szSeq = "right"; break;
		         default: szSeq = "death1";
               }
            }

		   animDesired = LookupSequence(szSeq);
		   if (animDesired == -1) animDesired = 0;
		   //if ( pev->sequence != animDesired || !m_fSequenceLoops ) 
            pev->frame = 0;
		   if (!m_fSequenceLoops) pev->effects |= EF_NOINTERP;
	      m_Activity = m_IdealActivity;
	      pev->sequence = animDesired;
		   ResetSequenceInfo( );
         pev->gaitsequence = animDesired;            
         return;
		   break;

	   case ACT_RANGE_ATTACK1:
		   if ( FBitSet( pev->flags, FL_DUCKING ) )	// crouching
			   strcpy( szAnim, "crouch_shoot_" );
		   else
			   strcpy( szAnim, "ref_shoot_" );
		   strcat( szAnim, m_szAnimExtention );
		   animDesired = LookupSequence( szAnim );
		   if (animDesired == -1)
			   animDesired = 0;

		   //if ( pev->sequence != animDesired || !m_fSequenceLoops )
			   pev->frame = 0;

		   if (!m_fSequenceLoops)
			   pev->effects |= EF_NOINTERP;

		   m_Activity = m_IdealActivity;

		   pev->sequence		= animDesired;
		   ResetSequenceInfo( );
		   break;

      case ACT_WALK:
		   if ((m_Activity != ACT_RANGE_ATTACK1           
            && m_Activity != ACT_RELOAD
            && m_Activity != ACT_DIESIMPLE) || m_fSequenceFinished)
		      {
			   if ( FBitSet( pev->flags, FL_DUCKING ) )	// crouching
				   strcpy( szAnim, "crouch_aim_" );
			   else
				   strcpy( szAnim, "ref_aim_" );
			   strcat( szAnim, m_szAnimExtention );
			   animDesired = LookupSequence( szAnim );
			   if (animDesired == -1)
				   animDesired = 0;
			   m_Activity = ACT_WALK;
            pev->gaitsequence = animDesired;
		      }
		   else
		      {
			   animDesired = pev->sequence;
		      }
         break;

      }

   if (!szSeq)
      {
      if (playerAnim == PLAYER_JUMP)
         {
         pev->gaitsequence	= LookupActivity( ACT_HOP );
         pev->gaitsequence	= LookupSequence( "jump" );
         }
      else if ( FBitSet( pev->flags, FL_DUCKING ) )
	      {
	      if ( speed == 0 || !FBitSet(pev->flags, FL_ONGROUND))
		      pev->gaitsequence	= LookupActivity( ACT_CROUCHIDLE );
	      else
		      pev->gaitsequence	= LookupActivity( ACT_CROUCH );
	      }
      else if (!FBitSet(pev->flags, FL_ONGROUND) && !IsOnLadder())
         {
         pev->gaitsequence	= LookupSequence( "jump" );
         pev->gaitsequence	= LookupActivity( ACT_HOP );
         }
      else if ( speed > 180 )
	      {
         if (!FBitSet(pev->flags, FL_ONGROUND) && !IsOnLadder())
            pev->gaitsequence	= LookupSequence( "jump" );
         else
            pev->gaitsequence	= LookupActivity( ACT_RUN );
	      }
      else if (speed > 0)
	      {
         if (!FBitSet(pev->flags, FL_ONGROUND) && !IsOnLadder())
            pev->gaitsequence	= LookupSequence( "jump" );
         else
		      pev->gaitsequence	= LookupActivity( ACT_WALK );
	      }
      else
	      {
	      pev->gaitsequence	= LookupSequence( "idle1" );
	      }
      }

	// Already using the desired animation?
	if (pev->sequence == animDesired)
		return;

	pev->sequence		= animDesired;
	pev->frame			= 0;
	ResetSequenceInfo( );
}


/*
===========
WaterMove
============
*/
#define AIRTIME	12		// lung full of air lasts this many seconds

void CBasePlayer::WaterMove()
{
	int air;

	if (pev->movetype == MOVETYPE_NOCLIP)
		return;

	if (pev->health < 0)
		return;

	if (pev->waterlevel != 3) 
	{
		// not underwater
		
		// play 'up for air' sound
		if (pev->air_finished < gpGlobals->time)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade1.wav", 1, ATTN_NORM);
		else if (pev->air_finished < gpGlobals->time + 9)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade2.wav", 1, ATTN_NORM);

		pev->air_finished = gpGlobals->time + AIRTIME;
		pev->dmg = 2;

		// if we took drowning damage, give it back slowly
		if (m_idrowndmg > m_idrownrestored)
		{
			// set drowning damage bit.  hack - dmg_drownrecover actually
			// makes the time based damage code 'give back' health over time.
			// make sure counter is cleared so we start count correctly.
			
			// NOTE: this actually causes the count to continue restarting
			// until all drowning damage is healed.

			m_bitsDamageType |= DMG_DROWNRECOVER;
			m_bitsDamageType &= ~DMG_DROWN;
			m_rgbTimeBasedDamage[itbd_DrownRecover] = 0;
		}

	}
	else
	{	// fully under water
		// stop restoring damage while underwater
		m_bitsDamageType &= ~DMG_DROWNRECOVER;
		m_rgbTimeBasedDamage[itbd_DrownRecover] = 0;

		if (pev->air_finished < gpGlobals->time)		// drown!
		{
			if (pev->pain_finished < gpGlobals->time)
			{
				// take drowning damage
				pev->dmg += 1;
				if (pev->dmg > 5)
					pev->dmg = 5;
				TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), pev->dmg, DMG_DROWN);
				pev->pain_finished = gpGlobals->time + 1;
				
				// track drowning damage, give it back when
				// player finally takes a breath

				m_idrowndmg += pev->dmg;
			} 
		}
		else
		{
			m_bitsDamageType &= ~DMG_DROWN;
		}
	}

	if (!pev->waterlevel)
	   {
		if (FBitSet(pev->flags, FL_INWATER))
		   {       
			ClearBits(pev->flags, FL_INWATER);
		   }
		return;
	   }
	
	// make bubbles
	air = (int)(pev->air_finished - gpGlobals->time);
	if (!RANDOM_LONG(0,0x1f) && RANDOM_LONG(0,AIRTIME-1) >= air)
	   {
		switch (RANDOM_LONG(0,3))
			{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim1.wav", 0.8, ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim2.wav", 0.8, ATTN_NORM); break;
			case 2:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim3.wav", 0.8, ATTN_NORM); break;
			case 3:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim4.wav", 0.8, ATTN_NORM); break;
		   }
	   }

	if (pev->watertype == CONTENT_LAVA)		// do damage
	   {
		if (pev->dmgtime < gpGlobals->time)
			TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), 10 * pev->waterlevel, DMG_BURN);
	   }
	
	if (!FBitSet(pev->flags, FL_INWATER))
	   {
		SetBits(pev->flags, FL_INWATER);
		pev->dmgtime = 0;
	   }
	
	if (!FBitSet(pev->flags, FL_WATERJUMP))
		pev->velocity = pev->velocity - 0.8 * pev->waterlevel * gpGlobals->frametime * pev->velocity;
}


// TRUE if the player is attached to a ladder
BOOL CBasePlayer::IsOnLadder( void )
{ 
	return (pev->movetype == MOVETYPE_FLY);
}

// 
// PlayerUse - handles USE keypress
//
#define	PLAYER_SEARCH_RADIUS	(float)64

void CBasePlayer::PlayerUse ( void )
{
	// Was use pressed or released?
	if ( ! ((pev->button | m_afButtonPressed | m_afButtonReleased) & IN_USE) )
		return;

	// Hit Use on a train?
	if ( m_afButtonPressed & IN_USE )
	{
		if ( m_pTank != NULL )
		{
			// Stop controlling the tank
			// TODO: Send HUD Update
			m_pTank->Use( this, this, USE_OFF, 0 );
			m_pTank = NULL;
			return;
		}
		else
		{
			if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
			{
				m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
				m_iTrain = TRAIN_NEW|TRAIN_OFF;
				return;
			}
			else
			{	// Start controlling the train!
				CBaseEntity *pTrain = CBaseEntity::Instance( pev->groundentity );

				if ( pTrain && !(pev->button & IN_JUMP) && FBitSet(pev->flags, FL_ONGROUND) && (pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) && pTrain->OnControls(pev) )
				{
					m_afPhysicsFlags |= PFLAG_ONTRAIN;
					m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
					m_iTrain |= TRAIN_NEW;
					EMIT_SOUND( ENT(pev), CHAN_ITEM, "plats/train_use1.wav", 0.8, ATTN_NORM);
					return;
				}
			}
		}
	}

	CBaseEntity *pObject = NULL;
	CBaseEntity *pClosest = NULL;
	Vector		vecLOS;
	float flMaxDot = VIEW_FIELD_NARROW;
	float flDot;

	UTIL_MakeVectors ( pev->v_angle );// so we know which way we are facing
	
	while ((pObject = UTIL_FindEntityInSphere( pObject, pev->origin, PLAYER_SEARCH_RADIUS )) != NULL)
	{

		if (pObject->ObjectCaps() & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE))
		{
			// !!!PERFORMANCE- should this check be done on a per case basis AFTER we've determined that
			// this object is actually usable? This dot is being done for every object within PLAYER_SEARCH_RADIUS
			// when player hits the use key. How many objects can be in that area, anyway? (sjb)
			vecLOS = (VecBModelOrigin( pObject->pev ) - (pev->origin + pev->view_ofs));
			
			// This essentially moves the origin of the target to the corner nearest the player to test to see 
			// if it's "hull" is in the view cone
			vecLOS = UTIL_ClampVectorToBox( vecLOS, pObject->pev->size * 0.5 );
			
			flDot = DotProduct (vecLOS , gpGlobals->v_forward);
			if (flDot > flMaxDot )
			{// only if the item is in front of the user
				pClosest = pObject;
				flMaxDot = flDot;
//				ALERT( at_console, "%s : %f\n", STRING( pObject->pev->classname ), flDot );
			}
//			ALERT( at_console, "%s : %f\n", STRING( pObject->pev->classname ), flDot );
		}
	}
	pObject = pClosest;

	// Found an object
	if (pObject )
	{
		//!!!UNDONE: traceline here to prevent USEing buttons through walls			
		int caps = pObject->ObjectCaps();

		//if ( m_afButtonPressed & IN_USE )
			//EMIT_SOUND( ENT(pev), CHAN_ITEM, "common/wpn_select.wav", 0.4, ATTN_NORM);

		if ( ( (pev->button & IN_USE) && (caps & FCAP_CONTINUOUS_USE) ) ||
			 ( (m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE|FCAP_ONOFF_USE)) ) )
		{
			if ( caps & FCAP_CONTINUOUS_USE )
				m_afPhysicsFlags |= PFLAG_USING;

			pObject->Use( this, this, USE_SET, 1 );
		}
		// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
		else if ( (m_afButtonReleased & IN_USE) && (pObject->ObjectCaps() & FCAP_ONOFF_USE) )	// BUGBUG This is an "off" use
		{
			pObject->Use( this, this, USE_SET, 0 );
		}
	}
   /*
	else
	{
		//if ( m_afButtonPressed & IN_USE )
			//EMIT_SOUND( ENT(pev), CHAN_ITEM, "common/wpn_denyselect.wav", 0.4, ATTN_NORM);
	}
   */
}



void CBasePlayer::Jump()
{
	Vector		vecWallCheckDir;// direction we're tracing a line to find a wall when walljumping
	Vector		vecAdjustedVelocity;
	Vector		vecSpot;
	TraceResult	tr;
	
	if (FBitSet(pev->flags, FL_WATERJUMP))
		return;
	
	if (pev->waterlevel >= 2)
		return;

	// jump velocity is sqrt( height * gravity * 2)

	// If this isn't the first frame pressing the jump button, break out.
	if ( !FBitSet( m_afButtonPressed, IN_JUMP ) )
		return;         // don't pogo stick

	if ( !(pev->flags & FL_ONGROUND) || !pev->groundentity )
		return;


// many features in this function use v_forward, so makevectors now.
	UTIL_MakeVectors (pev->angles);

	SetAnimation( PLAYER_JUMP );

	// If you're standing on a conveyor, add it's velocity to yours (for momentum)
	entvars_t *pevGround = VARS(pev->groundentity);
	if ( pevGround && (pevGround->flags & FL_CONVEYOR) )
		pev->velocity = pev->velocity + pev->basevelocity;

   if (aw.anti_cheats & GW_AC_BUNNYHOP)
      {
      if (m_fLastJumped > gpGlobals->time)
         {
         m_iJumpCount++;
         if (m_iJumpCount > 3)
            {
            EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "player/pl_fallpain3.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
            }
         }
      m_fLastJumped = gpGlobals->time + 1.2;
      }
}



// This is a glorious hack to find free space when you've crouched into some solid space
// Our crouching collisions do not work correctly for some reason and this is easier
// than fixing the problem :(
void FixPlayerCrouchStuck( edict_t *pPlayer )
{
	TraceResult trace;

	// Move up as many as 18 pixels if the player is stuck.
	for ( int i = 0; i < 18; i++ )
	   {
		UTIL_TraceHull( pPlayer->v.origin, pPlayer->v.origin, dont_ignore_monsters, head_hull, pPlayer, &trace );
		if ( trace.fStartSolid )
			pPlayer->v.origin.z ++;
		else
			break;
	   }
}

void CBasePlayer::Duck( )
{
	if (pev->button & IN_DUCK) 
		SetAnimation( PLAYER_WALK );
}

//
// ID's player as such.
//
int  CBasePlayer::Classify ( void )
{
	return CLASS_PLAYER;
}


#if 0
void CBasePlayer::CheckWeapon(void)
{
	// play a weapon idle anim if it's time!
	if ( gpGlobals->time > m_flTimeWeaponIdle )
	{
		WeaponIdle ( );
	}
}
#endif


// play a footstep if it's time - this will eventually be frame-based. not time based.

#define STEP_CONCRETE	0		// default step sound
#define STEP_METAL		1		// metal floor
#define STEP_DIRT		2		// dirt, sand, rock
#define STEP_VENT		3		// ventillation duct
#define STEP_GRATE		4		// metal grating
#define STEP_TILE		5		// floor tiles
#define STEP_SLOSH		6		// shallow liquid puddle
#define STEP_WADE		7		// wading in liquid
#define STEP_LADDER		8		// climbing ladder

// Play correct step sound for material we're on or in

void CBasePlayer :: PlayStepSound(int step, float fvol)
{
	static int iSkipStep = 0;
   if (m_afPhysicsFlags & PFLAG_OBSERVER)
      return;

   return;

	if ( !g_pGameRules->PlayFootstepSounds( this, fvol ) )
		return;
   //&& !pl->m_iStealth
	// irand - 0,1 for right foot, 2,3 for left foot
	// used to alternate left and right foot
	int irand = RANDOM_LONG(0,1) + (m_iStepLeft * 2);

	m_iStepLeft = !m_iStepLeft;

	switch (step)
	   {
	   default:
	   case STEP_CONCRETE:
		   switch (irand)
		      {
		      // right foot
		      case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_step1.wav", fvol, ATTN_NORM);	break;
		      case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_step3.wav", fvol, ATTN_NORM);	break;
		      // left foot
		      case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_step2.wav", fvol, ATTN_NORM);	break;
		      case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_step4.wav", fvol, ATTN_NORM);	break;
		      }
		   break;
	   case STEP_METAL:
		   switch(irand)
		      {
		      // right foot
		      case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_metal1.wav", fvol, ATTN_NORM);	break;
		      case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_metal3.wav", fvol, ATTN_NORM);	break;
		      // left foot
		      case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_metal2.wav", fvol, ATTN_NORM);	break;
		      case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_metal4.wav", fvol, ATTN_NORM);	break;
		      }
		   break;
	   case STEP_DIRT:
		   switch(irand)
		      {
		      // right foot
		      case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_dirt1.wav", fvol, ATTN_NORM);	break;
		      case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_dirt3.wav", fvol, ATTN_NORM);	break;
		      // left foot
		      case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_dirt2.wav", fvol, ATTN_NORM);	break;
		      case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_dirt4.wav", fvol, ATTN_NORM);	break;
		      }
		   break;
	   case STEP_VENT:
		   switch(irand)
		      {
		      // right foot
		      case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_duct1.wav", fvol, ATTN_NORM);	break;
		      case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_duct3.wav", fvol, ATTN_NORM);	break;
		      // left foot
		      case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_duct2.wav", fvol, ATTN_NORM);	break;
		      case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_duct4.wav", fvol, ATTN_NORM);	break;
		      }
		   break;
	   case STEP_GRATE:
		   switch(irand)
		      {
		      // right foot
		      case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_grate1.wav", fvol, ATTN_NORM);	break;
		      case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_grate3.wav", fvol, ATTN_NORM);	break;
		      // left foot
		      case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_grate2.wav", fvol, ATTN_NORM);	break;
		      case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_grate4.wav", fvol, ATTN_NORM);	break;
		      }
		   break;
	   case STEP_TILE:
		   switch(irand)
		      {
		      // right foot
		      case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_tile1.wav", fvol, ATTN_NORM);	break;
		      case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_tile3.wav", fvol, ATTN_NORM);	break;
		      // left foot
		      case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_tile2.wav", fvol, ATTN_NORM);	break;
		      case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_tile4.wav", fvol, ATTN_NORM);	break;
		      }
		   break;
	   case STEP_SLOSH:
		   switch(irand)
		      {
		      // right foot
		      case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_slosh1.wav", fvol, ATTN_NORM);	break;
		      case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_slosh3.wav", fvol, ATTN_NORM);	break;
		      // left foot
		      case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_slosh2.wav", fvol, ATTN_NORM);	break;
		      case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_slosh4.wav", fvol, ATTN_NORM);	break;
		      }
		   break;
	   case STEP_WADE:
		   if ( iSkipStep == 0 )
		      {
			      iSkipStep++;
			      break;
		      }

		   if ( iSkipStep++ == 3 )
		      {
			      iSkipStep = 0;
		      }

		   switch (irand)
		      {
		      // right foot
		      case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_wade1.wav", fvol, ATTN_NORM);	break;
		      case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_wade2.wav", fvol, ATTN_NORM);	break;
		      // left foot
		      case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_wade3.wav", fvol, ATTN_NORM);	break;
		      case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_wade4.wav", fvol, ATTN_NORM);	break;
		      }
		   break;
	   case STEP_LADDER:
		   switch(irand)
		      {
		      // right foot
		      case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_ladder1.wav", fvol, ATTN_NORM);	break;
		      case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_ladder3.wav", fvol, ATTN_NORM);	break;
		      // left foot
		      case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_ladder2.wav", fvol, ATTN_NORM);	break;
		      case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_ladder4.wav", fvol, ATTN_NORM);	break;
		      }
		   break;
	   }
}	

// Simple mapping from texture type character to step type

int MapTextureTypeStepType(char chTextureType)
{
   switch (chTextureType)
	   {
	   default:
	   case CHAR_TEX_CONCRETE:	return STEP_CONCRETE;	
	   case CHAR_TEX_METAL: return STEP_METAL;	
	   case CHAR_TEX_DIRT: return STEP_DIRT;	
	   case CHAR_TEX_VENT: return STEP_VENT;	
	   case CHAR_TEX_GRATE: return STEP_GRATE;	
	   case CHAR_TEX_TILE: return STEP_TILE;
	   case CHAR_TEX_SLOSH: return STEP_SLOSH;
	   }
}

// Play left or right footstep based on material player is on or in

void CBasePlayer :: UpdateStepSound( void )
{
	int	fWalking;
	float fvol;
	char szbuffer[64];
	const char *pTextureName;
	Vector start, end;
	float rgfl1[3];
	float rgfl2[3];
	Vector knee;
	Vector feet;
	Vector center;
	float height;
	float speed;
	float velrun;
	float velwalk;
	float flduck;
	int	fLadder;
	int step;

	if (gpGlobals->time <= m_flTimeStepSound)
		return;

	if (pev->flags & FL_FROZEN)
		return;

	speed = pev->velocity.Length();

	// determine if we are on a ladder
	fLadder = IsOnLadder();

	// UNDONE: need defined numbers for run, walk, crouch, crouch run velocities!!!!	
	if (FBitSet(pev->flags, FL_DUCKING) || fLadder)
	   {
		velwalk = 60;		// These constants should be based on cl_movespeedkey * cl_forwardspeed somehow
		velrun = 80;		// UNDONE: Move walking to server
		flduck = 0.1;
	   }
	else
	   {
		velwalk = 120;
		velrun = 210;
		flduck = 0.0;
	   }

	// ALERT (at_console, "vel: %f\n", vecVel.Length());
	
	// if we're on a ladder or on the ground, and we're moving fast enough,
	// play step sound.  Also, if m_flTimeStepSound is zero, get the new
	// sound right away - we just started moving in new level.

	if ((fLadder || FBitSet (pev->flags, FL_ONGROUND)) && pev->velocity != g_vecZero 
		&& (speed >= velwalk || !m_flTimeStepSound))
	   {
		SetAnimation( PLAYER_WALK );
		
		fWalking = speed < velrun;		

		center = knee = feet = (pev->absmin + pev->absmax) * 0.5;
		height = pev->absmax.z - pev->absmin.z;

		knee.z = pev->absmin.z + height * 0.2;
		feet.z = pev->absmin.z;

		// find out what we're stepping in or on...
		if (fLadder)
		   {
			step = STEP_LADDER;
			fvol = 0.35;
			m_flTimeStepSound = gpGlobals->time + 0.35;
		   }
		else if ( UTIL_PointContents ( knee ) == CONTENTS_WATER )
		   {
			step = STEP_WADE;
			fvol = 0.65;
			m_flTimeStepSound = gpGlobals->time + 0.6;
		   }
		else if (UTIL_PointContents ( feet ) == CONTENTS_WATER )
		   {
			step = STEP_SLOSH;
			fvol = fWalking ? 0.2 : 0.5;
			m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4 : gpGlobals->time + 0.3;		
		   }
		else
		   {
			// find texture under player, if different from current texture, 
			// get material type

			start = end = center;							// center point of player BB
			start.z = end.z = pev->absmin.z;				// copy zmin
			start.z += 4.0;									// extend start up
			end.z -= 24.0;									// extend end down
			
			start.CopyToArray(rgfl1);
			end.CopyToArray(rgfl2);

			pTextureName = TRACE_TEXTURE( ENT( pev->groundentity), rgfl1, rgfl2 );
			if ( pTextureName )
			   {
				// strip leading '-0' or '{' or '!'
				if (*pTextureName == '-')
					pTextureName += 2;
				if (*pTextureName == '{' || *pTextureName == '!')
					pTextureName++;
				
				if (_strnicmp(pTextureName, m_szTextureName, CBTEXTURENAMEMAX-1))
				   {
					// current texture is different from texture player is on...
					// set current texture
					strcpy(szbuffer, pTextureName);
					szbuffer[CBTEXTURENAMEMAX - 1] = 0;
					strcpy(m_szTextureName, szbuffer);
					
					// ALERT ( at_aiconsole, "texture: %s\n", m_szTextureName );

					// get texture type
					m_chTextureType = TEXTURETYPE_Find(m_szTextureName);	
				   }
			   }
			
			step = MapTextureTypeStepType(m_chTextureType);

			switch (m_chTextureType)
			   {
			   default:
			   case CHAR_TEX_CONCRETE:						
				   fvol = fWalking ? 0.2 : 0.5;
				   m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4 : gpGlobals->time + 0.3;
				   break;

			   case CHAR_TEX_METAL:	
				   fvol = fWalking ? 0.2 : 0.5;
				   m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4 : gpGlobals->time + 0.3;
				   break;

			   case CHAR_TEX_DIRT:	
				   fvol = fWalking ? 0.25 : 0.55;
				   m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4 : gpGlobals->time + 0.3;
				   break;

			   case CHAR_TEX_VENT:	
				   fvol = fWalking ? 0.4 : 0.7;
				   m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4 : gpGlobals->time + 0.3;
				   break;

			   case CHAR_TEX_GRATE:
				   fvol = fWalking ? 0.2 : 0.5;
				   m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4 : gpGlobals->time + 0.3;
				   break;

			   case CHAR_TEX_TILE:	
				   fvol = fWalking ? 0.2 : 0.5;
				   m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4 : gpGlobals->time + 0.3;
				   break;

			   case CHAR_TEX_SLOSH:
				   fvol = fWalking ? 0.2 : 0.5;
				   m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4 : gpGlobals->time + 0.3;
				   break;
			   }
		   }
		
      m_flTimeStepSound += flduck; // slower step time if ducking

		// play the sound
		// 35% volume if ducking
		if ( pev->flags & FL_DUCKING )
			fvol *= 0.35;
	   }
}


#define CLIMB_SHAKE_FREQUENCY	22	   // how many frames in between screen shakes when climbing
#define	MAX_CLIMB_SPEED		200	// fastest vertical climbing speed possible
#define	CLIMB_SPEED_DEC		15	   // climbing deceleration rate
#define	CLIMB_PUNCH_X			-7    // how far to 'punch' client X axis when climbing
#define CLIMB_PUNCH_Z			7	   // how far to 'punch' client Z axis when climbing

void SendTitle(char *print_string, int team);
void CBasePlayer::PreThink(void)
{
	int buttonsChanged = (m_afButtonLast ^ pev->button);	// These buttons have changed this frame
	
	// Debounced button codes for pressed/released
	// UNDONE: Do we need auto-repeat?
	m_afButtonPressed =  buttonsChanged & pev->button;		// The changed ones still down are "pressed"
	m_afButtonReleased = buttonsChanged & (~pev->button);	// The ones not down are "released"

	if ( g_fGameOver )
      {
      g_pGameRules->PlayerThink( this );
		return;
      }

   /*
   char xxx[256];
   sprintf(xxx, "Edicts %i", NUMBER_OF_ENTITIES());
   SendTitle(xxx, 2);
   */

   if (stat.ping_lasttime < gpGlobals->time)
      {
      int packet_loss, ping;
      PLAYER_CNX_STATS( edict(), &ping, &packet_loss);
      stat.ping_samples++;
      stat.ping_sum += ping;
      stat.ping_lasttime = gpGlobals->time + 5.0f;
      }

	UTIL_MakeVectors(pev->v_angle);             // is this still used?
	ItemPreFrame( );
	WaterMove();


   if (m_pActiveItem)
      if (m_pActiveItem->m_iId == WEAPON_SG550)
         {
         float speed = pev->velocity.Length();
         if (speed > 140)
            {
            SG550 *xxx = (SG550 *)m_pActiveItem->GetWeaponPtr();
            if (xxx && xxx->m_fInZoom)
               xxx->SecondaryAttack();
            }
         else if (speed > 5)
            {
            SG550 *xxx = (SG550 *)m_pActiveItem->GetWeaponPtr();
            if (xxx && xxx->m_fInZoom == 2)
               {
               xxx->m_fInZoom = 0;
               xxx->SecondaryAttack();
               }
            }
         }    


   
   if (!(pev->flags & FL_ONGROUND))
      {
      if (m_pActiveItem)
         {
         if (m_pActiveItem->m_iId == WEAPON_SG552)
            {
            SG552 *xxx = (SG552 *)m_pActiveItem->GetWeaponPtr();
            if (xxx && xxx->m_fInZoom)
               xxx->SecondaryAttack();
            }
         else if (m_pActiveItem->m_iId == WEAPON_AUG)
            {
            AUG *xxx = (AUG *)m_pActiveItem->GetWeaponPtr();
            if (xxx && xxx->m_fInZoom)
               xxx->SecondaryAttack();
            }
         else if (m_pActiveItem->m_iId == WEAPON_SG550)
            {
            SG550 *xxx = (SG550 *)m_pActiveItem->GetWeaponPtr();
            if (xxx && xxx->m_fInZoom)
               {
               xxx->m_fInZoom = 2;
               xxx->SecondaryAttack();
               }
            }
         }
      }

	if ( g_pGameRules && g_pGameRules->FAllowFlashlight() )
		m_iHideHUD &= ~HIDEHUD_FLASHLIGHT;
	else
		m_iHideHUD |= HIDEHUD_FLASHLIGHT;

   if (m_iClientFlag & CF_JUST_CONNECTED)
      {
      if (!m_iTeam && m_fShowTeamSelectTime < gpGlobals->time)
         {
         DisplayTeamMenu(this);
         m_fShowTeamSelectTime = m_fShowTeamSelectTime + 2;
         }
      }
   else
      {
      if (m_iTagged & TAG_HURT)
         {
         if (m_fShowTeamSelectTime < gpGlobals->time)
            {
            m_fShowTeamSelectTime = gpGlobals->time + .4f;
            TraceResult tr;
            Vector vecDir = gpGlobals->v_forward;
            ClearMultiDamage();
            m_LastHitGroup = HITGROUP_HEAD;
            AddMultiDamage( pev, this, 4, DMG_BULLET | DMG_NEVERGIB);
            ApplyMultiDamage(pev, pev); 
            }
         }
      }

   if (pev->flags & FL_GODMODE)
      {
      if (m_flGodModeTime < gpGlobals->time)
         pev->flags &= ~(FL_GODMODE);
      }
   
   if (m_flEnableControlTime)
      {
      if (m_flEnableControlTime > gpGlobals->time)
         {
         if (!m_iKilled)
            {
            if (!(m_iAdminTorture & ADMIN_TORTURE_ROUNDSTART))  
               {
               m_iAdminTorture |= ADMIN_TORTURE_ROUNDSTART;
               m_iLastMaxSpeed = -1;
               }
            }
         }
      else
         {
         m_iAdminTorture &= ~ADMIN_TORTURE_ROUNDSTART;
         m_iLastMaxSpeed = -1;
         m_flEnableControlTime = 0;	      
         }
      }

	UpdateClientData();
	
   if (pev->iuser1)
	   {
		Observer_HandleButtons();
		pev->impulse = 0;
		return;
	   }

	CheckTimeBasedDamage();
	//CheckSuitUpdate();


	if (pev->deadflag >= DEAD_DYING)
	   {
		PlayerDeathThink();
		return;
	   }

   // Sync time left between client and server:
   if (m_iTeam)
      {
      if (m_fNextUpdateTimeLeft < gpGlobals->time)
         {
         UpdateTimeIcon(this);
         m_fNextUpdateTimeLeft = gpGlobals->time + 10;
         }
      if (m_flNextID < gpGlobals->time)
         SendId(this);
      }

   if (m_iJumpCount && m_fLastJumped + 5 < gpGlobals->time)
      m_iJumpCount = 0;

   // Sync "player near armory" icon:
   if (m_fNextUpdateArmory < gpGlobals->time)
      {
      UpdateArmoryIcon(this);
      m_fNextUpdateArmory = gpGlobals->time + 1.5f;
      }

	// So the correct flags get sent to client asap.
	//
	if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
		pev->flags |= FL_ONTRAIN;
	else 
		pev->flags &= ~FL_ONTRAIN;

	// Train speed control
	if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
	   {
		CBaseEntity *pTrain = CBaseEntity::Instance( pev->groundentity );
		float vel;
		
		if ( !pTrain )
		   {
			TraceResult trainTrace;
			// Maybe this is on the other side of a level transition
			UTIL_TraceLine( pev->origin, pev->origin + Vector(0,0,-38), ignore_monsters, ENT(pev), &trainTrace );

			// HACKHACK - Just look for the func_tracktrain classname
			if ( trainTrace.flFraction != 1.0 && trainTrace.pHit )
			pTrain = CBaseEntity::Instance( trainTrace.pHit );


			if ( !pTrain || !(pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) || !pTrain->OnControls(pev) )
			   {
				//ALERT( at_error, "In train mode with no train!\n" );
				m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
				m_iTrain = TRAIN_NEW|TRAIN_OFF;
				return;
			   }
		   }
		else if ( !FBitSet( pev->flags, FL_ONGROUND ) || FBitSet( pTrain->pev->spawnflags, SF_TRACKTRAIN_NOCONTROL ) || (pev->button & (IN_MOVELEFT|IN_MOVERIGHT) ) )
		   {
			// Turn off the train if you jump, strafe, or the train controls go dead
			m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
			m_iTrain = TRAIN_NEW|TRAIN_OFF;
			return;
		   }

		pev->velocity = g_vecZero;
		vel = 0;
		if ( m_afButtonPressed & IN_FORWARD )
		   {
			vel = 1;
			pTrain->Use( this, this, USE_SET, (float)vel );
		   }
		else if ( m_afButtonPressed & IN_BACK )
		   {
			vel = -1;
			pTrain->Use( this, this, USE_SET, (float)vel );
		   }
		if (vel)
		   {
			m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
			m_iTrain |= TRAIN_ACTIVE|TRAIN_NEW;
		   }
	   } 
   else if (m_iTrain & TRAIN_ACTIVE)
		m_iTrain = TRAIN_NEW; // turn off train

	if (pev->button & IN_JUMP)
		Jump();

	// If trying to duck, already ducked, or in the process of ducking
	if ((pev->button & IN_DUCK) || FBitSet(pev->flags,FL_DUCKING) || (m_afPhysicsFlags & PFLAG_DUCKING) )
		Duck();
   else
	   // play a footstep if it's time - this will eventually be frame-based. not time based.
	   UpdateStepSound();

	if ( !FBitSet ( pev->flags, FL_ONGROUND ) )
		m_flFallVelocity = -pev->velocity.z;

	// StudioFrameAdvance( );//!!!HACKHACK!!! Can't be hit by traceline when not animating?

	// Clear out ladder pointer
	m_hEnemy = NULL;
	if ( m_afPhysicsFlags & PFLAG_ONBARNACLE )
		pev->velocity = g_vecZero;
}

/* Time based Damage works as follows: 
	1) There are several types of timebased damage:

		#define DMG_PARALYZE		(1 << 14)	// slows affected creature down
		#define DMG_DROWNRECOVER	(1 << 18)	// drown recovery
		#define DMG_SLOWBURN		(1 << 20)	// in an oven

	2) A new hit inflicting tbd restarts the tbd counter - each monster has an 8bit counter,
		per damage type. The counter is decremented every second, so the maximum time 
		an effect will last is 255/60 = 4.25 minutes.  Of course, staying within the radius
		of a damaging effect like fire, nervegas, radiation will continually reset the counter to max.

	3) Every second that a tbd counter is running, the player takes damage.  The damage
		is determined by the type of tdb.  
			Paralyze		- 1/2 movement rate, 30 second duration.
			Nervegas		- 5 points per second, 16 second duration = 80 points max dose.
			Poison			- 2 points per second, 25 second duration = 50 points max dose.
			Radiation		- 1 point per second, 50 second duration = 50 points max dose.
			Drown			- 5 points per second, 2 second duration.
			Acid/Chemical	- 5 points per second, 10 second duration = 50 points max.
			Burn			- 10 points per second, 2 second duration.
			Freeze			- 3 points per second, 10 second duration = 30 points max.

	4) Certain actions or countermeasures counteract the damaging effects of tbds:

		Armor/Heater/Cooler - Chemical(acid),burn, freeze all do damage to armor power, then to body
							- recharged by suit recharger
		Air In Lungs		- drowning damage is done to air in lungs first, then to body
							- recharged by poking head out of water
							- 10 seconds if swiming fast
		Air In SCUBA		- drowning damage is done to air in tanks first, then to body
							- 2 minutes in tanks. Need new tank once empty.
		Radiation Syringe	- Each syringe full provides protection vs one radiation dosage
		Antitoxin Syringe	- Each syringe full provides protection vs one poisoning (nervegas or poison).
		Health kit			- Immediate stop to acid/chemical, fire or freeze damage.
		Radiation Shower	- Immediate stop to radiation damage, acid/chemical or fire damage.
		
	
*/

// If player is taking time based damage, continue doing damage to player -
// this simulates the effect of being poisoned, gassed, dosed with radiation etc -
// anything that continues to do damage even after the initial contact stops.
// Update all time based damage counters, and shut off any that are done.

// The m_bitsDamageType bit MUST be set if any damage is to be taken.
// This routine will detect the initial on value of the m_bitsDamageType
// and init the appropriate counter.  Only processes damage every second.

//#define PARALYZE_DURATION	30		// number of 2 second intervals to take damage
//#define PARALYZE_DAMAGE		0.0		// damage to take each 2 second interval

//#define NERVEGAS_DURATION	16
//#define NERVEGAS_DAMAGE		5.0

//#define POISON_DURATION		25
//#define POISON_DAMAGE		2.0

//#define RADIATION_DURATION	50
//#define RADIATION_DAMAGE	1.0

//#define ACID_DURATION		10
//#define ACID_DAMAGE			5.0

//#define SLOWBURN_DURATION	2
//#define SLOWBURN_DAMAGE		1.0

//#define SLOWFREEZE_DURATION	1.0
//#define SLOWFREEZE_DAMAGE	3.0

/* */


void CBasePlayer::CheckTimeBasedDamage() 
{
	int i;
	BYTE bDuration = 0;

	static float gtbdPrev = 0.0;

	if (!(m_bitsDamageType & DMG_TIMEBASED))
		return;

	// only check for time based damage approx. every 2 seconds
	if (abs(gpGlobals->time - m_tbdPrev) < 2.0)
		return;
	
	m_tbdPrev = gpGlobals->time;

	for (i = 0; i < CDMG_TIMEBASED; i++)
	   {
		// make sure bit is set for damage type
		if (m_bitsDamageType & (DMG_PARALYZE << i))
		   {
			switch (i)
			   {
			   case itbd_Paralyze:
				   // UNDONE - flag movement as half-speed
				   bDuration = PARALYZE_DURATION;
				   break;
			   case itbd_DrownRecover:
				   // NOTE: this hack is actually used to RESTORE health
				   // after the player has been drowning and finally takes a breath
				   if (m_idrowndmg > m_idrownrestored)
				      {
					   int idif = min(m_idrowndmg - m_idrownrestored, 10);

					   TakeHealth(idif, DMG_GENERIC);
					   m_idrownrestored += idif;
				      }
				   bDuration = 4;	// get up to 5*10 = 50 points back
				   break;
			   case itbd_SlowBurn:
				   bDuration = SLOWBURN_DURATION;
				   break;
			   default:
				   bDuration = 0;
			   }

			if (m_rgbTimeBasedDamage[i])
			   {
				// decrement damage duration, detect when done.
				if (!m_rgbTimeBasedDamage[i] || --m_rgbTimeBasedDamage[i] == 0)
				   {
					m_rgbTimeBasedDamage[i] = 0;
					// if we're done, clear damage bits
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
				   }
			   }
			else
				// first time taking this damage type - init damage duration
				m_rgbTimeBasedDamage[i] = bDuration;
		   }
	}
}

/*
THE POWER SUIT

The Suit provides 3 main functions: Protection, Notification and Augmentation. 
Some functions are automatic, some require power. 
The player gets the suit shortly after getting off the train in C1A0 and it stays
with him for the entire game.

Protection

	Heat/Cold
		When the player enters a hot/cold area, the heating/cooling indicator on the suit 
		will come on and the battery will drain while the player stays in the area. 
		After the battery is dead, the player starts to take damage. 
		This feature is built into the suit and is automatically engaged.
	Radiation Syringe
		This will cause the player to be immune from the effects of radiation for N seconds. Single use item.
	Anti-Toxin Syringe
		This will cure the player from being poisoned. Single use item.
	Health
		Small (1st aid kits, food, etc.)
		Large (boxes on walls)
	Armor
		The armor works using energy to create a protective field that deflects a
		percentage of damage projectile and explosive attacks. After the armor has been deployed,
		it will attempt to recharge itself to full capacity with the energy reserves from the battery.
		It takes the armor N seconds to fully charge. 

Notification (via the HUD)

x	Health
x	Ammo  
x	Automatic Health Care
		Notifies the player when automatic healing has been engaged. 
x	Geiger counter
		Classic Geiger counter sound and status bar at top of HUD 
		alerts player to dangerous levels of radiation. This is not visible when radiation levels are normal.
x	Poison
	Armor
		Displays the current level of armor. 

Augmentation 

	Reanimation (w/adrenaline)
		Causes the player to come back to life after he has been dead for 3 seconds. 
		Will not work if player was gibbed. Single use.
	Long Jump
		Used by hitting the ??? key(s). Caused the player to further than normal.
	SCUBA	
		Used automatically after picked up and after player enters the water. 
		Works for N seconds. Single use.	
	
Things powered by the battery

	Armor		
		Uses N watts for every M units of damage.
	Heat/Cool	
		Uses N watts for every second in hot/cold area.
	Long Jump	
		Uses N watts for every jump.
	Alien Cloak	
		Uses N watts for each use. Each use lasts M seconds.
	Alien Shield	
		Augments armor. Reduces Armor drain by one half
 
*/

// if in range of radiation source, ping geiger counter
#define GEIGERDELAY 0.25

/*
================
CheckSuitUpdate

Play suit update if it's time
================
*/

#define SUITUPDATETIME	3.5
#define SUITFIRSTUPDATETIME 0.1

void CBasePlayer::CheckSuitUpdate()
{

}
 
// add sentence to suit playlist queue. if fgroup is true, then
// name is a sentence group (HEV_AA), otherwise name is a specific
// sentence name ie: !HEV_AA0.  If iNoRepeat is specified in
// seconds, then we won't repeat playback of this word or sentence
// for at least that number of seconds.
void CBasePlayer::SetSuitUpdate(char *name, int fgroup, int iNoRepeatTime)
{

}

/*
================
CheckPowerups

Check for turning off powerups

GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
================
*/
	static void
CheckPowerups(entvars_t *pev)
{
	if (pev->health <= 0)
		return;

	pev->modelindex = g_ulModelIndexPlayer;    // don't use eyes
}


//=========================================================
// UpdatePlayerSound - updates the position of the player's
// reserved sound slot in the sound list.
//=========================================================
void CBasePlayer :: UpdatePlayerSound ( void )
{
	int iBodyVolume;
	int iVolume;
	CSound *pSound;

	pSound = CSoundEnt::SoundPointerForIndex( CSoundEnt :: ClientSoundIndex( edict() ) );

	if ( !pSound )
	   {
		ALERT ( at_console, "Client lost reserved sound!\n" );
		return;
	   }

	pSound->m_iType = bits_SOUND_NONE;

	// now calculate the best target volume for the sound. If the player's weapon
	// is louder than his body/movement, use the weapon volume, else, use the body volume.
	
	if ( FBitSet ( pev->flags, FL_ONGROUND ) )
	   {	
		iBodyVolume = pev->velocity.Length(); 

		// clamp the noise that can be made by the body, in case a push trigger,
		// weapon recoil, or anything shoves the player abnormally fast. 
		if ( iBodyVolume > 512 )
			iBodyVolume = 512;
	   }
	else
		iBodyVolume = 0;

	if ( pev->button & IN_JUMP )
		iBodyVolume += 100;

   // convert player move speed and actions into sound audible by monsters.
	if ( m_iWeaponVolume > iBodyVolume )
	   {
		m_iTargetVolume = m_iWeaponVolume;

		// OR in the bits for COMBAT sound if the weapon is being louder than the player. 
		pSound->m_iType |= bits_SOUND_COMBAT;
	   }
	else
		m_iTargetVolume = iBodyVolume;

	// decay weapon volume over time so bits_SOUND_COMBAT stays set for a while
	m_iWeaponVolume -= 250 * gpGlobals->frametime;
	if ( m_iWeaponVolume < 0 )
		iVolume = 0;


	// if target volume is greater than the player sound's current volume, we paste the new volume in 
	// immediately. If target is less than the current volume, current volume is not set immediately to the
	// lower volume, rather works itself towards target volume over time. This gives monsters a much better chance
	// to hear a sound, especially if they don't listen every frame.
	iVolume = pSound->m_iVolume;

	if ( m_iTargetVolume > iVolume )
	   {
		iVolume = m_iTargetVolume;
	   }
	else if ( iVolume > m_iTargetVolume )
	   {
		iVolume -= 250 * gpGlobals->frametime;

		if ( iVolume < m_iTargetVolume )
			iVolume = 0;
	   }

	if ( m_fNoPlayerSound )
		// debugging flag, lets players move around and shoot without monsters hearing.
		iVolume = 0;

	if ( gpGlobals->time > m_flStopExtraSoundTime )
		// since the extra sound that a weapon emits only lasts for one client frame, we keep that sound around for a server frame or two 
		// after actual emission to make sure it gets heard.
		m_iExtraSoundTypes = 0;

	if ( pSound )
	   {
		pSound->m_vecOrigin = pev->origin;
		pSound->m_iType |= ( bits_SOUND_PLAYER | m_iExtraSoundTypes );
		pSound->m_iVolume = iVolume;
	   }

	// keep track of virtual muzzle flash
	m_iWeaponFlash -= 256 * gpGlobals->frametime;
	if (m_iWeaponFlash < 0)
		m_iWeaponFlash = 0;

	UTIL_MakeVectors ( pev->angles );
	gpGlobals->v_forward.z = 0;

	// Below are a couple of useful little bits that make it easier to determine just how much noise the 
	// player is making. 
	// UTIL_ParticleEffect ( pev->origin + gpGlobals->v_forward * iVolume, g_vecZero, 255, 25 );
	//ALERT ( at_console, "%d/%d\n", iVolume, m_iTargetVolume );
}


void CBasePlayer::PostThink()
{
	if ( g_fGameOver )
		goto pt_end;

	if (!IsAlive())
		goto pt_end;

	if (pev->iuser1)
		goto pt_end;


	// Handle Tank controlling
	if ( m_pTank != NULL )
	   { // if they've moved too far from the gun,  or selected a weapon, unuse the gun
		if ( m_pTank->OnControls( pev ) && !pev->weaponmodel )
		   {  
			m_pTank->Use( this, this, USE_SET, 2 );	// try fire the gun
		   }
		else
		   {  // they've moved off the platform
			m_pTank->Use( this, this, USE_OFF, 0 );
			m_pTank = NULL;
		   }
	   }

   // do weapon stuff
   ItemPostFrame( );

   // player landed/jumped down
	if ( (FBitSet(pev->flags, FL_ONGROUND)) && (pev->health > 0) && m_flFallVelocity >= PLAYER_FALL_PUNCH_THRESHHOLD )
	   {
		float fvol = 0.5;

		if (pev->watertype == CONTENT_WATER)
		   {
		   }
		else if ( m_flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED )
		   {
			float flFallDamage = g_pGameRules->FlPlayerFallDamage( this );

			if ( flFallDamage > pev->health )
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/bodysplat.wav", 1, ATTN_NORM);
         else
            {
            if (rand() & 1)
               EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_fallpain1.wav", 1, ATTN_NORM);
            else
               EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_fallpain2.wav", 1, ATTN_NORM);
            }


			if ( flFallDamage > 0 )
			   {
				TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), flFallDamage, DMG_FALL ); 
				pev->punchangle.x = 0;
			   }

			fvol = 1.0;
		   }
		else if ( m_flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED / 2 )
		   {
			fvol = 0.85;
		   }
		else if ( m_flFallVelocity < PLAYER_MIN_BOUNCE_SPEED )
		   {
			fvol = 0;
		   }

		if ( fvol > 0.0 )
		   {
			m_flTimeStepSound = 0;
			UpdateStepSound();
		   }

		if ( IsAlive() )
			SetAnimation( PLAYER_WALK );
      }

	if (FBitSet(pev->flags, FL_ONGROUND))
	   {		
		m_flFallVelocity = 0;
	   }

	// select the proper animation for the player character	
	if ( IsAlive() )
	   {
		if (!pev->velocity.x && !pev->velocity.y)
			SetAnimation( PLAYER_IDLE );
		else if ((pev->velocity.x || pev->velocity.y) && (FBitSet(pev->flags, FL_ONGROUND)))
			SetAnimation( PLAYER_WALK );
		else if (pev->waterlevel > 1)
			SetAnimation( PLAYER_WALK );
	   }

	StudioFrameAdvance( );
	CheckPowerups(pev);

	UpdatePlayerSound();

	// Track button info so we can detect 'pressed' and 'released' buttons next frame
	m_afButtonLast = pev->button;

pt_end:
#if defined( CLIENT_WEAPONS )
	// Decay timers on weapons
	// go through all of the weapons and make a list of the ones to pack
	for ( int i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	   {
		if ( m_rgpPlayerItems[ i ] )
		   {
			CBasePlayerItem *pPlayerItem = m_rgpPlayerItems[ i ];

			while ( pPlayerItem )
			   {
				CBasePlayerWeapon *gun;

				gun = (CBasePlayerWeapon *)pPlayerItem->GetWeaponPtr();
				
				if ( gun && gun->UseDecrement() )
				   {
					gun->m_flNextPrimaryAttack		= max( gun->m_flNextPrimaryAttack - gpGlobals->frametime, -1.0 );
					gun->m_flNextSecondaryAttack	= max( gun->m_flNextSecondaryAttack - gpGlobals->frametime, -0.001 );

					if ( gun->m_flTimeWeaponIdle != 1000 )
					   {
						gun->m_flTimeWeaponIdle		= max( gun->m_flTimeWeaponIdle - gpGlobals->frametime, -0.001 );
					   }
				   }

				pPlayerItem = pPlayerItem->m_pNext;
			   }
		   }
	   }

	m_flNextAttack -= gpGlobals->frametime;
	if ( m_flNextAttack < -0.001 )
		m_flNextAttack = -0.001;
#else
	return;
#endif
}


// checks if the spot is clear of players
BOOL IsSpawnPointValid( CBaseEntity *pPlayer, CBaseEntity *pSpot )
{
	CBaseEntity *ent = NULL;

	if ( !pSpot->IsTriggered( pPlayer ) )
	{
		return FALSE;
	}
   return TRUE;
}


DLL_GLOBAL CBaseEntity	*g_pLastSpawn;
inline int FNullEnt( CBaseEntity *ent ) { return (ent == NULL) || FNullEnt( ent->edict() ); }

/*
============
EntSelectSpawnPoint

Returns the entity to spawn at

USES AND SETS GLOBAL g_pLastSpawn
============
*/
edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer )
{
	CBaseEntity *pSpot;
	edict_t		*player;

	player = pPlayer->edict();

// choose a info_player_deathmatch point
	if (g_pGameRules->IsCoOp())
	   {
		pSpot = UTIL_FindEntityByClassname( g_pLastSpawn, "info_player_coop");
		if ( !FNullEnt(pSpot) )
			goto ReturnSpot;
		pSpot = UTIL_FindEntityByClassname( g_pLastSpawn, "info_player_start");
		if ( !FNullEnt(pSpot) ) 
			goto ReturnSpot;
	   }
	else if ( g_pGameRules->IsDeathmatch() )
	   {
		pSpot = g_pLastSpawn;
		// Randomize the start spot
		for ( int i = RANDOM_LONG(1,5); i > 0; i-- )
			pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );
		if ( FNullEnt( pSpot ) )  // skip over the null point
			pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );

		CBaseEntity *pFirstSpot = pSpot;

		do 
		   {
			if ( pSpot )
			   {
				// check if pSpot is valid
				if ( IsSpawnPointValid( pPlayer, pSpot ) )
				   {
					if ( pSpot->pev->origin == Vector( 0, 0, 0 ) )
					   {
						pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );
						continue;
					   }

					// if so, go to pSpot
					goto ReturnSpot;
				   }
			   }
			// increment pSpot
			pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );
		   } while ( pSpot != pFirstSpot ); // loop if we're not back to the start

		// we haven't found a place to spawn yet,  so kill any guy at the first spawn point and spawn there
		if ( !FNullEnt( pSpot ) )
		   {
			CBaseEntity *ent = NULL;
			while ( (ent = UTIL_FindEntityInSphere( ent, pSpot->pev->origin, 128 )) != NULL )
			   {
				// if ent is a client, kill em (unless they are ourselves)
				if ( ent->IsPlayer() && !(ent->edict() == player) )
					ent->TakeDamage( VARS(INDEXENT(0)), VARS(INDEXENT(0)), 300, DMG_GENERIC );
			   }
			goto ReturnSpot;
		   }
	   }

	// If startspot is set, (re)spawn there.
	if ( FStringNull( gpGlobals->startspot ) || !strlen(STRING(gpGlobals->startspot)))
	   {
		pSpot = UTIL_FindEntityByClassname(NULL, "info_player_start");
		if ( !FNullEnt(pSpot) )
			goto ReturnSpot;
	   }
	else
	   {
		pSpot = UTIL_FindEntityByTargetname( NULL, STRING(gpGlobals->startspot) );
		if ( !FNullEnt(pSpot) )
			goto ReturnSpot;
	   }

ReturnSpot:
	if ( FNullEnt( pSpot ) )
	   {
		ALERT(at_error, "PutClientInServer: no info_player_start on level");
		return INDEXENT(0);
	   }

	g_pLastSpawn = pSpot;
	return pSpot->edict();
}


void CBasePlayer::SpectatorSpawn( void )
{
  
	pev->classname		= MAKE_STRING("player");
   m_afPhysicsFlags  = 0;
	pev->flags			= FL_SPECTATOR;
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NOCLIP;
	pev->deadflag		= DEAD_DEAD;
	pev->takedamage	= DAMAGE_NO;
	pev->health			= 100;
	pev->armorvalue	= 0;
	pev->max_health	= pev->health;
	pev->air_finished	= gpGlobals->time + 12;
	pev->dmg			   = 2;				// initial water damage
	pev->effects		= 0;
	pev->dmg_take		= 0;
	pev->dmg_save		= 0;
	pev->friction		= 1.0;
	pev->gravity		= 1.0;
	m_bitsHUDDamage	= -1;
	m_bitsDamageType	= 0;
	m_fLongJump			= FALSE;// no longjump module. 
	//g_engfuncs.pfnSetPhysicsKeyValue( edict(), "slj", "0" );
	//g_engfuncs.pfnSetPhysicsKeyValue( edict(), "hl", "1" );

	m_iFOV				= 0;// init field of view.
	m_iClientFOV		= -1; // make sure fov reset is sent

	m_flNextDecalTime	= 0;// let this player decal as soon as he spawns.

	m_flTimeStepSound	= 0;
	m_iStepLeft = 0;
	m_flFieldOfView		= 0.5;// some monsters use this to determine whether or not the player is looking at them.

	m_bloodColor	= BLOOD_COLOR_RED;
	m_flNextAttack	= UTIL_WeaponTimeBase();
	StartSneaking();

	m_iFlashBattery = 99;
	m_flFlashLightTime = 1; // force first message

// dont let uninitialized value here hurt the player
	m_flFallVelocity = 0;

	if (!g_pGameRules->GetPlayerSpawnSpot( this ))
      return;

   SET_MODEL(ENT(pev), "models/player.mdl");
   g_ulModelIndexPlayer = pev->modelindex;
	pev->sequence		= LookupActivity( ACT_IDLE );

	if ( FBitSet(pev->flags, FL_DUCKING) ) 
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

   pev->view_ofs = VEC_VIEW;
	Precache();
	m_HackedGunPos		= Vector( 0, 32, 0 );

	if ( m_iPlayerSound == SOUNDLIST_EMPTY )
	   {
		ALERT ( at_console, "Couldn't alloc player sound slot!\n" );
	   }

	m_fNoPlayerSound = FALSE;// normal sound behavior.

	m_pLastItem = NULL;
	m_fInitHUD = TRUE;
	m_iClientHideHUD = -1;  // force this to be recalculated
	m_fWeapon = FALSE;
	m_pClientActiveItem = NULL;
	m_iClientBattery = -1;

	// reset all ammo values to 0
	for ( int i = 0; i < MAX_AMMO_SLOTS; i++ )
	   {
		m_rgAmmo[i] = 0;
		m_rgAmmoLast[i] = 0;  // client ammo values also have to be reset  (the death hud clear messages does on the client side)
	   }

	m_lastx = m_lasty = 0;
   
	//g_pGameRules->PlayerSpawn( this );
}

void CBasePlayer::Spawn( void )
{
   if (m_iGroup < 0)
      return;
	pev->classname		= MAKE_STRING("player");
   pev->flags			= FL_CLIENT;
   m_afPhysicsFlags	= 0;
   pev->armorvalue	= 0;
   pev->air_finished	= gpGlobals->time + 12;
	pev->dmg			   = 8;				// initial water damage
	pev->effects		= EF_NODRAW;
	pev->dmg_take		= 0;
	pev->dmg_save		= 0;
	pev->friction		= 1.0;
	pev->gravity		= 1.0;
	m_bitsHUDDamage	= -1;
	m_bitsDamageType	= 0;
	m_fLongJump			= FALSE;// no longjump module. 
   m_iWONID = g_engfuncs.pfnGetPlayerWONId(edict());
   strcpy(m_iSteamID, g_engfuncs.pfnGetPlayerAuthId(edict()));
   if (aw.fadeout)
      {
      if (m_iKilled)
         {
	      UTIL_ScreenFade(this, Vector(0,0,0), 1, 0, 255, FFADE_IN/* | FFADE_MODULATE*/ );
         }
      else
         {
	      UTIL_ScreenFade(this, Vector(0,0,0), 3, 0, 255, FFADE_IN/* | FFADE_MODULATE*/ );
         }  
      }

   if (m_iClientFlag & CF_JUST_CONNECTED || !m_iTeam)
      {
	   pev->solid			= SOLID_NOT;
	   pev->movetype		= MOVETYPE_NONE;
	   pev->takedamage	= DAMAGE_NO;
	   pev->health			= 0;
	   pev->max_health	= pev->health;
      }
   else
      {
	   pev->solid			= SOLID_SLIDEBOX;
	   pev->movetype		= MOVETYPE_WALK;
	   pev->takedamage	= DAMAGE_YES;
	   pev->health			= 100;
	   pev->max_health	= pev->health;
      }

   pev->nextthink = -1;


	//g_engfuncs.pfnSetPhysicsKeyValue( edict(), "slj", "0" );
	//g_engfuncs.pfnSetPhysicsKeyValue( edict(), "hl", "1" );

	m_iFOV				= 0;// init field of view.
	m_iClientFOV		= -1; // make sure fov reset is sent
   pev->body         = 0;

	m_flNextDecalTime	= 0;// let this player decal as soon as he spawns.

	m_flTimeStepSound	= 0;
	m_iStepLeft = 0;
	m_flFieldOfView		= 0.5;// some monsters use this to determine whether or not the player is looking at them.

	m_bloodColor	= BLOOD_COLOR_RED;
	m_flNextAttack	= UTIL_WeaponTimeBase();
	StartSneaking();

	m_iFlashBattery = 99;
	m_flFlashLightTime = 1; // force first message

// dont let uninitialized value here hurt the player
	m_flFallVelocity = 0;

	//g_pGameRules->GetPlayerSpawnSpot( this );
	if (!g_pGameRules->GetPlayerSpawnSpot( this ))
      return;

	pev->sequence = LookupActivity( ACT_IDLE );

	if ( FBitSet(pev->flags, FL_DUCKING) ) 
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

   pev->view_ofs = VEC_VIEW;
	Precache();
	m_HackedGunPos		= Vector( 0, 32, 0 );

	if ( m_iPlayerSound == SOUNDLIST_EMPTY )
	   {
		ALERT ( at_console, "Couldn't alloc player sound slot!\n" );
	   }

	m_fNoPlayerSound = FALSE;// normal sound behavior.

   m_iClientHideHUD = -1;

	if (pev->deadflag > DEAD_NO)
	   {
	   m_fInitHUD = TRUE;
		for ( int i = 0; i < MAX_AMMO_SLOTS; i++ )
		   {
			m_rgAmmo[i] = 0;
			m_rgAmmoLast[i] = 0;  
		   }
	   m_iClientBattery = -1;
	   m_pClientActiveItem = NULL;
	   m_fWeapon = FALSE;
      m_iGrenades = 0;
      //m_iPrimary = 0;
      //m_iSecondary = 0;
      if (m_iPrimary) m_iMaxSpeed = w_stat[m_iPrimary].speed;
      else m_iMaxSpeed = 300;
	   }

   m_lastx = m_lasty = 0;
   m_fNextUpdateArmory = 0;
   m_fNextUpdateTimeLeft = 0;
   m_fShowTeamSelectTime = 0;

   SET_MODEL(ENT(pev), "models/player.mdl");
   g_ulModelIndexPlayer = pev->modelindex;

   if (m_iNANA > 0)
      {
		m_iNANA = 0;
      SendBossStatus(0, this);
      }

   pev->body = 0;

	if (!m_iTeam || m_iClientFlag & CF_JUST_CONNECTED)
	   {
      SetThink(PlayerDeathThink);
	   pev->nextthink = gpGlobals->time + 0.1;
      pev->deadflag = DEAD_DEAD;
      }
   else
      {
	   pev->effects = 0;
      StopObserver();
	   g_pGameRules->PlayerSpawn( this );
      m_iGrenade = 0;
      pev->deadflag = DEAD_NO;
      }    


   m_ilast_ID = 0;
   MESSAGE_BEGIN(MSG_ONE, gmsgMoney, NULL, pev);
   	WRITE_SHORT( m_iMoney );
   MESSAGE_END();
   if (m_iTeam)
      {
      m_flEnableControlTime = map_stat.g_fRoundStartTime + 5.0f;
      m_flGodModeTime = gpGlobals->time + 10.0f;
      pev->flags |= FL_GODMODE;
      if (m_iClientFlag & CF_NEED_SERVER_VARIABLES)
         {
         SendServerVars();
         m_iClientFlag &= ~CF_NEED_SERVER_VARIABLES;
         }
      }
   m_iStealth = 0;
   m_iNANA = 0;
	// reset FOV
	m_iFOV = m_iClientFOV = 0;
   m_iNextMoney = 0;
   m_iTagged = 0;
   m_iJumpCount = 0;


	pev->fov = m_iFOV;
	MESSAGE_BEGIN( MSG_ONE, gmsgSetFOV, NULL, pev );
		WRITE_BYTE(0);
	MESSAGE_END();
}

void CBasePlayer :: Precache( void )
{
	m_flgeigerRange = 1;
	m_igeigerRangePrev = 1;
	m_bitsDamageType = 0;
	m_bitsHUDDamage = -1;
	m_iClientBattery = -1;
	m_iTrain = TRAIN_NEW;
	LinkUserMessages();
	m_iUpdateTime = 5;  // won't update for 1/2 a second
	if ( gInitHUD )
		m_fInitHUD = TRUE;
}

int CBasePlayer::Save( CSave &save )
{
	return 0;
}

void CBasePlayer::RenewItems(void)
{

}

int CBasePlayer::Restore( CRestore &restore )
{
   return 0;
}

void CBasePlayer::SelectNextItem( int iItem )
{
	CBasePlayerItem *pItem;

	pItem = m_rgpPlayerItems[ iItem ];
	
	if (!pItem)
		return;

	if (pItem == m_pActiveItem)
	   {
		// select the next one in the chain
		pItem = m_pActiveItem->m_pNext; 
		if (! pItem)
		   {
			return;
		   }

		CBasePlayerItem *pLast;
		pLast = pItem;
		while (pLast->m_pNext)
			pLast = pLast->m_pNext;

		// relink chain
		pLast->m_pNext = m_pActiveItem;
		m_pActiveItem->m_pNext = NULL;
		m_rgpPlayerItems[ iItem ] = pItem;
	   }

	// FIX, this needs to queue them up and delay
	if (m_pActiveItem)
	   {
		m_pActiveItem->Holster( );
	   }
	
	m_pActiveItem = pItem;

	if (m_pActiveItem)
	   {
		m_pActiveItem->Deploy( );
		m_pActiveItem->UpdateItemInfo( );
	   }
}

void CBasePlayer::SelectItem(const char *pstr)
{
	if (!pstr)
		return;

	CBasePlayerItem *pItem = NULL;
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	   {
		if (m_rgpPlayerItems[i])
		   {
			pItem = m_rgpPlayerItems[i];
	
			while (pItem)
			   {
				if (FClassnameIs(pItem->pev, pstr))
					break;
				pItem = pItem->m_pNext;
			   }
		   }

		if (pItem)
			break;
	   }

	if (!pItem)
		return;

	
	if (pItem == m_pActiveItem)
		return;

	// FIX, this needs to queue them up and delay
	if (m_pActiveItem)
		m_pActiveItem->Holster( );
	
	m_pLastItem = m_pActiveItem;
	m_pActiveItem = pItem;

	if (m_pActiveItem)
	   {
		m_pActiveItem->Deploy( );
		m_pActiveItem->UpdateItemInfo( );
	   }
}


void CBasePlayer::SelectLastItem(void)
{
	if (!m_pLastItem)
		return;

	if ( m_pActiveItem && !m_pActiveItem->CanHolster() )
		return;

	// FIX, this needs to queue them up and delay
	if (m_pActiveItem)
		m_pActiveItem->Holster( );
	
	CBasePlayerItem *pTemp = m_pActiveItem;
	m_pActiveItem = m_pLastItem;
	m_pLastItem = pTemp;
	m_pActiveItem->Deploy( );
	m_pActiveItem->UpdateItemInfo( );
}

//==============================================
// HasWeapons - do I have any weapons at all?
//==============================================
BOOL CBasePlayer::HasWeapons( void )
{
	int i;
	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
		if ( m_rgpPlayerItems[ i ] )
			return TRUE;
	return FALSE;
}

void CBasePlayer::SelectPrevItem( int iItem )
{
}



//==============================================
// !!!UNDONE:ultra temporary SprayCan entity to apply
// decal frame at a time. For PreAlpha CD
//==============================================
class CSprayCan : public CBaseEntity
{
public:
	void	Spawn ( entvars_t *pevOwner );
	void	Think( void );

	virtual int	ObjectCaps( void ) { return FCAP_DONT_SAVE; }
};

void CSprayCan::Spawn ( entvars_t *pevOwner )
{
	pev->origin = pevOwner->origin + Vector ( 0 , 0 , 32 );
	pev->angles = pevOwner->v_angle;
	pev->owner = ENT(pevOwner);
	pev->frame = 0;

	pev->nextthink = gpGlobals->time + 0.1;
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/sprayer.wav", 1, ATTN_NORM);
}

void CSprayCan::Think( void )
{
	TraceResult	tr;	
	int playernum;
	int nFrames;
	CBasePlayer *pPlayer;
	
	pPlayer = (CBasePlayer *)GET_PRIVATE(pev->owner);

	if (pPlayer)
		nFrames = pPlayer->GetCustomDecalFrames();
	else
		nFrames = -1;

	playernum = ENTINDEX(pev->owner);
	
	// ALERT(at_console, "Spray by player %i, %i of %i\n", playernum, (int)(pev->frame + 1), nFrames);

	UTIL_MakeVectors(pev->angles);
	UTIL_TraceLine ( pev->origin, pev->origin + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, & tr);

	// No customization present.
	if (nFrames == -1)
	   {
		UTIL_DecalTrace( &tr, DECAL_LAMBDA6 );
		UTIL_Remove( this );
	   }
	else
	   {
		UTIL_PlayerDecalTrace( &tr, playernum, pev->frame, TRUE );
		// Just painted last custom frame.
		if ( pev->frame++ >= (nFrames - 1))
			UTIL_Remove( this );
	   }

	pev->nextthink = gpGlobals->time + 0.1;
}

class	CBloodSplat : public CBaseEntity
{
public:
	void	Spawn ( entvars_t *pevOwner );
	void	Spray ( void );
};

void CBloodSplat::Spawn ( entvars_t *pevOwner )
{
	pev->origin = pevOwner->origin + Vector ( 0 , 0 , 32 );
	pev->angles = pevOwner->v_angle;
	pev->owner = ENT(pevOwner);

	SetThink ( Spray );
	pev->nextthink = gpGlobals->time + 0.1;
}

void CBloodSplat::Spray ( void )
{
	TraceResult	tr;	
	
	UTIL_MakeVectors(pev->angles);
	UTIL_TraceLine ( pev->origin, pev->origin + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, & tr);
	UTIL_BloodDecalTrace( &tr, BLOOD_COLOR_RED );
	SetThink ( SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1;
}

//==============================================

void CBasePlayer::GiveNamedItem( const char *pszName )
{
	edict_t	*pent;

	int istr = MAKE_STRING(pszName);

	pent = CREATE_NAMED_ENTITY(istr);
	if ( FNullEnt( pent ) )
		return;
	VARS( pent )->origin = pev->origin;
	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn( pent );
	DispatchTouch( pent, ENT( pev ) );
}


CBaseEntity *FindEntityForward( CBaseEntity *pMe )
{
	TraceResult tr;

	UTIL_MakeVectors(pMe->pev->v_angle);
	UTIL_TraceLine(pMe->pev->origin + pMe->pev->view_ofs,pMe->pev->origin + pMe->pev->view_ofs + gpGlobals->v_forward * 8192,dont_ignore_monsters, pMe->edict(), &tr );
	if ( tr.flFraction != 1.0 && !FNullEnt( tr.pHit) )
	   {
		CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
		return pHit;
	   }
	return NULL;
}


BOOL CBasePlayer :: FlashlightIsOn( void )
{
	return FBitSet(pev->effects, EF_DIMLIGHT);
}


void CBasePlayer :: FlashlightTurnOn( void )
{
	if ( !g_pGameRules->FAllowFlashlight() )
	   {
		return;
	   }

	if ( (pev->weapons & (1<<WEAPON_SUIT)) )
	   {
		EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, SOUND_FLASHLIGHT_ON, 1.0, ATTN_NORM, 0, PITCH_NORM );
		SetBits(pev->effects, EF_DIMLIGHT);
		MESSAGE_BEGIN( MSG_ONE, gmsgFlashlight, NULL, pev );
		WRITE_BYTE(1);
		WRITE_BYTE(m_iFlashBattery);
		MESSAGE_END();

		m_flFlashLightTime = FLASH_DRAIN_TIME + gpGlobals->time;
	   }
}


void CBasePlayer :: FlashlightTurnOff( void )
{
	EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, SOUND_FLASHLIGHT_OFF, 1.0, ATTN_NORM, 0, PITCH_NORM );
    ClearBits(pev->effects, EF_DIMLIGHT);
	MESSAGE_BEGIN( MSG_ONE, gmsgFlashlight, NULL, pev );
	WRITE_BYTE(0);
	WRITE_BYTE(m_iFlashBattery);
	MESSAGE_END();

	m_flFlashLightTime = FLASH_CHARGE_TIME + gpGlobals->time;

}

/*
===============
ForceClientDllUpdate

When recording a demo, we need to have the server tell us the entire client state
so that the client side .dll can behave correctly.
Reset stuff so that the state is transmitted.
===============
*/
void CBasePlayer :: ForceClientDllUpdate( void )
{
	m_iClientHealth  = -1;
	m_iClientBattery = -1;
	m_iTrain |= TRAIN_NEW;  // Force new train message.
	m_fWeapon = FALSE;          // Force weapon send
	m_fKnownItem = FALSE;    // Force weaponinit messages.

	// Now force all the necessary messages
	//  to be sent.
	UpdateClientData();
}

/*
============
ImpulseCommands
============
*/
extern float g_flWeaponCheat;

void CBasePlayer::ImpulseCommands( )
{
	TraceResult	tr;// UNDONE: kill me! This is temporary for PreAlpha CDs

	// Handle use events
	PlayerUse();
		
	int iImpulse = (int)pev->impulse;
	switch (iImpulse)
	   {
	   case 99:
		   {
		   int iOn;
		   if (!gmsgLogo)
		      {
			   iOn = 1;
			   gmsgLogo = REG_USER_MSG("Logo", 1);
		      } 
		   else 
			   iOn = 0;
		   ASSERT( gmsgLogo > 0 );
		   // send "health" update message
		   MESSAGE_BEGIN( MSG_ONE, gmsgLogo, NULL, pev );
			   WRITE_BYTE(iOn);
		   MESSAGE_END();
		   if(!iOn)
			   gmsgLogo = 0;
		   break;
		   }

	   case 100:
           // temporary flashlight for level designers
         /*
         if ( FlashlightIsOn() )
			   FlashlightTurnOff();
         else 
			   FlashlightTurnOn();
         */
		   break;

	   case	201:// paint decal	   
		   if ( gpGlobals->time < m_flNextDecalTime )
			   // too early!
			   break;

		   UTIL_MakeVectors(pev->v_angle);
		   UTIL_TraceLine ( pev->origin + pev->view_ofs, pev->origin + pev->view_ofs + gpGlobals->v_forward * 128, ignore_monsters, ENT(pev), & tr);

		   if ( tr.flFraction != 1.0 )
		      {// line hit something, so paint a decal
			   m_flNextDecalTime = gpGlobals->time + CVAR_GET_FLOAT("decalfrequency");
			   CSprayCan *pCan = GetClassPtr((CSprayCan *)NULL);
			   pCan->Spawn( pev );
		      }
		   break;

	   case    204:  //  Demo recording, update client dll specific data again.
		   ForceClientDllUpdate(); 
		   break;

	   default:
		   break;
	   }
	
	pev->impulse = 0;
}

//
// Add a weapon to the player (Item == Weapon == Selectable Object)
//
int CBasePlayer::AddPlayerItem( CBasePlayerItem *pItem )
{
	CBasePlayerItem *pInsert;
   
   if (pev->deadflag > DEAD_NO)
	   return FALSE;

	pInsert = m_rgpPlayerItems[pItem->iItemSlot()];

	while (pInsert)
	   {
		if (FClassnameIs( pInsert->pev, STRING( pItem->pev->classname) ))
		   {
			if (pItem->AddDuplicate( pInsert ))
			   {
				g_pGameRules->PlayerGotWeapon ( this, pItem );

				// ugly hack to update clip w/o an update clip message
				pInsert->UpdateItemInfo( );
				if (m_pActiveItem)
					m_pActiveItem->UpdateItemInfo( );

				pItem->Kill( );
			   }
			return FALSE;
		   }
		pInsert = pInsert->m_pNext;
	   }


	if (pItem->AddToPlayer( this ))
	   {
		g_pGameRules->PlayerGotWeapon ( this, pItem );
      m_pLastItem = m_pActiveItem;
		pItem->m_pNext = m_rgpPlayerItems[pItem->iItemSlot()];
		m_rgpPlayerItems[pItem->iItemSlot()] = pItem;

		// should we switch to this item?
		if ( g_pGameRules->FShouldSwitchWeapon( this, pItem ) )
			SwitchWeapon( pItem );

		return TRUE;
	   }
	return FALSE;
}



int CBasePlayer::RemovePlayerItem( CBasePlayerItem *pItem )
{
	if (m_pActiveItem == pItem)
	   {
		pItem->Holster( );
		pItem->pev->nextthink = 0;// crowbar may be trying to swing again, etc.
		pItem->SetThink( NULL );
		m_pActiveItem = NULL;
		pev->viewmodel = 0;
		pev->weaponmodel = 0;
	   }
	else if ( m_pLastItem == pItem )
		m_pLastItem = NULL;

	CBasePlayerItem *pPrev = m_rgpPlayerItems[pItem->iItemSlot()];

	if (pPrev == pItem)
	   {
		m_rgpPlayerItems[pItem->iItemSlot()] = pItem->m_pNext;
		return TRUE;
	   }
	else
	   {
		while (pPrev && pPrev->m_pNext != pItem)
		   {
			pPrev = pPrev->m_pNext;
		   }
		if (pPrev)
		   {
			pPrev->m_pNext = pItem->m_pNext;
			return TRUE;
		   }
	   }
	return FALSE;
}


//
// Returns the unique ID for the ammo, or -1 if error
//
int CBasePlayer :: GiveAmmo( int iCount, char *szName, int iMax )
{
	if ( !szName )
	   {
		// no ammo.
		return -1;
	   }

	if ( !g_pGameRules->CanHaveAmmo( this, szName, iMax ) )
	   {
		// game rules say I can't have any more of this ammo type.
		return -1;
	   }

	int i = 0;

	i = GetAmmoIndex( szName );

	if ( i < 0 || i >= MAX_AMMO_SLOTS )
		return -1;

	int iAdd = min( iCount, iMax - m_rgAmmo[i] );
	if ( iAdd < 1 )
		return i;

	m_rgAmmo[ i ] += iAdd;


	if ( gmsgAmmoPickup )  // make sure the ammo messages have been linked first
	   {
		// Send the message that ammo has been picked up
		MESSAGE_BEGIN( MSG_ONE, gmsgAmmoPickup, NULL, pev );
			WRITE_BYTE( GetAmmoIndex(szName) );		// ammo ID
			WRITE_BYTE( iAdd );		// amount
		MESSAGE_END();
	   }
	return i;
}


/*
============
ItemPreFrame

Called every frame by the player PreThink
============
*/
void CBasePlayer::ItemPreFrame()
{
#if defined( CLIENT_WEAPONS )
    if ( m_flNextAttack > 0 )
#else
    if ( gpGlobals->time < m_flNextAttack )
#endif
	   {
		return;
	   }

	if (!m_pActiveItem)
		return;

	m_pActiveItem->ItemPreFrame( );
}


/*
============
ItemPostFrame

Called every frame by the player PostThink
============
*/
void CBasePlayer::ItemPostFrame()
{
	static int fInSelect = FALSE;

	// check if the player is using a tank
	if ( m_pTank != NULL )
		return;

#if defined( CLIENT_WEAPONS )
    if ( m_flNextAttack > 0 )
#else
    if ( gpGlobals->time < m_flNextAttack )
#endif
	   {
		return;
	   }

	ImpulseCommands();

	if (!m_pActiveItem)
		return;

	m_pActiveItem->ItemPostFrame( );
}

int CBasePlayer::AmmoInventory( int iAmmoIndex )
{
	if (iAmmoIndex == -1)
	   {
		return -1;
	   }

	return m_rgAmmo[ iAmmoIndex ];
}

int CBasePlayer::GetAmmoIndex(const char *psz)
{
	int i;

	if (!psz)
		return -1;

	for (i = 1; i < MAX_AMMO_SLOTS; i++)
	   {
		if ( !CBasePlayerItem::AmmoInfoArray[i].pszName )
			continue;

		if (stricmp( psz, CBasePlayerItem::AmmoInfoArray[i].pszName ) == 0)
			return i;
	   }

	return -1;
}

// Called from UpdateClientData
// makes sure the client has all the necessary ammo info,  if values have changed
void CBasePlayer::SendAmmoUpdate(void)
{
	for (int i=0; i < MAX_AMMO_SLOTS;i++)
	   {
		if (m_rgAmmo[i] != m_rgAmmoLast[i])
		   {
			m_rgAmmoLast[i] = m_rgAmmo[i];

			ASSERT( m_rgAmmo[i] >= 0 );
			ASSERT( m_rgAmmo[i] < 255 );

			// send "Ammo" update message
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pev );
				WRITE_BYTE( i );
				WRITE_BYTE( max( min( m_rgAmmo[i], 254 ), 0 ) );  // clamp the value to one byte
			MESSAGE_END();
		   }
	   }
}

/*
=========================================================
	UpdateClientData

resends any changed player HUD info to the client.
Called every frame by PlayerPreThink
Also called at start of demo recording and playback by
ForceClientDllUpdate to ensure the demo gets messages
reflecting all of the HUD state info.
=========================================================
*/
void CBasePlayer :: UpdateClientData( void )
{
	if (m_fInitHUD)
	   {
		m_fInitHUD = FALSE;
		gInitHUD = FALSE;

		MESSAGE_BEGIN( MSG_ONE, gmsgResetHUD, NULL, pev );
			WRITE_BYTE( 0 );
		MESSAGE_END();

		if ( !m_fGameHUDInitialized )
		   {
			MESSAGE_BEGIN( MSG_ONE, gmsgInitHUD, NULL, pev );
			MESSAGE_END();

			g_pGameRules->InitHUD( this );
			m_fGameHUDInitialized = TRUE;
         /*
			if ( g_pGameRules->IsMultiplayer() )
			   {
				FireTargets( "game_playerjoin", this, this, USE_TOGGLE, 0 );
			   }
         */
		   }
		//FireTargets( "game_playerspawn", this, this, USE_TOGGLE, 0 );
	   }

	if ( m_iHideHUD != m_iClientHideHUD )
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgHideWeapon, NULL, pev );
			WRITE_BYTE( m_iHideHUD );
		MESSAGE_END();

		m_iClientHideHUD = m_iHideHUD;
	   }

	// HACKHACK -- send the message to display the game title
	if (gDisplayTitle)
	   {
		MESSAGE_BEGIN( MSG_ONE, gmsgShowGameTitle, NULL, pev );
		WRITE_BYTE( 0 );
		MESSAGE_END();
		gDisplayTitle = 0;
	   }
   
	if (pev->health != m_iClientHealth)
	   {
		int iHealth = max( pev->health, 0 );  // make sure that no negative health values are sent

		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgHealth, NULL, pev );
			WRITE_BYTE( iHealth );
		MESSAGE_END();

		m_iClientHealth = pev->health;
	   }


   /*
	if (pev->armorvalue != m_iClientBattery)
	   {
		m_iClientBattery = pev->armorvalue;

		ASSERT( gmsgBattery > 0 );
		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgBattery, NULL, pev );
			WRITE_SHORT( (int)pev->armorvalue);
		MESSAGE_END();
	   }
   */

	if (pev->dmg_take || pev->dmg_save || m_bitsHUDDamage != m_bitsDamageType)
	   {
		// Comes from inside me if not set
		Vector damageOrigin = pev->origin;
		// send "damage" message
		// causes screen to flash, and pain compass to show direction of damage
		edict_t *other = pev->dmg_inflictor;
		if ( other )
		   {
			CBaseEntity *pEntity = CBaseEntity::Instance(other);
			if ( pEntity )
				damageOrigin = pEntity->Center();
		   }

		// only send down damage type that have hud art
		int visibleDamageBits = m_bitsDamageType & DMG_SHOWNHUD;

		MESSAGE_BEGIN( MSG_ONE, gmsgDamage, NULL, pev );
			WRITE_BYTE( pev->dmg_save );
			WRITE_BYTE( pev->dmg_take );
			WRITE_LONG( visibleDamageBits );
			WRITE_COORD( damageOrigin.x );
			WRITE_COORD( damageOrigin.y );
			WRITE_COORD( damageOrigin.z );
		MESSAGE_END();
	
		pev->dmg_take = 0;
		pev->dmg_save = 0;
		m_bitsHUDDamage = m_bitsDamageType;
		
		// Clear off non-time-based damage indicators
		m_bitsDamageType &= DMG_TIMEBASED;
	   }

	// Update Flashlight
	if ((m_flFlashLightTime) && (m_flFlashLightTime <= gpGlobals->time))
	   {
		if (FlashlightIsOn())
		   {
			if (m_iFlashBattery)
			   {
				m_flFlashLightTime = FLASH_DRAIN_TIME + gpGlobals->time;
				m_iFlashBattery--;
				
				if (!m_iFlashBattery)
					FlashlightTurnOff();
			   }
		   }
		else
		   {
			if (m_iFlashBattery < 100)
			   {
				m_flFlashLightTime = FLASH_CHARGE_TIME + gpGlobals->time;
				m_iFlashBattery++;
			   }
			else
				m_flFlashLightTime = 0;
		   }
		MESSAGE_BEGIN( MSG_ONE, gmsgFlashBattery, NULL, pev );
		WRITE_BYTE(m_iFlashBattery);
		MESSAGE_END();
	   }
	if (m_iTrain & TRAIN_NEW)
	   {
		ASSERT( gmsgTrain > 0 );
		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgTrain, NULL, pev );
			WRITE_BYTE(m_iTrain & 0xF);
		MESSAGE_END();

		m_iTrain &= ~TRAIN_NEW;
	   }

	//
	// New Weapon?
	//
	if (!m_fKnownItem)
	   {
		m_fKnownItem = TRUE;

	// WeaponInit Message
	// byte  = # of weapons
	//
	// for each weapon:
	// byte		name str length (not including null)
	// bytes... name
	// byte		Ammo Type
	// byte		Ammo2 Type
	// byte		bucket
	// byte		bucket pos
	// byte		flags	
	// ????		Icons
		
		// Send ALL the weapon info now
		int i;

		for (i = 0; i < MAX_WEAPONS; i++)
		   {
			ItemInfo& II = CBasePlayerItem::ItemInfoArray[i];

			if ( !II.iId )
				continue;

			const char *pszName;
			if (!II.pszName)
				pszName = "Empty";
			else
				pszName = II.pszName;

			MESSAGE_BEGIN( MSG_ONE, gmsgWeaponList, NULL, pev );  
				WRITE_STRING(pszName);			// string	weapon name
				WRITE_BYTE(GetAmmoIndex(II.pszAmmo1));	// byte		Ammo Type
				WRITE_BYTE(II.iMaxAmmo1);				// byte     Max Ammo 1
				WRITE_BYTE(GetAmmoIndex(II.pszAmmo2));	// byte		Ammo2 Type
				WRITE_BYTE(II.iMaxAmmo2);				// byte     Max Ammo 2
				WRITE_BYTE(II.iSlot);					// byte		bucket
				WRITE_BYTE(II.iPosition);				// byte		bucket pos
				WRITE_BYTE(II.iId);						// byte		id (bit index into pev->weapons)
				WRITE_BYTE(II.iFlags);					// byte		Flags
			MESSAGE_END();
		   }
	   }


	SendAmmoUpdate();

	// Update all the items
	for ( int i = 0; i < MAX_ITEM_TYPES; i++ )
	   {
		if ( m_rgpPlayerItems[i] )  // each item updates it's successors
			m_rgpPlayerItems[i]->UpdateClientData( this );
	   }

	// Cache and client weapon change
	m_pClientActiveItem = m_pActiveItem;
	m_iClientFOV = m_iFOV;
}


//=========================================================
// FBecomeProne - Overridden for the player to set the proper
// physics flags when a barnacle grabs player.
//=========================================================
BOOL CBasePlayer :: FBecomeProne ( void )
{
	m_afPhysicsFlags |= PFLAG_ONBARNACLE;
	return TRUE;
}

//=========================================================
// BarnacleVictimBitten - bad name for a function that is called
// by Barnacle victims when the barnacle pulls their head
// into its mouth. For the player, just die.
//=========================================================
void CBasePlayer :: BarnacleVictimBitten ( entvars_t *pevBarnacle )
{
	TakeDamage ( pevBarnacle, pevBarnacle, pev->health + pev->armorvalue, DMG_SLASH | DMG_ALWAYSGIB );
}

//=========================================================
// BarnacleVictimReleased - overridden for player who has
// physics flags concerns. 
//=========================================================
void CBasePlayer :: BarnacleVictimReleased ( void )
{
	m_afPhysicsFlags &= ~PFLAG_ONBARNACLE;
}


//=========================================================
// Illumination 
// return player light level plus virtual muzzle flash
//=========================================================
int CBasePlayer :: Illumination( void )
{
	int iIllum = CBaseEntity::Illumination( );

	iIllum += m_iWeaponFlash;
	if (iIllum > 255)
		return 255;
	return iIllum;
}


void CBasePlayer :: EnableControl(BOOL fControl)
{
	if (!fControl)
      {
      m_afPhysicsFlags |= PFLAG_ONBARNACLE;
		pev->flags |= FL_FROZEN;
      }
	else
      {
      m_afPhysicsFlags &= ~PFLAG_ONBARNACLE;
		pev->flags &= ~FL_FROZEN;
      }

}


/*
=============
SetCustomDecalFrames

  UNDONE:  Determine real frame limit, 8 is a placeholder.
  Note:  -1 means no custom frames present.
=============
*/
void CBasePlayer :: SetCustomDecalFrames( int nFrames )
{
	if (nFrames > 0 &&
		nFrames < 8)
		m_nCustomSprayFrames = nFrames;
	else
		m_nCustomSprayFrames = -1;
}

/*
=============
GetCustomDecalFrames

  Returns the # of custom frames this player's custom clan logo contains.
=============
*/
int CBasePlayer :: GetCustomDecalFrames( void )
{
	return m_nCustomSprayFrames;
}


//=========================================================
// DropPlayerItem - drop the named item, or if no name,
// the active item. 
//=========================================================
void CBasePlayer::DropPlayerItem ( char *pszItemName )
{
   if (!pev)
      return;
	if (pszItemName && !strlen( pszItemName ) )
	   {
		// if this string has no length, the client didn't type a name!
		// assume player wants to drop the active item.
		// make the string null to make future operations in this function easier
		pszItemName = NULL;
	   } 

	CBasePlayerItem *pWeapon;
	int i; 

	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	   {
		pWeapon = m_rgpPlayerItems[ i ];
		while ( pWeapon )
		   {
			if ( pszItemName )
            {
				if ( !strcmp( pszItemName, STRING( pWeapon->pev->classname ) ) ) break;
            }
			else
				if ( pWeapon == m_pActiveItem ) break;
			pWeapon = pWeapon->m_pNext;
		   }

		if ( pWeapon)
		   {
         if (pev->deadflag == DEAD_NO)
			   g_pGameRules->GetNextBestWeapon( this, pWeapon );

			UTIL_MakeVectors ( pev->angles ); 

			pev->weapons &= ~(1<<pWeapon->m_iId);// take item off hud

         if (pWeapon->ItemInfoArray[pWeapon->m_iId].iWeight >= WEIGHT_SECONDARY)
            {
			   CWeaponBox *pWeaponBox = (CWeaponBox *)CBaseEntity::Create( "weaponbox", pev->origin + gpGlobals->v_forward * 10, pev->angles, edict() );
            pWeaponBox->pev->angles.x = 0;
			   pWeaponBox->pev->angles.z = 0;
			   pWeaponBox->PackWeapon( pWeapon );
            pWeaponBox->pev->iuser1 = pWeapon->m_iId;
			   pWeaponBox->pev->velocity = gpGlobals->v_forward * 300 + gpGlobals->v_forward * 100;
			   pWeaponBox->m_iWeight = pWeapon->iWeight();

            CBasePlayerWeapon *x = (CBasePlayerWeapon *)pWeapon;
            char *p = (char *) STRING(x->dropmodel);
            if (p>0)
      	      SET_MODEL(ENT(pWeaponBox->pev), p);
            
            // drop half of the ammo for this weapon.
			   int	iAmmoIndex;

			   iAmmoIndex = GetAmmoIndex ( pWeapon->pszAmmo1() ); // ???
			
			   if ( iAmmoIndex != -1 )
			      {
               if (pev->deadflag == DEAD_NO)
					   pWeaponBox->PackAmmo( MAKE_STRING(pWeapon->pszAmmo1()),  0);
               else
                  {
					   pWeaponBox->PackAmmo( MAKE_STRING(pWeapon->pszAmmo1()),  m_rgAmmo[ iAmmoIndex ]);
					   m_rgAmmo[ iAmmoIndex ] = 0; 
                  }
			      }
            }
			return;
		   }
	   }
}

//=========================================================
// HasPlayerItem Does the player already have this item?
//=========================================================
BOOL CBasePlayer::HasPlayerItem( CBasePlayerItem *pCheckItem )
{
	CBasePlayerItem *pItem = m_rgpPlayerItems[pCheckItem->iItemSlot()];

	while (pItem)
	   {
		if (FClassnameIs( pItem->pev, STRING( pCheckItem->pev->classname) ))
		   {
			return TRUE;
		   }
		pItem = pItem->m_pNext;
	   }

	return FALSE;
}

//=========================================================
// HasNamedPlayerItem Does the player already have this item?
//=========================================================
BOOL CBasePlayer::HasNamedPlayerItem( const char *pszItemName )
{
	CBasePlayerItem *pItem;
	int i;
 
	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	   {
		pItem = m_rgpPlayerItems[ i ];
		
		while (pItem)
		   {
			if ( !strcmp( pszItemName, STRING( pItem->pev->classname ) ) )
			   {
				return TRUE;
			   }
			pItem = pItem->m_pNext;
		   }
	   }
	return FALSE;
}

//=========================================================
// 
//=========================================================
BOOL CBasePlayer :: SwitchWeapon( CBasePlayerItem *pWeapon ) 
{
	if ( !pWeapon->CanDeploy() )
		return FALSE;
	
	if (m_pActiveItem)
		m_pActiveItem->Holster( );

	m_pActiveItem = pWeapon;
	pWeapon->Deploy( );

	return TRUE;
}


class CStripWeapons : public CPointEntity
{
public:
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

private:
};

LINK_ENTITY_TO_CLASS( player_weaponstrip, CStripWeapons );

void CStripWeapons :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBasePlayer *pPlayer = NULL;

	if ( pActivator && pActivator->IsPlayer() )
		pPlayer = (CBasePlayer *)pActivator;
	else if ( !g_pGameRules->IsDeathmatch() )
		pPlayer = (CBasePlayer *)CBaseEntity::Instance( g_engfuncs.pfnPEntityOfEntIndex( 1 ) );

	if ( pPlayer )
		pPlayer->RemoveAllItems( FALSE );
}


class CRevertSaved : public CPointEntity
{
public:
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	EXPORT MessageThink( void );
	void	EXPORT LoadThink( void );
	void	KeyValue( KeyValueData *pkvd );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	inline	float	Duration( void ) { return pev->dmg_take; }
	inline	float	HoldTime( void ) { return pev->dmg_save; }
	inline	float	MessageTime( void ) { return m_messageTime; }
	inline	float	LoadTime( void ) { return m_loadTime; }

	inline	void	SetDuration( float duration ) { pev->dmg_take = duration; }
	inline	void	SetHoldTime( float hold ) { pev->dmg_save = hold; }
	inline	void	SetMessageTime( float time ) { m_messageTime = time; }
	inline	void	SetLoadTime( float time ) { m_loadTime = time; }

private:
	float	m_messageTime;
	float	m_loadTime;
};

LINK_ENTITY_TO_CLASS( player_loadsaved, CRevertSaved );

TYPEDESCRIPTION	CRevertSaved::m_SaveData[] = 
{
	DEFINE_FIELD( CRevertSaved, m_messageTime, FIELD_FLOAT ),	// These are not actual times, but durations, so save as floats
	DEFINE_FIELD( CRevertSaved, m_loadTime, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CRevertSaved, CPointEntity );

void CRevertSaved :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "duration"))
	   {
		SetDuration( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	   }
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	   {
		SetHoldTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	   }
	else if (FStrEq(pkvd->szKeyName, "messagetime"))
	   {
		SetMessageTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	   }
	else if (FStrEq(pkvd->szKeyName, "loadtime"))
	   {
		SetLoadTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	   }
	else 
		CPointEntity::KeyValue( pkvd );
}

void CRevertSaved :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	UTIL_ScreenFadeAll( pev->rendercolor, Duration(), HoldTime(), pev->renderamt, FFADE_OUT );
	pev->nextthink = gpGlobals->time + MessageTime();
	SetThink( MessageThink );
}


void CRevertSaved :: MessageThink( void )
{
	UTIL_ShowMessageAll( STRING(pev->message) );
	float nextThink = LoadTime() - MessageTime();
	if ( nextThink > 0 ) 
	   {
		pev->nextthink = gpGlobals->time + nextThink;
		SetThink( LoadThink );
	   }
	else
		LoadThink();
}


void CRevertSaved :: LoadThink( void )
{
	if ( !gpGlobals->deathmatch )
		SERVER_COMMAND("reload\n");
}


//=========================================================
// Multiplayer intermission spots.
//=========================================================
class CInfoIntermission:public CPointEntity
{
	void Spawn( void );
	void Think( void );
};

void CInfoIntermission::Spawn( void )
{
	UTIL_SetOrigin( pev, pev->origin );
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->v_angle = g_vecZero;

	pev->nextthink = gpGlobals->time + 2;// let targets spawn!

}

void CInfoIntermission::Think ( void )
{
	edict_t *pTarget;

	// find my target
	pTarget = FIND_ENTITY_BY_TARGETNAME( NULL, STRING(pev->target) );

	if ( !FNullEnt(pTarget) )
	   {
		pev->v_angle = UTIL_VecToAngles( (pTarget->v.origin - pev->origin).Normalize() );
		pev->v_angle.x = -pev->v_angle.x;
	   }
}

void CBasePlayer::PlayerDeathThink(void)
{
	float flForward;
	if (FBitSet(pev->flags, FL_ONGROUND))
	   {
		flForward = pev->velocity.Length() - 20;
		if (flForward <= 0)
			pev->velocity = g_vecZero;
		else    
			pev->velocity = flForward * pev->velocity.Normalize();
	   }

	if (pev->modelindex && (!m_fSequenceFinished) && (pev->deadflag == DEAD_DYING))
	   {
		StudioFrameAdvance();
		m_iRespawnFrames++;
		if ( m_iRespawnFrames < 620 )  // animations should be no longer than this
			return;
	   }

	if (pev->deadflag == DEAD_DYING)
		pev->deadflag = DEAD_DEAD;
	
	StopAnimation();

   if ( HasWeapons() )
		PackDeadPlayerItems();

	pev->effects |= EF_NOINTERP;
	pev->framerate = 0.0;

   g_engfuncs.pfnSetClientMaxspeed( ENT( pev ), 540 );
   pev->button = 0;
	m_iRespawnFrames = 0;
   // !!FIXME!!
   if (m_iTeam && !(m_afPhysicsFlags & PFLAG_OBSERVER))
      CopyToBodyQue(pev);

	StartDeathCam();
	pev->nextthink = -1;
}
void SendClientsInfo(CBasePlayer* me, int team_index);
void CBasePlayer::StartDeathCam( void )
{
	if ( pev->view_ofs == g_vecZero )
		return;
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin + Vector( 0, 0, 128 ), ignore_monsters, edict(), &tr );
	StartObserver( tr.vecEndPos, UTIL_VecToAngles( tr.vecEndPos - pev->origin  ) );
}

void CBasePlayer::Radio (int RadioCommand, int iTeam)
{
	CBaseEntity* pPlayer = NULL;
	CBasePlayer* player;

   if (pev->deadflag > DEAD_NO)
		return;

	pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" );
	while ((pPlayer = UTIL_FindEntityByClassname ( pPlayer, "player" )) != NULL)
	   {
      if (FNullEnt(pPlayer->edict()))
         continue;

      if (!pPlayer->IsPlayer())
         continue;

      if (pPlayer->pev->flags & FL_DORMANT)
         continue;

		player = GetClassPtr((CBasePlayer *)pPlayer->pev);
      if (player->pev->deadflag > DEAD_NO)
         continue;

      if (iTeam && player->m_iTeam != iTeam)
         continue;
      /*
      MESSAGE_BEGIN (MSG_ONE, gmsgRadio, NULL, pPlayer->pev);
         WRITE_BYTE (ENTINDEX(player->edict()));
         WRITE_BYTE (RadioCommand);
      MESSAGE_END();
      */
	   }
}

void CBasePlayer :: ShowVGUIMenu(int iMenuID)
{
   /*
	MESSAGE_BEGIN(MSG_ONE, gmsgVGUIMenu, NULL, pev);
		WRITE_BYTE( iMenuID );
	MESSAGE_END();
   */
}


CBasePlayer *GetNextClient(int c_index);
CBasePlayer *GetPrevClient(int c_index);

sendstat_t ss;
void CBasePlayer::SendMyStats(int iid)
{
   CBasePlayer *s_player = NULL;
   int i;
   // unsigned short xxx;
   switch (iid)
      {
      case 0:
         s_player = this;
         break;
      case 1:
         ss.ClientID = m_iStatClientID;
         s_player = GetPrevClient(ss.ClientID);
         break;
      case 2:
         ss.ClientID = m_iStatClientID;
         s_player = GetNextClient(ss.ClientID);
         break;
      case -1:
		   if (m_hObserverTarget)
            s_player =  (CBasePlayer *) UTIL_PlayerByIndex( pev->iuser2 );
         break;
      default:
         return;
      }  
   if (!s_player)
      s_player = this;
   ss.ClientID = s_player->entindex();
   m_iStatClientID = ss.ClientID;
   ss.Shots = s_player->stat.Shots;
   ss.WonID = s_player->m_iWONID;
   strcpy(ss.SteamID, s_player->m_iSteamID);
   ss.Juice = s_player->m_iJuice;
   ss.Health = s_player->pev->health;
   ss.Hits = s_player->stat.Hits;
   ss.MaxRoundKills = s_player->stat.MaxRoundKills;
   ss.MaxRoundLives = s_player->stat.MaxRoundLives;
   ss.RoundKills = s_player->stat.RoundKills;
   ss.Rounds = s_player->stat.Rounds;
   ss.Offense = s_player->stat.Offense;
   ss.Defense = s_player->stat.Defense;
   ss.Suicides = s_player->stat.Suicides;
   ss.TimeOn = gpGlobals->time - s_player->stat.TimeOn;
   ss.LastKills = s_player->stat.LastKills;
   ss.Frags = s_player->pev->frags;
   ss.Deaths = s_player->m_iDeaths;
   ss.KnifeKills = s_player->stat.KnifeKills;
   ss.HeadShots = s_player->stat.HeadShots;
   ss.FavPrimary = 0;
   ss.PrimaryKills = 0;
   ss.FavSecondary = 0;
   ss.SecondaryKills = 0;
   for (i = WEAPON_TMP; i <= WEAPON_XM1014; i++)
      if (ss.FavPrimary < s_player->stat.Kills[i])
         {
         ss.FavPrimary = i;
         ss.PrimaryKills = s_player->stat.Kills[i];
         }
   for (i = WEAPON_92D; i <= WEAPON_DEAGLE; i++)
      if (ss.FavSecondary< s_player->stat.Kills[i])
         {
         ss.FavSecondary = i;
         ss.SecondaryKills = s_player->stat.Kills[i];
         }
   m_iTargetWONID = ss.WonID;

	MESSAGE_BEGIN(MSG_ONE, gsmgSendStats, NULL, pev);
      WRITE_SHORT( ss.ClientID );
      WRITE_SHORT( ss.Juice );
      WRITE_SHORT( ss.Health );
      WRITE_SHORT( ss.Shots );
      WRITE_SHORT( ss.Hits );
      WRITE_SHORT( ss.MaxRoundKills );
      WRITE_SHORT( ss.MaxRoundLives );
      WRITE_SHORT( ss.RoundKills );
      WRITE_SHORT( ss.Rounds );
      WRITE_SHORT( ss.Offense );
      WRITE_SHORT( ss.Defense );
      WRITE_SHORT( ss.Suicides );
      WRITE_SHORT( ss.TimeOn );
      WRITE_SHORT( ss.LastKills );
      WRITE_SHORT( ss.Frags );
      WRITE_SHORT( ss.Deaths );
      WRITE_SHORT( ss.KnifeKills );
      WRITE_SHORT( ss.HeadShots );
      WRITE_SHORT( ss.FavPrimary );
      WRITE_SHORT( ss.PrimaryKills );
      WRITE_SHORT( ss.FavSecondary );
      WRITE_SHORT( ss.SecondaryKills );
	MESSAGE_END();
	MESSAGE_BEGIN(MSG_ONE, gsmgSendSteamID, NULL, pev);
      WRITE_STRING( ss.SteamID );
	MESSAGE_END();

}

void CBasePlayer::SendTop32()
{
   struct tm * timeinfo;
   char output[256];
   char sdate[256];
   int i, length;
   char xmessage[4096] = {0}, *xm;
   xm = xmessage;
   for (i = 0; i < 10; i++)
      {
      if (map_stat.bs[i].name[0])
         {
         timeinfo = localtime ( &map_stat.bs[i].rawtime );
         strcpy(sdate, asctime(timeinfo));
         sdate[24] = 0;
         sprintf(output, "%02d) %s - %d\n", i+1,  map_stat.bs[i].name, map_stat.bs[i].juice);
         }
      else
         {
         sprintf(output, "%02d) %s - %d\n", i+1, "None", 0);
         }
      strcat(xmessage, output);
      }
   length = strlen(xm);
   length -= 80;
   memcpy(output, xm, 80);
   output[80] = 0;
   if (length > 0)
      {
	   MESSAGE_BEGIN(MSG_ONE, gsmgSendTop32, NULL, pev);
         WRITE_BYTE(1);
         WRITE_STRING(output);
	   MESSAGE_END();
      }
   else
      {
	   MESSAGE_BEGIN(MSG_ONE, gsmgSendTop32, NULL, pev);
         WRITE_BYTE(3);
         WRITE_STRING(output);
	   MESSAGE_END();
      }
   while (length > 0)
      {
      length -= 80;
      xm += 80;
      memcpy(output, xm, 80);
      output[80] = 0;
      if (length > 0)
         {
	      MESSAGE_BEGIN(MSG_ONE, gsmgSendTop32, NULL, pev);
            WRITE_BYTE(2);
            WRITE_STRING(output);
	      MESSAGE_END();
         }
      else
         {
	      MESSAGE_BEGIN(MSG_ONE, gsmgSendTop32, NULL, pev);
            WRITE_BYTE(3);
            WRITE_STRING(output);
	      MESSAGE_END();
         }
      }
}

void CBasePlayer::Slap(float damage)
{
   UTIL_MakeVectors ( pev->angles ); 			   
   EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "server/whip.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
   pev->velocity = gpGlobals->v_forward * 500 + gpGlobals->v_up * 300;
   if (damage <= 10) Pain();
   TakeDamage( pev, pev, damage, DMG_GENERIC );   
}

void CBasePlayer::PlayBeat(int beat)
{
	MESSAGE_BEGIN(MSG_ONE, gsmgPlayBeat, NULL, pev);
      WRITE_BYTE(beat);
	MESSAGE_END();
}

void CBasePlayer::PlayMp3(char *filename, int loop)
{
	MESSAGE_BEGIN(MSG_ONE, gsmgPlayMp3, NULL, pev);
      WRITE_BYTE(loop);
      WRITE_STRING(filename);
	MESSAGE_END();
}

void CBasePlayer::PlayTheme(int theme)
{
	MESSAGE_BEGIN(MSG_ONE, gsmgPlayTheme, NULL, pev);
      WRITE_BYTE(theme);
	MESSAGE_END();
}

int CBasePlayer::HighPingCheck()
{
   int min_ping, ban_time;
   char xxx[128];
   min_ping = CVAR_GET_FLOAT( "gw_ping_min" );
   //sprintf(xxx, "samples %i, min_ping %i\n", stat.ping_samples, min_ping);
   //ClientPrint(pev, HUD_PRINTCONSOLE, xxx);
   if (!stat.ping_samples || min_ping <= 0 || m_iAdmin)
      return 0;
   stat.ping_average = stat.ping_sum / stat.ping_samples;
   stat.ping_samples = 0;
   stat.ping_sum = 0;
   if (stat.ping_average > CVAR_GET_FLOAT( "gw_ping_min" ))
      {
      ban_time = CVAR_GET_FLOAT( "gw_ping_ban" );
      if (ban_time >= 1)
         {
         //sprintf(xxx, "You have been automatically banned due to high ping of %i\n", stat.ping_average);
         //ClientPrint(pev, HUD_PRINTCONSOLE, xxx);
         sprintf(xxx, "banid %i %s kick\n", ban_time, m_iSteamID);
         SERVER_COMMAND (xxx);
         return 1;
         }
      //sprintf(xxx, "You have been automatically kicked due to high ping of %i\n", stat.ping_average);
      //ClientPrint(pev, HUD_PRINTCONSOLE, xxx);
      sprintf(xxx, "kick \"%s\"\n", STRING(pev->netname));
      SERVER_COMMAND (xxx);
      return 1;
      }
   //sprintf(xxx, "Your average ping is %i\n", stat.ping_average);
   //ClientPrint(pev, HUD_PRINTCONSOLE, xxx);
   return 0;
}

void CBasePlayer::SendServerVars()
{
   unsigned short int cc;
   float ff;
   ff = aw.recoil * 1000.0f;
   cc = ff;
	MESSAGE_BEGIN(MSG_ONE, gsmgServerVars, NULL, pev);
      WRITE_SHORT(cc);
	MESSAGE_END();
}

void CBasePlayer::SendJustConnected()
{
   int i;
   char *nn;
   CBasePlayer *pPlayer;
   for (i = 1; i  <= gpGlobals->maxClients; i++)
      {
      pPlayer = (CBasePlayer *) UTIL_PlayerByIndex( i );
 		if (!pPlayer) continue;
 		if (FNullEnt(pPlayer->edict())) continue;
      if (pPlayer == this) continue;

      nn = (char *)STRING(pPlayer->pev->netname);
      if (nn && nn[0])
         {
	      MESSAGE_BEGIN(MSG_ONE, gsmgCLUpdate, NULL, pPlayer->pev);
            WRITE_BYTE(entindex());
	      MESSAGE_END();
         }
      }
}

LINK_ENTITY_TO_CLASS( info_intermission, CInfoIntermission );

