// Triangle rendering, if any
#include <windows.h>
//#include <gl/gl.h>
//#include <gl/glu.h>
#include "hud.h"
#include "cl_util.h"
 
//#pragma comment( lib, "opengl32.lib" )
//#pragma comment( lib, "glu32.lib" )

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "r_studioint.h"

#define DLLEXPORT __declspec( dllexport )

extern "C"
{
	void DLLEXPORT HUD_DrawNormalTriangles( void );
	void DLLEXPORT HUD_DrawTransparentTriangles( void );
};

//#define TEST_IT
//#if  defined( TEST_IT )
//#ifndef TEST_IT

/*
=================
Draw_Triangles

Example routine.  Draws a sprite offset from the player origin.
=================
*/
extern vec3_t radar_3d[2];
extern vec3_t myviewangle;
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
void VectorAngles( const vec3_t forward, vec3_t angles )
{
	float	tmp, yaw, pitch;
	
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt (forward[0]*forward[0] + forward[1]*forward[1]);
		pitch = (atan2(forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}
	
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
};

extern engine_studio_api_t IEngineStudio;
#define random()	((rand () & 0x7fff) / ((float)0x7fff))
void CrossProduct (const float *v1, const float *v2, float *cross);
int *gNANA;
extern cvar_t *cl_3dradar;
void Draw_Triangles( void )
{
   static HSPRITE hsprTexture = 0;
   const model_s *pTexture;
	cl_entity_t *player;
	vec3_t org, org1, org2, org3;
	vec3_t angles;
   vec3_t forward, right, up;
   vec3_t direction;
   vec3_t origin;
   float ff;


	// Load it up with some bogus data
	player = gEngfuncs.GetLocalPlayer();
	if ( !player )
		return;

   if (!cl_3dradar->value)
      return;

   IEngineStudio.GetViewInfo( origin, up, right, forward);
   org[0] = origin[0] -right[0] * 5 + forward[0] * 16 - up[0] * 10;
   org[1] = origin[1] -right[1] * 5 + forward[1] * 16 - up[1] * 10;
   org[2] = origin[2] -right[2] * 5 + forward[2] * 16 - up[2] * 10;
     

   if (!hsprTexture)
      {
      hsprTexture = LoadSprite( "sprites/animglow01.spr" );
      }
   pTexture = gEngfuncs.GetSpritePointer( hsprTexture ); 
   gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)pTexture, 0 ); 

   // Create a triangle, sigh
	gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );
	gEngfuncs.pTriAPI->Begin( TRI_TRIANGLES ); //TRI_QUADS
	// Overload p->color with index into tracer palette, p->packedColor with brightness
	
   // UNDONE: This gouraud shading causes tracers to disappear on some cards (permedia2)
   if (!*gNANA)
      {
      direction = radar_3d[0] - player->origin; 
      ff = direction.Length();
      if (ff < 2000)
         {
         if (ff > 600)
            gEngfuncs.pTriAPI->Color4f( 0.0, 1.0, 0.0, 0.2f );
         else
            gEngfuncs.pTriAPI->Color4f( 0.0, 1.0, 0.0, 1.0f );
         direction = direction.Normalize();
         org2[0] = org[0] + direction[0] * 2;
         org2[1] = org[1] + direction[1] * 2;
         org2[2] = org[2] + direction[2] * 2;
         forward[0] = direction[0];
         forward[1] = direction[1];
         forward[2] = direction[2] + 1;
         CrossProduct (direction, forward, right);
         right[0] /= 2; right[1] /= 2; right[2] /= 2;
         org1[0] = org[0] - right[0]; org3[0] = org[0] + right[0];
         org1[1] = org[1] - right[1]; org3[1] = org[1] + right[1];
         org1[2] = org[2] - right[2]; org3[2] = org[2] + right[2];

	      gEngfuncs.pTriAPI->Brightness( 1 );
	      gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
	      gEngfuncs.pTriAPI->Vertex3f( org1[0], org1[1], org1[2]);

	      gEngfuncs.pTriAPI->Brightness( 1 );
	      gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
	      gEngfuncs.pTriAPI->Vertex3f( org2[0], org2[1], org2[2]);

	      gEngfuncs.pTriAPI->Brightness( 1 );
	      gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
	      gEngfuncs.pTriAPI->Vertex3f( org3[0], org3[1], org3[2]);
         }
      }


	
   // UNDONE: This gouraud shading causes tracers to disappear on some cards (permedia2)
   direction = radar_3d[1] - player->origin;
   ff = direction.Length();
   if (ff < 2000)
      {
      if (ff > 600)
         gEngfuncs.pTriAPI->Color4f( 1.0, 0.0, 0.0, 0.2f );
      else
         gEngfuncs.pTriAPI->Color4f( 1.0, 0, 0.0, .5 + .5 * random() );
      direction = direction.Normalize();
      org[2] += .1f;
      org2[0] = org[0] + direction[0] * 2;
      org2[1] = org[1] + direction[1] * 2;
      org2[2] = org[2] + direction[2] * 2;
      forward[0] = direction[0];
      forward[1] = direction[1];
      forward[2] = direction[2] + 1;
      CrossProduct (direction, forward, right);
      right[0] /= 2; right[1] /= 2; right[2] /= 2;
      org1[0] = org[0] - right[0]; org3[0] = org[0] + right[0];
      org1[1] = org[1] - right[1]; org3[1] = org[1] + right[1];
      org1[2] = org[2] - right[2]; org3[2] = org[2] + right[2];


	   gEngfuncs.pTriAPI->Brightness( 1 );
	   gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
	   gEngfuncs.pTriAPI->Vertex3f( org1[0], org1[1], org1[2]);

	   gEngfuncs.pTriAPI->Brightness( 1 );
	   gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
	   gEngfuncs.pTriAPI->Vertex3f( org2[0], org2[1], org2[2]);

	   gEngfuncs.pTriAPI->Brightness( 1 );
	   gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
	   gEngfuncs.pTriAPI->Vertex3f( org3[0], org3[1], org3[2]);
      }

	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
}

//#endif

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void DLLEXPORT HUD_DrawNormalTriangles( void )
{

//#if defined( TEST_IT )
	//Draw_Triangles();
//#endif
}

/*
=================
HUD_DrawTransparentTriangles

Render any triangles with transparent rendermode needs here
=================
*/
extern engine_studio_api_t IEngineStudio;
void DLLEXPORT HUD_DrawTransparentTriangles( void )
{
   /*
   GLfloat fogColorogl[4] = {0.3f,0.3f,0.3f,1.0f}; // Fog Color 1.0f=255
   if( IEngineStudio.IsHardware() == 1 ) // OpenGL
      {
	   glFogi(GL_FOG_MODE, GL_LINEAR); // Fog Mode
	   glFogfv(GL_FOG_COLOR, fogColorogl); // Set Fog Color
	   glFogf(GL_FOG_DENSITY, (GLfloat)0.75f); // How Dense Will The Fog Be
	   glHint(GL_FOG_HINT, GL_NICEST); // Fog Hint Value
	   glFogf( GL_FOG_START, (GLfloat)200 );//Start 200 units  infront of the player
	   glFogf( GL_FOG_END, (GLfloat)1000 );//Where it will end 
	   glEnable(GL_FOG); // Enables GL_FOG
      }
   else
      exit(1);
   */

//#ifndef TEST_IT
//#if  defined( TEST_IT )
	Draw_Triangles();
   
//#endif
}

