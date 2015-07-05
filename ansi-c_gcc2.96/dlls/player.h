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
#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_FATAL_FALL_SPEED		1024// approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED	580// approx 20 feet
#define DAMAGE_FOR_FALL_SPEED		(float) 100 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED )// damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED		200
#define PLAYER_FALL_PUNCH_THRESHHOLD (float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.

//
// Player PHYSICS FLAGS bits
//
#define		PFLAG_ONLADDER		( 1<<0 )
#define		PFLAG_ONSWING		( 1<<0 )
#define		PFLAG_ONTRAIN		( 1<<1 )
#define		PFLAG_ONBARNACLE	( 1<<2 )
#define		PFLAG_DUCKING		( 1<<3 )		// In the process of ducking, but totally squatted yet
#define		PFLAG_USING			( 1<<4 )		// Using a continuous entity
#define		PFLAG_OBSERVER		( 1<<5 )		// player is locked in stationary cam mode. Spectators can move, observers can't.

//
// generic player
//
//-----------------------------------------------------
//This is Half-Life player entity
//-----------------------------------------------------
#define CSUITPLAYLIST	4		// max of 4 suit sentences queued up at any time

#define SUIT_GROUP			TRUE
#define	SUIT_SENTENCE		FALSE

#define	SUIT_REPEAT_OK		0
#define SUIT_NEXT_IN_30SEC	30
#define SUIT_NEXT_IN_1MIN	60
#define SUIT_NEXT_IN_5MIN	300
#define SUIT_NEXT_IN_10MIN	600
#define SUIT_NEXT_IN_30MIN	1800
#define SUIT_NEXT_IN_1HOUR	3600

#define CSUITNOREPEAT		32

#define	SOUND_FLASHLIGHT_ON		"items/flashlight1.wav"
#define	SOUND_FLASHLIGHT_OFF	"items/flashlight1.wav"

#define TEAM_NAME_LENGTH	16

typedef enum
{
   PLAYER_DUMMY,
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
   PLAYER_RELOAD
} PLAYER_ANIM;


#define STATUS_PLAYER_CHOOSETEAM          1
#define STATUS_PLAYER_CHOOSEGROUP         2
#define STATUS_PLAYER_BUYWEAPONMENU       3
#define STATUS_PLAYER_BUYPISTOLMENU       4
#define STATUS_PLAYER_BUYSHOTGUNMENU      5
#define STATUS_PLAYER_BUYSUBMENU          6
#define STATUS_PLAYER_BUYAUTOMENU         7
#define STATUS_PLAYER_BUYRIFLEMENU        8
#define STATUS_PLAYER_BUYSPECIALTYGUNMENU 9
#define STATUS_PLAYER_BUYITEMMENU         10
#define STATUS_PLAYER_STATISTICSMENU      11
#define STATUS_PLAYER_HISTORYMENU         12
#define STATUS_PLAYER_CONFIGMENU          13
#define STATUS_PLAYER_AUDIOMENU1          14
#define STATUS_PLAYER_AUDIOMENU2          15


#define CF_JUST_CONNECTED                 1
#define CF_NEED_SERVER_VARIABLES          2

#define OBS_CHASE_LOCKED		1
#define OBS_CHASE_FREE			2
#define OBS_ROAMING			   3
#define OBS_IN_EYE            4
#define OBS_MAP_FREE          5
#define OBS_MAP_CHASE         6

typedef struct 
   {
   int   Kills[48];
   int   Deaths[48];
   int   Shots;
   int   Hits;
   int   MaxRoundKills;
   int   MaxRoundLives;
   int   RoundKills;
   int   s0_index;
   int   s1_index;
   int   Rounds;
   int   Offense;
   int   Defense;
   int   Suicides;
   int   TimeOn;
   int   LastKills;
   int   KnifeKills;
   int   HeadShots;
   int   ping_sum;
   int   ping_samples;
   int   ping_rounds;
   int   ping_average;
   float ping_lasttime;
   float s0_lasttime;
   float s1_lasttime;
   } pstat_t;

