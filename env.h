#if !defined (__UMAKE_ENV_H_)
#  define __UMAKE_ENV_H_ 

#include "config.h"
#include "list.h"
#include "uchar.h"

struct userpath2 {
  struct uchar *name;
  struct uchar *mapper;
};

void env2_uninit (struct userpath2 **up);
int env2_find (LIST_TYPE2_(userpath2) list, us *ename, int cnlen, /* double intent */ struct uchar **epath);
int env2_find_weak (LIST_TYPE2_(userpath2) list, us *ename, int cnlen, /* double intent */ struct uchar **epath);
int env2_exist (LIST_TYPE2_(userpath2) list, us *ename, int cnlen) ;
int env2_exist_weak (LIST_TYPE2_(userpath2) list, us *ename, int cnlen) ;
int env2_insert (LIST_TYPE2_(userpath2) list, us *name, us *map, int cnlen, int maplen);

#endif 

