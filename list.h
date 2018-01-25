#if !defined (__UMAKE_LIST_H_)
#  define __UMAKE_LIST_H_ 

#include "config.h"
#include <stdint.h>
#include <errno.h>

struct pass_;
struct uchar;
struct spath_set;
struct spath;
struct xpath;
struct kpath;
struct pass_varChain;
struct string_var;
struct file_var; 
struct set_var;
struct udcache;
struct userpath2;
struct precond_;
struct chunkccp;
struct alter_;
struct atdep;
struct dep_cache;
struct shell_desc;
struct ctl_base_exec_infos;
struct cppdep_node2;
struct kcache_chain;

struct list_v {
  union {
    struct uchar *uchar_o;
    struct spath *spath_o;
    struct xpath *xpath_o;
    struct shell_desc *shell_o;
    struct pass_varChain *pass_varChain_o;
    struct string_var *string_var_o;
    struct file_var *file_var_o;
    struct set_var *set_var_o;
    struct udcache *udc_o;
    struct userpath2 *userp2_o;
    struct precond_ *prec_o;
    struct chunkccp *ccp_o;
    struct alter_ *alt_o;
    struct atdep *atdep_o;
    struct dep_cache *depc_o;
    struct ctl_base_exec_infos *cbei_o;
    struct cppdep_node2 *cpp2node_o;
    struct kcache_chain *kcc_o;
    void *obj; /*XXX:Sequential dependency, \
                       When necessary, it can be considered as a single linked list and queue */
  };
  union {
    struct list_v *level_atomic; /* for atomic and ABA question */
    struct list_v *level_p;
    int64_t *level_aip64;
    int32_t *level_aip32;
    /* int64_t level_ai64; */
    int32_t level_ai32;
    intptr_t level_aiptr;
  };
  union {
    struct list_v *level_n;/*XXX:Sequential dependency, \
                       When necessary, it can be considered as a single linked list and queue */
    int64_t *level_nip64;
    int32_t *level_nip32;
    /* int64_t level_ni64; */
    int32_t level_ni32;
    intptr_t level_niptr;
  };
};

struct list_ {
  struct list_v *set;
//struct list_v *set_cache;
  int nums;
//int nums_cache;
};

finline
void list_init (struct list_ **s) {

  struct list_ *u = umalloc (sizeof (struct list_));

  u->nums =0;
  u->set =null;
  *s = u;
}

finline
struct list_v *list_get (struct list_ *s, int pos) {

  int ii;
  struct list_v *pv = null;
  assert (s != null);
  
  if (pos == -1) {
    pos = s->nums - 1;
  }

  // ii = s->nums;
  if (pos <= (s->nums - 1)) {

    for (ii =0, pv = s->set; ii != pos; ii++) {
      pv =pv->level_n;
    }
  }
  return pv;
}

finline
void *list_getnode (struct list_ *s, int pos) {

  struct list_v *p= list_get (s, pos);
  if (p) {
    return p->obj;
  } else {
    return null;
  }
}

finline
int list_insert (struct list_ *s, int pos, void *obj) {

  struct list_v *pv = null;
  struct list_v *ci;
  assert (s != null);

  if (pos == -1) {
    pos = s->nums;
  }

  if (pos > (s->nums)) {
    return -1;
  } 

  pv = umalloc (sizeof (struct list_v));
  pv->obj = obj;

  if (s->nums == 0) {

    pv->level_n = pv;
    pv->level_p = pv;

    s->set = pv;
    s->nums++;
    return 0;
  }

  ci = list_get (s, pos == s->nums ?0: pos);

  pv->level_n = ci;
  pv->level_p = ci->level_p;

  ci->level_p->level_n = pv;
  ci->level_p = pv;

  if (pos ==0) {
    /* insert head **/
    s->set = pv;
  }
  s->nums++;
  return 0;
}

