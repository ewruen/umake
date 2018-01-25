#include "path.h"
#include "locate.h"
#include "file.h"

void xpath_init (struct xpath **x_path) {
  struct xpath *p = umalloc (sizeof (struct xpath));
  uchar_init (& p->suffix);
  list_init (& p->kpath_dt);
  * x_path = p;
}

void xpath_uninit (struct xpath **x_path) {
  if (x_path != null)  {
    struct xpath *p = * x_path;
    * x_path = null;
    if (p != null)  {
      uchar_uninit (& p->suffix);
      list_uninit_ISQ (& p->kpath_dt, uchar_uninit);
      ufree (p);
    }
  }
}

void xpath_copy (struct xpath **dst, struct xpath *src) {

  struct xpath *dc;
  if (*dst == null)
    xpath_init (dst);

  // copy suffix 
  dc = *dst ;
  uchar_assign0 (dc->suffix, src->suffix);

  // copyt bin-chain.
  {
    int id;
    struct list_v *p;
    LIST_FOREACH (src->kpath_dt, id, p) {
      struct uchar *uc;
      uchar_init (& uc);
      uchar_assign0 (uc, p->uchar_o);
      list_insert_tail (dc->kpath_dt, uc);
    }
  }
}

void xpath_delete (struct xpath *dst, struct uchar *binname) {

  // dlete bin
  {
    int id;
    struct list_v *p;
    LIST_FOREACH (dst->kpath_dt, id, p) {
      if (uchar_cmp3maxs (binname, p->uchar_o->str, binname->length)) {
        list_remove_ISQ_L (dst->kpath_dt, p, uchar_uninit);
        return ;
      }
    }
  }
  return ;
}


void spath_init (struct spath **s_path)  {
  struct spath *p = umalloc (sizeof (struct spath));
  uchar_init (& p->path);
  list_init (& p->xpath_dt);
  * s_path = p;
}

void spath_uninit (struct spath **s_path)  {
  if (s_path != null)  {
    struct spath *p = * s_path;
    * s_path = null;
    if (p != null)  {
      uchar_uninit (& p->path);
      list_uninit_ISQ (& p->xpath_dt, xpath_uninit);
      ufree (p);
    }
  }
}

void spath_copy (struct spath **dst, struct spath *src) {

  struct spath *dc;
  if (*dst == null)
    spath_init (dst);

  // copy path 
  dc = *dst ;
  uchar_assign0 (dc->path, src->path);

  // copyt path-chain.
  {
    int id;
    struct list_v *p;
    LIST_FOREACH (src->xpath_dt, id, p) {
      struct xpath *xpa = null;
      xpath_copy (& xpa, p->xpath_o);
      list_insert_tail (dc->xpath_dt, xpa);
    }
  }
}

void spath_chain_copy (struct list_ *dst, struct list_ *src) {
  
  // copyt spath-.
  {
    int id;
    struct list_v *p;
    LIST_FOREACH (src, id, p) {
      struct spath *spa = null;
      spath_copy (& spa, p->spath_o);
      list_insert_tail (dst, spa);
    }
  }
}

struct xpath
 * spath_find_xpath (struct spath *sp, struct uchar *suff) 
{
  struct list_v *p;
  int id;

  LIST_FOREACH (sp->xpath_dt, id, p) {
    kbool emp = kfalse;
    kbool emp2 = kfalse;
    if (suff== null || (suff->length <= 0))
      emp = ktrue;
    if (p->xpath_o->suffix || (p->xpath_o->suffix->length <= 0))
      emp2 = ktrue;
    if (emp && emp2) {
      return p->xpath_o;
    } else if (! emp &&(!emp2)) {
      if (! uchar_cmp3maxs(suff, p->xpath_o->suffix->str, p->xpath_o->suffix->length)) {
        return p->xpath_o;
      }
    }
  }
  return null;
}

void spath_chain_delete_spath (struct list_ *dst, struct uchar *base, struct uchar *bpath) {

  struct list_v *p;
  int id;

  LIST_FOREACH (dst, id, p) {
    if (path_cmp2 (p->spath_o->path, base, bpath) == 0) {
      list_remove_ISQ_L (dst, p, spath_uninit);
      return ;
    }
  }
}

