# include "uchar.h"
# include "cas.h"
# include "list.h"

/*
  simple free-list
 */
struct uchar_cache_link {
  struct uchar_cache_link *level;
  struct uchar *cache;
};
struct uchar_cache {
  struct uchar_cache_link *link;
  struct uchar_cache_link *dummy_head; /* only link node is empty */
#  if defined (_DEBUG)
  struct list_ *chkmem_list; /* dlink for check mem */
#  endif 
  int pure_chunk_nums;
  int dummy_chunk_nums;
  int32_t mspin_lock;
};
static struct
uchar_cache *g_cache_link = null;

void uchar_ginit (void) {
  g_cache_link = umalloc (sizeof (struct uchar_cache));
  g_cache_link->link = null;
  g_cache_link->dummy_head = null;
  g_cache_link->mspin_lock = 0;
  g_cache_link->dummy_chunk_nums = 0;
  g_cache_link->pure_chunk_nums = 0;
#  if defined (_DEBUG)
  list_init (& g_cache_link->chkmem_list);
#  endif 
}
static
void uchar_g0 (int nums, struct uchar_cache_link *link_head) {
  if (nums > 0 && (link_head != null)) {
    int id = nums;
    int g;
    struct uchar_cache_link *cclink = link_head;
    struct uchar_cache_link *cccache;
    for (g = 0; g != id; cclink = cccache, g++) {
      struct uchar *uc = cclink->cache;
      cccache = cclink->level;
      if (uc != null) {
        if (uc->str != null)
          ufree (uc->str);
        ufree (uc);
        ufree (cclink);
      }
    }
  }
}
void uchar_guninit (void) {
  if (g_cache_link) {
    uchar_g0 (g_cache_link->pure_chunk_nums, g_cache_link->link);
    uchar_g0 (g_cache_link->dummy_chunk_nums, g_cache_link->dummy_head);
# if defined (_DEBUG)
    list_uninit_ISQ(& g_cache_link->chkmem_list, null);
# endif
    ufree (g_cache_link);
    g_cache_link = null;
  }
}
struct uchar *
uchar_malloc (void)
{
  struct uchar *lc_cache;
  while (store32 (& g_cache_link->mspin_lock, 1) != 0)
    _mm_pause ();
  if (g_cache_link->pure_chunk_nums > 0) {
    struct uchar_cache_link *next_cache;
    struct uchar_cache_link *cur_cache;
    g_cache_link->pure_chunk_nums--;
    /* remove pure chunk's node */
    next_cache = g_cache_link->link->level;
    cur_cache = g_cache_link->link;
    g_cache_link->link = next_cache;
    lc_cache = cur_cache->cache;
    cur_cache->cache = null;
    /* list body drop into the dummy list **/
    cur_cache->level = g_cache_link->dummy_head;
    g_cache_link->dummy_head = cur_cache;
    g_cache_link->dummy_chunk_nums++;
  } else {
    lc_cache = umalloc (sizeof (struct uchar) );
    lc_cache->maxbuf = DEFAULT_UCHAR_INIT_SIZE;
    lc_cache->str = ucalloc ( sizeof (* lc_cache->str) * DEFAULT_UCHAR_INIT_SIZE + 2, 1);
  }
  lc_cache->length = 0;
  /* manager pointer */
# if defined (_DEBUG)
  list_insert_tail (g_cache_link->chkmem_list, lc_cache);
# endif 
  store32 (& g_cache_link->mspin_lock, 0);
  return lc_cache;
}

void uchar_makepure_temp_fromstack (struct uchar *u, us *cont, int cnlen) {
  u->str = cont;
  u->length = 0;
  u->maxbuf = cnlen;
}

void uchar_init (struct uchar **u) {

  struct uchar *pt = uchar_malloc ();
  *u = pt;
}

void uchar_copy (struct uchar **u, struct uchar *v)  {
  if (u != null)  {
    if (*u == null) 
      uchar_init (u);
    uchar_assign0 (*u, v);
  }
}

void uchar_clearbuf (struct uchar *u) {

  if (u) {
    u->length = 0;
  }
}

