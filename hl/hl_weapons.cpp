/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"

#include "usercmd.h"
#include "entity_state.h"
#include "demo_api.h"
#include "pm_defs.h"
#include "event_api.h"
#include "r_efx.h"


#include "../hud_iface.h"
#include "../com_weapons.h"
#include "../demo.h"

#define I_AM_CLIENT
#define TMP_MODULE
#define MAC10_MODULE
#define UMP45_MODULE
#define MP5_MODULE
#define FNP90_MODULE
#define AK47_MODULE
#define M4A1_MODULE
#define SG552_MODULE
#define AUG_MODULE
#define M3_MODULE
#define G3SG1_MODULE
#define SG550_MODULE
#define DEAGLE_MODULE
#define USP_MODULE
#define GLOCK_MODULE
#define P228_MODULE

#include "sdk_w_classes.h"

extern globalvars_t *gpGlobals;

// Pool of client side entities/entvars_t
static entvars_t	ev[ 32 ];
static int			num_ents = 0;

// The entity we'll use to represent the local client
static CBasePlayer	player;

// Local version of game .dll global variables ( time, etc. )
static globalvars_t	Globals; 

static CBasePlayerWeapon *g_pWpns[ 32 ];

// HLDM Weapon placeholder entities.
CGlock g_Glock;
TMP         g_TMP;
MAC10       g_MAC10;
UMP45       g_UMP45;
MP5         g_MP5;
P90         g_FNP90;
AK47        g_AK47;
M4A1        g_M4A1;
SG552       g_SG552;
AUG         g_AUG;
G3SG1       g_G3SG1;
SG550       g_SG550;
DEAGLE      g_DEAGLE;
USP         g_USP;
GLOCK       g_GLOCK;
P228        g_P228;
/*
======================
AlertMessage

Print debug messages to console
======================
*/
void AlertMessage( ALERT_TYPE atype, char *szFmt, ... )
{
	va_list		argptr;
	static char	string[1024];
	
	va_start (argptr, szFmt);
	vsprintf (string, szFmt,argptr);
	va_end (argptr);

	gEngfuncs.Con_Printf( "cl:  " );
	gEngfuncs.Con_Printf( string );
}

/*
=====================
HUD_PrepEntity

Links the raw entity to an entvars_s holder.  If a player is passed in as the owner, then
we set up the m_pPlayer field.
=====================
*/
void HUD_PrepEntity( CBaseEntity *pEntity, CBasePlayer *pWeaponOwner )
{
	memset( &ev[ num_ents ], 0, sizeof( entvars_t ) );
	pEntity->pev = &ev[ num_ents++ ];

	pEntity->Precache();
	pEntity->Spawn();

	if ( pWeaponOwner )
	{
		ItemInfo info;
		
		((CBasePlayerWeapon *)pEntity)->m_pPlayer = pWeaponOwner;
		
		((CBasePlayerWeapon *)pEntity)->GetItemInfo( &info );

		g_pWpns[ info.iId ] = (CBasePlayerWeapon *)pEntity;
	}
}

