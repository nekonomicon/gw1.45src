/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_global.h
   This is the global weapon definition header.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

#ifndef WEAPON_GLOBAL_H
#define WEAPON_GLOBAL_H

   #define M_SECONDARY     1
   #define M_RELOAD        2
   #define M_SILENCE       3
   #define M_ZOOM          4
   #define M_UPDATE        5

/* SMALL SHELL */
   #define SHELL_MODEL     "models/pshell.mdl"
   #define SHELL_MODEL2    "models/rshell.mdl"
   #define SHELL_MODEL3    "models/rshell_big.mdl"
   #define SHELL_MODEL4    "models/shotgunshell.mdl"

   #define IGNORE_DIM         dont_ignore_monsters
   #define DAMAGE_BNG         (DMG_BULLET | DMG_NEVERGIB)

/* Globals */
   // Weapon solid penetration:
   #define P_BODY_WIDTH   40     
   #define P_FENCE_WIDTH  24


/* MP5 */
#ifdef MP5_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum mp5_e
      {
	   A_MP5_IDLE = 0,
	   A_MP5_RELOAD,
      A_MP5_DRAW,
      A_MP5_SHOOT1,
      A_MP5_SHOOT2,
      A_MP5_SHOOT3
      };

   #ifndef I_AM_CLIENT
      #define MP5_WEAPON_V     "models/weapons/v_mp5_r.mdl"
      #define MP5_WEAPON_W     "models/weapons/w_mp5.mdl"
      #define MP5_WEAPON_P     "models/weapons/p_mp5.mdl"
      #define MP5_EV_SCRIPT    "events/mp5n.sc"
   #endif

   #define MP5_CLIPOUT     "weapons/mp5_clipout.wav"
   #define MP5_CLIPIN      "weapons/mp5_clipin.wav"
   #define MP5_SLIDEBACK   "weapons/mp5_slideback.wav"
   #define MP5_SHOOT1      "weapons/mp5-1.wav"
   //#define MP5_SHOOT2      "weapons/mp5-2.wav"
   #define MP5_EMPTY       "weapons/dryfire_rifle.wav"
   #define MP5_RELOAD_TIME    2.7f
   
   /*
   #define MP5_DAMAGE         12
   #define MP5_ATTACK1_TIME   0.08f

   #define MP5_SHAKE_LR       0.9f        
   #define MP5_SHAKE_UD       1.2f
   */

   #define MP5_ATTN           ATTN_NORM
   #define MP5_VOLUME         1.0f

   #define MP5_SLOT           0
   #define MP5_POSITION       3
#endif


/* TMP */
#ifdef TMP_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum tmp_e
      {
	   A_TMP_IDLE = 0,
	   A_TMP_RELOAD,
      A_TMP_DRAW,
      A_TMP_SHOOT1,
      A_TMP_SHOOT2,
      A_TMP_SHOOT3
      };

   #ifndef I_AM_CLIENT
      #define TMP_WEAPON_V     "models/weapons/v_tmp_r.mdl"
      #define TMP_WEAPON_W     "models/weapons/w_tmp.mdl"
      #define TMP_WEAPON_P     "models/weapons/p_tmp.mdl"
      #define TMP_EV_SCRIPT    "events/tmp.sc"
   #endif

   #define TMP_CLIPOUT   "weapons/clipout1.wav"
   #define TMP_CLIPIN    "weapons/clipin1.wav"
   #define TMP_SHOOT1    "weapons/tmp-1.wav"
   #define TMP_SHOOT2    "weapons/tmp-2.wav"
   #define TMP_EMPTY     "weapons/dryfire_rifle.wav"
   #define TMP_RELOAD_TIME    2.2f

   /*
   #define TMP_DAMAGE         8
   #define TMP_ATTACK1_TIME   0.06f

   #define TMP_SHAKE_LR       0.6f
   #define TMP_SHAKE_UD       0.6f
   */

   #define TMP_ATTN           ATTN_STATIC
   #define TMP_VOLUME         0.6f

   #define TMP_SLOT           0
   #define TMP_POSITION       2
#endif


/* MAC10 */
#ifdef MAC10_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum mac10_e
      {
	   A_MAC10_IDLE = 0,
	   A_MAC10_RELOAD,
      A_MAC10_DRAW,
      A_MAC10_SHOOT1,
      A_MAC10_SHOOT2,
      A_MAC10_SHOOT3
      };

   #ifndef I_AM_CLIENT
      #define MAC10_WEAPON_V     "models/weapons/v_mac10_r.mdl"
      #define MAC10_WEAPON_W     "models/weapons/w_mac10.mdl"
      #define MAC10_WEAPON_P     "models/weapons/p_mac10.mdl"
      #define MAC10_EV_SCRIPT    "events/mac10.sc"
   #endif

   #define MAC10_CLIPOUT   "weapons/mac10_clipout.wav"
   #define MAC10_CLIPIN    "weapons/mac10_clipin.wav"
   #define MAC10_BOLTPULL  "weapons/mac10_boltpull.wav"
   #define MAC10_SHOOT1    "weapons/mac10-1.wav"
   #define MAC10_EMPTY     "weapons/dryfire_rifle.wav"
   #define MAC10_RELOAD_TIME  3.2f
   
   /*
   #define MAC10_DAMAGE       12
   #define MAC10_ATTACK1_TIME 0.10f

   #define MAC10_SHAKE_LR     1.1f
   #define MAC10_SHAKE_UD     1.3f
   */

   #define MAC10_ATTN         ATTN_NORM
   #define MAC10_VOLUME       1.0f

   #define MAC10_SLOT            0
   #define MAC10_POSITION        1
