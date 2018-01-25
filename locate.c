#include "locate.h"

 

void locate_s_copy (struct locate_s_ *to, struct locate_s_ *from) {
  memcpy (to, from, sizeof (struct locate_s_));
}

void locate_obtinit (struct locate_essen_ *essen, struct locate_s_ *ltes) {
  locate_s_copy (ltes, & essen->init);
}

void locate_saveto (struct locate_essen_ *essen, struct locate_s_ *ltes) {
  locate_s_copy (ltes, & essen->doing);
}

void locate_resumefrom (struct locate_essen_ *essen, struct locate_s_ *ltes) {
  locate_s_copy (& essen->doing, ltes);  
  essen->rdoing.apos = essen->doing.apos + essen->doing.length - 1;
  essen->rdoing.length = ltes->length;
}

void locate_resumefrom_baseblen (struct locate_essen_ *essen, int pos_left) {
  essen->doing.apos = pos_left;
  essen->doing.length = essen->buf->length - pos_left + 1;
  essen->rdoing.apos = essen->doing.apos + essen->doing.length - 1;
  essen->rdoing.length = essen->doing.length;
}

void locate_resumefrom2 (struct locate_essen_ *essen, int pos_left, int pos_right) {
  essen->doing.apos = pos_left;
  essen->doing.length = pos_right - pos_left + 1;
  essen->rdoing.apos = essen->doing.apos + essen->doing.length - 1;
  essen->rdoing.length = essen->doing.length;
}

void locate_attach (struct locate_essen_ *essen, struct uchar *analysis, int line) {
  essen->buf = analysis;
  essen->lu = essen->buf->str;
  essen->init.length = analysis->length;
  essen->init.apos = 0;
  essen->line = line;
  essen->upos = 0;
  essen->at = null;
  essen->doing.length = analysis->length;
  essen->doing.apos = 0;
  essen->rdoing.apos = analysis->length - 1;
  essen->rdoing.length = analysis->length;
  locate_s_copy (& essen->doing, & essen->init);
}

void locate_attach_weak (struct locate_essen_ *essen, struct uchar *analysis, struct locate_s_ *es, int line) {
  essen->buf = analysis;
  essen->lu = null;
  essen->line = line;
  essen->upos = 0;
  essen->at = null;
  essen->init.length = analysis->length;
  essen->init.apos = 0;
  essen->doing.length = analysis->length;
  essen->doing.apos = 0;
  essen->rdoing.apos = analysis->length - 1;
  essen->rdoing.length = analysis->length;
  if (es != null)  {
    locate_s_copy (& essen->doing, es);
    essen->rdoing.apos = analysis->length - 1;
    essen->rdoing.length = analysis->length;
  }
}

int locate_set (struct locate_essen_ *essen, int pos) {
  int s = -1;
  if (pos <= (essen->init.length - 1)) {
    essen->doing.apos = pos;
    essen->doing.length = essen->init.length - pos;
    s = 0;
  }
  return s;
}

int locate_promote (struct locate_essen_ *essen, int delta) {
  int s = ESS_FAILED;
  int c = essen->doing.length - delta;
  if (c == 0)
    s = ESS_COMPELTE;
  else if (c > 0)
    s = ESS_OK;
  if (s != ESS_FAILED) {
    essen->doing.apos += delta;
    essen->doing.length = c;
  }
  return s;
}

int locate_getcc (struct locate_essen_ *essen, us *cc) {
  int s = -1;
  int c = essen->doing.length - 1;
  if (c >= 0)
    s = 0;
  if (s != -1) {
    if (cc != null)
      * cc= essen->buf->str[essen->doing.apos];
    essen->doing.apos += 1;
    essen->doing.length--;
    essen->rdoing.length --;
  }
  return s;
}

