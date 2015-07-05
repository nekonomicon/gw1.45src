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

===== world.cpp ========================================================

  precaches and defs for entities and other data that must always be available.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "soundent.h"
#include "client.h"
#include "decals.h"
#include "effects.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"

#define B92D_MODULE
#define MAC10_MODULE
#define TMP_MODULE
#define MP5_MODULE
#define UMP45_MODULE
#define AK47_MODULE
#define M16_MODULE
#define SG552_MODULE
#define AUG_MODULE
#define SG550_MODULE

#include "sdk_u_main.h"
#include "sdk_e_menu.h"
#include "sdk_c_global.h"
#include "sdk_w_global.h"

extern CSoundEnt *pSoundEnt;

int        map_vote_count;
map_vote_t map_vote[MAX_MAP_VOTES];
extern CBaseEntity				*g_pLastSpawn;
DLL_GLOBAL edict_t				*g_pBodyQueueHead;
CGlobalState					gGlobalState;
extern DLL_GLOBAL	int			gDisplayTitle;

admin_database_t adb[MAX_ADMINS];
int num_admins;
char admin_reject_message[512];
extern void W_Precache(void);

//
// This must match the list in util.h
//
DLL_DECALLIST gDecals[] = {
	{ "{shot1",	0 },		// DECAL_GUNSHOT1 
	{ "{shot2",	0 },		// DECAL_GUNSHOT2
	{ "{shot3",0 },			// DECAL_GUNSHOT3
	{ "{shot4",	0 },		// DECAL_GUNSHOT4
	{ "{shot5",	0 },		// DECAL_GUNSHOT5
	{ "{lambda01", 0 },		// DECAL_LAMBDA1
	{ "{lambda02", 0 },		// DECAL_LAMBDA2
	{ "{lambda03", 0 },		// DECAL_LAMBDA3
	{ "{lambda04", 0 },		// DECAL_LAMBDA4
	{ "{lambda05", 0 },		// DECAL_LAMBDA5
	{ "{lambda06", 0 },		// DECAL_LAMBDA6
	{ "{scorch1", 0 },		// DECAL_SCORCH1
	{ "{scorch2", 0 },		// DECAL_SCORCH2
	{ "{blood1", 0 },		// DECAL_BLOOD1
	{ "{blood2", 0 },		// DECAL_BLOOD2
	{ "{blood3", 0 },		// DECAL_BLOOD3
	{ "{blood4", 0 },		// DECAL_BLOOD4
	{ "{blood5", 0 },		// DECAL_BLOOD5
	{ "{blood6", 0 },		// DECAL_BLOOD6
	{ "{yblood1", 0 },		// DECAL_YBLOOD1
	{ "{yblood2", 0 },		// DECAL_YBLOOD2
	{ "{yblood3", 0 },		// DECAL_YBLOOD3
	{ "{yblood4", 0 },		// DECAL_YBLOOD4
	{ "{yblood5", 0 },		// DECAL_YBLOOD5
	{ "{yblood6", 0 },		// DECAL_YBLOOD6
	{ "{break1", 0 },		// DECAL_GLASSBREAK1
	{ "{break2", 0 },		// DECAL_GLASSBREAK2
	{ "{break3", 0 },		// DECAL_GLASSBREAK3
	{ "{bigshot1", 0 },		// DECAL_BIGSHOT1
	{ "{bigshot2", 0 },		// DECAL_BIGSHOT2
	{ "{bigshot3", 0 },		// DECAL_BIGSHOT3
	{ "{bigshot4", 0 },		// DECAL_BIGSHOT4
	{ "{bigshot5", 0 },		// DECAL_BIGSHOT5
	{ "{spit1", 0 },		// DECAL_SPIT1
	{ "{spit2", 0 },		// DECAL_SPIT2
	{ "{bproof1", 0 },		// DECAL_BPROOF1
	{ "{gargstomp", 0 },	// DECAL_GARGSTOMP1,	// Gargantua stomp crack
	{ "{smscorch1", 0 },	// DECAL_SMALLSCORCH1,	// Small scorch mark
	{ "{smscorch2", 0 },	// DECAL_SMALLSCORCH2,	// Small scorch mark
	{ "{smscorch3", 0 },	// DECAL_SMALLSCORCH3,	// Small scorch mark
	{ "{mommablob", 0 },	// DECAL_MOMMABIRTH		// BM Birth spray
	{ "{mommablob", 0 },	// DECAL_MOMMASPLAT		// BM Mortar spray?? need decal
};

