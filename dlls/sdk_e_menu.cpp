/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_e_menu.cpp
   This is the server side menu module.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#ifndef SDK_MENU_CPP
#define SDK_MENU_CPP

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"game.h"
#include "sdk_e_menu.h"
#include "sdk_u_main.h"
#include "sdk_w_armory.h"

extern int gmsgShowMenu;
extern int gmsgCSOSMenu;

#define CANCEL_BIT      (1<<9)
avail_weapons_t aw, old_aw;

void BuyDropWeapon(CBasePlayer *player, int primary);
CBasePlayerItem *GetWeaponByWeight(CBasePlayer *pPlayer, int weight);

extern int gmsgMoney;

void ShowMenu (CBasePlayer *pPlayer, int bitsValidSlots, int nDisplayTime, BOOL fNeedMore, char *pszText)
{
   MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pPlayer->pev);
      WRITE_SHORT( bitsValidSlots);
      WRITE_CHAR( nDisplayTime );
      WRITE_BYTE( fNeedMore );
      WRITE_STRING (pszText);
   MESSAGE_END();
}

void UpdateTeamMembers();
void CSOSMenu2(CBasePlayer *pPlayer, short menu_type)
{
   pPlayer->m_iMenu = 1;
   pPlayer->m_iPlayerStatus = menu_type;
   UpdateTeamMembers();
   MESSAGE_BEGIN(MSG_ONE, gmsgCSOSMenu, NULL, pPlayer->pev);
      WRITE_BYTE(menu_type);
      WRITE_BYTE(pPlayer->m_iTeam);
      WRITE_BYTE(team[TEAM_EAST].members);
      WRITE_BYTE(team[TEAM_WEST].members);
   MESSAGE_END();
}

void CSOSMenu(CBasePlayer *pPlayer, short menu_type)
{
   pPlayer->m_iMenu = 1;
   pPlayer->m_iPlayerStatus = menu_type;
   MESSAGE_BEGIN(MSG_ONE, gmsgCSOSMenu, NULL, pPlayer->pev);
      WRITE_BYTE(menu_type);
      WRITE_BYTE(pPlayer->m_iLicense);
   MESSAGE_END();
}

void CSOSMenuOptions(CBasePlayer *pPlayer, short menu_type)
{
   pPlayer->m_iMenu = 1;
   pPlayer->m_iPlayerStatus = menu_type;
   MESSAGE_BEGIN(MSG_ONE, gmsgCSOSMenu, NULL, pPlayer->pev);
      WRITE_BYTE(menu_type);
      WRITE_BYTE(pPlayer->m_iLicense);
      WRITE_BYTE(aw.pistols);
      WRITE_BYTE(aw.shotguns);
      WRITE_BYTE(aw.subs);
      WRITE_BYTE(aw.autos);
      WRITE_BYTE(0);
      WRITE_BYTE(0);
   MESSAGE_END();
}

void CSOSMenuItems(CBasePlayer *pPlayer, short menu_type)
{
}

void DisplayTeamMenu(CBasePlayer *pPlayer)
{
   CSOSMenu2(pPlayer, STATUS_PLAYER_CHOOSETEAM);
}

void DisplayGroupMenu(CBasePlayer *pPlayer, int team)
{
   pPlayer->m_iGroup = -1;
   CSOSMenu2(pPlayer, STATUS_PLAYER_CHOOSEGROUP);
}

void DisplayBuyWeaponMenu(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenuOptions(pPlayer, STATUS_PLAYER_BUYWEAPONMENU);
}

void DisplayBuyPistolMenu(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenu(pPlayer, STATUS_PLAYER_BUYPISTOLMENU);
}

void DisplayBuyShotgunMenu(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->m_iLicense < LICENSE_SHOTGUN) return;
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenu(pPlayer, STATUS_PLAYER_BUYSHOTGUNMENU);
}

void DisplayBuySubMenu(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->m_iLicense < LICENSE_SUB) return;
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenu(pPlayer, STATUS_PLAYER_BUYSUBMENU);
}

void DisplayBuyAutoMenu(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->m_iLicense < LICENSE_MRIFLE) return;
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenu(pPlayer, STATUS_PLAYER_BUYAUTOMENU);
}