#endif

   /* UMP45 */
#ifdef UMP45_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum ump45_e
      {
	   A_UMP45_IDLE = 0,
	   A_UMP45_RELOAD,
      A_UMP45_DRAW,
      A_UMP45_SHOOT1,
      A_UMP45_SHOOT2,
      A_UMP45_SHOOT3
      };

   #ifndef I_AM_CLIENT
      #define UMP45_WEAPON_V     "models/weapons/v_ump45_r.mdl"
      #define UMP45_WEAPON_W     "models/weapons/w_ump45.mdl"
      #define UMP45_WEAPON_P     "models/weapons/p_ump45.mdl"
      #define UMP45_EV_SCRIPT    "events/ump45.sc"
   #endif

   #define UMP45_CLIPOUT   "weapons/ump45_clipout.wav"
   #define UMP45_CLIPIN    "weapons/ump45_clipin.wav"
   #define UMP45_BOLTSLAP  "weapons/ump45_boltslap.wav"
   #define UMP45_SHOOT1    "weapons/ump45-1.wav"
   #define UMP45_EMPTY     "weapons/dryfire_rifle.wav"
   #define UMP45_RELOAD_TIME  3.5f
   
   /*
   #define UMP45_DAMAGE       15
   #define UMP45_ATTACK1_TIME 0.09f

   #define UMP45_SHAKE_LR     1.2f
   #define UMP45_SHAKE_UD     1.5f
   */

   #define UMP45_ATTN         ATTN_NORM
   #define UMP45_VOLUME       1.0f

   #define UMP45_SLOT         0
   #define UMP45_POSITION     4
#endif

/* FNP90 */
#ifdef FNP90_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum fnp90_e
      {
	   A_FNP90_IDLE = 0,
	   A_FNP90_RELOAD,
      A_FNP90_DRAW,
      A_FNP90_SHOOT1,
      A_FNP90_SHOOT2,
      A_FNP90_SHOOT3
      };

   #ifndef I_AM_CLIENT
      #define P90_WEAPON_V     "models/weapons/v_p90_r.mdl"
      #define P90_WEAPON_W     "models/weapons/w_p90.mdl"
      #define P90_WEAPON_P     "models/weapons/p_p90.mdl"
      #define P90_EV_SCRIPT    "events/p90.sc"
   #endif

   #define FNP90_CLIPOUT      "weapons/p90_clipout.wav"
   #define FNP90_CLIPIN       "weapons/p90_clipin.wav"
   #define FNP90_BOLTPULL     "weapons/p90_boltpull.wav"
   #define FNP90_CLIPRELEASE  "weapons/p90_cliprelease.wav"
   #define FNP90_SHOOT1       "weapons/p90-1.wav"
   #define FNP90_EMPTY        "weapons/dryfire_rifle.wav"
   #define FNP90_RELOAD_TIME  3.6f
   
   /*
   #define FNP90_DAMAGE       15
   #define FNP90_ATTACK1_TIME 0.07f

   #define FNP90_SHAKE_LR     0.7f
   #define FNP90_SHAKE_UD     0.9f
   */

   #define FNP90_ATTN         ATTN_NORM
   #define FNP90_VOLUME       0.8f

   #define FNP90_SLOT            0
   #define FNP90_POSITION        5
#endif

/* AK47 */
#ifdef AK47_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum ak47_e
      {
	   A_AK47_IDLE = 0,
	   A_AK47_RELOAD,
      A_AK47_DRAW,
      A_AK47_SHOOT1,
      A_AK47_SHOOT2,
      A_AK47_SHOOT3
      };

   #ifndef I_AM_CLIENT
      #define AK47_WEAPON_V     "models/weapons/v_ak47_r.mdl"
      #define AK47_WEAPON_W     "models/weapons/w_ak47.mdl"
      #define AK47_WEAPON_P     "models/weapons/p_ak47.mdl"
      #define AK47_EV_SCRIPT    "events/ak47.sc"
   #endif

   #define AK47_BOLTPULL   "weapons/ak47_boltpull.wav"
   #define AK47_CLIPIN     "weapons/ak47_clipin.wav"
   #define AK47_CLIPOUT    "weapons/ak47_clipout.wav"
   #define AK47_SHOOT1     "weapons/ak47-1.wav"
   #define AK47_SHOOT2     "weapons/ak47-2.wav"
   #define AK47_EMPTY      "weapons/dryfire_rifle.wav"
   #define AK47_RELOAD_TIME   2.3f

   /*
   #define AK47_DAMAGE1       36
   #define AK47_DAMAGE2       16
   #define AK47_ATTACK1_TIME  0.1f

   #define AK47_SHAKE_LR      1.3f
   #define AK47_SHAKE_UD      1.5f
   */

   #define AK47_ATTN          ATTN_NORM
   #define AK47_VOLUME        1.0f

   #define AK47_SLOT          0
   #define AK47_POSITION      6