/*
==============================================================================

BODY QUE

==============================================================================
*/

#define SF_DECAL_NOTINDEATHMATCH		2048

class CDecal : public CBaseEntity
{
public:
	void	Spawn( void );
	void	KeyValue( KeyValueData *pkvd );
	void	EXPORT StaticDecal( void );
	void	EXPORT TriggerDecal( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};

LINK_ENTITY_TO_CLASS( infodecal, CDecal );

// UNDONE:  These won't get sent to joining players in multi-player
void CDecal :: Spawn( void )
{
	if ( pev->skin < 0 || (gpGlobals->deathmatch && FBitSet( pev->spawnflags, SF_DECAL_NOTINDEATHMATCH )) )
	{
		REMOVE_ENTITY(ENT(pev));
		return;
	}

	if ( FStringNull ( pev->targetname ) )
	{
	// swmpdg-added &CDecal::
		SetThink( &CDecal::StaticDecal );
		// if there's no targetname, the decal will spray itself on as soon as the world is done spawning.
		pev->nextthink = gpGlobals->time;
	}
	else
	{
		// if there IS a targetname, the decal sprays itself on when it is triggered.
		// swmpdg-added &CDecal::
		SetThink ( &CDecal::SUB_DoNothing );
		SetUse( &CDecal::TriggerDecal );
	}
}

void CDecal :: TriggerDecal ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// this is set up as a USE function for infodecals that have targetnames, so that the
	// decal doesn't get applied until it is fired. (usually by a scripted sequence)
	TraceResult trace;
	int			entityIndex;

	UTIL_TraceLine( pev->origin - Vector(5,5,5), pev->origin + Vector(5,5,5),  ignore_monsters, ENT(pev), &trace );

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE( TE_BSPDECAL );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( (int)pev->skin );
		entityIndex = (short)ENTINDEX(trace.pHit);
		WRITE_SHORT( entityIndex );
		if ( entityIndex )
			WRITE_SHORT( (int)VARS(trace.pHit)->modelindex );
	MESSAGE_END();
	// swmpdg-added &CDecal::
	SetThink( &CDecal::SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1;
}


void CDecal :: StaticDecal( void )
{
	TraceResult trace;
	int			entityIndex, modelIndex;

	UTIL_TraceLine( pev->origin - Vector(5,5,5), pev->origin + Vector(5,5,5),  ignore_monsters, ENT(pev), &trace );

	entityIndex = (short)ENTINDEX(trace.pHit);
	if ( entityIndex )
		modelIndex = (int)VARS(trace.pHit)->modelindex;
	else
		modelIndex = 0;

	g_engfuncs.pfnStaticDecal( pev->origin, (int)pev->skin, entityIndex, modelIndex );

	SUB_Remove();
}


void CDecal :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "texture"))
	{
		pev->skin = DECAL_INDEX( pkvd->szValue );
		
		// Found
		if ( pev->skin >= 0 )
			return;
		ALERT( at_console, "Can't find decal %s\n", pkvd->szValue );
	}
	else
		CBaseEntity::KeyValue( pkvd );
}


// Body queue class here.... It's really just CBaseEntity
class CCorpse : public CBaseEntity
{
	virtual int ObjectCaps( void ) { return FCAP_DONT_SAVE; }	

   virtual void Think( void ) {pev->effects = EF_NODRAW;}
};

LINK_ENTITY_TO_CLASS( bodyque, CCorpse );

static void InitBodyQue(void)
{
	string_t	istrClassname = MAKE_STRING("bodyque");

	g_pBodyQueueHead = CREATE_NAMED_ENTITY( istrClassname );
	entvars_t *pev = VARS(g_pBodyQueueHead);
	
	// Reserve 3 more slots for dead bodies

   // !!FIXME!!
	for ( int i = 0; i < 3; i++ )
	{
		pev->owner = CREATE_NAMED_ENTITY( istrClassname );
		pev = VARS(pev->owner);
	}
	
	pev->owner = g_pBodyQueueHead;
}