void DisplayBuyRifleMenu(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->m_iLicense < LICENSE_SNIPER) return;
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenu(pPlayer, STATUS_PLAYER_BUYRIFLEMENU);
}

void DisplayBuySpecialtyGunMenu(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->m_iLicense < LICENSE_SNIPER) return;
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenu(pPlayer, STATUS_PLAYER_BUYSPECIALTYGUNMENU);
}

void DisplayBuyItemMenu(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenuItems(pPlayer, STATUS_PLAYER_BUYITEMMENU);
}

void DisplayAudioMenu1(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenu(pPlayer, STATUS_PLAYER_AUDIOMENU1);
}

void DisplayAudioMenu2(CBasePlayer *pPlayer, int team)
{
   if (pPlayer->pev->deadflag > DEAD_NO) return;
   CSOSMenu(pPlayer, STATUS_PLAYER_AUDIOMENU2);
}

int BuyWeapon(CBasePlayer *pPlayer, int weapon_id, int price, char *weapon_name, int primary)
{
   if (pPlayer->pev->deadflag > DEAD_NO) return -1;
   if (pPlayer->m_iMoney >= price)
      {
      pPlayer->m_iMoney -= price;
      MESSAGE_BEGIN(MSG_ONE, gmsgMoney, NULL, pPlayer->pev);
   	   WRITE_SHORT( pPlayer->m_iMoney );
      MESSAGE_END();
      BuyDropWeapon(pPlayer, primary);
      pPlayer->GiveNamedItem(weapon_name);
      return 1;
      }
   return 0;
}