int locate_match_sym (struct locate_essen_ *essen, us cc, int *pstart, int *pend) {
  int s = -1;
  int pst;
  int pse;
  struct locate_s_ ls;
  us cc__;
  pst = essen->doing.apos;
  locate_saveto (essen, & ls);
  while (locate_getcc (essen, & cc__) == 0) {
    if (cc__ == cc) {
      pse = essen->doing.apos - 1;
      if (pstart != null)
        *pstart = pst;
      if (pend != null)
        * pend = pse;
      return 0;
    }
  }
  locate_resumefrom (essen, & ls);
  return -1;
}

int locate_match_sym_pure (struct locate_essen_ *essen, us cc, int *pstart, int *pend) {
  int s = -1;
  int pst;
  int pse;
  struct locate_s_ ls;
  us cc__;
  pst = essen->doing.apos;
  locate_saveto (essen, & ls);
  while (locate_getcc (essen, & cc__) == 0) {
    if (cc__ == cc) {
      pse = essen->doing.apos - 1;
      if (pstart != null)
        *pstart = pst;
      if (pend != null)
        * pend = pse;
      return 0;
    } else if (cc__ == _GC (' ') || (cc__ == _GC ('\t'))) {
      continue;
    } else {
      break;
    }
  }
  locate_resumefrom (essen, & ls);
  return -1;
}

int locate_match_var_pure (struct locate_essen_ *essen, struct uchar **u) {
  int s = -1;
  int pst;
  int pse;
  struct locate_s_ ls;
  struct locate_desc_ ld[1];
  ld[0].etype = LOCATE_TYPE_VARIABLE;

  if (locate_searchp (essen, & ld[0], 1, null) == 0) {
    if ((*u) == null)
      uchar_init (u);
    uchar_clearbuf (*u);
    uchar_assign(*u, ld[0].acceptORacc, ld[0].ac_lens);
    return 0;
  }
  return -1;
}

int locate_noremain (struct locate_essen_ *essen) {
  if (essen->doing.length - 1 >= 0)
    return -1;
  return 0;
}

int locate_getrcc (struct locate_essen_ *essen, us *cc) {
  int s = -1;
  int c = essen->rdoing.length - 1;
  if (c >= 0)
    s = 0;
  if (s != -1) {
    if (cc != null)
      * cc= essen->buf->str[essen->rdoing.apos];
    essen->rdoing.apos -= 1;
    essen->rdoing.length --;
    essen->doing.length --;
  }
  return s;
}

void locate_backupcc (struct locate_essen_ *essen) {
  essen->doing.apos--;
  essen->doing.length++;
  essen->rdoing.length++;
  if (essen->doing.apos < 0) {
    essen->doing.apos++;
    essen->doing.length--;
    essen->rdoing.length--;
  }
}

void locate_backuprcc (struct locate_essen_ *essen) {
  essen->rdoing.apos++;
  essen->rdoing.length++;
  essen->doing.length++;
  if (essen->doing.apos >= essen->init.length) {
    essen->rdoing.apos--;
    essen->rdoing.length--;
    essen->doing.length--;
  }
}

us locate_throwtoken (struct locate_essen_ *essen) {
  return essen->buf->str[essen->doing.apos];
}

void locate_settoken2 (struct locate_essen_ *essen, int offrel, us cc) {
  essen->buf->str[essen->doing.apos+offrel] = cc;
}

us *locate_throwref (struct locate_essen_ *essen, int pos) {
  return & essen->buf->str[pos];
}

int locate_uselessdata (struct locate_essen_ *essen) {
  us cc;
  int sig =0;
  struct locate_s_ ltes;
  locate_saveto (essen, & ltes);

  for (; locate_getrcc (essen, & cc) == 0;) {
    if (cc == _GC (' ') || cc == _GC ('\t'))
      continue  ;
    else  { sig = -1; break; }
  }
  locate_resumefrom(essen, & ltes);
  return sig;
}

int locate_uselessdata2 (struct locate_essen_ *essen, int left, int right) {
  us cc;
  int sig =0;
  struct locate_s_ ltes;
  struct locate_s_ ltes2;
  locate_saveto (essen, & ltes);
  locate_resumefrom2 (essen, left, right);

  for (; locate_getrcc (essen, & cc) == 0;) {
    if (cc == _GC (' ') || cc == _GC ('\t'))
      continue  ;
    else  { sig = -1; break; }
  }
  locate_resumefrom(essen, & ltes);
  return sig;
}