#endif


/* SG552 */
#ifdef SG552_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum sg552_e
      {
	   A_SG552_IDLE = 0,
	   A_SG552_RELOAD,
      A_SG552_DRAW,
      A_SG552_SHOOT1,
      A_SG552_SHOOT2,
      A_SG552_SHOOT3
      };

   #ifndef I_AM_CLIENT
      #define SG552_WEAPON_V     "models/weapons/v_sg552_r.mdl"
      #define SG552_WEAPON_W     "models/weapons/w_sg552.mdl"
      #define SG552_WEAPON_P     "models/weapons/p_sg552.mdl"
      #define SG552_EV_SCRIPT    "events/sg552.sc"
   #endif

   #define SG552_BOLTPULL  "weapons/sg552_boltpull.wav"
   #define SG552_CLIPIN    "weapons/sg552_clipin.wav"
   #define SG552_CLIPOUT   "weapons/sg552_clipout.wav"
   #define SG552_SHOOT1    "weapons/sg552-1.wav"
   #define SG552_SHOOT2    "weapons/sg552-2.wav"
   #define SG552_ZOOM      "weapons/zoom.wav"
   #define SG552_EMPTY     "weapons/dryfire_rifle.wav"
   #define SG552_RELOAD_TIME  2.3f
  
   /*
   #define SG552_DAMAGE1      28
   #define SG552_DAMAGE2      16
   #define SG552_ATTACK1_TIME 0.1f
   #define SG552_ATTACK2_TIME 0.15f

   #define SG552_SHAKE_LR     1.3f
   #define SG552_SHAKE_UD     1.5f
   #define SG552_SHAKE_LR1    0.7f
   #define SG552_SHAKE_UD1    0.7f
   */

   #define SG552_ATTN         ATTN_NORM
   #define SG552_VOLUME       1.0f

   #define SG552_SLOT         0
   #define SG552_POSITION     8
#endif

/* AUG */
#ifdef AUG_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum aug_e
      {
	   A_AUG_IDLE = 0,
	   A_AUG_RELOAD,
      A_AUG_DRAW,
      A_AUG_SHOOT1,
      A_AUG_SHOOT2,
      A_AUG_SHOOT3,
      A_AUG_SLAP
      };

   #ifndef I_AM_CLIENT
      #define AUG_WEAPON_V     "models/weapons/v_aug_r.mdl"
      #define AUG_WEAPON_W     "models/weapons/w_aug.mdl"
      #define AUG_WEAPON_P     "models/weapons/p_aug.mdl"
      #define AUG_EV_SCRIPT    "events/aug.sc"
   #endif

   #define AUG_BOLTPULL    "weapons/aug_boltpull.wav"
   #define AUG_BOLTSLAP    "weapons/aug_boltslap.wav"
   #define AUG_CLIPIN      "weapons/aug_clipin.wav"
   #define AUG_CLIPOUT     "weapons/aug_clipout.wav"
   #define AUG_FOREARM     "weapons/aug_forearm.wav"
   #define AUG_SHOOT1      "weapons/aug-1.wav"
   #define AUG_ZOOM        "weapons/zoom.wav"
   #define AUG_EMPTY       "weapons/dryfire_rifle.wav"
   #define AUG_RELOAD_TIME    3.6f
   
   /*
   #define AUG_DAMAGE1        40
   #define AUG_DAMAGE2        24
   #define AUG_ATTACK1_TIME   0.1f
   #define AUG_ATTACK2_TIME   0.15f

   #define AUG_SHAKE_LR       1.3f
   #define AUG_SHAKE_UD       1.5f
   #define AUG_SHAKE_LR1      0.7f
   #define AUG_SHAKE_UD1      0.7f
   */

   #define AUG_ATTN           ATTN_NORM
   #define AUG_VOLUME         1.0f

   #define AUG_SLOT           0
   #define AUG_POSITION       9
#endif