typedef struct 
   {
   short ClientID;
   short Juice;
   short Health;
   short Shots;
   short Hits;
   short MaxRoundKills;
   short MaxRoundLives;
   short RoundKills;
   short Rounds;
   short Offense;
   short Defense;
   short Suicides;
   short TimeOn;
   short LastKills;
   short Frags;
   short Deaths;
   short KnifeKills;
   short HeadShots;
   short FavPrimary;
   short FavSecondary;
   short PrimaryKills;
   short SecondaryKills;
   unsigned int WonID;
   char SteamID[64];
   } sendstat_t;



class CBasePlayer : public CBaseMonster
{
public:
	int					random_seed;    // See that is shared between client & server for shared weapons code

	int					m_iPlayerSound;// the index of the sound list slot reserved for this player
	int					m_iTargetVolume;// ideal sound volume. 
	int					m_iWeaponVolume;// how loud the player's weapon is right now.
	int					m_iExtraSoundTypes;// additional classification for this weapon's sound
	int					m_iWeaponFlash;// brightness of the weapon flash
	float				m_flStopExtraSoundTime;
	
	float				m_flFlashLightTime;	// Time until next battery draw/Recharge
	int					m_iFlashBattery;		// Flashlight Battery Draw

	int					m_afButtonLast;
	int					m_afButtonPressed;
	int					m_afButtonReleased;
	
	edict_t			   *m_pentSndLast;			// last sound entity to modify player room type
	float				m_flSndRoomtype;		// last roomtype set by sound entity
	float				m_flSndRange;			// dist from player to sound entity

	float				m_flFallVelocity;
	
	int					m_rgItems[MAX_ITEMS];
	int					m_fKnownItem;		// True when a new item needs to be added
	int					m_fNewAmmo;			// True when a new item has been added

	unsigned int		m_afPhysicsFlags;	// physics flags - set when 'normal' physics should be revisited or overriden
	float				m_fNextSuicideTime; // the time after which the player can next use the suicide command


// these are time-sensitive things that we keep track of
	float				m_flTimeStepSound;	// when the last stepping sound was made
	float				m_flTimeWeaponIdle; // when to play another weapon idle animation.
	float				m_flSwimTime;		// how long player has been underwater
	float				m_flDuckTime;		// how long we've been ducking
	float				m_flWallJumpTime;	// how long until next walljump

	float				m_flSuitUpdate;					// when to play next suit update
	int					m_rgSuitPlayList[CSUITPLAYLIST];// next sentencenum to play for suit update
	int					m_iSuitPlayNext;				// next sentence slot for queue storage;
	int					m_rgiSuitNoRepeat[CSUITNOREPEAT];		// suit sentence no repeat list
	float				m_rgflSuitNoRepeatTime[CSUITNOREPEAT];	// how long to wait before allowing repeat
	int					m_lastDamageAmount;		// Last damage taken
	float				m_tbdPrev;				// Time-based damage timer

	float				m_flgeigerRange;		// range to nearest radiation source
	float				m_flgeigerDelay;		// delay per update of range msg to client
	int					m_igeigerRangePrev;
	int					m_iStepLeft;			// alternate left/right foot stepping sound
	char				m_szTextureName[CBTEXTURENAMEMAX];	// current texture name we're standing on
	char				m_chTextureType;		// current texture type

	int					m_idrowndmg;			// track drowning damage taken
	int					m_idrownrestored;		// track drowning damage restored

	int					m_bitsHUDDamage;		// Damage bits for the current fame. These get sent to 
												// the hude via the DAMAGE message
	BOOL				m_fInitHUD;				// True when deferred HUD restart msg needs to be sent
	BOOL				m_fGameHUDInitialized;
	int					m_iTrain;				// Train control position
	BOOL				m_fWeapon;				// Set this to FALSE to force a reset of the current weapon HUD info

	EHANDLE				m_pTank;				// the tank which the player is currently controlling,  NULL if no tank
	float				m_fDeadTime;			// the time at which the player died  (used in PlayerDeathThink())

	BOOL			m_fNoPlayerSound;	// a debugging feature. Player makes no sound if this is true. 
	BOOL			m_fLongJump; // does this player have the longjump module?

