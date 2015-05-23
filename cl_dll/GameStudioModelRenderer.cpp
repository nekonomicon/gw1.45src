#include <assert.h>
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "com_model.h"
#include "studio.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "dlight.h"
#include "triangleapi.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "studio_util.h"
#include "r_studioint.h"

#include "StudioModelRenderer.h"
#include "GameStudioModelRenderer.h"
#include "eiface.h"

void AlertMessage( ALERT_TYPE atype, char *szFmt, ... );

// Predicted values saved off in hl_weapons.cpp
void Game_GetSequence( int *seq, int *gaitseq );
void Game_GetOrientation( float *o, float *a );

float g_flStartScaleTime;
int iPrevRenderState;
int iRenderStateChanged;

// Global engine <-> studio model rendering code interface
extern engine_studio_api_t IEngineStudio;

/*#define ANIM_WALK_SEQUENCE			13 //was 3 -maybe 13
#define ANIM_JUMP_SEQUENCE			8 //was 6
#define ANIM_SWIM_1					10 //was 8
#define ANIM_SWIM_2					11 //was 9
#define ANIM_FIRST_DEATH_SEQUENCE	18 //was 89 - 111*/

#define ANIM_WALK_SEQUENCE			3
#define ANIM_JUMP_SEQUENCE			6
#define ANIM_SWIM_1					8
#define ANIM_SWIM_2					9
#define ANIM_FIRST_DEATH_SEQUENCE	89

typedef struct
{
	vec3_t		origin;
	vec3_t		angles;

	vec3_t		realangles;

	float		animtime;
	float		frame;
	int			sequence;
	int			gaitsequence;
	float		framerate;

	int			m_fSequenceLoops;
	int			m_fSequenceFinished;

	byte		controller[ 4 ];
	byte		blending[ 2 ];

	latchedvars_t	lv;
} client_anim_state_t;

static client_anim_state_t g_state;
static client_anim_state_t g_clientstate;

// The renderer object, created on the stack.
CGameStudioModelRenderer g_StudioRenderer;
//CStudioModelRenderer g_StudioRenderer;
/*
====================
CGameStudioModelRenderer

====================
*/
CGameStudioModelRenderer::CGameStudioModelRenderer( void )
{
	// If you want to predict animations locally, set this to TRUE
	// NOTE:  The animation code is somewhat broken, but gives you a sense for how
	//  to do client side animation of the predicted player in a third person game.
	m_bLocal = false;
}

/*
====================
LookupAnimation
->index starts at 0 for the first set of bones
====================
*/
mstudioanim_t* CGameStudioModelRenderer::LookupAnimation( mstudioseqdesc_t *pseqdesc, int index )
{
	mstudioanim_t *panim = NULL;

	panim = StudioGetAnim( m_pRenderModel, pseqdesc );
	
	assert( panim );
	
	if ( index < 0 )
	{
		return panim;
	}

	if ( index > ( pseqdesc->numblends - 1 ) )
	{
		return panim;
	}

	panim += index * m_pStudioHeader->numbones;

	return panim;
}