us *locate_throwref2 (struct locate_essen_ *essen, struct locate_desc_ *ltes) {
  return & essen->buf->str[ltes->ac_pos];
}

us *locate_throwref3 (struct locate_essen_ *essen, int offrel) {
  return &   essen->buf->str[essen->doing.apos+offrel];
}

int locate_current (struct locate_essen_ *essen, int offrel) {
  return (essen->doing.apos+offrel);
}

us locate_setnullc (struct locate_desc_ *ld) {
  us cc;
  cc  = ld->acceptORacc[ld->ac_lens];
  ld->acceptORacc[ld->ac_lens] = 0;
  return cc;
}

void locate_setnullq (struct locate_desc_ *ld, us cc) {
  ld->acceptORacc[ld->ac_lens] = cc;
}


us locate_throwtoken2 (struct locate_essen_ *essen, int pos) {
  return essen->buf->str[pos];
}

struct _pd_disp {
  int pos;
  int pos_vailedh;
  int len;
};

static 
int U_serach (enum locate_type type, struct locate_essen_ *essen, 
              struct locate_desc_ *U,
              struct locate_s_ *seb, struct _pd_disp *pd)
{
  // serach NUMBER|VAR|SYMBOLS|STRING 
  struct locate_s_ u0;
  int L;
  int R;
  int S;

  int Q;
  int C;
  int G;
  int I = _GC ('#');
  us cc;
  // mark current pos 
  locate_saveto (essen, & u0);
  S = essen->doing.apos;
  L = R = -1;

  Q = 0;
  G = 0;
  C = U->ac_lens;
  
  for (; locate_getcc (essen, & cc) == 0; S++) {

    switch (type)  {

    case  LOCATE_TYPE_ACCURATE_RELAXED:
    case  LOCATE_TYPE_ACCURATE_TIGHTLY:
      if (cc  ==  U->acceptORacc[Q])
      {
        R = S;
        C --;
        Q ++;

        if (L == -1) 
        {
          L = S; 
          U->pt_pos = L;
        }
        if (C == 0)
        {
          achieve L9;
        }
      }
      else if (  ! ((cc == _GC (' ') || cc == _GC ('\t') ) 
                && ( (type == LOCATE_TYPE_ACCURATE_RELAXED) 
                       ? ktrue 
                       : L == -1)) )
      {
        return  -1;
      }
      continue  ;
      
    case  LOCATE_TYPE_VARIABLE:
    case  LOCATE_TYPE_NUMBER:
    case  LOCATE_TYPE_STRING:
    case  LOCATE_TYPE_ALPHA:
      if ( (cc == _GC (' ') || cc == _GC ('\t') ) && L == -1) 
        continue ;
      if (type != LOCATE_TYPE_STRING) {

        if ( ((type == LOCATE_TYPE_NUMBER) && (iswdigit((wint_t) cc) > 0))
          ||(type == LOCATE_TYPE_ALPHA 
          &&     (iswalpha((wint_t) cc) > 0))
          ||      (type == LOCATE_TYPE_VARIABLE 
               &&     (iswalpha((wint_t) cc) > 0 || cc == _GC('_'))
                  ||  (iswdigit((wint_t) cc) > 0 && L != -1)) )
        {
          if (L == -1)
          {
            L = S;
            if (type == LOCATE_TYPE_ALPHA) {
              R = S ;
              U->alpha = cc;
              achieve L8;
            }
          }
          R = S ;
          continue  ;
        }

        if (L != -1) 
        {
          locate_backupcc (essen);

     L8:  U->ac_pos = L;
          U->ac_lens = R - L + 1;
          U->acceptORacc = & essen->buf->str[L];
     L9:  if (pd != null) 
          {
            pd->pos_vailedh = L;
            pd->pos = u0.apos;
            pd->len = essen->doing.apos - pd->pos;
          }
          return 0;
        }
        return -1;
      } 
        // string  . 
      if ( cc == _GC ('"') || cc == _GC ('\'') || cc == _GC ('`')) {
        if (L == -1) {
          L = S;   // mark left qute 
          I = cc;
          continue  ;
        } else {
          if (cc != I)
            achieve L4;
          // serach left, exist \?? 
          R = S;
          U->acceptORacc = & essen->buf->str[L+1];
          U->ac_pos = 0;
          U->ac_lens = Q;
          achieve L9;
        }
      } else if (L == -1) {
        return -1;
      } else if (1)
L4:  // U->acceptORacc[Q++] = cc;
        Q ++;
        continue ;
    }
  }
  switch (type)  {
  case  LOCATE_TYPE_VARIABLE:
  case  LOCATE_TYPE_NUMBER:
    if (L == -1)
  case  LOCATE_TYPE_ACCURATE_RELAXED:
  case  LOCATE_TYPE_ACCURATE_TIGHTLY:
  case  LOCATE_TYPE_STRING: 
      return -1;
    achieve L8 ;
  }
  return -1;
}