/*
// FIXME:  In order to predict animations client side, you'll need to work with the following code.
//  It's not quite working, but it should be of use if you want
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pmodel - 
//			*label - 
// Output : int
//-----------------------------------------------------------------------------
int LookupSequence( void *pmodel, const char *label )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return 0;

	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);

	for (int i = 0; i < pstudiohdr->numseq; i++)
	{
		if (stricmp( pseqdesc[i].label, label ) == 0)
			return i;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *label - 
// Output : int CBaseAnimating :: LookupSequence
//-----------------------------------------------------------------------------
int CBaseAnimating :: LookupSequence ( const char *label )
{
	cl_entity_t *current;

	current = gEngfuncs.GetLocalPlayer();
	if ( !current || !current->model )
		return 0;

	return ::LookupSequence( (studiohdr_t *)IEngineStudio.Mod_Extradata( current->model ), label );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pmodel - 
//			*pev - 
//			*pflFrameRate - 
//			*pflGroundSpeed - 
//-----------------------------------------------------------------------------
void GetSequenceInfo( void *pmodel, entvars_t *pev, float *pflFrameRate, float *pflGroundSpeed )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return;

	mstudioseqdesc_t	*pseqdesc;

	if (pev->sequence >= pstudiohdr->numseq)
	{
		*pflFrameRate = 0.0;
		*pflGroundSpeed = 0.0;
		return;
	}

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	if (pseqdesc->numframes > 1)
	{
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = sqrt( pseqdesc->linearmovement[0]*pseqdesc->linearmovement[0]+ pseqdesc->linearmovement[1]*pseqdesc->linearmovement[1]+ pseqdesc->linearmovement[2]*pseqdesc->linearmovement[2] );
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pmodel - 
//			*pev - 
// Output : int
//-----------------------------------------------------------------------------
int GetSequenceFlags( void *pmodel, entvars_t *pev )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if ( !pstudiohdr || pev->sequence >= pstudiohdr->numseq )
		return 0;

	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	return pseqdesc->flags;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : 
//-----------------------------------------------------------------------------
void CBaseAnimating :: ResetSequenceInfo ( )
{
	cl_entity_t *current;

	current = gEngfuncs.GetLocalPlayer();
	if ( !current || !current->model )
		return;

	void *pmodel = (studiohdr_t *)IEngineStudio.Mod_Extradata( current->model );

	GetSequenceInfo( pmodel, pev, &m_flFrameRate, &m_flGroundSpeed );
	m_fSequenceLoops = ((GetSequenceFlags() & STUDIO_LOOPING) != 0);
	pev->animtime = gpGlobals->time;
	pev->framerate = 1.0;
	m_fSequenceFinished = FALSE;
	m_flLastEventCheck = gpGlobals->time;
}
*/
/*
=====================
CBaseEntity :: Killed

If weapons code "kills" an entity, just set its effects to EF_NODRAW
=====================
*/
void CBaseEntity :: Killed( entvars_t *pevAttacker, int iGib )
{
	pev->effects |= EF_NODRAW;
}

/*
=====================
CBasePlayerWeapon :: DefaultReload
=====================
*/
BOOL CBasePlayerWeapon :: DefaultReload( int iClipSize, int iAnim, float fDelay )
{
#if 0 // FIXME, need to know primary ammo to get this right
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return FALSE;

	int j = min(iClipSize - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);	

	if (j == 0)
		return FALSE;
#endif

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + fDelay;

	//!!UNDONE -- reload sound goes here !!!
	SendWeaponAnim( iAnim );

	m_fInReload = TRUE;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
	return TRUE;
}

/*
=====================
CBasePlayerWeapon :: CanDeploy
=====================
*/
BOOL CBasePlayerWeapon :: CanDeploy( void ) 
{
	BOOL bHasAmmo = 0;

	if ( !pszAmmo1() )
	{
		// this weapon doesn't use ammo, can always deploy.
		return TRUE;
	}

	if ( pszAmmo1() )
	{
		bHasAmmo |= (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0);
	}
	if ( pszAmmo2() )
	{
		bHasAmmo |= (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] != 0);
	}
	if (m_iClip > 0)
	{
		bHasAmmo |= 1;
	}
	if (!bHasAmmo)
	{
		return FALSE;
	}

	return TRUE;
}

/*
=====================
CBasePlayerWeapon :: DefaultDeploy

=====================
*/
BOOL CBasePlayerWeapon :: DefaultDeploy( char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, int skiplocal )
{
	if ( !CanDeploy() )
		return FALSE;

	gEngfuncs.CL_LoadModel( szViewModel, &m_pPlayer->pev->viewmodel );
	
	SendWeaponAnim( iAnim );

	m_pPlayer->m_flNextAttack = 0.5;
	m_flTimeWeaponIdle = 1.0;
	return TRUE;
}