finline
int list_insert_tail (struct list_ *s, void *obj) {

  struct list_v *pv;
  assert (s != null);

  pv = umalloc (sizeof (struct list_v));
  pv->obj = obj;

  if (s->nums == 0) {

    pv->level_n = pv;
    pv->level_p = pv;

    s->set = pv;
  } else {
    pv->level_n = s->set;
    pv->level_p = s->set->level_p;
    s->set->level_p->level_n = pv;
    s->set->level_p = pv;
  }
  s->nums++;
  return 0;
}

finline
int list_insert_tail_L (struct list_ *s, struct list_v *pv) {

  // struct list_v *pv;
  assert (s != null);

  // pv = umalloc (sizeof (struct list_v));
  // pv->obj = obj;

  if (s->nums == 0) {

    pv->level_n = pv;
    pv->level_p = pv;

    s->set = pv;
  } else {
    pv->level_n = s->set;
    pv->level_p = s->set->level_p;
    s->set->level_p->level_n = pv;
    s->set->level_p = pv;
  }
  s->nums++;
  return 0;
}

finline
int list_insert_L (struct list_ *s, int pos, struct list_v *liobj) {

  struct list_v *pv = liobj;
  struct list_v *ci;
  assert (s != null);

  if (pos == -1) {
    pos = s->nums;
  }

  if (pos > (s->nums)) {
    return -1;
  } 

  if (s->nums == 0) {

    pv->level_n = pv;
    pv->level_p = pv;

    s->set = pv;
    s->nums++;
    return 0;
  }

  ci = list_get (s, pos == s->nums ?0: pos);

  pv->level_n = ci;
  pv->level_p = ci->level_p;

  ci->level_p->level_n = pv;
  ci->level_p = pv;

  if (pos ==0) {
    /* insert head **/
    s->set = pv;
  }
  s->nums++;
  return 0;
}

finline
void list_single_free (struct list_v *sv) {

  if (sv != null) {
    ufree (sv);
  }
}

finline
struct list_v *list_pop (struct list_ *s, int pos) {

  struct list_v *pv = null;
  struct list_v *ci;
  assert (s != null);

  if (pos == -1) {
    pos = s->nums - 1;
  }

  if (pos >= (s->nums) || (s->nums <= 0)) {
    return null;
  } 

  ci = list_get (s,  pos);
  pv = ci;

  if (pos ==0) {
    s->set = ci->level_n;
  }
  {
    void *a = ci->level_n;
    void *b = ci->level_p;
    struct list_v **c = & ci->level_p->level_n;
    struct list_v **d = & ci->level_n->level_p;
    *c = a;
    *d = b;
  }

  if ( (-- s->nums) == 0) {
    s->set = null;
  }
  return ci;
}

finline
void *list_pop2 (struct list_ *s, int pos) {

  struct list_v *pv = null;
  struct list_v *ci;
  void *obj;
  assert (s != null);

  if (pos == -1) {
    pos = s->nums - 1;
  }

  if (pos >= (s->nums) || (s->nums <= 0)) {
    return null;
  } 

  ci = list_get (s,  pos);
  pv = ci;

  if (pos ==0) {
    s->set = ci->level_n;
  }
  {
    void *a = ci->level_n;
    void *b = ci->level_p;
    struct list_v **c = & ci->level_p->level_n;
    struct list_v **d = & ci->level_n->level_p;
    *c = a;
    *d = b;
  }

  if ( (-- s->nums) == 0) {
    s->set = null;
  }
  obj = ci->obj;
  ufree (ci);
  return obj;
}

finline
void list_remove_ISQ (struct list_ *s, int pos, void (*pernode_)(void *node)) {

  struct list_v *pv = null;
  struct list_v *ci;
  assert (s != null);

  if (pos == -1) {
    pos = s->nums - 1;
  }

  if (pos >= (s->nums) || (s->nums <= 0)) {
    return ;
  } 

  ci = list_get (s,  pos);
  pv = ci;

  if (pos ==0) {
    s->set = ci->level_n; 
  }

  {
    void *a = ci->level_n;
    void *b = ci->level_p;
    struct list_v **c = & ci->level_p->level_n;
    struct list_v **d = & ci->level_n->level_p;
    *c = a;
    *d = b;
  }

  if (pernode_ != null) {
    pernode_ (& ci->obj);
  }

  ufree (ci);
  if ( (-- s->nums) == 0) {
    s->set = null;
  }
}