# if 0
int main (void) {

  struct uchar *s;
  struct locate_essen_ sek;
  struct locate_essen_ bb;
  struct _pd_disp p;
  struct locate_desc_ Ud;
  int ii = 0;
  int cc ;
  int m;
  us aabuf[ 1024];
  us bbbuf[ 512 ];
  memset (aabuf, 0, sizeof (aabuf));
  uchar_ginit ();
  uchar_init (&s); 
  uchar_assign3 (s, _GR ("    0   1   2   "));
                      //  " ASSAAS\"
  //                      " ASSAAS""
                      //  0123456789
  useek_Create (& sek, s, aabuf, 3 );
  Ud.acceptptr = & m;
  Ud.ac_lens = 3;
  Ud.acceptORacc = _GR ("012");
  ii = U_serach (LOCATE_TYPE_ACCURATE_RELAXED, & sek, &Ud, & bb, & p);
  wprintf (aabuf);
  uchar_guninit ();
  return 0;

}
# endif 



static //   LOCATE_TYPE_SYMBOL_TIGHTLY | LOCATE_TYPE_SYMBOL_RELAXED,
int V_serach (enum locate_type type, struct locate_essen_ *essen, 
              struct locate_desc_ *U,
              struct locate_s_ *seb, struct _pd_disp *pd)
{
  int id = 0;
  int L= -1;
  int S= -1;
  enum locate_type type2;
  struct locate_s_ u2, u0;
  // mark current pos 
  locate_saveto (essen, & u2);
  locate_saveto (essen, & u0);
  U->sig_match = 0;
  U->sig_exmatch = 0;

  // try match 
  if  ( type == LOCATE_TYPE_SYMBOL_TIGHTLY)
    type2 = LOCATE_TYPE_ACCURATE_TIGHTLY ;
  else if ( type == LOCATE_TYPE_SYMBOL_RELAXED)
    type2 = LOCATE_TYPE_ACCURATE_RELAXED ;
  else assert (0) ;

  switch (U->sig_ctl)  {
  default : assert (0);
  case USIG_AUTO_RANGE_: 
  case USIG_AUTO_TRUNC:
  case USIG_AUTO_TRUNC_INCLUED0:  S = 0;
  case USIG_AUTO_EQUAL:
    
    while (TRUE) {
      if (U->sig_ctl == USIG_AUTO_EQUAL && id == U->sig_setnums) {
        if (U->sig_get == UGET_STOP) {
          U->sig_match = id;
          achieve L0;
        }
        S = 0;
      }
      if (U_serach (type2, essen, U, seb, pd) == 0) {
        id++;
        locate_saveto (essen, & u2);

        if (pd != null) {
          if (L == -1)
            L = pd->pos_vailedh;
        }
      } else {
        locate_resumefrom (essen, & u2);
        if (type != USIG_AUTO_EQUAL)
          U->sig_match = id ; 
        else 
          U->sig_exmatch = id - U->sig_match;
        if ( (S == -1)
          || (id == 0 && U->sig_ctl == USIG_AUTO_TRUNC)
          || (U->sig_ctl == USIG_AUTO_RANGE_ && ! (id > U->sig_setnums && id < U->sig_setnums_hi))) 
          return -1; 
L0:     if (pd != null) {
          pd->pos_vailedh = (L == -1)?u0.apos:L;
          pd->pos = u0.apos;
          pd->len = essen->doing.apos - pd->pos;
        }
        return 0;
      }
    }
  }
  return -1;
}