struct spath *
spath_chain_find_spath (struct list_ *dst, struct uchar *base, struct uchar *bpath) {

  struct list_v *p;
  int id;

  LIST_FOREACH (dst, id, p) {
    if (path_cmp2 (p->spath_o->path, base, bpath) == 0) {
      return p->spath_o;
    }
  }
  return null;
}

void spath_chain_delete_spath_suff (struct list_ *dst, struct uchar *base, struct uchar *bpath, struct uchar *suff) 
{
  struct list_v *p;
  int id;

  LIST_FOREACH (dst, id, p) {
    if (path_cmp2 (p->spath_o->path, base, bpath) == 0) {
       // find suffix 
      struct list_v *p2;
      int id2;
      LIST_FOREACH (p->spath_o->xpath_dt, id2, p2)  {
        kbool emp = kfalse;
        kbool emp2 = kfalse;
        if (suff== null || (suff->length <= 0))
          emp = ktrue;
        if (p2->xpath_o->suffix || (p2->xpath_o->suffix->length <= 0))
          emp2 = ktrue;
        if (emp && emp2) {
          // remove from current spath.
          list_remove_ISQ_L (p->spath_o->xpath_dt, p2, xpath_uninit);
          return ;
        } else if (! emp &&(!emp2)) {
          if (! uchar_cmp3maxs(suff, p2->xpath_o->suffix->str, p->xpath_o->suffix->length)) {
            list_remove_ISQ_L (p->spath_o->xpath_dt, p2, xpath_uninit);
            return ;
          }
        }
      }
    }
  }
}

void spath_chain_delete_spath_suff_block (struct list_ *dst, struct uchar *base, struct uchar *bpath, struct uchar *suff, struct list_ *block) {
  struct list_v *p;
  int id;

  LIST_FOREACH (dst, id, p) {
    if (path_cmp2 (p->spath_o->path, base, bpath) == 0) {
      // find suffix 
      struct list_v *p2;
      int id2;
      LIST_FOREACH (p->spath_o->xpath_dt, id2, p2)  {
        kbool emp = kfalse;
        kbool emp2 = kfalse;
        if (suff== null || (suff->length <= 0))
          emp = ktrue;
        if (p2->xpath_o->suffix || (p2->xpath_o->suffix->length <= 0))
          emp2 = ktrue;
        if (emp && emp2) {
          // remove from current spath.
          achieve Q3;
        } else if (! emp &&(!emp2)) {
          if (! uchar_cmp3maxs(suff, p2->xpath_o->suffix->str, p2->xpath_o->suffix->length)) {
            struct list_v *p3;
            int id3;
        Q3:  LIST_FOREACH (block, id3, p3)
              xpath_delete (p2->xpath_o, p3->uchar_o);
            if (p->spath_o->xpath_dt->nums <= 0)
            list_remove_ISQ_L (p->spath_o->xpath_dt, p2, xpath_uninit);
            return ;
          }
        }
      }
    }
  }
}

int path_clearself (   struct uchar *ucc  ) 
{
  int q, b, p, d;
  us *c;//
  q = ucc->length;
  c = ucc->str;
  b = 0;
LLR:
  p = 0;

  for (; b < q; b++) {
    /* match ADB\..\|.\ */
    if (c[b] == _GC ('.'))
      p ++;
    else if (isslash (c[b]) == 0) {
      /* last is slash?, if not .nodone */
      if ((p>=1 && p<=2) && isslash (c[b-p-1]) == 0) {
        b -= p;
        uchar_remove (ucc, b, p+1);
        if (p == 2) {
          /* path backup one times  */
          d = b - 2;
          while (d >= 0) {
            if (isslash(c[d]) == 0)
              break;
            else ;
            d -- ;
          }
          if (d < 0)
            return -1;
          else ;
          uchar_remove (ucc, d + 1, b- 2 - d - 1 + 2); 
          b = d;
        }
        q = ucc->length;
        achieve LLR;
      }
    } else {
      p= 0;
    }
  }
  
  slash_clearand_totail (ucc);
  return 0;
}

int slash_clearand_totail (struct uchar *path) {
  int id = 0;
  slashtail_clear (path->str, path->length, & id);
  assert (id != 0);
  uchar_setlens (path, id);
  uchar_insert3 (path, -1, SLASH_STRING_);
  return 0;
}

