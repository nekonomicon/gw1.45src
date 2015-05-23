May 12, 2015
12:25 AM
swampdog@modriotgaming.ga

---
Changes made to code:
---

In "gamerules.h", I fixed the spot that was causing "LNK2001" error. 

These were the only errors I ran into while compiling under certain settings in a certain IDE:

multiplay_gamerules.obj : error LNK2001: unresolved external symbol "public: virtual void __thiscall CGameRules::CheckDeadRoundEnd(void)" (?CheckDeadRoundEnd@CGameRules@@UAEXXZ)
singleplay_gamerules.obj : error LNK2001: unresolved external symbol "public: virtual void __thiscall CGameRules::CheckDeadRoundEnd(void)" (?CheckDeadRoundEnd@CGameRules@@UAEXXZ)
multiplay_gamerules.obj : error LNK2001: unresolved external symbol "public: virtual void __thiscall CGameRules::CheckRoundEnd(void)" (?CheckRoundEnd@CGameRules@@UAEXXZ)
singleplay_gamerules.obj : error LNK2001: unresolved external symbol "public: virtual void __thiscall CGameRules::CheckRoundEnd(void)" (?CheckRoundEnd@CGameRules@@UAEXXZ)
.\debughl/hl.dll : fatal error LNK1120: 2 unresolved externals
Error executing link.exe.

hl.dll - 5 error(s), 0 warning(s)

----

I also fixed a warning message related to the first line in the file "hl.def" in the dlls folder. I removed the first line, and the warning went away. This warning has been known to cause problems with servers. There may have been another issue related to the files "hl.dsw" and "hl.dsp". I renamed made copies of those files and renamed the copies to "mp.dsw" and "mp.dsp". I then created a new workspace and added the new "mp" project file to the project, rather than the "hl" file.

---