#ifdef M3_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum m3_e
      {
	      A_M3_IDLE = 0,
         A_M3_SHOOT1,
         A_M3_SHOOT2,
         A_M3_INSERT,
         A_M3_RELOAD2,
         A_M3_RELOAD1,
         A_M3_DRAW
      };

   #ifndef I_AM_CLIENT
      #define M3_WEAPON_P     "models/weapons/p_m3.mdl"
      #define M3_WEAPON_V     "models/weapons/v_m3_r.mdl"
      #define M3_WEAPON_W     "models/weapons/w_m3.mdl"
      #define M3_EV_SCRIPT    "events/m3.sc"
   #endif

   #define M3_RELOAD       "weapons/m3_insertshell.wav"
   #define M3_DEPLOY       "weapons/m3_pump.wav"
   #define M3_SHOOT1       "weapons/m3-1.wav"
   #define M3_EMPTY        "weapons/dryfire_rifle.wav"
   #define M3_RELOAD_TIME     24.0f

   /*
   #define M3_DAMAGE1         22
   #define M3_DAMAGE2         16
   #define M3_ATTACK1_TIME    1.0f

   #define M3_SHAKE_LR        1.5f
   #define M3_SHAKE_UD        1.7f
   */

   #define M3_ATTN            ATTN_NORM
   #define M3_VOLUME          1.0f

   #define M3_SLOT            0
   #define M3_POSITION        10
#endif

#ifdef XM1014_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum xm1014_e
      {
	   A_XM1014_IDLE = 0,
      A_XM1014_SHOOT1,
      A_XM1014_SHOOT2,
      A_XM1014_INSERT,
      A_XM1014_RELOAD2,
      A_XM1014_RELOAD1,
      A_XM1014_DRAW
      };

   #ifndef I_AM_CLIENT
      #define XM1014_WEAPON_P "models/weapons/p_xm1014.mdl"
      #define XM1014_WEAPON_V "models/weapons/v_xm1014_r.mdl"
      #define XM1014_WEAPON_W "models/weapons/w_xm1014.mdl"
      #define XM1014_EV_SCRIPT    "events/xm1014.sc"
   #endif

   #define XM1014_RELOAD   "weapons/m3_insertshell.wav"
   #define XM1014_DEPLOY   "weapons/m3_pump.wav"
   #define XM1014_SHOOT1   "weapons/xm1014-1.wav"
   #define XM1014_EMPTY    "weapons/dryfire_rifle.wav"
   #define XM1014_RELOAD_TIME     24.0f

   /*
   #define XM1014_DAMAGE1         18
   #define XM1014_DAMAGE2         14
   #define XM1014_ATTACK1_TIME    0.3f

   #define XM1014_SHAKE_LR        1.5f
   #define XM1014_SHAKE_UD        1.7f
   */

   #define XM1014_ATTN            ATTN_NORM
   #define XM1014_VOLUME          1.0f

   #define XM1014_SLOT           0
   #define XM1014_POSITION       11
#endif

#ifdef SG550_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT

   enum sg550_e 
      {
      A_SG550_IDLE1 = 0,
      A_SG550_SHOOT1,
      A_SG550_SHOOT2,
      A_SG550_RELOAD,
      A_SG550_DRAW
      };
   #ifndef I_AM_CLIENT
      #define SG550_WEAPON_V "models/weapons/v_sg550_r.mdl"
      #define SG550_WEAPON_W "models/weapons/w_sg550.mdl"
      #define SG550_WEAPON_P "models/weapons/p_sg550.mdl"
      #define SG550_EV_SCRIPT    "events/sg550.sc"
   #endif

   #define SG550_CLIPIN   "weapons/sg550_clipin.wav"
   #define SG550_CLIPOUT  "weapons/sg550_clipout.wav"
   #define SG550_DEPLOY   "weapons/sg550_boltpull.wav"
   #define SG550_ZOOM     "weapons/zoom.wav"
   #define SG550_SHOOT1   "weapons/sg550-1.wav"
   #define SG550_EMPTY    "weapons/dryfire_rifle.wav"
   #define SG550_RELOAD_TIME     4.6f

   /*
   #define SG550_DAMAGE1         70
   #define SG550_DAMAGE2         56
   #define SG550_ATTACK1_TIME    0.4f
   #define SG550_ATTACK12_TIME   0.6f

   #define SG550_SHAKE_LR        0.7f
   #define SG550_SHAKE_UD        0.5f
   #define SG550_SHAKE_LR1       0.3f
   #define SG550_SHAKE_UD1       0.2f
   */

   #define SG550_ATTN            ATTN_NORM
   #define SG550_VOLUME          0.8f

   #define SG550_SLOT            0
   #define SG550_POSITION        14
#endif

