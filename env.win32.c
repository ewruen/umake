#include "env.h"
#include "locate.h"
#include "file.h"

static 
int env2_init (struct userpath2 **up, us *name, us *map, int cnlen, int maplen) {

  if (up && name && map && (cnlen > 0) && (maplen >0)) {
    struct userpath2 *ap = umalloc (sizeof (struct userpath2));
    uchar_init (& ap->name);
    uchar_init (& ap->mapper);
    uchar_assign(ap->name, name, cnlen);
    {
      /* remove \\ */
      int id;
      for (id = maplen -1; id >= 0; id--) {
        if (map[id] != _GC('\\') || map[id] != _GC('/')) {
          break;
        }
      }
      if (++id <= 1)
        id = maplen ;
      else ;

      uchar_assign(ap->mapper, map, id);
      * up = ap;
      return 0;
    }
  }
  return -1;
}

void env2_uninit (struct userpath2 **up) {
  if (up != null) {
    if (*up != null) {
      struct userpath2 *up2 = *up;
      * up = null;
      if (up2->mapper != null) {
        uchar_uninit (& up2->mapper);
      }
      if (up2->name != null) {
        uchar_uninit (& up2->name);
      }
      ufree (up2);
    }
  }
}

int env2_find (LIST_TYPE2_(userpath2) list, us *ename, int c, /* double intent */ struct uchar **epath) {
  int id;
  struct list_v *pv;
  kbool _init = kfalse;
  if (epath && ename && (c > 0)) {
    /* serach list */
    LIST_FOREACH (list, id, pv) {
      if (uchar_cmp3 (pv->userp2_o->name, ename, c) == 0) {
        if (*epath == null)
          uchar_init (epath);
        _init = ktrue;
        uchar_clearbuf(*epath);
        uchar_assign0(*epath, pv->userp2_o->mapper);
        achieve __clrslash; 
      }
    }
    /* serach current env */ 
    {
      us tm[512];
      us *pb = umalloc ((c+8)*sizeof(us)); 
      DWORD rm;
      memcpy (& pb[2], ename, c*sizeof(us));
      pb[c+2] = 0;
      rm = GetEnvironmentVariableW (& pb[2], tm, sizeof (tm)/sizeof (tm[0]));
      if (rm != 0)  {
          /* find */
        if (*epath == null)
          uchar_init (epath);
        uchar_clearbuf(*epath);
        uchar_assign (*epath, tm, rm);
        ufree (pb);
        achieve __clrslash;
      }

      /* serach system env */
      pb[1] = _GC('%');
      pb[c+2] = _GC('%');
      pb[c+3] = 0;
      rm = ExpandEnvironmentStringsW (& pb[1], & tm[0], sizeof (tm));
      if (rm != 0) {
        if (wcscmp (& pb[1], & tm[0]) != 0) {
          if (*epath == null)
            uchar_init (epath);
          uchar_clearbuf(*epath);
          uchar_assign (*epath, tm, rm - 1);
          ufree (pb);
          achieve __clrslash;
        } 
        ufree (pb);
        return -1;
      } else {
        assert (0);
      }

    }
  }
  return -1;
__clrslash:
  slash_clearand_totail (*epath);
  if (_init == kfalse) {
    struct userpath2 *ups2 = null;
    env2_init (& ups2, ename, (*epath)->str, c, (*epath)->length);
    list_insert_tail (list, ups2);
    kprint2 (_GR ("env-warning[%ls?%ls]:env inherit from system"), ups2->name->str, ups2->mapper->str);
  }
  return 0;
}

int env2_find_weak (LIST_TYPE2_(userpath2) list, us *ename, int c, /* double intent */ struct uchar **epath) {
  int id;
  struct list_v *pv;
  if (epath && ename && (c > 0)) {
    /* serach list */
    LIST_FOREACH (list, id, pv) {
      if (uchar_cmp3 (pv->userp2_o->name, ename, c) == 0) {
        if (*epath == null)
          uchar_init (epath);
        uchar_clearbuf(*epath);
        uchar_assign0(*epath, pv->userp2_o->mapper);
        return  0; 
      }
    }
  }
  return -1;
}