finline
void list_remove_ISQ_L (struct list_ *s, struct list_v *pf, void (*pernode_)(void *node)) {

  struct list_v *pv = null;
  assert (s != null);

  if ((s->nums <= 0)) {
    return ;
  } 

  if (s->set ==pf) {
    s->set = pf->level_n; 
  }

  {
    void *a = pf->level_n;
    void *b = pf->level_p;
    void **c = & pf->level_p->level_n;
    void **d = & pf->level_n->level_p;
    *c = a;
    *d = b;
  }

  if (pernode_ != null) {
    pernode_ (& pf->obj);
  }

  ufree (pf);
  if ( (-- s->nums) == 0) {
    s->set = null;
  }
}

finline
void
  list_pop_L (struct list_ *s, struct list_v *pf) {

  struct list_v *pv = null;
  assert (s != null);

  if ((s->nums <= 0)) {
    return ;
  } 

  if (s->set ==pf) {
    s->set = pf->level_n; 
  }

  {
    void *a = pf->level_n;
    void *b = pf->level_p;
    void **c = & pf->level_p->level_n;
    void **d = & pf->level_n->level_p;
    *c = a;
    *d = b;
  }
  if ( (-- s->nums) == 0) {
    s->set = null;
  }
}

finline
void *
  list_pop_L2 (struct list_ *s, struct list_v *pf) {

  struct list_v *pv = null;
  void *no;
  assert (s != null);

  if ((s->nums <= 0)) {
    return null;
  } 

  if (s->set ==pf) {
    s->set = pf->level_n; 
  }

  {
    void *a = pf->level_n;
    void *b = pf->level_p;
    void **c = & pf->level_p->level_n;
    void **d = & pf->level_n->level_p;
    *c = a;
    *d = b;
  }
  if ( (-- s->nums) == 0) {
    s->set = null;
  }
  no = pf->obj;
  ufree (pf);
  return no;
}

finline 
int 
list_retidx (struct list_ *s, struct list_v *litemv) {

  if (s && litemv) {
    int ii;
    struct list_v *pmv = s->set;
    for  ( ii= 0; ii != s->nums; ii++) {
      if ( pmv == litemv) {
        return ii;
      }
      pmv= pmv->level_n;
    }
  }
  return -1;
}

finline 
int 
list_retidx2 (struct list_ *s, void *node) {

  if (s && node) {
    int ii;
    struct list_v *pmv = s->set;
    for  ( ii= 0; ii != s->nums; ii++) {
      if ( pmv->obj == node) {
        return ii;
      }
      pmv= pmv->level_n;
    }
  }
  return -1;
}

finline
void list_remove (struct list_ *s, int pos) {

  list_remove_ISQ (s, pos, null);
}

finline
void list_remove_ISQ2 (struct list_ *s, void (*pernode_)(void *base),  struct list_v *base, int delnums) {

  if (s && base && (delnums > 0 || (delnums == -1))) {
    struct list_v *pmv = s->set;
    int pos = list_retidx (s, base);
    if (delnums == -1)
      delnums = s->nums - pos;
    if (pos != -1) {
    __retry:
      list_remove_ISQ(s, pos, pernode_);
      if (-- delnums > 0) {
        achieve __retry;
      }    
    }
  }
}

finline
void list_remove_ISQ3 (struct list_ *s, void (*pernode_)(void *node),  void *node, int delnums) {

  if (s && node && (delnums > 0 || (delnums == -1))) {
    struct list_v *pmv = s->set;
    int pos = list_retidx2 (s, node);
    if (delnums == -1)
      delnums = s->nums - pos;
    if (pos != -1) {
    __retry:
      list_remove_ISQ(s, pos, pernode_);
      if (-- delnums > 0) {
        achieve __retry;
      }    
    }
  }
}