#ifdef DEAGLE_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT

   enum deagle_e
      {
	   A_DEAGLE_IDLE = 0,
      A_DEAGLE_SHOOT1,
      A_DEAGLE_SHOOT2,
      A_DEAGLE_SHOOT_EMPTY,
	   A_DEAGLE_RELOAD,
      A_DEAGLE_DRAW
      };
   #ifndef I_AM_CLIENT
      #define DEAGLE_WEAPON_P        "models/weapons/p_magnum.mdl"
      #define DEAGLE_WEAPON_V        "models/weapons/v_magnum_r.mdl"
      #define DEAGLE_WEAPON_W        "models/weapons/w_magnum.mdl"
      #define DEAGLE_EV_SCRIPT       "events/magnum.sc"
   #endif

   #define DEAGLE_CLIPIN   "weapons/de_clipin.wav"
   #define DEAGLE_CLIPOUT  "weapons/de_clipout.wav"
   #define DEAGLE_DEPLOY   "weapons/de_deploy.wav"
   #define DEAGLE_SHOOT1   "weapons/deagle-1.wav"
   #define DEAGLE_SHOOT2   "weapons/deagle-2.wav"
   #define DEAGLE_EMPTY    "weapons/dryfire_pistol.wav"
   #define DEAGLE_RELOAD_TIME    2.0f
   
   /*
   #define DEAGLE_DAMAGE1        30
   #define DEAGLE_DAMAGE2        16
   #define DEAGLE_ATTACK1_TIME   0.14f

   #define DEAGLE_SHAKE_LR       1.8f
   #define DEAGLE_SHAKE_UD       1.4f
   */

   #define DEAGLE_ATTN           ATTN_NORM
   #define DEAGLE_VOLUME         1.0f

   #define DEAGLE_SLOT           1
   #define DEAGLE_POSITION       0
#endif

#ifdef GLOCK_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT

   enum glock18_e
      {
	   A_GLOCK_IDLE1 = 0,
	   A_GLOCK_IDLE2,
	   A_GLOCK_IDLE3,
      A_GLOCK_SHOOT1,
      A_GLOCK_SHOOT2,
      A_GLOCK_SHOOT3,
      A_GLOCK_SHOOT_EMPTY,
	   A_GLOCK_RELOAD1,
      A_GLOCK_DRAW1,
      A_GLOCK_HOLSTER,
      A_GLOCK_SILENCER,
      A_GLOCK_DRAW2,
      A_GLOCK_RELOAD2
      };
   #ifndef I_AM_CLIENT
      #define GLOCK_WEAPON_P        "models/weapons/p_b93r.mdl"
      #define GLOCK_WEAPON_V        "models/weapons/v_b93r_r.mdl"
      #define GLOCK_WEAPON_W        "models/weapons/w_b93r.mdl"
      #define GLOCK_EV_SCRIPT       "events/b93r.sc"
   #endif

   #define GLOCK_CLIPIN       "weapons/clipin1.wav"
   #define GLOCK_CLIPOUT      "weapons/clipout1.wav"
   #define GLOCK_SLIDEBACK    "weapons/boltpull1.wav"
   #define GLOCK_SHOOT1       "weapons/glock18-2.wav"
   #define GLOCK_EMPTY        "weapons/dryfire_pistol.wav"
   #define GLOCK_RELOAD_TIME    2.0f
   
   /*
   #define GLOCK_DAMAGE1        13
   #define GLOCK_ATTACK1_TIME   0.12f

   #define GLOCK_SHAKE_LR       0.5f
   #define GLOCK_SHAKE_UD       0.4f
   */

   #define GLOCK_ATTN           ATTN_NORM
   #define GLOCK_VOLUME         1.0f

   #define GLOCK_SLOT           1
   #define GLOCK_POSITION       2
#endif

#ifdef B92D_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT

   enum b92d_e
      {
	   A_B92D_IDLE = 0,
      A_B92D_SHOOT1,
      A_B92D_SHOOT2,
      A_B92D_SHOOT3,
      A_B92D_SHOOT_EMPTY,
	   A_B92D_RELOAD,
      A_B92D_DRAW
      };
   #ifndef I_AM_CLIENT
      #define B92D_WEAPON_P        "models/weapons/p_92f.mdl"
      #define B92D_WEAPON_V        "models/weapons/v_92f_r.mdl"
      #define B92D_WEAPON_W        "models/weapons/w_92f.mdl"
      #define B92D_EV_SCRIPT       "events/92f.sc"
   #endif

   #define B92D_CLIPIN        "weapons/p228_clipin.wav"
   #define B92D_CLIPOUT       "weapons/p228_clipout.wav"
   #define B92D_SLIDEBACK     "weapons/p228_slidepull.wav"
   #define B92D_SLIDERELEASE  "weapons/p228_sliderelease.wav"
   #define B92D_SHOOT1        "weapons/92d-1.wav"
   #define B92D_EMPTY         "weapons/dryfire_pistol.wav"
   #define B92D_RELOAD_TIME   2.7f
   
   /*
   #define B92D_DAMAGE1       20
   #define B92D_DAMAGE2       10
   #define B92D_ATTACK1_TIME  0.12f

   #define B92D_SHAKE_LR      1.0f
   #define B92D_SHAKE_UD      0.6f
   */

   #define B92D_ATTN          ATTN_NORM
   #define B92D_VOLUME        1.0f

   #define B92D_SLOT          1
   #define B92D_POSITION      3
