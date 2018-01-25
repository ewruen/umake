#if !defined (__UMAKE_LOCATE_H_)
#  define __UMAKE_LOCATE_H_ 

#include "config.h"
#include "list.h"
#include "uchar.h"
#include <Locale.h>

enum locate_type {
  LOCATE_TYPE_STRING,
  LOCATE_TYPE_VARIABLE,
  LOCATE_TYPE_NUMBER,
  LOCATE_TYPE_ALPHA,
  LOCATE_TYPE_SEMIOTIC_IMPLICIT_TIGHTLY, // FIXME: noset 
  LOCATE_TYPE_SEMIOTIC_IMPLICIT_RELAXED, // FIXME: noset 
  LOCATE_TYPE_SYMBOL_TIGHTLY,
  LOCATE_TYPE_SYMBOL_RELAXED,
  LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY,
  LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7,
  LOCATE_TYPE_ACCURATE_TIGHTLY,
  LOCATE_TYPE_ACCURATE_RELAXED,
};

# define LOCATE_SETACC_RELAXED_CONST(/*locate_desc_**/cc_Ccc, /*dummyconst us**/UuuUUs)   \
  ((struct locate_desc_ *)(cc_Ccc))->etype = LOCATE_TYPE_ACCURATE_RELAXED;  \
  ((struct locate_desc_ *)(cc_Ccc))->acceptORacc = UuuUUs;  \
  ((struct locate_desc_ *)(cc_Ccc))->ac_lens = wcslen (UuuUUs)

# define LOCATE_SETACC_TIGHTLY_CONST(/*locate_desc_**/cc_Ccc, /*dummyconst us**/UuuUUs)   \
  ((struct locate_desc_ *)(cc_Ccc))->etype = LOCATE_TYPE_ACCURATE_TIGHTLY;  \
  ((struct locate_desc_ *)(cc_Ccc))->acceptORacc = UuuUUs;  \
  ((struct locate_desc_ *)(cc_Ccc))->ac_lens = wcslen (UuuUUs)

# define LOCATE_SETACC_SYMBOLS_TIGHTLY_CONST(/*locate_desc_**/cc_Ccc, /*dummyconst us**/UuuUUs, lo, hi)   \
  ((struct locate_desc_ *)(cc_Ccc))->etype = LOCATE_TYPE_SYMBOL_TIGHTLY;  \
  ((struct locate_desc_ *)(cc_Ccc))->acceptORacc = UuuUUs;  \
  ((struct locate_desc_ *)(cc_Ccc))->ac_lens = wcslen (UuuUUs);  \
  ((struct locate_desc_ *)(cc_Ccc))->sig_ctl = USIG_AUTO_RANGE_;  \
  ((struct locate_desc_ *)(cc_Ccc))->sig_setnums = lo;  \
  ((struct locate_desc_ *)(cc_Ccc))->sig_setnums_hi = hi

# define LOCATE_SETACC_SYMBOLS_RELAXED_CONST2(/*locate_desc_**/cc_Ccc, /*dummyconst us**/UuuUUs)   \
  ((struct locate_desc_ *)(cc_Ccc))->etype = LOCATE_TYPE_SYMBOL_RELAXED;  \
  ((struct locate_desc_ *)(cc_Ccc))->acceptORacc = UuuUUs;  \
  ((struct locate_desc_ *)(cc_Ccc))->ac_lens = wcslen (UuuUUs);  \
  ((struct locate_desc_ *)(cc_Ccc))->sig_ctl = USIG_AUTO_TRUNC_INCLUED0

# define LOCATE_SETACC_SYMBOLS_RELAXED_CONST3(/*locate_desc_**/cc_Ccc, /*dummyconst us**/UuuUUs)   \
  ((struct locate_desc_ *)(cc_Ccc))->etype = LOCATE_TYPE_SYMBOL_RELAXED;  \
  ((struct locate_desc_ *)(cc_Ccc))->acceptORacc = UuuUUs;  \
  ((struct locate_desc_ *)(cc_Ccc))->ac_lens = wcslen (UuuUUs);  \
  ((struct locate_desc_ *)(cc_Ccc))->sig_ctl = USIG_AUTO_TRUNC

# define LOCATE_SETACC_ACCURATE_RELAXED_CONST2(/*locate_desc_**/cc_Ccc, /*dummyconst us**/UuuUUs)   \
  ((struct locate_desc_ *)(cc_Ccc))->etype = LOCATE_TYPE_ACCURATE_RELAXED;  \
  ((struct locate_desc_ *)(cc_Ccc))->acceptORacc = UuuUUs;  \
  ((struct locate_desc_ *)(cc_Ccc))->ac_lens = wcslen (UuuUUs)


# define USIG_AUTO_TRUNC -1
# define USIG_AUTO_TRUNC_INCLUED0 -2
// # define USIG_AUTO_GE -3 // >= 
// # define USIG_AUTO_LE -4 // <=
// # define USIG_AUTO_L -5 // < 
# define USIG_AUTO_RANGE_ -6  // > range-a < range b 
# define USIG_AUTO_EQUAL -7 // == 