finline
void list_remove_ISQ3a (struct list_ *s, void (*pernode_)(void **node)) {

  if (s ) {
    while (s->nums > 0) 
      list_remove_ISQ(s, 0, pernode_);
  }
}
finline
void list_uninit_ISQ (struct list_ **s, void (*pernode_)(void *node)) {

  if (s != null && (*s != null)) {
    while ( (*s)->nums > 0) 
      list_remove_ISQ (* s, 0, pernode_);
    ufree (*s); 
    * s = null;
  }
}
finline
void list_removeALL_ISQ (struct list_ *s, void (*pernode_)(void *node)) {

  if (s != null) {
    while ( s->nums > 0) 
      list_remove_ISQ (s, 0, pernode_);
    s->set = null;
  }
}

finline
void list_uninit (struct list_ **s) {

  list_uninit_ISQ (s, null);
}

/* if predicate callback return ktrue
   then list_nodeopc0 return current list_v ptr */
finline
struct list_v *
list_nodeopc0 (struct list_ *s, kbool (* predicate)(void *ppnode, void *userdata), void *userdata) {

  if (s && predicate) {
    if (s->nums >0) {
      int ii;
      struct list_v *pv= s->set;
      for ( ii= 0; ii != s->nums; ii++) {
        kbool rwq = predicate (& pv->obj, userdata);
        if (rwq != kfalse) {
          return pv;
        }
        pv = pv->level_n;
      }
    }
  }
  return null;
}

finline
void
list_foreach (struct list_ *s, void (* predicate)(void *obj)) {

  if (s && predicate) {
    if (s->nums >0) {
      int ii;
      struct list_v *pv= s->set;
      for ( ii= 0; ii != s->nums; ii++) {
        predicate (pv->obj);
        pv = pv->level_n;
      }
    }
  }
}

finline
void
list_foreach_ (struct list_ *s, void (* predicate)(void *obj, void *udata), void *udata) {

  if (s && predicate) {
    if (s->nums >0) {
      int ii;
      struct list_v *pv= s->set;
      for ( ii= 0; ii != s->nums; ii++) {
        predicate (pv->obj, udata);
        pv = pv->level_n;
      }
    }
  }
}

finline
void
list_foreach_r_ (struct list_ *s, void (* predicate)(void *udate, void *obj), void *udata) {

  if (s && predicate) {
    if (s->nums >0) {
      int ii;
      struct list_v *pv= s->set;
      for ( ii= 0; ii != s->nums; ii++) {
        predicate (udata, pv->obj);
        pv = pv->level_n;
      }
    }
  }
}

finline
void
list_foreach2 (struct list_ *s, void (* predicate)(void **obj)) {

  if (s && predicate) {
    if (s->nums >0) {
      int ii;
      struct list_v *pv= s->set;
      for ( ii= 0; ii != s->nums; ii++) {
        predicate (& pv->obj);
        pv = pv->level_n;
      }
    }
  }
}
finline
void
list_foreach3 (struct list_ *s, void (* predicate)(void *obj)) {

  if (s && predicate) {
    if (s->nums >0) {
      int ii;
      struct list_v *pv= s->set;
      for ( ii= 0; ii != s->nums; ii++) {
        predicate (pv->obj);
        pv = pv->level_n;
      }
    }
  }
}

finline
kbool
list_foreach_interrupt (struct list_ *s, kbool (* predicate)(void *obj, void *cmpobj), void *cmpobj) {

  if (s && predicate) {
    if (s->nums >0) {
      int ii;
      struct list_v *pv= s->set;
      for ( ii= 0; ii != s->nums; ii++) {
        kbool s = predicate (pv->obj, cmpobj);
        if (s == ktrue) {
          return ktrue;
        }
        pv = pv->level_n;
      }
    }
  }
  return kfalse;
}