/*
=====================
CBasePlayerWeapon :: PlayEmptySound

=====================
*/
BOOL CBasePlayerWeapon :: PlayEmptySound( void )
{
	if (m_iPlayEmptySound)
	{
		HUD_PlaySound( "weapons/357_cock1.wav", 0.8 );
		m_iPlayEmptySound = 0;
		return 0;
	}
	return 0;
}

/*
=====================
CBasePlayerWeapon :: ResetEmptySound

=====================
*/
void CBasePlayerWeapon :: ResetEmptySound( void )
{
	m_iPlayEmptySound = 1;
}

/*
=====================
CBasePlayerWeapon::Holster

Put away weapon
=====================
*/
void CBasePlayerWeapon::Holster( int skiplocal /* = 0 */ )
{ 
	m_fInReload = FALSE; // cancel any reload in progress.
	m_pPlayer->pev->viewmodel = 0; 
}

/*
=====================
CBasePlayerWeapon::SendWeaponAnim

Animate weapon model
=====================
*/
void CBasePlayerWeapon::SendWeaponAnim( int iAnim, int skiplocal )
{
	m_pPlayer->pev->weaponanim = iAnim;
	
	int body = 0;

	HUD_SendWeaponAnim( iAnim, body, 0 );
}

/*
=====================
CBasePlayerWeapon::ItemPostFrame

Handles weapon firing, reloading, etc.
=====================
*/
void CBasePlayerWeapon::ItemPostFrame( void )
{
	// catch all
   if (!(m_pPlayer->pev->button & IN_ATTACK))
      m_bTriggerReleased = true;

	if ((m_fInReload) && (m_pPlayer->m_flNextAttack <= 0.0))
	   {
#if 0 // FIXME, need ammo on client to make this work right
		// complete the reload. 
		int j = min( iMaxClip() - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);	

		// Add them to the clip
		m_iClip += j;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= j;
#else	
		m_iClip += 10;
#endif
		m_fInReload = FALSE;
	   }
   /*
	if ((m_pPlayer->pev->button & IN_ATTACK2) && (m_flNextSecondaryAttack <= 0.0))
	   {
		if ( pszAmmo2() && !m_pPlayer->m_rgAmmo[SecondaryAmmoIndex()] )
		   {
			m_fFireOnEmpty = TRUE;
		   }

		SecondaryAttack();
		m_pPlayer->pev->button &= ~IN_ATTACK2;
	   }

	else*/ if ((m_pPlayer->pev->button & IN_ATTACK) && (m_flNextPrimaryAttack <= 0.0))
	   {
		if ( (m_iClip == 0 && pszAmmo1()) || (iMaxClip() == -1 && !m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] ) )
		   {
			m_fFireOnEmpty = TRUE;
		   }
		PrimaryAttack();
      return;
	   }
/*
   if (m_flNextWeaponIdle <= 0.0)
      {
      WeaponIdle();
      }
*/
   /*
	else if ( m_pPlayer->pev->button & IN_RELOAD && iMaxClip() != WEAPON_NOCLIP && !m_fInReload ) 
	   {
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
	   }
   */
   /*
	else if ( !(m_pPlayer->pev->button & (IN_ATTACK|IN_ATTACK2) ) )
	   {
		// no fire buttons down

		m_fFireOnEmpty = FALSE;

		// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
		if ( m_iClip == 0 && !(iFlags() & ITEM_FLAG_NOAUTORELOAD) && m_flNextPrimaryAttack < 0.0 )
		   {
			Reload();
			return;
		   }

		WeaponIdle( );
		return;
	   }
	// catch all
	if ( ShouldWeaponIdle() )
	   {
		WeaponIdle();
	   }
	*/
}

