#ifndef SDK_C_RADAR
#define SDK_C_RADAR

class CHudRadar: public CHudBase

{
   public:
      int Init( void );
      int VidInit( void );
      int Draw(float flTime);
      virtual void Reset( void );
      int MsgFunc_Radar(const char *pszName,  int iSize, void *pbuf );

   private:
      HSPRITE m_hSpriteRadar;
      HSPRITE m_hSpriteSpot;
      int radar_height, radar_width;
      Vector origin[2], direction, angle;
      float view_angle;
      float distance;
};


#endif