	float       m_tSneaking;
	int			m_iUpdateTime;		// stores the number of frame ticks before sending HUD update messages
	int			m_iClientHealth;	// the health currently known by the client.  If this changes, send a new
	int			m_iClientBattery;	// the Battery currently known by the client.  If this changes, send a new
	int			m_iHideHUD;		// the players hud weapon info is to be hidden
	int			m_iClientHideHUD;
	int			m_iFOV;			// field of view
	int			m_iClientFOV;	// client's known FOV
	// usable player items 
	CBasePlayerItem	*m_rgpPlayerItems[MAX_ITEM_TYPES];
	CBasePlayerItem *m_pActiveItem;
	CBasePlayerItem *m_pClientActiveItem;  // client version of the active item
	CBasePlayerItem *m_pLastItem;
	// shared ammo slots
	int	m_rgAmmo[MAX_AMMO_SLOTS];
	int	m_rgAmmoLast[MAX_AMMO_SLOTS];

	int					m_iDeaths;
	float				m_iRespawnFrames;	// used in PlayerDeathThink() to make sure players can always respawn

	int m_lastx, m_lasty;  // These are the previous update's crosshair angles, DON"T SAVE/RESTORE

	int m_nCustomSprayFrames;// Custom clan logo frames for this player
	float	m_flNextDecalTime;// next time this player can spray a decal

	char m_szTeamName[TEAM_NAME_LENGTH];
   char m_szModelName[TEAM_NAME_LENGTH];

	virtual void Spawn( void );
	void Pain( void );

//	virtual void Think( void );
	virtual void Jump( void );
	virtual void Duck( void );
	virtual void PreThink( void );
	virtual void PostThink( void );
	virtual Vector GetGunPosition( void );
	virtual int TakeHealth( float flHealth, int bitsDamageType );
	virtual void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	virtual void	Killed( entvars_t *pevAttacker, int iGib );
	virtual Vector BodyTarget( const Vector &posSrc ) { return Center( ) + pev->view_ofs * RANDOM_FLOAT( 0.5, 1.1 ); };		// position to shoot at
	virtual void StartSneaking( void ) { m_tSneaking = gpGlobals->time - 1; }
	virtual void StopSneaking( void ) { m_tSneaking = gpGlobals->time + 30; }
	virtual BOOL IsSneaking( void ) { return m_tSneaking <= gpGlobals->time; }
	virtual BOOL IsAlive( void ) { return (pev->deadflag == DEAD_NO) && pev->health > 0; }
	virtual	BOOL IsPlayer( void ) { return TRUE; }			// Spectators should return FALSE for this, they aren't "players" as far as game logic is concerned

	virtual BOOL IsNetClient( void ) { return TRUE; }		// Bots should return FALSE for this, they can't receive NET messages
															// Spectators should return TRUE for this
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	void RenewItems(void);
	void PackDeadPlayerItems( void );
	void RemoveAllItems( BOOL removeSuit );
	BOOL SwitchWeapon( CBasePlayerItem *pWeapon );

	// JOHN:  sends custom messages if player HUD data has changed  (eg health, ammo)
	virtual void UpdateClientData( void );
	
	static	TYPEDESCRIPTION m_playerSaveData[];