finline
void *
list_foreach_interrupt2 (struct list_ *s, kbool (* predicate)(void *obj, void *cmpobj), void *cmpobj) {

  if (s && predicate) {
    if (s->nums >0) {
      int ii;
      struct list_v *pv= s->set;
      for ( ii= 0; ii != s->nums; ii++) {
        kbool s = predicate (pv->obj, cmpobj);
        if (s == ktrue) {
          return pv->obj;
        }
        pv = pv->level_n;
      }
    }
  }
  return null;
}

finline 
void *
list_nodeopn0 (struct list_ *s, kbool (* predicate)(void *ppnode, void *userdata), void *userdata) {

  if (s && predicate) {
    if (s->nums >0) {
      int ii;
      struct list_v *pv= s->set;
      for ( ii= 0; ii != s->nums; ii++) {
        kbool rwq = predicate (& pv->obj, userdata);
        if (rwq != kfalse) {
          return pv->obj;
        }
        pv = pv->level_n;
      }
    }
  }
  return null;
}

finline
struct list_v *list_get2 (struct list_v *base, int pos) {

  if (base && (pos >= 0)) {
    int ii;
    struct list_v *pv = base;
    for ( ii= 0; ii != pos; ii++) {
      pv =pv->level_n;
    }
    return pv;
  }
  return null;
}

finline
void *list_getnode2 (struct list_v *base, int pos) {

  if (base && (pos >= 0)) {
    int ii;
    struct list_v *pv = base;
    for ( ii= 0; ii != pos; ii++) {
      pv =pv->level_n;
    }
    return pv->obj;
  }
  return null;
}

finline
kbool list_getnode3 (struct list_v *base, int pos, int lens, void **accepta) {

  if (base && (pos >= 0) && (lens > 0) && accepta) {
    int ii;
    for ( ii =0; ii != lens; ii++) {
      void *apts = list_getnode2 (base, pos+ ii);
      if (apts != null) {
        accepta[ii] = apts;
      } else {
        return kfalse;
      }
    }
    return ktrue;
  }
  return kfalse;
}

finline
kbool list_getlist3 (struct list_v *base, int pos, int lens, struct list_v **accepta) {

  if (base && (pos >= 0) && (lens > 0) && accepta) {
    int ii;
    for ( ii =0; ii != lens; ii++) {
      struct list_v *apts = list_get2 (base, pos+ ii);
      if (apts != null) {
        accepta[ii] = apts;
      } else {
        return kfalse;
      }
    }
    return ktrue;
  }
  return kfalse;
}

finline
void list_merge (struct list_ *base, struct list_ *sacrifice) {

  if (base && sacrifice ) {
    if (sacrifice->nums > 0) {
      if (base->nums <= 0)  {
        base->set = sacrifice->set;
      } else  {
        base->set->level_p->level_n = sacrifice->set;
        sacrifice->set->level_p->level_n = base->set;
      }
      sacrifice->set = null;
      base->nums += sacrifice->nums;
      sacrifice->nums = 0;
    }
  }
}

finline
void list_merge_dest (struct list_ *base, struct list_ **sacrifice_) {

  if (base && sacrifice_ ) {
    struct list_ *sacrifice = *sacrifice_;
    if (sacrifice_ && (sacrifice->nums > 0)) {
      if (base->nums >= 0) {
        base->nums += sacrifice->nums;
        sacrifice->nums = 0;
        base->set->level_p->level_n = sacrifice->set;
        sacrifice->set->level_p->level_n = base->set;
        sacrifice->set = null;
        list_uninit_ISQ (sacrifice_, null);
      }
    }
  }
}

/*
   not mine, but it looks pretty bad in this version.... 
 */
#define LIST_FOREACH(/* list_ * */ head, /* int*/ id_, /* list_v */ node) \
  for (    (id_) = 0, (node) = (head)->set;                                  \
           (id_) != (head)->nums;                                            \
                   (node) = (node)->level_n,  (id_)++) 

#endif 