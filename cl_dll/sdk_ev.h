#ifndef SDK_EV_H
#define SDK_EV_H


#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "entity_types.h"
#include "usercmd.h"
#include "pm_defs.h"
#include "pm_materials.h"

#include "eventscripts.h"
#include "ev_hldm.h"

#include "r_efx.h"
#include "event_api.h"
#include "event_args.h"
#include "in_defs.h"

#include <string.h>

static int tracerCount[ 32 ];


extern "C" char PM_FindTextureType( char *name );

void V_PunchAxis( int axis, float punch );

extern "C"
{
void EV_FireMP5NAVY( struct event_args_s *args );
void EV_FireTMP( struct event_args_s *args );
void EV_FireMAC10( struct event_args_s *args );
void EV_FireUMP45( struct event_args_s *args );
void EV_FireAK47( struct event_args_s *args );
void EV_FireSG552( struct event_args_s *args );
void EV_FireAUG( struct event_args_s *args );
void EV_FireM3( struct event_args_s *args );
void EV_FireXM1014( struct event_args_s *args );
void EV_FireSG550( struct event_args_s *args );
void EV_FireDEAGLE( struct event_args_s *args );
void EV_FireGLOCK18( struct event_args_s *args );
void EV_FireB92D( struct event_args_s *args );
void EV_FireM16( struct event_args_s *args );
}

#define VECTOR_CONE_1DEGREES Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES Vector( 0.06105, 0.06105, 0.06105 )	
#define VECTOR_CONE_8DEGREES Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES Vector( 0.17365, 0.17365, 0.17365 )

typedef enum
	{
	at_notice,
	at_console,		// same as at_notice, but forces a ConPrintf, not a message box
	at_aiconsole,	// same as at_console, but only shown if developer level is 2!
	at_warning,
	at_error,
	at_logged		// Server print to console ( only in multiplayer games ).
	} ALERT_TYPE;
void AlertMessage( ALERT_TYPE atype, char *szFmt, ... );

#endif