/*
=====================
CBasePlayer::SelectItem

  Switch weapons
=====================
*/
void CBasePlayer::SelectItem(const char *pstr)
{
	if (!pstr)
		return;

	CBasePlayerItem *pItem = NULL;

	if (!pItem)
		return;

	
	if (pItem == m_pActiveItem)
		return;

   /*
	if (m_pActiveItem)
		m_pActiveItem->Holster( );
	*/

	m_pLastItem = m_pActiveItem;
	m_pActiveItem = pItem;

   /*
	if (m_pActiveItem)
	{
		m_pActiveItem->Deploy( );
	}
   */
}

/*
=====================
CBasePlayer::SelectLastItem

=====================
*/
void CBasePlayer::SelectLastItem(void)
{
	if (!m_pLastItem)
	{
		return;
	}

	if ( m_pActiveItem && !m_pActiveItem->CanHolster() )
	{
		return;
	}

	if (m_pActiveItem)
		m_pActiveItem->Holster( );
	
	CBasePlayerItem *pTemp = m_pActiveItem;
	m_pActiveItem = m_pLastItem;
	m_pLastItem = pTemp;
	m_pActiveItem->Deploy( );
}

/*
=====================
CBasePlayer::Killed

=====================
*/
void CBasePlayer::Killed( entvars_t *pevAttacker, int iGib )
{
	// Holster weapon immediately, to allow it to cleanup
	if (m_pActiveItem)
		m_pActiveItem->Holster( );
}

/*
=====================
CBasePlayer::Spawn

=====================
*/
void CBasePlayer::Spawn( void )
{
   /*
	if (m_pActiveItem)
		m_pActiveItem->Deploy( );
   */
}

/*
=====================
UTIL_TraceLine

Don't actually trace, but act like the trace didn't hit anything.
=====================
*/
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr )
{
	memset( ptr, 0, sizeof( *ptr ) );
	ptr->flFraction = 1.0;
}

/*
=====================
UTIL_ParticleBox

For debugging, draw a box around a player made out of particles
=====================
*/
void UTIL_ParticleBox( CBasePlayer *player, float *mins, float *maxs, float life, unsigned char r, unsigned char g, unsigned char b )
{
	int i;
	vec3_t mmin, mmax;

	for ( i = 0; i < 3; i++ )
	{
		mmin[ i ] = player->pev->origin[ i ] + mins[ i ];
		mmax[ i ] = player->pev->origin[ i ] + maxs[ i ];
	}

	gEngfuncs.pEfxAPI->R_ParticleBox( (float *)&mmin, (float *)&mmax, 5.0, 0, 255, 0 );
}

