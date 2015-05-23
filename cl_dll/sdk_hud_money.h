#ifndef HUD_MONEY_H
#define HUD_MONEY_H

#define WIN_IMAGE_LIFE		2	// seconds that image is up

class CHudMoney: public CHudBase
{
public:
   CHudMoney() {m_iMoney = 800;}
	virtual int Init( void );
	virtual int VidInit( void );
	virtual int Draw(float fTime);
	virtual void Reset( void );
	int MsgFunc_NANA(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_Money(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_Armory(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_Time(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_NewTeam(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_GetTeams(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_CLAdvert(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_CLStat(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_CLID(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_CLSpawn(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_ServerVars(const char *pszName,  int iSize, void *pbuf);
	int MsgFunc_CLUpdate(const char *pszName,  int iSize, void *pbuf);
   int m_iNANA, m_iNANAindex;
	int m_iMoney;
	int m_iNearArmory;
	int m_fTimeLeft;
	int m_fBuyTimeLeft;
	int m_HUD_dollar;
   int m_iID;
   int m_iHealth;
   int m_iJuice;
	float m_fFade;

private:
	HSPRITE m_hSprite;
	void UpdateTiles(float fTime, long bits);
};	

#endif