#endif

/* M16 */
#ifdef M16_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT
   enum m16_e
      {
	   A_M16_OIDLE = 0,
      A_M16_OSHOOT1,
      A_M16_OSHOOT2,
      A_M16_OSHOOT3,
	   A_M16_ORELOAD,
      A_M16_ODRAW,
      A_M16_OADDSILENCER,
      A_M16_IDLE,
      A_M16_SHOOT1,
      A_M16_SHOOT2,
      A_M16_SHOOT3,
      A_M16_RELOAD,
      A_M16_DRAW,
      A_M16_OREMOVESILENCER
      };

   #ifndef I_AM_CLIENT
      #define M16_WEAPON_V     "models/weapons/v_m16_r.mdl"
      #define M16_WEAPON_W     "models/weapons/w_m16.mdl"
      #define M16_WEAPON_P     "models/weapons/p_m16.mdl"
      #define M16_EV_SCRIPT    "events/m16.sc"
   #endif

   #define M16_BOLTPULL      "weapons/m4a1_boltpull.wav"
   #define M16_CLIPIN        "weapons/m4a1_clipin.wav"
   #define M16_CLIPOUT       "weapons/m4a1_clipout.wav"
   #define M16_DEPLOY        "weapons/m4a1_deploy.wav"
   #define M16_SHOOT1        "weapons/m16_unsil-1.wav"
   #define M16_SHOOT2        "weapons/m16_unsil-2.wav"
   #define M16_EMPTY         "weapons/dryfire_rifle.wav"
   #define M16_RELOAD_TIME   3.1f
   
   /*
   #define M16_DAMAGE1       28
   #define M16_DAMAGE2       16
   #define M16_DAMAGE3       20
   #define M16_DAMAGE4       12
   #define M16_ATTACK1_TIME  0.09f

   #define M16_SHAKE_LR      0.7f
   #define M16_SHAKE_UD      0.7f
   */

   #define M16_ATTN          ATTN_NORM
   #define M16_VOLUME        1.0f

   #define M16_SLOT          0
   #define M16_POSITION      22
#endif

#ifdef HEGRENADE_MODULE
   #undef WEAPON_V
   #undef WEAPON_W
   #undef WEAPON_P
   #undef EV_SCRIPT

   enum hegrenade_e 
      {
	   A_HEGRENADE_IDLE = 0,
	   A_HEGRENADE_PINPULL,
	   A_HEGRENADE_THROW,
	   A_HEGRENADE_DRAW
      };

   #ifndef I_AM_CLIENT
      #define HE_WEAPON_P        "models/weapons/p_hegrenade.mdl"
      #define HE_WEAPON_V        "models/weapons/v_hegrenade_r.mdl"
      #define HE_WEAPON_W        "models/weapons/w_hegrenade.mdl"
   #endif

   #define HEGRENADE_SHOOT1   "weapons/hegrenade-1.wav"
   #define HEGRENADE_SHOOT2   "weapons/hegrenade-2.wav"
   #define HEGRENADE_BOUNCE   "weapons/he_bounce-1.wav"
   #define HEGRENADE_PINPULL  "weapons/pinpull.wav"
   
   #define HEGRENADE_DAMAGE1        130
   #define HEGRENADE_DAMAGE2        56
   #define HEGRENADE_RELOAD_TIME    2.0f
   #define HEGRENADE_ATTACK1_TIME   0.5f

   #define HEGRENADE_ATTN           ATTN_NORM
   #define HEGRENADE_VOLUME         1.0f

   #define HEGRENADE_SLOT           3
   #define HEGRENADE_POSITION       0
#endif


#ifndef CLIENT_DLL 
#include "player.h"
inline FireSharpBullet(CBasePlayer *m_pPlayer, entvars_t *pev, const int weapon_enum)
{
   TraceResult tr;  
   int retraced = 0;
   Vector anglesAim, vecDir, vecSrc, vecEnd, vecEnd2;  
   int throughwall = 0;

   if (pev->flags & FL_ONGROUND)
      {
      if (m_pPlayer->pev->flags & FL_DUCKING)
         {
         m_pPlayer->pev->punchangle.x += (.7f * -RANDOM_FLOAT(0, w_stat[weapon_enum].recoil_pitch));
         m_pPlayer->pev->punchangle.y += (.7f * RANDOM_FLOAT(-w_stat[weapon_enum].recoil_yaw, w_stat[weapon_enum].recoil_yaw));
	      }
      else
         {
         m_pPlayer->pev->punchangle.x += -RANDOM_FLOAT(0, w_stat[weapon_enum].recoil_pitch);
         m_pPlayer->pev->punchangle.y += RANDOM_FLOAT(-w_stat[weapon_enum].recoil_yaw, w_stat[weapon_enum].recoil_yaw);
         }
      }
   else
      {
      m_pPlayer->pev->punchangle.x += (1.3 * -RANDOM_FLOAT(0, w_stat[weapon_enum].recoil_pitch));
      m_pPlayer->pev->punchangle.y += (1.3 * RANDOM_FLOAT(-w_stat[weapon_enum].recoil_yaw, w_stat[weapon_enum].recoil_yaw));
      }

   m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );

   vecSrc = m_pPlayer->GetGunPosition() - gpGlobals->v_up * 2;
	vecDir = gpGlobals->v_forward;
   vecEnd = vecSrc + vecDir * w_stat[weapon_enum].range;
   vecEnd2 = vecSrc;

