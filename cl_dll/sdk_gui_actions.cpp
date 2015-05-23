/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_gui_actions.cpp 
   This is the player actions GUI module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#include "hud.h"
#include "cl_util.h"
#include "VGUI_Font.h"
#include "VGUI_ScrollPanel.h"
#include "VGUI_TextImage.h"
#include "vgui_int.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_ServerBrowser.h"
#include "sdk_e_menu.h"
#include "vgui_textpanel.h"
#include "sdk_w_matrix.h"
#include "sdk_e_menu.h"
#include "sdk_u_mp3.h"

extern cvar_t *cl_beats, *cl_chat;

CMenuHandler_FlipMode::CMenuHandler_FlipMode( int iState )
{
   m_iState = iState;
}

void CMenuHandler_FlipMode::actionPerformed(Panel* panel)
{
   switch (m_iState)
      {
      case ACTION_TOGGLE_AUDIO:
         if (cl_beats->value) 
            gEngfuncs.pfnClientCmd("cl_beats 0");
         else 
            gEngfuncs.pfnClientCmd("cl_beats 1");        
         gViewPort->HideTopMenu();
         break;
      case ACTION_TOGGLE_MUTE:
         if (cl_chat->value) 
            gEngfuncs.pfnClientCmd("cl_chat 0");
         else 
            gEngfuncs.pfnClientCmd("cl_chat 1");        
         gViewPort->HideTopMenu();
         break;
      default:
         break;
      }
}