int BuyPrimaryAmmo(CBasePlayer *pPlayer)
{
   bool bought = 0;
   if (pPlayer->pev->deadflag > DEAD_NO) return -1;
   CBasePlayerItem *pItem = GetWeaponByWeight(pPlayer, WEIGHT_PRIMARY);
   // No primary gun:
   if (!pItem) return 0;
   switch (pItem->m_iId)
      {
      // 9mm / 0.357":
      case WEAPON_MAC10:
         if (pPlayer->m_iLicense >= LICENSE_SUB)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_9mm")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_9MM) 
            {
				pPlayer->GiveAmmo(w_stat[WEAPON_MAC10].clip, "ammo_9mm", w_stat[WEAPON_MAC10].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_9MM;
            bought = true;
            }
         break;
      case WEAPON_TMP:
         if (pPlayer->m_iLicense >= LICENSE_SUB)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_9mm")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_9MM) 
            {
				pPlayer->GiveAmmo(w_stat[WEAPON_TMP].clip, "ammo_9mm", w_stat[WEAPON_TMP].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_9MM;
            bought = true;
            }
         break;
      case WEAPON_MP5NAVY:
         if (pPlayer->m_iLicense >= LICENSE_SUB)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_9mm")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_9MM) 
            {
				pPlayer->GiveAmmo(w_stat[WEAPON_MP5NAVY].clip, "ammo_9mm", w_stat[WEAPON_MP5NAVY].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_9MM;
            bought = true;
            }
         break;
      case WEAPON_UMP45:
         if (pPlayer->m_iLicense >= LICENSE_SUB)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_9mm")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_9MM) 
            {
				pPlayer->GiveAmmo(w_stat[WEAPON_UMP45].clip, "ammo_9mm", w_stat[WEAPON_UMP45].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_9MM;
            bought = true;
            }
         break;
      
      // Pepper bullet:
      case WEAPON_M3:
         if (pPlayer->m_iLicense >= LICENSE_SHOTGUN)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_buckshot")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_BUCKSHOT) 
            {
            pPlayer->GiveAmmo(w_stat[WEAPON_M3].clip, "ammo_buckshot", w_stat[WEAPON_M3].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_BUCKSHOT;
            bought = true;
            }
         break;
      case WEAPON_XM1014:
         if (pPlayer->m_iLicense >= LICENSE_SHOTGUN)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_buckshot")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_BUCKSHOT) 
            {
            pPlayer->GiveAmmo(w_stat[WEAPON_XM1014].clip, "ammo_buckshot", w_stat[WEAPON_XM1014].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_BUCKSHOT;
            bought = true;
            }
         break;

      // 5.56mm NATO:
      case WEAPON_M16:
         if (pPlayer->m_iLicense >= LICENSE_MRIFLE)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_556nato")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_556NATO) 
            {
            pPlayer->GiveAmmo(w_stat[WEAPON_M16].clip, "ammo_556nato", w_stat[WEAPON_M16].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_556NATO;
            bought = true;
            }
         break;
      case WEAPON_SG552:
         if (pPlayer->m_iLicense >= LICENSE_AUTO)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_556nato")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_556NATO) 
            {
            pPlayer->GiveAmmo(w_stat[WEAPON_SG552].clip, "ammo_556nato", w_stat[WEAPON_SG552].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_556NATO;
            bought = true;
            }
         break;

      // 7.62mm NATO:
      case WEAPON_AK47:
         if (pPlayer->m_iLicense >= LICENSE_MRIFLE)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_762nato")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_762NATO) 
            {
            pPlayer->GiveAmmo(w_stat[WEAPON_AK47].clip, "ammo_762nato", w_stat[WEAPON_AK47].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_762NATO;
            bought = true;
            }
         break;
      case WEAPON_SG550:
         if (pPlayer->m_iLicense >= LICENSE_SNIPER)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_762nato")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_762NATO) 
            {
            pPlayer->GiveAmmo(w_stat[WEAPON_SG550].clip, "ammo_762nato", w_stat[WEAPON_SG550].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_762NATO;
            bought = true;
            }
         break;
      case WEAPON_AUG:
         if (pPlayer->m_iLicense >= LICENSE_AUTO)
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_762nato")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_762NATO) 
            {
            pPlayer->GiveAmmo(w_stat[WEAPON_AUG].clip, "ammo_762nato", w_stat[WEAPON_AUG].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_762NATO;
            bought = true;
            }
         break;
      }
   if (bought)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgMoney, NULL, pPlayer->pev);
         WRITE_SHORT( pPlayer->m_iMoney );
      MESSAGE_END();
      EMIT_SOUND(ENT(pPlayer->pev), CHAN_WEAPON, "weapons/m3_insertshell.wav", 0.8, ATTN_NORM);
      return 1;
      }
   return 0;
}

int BuySecondaryAmmo(CBasePlayer *pPlayer)
{
   bool bought = 0;
   if (pPlayer->pev->deadflag > DEAD_NO) return -1;
   CBasePlayerItem *pItem = GetWeaponByWeight(pPlayer, WEIGHT_SECONDARY);
   // No secondary gun:
   if (!pItem) return 0;
   switch (pItem->m_iId)
      {
      case WEAPON_DEAGLE:
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_50ae")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_45ACP) 
            {
				pPlayer->GiveAmmo(w_stat[WEAPON_DEAGLE].clip, "ammo_50ae", w_stat[WEAPON_DEAGLE].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_45ACP;
            bought = true;
            }
         break;
      case WEAPON_GLOCK18:
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_9mm")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_9MM) 
            {
				pPlayer->GiveAmmo(w_stat[WEAPON_GLOCK18].clip, "ammo_9mm", w_stat[WEAPON_GLOCK18].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_9MM;
            bought = true;
            }
         break;
      case WEAPON_92D:
         if (pPlayer->m_rgAmmo[pPlayer->GetAmmoIndex("ammo_357sig")] < pItem->iMaxAmmo1())
			if (pPlayer->m_iMoney >= PRICE_AMMO_357SIG) 
            {
				pPlayer->GiveAmmo(w_stat[WEAPON_92D].clip, "ammo_357sig", w_stat[WEAPON_92D].max_carry);
				pPlayer->m_iMoney -= PRICE_AMMO_357SIG;
            bought = true;
            }
         break;
      }
   if (bought)
      {
      MESSAGE_BEGIN(MSG_ONE, gmsgMoney, NULL, pPlayer->pev);
         WRITE_SHORT( pPlayer->m_iMoney );
      MESSAGE_END();
      EMIT_SOUND(ENT(pPlayer->pev), CHAN_WEAPON, "weapons/m3_insertshell.wav", 0.8, ATTN_NORM);
      return 1;
      }
   return 0;
}

#endif