void uchar_uninit (struct uchar **u) {

  if (u != null && *u != null) {
    struct list_v *p;
    struct uchar *pt = * u;
    int id;
    * u = null;
    /* not remove, drop to cache */
    while (store32 (& g_cache_link->mspin_lock, 1) != 0)
      _mm_pause ();
    if (  1 ) {
# if defined (_DEBUG)
    /* check memvailed */
      LIST_FOREACH (g_cache_link->chkmem_list, id, p) {
        if (p->obj == pt) {
# endif 
          struct uchar_cache_link *link_body = null;
          /* try get link body from dummy list */
          if (g_cache_link->dummy_chunk_nums > 0) {
            -- g_cache_link->dummy_chunk_nums;
            link_body = g_cache_link->dummy_head;
            g_cache_link->dummy_head = g_cache_link->dummy_head->level;
          } else link_body = umalloc (sizeof (struct uchar_cache_link));
          link_body->cache = pt;
          link_body->level = g_cache_link->link;
          g_cache_link->link = link_body;
          ++ g_cache_link->pure_chunk_nums;
#  if defined (_DEBUG)
          list_remove_ISQ_L (g_cache_link->chkmem_list, p, null);
#  endif 
          store32 (& g_cache_link->mspin_lock, 0);
          return ;
# if defined (_DEBUG)
        }
      }
# endif 
      /* illegal free memory */
      assert (0);
    }
  }
}

void uchar_insert (struct uchar *u, int pos, us *cont, int len) {

  if (cont != null && (len > 0) && (u)) {
    int total = u->length + len;
    if (pos == -1) {
      pos = u->length;
    }
    if (total >= u->maxbuf) {
      /* must alloc new memory */
      u->maxbuf = total + 16;
      u->str = urealloc (u->str, (u->maxbuf + 1) * sizeof (us));
    }
    if (pos >= u->length) {
      /* insert imm */ 
      memcpy (& u->str[u->length], cont, len * sizeof (us));
    } else {
      /* copy temp, save data */
      int frs = (u->length - pos) * sizeof (us);
      memcpy (& u->str[pos+len], & u->str[pos], frs);
      memcpy (& u->str[pos], cont, len* sizeof (us));
    }
    u->length = total;
    u->str[total] = 0;
  }
}

void uchar_insert8 (struct uchar *u, int pos, struct uchar *v) {

  uchar_insert (u, pos, v->str, v->length);
}

void uchar_expand (struct uchar *u, int nums) {

  if (u != null && (nums > 0)) {
    u->maxbuf = u->maxbuf + nums + 16;
    u->str = urealloc (u->str, (u->maxbuf + 1) * sizeof (us));
  }
}

void uchar_insert3 (struct uchar *u, int pos, us *cont) {

  uchar_insert (u, pos, cont, wcslen (cont));
}

void uchar_insert4 (struct uchar *u, int pos, struct uchar *v) {

  uchar_insert (u, pos, v->str, v->length);
}

void uchar_remove (struct uchar *u, int pos, int len) {

  if (len > 0 && (u) && (u->length > 0) && (pos < u->length)) {
    int s_total;
    if (pos == -1) {
      pos = u->length - 1;
    }

    s_total = pos +len;
    if (s_total >= u->length) {
      len = u->length -pos;
      s_total = pos +len;
    }
    memcpy (& u->str[pos], & u->str[s_total], (u->length - s_total) * sizeof (us));

    u->length -= len;
    u->str[u->length] = 0;
  }
}

void uchar_replace (struct uchar *u, int pos, int len, us *cont, int cnlen) {

  uchar_remove (u, pos, len);
  uchar_insert (u, pos, cont, cnlen);
}

void uchar_replace7 (struct uchar *u, int pos, int len, uconst us *cont) {

  uchar_remove (u, pos, len);
  uchar_insert (u, pos, cont, wcslen (cont));
}

void uchar_assign0 (struct uchar *u, struct uchar *v) {
  if (u && v) {
    if (v->length > 0) {
      uchar_assign2 (u, v->str);
    }
  }
}

void uchar_shrink (struct uchar *u, int nums) {
  if (u && (u->length > 0)) {
    if (nums == -1) 
      u->length = 0;
    else if (nums <= u->length )
      u->length -= nums;
    else ;
    u->str[u->length] = 0;
  }
}

void uchar_setlens (struct uchar *u, int nums) {
  if (u) {
    if (nums == -1) 
      u->length = 0;
    else
      u->length = nums;
    u->str[u->length] = 0;
  }
}

void uchar_assign (struct uchar *u, us *cont, int len) {

  if  (u == null || cont == null)
    return;

  if (cont != null && (len > 0)) {
    int total = len;
    if (total >= u->maxbuf) {
      /* must alloc new memory **/
      u->maxbuf = total + 16;
      u->str = urealloc (u->str, (u->maxbuf + 1) * sizeof (us));
    }
    memcpy (& u->str[0], cont, len * sizeof (us));
    u->length = total;
    u->str[total] = 0;
  }
}

void uchar_assign2 (struct uchar *u, uconst us *cont) {

  if (cont && u)
  {
    uchar_assign (u, (us *) cont, wcslen (cont));
  }
}

