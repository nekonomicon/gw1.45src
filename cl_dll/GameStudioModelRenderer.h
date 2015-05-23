#if !defined( GAMESTUDIOMODELRENDERER_H )
#define GAMESTUDIOMODELRENDERER_H
#if defined( _WIN32 )
#pragma once
#endif

/*
====================
CGameStudioModelRenderer

====================
*/
class CGameStudioModelRenderer : public CStudioModelRenderer
{
public:
	CGameStudioModelRenderer( void );

	// Set up model bone positions
	virtual void StudioSetupBones ( void );	

	// Estimate gait frame for player
	virtual void StudioEstimateGait ( entity_state_t *pplayer );

	// Process movement of player
	virtual void StudioProcessGait ( entity_state_t *pplayer );

	// Player drawing code
	virtual int StudioDrawPlayer( int flags, entity_state_t *pplayer );
	virtual int _StudioDrawPlayer( int flags, entity_state_t *pplayer );

	// Apply special effects to transform matrix
	virtual void StudioFxTransform( cl_entity_t *ent, float transform[3][4] );

	// Player specific data
	// Determine pitch and blending amounts for players
	virtual void StudioPlayerBlend ( mstudioseqdesc_t *pseqdesc, int *pBlend, float *pPitch );
	virtual void CalculateYawBlend( entity_state_t *pplayer );
	virtual void CalculatePitchBlend( entity_state_t *pplayer );
private:
	// For local player, in third person, we need to store real render data and then
	//  setup for with fake/client side animation data
	void SavePlayerState( entity_state_t *pplayer );
	// Called to set up local player's animation values
	void SetupClientAnimation( entity_state_t *pplayer );
	// Called to restore original player state information
	void RestorePlayerState( entity_state_t *pplayer );
	// Called to get the correct pointer to the model's bones
	mstudioanim_t* LookupAnimation( mstudioseqdesc_t *pseqdesc, int index );

	int m_nPlayerGaitSequences[MAX_CLIENTS];

private: 
	// Private data
	bool m_bLocal;
};

#endif // GAMESTUDIOMODELRENDERER_H