int path_inherit (   struct uchar **uca, 
                       struct uchar *bt_path,
                       struct uchar *as_path  ) 
{
  /* check is abs path??*/
  int id;
  int q;
  int p;
  int b;
  int g, s;

  us *cc = as_path->str;
  b = as_path->length;
  q = b;
  id = 0;
  p = 0;
  for (; id != b; id++) {
    if (cc[id] == _GC (' ') || cc[id] == _GC ('\t'))
      continue  ;
    if (iswalpha (cc[id]) != 0
      && (p == 0))
      p = 1;
    else if (cc[id] == _GC(':') 
      && (p == 1))
    {
      uchar_copy (uca, as_path);
      path_clearself ((*uca));
      return 0;
    }
    else 
      break ;
  }
  /* clear ..\.\  */
  p = 0;
  s = (*uca) ? 0 : -1;
  uchar_copy (uca, as_path);
  slashtail_clear (bt_path->str, bt_path->length, & g);
//slashtail_clear ((*uca)->str, (*uca)->length, & q);
  uchar_insert3 ((*uca), 0, SLASH_STRING_);
  uchar_insert ((*uca), 0, bt_path->str, g);
  if ( path_clearself ((*uca)) == 0) 
    return 0;
  else if (s)
    uchar_uninit (uca);
  return -1;
}

int path_inherit2 (   struct uchar *bt_path,
                       struct uchar *as_path  ) 
{
  /* check is abs path??*/
  int id;
  int q;
  int p;
  int b;
  int g, s;

  us *cc = as_path->str;
  b = as_path->length;
  q = b;
  id = 0;
  p = 0;
  for (; id != b; id++) {
    if (cc[id] == _GC (' ') || cc[id] == _GC ('\t'))
      continue  ;
    if (iswalpha (cc[id]) != 0
      && (p == 0))
      p = 1;
    else if (cc[id] == _GC(':') 
      && (p == 1))
    {
      path_clearself (as_path);
      return 0;
    }
    else 
      break ;
  }
  /* clear ..\.\  */
  slashtail_clear (bt_path->str, bt_path->length, & g);
//slashtail_clear ((*uca)->str, (*uca)->length, & q);
  uchar_insert3 (as_path, 0, SLASH_STRING_);
  uchar_insert (as_path, 0, bt_path->str, g);
  if ( path_clearself (as_path) == 0) 
    return 0;
  else 
    return -1;
}
int path_inheritgetbin (   struct uchar **uca, 
                          struct uchar *bt_path,
                          struct uchar *as_path  ) 
{
  int s = 0;
  if (path_inherit (uca, bt_path, as_path) == 0)
    uchar_shrink (*uca, 1);
  else 
    s = -1;
  return s ;
}

int path_cmp (struct uchar *u, struct uchar *v) {
  int s, d;
  slashtail_clear (u->str, u->length, & s);
  slashtail_clear (v->str, v->length, & d);

  if (d == s && (! uchar_cmp4 (u, v, d)))
    return 0;
  else 
    return -1;
}

int path_cmp2 (struct uchar *u, struct uchar *v, struct uchar *bpath) {
  int s, d;
  int q, c;
  int sig = -1;
  struct uchar *u2 = null;
  struct uchar *v2 = null;
  q = path_inherit (& u2, bpath, u);
  c = path_inherit (& v2, bpath, v);
  if (q || c) 
    achieve Q2;

  slashtail_clear (u2->str, u2->length, & s);
  slashtail_clear (v2->str, v2->length, & d);

  if (d == s && (! uchar_cmp4 (u2, v2, d)))
    sig = 0;
Q2: uchar_uninit (& u2);
  uchar_uninit (& v2);
  return sig;
}

# if  0


int main (void) {




  uchar_ginit ();
  {
    struct uchar *accept = null;
    struct uchar *amc;
    struct uchar *amq;
    uchar_init (& amc);
    uchar_init (& amq);
    uchar_assign2(amc, _GR ("D:\\ABC")); // base
    uchar_assign2(amq, _GR ("..\\ppo\\..\\..\\..\\saws\\..\\")); // toinert 


    path_inherit(& accept, amc, amq);
    path_clearself(accept);
                   return 0;
  }
}
# endif