#if !defined (__UMAKE_PATH_H_)
#  define __UMAKE_PATH_H_ 

#include "config.h"
#include "list.h"
#include "uchar.h"

struct spath;
struct xpath;

struct spath {
  struct uchar *path;
  LIST_TYPE2_(xpath) xpath_dt; 
};
struct xpath {
  struct uchar *suffix;
  LIST_TYPE2_(uchar) kpath_dt; /* not recursion */
};

void xpath_init (struct xpath **x_path);
void xpath_uninit (struct xpath **x_path);
void xpath_copy (struct xpath **dst, struct xpath *src);
void xpath_delete (struct xpath *dst, struct uchar *binname);
struct xpath *spath_find_xpath (struct spath *sp, struct uchar *suff);
void spath_chain_delete_spath_suff (struct list_ *dst, struct uchar *base, struct uchar *bpath, struct uchar *suff);
void spath_chain_delete_spath_suff_block (struct list_ *dst, struct uchar *base, struct uchar *bpath, struct uchar *suff, struct list_ *block);
void spath_init (struct spath **s_path);
void spath_uninit (struct spath **s_path);
void spath_copy (struct spath **dst, struct spath *src);
void spath_chain_copy (struct list_ *dst, struct list_ *src);
struct spath *spath_chain_find_spath (struct list_ *dst, struct uchar *base, struct uchar *bpath);
void spath_chain_delete_spath (struct list_ *dst, struct uchar *base, struct uchar *bpath);
int slash_clearand_totail (struct uchar *path);
int path_inherit ( struct uchar **uca, struct uchar *bt_path, struct uchar *as_path );
int path_inherit2 ( struct uchar *bt_path /*inout */, struct uchar *as_path );
int path_inheritgetbin (   struct uchar **uca, struct uchar *bt_path, struct uchar *as_path  ) ;

int path_cmp (struct uchar *u, struct uchar *v);
int path_cmp2 (struct uchar *u, struct uchar *v, struct uchar *bpath);

finline 
int isslash (us cc)  {
  if (cc == _GC ('/') || (cc == _GC ('\\')))
    return 0;
  else 
    return -1;
}

finline 
void setslash (us *cc)  {
#  if defined (_WIN32)
  cc[0]  = _GC ('\\');
#  else 
  cc[0]  = _GC ('/');
#  endif 
}

finline 
void setslash2 (us *cc, int pos)  {
  setslash(& cc[pos]);
}

#endif 