/*
=====================
UTIL_ParticleBoxes

For debugging, draw boxes for other collidable players
=====================
*/
void UTIL_ParticleBoxes( void )
{
	int idx;
	physent_t *pe;
	cl_entity_t *player;
	vec3_t mins, maxs;
	
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	player = gEngfuncs.GetLocalPlayer();
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( player->index - 1 );	

	for ( idx = 1; idx < 100; idx++ )
	{
		pe = gEngfuncs.pEventAPI->EV_GetPhysent( idx );
		if ( !pe )
			break;

		if ( pe->info >= 1 && pe->info <= gEngfuncs.GetMaxClients() )
		{
			mins = pe->origin + pe->mins;
			maxs = pe->origin + pe->maxs;

			gEngfuncs.pEfxAPI->R_ParticleBox( (float *)&mins, (float *)&maxs, 0, 0, 255, 2.0 );
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

/*
=====================
UTIL_ParticleLine

For debugging, draw a line made out of particles
=====================
*/
void UTIL_ParticleLine( CBasePlayer *player, float *start, float *end, float life, unsigned char r, unsigned char g, unsigned char b )
{
	gEngfuncs.pEfxAPI->R_ParticleLine( start, end, r, g, b, life );
}

/*
=====================
CBasePlayerWeapon::PrintState

For debugging, print out state variables to log file
=====================
*/
void CBasePlayerWeapon::PrintState( void )
{
	COM_Log( "c:\\hl.log", "%.4f ", gpGlobals->time );
	COM_Log( "c:\\hl.log", "%.4f ", m_pPlayer->m_flNextAttack );
	COM_Log( "c:\\hl.log", "%.4f ", m_flNextPrimaryAttack );
	COM_Log( "c:\\hl.log", "%.4f ", m_flTimeWeaponIdle - gpGlobals->time);
	COM_Log( "c:\\hl.log", "%i ", m_iClip );
}

/*
=====================
HUD_InitClientWeapons

Set up weapons, player and functions needed to run weapons code client-side.
=====================
*/
void HUD_InitClientWeapons( void )
{
	// Set up pointer ( dummy object )
	gpGlobals = &Globals;

	// Fill in current time ( probably not needed )
	gpGlobals->time = gEngfuncs.GetClientTime();

	// Fake functions
	g_engfuncs.pfnPrecacheModel = stub_PrecacheModel;
	g_engfuncs.pfnPrecacheSound = stub_PrecacheSound;
	g_engfuncs.pfnPrecacheEvent = stub_PrecacheEvent;
	g_engfuncs.pfnNameForFunction	= stub_NameForFunction;
	g_engfuncs.pfnSetModel = stub_SetModel;
	g_engfuncs.pfnSetClientMaxspeed = HUD_SetMaxSpeed;

	// Handled locally
	g_engfuncs.pfnPlaybackEvent = HUD_PlaybackEvent;
	g_engfuncs.pfnAlertMessage = AlertMessage;

	// Pass through to engine
	g_engfuncs.pfnPrecacheEvent = gEngfuncs.pfnPrecacheEvent;
	g_engfuncs.pfnRandomFloat = gEngfuncs.pfnRandomFloat;
	g_engfuncs.pfnRandomLong = gEngfuncs.pfnRandomLong;

	// Allocate a slot for the local player
	HUD_PrepEntity( &player	, NULL );

	// Allocate slot(s) for each weapon that we are going to be predicting
	//HUD_PrepEntity( &g_Glock , &player );
	HUD_PrepEntity( &g_TMP,    &player );
	HUD_PrepEntity( &g_MAC10,  &player );
	HUD_PrepEntity( &g_UMP45,  &player );
	HUD_PrepEntity( &g_MP5,    &player );
	HUD_PrepEntity( &g_FNP90,  &player );
	HUD_PrepEntity( &g_AK47,   &player );
	HUD_PrepEntity( &g_M4A1,   &player );
	HUD_PrepEntity( &g_SG552,  &player );
	HUD_PrepEntity( &g_AUG,    &player );
	HUD_PrepEntity( &g_G3SG1,  &player );
	HUD_PrepEntity( &g_SG550,  &player );
	HUD_PrepEntity( &g_DEAGLE, &player );
	HUD_PrepEntity( &g_USP,    &player );
	HUD_PrepEntity( &g_GLOCK,  &player );
	HUD_PrepEntity( &g_P228,   &player );
}

/*
=====================
HUD_WeaponsPostThink

Run Weapon firing code on client
=====================
*/
void HUD_WeaponsPostThink( local_state_s *from, local_state_s *to, usercmd_t *cmd, double time, unsigned int random_seed )
{
	int i;
	int buttonsChanged;
	CBasePlayerWeapon *pWeapon = NULL;
	CBasePlayerWeapon *pCurrent;
	weapon_data_t nulldata, *pfrom, *pto;
	static int lasthealth;
	static int initialized = 0;

	memset( &nulldata, 0, sizeof( nulldata ) );

	if (!initialized)
      {
	   HUD_InitClientWeapons();
      initialized = 1;
      }
	
	// Get current clock
	gpGlobals->time = time;

	// Fill in data based on selected weapon
	// FIXME, make this a method in each weapon?  where you pass in an entity_state_t *?
	switch ( from->client.m_iId )
	   {
      case WEAPON_TMP:
		   pWeapon = &g_TMP;
		   break;
      case WEAPON_MAC10:
		   pWeapon = &g_MAC10;
		   break;
      case WEAPON_UMP45:
		   pWeapon = &g_UMP45;
		   break;
      case WEAPON_MP5NAVY:
		   pWeapon = &g_MP5;
		   break;
      case WEAPON_P90:
		   pWeapon = &g_FNP90;
		   break;
      case WEAPON_AK47:
		   pWeapon = &g_AK47;
		   break;
      case WEAPON_M4A1:
		   pWeapon = &g_M4A1;
		   break;
      case WEAPON_SG552:
		   pWeapon = &g_SG552;
		   break;
      case WEAPON_AUG:
		   pWeapon = &g_AUG;
		   break;
      case WEAPON_G3SG1:
		   pWeapon = &g_G3SG1;
		   break;
      case WEAPON_SG550:
		   pWeapon = &g_SG550;
		   break;
      case WEAPON_DEAGLE:
		   pWeapon = &g_DEAGLE;
		   break;
      case WEAPON_USP:
		   pWeapon = &g_USP;
		   break;
      case WEAPON_GLOCK18:
		   pWeapon = &g_GLOCK;
		   break;
      case WEAPON_P228:
		   pWeapon = &g_P228;
		   break;
	   }
	// We are not predicting the current weapon, just bow out here.
	if ( !pWeapon )
		return;

	for ( i = 0; i < 32; i++ )
	   {
		   pCurrent = g_pWpns[ i ];
		   if ( !pCurrent )
		      {
			   continue;
   		   }

		   pfrom = &from->weapondata[ i ];
		   
		   pCurrent->m_fInReload = pfrom->m_fInReload;
		   pCurrent->m_iClip	= pfrom->m_iClip;
		   pCurrent->m_flNextPrimaryAttack = pfrom->m_flNextPrimaryAttack;
		   pCurrent->m_flNextSecondaryAttack = pfrom->m_flNextSecondaryAttack;
		   pCurrent->m_flTimeWeaponIdle = pfrom->m_flTimeWeaponIdle;
	   }

	// For random weapon events, use this seed to seed random # generator
	player.random_seed = random_seed;

	// Get old buttons from previous state.
	player.m_afButtonLast = from->playerstate.oldbuttons;

	// Which buttsons chave changed
	buttonsChanged = (player.m_afButtonLast ^ cmd->buttons);	// These buttons have changed this frame
	
	// Debounced button codes for pressed/released
	// The changed ones still down are "pressed"
	player.m_afButtonPressed = buttonsChanged & cmd->buttons;	
	// The ones not down are "released"
	player.m_afButtonReleased = buttonsChanged & (~cmd->buttons);

	// Set player variables that weapons code might check/alter
	player.pev->button = cmd->buttons;

	player.pev->velocity = from->client.velocity;
	player.pev->flags = from->client.flags;

	player.pev->deadflag = from->client.deadflag;
	player.pev->waterlevel = from->client.waterlevel;
	player.pev->maxspeed = from->client.maxspeed;
	player.pev->fov = from->client.fov;
	player.pev->weaponanim = from->client.weaponanim;
	player.pev->viewmodel = from->client.viewmodel;
	player.m_flNextAttack = from->client.m_flNextAttack;

	// Point to current weapon object
	if ( from->client.m_iId )
	   {
		player.m_pActiveItem = g_pWpns[ from->client.m_iId ];
	   }

	// Store pointer to our destination entity_state_t so we can get our origin, etc. from it
	//  for setting up events on the client
	g_finalstate = to;

	// Don't go firing anything if we have died.
	// Or if we don't have a weapon model deployed
	if ( ( player.pev->deadflag != ( DEAD_DISCARDBODY + 1 ) ) && !CL_IsDead() && player.pev->viewmodel )
	   {
		if ( player.m_flNextAttack <= 0 )
		   {
   		pWeapon->ItemPostFrame();
		   }
	   }

	// If we are running events/etc. go ahead and see if we
	//  managed to die between last frame and this one
	// If so, run the appropriate player killed or spawn function
	if ( g_runfuncs )
	   {
		if ( to->client.health <= 0 && lasthealth > 0 )
		   {
			player.Killed( NULL, 0 );
		   }
		else if ( to->client.health > 0 && lasthealth <= 0 )
		   {
			player.Spawn();
		   }

		lasthealth = to->client.health;
	   }

	// Assume that we are not going to switch weapons
	to->client.m_iId = from->client.m_iId;

	// Now see if we issued a changeweapon command ( and we're not dead )
	if ( cmd->weaponselect && ( player.pev->deadflag != ( DEAD_DISCARDBODY + 1 ) ) )
	   {
		// Switched to a different weapon?
		if ( from->weapondata[ cmd->weaponselect ].m_iId == cmd->weaponselect )
		   {
			CBasePlayerWeapon *pNew = g_pWpns[ cmd->weaponselect ];
			if ( pNew && ( pNew != pWeapon ) )
			   {
            extern int weapon_zoom;
            weapon_zoom = 0;
				// Put away old weapon
            /*
				if (player.m_pActiveItem)
					player.m_pActiveItem->Holster( );
				
				player.m_pLastItem = player.m_pActiveItem;
				player.m_pActiveItem = pNew;

				// Deploy new weapon
				if (player.m_pActiveItem)
				   {
					player.m_pActiveItem->Deploy( );
				   }
            */

				// Update weapon id so we can predict things correctly.
				to->client.m_iId = cmd->weaponselect;
			   }
		   }
	   }

	// Copy in results of predcition code
	to->client.viewmodel	= player.pev->viewmodel;
	to->client.fov	= player.pev->fov;
	to->client.weaponanim = player.pev->weaponanim;
	to->client.m_flNextAttack = player.m_flNextAttack;
	to->client.maxspeed = player.pev->maxspeed;

	// Make sure that weapon animation matches what the game .dll is telling us
	//  over the wire ( fixes some animation glitches )
   if ( g_runfuncs && ( HUD_GetWeaponAnim() != to->client.weaponanim ) )
	   {
		//int body = 2;
		// Force a fixed anim down to viewmodel
		HUD_SendWeaponAnim( to->client.weaponanim, 0, 2 );
	   }

   float thousandth_msec = cmd->msec / 1000.0;
	for ( i = 0; i < 32; i++ )
	   {
		pCurrent = g_pWpns[ i ];

		pto = &to->weapondata[ i ];

		if ( !pCurrent )
		   {
			memset( pto, 0, sizeof( weapon_data_t ) );
			continue;
		   }
	
		pto->m_fInReload = pCurrent->m_fInReload;
		pto->m_iClip = pCurrent->m_iClip; 
		pto->m_flNextPrimaryAttack	= pCurrent->m_flNextPrimaryAttack;
		pto->m_flNextSecondaryAttack = pCurrent->m_flNextSecondaryAttack;
		pto->m_flTimeWeaponIdle	= pCurrent->m_flTimeWeaponIdle;

		// Decrement weapon counters, server does this at same time ( during post think, after doing everything else )
		pto->m_flNextReload -= thousandth_msec;
		pto->m_fNextAimBonus	-= thousandth_msec;
		pto->m_flNextPrimaryAttack	-= thousandth_msec;
		pto->m_flNextSecondaryAttack -= thousandth_msec;
		pto->m_flTimeWeaponIdle	-= thousandth_msec;

		if ( pto->m_flPumpTime != -9999 )
		   {
			pto->m_flPumpTime -= thousandth_msec;
			if ( pto->m_flPumpTime < -0.001 )
				pto->m_flPumpTime = -0.001;
		   }

		if ( pto->m_fNextAimBonus < -1.0 )
		   {
			pto->m_fNextAimBonus = -1.0;
		   }

		if ( pto->m_flNextPrimaryAttack < -1.0 )
		   {
			pto->m_flNextPrimaryAttack = -1.0;
		   }

		if ( pto->m_flNextSecondaryAttack < -0.001 )
		   {
			pto->m_flNextSecondaryAttack = -0.001;
		   }

		if ( pto->m_flTimeWeaponIdle < -0.001 )
		   {
			pto->m_flTimeWeaponIdle = -0.001;
		   }

		if ( pto->m_flNextReload < -0.001 )
		   {
			pto->m_flNextReload = -0.001;
		   }
	   }

	// m_flNextAttack is now part of the weapons, but is part of the player instead
   to->client.m_flNextAttack -= thousandth_msec;
	if ( to->client.m_flNextAttack < -0.001 )
	   {
		to->client.m_flNextAttack = -0.001;
	   }
	// Wipe it so we can't use it after this frame
	g_finalstate = NULL;
}

// For storing predicted sequence and gaitsequence and origin/angles data
static int g_rseq = 0, g_gaitseq = 0;
static vec3_t g_clorg, g_clang;

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *seq - 
//			*gaitseq - 
//-----------------------------------------------------------------------------
void Game_GetSequence( int *seq, int *gaitseq )
{
	*seq = g_rseq;
	*gaitseq = g_gaitseq;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : seq - 
//			gaitseq - 
//-----------------------------------------------------------------------------
void Game_SetSequence( int seq, int gaitseq )
{
	g_rseq = seq;
	g_gaitseq = gaitseq;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : o - 
//			a - 
//-----------------------------------------------------------------------------
void Game_SetOrientation( vec3_t o, vec3_t a )
{
	g_clorg = o;
	g_clang = a;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *o - 
//			*a - 
//-----------------------------------------------------------------------------
void Game_GetOrientation( float *o, float *a )
{
	int i;
	
	for ( i = 0; i < 3; i++ )
	{
		o[ i ] = g_clorg[ i ];
		a[ i ] = g_clang[ i ];
	}
}

/*
=====================
HUD_PostRunCmd

Client calls this during prediction, after it has moved the player and updated any info changed into to->
time is the current client clock based on prediction
cmd is the command that caused the movement, etc
runfuncs is 1 if this is the first time we've predicted this command.  If so, sounds and effects should play, otherwise, they should
be ignored
=====================
*/
void _DLLEXPORT HUD_PostRunCmd( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed )
{
	g_runfuncs = runfuncs;

	// Only run post think stuff for glock for the sample
	//  implementation
#if defined( CLIENT_WEAPONS )
	if ( cl_lw && cl_lw->value)	
	   {
      switch (from->client.m_iId)
         {
         case WEAPON_TMP:
         case WEAPON_MAC10:
         case WEAPON_UMP45:
         case WEAPON_MP5NAVY:
         case WEAPON_P90:
         case WEAPON_AK47:
         case WEAPON_M4A1:
         case WEAPON_SG552:
         case WEAPON_AUG:
         case WEAPON_G3SG1:
         case WEAPON_SG550:
         case WEAPON_DEAGLE:
         case WEAPON_USP:
         case WEAPON_GLOCK18:
         case WEAPON_P228:
            HUD_WeaponsPostThink( from, to, cmd, time, random_seed );
            break;
         }

	   }
	else
#endif
	   {
		   //to->client.fov = g_lastFOV;
	   }

	// Store of final sequence, etc. for client side animation
	if ( g_runfuncs )
	   {
		   Game_SetSequence( to->playerstate.sequence, to->playerstate.gaitsequence );
		   Game_SetOrientation( to->playerstate.origin, cmd->viewangles );
	   }

	// All games can use FOV state
	//g_lastFOV = to->client.fov;
}