int env2_exist (LIST_TYPE2_(userpath2) list, us *ename, int c) {
  int id;
  struct list_v *pv;
  struct uchar *epm = null;
  kbool _init = kfalse;
  if (ename && (c > 0)) {
    /* serach list */
    LIST_FOREACH (list, id, pv) {
      if (uchar_cmp3 (pv->userp2_o->name, ename, c) == 0) {
        return 0; 
      }
    }
    /* serach current env */ 
    {
      us tm[512];
      us *pb = umalloc ((c+8)*sizeof(us)); 
      DWORD rm;
      memcpy (& pb[2], ename, c*sizeof(us));
      pb[c+2] = 0;
      rm = GetEnvironmentVariableW (& pb[2], tm, sizeof (tm)/sizeof (tm[0]));
      if (rm != 0)  {
          /* find */
        uchar_init (& epm);
        uchar_clearbuf(epm);
        uchar_assign (epm, tm, rm);
        ufree (pb);
        achieve __clrslash;
      }

      /* serach system env */
      pb[1] = _GC('%');
      pb[c+2] = _GC('%');
      pb[c+3] = 0;
      rm = ExpandEnvironmentStringsW (& pb[1], & tm[0], sizeof (tm));
      if (rm != 0) {
        if (wcscmp (& pb[1], & tm[0]) != 0) {
          uchar_init (& epm);
          uchar_clearbuf(epm);
          uchar_assign (epm, tm, rm - 1);
          ufree (pb);
          achieve __clrslash;
        } 
        ufree (pb);
        return -1;
      } else {
        assert (0);
      }

    }
  }
  return -1;
__clrslash:
  slash_clearand_totail (epm);
  if (_init == kfalse) {
    struct userpath2 *ups2 = null;
    env2_init (& ups2, ename, epm->str, c, epm->length);
    list_insert_tail (list, ups2);
    kprint2 (_GR ("env-warning[%ls?%ls]:env inherit from system"), ups2->name->str, ups2->mapper->str);
  }
  return 0;
}

int env2_exist_weak (LIST_TYPE2_(userpath2) list, us *ename, int c) {
  int id;
  struct list_v *pv;
  if (ename && (c > 0)) {
    /* serach list */
    LIST_FOREACH (list, id, pv) {
      if (uchar_cmp3 (pv->userp2_o->name, ename, c) == 0) {
        return  0; 
      }
    }
  }
  return -1;
}

int env2_insert (LIST_TYPE2_(userpath2) list, us *name, us *map, int cnlen, int maplen) {
  int id;
  struct list_v *pv;
  struct userpath2 *up2;
  BOOL sig;
  /* serach list */
  LIST_FOREACH (list, id, pv) {
    if (uchar_cmp3 (pv->userp2_o->name, name, cnlen) == 0)
        return -1; 
    else ;
  }
  env2_init (& up2, name, map, cnlen, maplen);
  list_insert_tail (list, up2);
  /* set process env-var */
  sig = SetEnvironmentVariableW (up2->name->str, up2->mapper->str);
  assert (sig != FALSE);
  return 0;
}

# if 1





#if 0

int main (void)
{

  struct uchar *um = null;
  struct uchar *k = null;
  struct list_ *c;
  struct uchar *s;
  
  uchar_ginit ();
  uchar_init (& s);
  uchar_init ( &um);
  list_init (& c);
  env2_find (c, L"PSModulePath", wcslen(L"PSModulePath"),& um);
  uchar_assign2(s, L"E:\\SDL");
  __spliter_path (& k, um, s);
  uchar_uninit (&um);
  uchar_guninit ();
}

#endif 

# endif 