//
// make a body que entry for the given ent so the ent can be respawned elsewhere
//
// GLOBALS ASSUMED SET:  g_eoBodyQueueHead
//
void CopyToBodyQue(entvars_t *pev) 
{
	if (pev->effects & EF_NODRAW)
		return;

	entvars_t *pevHead	= VARS(g_pBodyQueueHead);

	pevHead->angles	   = pev->angles;
	pevHead->model		   = pev->model;
	pevHead->modelindex	= pev->modelindex;
	pevHead->colormap	   = pev->colormap;
	pevHead->velocity	   = pev->velocity;
	pevHead->flags		   = 0;
	pevHead->deadflag	   = pev->deadflag;
	pevHead->renderfx	   = kRenderFxDeadPlayer;
	pevHead->renderamt	= ENTINDEX( ENT( pev ) );

	pevHead->effects     = pev->effects | EF_NOINTERP;
	//pevHead->goalstarttime = pev->goalstarttime;
	//pevHead->goalframe	= pev->goalframe;
	//pevHead->goalendtime = pev->goalendtime ;
	
	pevHead->frame		   = pev->frame;
	pevHead->sequence    = pev->sequence;
	pevHead->animtime    = pev->animtime;
   pevHead->gaitsequence = 0;
   pevHead->framerate = 0;

   // !!FIXME!!
   pevHead->nextthink = gpGlobals->time + 3;

	UTIL_SetOrigin(pevHead, pev->origin);
   if (pev->gaitsequence == 94)
      {
      //ClientPrint(pev, HUD_PRINTTALK, "Crouched\n");
      pevHead->mins = Vector( 0, 0, 0 );
		pevHead->maxs = Vector( 0, 0, 0 );
	   pevHead->movetype	= MOVETYPE_NONE;
      }
   else   
      {
	   UTIL_SetSize(pevHead, pev->mins, pev->maxs);
	   pevHead->movetype	= MOVETYPE_TOSS;
      }
	g_pBodyQueueHead = pevHead->owner;
}


CGlobalState::CGlobalState( void )
{
	Reset();
}

void CGlobalState::Reset( void )
{
	m_pList = NULL; 
	m_listCount = 0;
}

globalentity_t *CGlobalState :: Find( string_t globalname )
{
	if ( !globalname )
		return NULL;

	globalentity_t *pTest;
	const char *pEntityName = STRING(globalname);

	
	pTest = m_pList;
	while ( pTest )
	{
		if ( FStrEq( pEntityName, pTest->name ) )
			break;
	
		pTest = pTest->pNext;
	}

	return pTest;
}


// This is available all the time now on impulse 104, remove later
//#ifdef _DEBUG
void CGlobalState :: DumpGlobals( void )
{
	static char *estates[] = { "Off", "On", "Dead" };
	globalentity_t *pTest;

	ALERT( at_console, "-- Globals --\n" );
	pTest = m_pList;
	while ( pTest )
	{
		ALERT( at_console, "%s: %s (%s)\n", pTest->name, pTest->levelName, estates[pTest->state] );
		pTest = pTest->pNext;
	}
}
//#endif


void CGlobalState :: EntityAdd( string_t globalname, string_t mapName, GLOBALESTATE state )
{
	ASSERT( !Find(globalname) );

	globalentity_t *pNewEntity = (globalentity_t *)calloc( sizeof( globalentity_t ), 1 );
	ASSERT( pNewEntity != NULL );
	pNewEntity->pNext = m_pList;
	m_pList = pNewEntity;
	strcpy( pNewEntity->name, STRING( globalname ) );
	strcpy( pNewEntity->levelName, STRING(mapName) );
	pNewEntity->state = state;
	m_listCount++;
}


void CGlobalState :: EntitySetState( string_t globalname, GLOBALESTATE state )
{
	globalentity_t *pEnt = Find( globalname );

	if ( pEnt )
		pEnt->state = state;
}


const globalentity_t *CGlobalState :: EntityFromTable( string_t globalname )
{
	globalentity_t *pEnt = Find( globalname );

	return pEnt;
}


GLOBALESTATE CGlobalState :: EntityGetState( string_t globalname )
{
	globalentity_t *pEnt = Find( globalname );
	if ( pEnt )
		return pEnt->state;

	return GLOBAL_OFF;
}


// Global Savedata for Delay
TYPEDESCRIPTION	CGlobalState::m_SaveData[] = 
{
	DEFINE_FIELD( CGlobalState, m_listCount, FIELD_INTEGER ),
};

// Global Savedata for Delay
TYPEDESCRIPTION	gGlobalEntitySaveData[] = 
{
	DEFINE_ARRAY( globalentity_t, name, FIELD_CHARACTER, 64 ),
	DEFINE_ARRAY( globalentity_t, levelName, FIELD_CHARACTER, 32 ),
	DEFINE_FIELD( globalentity_t, state, FIELD_INTEGER ),
};


