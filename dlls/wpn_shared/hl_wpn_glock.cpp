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
#include "player.h"

void CGlock::Spawn( )
{
}


void CGlock::Precache( void )
{
}

int CGlock::GetItemInfo(ItemInfo *p)
{
   return 0;
}

BOOL CGlock::Deploy( )
{
	return 0;
}

void CGlock::SecondaryAttack( void )
{
}

void CGlock::PrimaryAttack( void )
{
}

void CGlock::GlockFire( float flSpread , float flCycleTime, BOOL fUseAutoAim )
{
}


void CGlock::Reload( void )
{
}



void CGlock::WeaponIdle( void )
{
}








class CGlockAmmo : public CBasePlayerAmmo
{
};















