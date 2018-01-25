#if !defined (__UMAKE_CAS_H_)
#  define __UMAKE_CAS_H_ 

/* simple atomic operate **/

#include "config.h"
#include <stdint.h>

int32_t fastcall store32 (void *p, int32_t val);
int32_t fastcall add32 (void *p, int32_t val);
int64_t fastcall add64 (void *p, int64_t val);
void fastcall or32 (void *p, int32_t val);
kbool fastcall casdp3 (  void **p, 
                                void **val_gp, 
                                void **cmp_gp );
#endif 