int CGlobalState::Save( CSave &save )
{
	int i;
	globalentity_t *pEntity;

	if ( !save.WriteFields( "GLOBAL", this, m_SaveData, ARRAYSIZE(m_SaveData) ) )
		return 0;
	
	pEntity = m_pList;
	for ( i = 0; i < m_listCount && pEntity; i++ )
	{
		if ( !save.WriteFields( "GENT", pEntity, gGlobalEntitySaveData, ARRAYSIZE(gGlobalEntitySaveData) ) )
			return 0;

		pEntity = pEntity->pNext;
	}
	
	return 1;
}

int CGlobalState::Restore( CRestore &restore )
{
	int i, listCount;
	globalentity_t tmpEntity;


	ClearStates();
	if ( !restore.ReadFields( "GLOBAL", this, m_SaveData, ARRAYSIZE(m_SaveData) ) )
		return 0;
	
	listCount = m_listCount;	// Get new list count
	m_listCount = 0;				// Clear loaded data

	for ( i = 0; i < listCount; i++ )
	{
		if ( !restore.ReadFields( "GENT", &tmpEntity, gGlobalEntitySaveData, ARRAYSIZE(gGlobalEntitySaveData) ) )
			return 0;
		EntityAdd( MAKE_STRING(tmpEntity.name), MAKE_STRING(tmpEntity.levelName), tmpEntity.state );
	}
	return 1;
}

void CGlobalState::EntityUpdate( string_t globalname, string_t mapname )
{
	globalentity_t *pEnt = Find( globalname );

	if ( pEnt )
		strcpy( pEnt->levelName, STRING(mapname) );
}


void CGlobalState::ClearStates( void )
{
	globalentity_t *pFree = m_pList;
	while ( pFree )
	{
		globalentity_t *pNext = pFree->pNext;
		free( pFree );
		pFree = pNext;
	}
	Reset();
}


void SaveGlobalState( SAVERESTOREDATA *pSaveData )
{
	CSave saveHelper( pSaveData );
	gGlobalState.Save( saveHelper );
}


void RestoreGlobalState( SAVERESTOREDATA *pSaveData )
{
	CRestore restoreHelper( pSaveData );
	gGlobalState.Restore( restoreHelper );
}


void ResetGlobalState( void )
{
	gGlobalState.ClearStates();
	gInitHUD = TRUE;	// Init the HUD on a new game / load game
}

// moved CWorld class definition to cbase.h
//=======================
// CWorld
//
// This spawns first when each level begins.
//=======================

LINK_ENTITY_TO_CLASS( worldspawn, CWorld );

#define SF_WORLD_DARK		0x0001		// Fade from black at startup
#define SF_WORLD_TITLE		0x0002		// Display game title at startup
#define SF_WORLD_FORCETEAM	0x0004		// Force teams

extern DLL_GLOBAL BOOL		g_fGameOver;
float g_flWeaponCheat; 

float map_bag_position[5][3];
int map_bags;

