/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_matrix.cpp 
   This is the weapon matrix field definition header.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/


#ifndef WEAPON_MATRIX_H
#define WEAPON_MATRIX_H

#define WEAPON_KNIFE          31
#define WEAPON_HEGRENADE      30
#define WEAPON_92D            1
#define WEAPON_GLOCK18        2
#define WEAPON_DEAGLE         3
#define WEAPON_TMP            4
#define WEAPON_MAC10          5
#define WEAPON_UMP45          6
#define WEAPON_MP5NAVY        7
#define WEAPON_AK47           8
#define WEAPON_M16            9
#define WEAPON_SG552          10
#define WEAPON_AUG            11
#define WEAPON_SG550          12
#define WEAPON_M3             13
#define WEAPON_XM1014         14


#define MAX_WEAPONS  48

typedef struct
{
   unsigned char name[32];             // Name
   unsigned char id;                   // Id
   unsigned char clip;                 // Clip
   unsigned char max_carry;            // Maximum amount of bullets
   unsigned char penetration;          // Hull penetration [0-6]
   int   price;                        // Price
   float recoil_pitch;                 // Recoil pitch [up/down]
   float recoil_yaw;                   // Recoil yaw [left/right]
   float zoom1;                        // Zoom1 factor [FOV]
   float zoom2;                        // Zoom2 factor [FOV]
   float damage1;                      // Estimated primary damage
   float damage2;                      // Estimated secondary damage
   float rate;                         // Firing rate
   float rate2;                        // Firing rate2
   float speed;                        // Speed of weapon carrier
   float range;                        // Range of weapon
} weapon_stat;

typedef struct
   {
   unsigned char length;
   char data[0xFF];
   }  advertisement_t;

#define WEAPON_DEF_FILENAME "weapon_def.txt"
int LoadWeaponMatrix(char *filename);

extern weapon_stat w_stat[MAX_WEAPONS];

#endif
