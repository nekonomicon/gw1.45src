/*
   (c) 2002 Gangwars Network. All rights reserved.
   Copy and modification of this file is permitted under the EULA supplemented
   with this package.

   FILENAME: sdk_w_classes.h 
   This is the weapon class definition header.

   Written by Nawfel Tricha
   Topaz@gangwars.net, Topaz@e-toxin.com
*/

/* Turn LOCAL_WEAPON only when compiling client code! */

#include "whichdll.h"
#ifdef CLIENT_DLL
   #define LOCAL_WEAPON
#endif


void	HUD_PlaybackEvent( int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 );

#ifndef WEAPON_CLASSES_H
#define WEAPON_CLASSES_H

   #ifdef MP5_MODULE 
      class MP5 : public CBasePlayerWeapon
      {
      public:
	      void Spawn( void );
	      void Precache( void );
	      int iItemSlot( void ) { return 3; }
	      int GetItemInfo(ItemInfo *p);
	      int AddToPlayer( CBasePlayer *pPlayer );
         BOOL Deploy( void );

	      void PrimaryAttack( void );
         #ifndef LOCAL_WEAPON   
	         void Reload( void );
	         void WeaponIdle( void );
         #endif
	      virtual BOOL UseDecrement( void )
	         { 
            #ifdef CLIENT_WEAPONS 
                  return TRUE;
            #else return FALSE;
            #endif
	         }
      private:
         unsigned short m_usMP5;
      };
   #endif

   #ifdef AK47_MODULE 
      class AK47 : public CBasePlayerWeapon
      {
      public:
	      void Spawn( void );
	      void Precache( void );
	      int iItemSlot( void ) { return 3; }
	      int GetItemInfo(ItemInfo *p);
	      int AddToPlayer( CBasePlayer *pPlayer );
	      BOOL Deploy( void );

         void PrimaryAttack( void );
         #ifndef LOCAL_WEAPON   
	         void Reload( void );
	         void WeaponIdle( void );
         #endif
	      virtual BOOL UseDecrement( void )
	         { 
            #ifdef CLIENT_WEAPONS 
                  return TRUE;
            #else return FALSE;
            #endif
	         }   
      private:
	      unsigned short m_usAK47;
      };
   #endif

   #ifdef AUG_MODULE
      class AUG : public CBasePlayerWeapon
      {
      public:
	      void Spawn( void );
	      void Precache( void );
	      int iItemSlot( void ) { return 1; }
	      int GetItemInfo(ItemInfo *p);
	      int AddToPlayer( CBasePlayer *pPlayer );
	      BOOL Deploy( void );

	      void PrimaryAttack( void );
         #ifndef LOCAL_WEAPON   
	         void SecondaryAttack( void );
	         void Reload( void );
	         void WeaponIdle( void );
         #endif
         int m_fInZoom;
	      virtual BOOL UseDecrement( void )
	         { 
            #ifdef CLIENT_WEAPONS 
                  return TRUE;
            #else return FALSE;
            #endif
	         }
      private:
         unsigned short m_usAUG;
      };
   #endif 

   #ifdef MAC10_MODULE
      class MAC10 : public CBasePlayerWeapon
      {
      public:
	      void Spawn( void );
	      void Precache( void );
	      int iItemSlot( void ) { return 3; }
	      int GetItemInfo(ItemInfo *p);
	      int AddToPlayer( CBasePlayer *pPlayer );
	      BOOL Deploy( void );

	      void PrimaryAttack( void );
         #ifndef LOCAL_WEAPON   
	         void Reload( void );
	         void WeaponIdle( void );
         #endif
	      virtual BOOL UseDecrement( void )
	         { 
            #ifdef CLIENT_WEAPONS 
                  return TRUE;
            #else return FALSE;
            #endif
	         }
      private:
         unsigned short m_usMAC10;
      };
   #endif


   #ifdef SG552_MODULE
      class SG552 : public CBasePlayerWeapon
      {
      public:
	      void Spawn( void );
	      void Precache( void );
	      int iItemSlot( void ) { return 3; }
	      int GetItemInfo(ItemInfo *p);
	      int AddToPlayer( CBasePlayer *pPlayer );
	      BOOL Deploy( void );

	      void PrimaryAttack( void );
         #ifndef LOCAL_WEAPON   
	         void SecondaryAttack( void );
	         void Reload( void );
	         void WeaponIdle( void );
         #endif
         int m_fInZoom;
	      virtual BOOL UseDecrement( void )
	         { 
            #ifdef CLIENT_WEAPONS 
                  return TRUE;
            #else return FALSE;
            #endif
	         }
      private:
	      unsigned short m_usSG552;
      };
   #endif

   #ifdef TMP_MODULE
      class TMP : public CBasePlayerWeapon
      {
      public:
	      void Spawn( void );
	      void Precache( void );
	      int iItemSlot( void ) { return 3; }
	      int GetItemInfo(ItemInfo *p);
	      int AddToPlayer( CBasePlayer *pPlayer );
	      BOOL Deploy( void );

	      void PrimaryAttack( void );
         #ifndef LOCAL_WEAPON   
	         void Reload( void );
	         void WeaponIdle( void );
         #endif
	      virtual BOOL UseDecrement( void )
	         { 
            #ifdef CLIENT_WEAPONS 
                  return TRUE;
            #else return FALSE;
            #endif
	         }
      private:
         unsigned short m_usTMP;
      };
   #endif

   #ifdef UMP45_MODULE
      class UMP45 : public CBasePlayerWeapon
      {
      public:
	      void Spawn( void );
	      void Precache( void );
	      int iItemSlot( void ) { return 3; }
	      int GetItemInfo(ItemInfo *p);
	      int AddToPlayer( CBasePlayer *pPlayer );
	      BOOL Deploy( void );

	      void PrimaryAttack( void );
         #ifndef LOCAL_WEAPON   
	         void Reload( void );
	         void WeaponIdle( void );
         #endif
	      virtual BOOL UseDecrement( void )
	         { 
            #ifdef CLIENT_WEAPONS 
                  return TRUE;
            #else return FALSE;
            #endif
	         }
      private:
         unsigned short m_usUMP45;
      };
   #endif

   #ifdef M3_MODULE
   class M3 : public CBasePlayerWeapon
      {
      public:
	      void Spawn( void );
	      void Precache( void );
	      int iItemSlot( void ) { return 3; }
	      int GetItemInfo(ItemInfo *p);
	      int AddToPlayer( CBasePlayer *pPlayer );
	      BOOL Deploy( void );

	      void PrimaryAttack( void );
         #ifndef LOCAL_WEAPON   
	         void Reload( void );
	         void WeaponIdle( void );
         #endif
	      int m_iShell;
	      virtual BOOL UseDecrement( void )
	         { 
            #ifdef CLIENT_WEAPONS 
                  return TRUE;
            #else return FALSE;
            #endif
	         }
      private:
	      unsigned short m_usM3;
         unsigned short m_iPump;
      };
   #endif

   #ifdef XM1014_MODULE
   class XM1014 : public CBasePlayerWeapon
   {
   public:
	   void Spawn( void );
	   void Precache( void );
	   int iItemSlot( void ) { return 3; }
	   int GetItemInfo(ItemInfo *p);
	   int AddToPlayer( CBasePlayer *pPlayer );
	   BOOL Deploy( void );

	   void PrimaryAttack( void );
      #ifndef LOCAL_WEAPON   
	      void Reload( void );
	      void WeaponIdle( void );
      #endif
	   virtual BOOL UseDecrement( void )
	      { 
         #ifdef CLIENT_WEAPONS 
               return TRUE;
         #else return FALSE;
         #endif
	      }
   private:
	   unsigned short m_usXM1014;
   };
   #endif

   #ifdef SG550_MODULE
   class SG550 : public CBasePlayerWeapon
   {
   public:
	   void Spawn( void );
	   void Precache( void );
	   int iItemSlot( ) { return 3; }
	   int GetItemInfo(ItemInfo *p);
	   BOOL Deploy( );

	   void PrimaryAttack( void );
      #ifndef LOCAL_WEAPON   
	      void SecondaryAttack( void );
	      void Reload( void );
	      void WeaponIdle( void );
      #endif
	   int AddToPlayer( CBasePlayer *pPlayer );
	   virtual BOOL UseDecrement( void )
	      { 
         #ifdef CLIENT_WEAPONS 
               return TRUE;
         #else return FALSE;
         #endif
	      }
	   int m_fInZoom;
   private:
	   unsigned short m_usSG550;
   };
   #endif

   #ifdef DEAGLE_MODULE
   class DEAGLE : public CBasePlayerWeapon
   {
   public:
	   void Spawn( void );
	   void Precache( void );
	   int iItemSlot( void ) { return 3; }
	   int GetItemInfo(ItemInfo *p);
	   int AddToPlayer( CBasePlayer *pPlayer );
	   BOOL Deploy( void );

	   void PrimaryAttack( void );
      #ifndef LOCAL_WEAPON   
	      void Reload( void );
	      void WeaponIdle( void );
      #endif
	   virtual BOOL UseDecrement( void )
	      { 
         #ifdef CLIENT_WEAPONS 
               return TRUE;
         #else return FALSE;
         #endif
	      }
   private:
      unsigned short m_usDEAGLE;
   };
   #endif

   #ifdef GLOCK_MODULE
   class GLOCK : public CBasePlayerWeapon
   {
   public:
	   void Spawn( void );
	   void Precache( void );
	   int iItemSlot( void ) { return 3; }
	   int GetItemInfo(ItemInfo *p);
	   int AddToPlayer( CBasePlayer *pPlayer );
	   BOOL Deploy( void );

	   void PrimaryAttack( void );
      #ifndef LOCAL_WEAPON   
	      void Reload( void );
	      void WeaponIdle( void );
      #endif
	   virtual BOOL UseDecrement( void )
	      { 
         #ifdef CLIENT_WEAPONS 
               return TRUE;
         #else return FALSE;
         #endif
	      }
   private:
      unsigned short m_usGLOCK;
   };
   #endif

   #ifdef B92D_MODULE
   class B92D : public CBasePlayerWeapon
   {
   public:
	   void Spawn( void );
	   void Precache( void );
	   int iItemSlot( void ) { return 3; }
	   int GetItemInfo(ItemInfo *p);
	   int AddToPlayer( CBasePlayer *pPlayer );
	   BOOL Deploy( void );

	   void PrimaryAttack( void );
      #ifndef LOCAL_WEAPON   
	      void Reload( void );
	      void WeaponIdle( void );
      #endif
	   virtual BOOL UseDecrement( void )
	      { 
         #ifdef CLIENT_WEAPONS 
               return TRUE;
         #else return FALSE;
         #endif
	      }
   private:
      unsigned short m_usB92D;
   };
   #endif

   #ifdef M16_MODULE
      class M16 : public CBasePlayerWeapon
      {
      public:
	      void Spawn( void );
	      void Precache( void );
	      int iItemSlot( void ) { return 3; }
	      int GetItemInfo(ItemInfo *p);
	      int AddToPlayer( CBasePlayer *pPlayer );
	      BOOL Deploy( void );

	      void PrimaryAttack( void );
         #ifndef LOCAL_WEAPON   
	         void Reload( void );
	         void WeaponIdle( void );
         #endif
	      virtual BOOL UseDecrement( void )
	         { 
            #ifdef CLIENT_WEAPONS 
                  return TRUE;
            #else return FALSE;
            #endif
	         }
      private:
	      unsigned short m_usM16;
      };
   #endif 

   #ifdef HEGRENADE_MODULE
   class HEGRENADE : public CBasePlayerWeapon
   {
   public:
	   void Spawn( void );
	   void Precache( void );
	   int iItemSlot( void ) { return 3; }
	   int GetItemInfo(ItemInfo *p);
	   BOOL Deploy( void );
      BOOL CanHolster();
      void Holster();

	   void PrimaryAttack( void );
      #ifndef LOCAL_WEAPON   
	      void WeaponIdle( void );
      #endif
	   virtual BOOL UseDecrement( void )
	      { 
         #ifdef CLIENT_WEAPONS 
               return TRUE;
         #else return FALSE;
         #endif
	      }
   private:
	   float m_flStartThrow;
	   float m_flReleaseThrow;
      unsigned short m_usHEGRENADE;
   };
   #endif

#endif