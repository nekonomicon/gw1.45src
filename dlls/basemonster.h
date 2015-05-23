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

#ifndef BASEMONSTER_H
#define BASEMONSTER_H

//
// generic Monster
//
class CBaseMonster : public CBaseToggle
{
   private:
		int					m_afConditions;

   public:
		// these fields have been added in the process of reworking the state machine. (sjb)
		EHANDLE			m_hEnemy;		      // the entity that the monster is fighting.
		EHANDLE			m_hTargetEnt;	      // the entity that the monster is trying to reach

		float				m_flFieldOfView;     // width of monster's field of view ( dot product )
		Activity			m_Activity;          // what the monster is doing (animation)
		Activity			m_IdealActivity;     // monster should switch to this activity	
		int				m_LastHitGroup;      // the last body region that took damage
	   float				m_flNextAttack;		// cannot attack again until this time

	   int				m_bitsDamageType;	   // what types of damage has monster (player) taken
	   BYTE				m_rgbTimeBasedDamage[CDMG_TIMEBASED];
	   int				m_lastDamageAmount;  // how much damage did monster (player) last take
	   int				m_bloodColor;		   // color of blood particless
	   string_t			m_iszTriggerTarget;  // name of target that should be fired. 

	   Vector			m_HackedGunPos;	   // HACK until we can query end of gun

	   virtual int		Save( CSave &save ); 
	   virtual int		Restore( CRestore &restore );
	
	   static	TYPEDESCRIPTION m_SaveData[];
	   void KeyValue( KeyValueData *pkvd );

   // monster use function
	   void EXPORT			MonsterUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	   void EXPORT			CorpseUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

   // overrideable Monster member functions
	   virtual int	 BloodColor( void ) { return m_bloodColor; }
	   virtual CBaseMonster *MyMonsterPointer( void ) { return this; }
	   virtual BOOL	IsAlive( void ) { return (pev->deadflag == DEAD_NO); }

   // Basic Monster AI functions
	   float DamageForce( float damage );

   // stuff written for new state machine
	   virtual void MonsterThink( void );
	   void EXPORT	CallMonsterThink( void ) { this->MonsterThink(); }
	   void EXPORT CorpseFallThink( void );

	   void EXPORT MonsterInitThink ( void );
	   virtual BOOL FInViewCone ( CBaseEntity *pEntity );// see if pEntity is in monster's view cone
	   virtual BOOL FInViewCone ( Vector *pOrigin );// see if given location is in monster's view cone
	   virtual void HandleAnimEvent( MonsterEvent_t *pEvent );

	   // This will stop animation until you call ResetSequenceInfo() at some point in the future
	   inline void StopAnimation( void ) { pev->framerate = 0; }
	   virtual void BarnacleVictimBitten( entvars_t *pevBarnacle );
	   virtual void BarnacleVictimReleased( void );

	   CBaseEntity *CheckTraceHullAttack( float flDist, int iDamage, int iDmgType );
	   BOOL BBoxFlat( void );

	   void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	   // combat functions
	   virtual void Killed( entvars_t *pevAttacker, int iGib );
	   virtual Vector BodyTarget( const Vector &posSrc ) { return Center( ) * 0.75 + EyePosition() * 0.25; };		// position to shoot at
	   virtual	Vector  GetGunPosition( void );
	   virtual int TakeHealth( float flHealth, int bitsDamageType );
	   virtual int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);

	   void RadiusDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );
	   void RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );
	   virtual void DeathSound ( void ) { return; };
	   virtual void PainSound ( void ) { return; };
};



#endif // BASEMONSTER_H