int LoadBagPositions()
{
   FILE *fp;
   int xmap_bags = 0;
   float *x;
   float a, b, c;
   int k = 0, index;
   char xfilename[256];
   char xin[256], tag[256], ch;
   memset(&aw, 0, sizeof(aw));
   aw.upgradeprice = 2700;
   aw.damage = 1;
   aw.baselicense = 8;
   aw.pistols = 7;
   aw.shotguns = 3;
   aw.subs = 15;
   aw.autos = 31;
   aw.items = 0;
   aw.headshot = 3.0f;
   aw.grenade = 8.0f;
   aw.grenades = 0;
   aw.respawntime = 1;
   aw.respawn_min = 4;
   aw.respawns = 2;
   aw.maxmoney = 12000;
   aw.winmoney = 1100;
   aw.losemoney = 900;
   aw.drawmoney = 500;
   aw.killmoney = 250;
   aw.respawnmoney = 300;
   aw.lastkillmoney = 400;
   aw.anti_cheats = GW_AC_BUNNYHOP;
   aw.timelimit = CVAR_GET_FLOAT("mp_timelimit");
   aw.speed = 1;
   aw.recoil = .75;
   aw.kickback = 10;
   aw.fadeout = 5;
   aw.freebies = 0;
   aw.quiet = 0;
   memset(aw.nextmap, 0, 63);
   sprintf (xfilename, "gangwars/maps/%s.frm", (char *)STRING(gpGlobals->mapname));
   fp = fopen (xfilename, "r");
   if (!fp)
      {
      sprintf (xfilename, "gangwars/maps/genericmap.frm");
      fp = fopen (xfilename, "r");
      }
   if (!fp) return 0;
   while (!feof(fp))
      {
      index = 0;
      xin[0] = 0;     
      do
         {
         ch = fgetc(fp);
         if (!ch || ch == '\n') break;
         if (index < 128) xin[index++] = ch;
         } while (!feof(fp));
      xin[index] = 0;
      if (xin[0] == 'g' && xin[1] == 'w')
         {
         sscanf (xin, "%s", tag);
         if (!strcmp(tag, "gw_bag1"))
            {
            sscanf(xin, "%s %f %f %f", tag, &a, &b, &c);
            x = &map_bag_position[0][0];
            x[0] = a; x[1] = b; x[2] = c;
            xmap_bags++;
            }
         else if (!strcmp(tag, "gw_bag2"))
            {
            sscanf(xin, "%s %f %f %f", tag, &a, &b, &c);
            x = &map_bag_position[1][0];
            x[0] = a; x[1] = b; x[2] = c;
            xmap_bags++;
            }
         else if (!strcmp(tag, "gw_pistols"))
            sscanf(xin, "%s %i", tag, &aw.pistols);
         else if (!strcmp(tag, "gw_shotguns"))
            sscanf(xin, "%s %i", tag, &aw.shotguns);
         else if (!strcmp(tag, "gw_subs"))
            sscanf(xin, "%s %i", tag, &aw.subs);
         else if (!strcmp(tag, "gw_autos"))
            sscanf(xin, "%s %i", tag, &aw.autos);
         else if (!strcmp(tag, "gw_items"))
            sscanf(xin, "%s %i", tag, &aw.items);
         else if (!strcmp(tag, "gw_roundtime"))
            sscanf(xin, "%s %i", tag, &map_stat.g_iRoundTime);
         else if (!strcmp(tag, "gw_baselicense"))
            sscanf(xin, "%s %i", tag, &aw.baselicense);
         else if (!strcmp(tag, "gw_basemoney"))
            sscanf(xin, "%s %i", tag, &aw.basemoney);
         else if (!strcmp(tag, "gw_damage"))
            sscanf(xin, "%s %f", tag, &aw.damage);
         else if (!strcmp(tag, "gw_grenade"))
            sscanf(xin, "%s %f", tag, &aw.grenade);
         else if (!strcmp(tag, "gw_timelimit"))
            sscanf(xin, "%s %f", tag, &aw.timelimit);
         else if (!strcmp(tag, "gw_nextmap"))
            sscanf(xin, "%s %s", tag, aw.nextmap);
         else if (!strcmp(tag, "gw_headshot"))
            sscanf(xin, "%s %f", tag, &aw.headshot);
         else if (!strcmp(tag, "gw_grenades"))
            sscanf(xin, "%s %i", tag, &aw.grenades);
         else if (!strcmp(tag, "gw_respawns"))
            sscanf(xin, "%s %i", tag, &aw.respawntime);
         else if (!strcmp(tag, "gw_respawntime"))
            sscanf(xin, "%s %i", tag, &aw.respawns);
         else if (!strcmp(tag, "gw_respawn_min"))
            sscanf(xin, "%s %i", tag, &aw.respawn_min);         
         else if (!strcmp(tag, "gw_winmoney"))
            sscanf(xin, "%s %i", tag, &aw.winmoney);         
         else if (!strcmp(tag, "gw_losemoney"))
            sscanf(xin, "%s %i", tag, &aw.losemoney);         
         else if (!strcmp(tag, "gw_killmoney"))
            sscanf(xin, "%s %i", tag, &aw.killmoney);         
         else if (!strcmp(tag, "gw_lastkillmoney"))
            sscanf(xin, "%s %i", tag, &aw.lastkillmoney);         
         else if (!strcmp(tag, "gw_respawnmoney"))
            sscanf(xin, "%s %i", tag, &aw.respawnmoney);         
         else if (!strcmp(tag, "gw_drawmoney"))
            sscanf(xin, "%s %i", tag, &aw.drawmoney);         
         else if (!strcmp(tag, "gw_maxmoney"))
            sscanf(xin, "%s %i", tag, &aw.maxmoney);       
         else if (!strcmp(tag, "gw_recoil"))
            sscanf(xin, "%s %f", tag, &aw.recoil);       
         else if (!strcmp(tag, "gw_speed"))
            sscanf(xin, "%s %f", tag, &aw.speed);         
         else if (!strcmp(tag, "gw_kickback"))
            sscanf(xin, "%s %f", tag, &aw.kickback);         
         else if (!strcmp(tag, "gw_fadeout"))
            sscanf(xin, "%s %f", tag, &aw.fadeout);         
         else if (!strcmp(tag, "gw_freebies"))
            sscanf(xin, "%s %i", tag, &aw.freebies);
         else if (!strcmp(tag, "gw_quiet"))
            sscanf(xin, "%s %i", tag, &aw.quiet);
         }
      }
   if (aw.kickback > 20) aw.kickback = 20;
   CVAR_SET_FLOAT("mp_timelimit", aw.timelimit);
   for (k = 0; k < MAX_WEAPONS; k++)
      {
      w_stat[k].damage1 *= aw.damage;
      w_stat[k].damage2 *= aw.damage;
      w_stat[k].recoil_pitch *= aw.recoil;
      w_stat[k].recoil_yaw *= aw.recoil;
      }
   old_aw = aw;
   fclose(fp);
   return xmap_bags;   
}