// not alpha|numb|_|' '|\t 
static 
kbool issemiotic (us cc)
{
  /* if  (cc == )
  if ( ((type == LOCATE_TYPE_NUMBER) && (iswdigit((wint_t) cc) > 0))
    ||(type == LOCATE_TYPE_ALPHA 
    &&     (iswalpha((wint_t) cc) > 0))
    ||      (type == LOCATE_TYPE_VARIABLE 
    &&     (iswalpha((wint_t) cc) > 0 || cc == _GC('_'))
    ||  (iswdigit((wint_t) cc) > 0 && L != -1)) )  */
}





static 
  int T_serach2 (enum locate_type type, struct locate_essen_ *essen, 
struct locate_desc_ *U,
struct locate_s_ *seb, struct _pd_disp *pd)
{
  int id;
  struct locate_s_ ls;
  locate_saveto (essen, & ls);

  for (id = 0; id != U->sig_setnums; id++)   {
    struct locate_desc_ ld;

    ld.etype = LOCATE_TYPE_VARIABLE;

    if (locate_searchp(essen, (void *)&ld, 1, null) == 0)  {
      if (ld.ac_lens == wcslen (U->dmtach_blk[id])
        && (!wcsncmp (ld.acceptORacc, U->dmtach_blk[id], ld.ac_lens))) {
        U->sig_match = id;
      return 0;
      } 
      locate_resumefrom (essen, & ls);
    }
  }
  locate_resumefrom (essen, & ls);
  return -1;
}

static 
int T_serach (enum locate_type type, struct locate_essen_ *essen, 
              struct locate_desc_ *U,
              struct locate_s_ *seb, struct _pd_disp *pd)
{
  int id;
  for (id = 0; id != U->sig_setnums; id++)   {
    struct locate_desc_ ld[1];
    LOCATE_SETACC_TIGHTLY_CONST (& ld[0], U->dmtach_blk[id]);
    if (locate_searchp(essen, & ld[0], 1, null) == 0)  {
      U->sig_match = id;
      return 0;
    }
  }
  return -1;
}

int locate_searchp (struct locate_essen_ *essen, struct locate_desc_ *serg, int nums, struct locate_s_ *gpc) {

  int id = 0;
  int L= -1;
  int S= -1;
  int T= 0;
  int C= essen->upos;
  struct locate_s_ u0, u2;
  struct _pd_disp pd;
  // mark current pos 
  locate_saveto (essen, & u0);
  locate_saveto (essen, & u2);
  // U->sig_match = 0;
  // U->sig_exmatch = 0;
  for (id = 0; id != nums; id++)  {
    struct locate_desc_ *ls = & serg[id];
    if (ls->etype == LOCATE_TYPE_STRING)
      ls->acceptORacc = & essen->buf->str[essen->doing.apos];
    if (ls->etype == LOCATE_TYPE_SYMBOL_TIGHTLY || ls->etype == LOCATE_TYPE_SYMBOL_RELAXED)
      S = V_serach (ls->etype, essen, ls, & u0, & pd);
    else if (ls->etype != LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY && (ls->etype != LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7))
      S = U_serach (ls->etype, essen, ls, & u0, & pd);
    else if (ls->etype == LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY)
      S = T_serach (ls->etype, essen, ls, & u0, & pd);
    else if (ls->etype == LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7)
      S = T_serach2 (ls->etype, essen, ls, & u0, & pd);

    if (S == -1) {
      locate_resumefrom (essen, & u2);
      essen->upos = C ;
      return -1;
    }
    if (ls->etype == LOCATE_TYPE_STRING) 
      essen->upos += ls->ac_lens;
  }
  if (gpc != null)  {
    gpc->apos = u2.apos;
    gpc->length = essen->doing.apos - u2.apos;
  }
  return 0;
}

