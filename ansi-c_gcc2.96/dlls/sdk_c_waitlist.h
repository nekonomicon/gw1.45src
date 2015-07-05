#ifndef WAITLIST_H
#define WAITLIST_H

typedef struct DeadQueue_u
   {
   CBasePlayer *player;
   int used;
   } DeadQueue_t;


class WaitList
{
   public:
      WaitList();
      ~WaitList();
      void FlushWaitList();
      void AddToWaitList(CBasePlayer *p);
      CBasePlayer *NextOnWaitList(int team);
      void ShiftDown();
      void DisplayList(CBasePlayer *me);
   private: 
      DeadQueue_t *q;
      int index;
};

extern WaitList wl[4];

#endif