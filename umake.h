#if !defined (__UMAKE_H_)
#  define __UMAKE_H_ 

#include "config.h"
#include "list.h"
#include "uchar.h"
#include "path.h"

/* e.g. "D:\masm_path\ml.exe"  1452445 (timestamp 64bit)*/
typedef void *udcache_t;

struct udcache_line {
  int64_t timestamp64;
  struct uchar *fullpath;
};
struct udcache {
  int count;
  struct udcache_line *ucache_asr;
};
void udcache_uninit (struct udcache **udcache_);
void udcache_line_init (struct udcache_line **udl);
void udcache_line_uninit (struct udcache_line **udl);
int udcache_find (struct udcache *udcache_, struct uchar* fname, int64_t *timestamp64);
int udcache_load (LIST_TYPE2_(userpath2) uplist, uconst us *file_name, struct udcache **udcache_);
int udcache_open (LIST_TYPE2_(userpath2) uplist, uconst us *file_name, udcache_t *udc_t);
void udcache_close (udcache_t *udc_t);
int udcache_appenditem (udcache_t udc_t, uconst us *fullpath, int64_t timestamp64);

#endif 