retrace:
   UTIL_TraceLine(vecSrc, vecEnd, IGNORE_DIM, m_pPlayer->edict(), &tr);
   if ( tr.flFraction != 1.0)
      {
	   if (!FNullEnt( tr.pHit) && tr.pHit->v.takedamage)
	      {
		   ClearMultiDamage( );
         if (!throughwall)
            {
            CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
            if (pHit)
               {
               pHit->TraceAttack
               (m_pPlayer->pev, w_stat[weapon_enum].damage1, vecDir, &tr, DAMAGE_BNG); 
		         ApplyMultiDamage(pev, m_pPlayer->pev);
               }
            vecSrc = tr.vecEndPos + vecDir * P_BODY_WIDTH;
            if (UTIL_PointContents(vecSrc) != CONTENT_SOLID)
               {
               vecEnd2 = vecSrc;
               throughwall++;
               goto retrace;
               }
            }
         else
            {
            CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
            if (pHit)
               {
               pHit->TraceAttack
               (m_pPlayer->pev, w_stat[weapon_enum].damage2, vecDir, &tr, DAMAGE_BNG); 
		         ApplyMultiDamage(pev, m_pPlayer->pev);
               }
            }
         }
	   else
	      {           
         const char *pTextureName;
         CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
         if (!pEntity)
            {
            UTIL_LogPrintf("Crashed @ 005\n");
            }
         else
            {
		      if (retraced < 4)
               {
			      pTextureName = TRACE_TEXTURE( ENT(pEntity->pev), vecSrc, vecEnd );
               if (pTextureName)
			         if (*pTextureName == '{' || *pTextureName == '!')
                     {
                     retraced++;
        	            vecSrc = tr.vecEndPos + vecDir * P_FENCE_WIDTH;
                     if (UTIL_PointContents(vecSrc) != CONTENT_SOLID)
                        goto retrace;
                     }
               }
            if (!throughwall)
               {
               vecSrc = tr.vecEndPos + vecDir * w_stat[weapon_enum].penetration;
               if (UTIL_PointContents(vecSrc) != CONTENT_SOLID)
                  {
                  throughwall++;
                  goto retrace;
                  }  
               }
            }
         }
      }
   if (throughwall > 0)
      {
      throughwall = -1;
      vecEnd = vecEnd2;
      goto retrace;      
      }
}

inline FireFlatBullet(CBasePlayer *m_pPlayer, entvars_t *pev, const int weapon_enum)
{
   TraceResult tr;
   int retraced = 0;
   Vector anglesAim, vecDir, vecSrc, vecEnd;

   if (pev->flags & FL_ONGROUND)
      {
      if (m_pPlayer->pev->flags & FL_DUCKING)
         {
         m_pPlayer->pev->punchangle.x += (.7f * -RANDOM_FLOAT(0, w_stat[weapon_enum].recoil_pitch));
         m_pPlayer->pev->punchangle.y += (.7f * RANDOM_FLOAT(-w_stat[weapon_enum].recoil_yaw, w_stat[weapon_enum].recoil_yaw));
	      }
      else
         {
         m_pPlayer->pev->punchangle.x += -RANDOM_FLOAT(0, w_stat[weapon_enum].recoil_pitch);
         m_pPlayer->pev->punchangle.y += RANDOM_FLOAT(-w_stat[weapon_enum].recoil_yaw, w_stat[weapon_enum].recoil_yaw);
         }
      }
   else
      {
      m_pPlayer->pev->punchangle.x += (1.3 * -RANDOM_FLOAT(0, w_stat[weapon_enum].recoil_pitch));
      m_pPlayer->pev->punchangle.y += (1.3 * RANDOM_FLOAT(-w_stat[weapon_enum].recoil_yaw, w_stat[weapon_enum].recoil_yaw));
      }

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );
	vecSrc = m_pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
	vecDir = gpGlobals->v_forward;
   vecEnd = vecSrc + vecDir * w_stat[weapon_enum].range;

