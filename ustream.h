#if !defined (__UMAKE_UNICODE_STREAM_H_)
#  define __UMAKE_UNICODE_STREAM_H_ 

#include "config.h"
#include <stdint.h>

struct raw_line {
  kbool invalid; /*deal or empty or \ replace */
  struct uchar *content;
};
struct ustream {
  struct raw_line **line_gp;
  int total; /* start 1*/
};

int open_usfile (uconst us *path_name, struct ustream **uss);
int open_usfile2 (uconst us *ptname, kbool clearsome, struct ustream **uss, uint64_t *mod64);
void close_usfile (struct ustream **uss);
void outinfos_usfile (struct ustream *uss);

#endif 