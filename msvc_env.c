# include "env.h"
# include <Shlwapi.h>

int msvc_envdo (LIST_TYPE2_(userpath2) list) {

  const int VersionMax = 20;
  struct uchar *uca = null;
  int mCount;
  int sCount;
  int id, iq, ib, ip;

  us xfb[48];
  us gfb[1024];
  us *cc;

  xfb[0]= _GC ('%');

  iq = wsprintfW (& xfb[1], _GR("%ls"), _GR ("DXSDK_DIR"));
  xfb[iq+1] = _GC ('%');
  xfb[iq+2] = 0;
  ExpandEnvironmentStringsW (& xfb[0], & gfb[0], sizeof (gfb)/ sizeof (gfb[0]));
  if (wcscmp (xfb, gfb) != 0)
    env2_insert (list, _GR ("DIRECTX_SDK"), & gfb[0], wcslen (_GR ("DIRECTX_SDK")), wcslen (& gfb[0]));
  else ;

  for (id = 1; id != VersionMax; id++) {

    iq = wsprintfW (& xfb[1], _GR ("VS%d0COMNTOOLS"), id);
    xfb[iq+1] = _GC ('%');
    xfb[iq+2] = 0;
    ExpandEnvironmentStringsW (& xfb[0], & gfb[0], sizeof (gfb)/ sizeof (gfb[0]));
    if (wcscmp (xfb, gfb) != 0)
      achieve LLM;
    else ;
  }
  achieve LLG;

LLM:;
  /*serach common7 e.g. D:\VS2010\Common7\Tools\ */
  uchar_init (& uca);
  iq = wcslen (gfb);
  id = wcslen (_GR ("Common7"));
  cc = wcsstr (gfb, _GR ("Common7"));
  ib = (((uintptr_t)cc) - (uintptr_t)gfb)/sizeof (us) + 1 + id ;
  /*%MSVC_IDE append */
  uchar_assign (uca, gfb, ib);
  uchar_insert3 (uca, -1, _GR ("IDE\\"));
  env2_insert (list, _GR ("MSVC_IDE"), uca->str, wcslen (_GR ("MSVC_IDE")), uca->length);
  uchar_assign (uca, gfb, ib);
  uchar_insert3 (uca, -1, _GR ("VC"));

  /*serach VC filepath */
  if (PathIsDirectoryW (uca->str) != FALSE)
    achieve LLX;
  else ;

  /* continue backup \ */
  cc [0] = 0;
  uchar_assign2 (uca, gfb);
  uchar_insert3 (uca, -1, _GR ("VC"));
  if (PathIsDirectoryW (uca->str) != FALSE) {
    struct uchar *epm;
    HKEY sKey = null;
LLX:epm = null;
    uchar_copy(& epm, uca);
    uchar_insert3 (epm, -1, _GR ("\\bin\\"));
    env2_insert (list, _GR ("MSVC_BIN"), epm->str, wcslen (_GR ("MSVC_BIN")), epm->length);
    uchar_copy(& epm, uca);
    uchar_insert3 (epm, -1, _GR ("\\include\\"));
    env2_insert (list, _GR ("MSVC_INCLUDE"), epm->str, wcslen (_GR ("MSVC_INCLUDE")), epm->length);
    uchar_copy(& epm, uca);
    uchar_insert3 (epm, -1, _GR ("\\lib\\"));
    env2_insert (list, _GR ("MSVC_LIB"), epm->str, wcslen (_GR ("MSVC_LIB")), epm->length);
    uchar_uninit (& epm);
    uchar_uninit (& uca);
LLG : // try get WINDOWS_SDK
    if ( RegOpenKeyExW (HKEY_LOCAL_MACHINE, 
       _GR ("SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows"), 
         0, KEY_READ | KEY_WOW64_32KEY,
      & sKey) == ERROR_SUCCESS)
    {
      DWORD result = 0;
      DWORD size = 0;
      DWORD type = 0;
      WCHAR wCbuff[512];

      result = RegQueryValueExW (sKey, _GR ("CurrentInstallFolder"), NULL, & type, NULL, & size);
      result = RegQueryValueExW (sKey, _GR ("CurrentInstallFolder"), NULL, NULL, (PVOID) & wCbuff[0], & size);
      assert (result == ERROR_SUCCESS);
      env2_insert (list, _GR ("WINDOWS_SDK"), wCbuff, wcslen (_GR ("WINDOWS_SDK")), wcslen (wCbuff));

      RegCloseKey (sKey);
      return 0;
    }
    return 0;
  }
  assert (0);

  return 0;
}




#  if 0


int main(void)
{

  struct list_ *mmc;
  list_init (& mmc);
  uchar_ginit ();
  msvc_envdo (mmc);
}

#  endif 

