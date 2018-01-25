#if !defined (__UMAKE_UCHAR_H_)
#  define __UMAKE_UCHAR_H_ 

#include "config.h"

/***
 * XXX:unicode only 
 ***/

struct uchar {
  int length;  /*  unicode buffer length */
  int maxbuf;/*  buffer max **/
  us *str;/*  real buf to unicode char **/
};

void uchar_ginit (void);
void uchar_guninit (void);
void uchar_init (struct uchar **u);
void uchar_copy (struct uchar **u, struct uchar *v);
void uchar_clearbuf (struct uchar *u);
/* void uchar_init_fromstack (struct uchar *u); */
/* void uchar_free_fromstack (struct uchar *u); */
void uchar_makepure_temp_fromstack (struct uchar *u, us *cont, int cnlen);
void uchar_uninit (struct uchar **u);
void uchar_initandopy (struct uchar **u, struct uchar *copy);
void uchar_replace (struct uchar *u, int pos, int len, us *cont, int cnlen);
void uchar_replace7 (struct uchar *u, int pos, int len, uconst us *cont);
void uchar_assign0 (struct uchar *u, struct uchar *v);
void uchar_shrink (struct uchar *u, int nums); /* > noset, -1 setall **/
void uchar_setlens (struct uchar *u, int nums);
void uchar_assign (struct uchar *u, us *cont, int len);
void uchar_insert (struct uchar *u, int pos, us *cont, int len);
void uchar_expand (struct uchar *u, int nums);
void uchar_insert3 (struct uchar *u, int pos, us *cont);
void uchar_insert4 (struct uchar *u, int pos, struct uchar *v);
void uchar_insert8 (struct uchar *u, int pos, struct uchar *v);
void /*__cdecl */ uchar_insert7 (struct uchar *u, int pos, uconst us *format, ...);
void uchar_remove (struct uchar *u, int pos, int len);
int uchar_cmp (struct uchar *u, struct uchar *v);
int uchar_cmp2 (struct uchar *u, uconst us*cont);
int uchar_cmp3 (struct uchar *u, us* cont, int ch_len);
int uchar_cmp3maxs (struct uchar *u, us* cont, int ch_len);
int uchar_cmp4 (struct uchar *u, struct uchar *v, int n);
void /*__cdecl */ uchar_assign3 (struct uchar *u, uconst us *format, ...);
int uchar_same_inchain (LIST_TYPE2_(uchar) list, struct uchar *uca);
void uchar_assign2 (struct uchar *u, uconst us *cont);
void uchar_merge_andfreetail (struct uchar *uh, struct uchar **ut);
#define uchar_setnull(/*struct uchar * */u)do { if ( (u) && (((u)->length) >=0)) (u)->str[(u)->length] = 0; } while (0)

#endif 