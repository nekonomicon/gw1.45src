/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
/*

===== monsters.cpp ========================================================

  Monster-related utility code

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "animation.h"
#include "saverestore.h"
#include "weapons.h"
#include "scripted.h"
//#include "squadmonster.h"
#include "decals.h"
#include "soundent.h"
#include "gamerules.h"

#define MONSTER_CUT_CORNER_DIST		8 // 8 means the monster's bounding box is contained without the box of the node in WC


Vector VecBModelOrigin( entvars_t* pevBModel );

extern DLL_GLOBAL	BOOL	g_fDrawLines;
extern DLL_GLOBAL	short	g_sModelIndexLaser;// holds the index for the laser beam
void CBaseMonster :: HandleAnimEvent( MonsterEvent_t *pEvent ){}
Vector CBaseMonster :: GetGunPosition( ){return NULL;}
void CBaseMonster :: KeyValue( KeyValueData *pkvd ){}
int CBaseMonster::Save( CSave &save ){return 0;}
int CBaseMonster::Restore( CRestore &restore ){return 0;}
void CBaseMonster :: BarnacleVictimBitten ( entvars_t *pevBarnacle ){}
void CBaseMonster :: BarnacleVictimReleased ( void ){}
void CBaseMonster :: MonsterUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ){}

void CBaseMonster :: MonsterThink ( void )
{
	pev->nextthink = gpGlobals->time + 0.1;// keep monster thinking.
	float flInterval = StudioFrameAdvance( ); // animate
	if ( m_Activity == ACT_IDLE && m_fSequenceFinished )
	   {
		int iSequence;

		if ( m_fSequenceLoops )
		   {
			// animation does loop, which means we're playing subtle idle. Might need to 
			// fidget.
			iSequence = LookupActivity ( m_Activity );
		   }
		else
		   {
			// animation that just ended doesn't loop! That means we just finished a fidget
			// and should return to our heaviest weighted idle (the subtle one)
			iSequence = LookupActivityHeaviest ( m_Activity );
		   }
		if ( iSequence != ACTIVITY_NOT_AVAILABLE )
		   {
			pev->sequence = iSequence;	// Set to new anim (if it's there)
			ResetSequenceInfo( );
		   }
	   }

	DispatchAnimEvents( flInterval );
}


//=========================================================
// IRelationship - returns an integer that describes the 
// relationship between two types of monster.
//=========================================================
void CBaseMonster::CorpseFallThink( void )
{
	if ( pev->flags & FL_ONGROUND )
	   {
		SetThink ( NULL );

		SetSequenceBox( );
		UTIL_SetOrigin( pev, pev->origin );// link into world.
	   }
	else
		pev->nextthink = gpGlobals->time + 0.1;
}


//=========================================================
// BBoxIsFlat - check to see if the monster's bounding box
// is lying flat on a surface (traces from all four corners
// are same length.)
//=========================================================
BOOL CBaseMonster :: BBoxFlat ( void )
{
	TraceResult	tr;
	Vector		vecPoint;
	float		flXSize, flYSize;
	float		flLength;
	float		flLength2;

	flXSize = pev->size.x / 2;
	flYSize = pev->size.y / 2;

	vecPoint.x = pev->origin.x + flXSize;
	vecPoint.y = pev->origin.y + flYSize;
	vecPoint.z = pev->origin.z;

	UTIL_TraceLine ( vecPoint, vecPoint - Vector ( 0, 0, 100 ), ignore_monsters, ENT(pev), &tr );
	flLength = (vecPoint - tr.vecEndPos).Length();

	vecPoint.x = pev->origin.x - flXSize;
	vecPoint.y = pev->origin.y - flYSize;

	UTIL_TraceLine ( vecPoint, vecPoint - Vector ( 0, 0, 100 ), ignore_monsters, ENT(pev), &tr );
	flLength2 = (vecPoint - tr.vecEndPos).Length();
	if ( flLength2 > flLength )
	{
		return FALSE;
	}
	flLength = flLength2;

	vecPoint.x = pev->origin.x - flXSize;
	vecPoint.y = pev->origin.y + flYSize;
	UTIL_TraceLine ( vecPoint, vecPoint - Vector ( 0, 0, 100 ), ignore_monsters, ENT(pev), &tr );
	flLength2 = (vecPoint - tr.vecEndPos).Length();
	if ( flLength2 > flLength )
	{
		return FALSE;
	}
	flLength = flLength2;

	vecPoint.x = pev->origin.x + flXSize;
	vecPoint.y = pev->origin.y - flYSize;
	UTIL_TraceLine ( vecPoint, vecPoint - Vector ( 0, 0, 100 ), ignore_monsters, ENT(pev), &tr );
	flLength2 = (vecPoint - tr.vecEndPos).Length();
	if ( flLength2 > flLength )
	{
		return FALSE;
	}
	flLength = flLength2;

	return TRUE;
}