int locate_searchp2 (struct locate_essen_ *essen, struct locate_desc_ *serg, int nums, struct locate_s_ *gpc, int *fvail) {

  int id = 0;
  int L= -1;
  int S= -1;
  int T= 0;
  int C= essen->upos;
  struct locate_s_ u0, u2;
  struct _pd_disp pd;
  // mark current pos 
  locate_saveto (essen, & u0);
  locate_saveto (essen, & u2);
  // U->sig_match = 0;
  // U->sig_exmatch = 0;
  for (id = 0; id != nums; id++)  {
    struct locate_desc_ *ls = & serg[id];
    if (ls->etype == LOCATE_TYPE_STRING)
      ls->acceptORacc = & essen->buf->str[essen->doing.apos];
    if (ls->etype == LOCATE_TYPE_SYMBOL_TIGHTLY || ls->etype == LOCATE_TYPE_SYMBOL_RELAXED)
      S = V_serach (ls->etype, essen, ls, & u0, & pd);
    else if (ls->etype != LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY && (ls->etype != LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7))
      S = U_serach (ls->etype, essen, ls, & u0, & pd);
    else if (ls->etype == LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY)
      S = T_serach (ls->etype, essen, ls, & u0, & pd);
    else if (ls->etype == LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7)
      S = T_serach2 (ls->etype, essen, ls, & u0, & pd);

    if (S == -1) {
      locate_resumefrom (essen, & u2);
      essen->upos = C ;
      return -1;
    }
    if (ls->etype == LOCATE_TYPE_STRING) 
      essen->upos += ls->ac_lens;
    if (L == -1)
      L = pd.pos_vailedh;
  }
  if (gpc != null)  {
    gpc->apos = u2.apos;
    gpc->length = essen->doing.apos - u2.apos;
  }
  if (fvail != null)  {
    * fvail = L;
  }
  return 0;
}
int locate_clearrc (struct locate_essen_ *essen) {
  // clear right space and comment
  struct locate_s_ ltes, ltes2 ;
  int S = 0;
  us cc2 = _GC ('#');
  us cc = _GC ('#');
  locate_saveto (essen, & ltes);
  locate_saveto (essen, & ltes2);
  for (; locate_getcc (essen, & cc) == 0;) {
    if (cc == _GC ('@') && cc2 == _GC ('@')) {
      // comment clear right . 
      int ac= ltes.length - 2 - essen->doing.length;
      if (ac <= 0)
        return 0 ; // FIXME:no set 
      ltes.length = ac;
      locate_resumefrom (essen, & ltes);
      achieve L0 ;
    }
    cc2 = cc; // update cache 
  }
L0:
  locate_resumefrom (essen, & ltes);
  // clear right space 
  for (; (S = locate_getrcc (essen, & cc)) == 0;) {
    if (cc == _GC (' ') || cc == _GC ('\t'))
      continue  ;
    else  break  ;
  }
  if (S != 0)
    return 0 ;

  locate_backuprcc (essen);
  return -1 ;
}





#if 0
# if 1
int main (void) {

  struct uchar *s;
  struct locate_essen_ sek;
  struct locate_essen_ bb;
  struct _pd_disp p;
  struct locate_desc_ Ud[3];
  int ii = 0;
  us aabuf[ 1024];
  memset (aabuf, 0, sizeof (aabuf));
  uchar_ginit ();
  uchar_init (&s); 
  uchar_assign3 (s, _GR ("          @@                            "));
  //  " ASSAAS\"
  //                      " ASSAAS""
  //  0123456789
  locate_attach (& sek, s, 300, aabuf, 3 );
  locate_clearrc (& sek);
  uchar_guninit ();
  return 0;

}
# endif
#endif 