/*
====================
StudioSetupBones

====================
*/
void CGameStudioModelRenderer::StudioSetupBones ( void )
{
	int					i;
	double				f;

	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static float		pos[MAXSTUDIOBONES][3];
	static vec4_t		q[MAXSTUDIOBONES];
	float				bonematrix[3][4];

	static float		pos2[MAXSTUDIOBONES][3];
	static vec4_t		q2[MAXSTUDIOBONES];
	static float		pos3[MAXSTUDIOBONES][3];
	static vec4_t		q3[MAXSTUDIOBONES];
	static float		pos4[MAXSTUDIOBONES][3];
	static vec4_t		q4[MAXSTUDIOBONES];

	// Use default bone setup for nonplayers
	if ( !m_pCurrentEntity->player )
	{
		CStudioModelRenderer::StudioSetupBones();
		return;
	}

	// bounds checking.
	if ( m_pCurrentEntity->curstate.sequence >= m_pStudioHeader->numseq ) 
	{
		m_pCurrentEntity->curstate.sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->curstate.sequence;

	// pointer to the bone information
	panim = StudioGetAnim( m_pRenderModel, pseqdesc );

//tagES-HACK-HACK (always want the jump animation to start on frame zero)
	if ( m_pPlayerInfo )
	{
		int playerNum = m_pCurrentEntity->index - 1;

		// new jump gaitsequence?  start from frame zero
      // FLIFLA: New kill sequences?
		if (m_nPlayerGaitSequences[ playerNum ] != ANIM_JUMP_SEQUENCE)
		   {
         if (m_pPlayerInfo->gaitsequence == ANIM_JUMP_SEQUENCE )
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 85)
		   {
         if (m_pPlayerInfo->gaitsequence == 85)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 86)
		   {
         if (m_pPlayerInfo->gaitsequence == 86)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 87)
		   {
         if (m_pPlayerInfo->gaitsequence == 87)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 88)
		   {
         if (m_pPlayerInfo->gaitsequence == 88)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 89)
		   {
         if (m_pPlayerInfo->gaitsequence == 89)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 90)
		   {
         if (m_pPlayerInfo->gaitsequence == 90)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 91)
		   {
         if (m_pPlayerInfo->gaitsequence == 91)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 92)
		   {
         if (m_pPlayerInfo->gaitsequence == 92)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 93)
		   {
         if (m_pPlayerInfo->gaitsequence == 93)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		if ( m_nPlayerGaitSequences[ playerNum ] != 94)
		   {
         if (m_pPlayerInfo->gaitsequence == 94)
			   m_pPlayerInfo->gaitframe = 0.0;
		   }
		m_nPlayerGaitSequences[ playerNum ] = m_pPlayerInfo->gaitsequence;
	}

	f = StudioEstimateFrame( pseqdesc );

//tagES-HACK-HACK (walking animation needs to be turned a little to match the others)
	if ( m_pPlayerInfo->gaitsequence == ANIM_WALK_SEQUENCE )
	{
		if ( m_pCurrentEntity->curstate.blending[0] <= 26 )
		{
			m_pCurrentEntity->curstate.blending[0] = 0;
			m_pCurrentEntity->latched.prevseqblending[0] = m_pCurrentEntity->curstate.blending[0];
		}
		else
		{
			m_pCurrentEntity->curstate.blending[0] -= 26;
			m_pCurrentEntity->latched.prevseqblending[0] = m_pCurrentEntity->curstate.blending[0];
		}

	}

	if ( pseqdesc->numblends == 9 )
	{
		float s = m_pCurrentEntity->curstate.blending[0];	// yaw (left -> right)
		float t = m_pCurrentEntity->curstate.blending[1];	// pitch (top -> bottom)

		// nine-way blends are done left -> right, top -> bottom

		/***********************
		
			0	-	1	-	2

			|	\	|	/	|

			3	-	4	-	5

			|	/	|	\	|

			6	-	7	-	8

		 ***********************/

		// Blending is 0-127 == Left to Middle, 128 to 255 == Middle to Right
		if ( s <= 127.0 )
		{
			// Scale 0-127 blending up to 0-255
			s = ( s * 2.0 );

			if ( t <= 127.0 )
			{
				// Blending is 0-127 == Top to Middle, 128 to 255 == Middle to Bottom
				t = ( t * 2.0 );

				// need to blend 0 - 1 - 3 - 4

				StudioCalcRotations( pos, q, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 1 );
				StudioCalcRotations( pos2, q2, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 3 );
				StudioCalcRotations( pos3, q3, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 4 );
				StudioCalcRotations( pos4, q4, pseqdesc, panim, f );
			}
			else
			{
				// Scale 128-255 blending up to 0-255
				t = 2.0 * ( t - 127.0 );

				// need to blend 3 - 4 - 6 - 7

				panim = LookupAnimation( pseqdesc, 3 );
				StudioCalcRotations( pos, q, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 4 );
				StudioCalcRotations( pos2, q2, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 6 );
				StudioCalcRotations( pos3, q3, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 7 );
				StudioCalcRotations( pos4, q4, pseqdesc, panim, f );
			}
		}
		else
		{		
			// Scale 127-255 blending up to 0-255
			s = 2.0 * ( s - 127.0 );

			if ( t <= 127.0 )
			{
				// Blending is 0-127 == Top to Middle, 128 to 255 == Middle to Bottom
				t = ( t * 2.0 );

				// need to blend 1 - 2 - 4 - 5

				panim = LookupAnimation( pseqdesc, 1 );
				StudioCalcRotations( pos, q, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 2 );
				StudioCalcRotations( pos2, q2, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 4 );
				StudioCalcRotations( pos3, q3, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 5 );
				StudioCalcRotations( pos4, q4, pseqdesc, panim, f );
			}
			else
			{
				// Scale 128-255 blending up to 0-255
				t = 2.0 * ( t - 127.0 );

				// need to blend 4 - 5 - 7 - 8

				panim = LookupAnimation( pseqdesc, 4 );
				StudioCalcRotations( pos, q, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 5 );
				StudioCalcRotations( pos2, q2, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 7 );
				StudioCalcRotations( pos3, q3, pseqdesc, panim, f );
				panim = LookupAnimation( pseqdesc, 8 );
				StudioCalcRotations( pos4, q4, pseqdesc, panim, f );
			}
		}

		// Normalize interpolants
		s /= 255.0;
		t /= 255.0;

		// Spherically interpolate the bones
		StudioSlerpBones( q, pos, q2, pos2, s );
		StudioSlerpBones( q3, pos3, q4, pos4, s );

		StudioSlerpBones( q, pos, q3, pos3, t );
	}
	else
	{
		StudioCalcRotations( pos, q, pseqdesc, panim, f );
	}

	// Are we in the process of transitioning from one sequence to another.
	if (   m_fDoInterp &&
		   m_pCurrentEntity->latched.sequencetime &&
	     ( m_pCurrentEntity->latched.sequencetime + 0.2 > m_clTime ) && 
		 ( m_pCurrentEntity->latched.prevsequence < m_pStudioHeader->numseq ) )
	{
		// blend from last sequence
		static float		pos1b[MAXSTUDIOBONES][3];
		static vec4_t		q1b[MAXSTUDIOBONES];
		float				s = m_pCurrentEntity->latched.prevseqblending[0];
		float				t = m_pCurrentEntity->latched.prevseqblending[1];

		// Point at previous sequenece
		pseqdesc = (mstudioseqdesc_t *)((byte *)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->latched.prevsequence;

		// pointer to the bone information
		panim = StudioGetAnim( m_pRenderModel, pseqdesc );
		
		if ( pseqdesc->numblends == 9 )
		{
			// Blending is 0-127 == Left to Middle, 128 to 255 == Middle to Right
			if ( s <= 127.0 )
			{
				// Scale 0-127 blending up to 0-255
				s = ( s * 2.0 );

				if ( t <= 127.0 )
				{
					// Blending is 0-127 == Top to Middle, 128 to 255 == Middle to Bottom
					t = ( t * 2.0 );

					// need to blend 0 - 1 - 3 - 4

					StudioCalcRotations( pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 1 );
					StudioCalcRotations( pos2, q2, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 3);
					StudioCalcRotations( pos3, q3, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 4 );
					StudioCalcRotations( pos4, q4, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
				}
				else
				{
					// Scale 128-255 blending up to 0-255
					t = 2.0 * ( t - 127.0 );

					// need to blend 3 - 4 - 6 - 7

					panim = LookupAnimation( pseqdesc, 3 );
					StudioCalcRotations( pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 4 );
					StudioCalcRotations( pos2, q2, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 6 );
					StudioCalcRotations( pos3, q3, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 7 );
					StudioCalcRotations( pos4, q4, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
				}
			}
			else
			{		
				// Scale 127-255 blending up to 0-255
				s = 2.0 * ( s - 127.0 );

				if ( t <= 127.0 )
				{
					// Blending is 0-127 == Top to Middle, 128 to 255 == Middle to Bottom
					t = ( t * 2.0 );

					// need to blend 1 - 2 - 4 - 5

					panim = LookupAnimation( pseqdesc, 1 );
					StudioCalcRotations( pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 2 );
					StudioCalcRotations( pos2, q2, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 4 );
					StudioCalcRotations( pos3, q3, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 5 );
					StudioCalcRotations( pos4, q4, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
				}
				else
				{
					// Scale 128-255 blending up to 0-255
					t = 2.0 * ( t - 127.0 );

					// need to blend 4 - 5 - 7 - 8

					panim = LookupAnimation( pseqdesc, 4 );
					StudioCalcRotations( pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 5 );
					StudioCalcRotations( pos2, q2, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 7 );
					StudioCalcRotations( pos3, q3, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
					panim = LookupAnimation( pseqdesc, 8 );
					StudioCalcRotations( pos4, q4, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
				}
			}

			// Normalize interpolants
			s /= 255.0;
			t /= 255.0;

			// Spherically interpolate the bones
			StudioSlerpBones( q1b, pos1b, q2, pos2, s );
			StudioSlerpBones( q3, pos3, q4, pos4, s );

			StudioSlerpBones( q1b, pos1b, q3, pos3, t );

		}
		else
		{
			StudioCalcRotations( pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );
		}

		// Now blend last frame of previous sequence with current sequence.
		s = 1.0 - ( m_clTime - m_pCurrentEntity->latched.sequencetime ) / 0.2;
		StudioSlerpBones( q, pos, q1b, pos1b, s );
	}
	else
	{
		m_pCurrentEntity->latched.prevframe = f;
	}

	// Now convert quaternions and bone positions into matrices
	pbones = (mstudiobone_t *)( (byte *)m_pStudioHeader + m_pStudioHeader->boneindex );

	// calc gait animation 
	// death sequences are last in the list of sequences, and we don't do gait for those
	// ANIM_FIRST_DEATH_SEQUENCE marks the first death sequence
	if ( m_pPlayerInfo && 
		 m_pCurrentEntity->curstate.sequence < ANIM_FIRST_DEATH_SEQUENCE &&
		 m_pCurrentEntity->curstate.sequence != ANIM_SWIM_1 && 
		 m_pCurrentEntity->curstate.sequence != ANIM_SWIM_2 ) 
	{
		int copy = 1;

		// bounds checking
		if ( m_pPlayerInfo->gaitsequence >= m_pStudioHeader->numseq )
		{
			m_pPlayerInfo->gaitsequence = 0;
		}

		pseqdesc = (mstudioseqdesc_t *)( (byte *)m_pStudioHeader + m_pStudioHeader->seqindex ) + m_pPlayerInfo->gaitsequence;

		panim = StudioGetAnim( m_pRenderModel, pseqdesc );
		StudioCalcRotations( pos2, q2, pseqdesc, panim, m_pPlayerInfo->gaitframe );

		for ( i = 0; i < m_pStudioHeader->numbones; i++ )
		{
			if ( !strcmp( pbones[i].name, "Bip01 Spine" ) )
			{
				copy = 0;
			}
			else if ( !strcmp( pbones[ pbones[i].parent ].name, "Bip01 Pelvis" ) )
			{
				copy = 1;
			}
				
			if ( copy )
			{
				memcpy( pos[i], pos2[i], sizeof( pos[i] ) );
				memcpy( q[i], q2[i], sizeof( q[i] ) );
			}
		}
	}

	for (i = 0; i < m_pStudioHeader->numbones; i++) 
	{
		QuaternionMatrix( q[i], bonematrix );

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if ( pbones[i].parent == -1 ) 
		{
			if ( IEngineStudio.IsHardware() )
			{
				ConcatTransforms ( (*m_protationmatrix), bonematrix, (*m_pbonetransform)[i] );
				//ConcatTransforms ( (*m_protationmatrix), bonematrix, (*m_plighttransform)[i] );
				MatrixCopy( (*m_pbonetransform)[i], (*m_plighttransform)[i] );
			}
			else
			{
				ConcatTransforms ( (*m_paliastransform), bonematrix, (*m_pbonetransform)[i] );
				ConcatTransforms ( (*m_protationmatrix), bonematrix, (*m_plighttransform)[i] );
			}

			// Apply client-side effects to the transformation matrix
			StudioFxTransform( m_pCurrentEntity, (*m_pbonetransform)[i] );
		} 
		else 
		{
			ConcatTransforms ( (*m_pbonetransform)[pbones[i].parent], bonematrix, (*m_pbonetransform)[i] );
			ConcatTransforms ( (*m_plighttransform)[pbones[i].parent], bonematrix, (*m_plighttransform)[i] );
		}
	}
}

/*
====================
StudioEstimateGait

====================
*/
void CGameStudioModelRenderer::StudioEstimateGait( entity_state_t *pplayer )
{
	float dt;
	vec3_t est_velocity;

	dt = (m_clTime - m_clOldTime);
	dt = max( 0.0, dt );
	dt = min( 1.0, dt );

	if (dt == 0 || m_pPlayerInfo->renderframe == m_nFrameCount)
	   {
		m_flGaitMovement = 0;
		return;
	   }

	// VectorAdd( pplayer->velocity, pplayer->prediction_error, est_velocity );
	if ( m_fGaitEstimation )
	   {
		VectorSubtract( m_pCurrentEntity->origin, m_pPlayerInfo->prevgaitorigin, est_velocity );
		VectorCopy( m_pCurrentEntity->origin, m_pPlayerInfo->prevgaitorigin );
		m_flGaitMovement = Length( est_velocity );
		if (dt <= 0 || m_flGaitMovement / dt < 5)
		   {
			m_flGaitMovement = 0;
			est_velocity[0] = 0;
			est_velocity[1] = 0;
		   }
	   }
	else
	   {
		VectorCopy( pplayer->velocity, est_velocity );
		m_flGaitMovement = Length( est_velocity ) * dt;
	   }

	if (est_velocity[1] == 0 && est_velocity[0] == 0)
	   {
		float flYawDiff = m_pCurrentEntity->angles[YAW] - m_pPlayerInfo->gaityaw;
		flYawDiff = flYawDiff - (int)(flYawDiff / 360) * 360;
		if (flYawDiff > 180)
			flYawDiff -= 360;
		if (flYawDiff < -180)
			flYawDiff += 360;

		if (dt < 0.25)
			flYawDiff *= dt * 4;
		else
			flYawDiff *= dt;

		m_pPlayerInfo->gaityaw += flYawDiff;
		m_pPlayerInfo->gaityaw = m_pPlayerInfo->gaityaw - (int)(m_pPlayerInfo->gaityaw / 360) * 360;

		m_flGaitMovement = 0;
	   }
	else
	   {
		m_pPlayerInfo->gaityaw = (atan2(est_velocity[1], est_velocity[0]) * 180 / M_PI);
		if (m_pPlayerInfo->gaityaw > 180)
			m_pPlayerInfo->gaityaw = 180;
		if (m_pPlayerInfo->gaityaw < -180)
			m_pPlayerInfo->gaityaw = -180;
	   }
}

/*
====================
StudioPlayerBlend

====================
*/
void CGameStudioModelRenderer::StudioPlayerBlend( mstudioseqdesc_t *pseqdesc, int *pBlend, float *pPitch )
{
	// models use a pitch range of 45 degrees ~ -45 degrees
	float range = 45.0;

	// calc up/down pointing
	*pBlend = (*pPitch * 3);

	if ( *pBlend <= -range )
	   {
		*pBlend = 255;
	   }
	else if ( *pBlend >= range )
	   {
		*pBlend = 0;
	   }
	else
	   {
		*pBlend = 255 * ( range - *pBlend ) / ( 2 * range );
	   }

	*pPitch = 0;
}

/*
====================
CalculatePitchBlend

====================
*/
void CGameStudioModelRenderer::CalculatePitchBlend( entity_state_t *pplayer )
{
	mstudioseqdesc_t	*pseqdesc;
	int					iBlend;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->curstate.sequence;

	StudioPlayerBlend( pseqdesc, &iBlend, &m_pCurrentEntity->angles[PITCH] );

	m_pCurrentEntity->latched.prevangles[PITCH] = m_pCurrentEntity->angles[PITCH];
	m_pCurrentEntity->curstate.blending[1] = iBlend;
	m_pCurrentEntity->latched.prevblending[1] = m_pCurrentEntity->curstate.blending[1];
	m_pCurrentEntity->latched.prevseqblending[1] = m_pCurrentEntity->curstate.blending[1];
}

/*
====================
CalculateYawBlend

====================
*/
void CGameStudioModelRenderer::CalculateYawBlend( entity_state_t *pplayer )
{
	float				dt;
	float				flYaw;	// view direction relative to movement ( left <-> right )

	dt = (m_clTime - m_clOldTime);
	dt = max( 0.0, dt );
	dt = min( 1.0, dt );

	StudioEstimateGait( pplayer );

	// calc side to side turning
	flYaw = m_pCurrentEntity->angles[YAW] - m_pPlayerInfo->gaityaw;

	flYaw = fmod( flYaw, 360.0 );

	if (flYaw < -180)
	   {
		flYaw = flYaw + 360;
	   }
	else if (flYaw > 180)
	   {
		flYaw = flYaw - 360;
	   }

   // FLIFLA: Used to be 120
   // FIXME
	//float maxyaw = 180.0; 
	if (flYaw > 120)
	   {
		m_pPlayerInfo->gaityaw = m_pPlayerInfo->gaityaw - 180;
		m_flGaitMovement = -m_flGaitMovement;
		flYaw = flYaw - 180;
	   }
	else if (flYaw < -120.0)
	   {
		m_pPlayerInfo->gaityaw = m_pPlayerInfo->gaityaw + 180;
		m_flGaitMovement = -m_flGaitMovement;
		flYaw = flYaw + 180;
	   }

	float blend_yaw = ( flYaw / 90.0 ) * 128.0 + 127.0;

	blend_yaw = min( 255.0, blend_yaw );
	blend_yaw = max( 0.0, blend_yaw );
	
	blend_yaw = 255.0 - blend_yaw;

	m_pCurrentEntity->curstate.blending[0] = (int)(blend_yaw);

	m_pCurrentEntity->latched.prevblending[0] = m_pCurrentEntity->curstate.blending[0];
	m_pCurrentEntity->latched.prevseqblending[0] = m_pCurrentEntity->curstate.blending[0];

	m_pCurrentEntity->angles[YAW] = m_pPlayerInfo->gaityaw;
	if (m_pCurrentEntity->angles[YAW] < -0)
	   {
		m_pCurrentEntity->angles[YAW] += 360;
	   }
	m_pCurrentEntity->latched.prevangles[YAW] = m_pCurrentEntity->angles[YAW];
}

/*
====================
StudioProcessGait

====================
*/
void CGameStudioModelRenderer::StudioProcessGait( entity_state_t *pplayer )
{
	mstudioseqdesc_t	*pseqdesc;
	float				dt;

	CalculateYawBlend( pplayer );
	CalculatePitchBlend( pplayer );

	dt = (m_clTime - m_clOldTime);
	dt = max( 0.0, dt );
	dt = min( 1.0, dt );

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pStudioHeader + m_pStudioHeader->seqindex) + pplayer->gaitsequence;

	// Calc gait frame
	if (pseqdesc->linearmovement[0] > 0)
	{
		m_pPlayerInfo->gaitframe += (m_flGaitMovement / pseqdesc->linearmovement[0]) * pseqdesc->numframes;
	}
	else
	{
		m_pPlayerInfo->gaitframe += pseqdesc->fps * dt * m_pCurrentEntity->curstate.framerate;
	}

	// Do modulo
	m_pPlayerInfo->gaitframe = m_pPlayerInfo->gaitframe - (int)(m_pPlayerInfo->gaitframe / pseqdesc->numframes) * pseqdesc->numframes;
	if (m_pPlayerInfo->gaitframe < 0)
	{
		m_pPlayerInfo->gaitframe += pseqdesc->numframes;
	}
}

/*
==============================
SavePlayerState

For local player, in third person, we need to store real render data and then
  setup for with fake/client side animation data
==============================
*/
void CGameStudioModelRenderer::SavePlayerState( entity_state_t *pplayer )
{
	client_anim_state_t *st;
	cl_entity_t *ent = IEngineStudio.GetCurrentEntity();
	assert( ent );
	if ( !ent )
		return;

	st = &g_state;

	st->angles		 = ent->curstate.angles;
	st->origin		 = ent->curstate.origin;

	st->realangles	 = ent->angles;

	st->sequence	 = ent->curstate.sequence;
	st->gaitsequence = pplayer->gaitsequence;
	st->animtime	 = ent->curstate.animtime;
	st->frame		 = ent->curstate.frame;
	st->framerate	 = ent->curstate.framerate;
	memcpy( st->blending, ent->curstate.blending, 2 );
	memcpy( st->controller, ent->curstate.controller, 4 );

	st->lv = ent->latched;
}

void GetSequenceInfo( void *pmodel, client_anim_state_t *pev, float *pflFrameRate, float *pflGroundSpeed )
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

int GetSequenceFlags( void *pmodel, client_anim_state_t *pev )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if ( !pstudiohdr || pev->sequence >= pstudiohdr->numseq )
		return 0;

	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	return pseqdesc->flags;
}

float StudioFrameAdvance ( client_anim_state_t *st, float framerate, float flInterval )
{
	if (flInterval == 0.0)
	{
		flInterval = (gEngfuncs.GetClientTime() - st->animtime);
		if (flInterval <= 0.001)
		{
			st->animtime = gEngfuncs.GetClientTime();
			return 0.0;
		}
	}
	if (!st->animtime)
		flInterval = 0.0;
	
	st->frame += flInterval * framerate * st->framerate;
	st->animtime = gEngfuncs.GetClientTime();

	if (st->frame < 0.0 || st->frame >= 256.0) 
	{
		if ( st->m_fSequenceLoops )
			st->frame -= (int)(st->frame / 256.0) * 256.0;
		else
			st->frame = (st->frame < 0.0) ? 0 : 255;
		st->m_fSequenceFinished = TRUE;	// just in case it wasn't caught in GetEvents
	}

	return flInterval;
}

/*
==============================
SetupClientAnimation

Called to set up local player's animation values
==============================
*/
void CGameStudioModelRenderer::SetupClientAnimation( entity_state_t *pplayer )
{
	static double oldtime;
	double curtime, dt;

	client_anim_state_t *st;
	float fr, gs;

	cl_entity_t *ent = IEngineStudio.GetCurrentEntity();
	assert( ent );
	if ( !ent )
		return;

	curtime = gEngfuncs.GetClientTime();
	dt = curtime - oldtime;
	dt = min( 1.0, max( 0.0, dt ) );

	oldtime = curtime;
	st = &g_clientstate;
	
	st->framerate = 1.0;

	int oldseq = st->sequence;
	Game_GetSequence( &st->sequence, &st->gaitsequence );
	Game_GetOrientation( (float *)&st->origin, (float *)&st->angles );
	st->realangles = st->angles;

	if ( st->sequence != oldseq )
	{
		st->frame = 0.0;
		st->lv.prevsequence = oldseq;
		st->lv.sequencetime = st->animtime;

		memcpy( st->lv.prevseqblending, st->blending, 2 );
		memcpy( st->lv.prevcontroller, st->controller, 4 );
	}

	void *pmodel = (studiohdr_t *)IEngineStudio.Mod_Extradata( ent->model );

	GetSequenceInfo( pmodel, st, &fr, &gs );
	st->m_fSequenceLoops = ((GetSequenceFlags( pmodel, st ) & STUDIO_LOOPING) != 0);
	StudioFrameAdvance( st, fr, dt );
	
	ent->angles				= st->realangles;
	ent->curstate.angles	= st->angles;
	ent->curstate.origin	= st->origin;

	ent->curstate.sequence	= st->sequence;
	pplayer->gaitsequence   = st->gaitsequence;
	ent->curstate.animtime	= st->animtime;
	ent->curstate.frame		= st->frame;
	ent->curstate.framerate	= st->framerate;
	memcpy( ent->curstate.blending, st->blending, 2 );
	memcpy( ent->curstate.controller, st->controller, 4 );

	ent->latched = st->lv;
}

/*
==============================
RestorePlayerState

Called to restore original player state information
==============================
*/
void CGameStudioModelRenderer::RestorePlayerState( entity_state_t *pplayer )
{
	client_anim_state_t *st;
	cl_entity_t *ent = IEngineStudio.GetCurrentEntity();
	assert( ent );
	if ( !ent )
		return;

	st = &g_clientstate;

	st->angles		= ent->curstate.angles;
	st->origin		= ent->curstate.origin;
	st->realangles  = ent->angles;

	st->sequence	= ent->curstate.sequence;
	st->gaitsequence = pplayer->gaitsequence;
	st->animtime	= ent->curstate.animtime;
	st->frame		= ent->curstate.frame;
	st->framerate	= ent->curstate.framerate;
	memcpy( st->blending, ent->curstate.blending, 2 );
	memcpy( st->controller, ent->curstate.controller, 4 );

	st->lv = ent->latched;

	st = &g_state;

	ent->curstate.angles	= st->angles;
	ent->curstate.origin	= st->origin;
	ent->angles				= st->realangles;

	ent->curstate.sequence	= st->sequence;
	pplayer->gaitsequence = st->gaitsequence;
	ent->curstate.animtime	= st->animtime;
	ent->curstate.frame		= st->frame;
	ent->curstate.framerate	= st->framerate;
	memcpy( ent->curstate.blending, st->blending, 2 );
	memcpy( ent->curstate.controller, st->controller, 4 );

	ent->latched = st->lv;
}

/*
==============================
StudioDrawPlayer

==============================
*/
int CGameStudioModelRenderer::StudioDrawPlayer( int flags, entity_state_t *pplayer )
{
	int iret = 0;

	bool isLocalPlayer = false;
		
	// Set up for client?
	if ( m_bLocal && IEngineStudio.GetCurrentEntity() == gEngfuncs.GetLocalPlayer() )
	{
		isLocalPlayer = true;
	}

	if ( isLocalPlayer )
	{
		// Store original data
		SavePlayerState( pplayer );

		// Copy in client side animation data
		SetupClientAnimation( pplayer );
	}

	// Call real draw function
	iret = _StudioDrawPlayer( flags, pplayer );

	// Restore for client?
	if ( isLocalPlayer )
	{
		// Restore the original data for the player
		RestorePlayerState( pplayer );
	}

	return iret;
}

bool WeaponHasAttachments( entity_state_t *pplayer )
{
	studiohdr_t *modelheader = NULL;
	model_t *pweaponmodel;

	if ( !pplayer )
		return FALSE;

	pweaponmodel = IEngineStudio.GetModelByIndex( pplayer->weaponmodel );

	modelheader = (studiohdr_t *)IEngineStudio.Mod_Extradata( pweaponmodel );
	return (modelheader->numattachments != 0);
}

/*
====================
_StudioDrawPlayer

====================
*/
int CGameStudioModelRenderer::_StudioDrawPlayer( int flags, entity_state_t *pplayer )
{
	alight_t lighting;
	vec3_t dir;

	m_pCurrentEntity = IEngineStudio.GetCurrentEntity();
	IEngineStudio.GetTimes( &m_nFrameCount, &m_clTime, &m_clOldTime );
	IEngineStudio.GetViewInfo( m_vRenderOrigin, m_vUp, m_vRight, m_vNormal );
	IEngineStudio.GetAliasScale( &m_fSoftwareXScale, &m_fSoftwareYScale );

	m_nPlayerIndex = pplayer->number - 1;

	if (m_nPlayerIndex < 0 || m_nPlayerIndex >= gEngfuncs.GetMaxClients())
		return 0;

	m_pRenderModel = IEngineStudio.SetupPlayerModel( m_nPlayerIndex );
	if (m_pRenderModel == NULL)
		return 0;

	m_pStudioHeader = (studiohdr_t *)IEngineStudio.Mod_Extradata (m_pRenderModel);
	IEngineStudio.StudioSetHeader( m_pStudioHeader );
	IEngineStudio.SetRenderModel( m_pRenderModel );

	// bounds checking sequence
	//--
	if ( m_pCurrentEntity->curstate.sequence >= m_pStudioHeader->numseq )
	{
		m_pCurrentEntity->curstate.sequence = 0;
	}

	if ( pplayer->sequence >= m_pStudioHeader->numseq )
	{
		pplayer->sequence = 0;
	}
	//--

	// bounds checking gaitsequence
	//--
	if ( m_pCurrentEntity->curstate.gaitsequence >= m_pStudioHeader->numseq )
	{
		m_pCurrentEntity->curstate.gaitsequence = 0;
	}

	if ( pplayer->gaitsequence >= m_pStudioHeader->numseq )
	{
		pplayer->gaitsequence = 0;
	}
	//--

	if (pplayer->gaitsequence)
	{
		vec3_t orig_angles;
		m_pPlayerInfo = IEngineStudio.PlayerInfo( m_nPlayerIndex );

		VectorCopy( m_pCurrentEntity->angles, orig_angles );
	
		StudioProcessGait( pplayer );

		m_pPlayerInfo->gaitsequence = pplayer->gaitsequence;
		m_pPlayerInfo = NULL;

		StudioSetUpTransform( 0 );
		VectorCopy( orig_angles, m_pCurrentEntity->angles );
	}
	else
	{
		m_pCurrentEntity->curstate.controller[0] = 127;
		m_pCurrentEntity->curstate.controller[1] = 127;
		m_pCurrentEntity->curstate.controller[2] = 127;
		m_pCurrentEntity->curstate.controller[3] = 127;
		m_pCurrentEntity->latched.prevcontroller[0] = m_pCurrentEntity->curstate.controller[0];
		m_pCurrentEntity->latched.prevcontroller[1] = m_pCurrentEntity->curstate.controller[1];
		m_pCurrentEntity->latched.prevcontroller[2] = m_pCurrentEntity->curstate.controller[2];
		m_pCurrentEntity->latched.prevcontroller[3] = m_pCurrentEntity->curstate.controller[3];

		m_pPlayerInfo = IEngineStudio.PlayerInfo( m_nPlayerIndex );

		CalculatePitchBlend( pplayer );
		CalculateYawBlend( pplayer );

		m_pPlayerInfo->gaitsequence = 0;
		StudioSetUpTransform( 0 );
	}

	if (flags & STUDIO_RENDER)
	{
		// see if the bounding box lets us trivially reject, also sets
		if (!IEngineStudio.StudioCheckBBox ())
			return 0;

		(*m_pModelsDrawn)++;
		(*m_pStudioModelCount)++; // render data cache cookie

		if (m_pStudioHeader->numbodyparts == 0)
			return 1;
	}

	m_pPlayerInfo = IEngineStudio.PlayerInfo( m_nPlayerIndex );
	StudioSetupBones( );
	StudioSaveBones( );
	m_pPlayerInfo->renderframe = m_nFrameCount;

	m_pPlayerInfo = NULL;

	if (flags & STUDIO_EVENTS
		&& ( !( flags & STUDIO_RENDER ) || !pplayer->weaponmodel || !WeaponHasAttachments(pplayer) ) )
	{
		StudioCalcAttachments( );
		IEngineStudio.StudioClientEvents( );
		// copy attachments into global entity array
		if ( m_pCurrentEntity->index > 0 )
		{
			cl_entity_t *ent = gEngfuncs.GetEntityByIndex( m_pCurrentEntity->index );

			memcpy( ent->attachment, m_pCurrentEntity->attachment, sizeof( vec3_t ) * 4 );
		}
	}

	if (flags & STUDIO_RENDER)
	{
		/*
		if (m_pCvarHiModels->value && m_pRenderModel != m_pCurrentEntity->model  )
		{
			// show highest resolution multiplayer model
			m_pCurrentEntity->curstate.body = 255;
		}

		if (!(m_pCvarDeveloper->value == 0 && gEngfuncs.GetMaxClients() == 1 ) && ( m_pRenderModel == m_pCurrentEntity->model ) )
		{
			m_pCurrentEntity->curstate.body = 1; // force helmet
		}
		*/

		lighting.plightvec = dir;
		IEngineStudio.StudioDynamicLight(m_pCurrentEntity, &lighting );

		IEngineStudio.StudioEntityLight( &lighting );

		// model and frame independant
		IEngineStudio.StudioSetupLighting (&lighting);

		m_pPlayerInfo = IEngineStudio.PlayerInfo( m_nPlayerIndex );

		// get remap colors
		m_nTopColor = m_pPlayerInfo->topcolor;
		if (m_nTopColor < 0)
			m_nTopColor = 0;
		if (m_nTopColor > 360)
			m_nTopColor = 360;
		m_nBottomColor = m_pPlayerInfo->bottomcolor;
		if (m_nBottomColor < 0)
			m_nBottomColor = 0;
		if (m_nBottomColor > 360)
			m_nBottomColor = 360;

		IEngineStudio.StudioSetRemapColors( m_nTopColor, m_nBottomColor );

		StudioRenderModel( );
		m_pPlayerInfo = NULL;

		if ( pplayer->weaponmodel )
		{
			studiohdr_t *saveheader = m_pStudioHeader;
			cl_entity_t saveent = *m_pCurrentEntity;

			model_t *pweaponmodel = IEngineStudio.GetModelByIndex( pplayer->weaponmodel );

			m_pStudioHeader = (studiohdr_t *)IEngineStudio.Mod_Extradata (pweaponmodel);
			IEngineStudio.StudioSetHeader( m_pStudioHeader );

			StudioMergeBones( pweaponmodel );

			IEngineStudio.StudioSetupLighting( &lighting );

			StudioRenderModel( );

			StudioCalcAttachments( );

			if ( m_pCurrentEntity->index > 0 )
				memcpy( saveent.attachment, m_pCurrentEntity->attachment, sizeof( vec3_t ) * m_pStudioHeader->numattachments );

			*m_pCurrentEntity = saveent;
			m_pStudioHeader = saveheader;
			IEngineStudio.StudioSetHeader( m_pStudioHeader );

			if ( flags & STUDIO_EVENTS )
			{
				IEngineStudio.StudioClientEvents( );
			}
		}
	}

	return 1;
}

/*
====================
Studio_FxTransform

====================
*/
void CGameStudioModelRenderer::StudioFxTransform( cl_entity_t *ent, float transform[3][4] )
{
	switch( ent->curstate.renderfx )
	{
	case kRenderFxDistort:
	case kRenderFxHologram:
		if ( gEngfuncs.pfnRandomLong(0,49) == 0 )
		{
			int axis = gEngfuncs.pfnRandomLong(0,1);
			if ( axis == 1 ) // Choose between x & z
				axis = 2;
			VectorScale( transform[axis], gEngfuncs.pfnRandomFloat(1,1.484), transform[axis] );
		}
		else if ( gEngfuncs.pfnRandomLong(0,49) == 0 )
		{
			float offset;
			int axis = gEngfuncs.pfnRandomLong(0,1);
			if ( axis == 1 ) // Choose between x & z
				axis = 2;
			offset = gEngfuncs.pfnRandomFloat(-10,10);
			transform[gEngfuncs.pfnRandomLong(0,2)][3] += offset;
		}
		break;
	case kRenderFxExplode:
		{
			if ( iRenderStateChanged )
			{
				g_flStartScaleTime = m_clTime;
				iRenderStateChanged = FALSE;
			}

			// Make the Model continue to shrink
			float flTimeDelta = m_clTime - g_flStartScaleTime;
			if ( flTimeDelta > 0 )
			{
				float flScale = 0.001;
				// Goes almost all away
				if ( flTimeDelta <= 2.0 )
					flScale = 1.0 - (flTimeDelta / 2.0);

				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
						transform[i][j] *= flScale;
				}
			}
		}
		break;
	}
}

////////////////////////////////////
// Hooks to class implementation
////////////////////////////////////

/*
====================
R_StudioDrawPlayer

====================
*/
int R_StudioDrawPlayer( int flags, entity_state_t *pplayer )
{
	return g_StudioRenderer.StudioDrawPlayer( flags, pplayer );
}

/*
====================
R_StudioDrawModel

====================
*/
int R_StudioDrawModel( int flags )
{
	return g_StudioRenderer.StudioDrawModel( flags );
}

/*
====================
R_StudioInit

====================
*/
void R_StudioInit( void )
{
	g_StudioRenderer.Init();
}

// The simple drawing interface we'll pass back to the engine
r_studio_interface_t studio =
{
	STUDIO_INTERFACE_VERSION,
	R_StudioDrawModel,
	R_StudioDrawPlayer,
};

/*
====================
HUD_GetStudioModelInterface

Export this function for the engine to use the studio renderer class to render objects.
====================
*/
#undef DLLEXPORT
#define DLLEXPORT __declspec( dllexport )
extern "C" int DLLEXPORT HUD_GetStudioModelInterface( int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio )
{
	if ( version != STUDIO_INTERFACE_VERSION )
		return 0;

	// Point the engine to our callbacks
	*ppinterface = &studio;

	// Copy in engine helper functions
	memcpy( &IEngineStudio, pstudio, sizeof( IEngineStudio ) );

	// Initialize local variables, etc.
	R_StudioInit();

	// Success
	return 1;
}