	// Player is moved across the transition by other means
	virtual int		ObjectCaps( void ) { return CBaseMonster :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual void	Precache( void );
	BOOL			IsOnLadder( void );
	BOOL			FlashlightIsOn( void );
	void			FlashlightTurnOn( void );
	void			FlashlightTurnOff( void );
	
	void UpdatePlayerSound ( void );
	void DeathSound ( void );

	int Classify ( void );
	void SetAnimation( PLAYER_ANIM playerAnim );
	void SetWeaponAnimType( const char *szExtention );
	char m_szAnimExtention[32];

	// custom player functions
	virtual void ImpulseCommands( void );
	void CheatImpulseCommands( int iImpulse );

	void StartDeathCam( void );
	//void StartObserver( Vector vecPosition, Vector vecViewAngle );

	BOOL AddPlayerItem( CBasePlayerItem *pItem );
	BOOL RemovePlayerItem( CBasePlayerItem *pItem );
	void DropPlayerItem ( char *pszItemName );
	BOOL HasPlayerItem( CBasePlayerItem *pCheckItem );
	BOOL HasNamedPlayerItem( const char *pszItemName );
	BOOL HasWeapons( void );// do I have ANY weapons?
	void SelectPrevItem( int iItem );
	void SelectNextItem( int iItem );
	void SelectLastItem(void);
	void SelectItem(const char *pstr);
	void ItemPreFrame( void );
	void ItemPostFrame( void );
	void GiveNamedItem( const char *szName );
	void EnableControl(BOOL fControl);

	int  GiveAmmo( int iAmount, char *szName, int iMax );
	void SendAmmoUpdate(void);

	void WaterMove( void );
	void EXPORT PlayerDeathThink( void );
	void PlayerUse( void );

	void CheckSuitUpdate();
	void SetSuitUpdate(char *name, int fgroup, int iNoRepeat);
	void CheckTimeBasedDamage( void );
	void UpdateStepSound( void );
	void PlayStepSound(int step, float fvol);

	BOOL FBecomeProne ( void );
	void BarnacleVictimBitten ( entvars_t *pevBarnacle );
	void BarnacleVictimReleased ( void );
	static int GetAmmoIndex(const char *psz);
	int AmmoInventory( int iAmmoIndex );
	int Illumination( void );

	void ForceClientDllUpdate( void );  // Forces all client .dll specific data to be resent to client.

	void DeathMessage( entvars_t *pevKiller );

	void SetCustomDecalFrames( int nFrames );
   void SpectatorSpawn();
	int GetCustomDecalFrames( void );

   int m_iPlayerStatus;
   int m_iTeam;
   int m_iMoney;
   int m_iNextMoney;
   int m_iLicense, m_iOldLicense;
   int m_iJuice;
   int m_iPoints;
   int m_iGroup;
   int m_iMenu;
   int m_iStealth;
   int m_iGrenades;
   int m_iClientFlag;
   unsigned int m_iWONID;
   unsigned int m_iTargetWONID;
   char m_iSteamID[64];
   char m_iTargetSteamID[64];
   
   int m_iNearArmory;
   float m_fNextUpdateArmory;
   float m_fNextUpdateTimeLeft;
   float m_fNextIdentifyEntity;
   float m_fShowTeamSelectTime;
   float m_flEnableControlTime;
   float m_flGodModeTime;
   
   void StartObserver( Vector vecPosition, Vector vecViewAngle );
   void StopObserver( void );
   void Observer_FindNextPlayer( bool bReverse );
   void Observer_HandleButtons();
   void Observer_SetMode( int iMode );
	void Radio(int RadioCommand, int iTeam);
   void Speak(int soundindex);
   void ShowVGUIMenu(int iMenuID);
   void SendMyStats(int iId);
   void SendTop32();
   void Slap(float damage);
   void PlayBeat(int beat);
   void PlayMp3(char *filename, int loop);
   void PlayTheme(int theme);
   int  HighPingCheck();
   void SendServerVars();
   void SendJustConnected();

   EHANDLE	m_hObserverTarget;
	float	m_flNextObserverInput;
	float	m_flNextNANABlip;
   float m_flNextMapVote;
   int   m_iNANA;
   int   m_iKilled;
   int   m_iGrenade;
   float m_flNextID;
   int   m_ilast_ID;;
   int   m_iAdmin;
   int   m_iTagged;
   int   m_iMaxSpeed;
   int   m_iLastMaxSpeed;
   float m_fLastJumped;
   int   m_iJumpCount;
   int   m_iPrimary;
   int   m_iSecondary;
   int   m_iStatClientID;
   int   m_iVoteMap;
   int   m_iLastSay;
   int   m_fNextLastSay;
   float m_fNextCheat;
   unsigned short int m_iAdminTorture;
   pstat_t  stat;
};


#define AUTOAIM_2DEGREES  0.0348994967025
#define AUTOAIM_5DEGREES  0.08715574274766
#define AUTOAIM_8DEGREES  0.1391731009601
#define AUTOAIM_10DEGREES 0.1736481776669


extern int	gmsgHudText;
extern BOOL gInitHUD;

#endif // PLAYER_H