# define UGET_CONTINUE 0
# define UGET_STOP 1 

# define ESS_COMPELTE 1 
# define ESS_OK 0
# define ESS_FAILED -1

# if defined (_WIN32)
#   define STRING_DQLINK _GC ('\\')
# else 
# endif 

struct locate_s_ {
  int apos;
  int length;
};

struct locate_desc_ {
  us *acceptORacc;
  us alpha;
  struct uchar *errs;
  enum locate_type etype ;
  int ac_pos;
  int ac_lens;
  int pt_pos;
  uconst us **dmtach_blk;
  int sig_setnums;
  int sig_setnums_hi; // >  < 
  int sig_ctl; // - 1 auto trunc, 
  int sig_match;
  int sig_get; // 
  int sig_exmatch;
  int sig_tuple_nums;
};

struct locate_essen_ {
  us *lu;
  us *at;
  int line;
  int upos;
  struct uchar *buf;
  struct locate_s_ init;
  struct locate_s_ doing;
  struct locate_s_ rdoing;
};

void locate_s_copy (struct locate_s_ *to, struct locate_s_ *from);
int locate_searchp (struct locate_essen_ *essen, struct locate_desc_ *serg, int nums, struct locate_s_ *gpc);
int locate_searchp2 (struct locate_essen_ *essen, struct locate_desc_ *serg, int nums, struct locate_s_ *gpc, int *fvail); // FIXME:uncompelte debug.
int locate_searchp3 (struct locate_essen_ *essen, int set_nums, ...); // llazy to mmmake ...
int locate_set (struct locate_essen_ *essen, int pos);
int locate_match_sym (struct locate_essen_ *essen, us cc, int *pstart, int *pend);
int locate_match_sym_pure (struct locate_essen_ *essen, us cc, int *pstart, int *pend);
int locate_match_var_pure (struct locate_essen_ *essen, struct uchar **u);
int locate_getcc (struct locate_essen_ *essen, us *cc);
int locate_getrcc (struct locate_essen_ *essen, us *cc);
int locate_promote (struct locate_essen_ *essen, int delta);
int locate_noremain (struct locate_essen_ *essen);
int locate_current (struct locate_essen_ *essen, int offrel);
void locate_attach (struct locate_essen_ *essen, struct uchar *analysis, int line);
void locate_attach_weak (struct locate_essen_ *essen, struct uchar *analysis, struct locate_s_ *es, int line);
void locate_obtinit (struct locate_essen_ *essen, struct locate_s_ *ltes);
void locate_saveto (struct locate_essen_ *essen, struct locate_s_ *ltes);
void locate_resumefrom2 (struct locate_essen_ *essen, int pos_left, int pos_right);
void locate_resumefrom (struct locate_essen_ *essen, struct locate_s_ *ltes);
void locate_resumefrom_baseblen (struct locate_essen_ *essen, int left);
void locate_backupcc (struct locate_essen_ *essen);
int locate_uselessdata (struct locate_essen_ *essen);
int locate_uselessdata2 (struct locate_essen_ *essen, int left, int right);
void locate_settoken2 (struct locate_essen_ *essen, int offrel, us cc);
us *locate_throwref3 (struct locate_essen_ *essen, int offrel);
int locate_clearrc (struct locate_essen_ *essen); /* remove margin spaces and notes on the right */
us *locate_throwref (struct locate_essen_ *essen, int pos);
us *locate_throwref2 (struct locate_essen_ *essen, struct locate_desc_ *ld);
us locate_throwtoken (struct locate_essen_ *essen);
us locate_throwtoken2 (struct locate_essen_ *essen, int pos);
us locate_setnullc (struct locate_desc_ *ld);
void locate_setnullq (struct locate_desc_ *ld, us cc);

#define kprint(fmt,...) do               \
{\
  extern volatile \
  long uspinlock;\
  \
  while ( InterlockedExchange (& uspinlock, 1) != 0)\
  _mm_pause ();\
  _mm_mfence();\
  wprintf (fmt, __VA_ARGS__);\
  _mm_mfence();\
  InterlockedExchange (& uspinlock, 0);\
} while (0)

#define kprint2(fmt,...) do               \
{\
  extern volatile \
  long uspinlock;\
  \
  while ( InterlockedExchange (& uspinlock, 1) != 0)\
  _mm_pause ();\
  _mm_mfence();\
  wprintf (fmt, __VA_ARGS__);\
  wprintf (WRAP_STRING_);\
  _mm_mfence();\
  InterlockedExchange (& uspinlock, 0);\
} while (0)

#define kprint_pure( /*uconst us **/content___ ) do               \
{\
  extern volatile \
  long uspinlock;\
  \
  while ( InterlockedExchange (& uspinlock, 1) != 0)\
  _mm_pause ();\
  _mm_mfence();\
{\
  int id;\
  int len = wcslen (content___);\
  for (id= 0; id < len; id++)\
  putwchar (content___[id]);\
}\
  _mm_mfence();\
  InterlockedExchange (& uspinlock, 0);\
} while (0)

#endif 