int uchar_same_inchain (LIST_TYPE2_(uchar) list, struct uchar *uca)  {
  if (uca != null)  {
    if (list != null)  {
      struct list_v *p;
      int id;
      LIST_FOREACH (list, id, p)  {
        if (p->uchar_o != null)  {
           if (p->uchar_o->length == uca->length)  {
             if (! wcsncmp (p->uchar_o->str, uca->str, uca->length))
               return  0;
             else  ;
           }
        }
      }
    }
  }
  return -1;
}

void /*__cdecl */
uchar_assign3 (struct uchar *u, uconst us *format, ...) {

  if (u != null) {
    us wtempbuf[2048];
    int vnums ;
    va_list args ;
    va_start (args, format) ;
    vnums = wvsprintfW (wtempbuf, format, args);
    va_end(args);
    if (vnums >= 0) {
      uchar_assign (u, wtempbuf, vnums);
    }
  }
}
/* __cdecl */
void uchar_insert7 (struct uchar *u, int pos, uconst us *format, ...) {

  if (u != null) {
    us wtempbuf[2048];
    int vnums ;
    va_list args ;
    va_start (args, format) ;
    vnums = wvsprintfW (wtempbuf, format, args);
    va_end(args);
    if (vnums >= 0) {
      uchar_insert (u, pos, wtempbuf, vnums);
    }
  }
}

void uchar_initandopy (struct uchar **u, struct uchar *copy) {

  uchar_init (u);
  uchar_insert (*u, 0, copy->str, copy->length);
}

int uchar_cmp2 (struct uchar *u, uconst us* cont) {

  if (cont && u) {
    if (u->str && (u->length >= 0)) {
      int n = wcslen (cont);
      if (n == u->length) {
        return wcsncmp (u->str, cont, n);
      }
    }
  }
  return -1;
}

int uchar_cmp (struct uchar *u, struct uchar *v) {
  if (u == null)
    return -1;
  if (v == null)
    return -1;
  if (u->length==0 && (u->length == 0))
    return 0;
  return uchar_cmp2 (u, v->str);
}

int uchar_cmp4 (struct uchar *u, struct uchar *v, int n) {
  if (u == null)
    return -1;
  if (v == null)
    return -1;
  if (v->str == null)
    return -1;
  return uchar_cmp3 (u, v->str, n);
}

int uchar_cmp3 (struct uchar *u, us* cont, int ch_len) {

  if (cont && u) {
    if (u->str && (u->length >= 0)) {
      // if (ch_len == u->length) {
        return wcsncmp (u->str, cont, ch_len);
      // }
    }
  }
  return -1;
}

int uchar_cmp3maxs (struct uchar *u, us* cont, int ch_len) {

  if (cont && u) {
    if (u->str && (u->length >= 0)) {
      if (ch_len == u->length) {
        return wcsncmp (u->str, cont, ch_len);
      }
    }
  }
  return -1;
}

void uchar_merge_andfreetail (struct uchar *uhead, struct uchar **utail) {

  assert (uhead != null);
  assert (utail != null);
  assert (*utail != null);

  uchar_setnull(*utail);
  uchar_insert3(uhead, -1, (*utail)->str);
  uchar_uninit (utail );
}

/* easy test*/

# if 0

int main (void)
{
  struct uchar *ppc;
  int id;

  uchar_ginit ();
  
  uchar_init (& ppc);
  uchar_assign2(ppc, _GR("HELLO WORLD"));
  uchar_insert3(ppc, -1, _GR(" GOODBYE WORLD "));

  uchar_uninit (& ppc);
  uchar_guninit();
  leak_infos_get();
  return 0;
}

#endif

/* easy test*/

# if 0

int main (void)
{
  struct uchar *ppc[120];
  int id;

  uchar_ginit ();
  for (id = 0; id != 120; id++)
    uchar_init (& ppc[id]);
  for (id = 0; id != 120; id++)
    uchar_uninit (& ppc[id]);
  for (id = 0; id != 60; id++)
    uchar_init (& ppc[id]);
  for (id = 0; id != 60; id++)
    uchar_uninit (& ppc[id]);
  uchar_guninit();
  leak_infos_get();
  return 0;
}

#endif

/* easy test[old]*/

# if 0

int main (void)
{
  struct uchar *ppc  = NULL;
  uchar_ginit ();
  uchar_init( & ppc);

  uchar_insert (ppc, 0, L"Hello ", 6);
  uchar_insert (ppc, 1, L"Goodbye ", 8);
  uchar_replace (ppc, 0, 14, L"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS", 20);
  // uchar_insert (ppc, 4, L"World", 5);

  // sma_strw_remove (ppc, 6, 4);
  uchar_replace (ppc, 1, 3, L"SSSSSSSSSS", 0);
  uchar_uninit (& ppc);
  uchar_guninit();
  leak_infos_get();
  return 0;
}

#endif 