void LoadAdminInfo()
{
   FILE *fp;
   int index;
   char xin[256], tag[256], ch, *pxin;
   memset(&adb, 0, sizeof(admin_database_t) * MAX_ADMINS);
   num_admins = 0;
   strcpy(admin_reject_message, "INVALID AUTHENTICATION:\n");

   fp = fopen ("gangwars/admins.txt", "r");
   if (!fp) return;
   while (!feof(fp))
      {
      index = 0;
      xin[0] = 0;     
      do
         {
         ch = fgetc(fp);
         if (!ch || ch == '\n') break;
         if (index < 128) xin[index++] = ch;
         } while (!feof(fp));
      xin[index] = 0;
      if (xin[0] == 'g' && xin[1] == 'w')
         {
         sscanf (xin, "%s", tag);
         pxin = xin + strlen(tag) + 1;
         if (!strcmp(tag, "gw_fail"))
            {
            strcat(admin_reject_message, pxin);
            }
         else if (!strcmp(tag, "gw_admin"))
            {
            sscanf(pxin, "%u %s %i %s", &adb[num_admins].WONID, 
                                    adb[num_admins].password, 
                                    &adb[num_admins].level,
                                    &adb[num_admins].SteamID);
            num_admins++;
            }
         }
      }
   strcat(admin_reject_message, "\n");
   fclose(fp);
}

int LoadVoteMap()
{
   FILE *fp;
   int k = 0, index;
   char xin[256], ch;
   map_vote_count = 0;
   memset(map_vote, 0, MAX_MAP_VOTES * sizeof(map_vote_t));
   fp = fopen ("gangwars/maps/votemaps.txt", "r");
   if (!fp) return 0;
   while (!feof(fp) && k < MAX_MAP_VOTES)
      {
      index = 0;
      xin[0] = 0;     
      do
         {
         ch = fgetc(fp);
         if (!ch || ch == '\n') break;
         if (index < 32) xin[index++] = ch;
         } while (!feof(fp));
      xin[index] = 0;
      sscanf (xin, "%s", map_vote[k].name);
      map_vote[k].name[32] = 0;
      k++;
      }
   map_vote_count = k;
   fclose(fp);
   return 1;   
}

void CWorld :: Spawn( void )
{
	g_fGameOver = FALSE;
	Precache( );
	g_flWeaponCheat = CVAR_GET_FLOAT( "sv_cheats" );  // Is the impulse 101 command allowed?
  
}

