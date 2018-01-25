#if !defined (__UMAKE_PRECOND_H_)
#  define __UMAKE_PRECOND_H_ 

#include "config.h"
#include "list.h"
#include "uchar.h"

struct precond_ {
  struct uchar *name;
  int numb;
  kbool type_numb;
};

void precond_init (struct precond_ **prec);
void precond_uninit (struct precond_ **prec);
struct precond_  *precond_exist (LIST_TYPE2_(precond_) list, us *name, int cclen);
/*  simple macro processing, same as clang */
int precond_set (  LIST_TYPE2_(userpath2) up2_chain, 
                   LIST_TYPE2_(precond_) prc_chain, struct ustream *ust, struct uchar *err);

#endif 