retrace:
   UTIL_TraceLine(vecSrc, vecEnd, IGNORE_DIM, m_pPlayer->edict(), &tr);
   if ( tr.flFraction != 1.0 && !FNullEnt( tr.pHit) )      
      {
	   if (tr.pHit->v.takedamage)
	      {
		   ClearMultiDamage( );
         CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
         if (pEntity) 
            {
            pEntity->TraceAttack(m_pPlayer->pev, w_stat[weapon_enum].damage1, vecDir, &tr, DAMAGE_BNG); 
            ApplyMultiDamage( pev, m_pPlayer->pev );
            }
         else UTIL_LogPrintf("Crashed @ 006\n");		   
         }
      else
         {
         const char *pTextureName;
         CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		   if (pEntity && retraced < 4)
            {
			   pTextureName = TRACE_TEXTURE( ENT(pEntity->pev), vecSrc, vecEnd );
            if (pTextureName)
			      if (*pTextureName == '{' || *pTextureName == '!')
                  {
                  retraced++;
        	         vecSrc = tr.vecEndPos + vecDir * P_FENCE_WIDTH;
                  if (UTIL_PointContents(vecSrc) != CONTENT_SOLID)
                     goto retrace;
                  }
            }
         }
      }
}

inline FireStrongBullet(CBasePlayer *m_pPlayer, entvars_t *pev, const int weapon_enum, int m_fInZoom)
{
   TraceResult tr;      
   Vector anglesAim, vecDir, vecSrc, vecEnd, vecEnd2;  
   int retraced = 0;
   int throughwall = 0;
   float pitch, yaw;
   pitch = -RANDOM_FLOAT(0, w_stat[weapon_enum].recoil_pitch);
   yaw = RANDOM_FLOAT(-w_stat[weapon_enum].recoil_yaw, w_stat[weapon_enum].recoil_yaw);
   if (pev->flags & FL_ONGROUND)
      {
      if (m_pPlayer->pev->flags & FL_DUCKING)
         {
         pitch *= .4f;
         yaw *= .4f;
	      }
      }
   else
      {
      pitch *= 1.3;
      yaw *= 1.3;
      }

   if (m_fInZoom == 1)
      {
      pitch *= .5f; yaw *= .5f;     
      }
   else if (m_fInZoom == 2)
      {
      pitch *= .2f; yaw *= .2f;     
      }
   else
      {
      pitch *= 2.0f; yaw *= 2.0f;
      }

   m_pPlayer->pev->punchangle.x += pitch;
   m_pPlayer->pev->punchangle.y += yaw;
   m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );

   vecSrc = m_pPlayer->GetGunPosition() - gpGlobals->v_up * 2;
	vecDir = gpGlobals->v_forward;
   vecEnd = vecSrc + vecDir * w_stat[weapon_enum].range;
   vecEnd2 = vecSrc;

retrace:
   UTIL_TraceLine(vecSrc, vecEnd, IGNORE_DIM, m_pPlayer->edict(), &tr);
   if ( tr.flFraction != 1.0)
      {
	   if (!FNullEnt( tr.pHit) && tr.pHit->v.takedamage)
	      {
		   ClearMultiDamage( );
         if (!throughwall)
            {
            CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
            if (pHit)
               {
               pHit->TraceAttack
               (m_pPlayer->pev, w_stat[weapon_enum].damage1, vecDir, &tr, DAMAGE_BNG); 
		         ApplyMultiDamage(pev, m_pPlayer->pev);
               }
            vecSrc = tr.vecEndPos + vecDir * P_BODY_WIDTH;
            if (UTIL_PointContents(vecSrc) != CONTENT_SOLID)
               {
               vecEnd2 = vecSrc;
               throughwall++;
               goto retrace;
               }
            }
         else
            {
            CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
            if (pHit)
               {
               pHit->TraceAttack
               (m_pPlayer->pev, w_stat[weapon_enum].damage2, vecDir, &tr, DAMAGE_BNG); 
		         ApplyMultiDamage(pev, m_pPlayer->pev);
               }
            }
         }
	   else
	      {
         const char *pTextureName;
         CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		   if (pEntity && retraced < 4)
            {
			   pTextureName = TRACE_TEXTURE( ENT(pEntity->pev), vecSrc, vecEnd );
            if (pTextureName)
			      if (*pTextureName == '{' || *pTextureName == '!')
                  {
                  retraced++;
        	         vecSrc = tr.vecEndPos + vecDir * P_FENCE_WIDTH;
                  if (UTIL_PointContents(vecSrc) != CONTENT_SOLID)
                     goto retrace;
                  }
            }
         if (!throughwall)
            {
            vecSrc = tr.vecEndPos + vecDir * w_stat[weapon_enum].penetration;
            if (UTIL_PointContents(vecSrc) != CONTENT_SOLID)
               {
               throughwall++;
               goto retrace;
               }  
            }
         }
      }
   if (throughwall > 0)
      {
      throughwall = -1;
      vecEnd = vecEnd2;
      goto retrace;      
      }
}

#endif
#endif