void CWorld :: Precache( void )
{
   int ix, i;
   for (ix = 0; ix < TEAM_MAX; ix++)
      memset(&team[ix], 0, sizeof(team[ix]));

   //map_stat.logfile = fopen("gangwars/server_log.txt", "a");

   LoadWeaponMatrix("gangwars/gw.wd");

   memset(&map_stat, 0, sizeof(map_stat));
   map_stat.g_fRoundStartTime = 0;
   map_stat.g_fRoundEndTime = 0;
   map_stat.g_fRoundEndRespawnTime = 0;
   map_stat.g_fRoundCriticalTime = 0;
   map_stat.g_fRoundArmoryCloseTime = 0;

   g_pLastSpawn = NULL;
	
#if 1
	CVAR_SET_STRING("sv_gravity", "800"); // 67ft/sec
	CVAR_SET_STRING("sv_stepsize", "18");
#else
	CVAR_SET_STRING("sv_gravity", "384"); // 32ft/sec
	CVAR_SET_STRING("sv_stepsize", "24");
#endif

	CVAR_SET_STRING("room_type", "0");// clear DSP

	// Set up game rules
	if (g_pGameRules)
	   {
		delete g_pGameRules;
	   }

	g_pGameRules = InstallGameRules( );
   map_bags = LoadBagPositions();
   LoadAdminInfo();
   LoadVoteMap();
   LoadBestScore();
   team[TEAM_EAST].win_points = EAST_WINS_POINTS;
   team[TEAM_EAST].winner_points = EAST_WINNER_POINTS;
   team[TEAM_EAST].lose_points = EAST_LOSES_POINTS;
   team[TEAM_EAST].objective_points = EAST_OBJECTIVE_POINTS;
   team[TEAM_EAST].draw_points = EAST_DRAW_POINTS;

   team[TEAM_WEST].win_points = WEST_WINS_POINTS;
   team[TEAM_WEST].winner_points = WEST_WINNER_POINTS;
   team[TEAM_WEST].lose_points = WEST_LOSES_POINTS;
   team[TEAM_WEST].objective_points = WEST_OBJECTIVE_POINTS;
   team[TEAM_WEST].draw_points = WEST_DRAW_POINTS;

   team[TEAM_EAST].win_money = aw.winmoney;
   team[TEAM_EAST].winner_money = aw.winmoney;
   team[TEAM_EAST].lose_money = aw.losemoney;
   team[TEAM_EAST].objective_money = EAST_OBJECTIVE_MONEY;
   team[TEAM_EAST].draw_money = aw.drawmoney;


   team[TEAM_WEST].win_money = aw.winmoney;
   team[TEAM_WEST].winner_money = aw.winmoney;
   team[TEAM_WEST].lose_money = aw.losemoney;
   team[TEAM_WEST].objective_money = EAST_OBJECTIVE_MONEY;
   team[TEAM_WEST].draw_money = aw.drawmoney;

	//!!!UNDONE why is there so much Spawn code in the Precache function? I'll just keep it here 

	///!!!LATER - do we want a sound ent in deathmatch? (sjb)
	//pSoundEnt = CBaseEntity::Create( "soundent", g_vecZero, g_vecZero, edict() );
	pSoundEnt = GetClassPtr( ( CSoundEnt *)NULL );
	pSoundEnt->Spawn();

	if ( !pSoundEnt )
	{
		ALERT ( at_console, "**COULD NOT CREATE SOUNDENT**\n" );
	}

	InitBodyQue();
	
// init sentence group playback stuff from sentences.txt.
// ok to call this multiple times, calls after first are ignored.

	SENTENCEG_Init();

// init texture type array from materials.txt

	TEXTURETYPE_Init();


// the area based ambient sounds MUST be the first precache_sounds

// player precaches     
	W_Precache ();									// get weapon precaches

	ClientPrecache();

// sounds used from C physics code
	PRECACHE_SOUND("common/null.wav");				// clears sound channels

	PRECACHE_SOUND( "items/suitchargeok1.wav" );//!!! temporary sound for respawning weapons.
	PRECACHE_SOUND( "items/gunpickup2.wav" );// player picks up a gun.

	PRECACHE_SOUND( "common/bodydrop3.wav" );// dead bodies hitting the ground (animation events)
	PRECACHE_SOUND( "common/bodydrop4.wav" );
	
	g_Language = (int)CVAR_GET_FLOAT( "sv_language" );
   PRECACHE_MODEL( "models/hgibs.mdl" );
	PRECACHE_MODEL( "models/agibs.mdl" );

	PRECACHE_SOUND ("weapons/ric1.wav");
	PRECACHE_SOUND ("weapons/ric2.wav");
	PRECACHE_SOUND ("weapons/ric3.wav");
	PRECACHE_SOUND ("weapons/ric4.wav");
	PRECACHE_SOUND ("weapons/ric5.wav");

//
// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.
//

	// 0 normal
	LIGHT_STYLE(0, "m");
	
	// 1 FLICKER (first variety)
	LIGHT_STYLE(1, "mmnmmommommnonmmonqnmmo");
	
	// 2 SLOW STRONG PULSE
	LIGHT_STYLE(2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");
	
	// 3 CANDLE (first variety)
	LIGHT_STYLE(3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");
	
	// 4 FAST STROBE
	LIGHT_STYLE(4, "mamamamamama");
	
	// 5 GENTLE PULSE 1
	LIGHT_STYLE(5,"jklmnopqrstuvwxyzyxwvutsrqponmlkj");
	
	// 6 FLICKER (second variety)
	LIGHT_STYLE(6, "nmonqnmomnmomomno");
	
	// 7 CANDLE (second variety)
	LIGHT_STYLE(7, "mmmaaaabcdefgmmmmaaaammmaamm");
	
	// 8 CANDLE (third variety)
	LIGHT_STYLE(8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");
	
	// 9 SLOW STROBE (fourth variety)
	LIGHT_STYLE(9, "aaaaaaaazzzzzzzz");
	
	// 10 FLUORESCENT FLICKER
	LIGHT_STYLE(10, "mmamammmmammamamaaamammma");

	// 11 SLOW PULSE NOT FADE TO BLACK
	LIGHT_STYLE(11, "abcdefghijklmnopqrrqponmlkjihgfedcba");
	
	// 12 UNDERWATER LIGHT MUTATION
	// this light only distorts the lightmap - no contribution
	// is made to the brightness of affected surfaces
	LIGHT_STYLE(12, "mmnnmmnnnmmnn");
	
	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	LIGHT_STYLE(63, "a");

	for (i = 0; i < ARRAYSIZE(gDecals); i++ )
		gDecals[i].index = DECAL_INDEX( gDecals[i].name );

	if ( pev->speed > 0 )
		CVAR_SET_FLOAT( "sv_zmax", pev->speed );
	else
		CVAR_SET_FLOAT( "sv_zmax", 4096 );

	if ( pev->netname )
	{
		ALERT( at_aiconsole, "Chapter title: %s\n", STRING(pev->netname) );
		CBaseEntity *pEntity = CBaseEntity::Create( "env_message", g_vecZero, g_vecZero, NULL );
		if ( pEntity )
		{
		// swmpdg-added &CWorld::
			pEntity->SetThink( &CWorld::SUB_CallUseToggle );
			pEntity->pev->message = pev->netname;
			pev->netname = 0;
			pEntity->pev->nextthink = gpGlobals->time + 0.3;
			pEntity->pev->spawnflags = SF_MESSAGE_ONCE;
		}
	}

	if ( pev->spawnflags & SF_WORLD_DARK )
		CVAR_SET_FLOAT( "v_dark", 1.0 );
	else
		CVAR_SET_FLOAT( "v_dark", 0.0 );

	if ( pev->spawnflags & SF_WORLD_TITLE )
		gDisplayTitle = TRUE;		// display the game title if this key is set
	else
		gDisplayTitle = FALSE;

	if ( pev->spawnflags & SF_WORLD_FORCETEAM )
	{
		CVAR_SET_FLOAT( "mp_defaultteam", 1 );
	}
	else
	{
		CVAR_SET_FLOAT( "mp_defaultteam", 0 );
	}
}


//
// Just to ignore the "wad" field.
//
void CWorld :: KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq(pkvd->szKeyName, "skyname") )
	{
		// Sent over net now.
		CVAR_SET_STRING( "sv_skyname", pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "sounds") )
	{
		gpGlobals->cdAudioTrack = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "WaveHeight") )
	{
		// Sent over net now.
		pev->scale = atof(pkvd->szValue) * (1.0/8.0);
		pkvd->fHandled = TRUE;
		CVAR_SET_FLOAT( "sv_wateramp", pev->scale );
	}
	else if ( FStrEq(pkvd->szKeyName, "MaxRange") )
	{
		pev->speed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "chaptertitle") )
	{
		pev->netname = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "startdark") )
	{
		// UNDONE: This is a gross hack!!! The CVAR is NOT sent over the client/sever link
		// but it will work for single player
		int flag = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
		if ( flag )
			pev->spawnflags |= SF_WORLD_DARK;
	}
	else if ( FStrEq(pkvd->szKeyName, "newunit") )
	{
		// Single player only.  Clear save directory if set
		if ( atoi(pkvd->szValue) )
			CVAR_SET_FLOAT( "sv_newunit", 1 );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "gametitle") )
	{
		if ( atoi(pkvd->szValue) )
			pev->spawnflags |= SF_WORLD_TITLE;

		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "mapteams") )
	{
		pev->team = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "defaultteam") )
	{
		if ( atoi(pkvd->szValue) )
		{
			pev->spawnflags |= SF_WORLD_FORCETEAM;
		}
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}
