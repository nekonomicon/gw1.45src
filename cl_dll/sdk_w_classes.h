/***************************************************\
 Project X: Open Source version
 Written and compiled by
 Asaf Mesika (aeroj) and Nawfel Tricha (topaz)
 http://www.planetquake.com/qer/

 Class definition for weapons:
\***************************************************/

/* Turn LOCAL_WEAPON only when compiling client code! */
#define LOCAL_WEAPON
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
	      float m_flNextAnimTime;
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
	      float m_flNextAnimTime;
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
	      float m_flNextAnimTime;
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
	      float m_flNextAnimTime;
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
	      float m_flNextAnimTime;
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
	      float m_flNextAnimTime;
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
	      float m_flNextAnimTime;
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
	      float m_flNextAnimTime;
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

#endif