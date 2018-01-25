# include "pattern.h"
# include "locate.h"
# include "list.h"
# include "path.h"
# include "analysis.h"
# include "file.h"

match_result
catch_string (struct stringv_desc *st_desc,
                  struct locate_essen_ *essen,
                  struct pass_node *self,
                  struct analysis *lysis,
                  struct uchar *err) 
{
  struct locate_s_ ls;
  struct locate_desc_ ld[3];

  ld[0].etype = LOCATE_TYPE_VARIABLE;
  ld[2].etype = LOCATE_TYPE_VARIABLE;
  LOCATE_SETACC_RELAXED_CONST (& ld[1], _GR ("."));

  /* check is passname.stringVar */
  locate_saveto (essen, & ls);
  st_desc->pvarc = self->varChain;
  st_desc->strvar = null;

  if (locate_searchp (essen, & ld[0], 3, null) == 0) {
    /* check pass and stringVar */
    st_desc->pvarc = varChain_findg2 (lysis->gsym, ld[0].acceptORacc, ld[0].ac_lens);
    if (st_desc->pvarc == null) {
      essen->buf->str[ld[0].ac_pos+ld[0].ac_lens] = 0;
      uchar_assign3(err, _GR("analysis-error:[%d?%ls] !no exist pass or serialize pass \r\n"), 
        essen->line, 
        & essen->buf->str[ld[0].ac_pos]);
      return MATCH_FATAL_ERROR;
    }
    essen->buf->str[ld[2].ac_pos+ld[2].ac_lens] = 0;
    achieve L0;
  }
  /* check is stringVar */
  if (locate_searchp (essen, & ld[2], 1, null) == 0) {
L0: st_desc->strvar = stringVar_find2 (st_desc->pvarc, ld[2].acceptORacc, ld[2].ac_lens);
    if (st_desc->strvar == null) {
      locate_resumefrom (essen, & ls);
        return MATCH_SFALSE;
    }
    return MATCH_FIND;
  }
  locate_resumefrom (essen, & ls);
  return MATCH_SFALSE;
}
 
match_result
catch_set (struct setv_desc *st_desc,
                  struct locate_essen_ *essen,
                  struct pass_node *self,
                  struct analysis *lysis,
                  struct uchar *err) 
{
  struct locate_s_ ls;
  struct locate_desc_ ld[3];

  ld[0].etype = LOCATE_TYPE_VARIABLE;
  ld[2].etype = LOCATE_TYPE_VARIABLE;
  LOCATE_SETACC_RELAXED_CONST (& ld[1], _GR (".@"));

  /* check is passname.setVar */
  locate_saveto (essen, & ls);
  st_desc->pvarc = self->varChain;
  st_desc->setvar = null;

  if (locate_searchp (essen, & ld[0], 3, null) == 0) {
    /* check pass and setVar */
    st_desc->pvarc = varChain_findg2 (lysis->gsym, ld[0].acceptORacc, ld[0].ac_lens);
    if (st_desc->pvarc == null) {
      essen->buf->str[ld[0].ac_pos+ld[0].ac_lens] = 0;
      uchar_assign3(err, _GR("analysis-error:[%d?%ls] !no exist pass or serialize pass \r\n"), 
        essen->line, 
        & essen->buf->str[ld[0].ac_pos]);
      return MATCH_FATAL_ERROR;
    }
    achieve L0;
  }
  /* check is setVar */
  LOCATE_SETACC_RELAXED_CONST (& ld[1], _GR ("@"));
  if (locate_searchp (essen, & ld[1], 2, null) == 0) {
L0: st_desc->setvar = setVar_find2 (st_desc->pvarc, ld[2].acceptORacc, ld[2].ac_lens);
    if (st_desc->setvar == null) {
      locate_resumefrom (essen, & ls);
      return MATCH_SFALSE;
    }
    return MATCH_FIND;
  }
  locate_resumefrom (essen, & ls);
  return MATCH_SFALSE;
}

match_result 
chunk_string (  struct uchar **gen,
                struct locate_essen_ *essen,
                struct pass_node *self,
                struct analysis *lysis,
                struct uchar *err) 
{
  struct locate_s_ ls;
  struct stringv_desc st;
  struct locate_desc_ ld[5];
  kbool extrn = kfalse;
  match_result m = MATCH_SFALSE;

  ld[0].etype = LOCATE_TYPE_STRING;
  locate_saveto (essen, & ls);
  if (*gen == null) 
    uchar_init (gen);
  else extrn = ktrue;
  uchar_clearbuf (*gen);

  switch (catch_string(& st, essen, self, lysis, err)) {
  case MATCH_SFALSE:
    if (locate_searchp (essen, & ld[0], 1, null) == 0) {
      uchar_insert (*gen, -1, ld[0].acceptORacc, ld[0].ac_lens);
      if (  0)
  case MATCH_FIND:
        uchar_insert3 (*gen, -1, st.strvar->mapper->str);
      return MATCH_FIND;
  case MATCH_FATAL_ERROR: 
  default:
      m = MATCH_FATAL_ERROR;
      break;
    }
    if (extrn == kfalse)
      uchar_uninit (gen);
    locate_resumefrom (essen, & ls);
    return m;
  }
  return m;
}

match_result 
expr_string (  struct uchar *gen/* di arg */,
                struct interrupt_desc *interrupt,
                struct locate_essen_ *essen,
                struct pass_node *self,
                struct analysis *lysis,
                struct uchar *err) 
{
  /* item : string -> "you surf the internet, too"
            string-val -> stringVar
            string-val[with pass] -> passname.stringVar 
            alter -> [has been processed] 
            enval -> [has been processed] 

      out := item exp
      exp := null | exp + item
   */
  match_result ares;
  struct locate_s_ ls;
  struct stringv_desc st;
  struct locate_desc_ ld[5];

  ld[0].etype = LOCATE_TYPE_STRING;
  locate_saveto (essen, & ls);
  uchar_clearbuf (gen);

  LOCATE_SETACC_RELAXED_CONST (& ld[2], _GR ("+"));

  do 
  {   /* tyr match stringVar */
    switch (catch_string(& st, essen, self, lysis, err)) {
    case MATCH_SFALSE:
       /* try match string */
      if (locate_searchp (essen, & ld[0], 1, null) == 0) {
        uchar_insert (gen, -1, ld[0].acceptORacc, ld[0].ac_lens);
        if (  0)
    case MATCH_FIND:
          uchar_insert3(gen, -1, st.strvar->mapper->str);
        else ;
          /* check tail infos */
        if (locate_uselessdata (essen) == 0) {
          interrupt->inter_reason = INTERRUPT_byEOF;
          return MATCH_FIND;
        }
        if (interrupt->interrupt_bysymbols != kfalse) {
            /* interrupt symset test */
          int id;
          for (id = 0; id != interrupt->sig_setnums; id++)  {
            LOCATE_SETACC_RELAXED_CONST (& ld[4], interrupt->terminal_symset[id]);
            if (locate_searchp (essen, & ld[4], 1, null) == 0) {
              interrupt->sig_retnums = id;
              interrupt->inter_reason = INTERRUPT_terminal_ch;
              return MATCH_FIND;
            }  
          }
        }
         /* check link symbols */
        if (locate_searchp (essen, & ld[2], 1, null) == 0)
          continue;
        uchar_assign3(err, _GR("analysis-error[%d]:no find link symbol +"), essen->line);
    case MATCH_FATAL_ERROR: 
        return MATCH_FATAL_ERROR;
      }
      /* no any match **/
      locate_resumefrom (essen, & ls);
      ares = (!essen->doing.length) ? MATCH_SFALSE : MATCH_FATAL_ERROR;
      if (ares == MATCH_FATAL_ERROR)
        uchar_assign3(err, _GR("analysis-error[%d]:interrupt in _expr_string block"), essen->line);
      else ;
      return ares;
    }
  } while (ktrue);
}

match_result 
expr_string_trunc (  struct uchar *gen/* di arg */,
                struct locate_essen_ *essen,
                struct pass_node *self,
                struct analysis *lysis,
                struct uchar *err) 
{
  match_result ares;
  struct locate_s_ ls;
  struct stringv_desc st;
  struct locate_desc_ ld[5];

  kbool isinit = kfalse;
  ld[0].etype = LOCATE_TYPE_STRING;
  locate_saveto (essen, & ls);
  uchar_clearbuf (gen);

  LOCATE_SETACC_RELAXED_CONST (& ld[2], _GR ("+"));

  do 
  {   /* tyr match stringVar */
    switch (catch_string(& st, essen, self, lysis, err)) {
    case MATCH_SFALSE:
       /* try match string */
      if (locate_searchp (essen, & ld[0], 1, null) == 0) {
        uchar_insert (gen, -1, ld[0].acceptORacc, ld[0].ac_lens);
        if (  0)
    case MATCH_FIND:
          uchar_insert3(gen, -1, st.strvar->mapper->str);
        else ;
          /* check tail infos */
        if (locate_uselessdata (essen) == 0) {
          return MATCH_FIND;
        }
         /* check link symbols */
        isinit = ktrue;
        locate_saveto (essen, & ls);
        if (locate_searchp (essen, & ld[2], 1, null) == 0)
          continue;
        uchar_assign3(err, _GR("analysis-error[%d]:no find link symbol +"), essen->line);
        return MATCH_FIND;
    case MATCH_FATAL_ERROR: 
        locate_resumefrom (essen, & ls);
        return MATCH_FATAL_ERROR;
      }
      if (isinit == kfalse) {
        uchar_assign3(err, _GR("analysis-error[%d]:interrupt in _expr_string block"), essen->line);
        return MATCH_FATAL_ERROR;
      }
      locate_resumefrom (essen, & ls);
      return MATCH_FIND;
    }
  } while (ktrue);
}

int pattern_readline ( struct __pattern *__pat,
                      struct locate_essen_ *essen,
                 struct pass_node *self,
                 struct analysis *lysis,
                 struct uchar *err )

{
  /* expr_pattern := setvar 
   *        | expr_string 
   *        | expr_string, <null | .suffixname>
   *        | expr_string, <null | .suffixname>, prefix_set | suffix_set 
   *        | expr_string, <null | .suffixname>, [block set]
   *        | expr_string, <null | .suffixname>, [block set], prefix_set | suffix_set 
   * setvar := @varname | passname.@varname 
   * stringvar := @varname | passname.varname 
   * string := ' content' | " content " | ` content ` 
   * expr_string_base := %% envar | %% alter | string | stringvar 
   * expr_string_base2 := null | expr_string_base2 + expr_string_base
   * expr_string := expr_string_base expr_string_base2
  */
  struct locate_s_ ls, ls2;
  struct setv_desc setv;
  struct stringv_desc stringv;
  struct interrupt_desc intdesc;
  uconst us *cm_comma[] =  { _GR (",") };
  match_result aqres;

  intdesc.terminal_symset = & cm_comma[0];
  intdesc.interrupt_bysymbols = ktrue;
  intdesc.sig_setnums = sizeof (cm_comma)/ sizeof (cm_comma[0]);

  locate_saveto (essen, & ls);
  locate_saveto (essen, & ls2);

  /*try match setvar */
  aqres = catch_set (& setv, essen, self, lysis, err);
  if (aqres == MATCH_FIND) {
    __pat->amat = AMT_MATCH;
    __pat->set = setv.setvar;
    if (locate_uselessdata (essen) == 0)
      return 0;
    else {
      uchar_assign3 (err, _GR("analysis-error:[%d] !some dirty things in tails/base chunk"),  essen->line);
      return -1;
    }
  } else if (aqres == MATCH_FATAL_ERROR) {
    return -1;
  } else {
    uchar_init (& __pat->expr_string);
    if ( expr_string (__pat->expr_string, & intdesc, essen, self, lysis, err) != MATCH_FIND) {
      uchar_assign3 (err, _GR("analysis-error:[%d] !not collect expr_string base"),  essen->line);
  LQ: pattern_uninit (__pat);
      return -1;
    } else if (intdesc.inter_reason == INTERRUPT_byEOF) {
      __pat->amat = AMT_MATCH2;
      return 0;
    } else {
      /* exist , try get suffix 
       * < . cpp> suffix cpp 
       * < cpp>  suffix cpp 
       * <     > no suffix
       * <>   no suffix
      */
      struct locate_desc_ ld[3];
      int sig = 0;
      locate_saveto (essen, & ls2);
      if (locate_match_sym_pure (essen, _GC ('<'), null, null) == 0) {
        if (locate_match_sym_pure (essen, _GC ('>'), null, null) == 0) {
          // empty suffix 
          __pat->suffix = null;
          if (locate_uselessdata (essen) == 0) {
            __pat->amat = AMT_MATCH3;
            return 0;
          }
          uchar_assign3 (err, _GR("analysis-error:[%d] !some dirty things in tails/<>"),  essen->line);
          achieve LQ;
        } else {
          locate_match_sym_pure (essen, _GC ('.'), null, null);
          ld[0].etype = LOCATE_TYPE_VARIABLE;
          LOCATE_SETACC_RELAXED_CONST (& ld[1], _GR (">"));
          LOCATE_SETACC_SYMBOLS_TIGHTLY_CONST (& ld[2], _GR (","), -1, 2);
          if (locate_searchp (essen, & ld[0], 3, null) != 0) {
            uchar_assign3 (err, _GR("analysis-error:[%d] !match block chunk faile"),  essen->line);
            achieve LQ;
          }
          uchar_init (& __pat->suffix);
          uchar_assign (__pat->suffix, ld[0].acceptORacc, ld[0].ac_lens);
          if ( (locate_uselessdata (essen) == 0) 
            && (ld[2].sig_match == 0) )
          {
            __pat->amat = AMT_MATCH3;
            return 0;
          }
          else if ((locate_uselessdata (essen) != 0) 
            && (ld[2].sig_match == 1))
          {
            if  ( locate_match_sym_pure (essen, _GC ('['), null, null) == 0) {
              // expr [ .... 
              // content , | content ]
              us *ccs[2] = { _GR (","), _GR ("]") };
              // struct list_ *block_list = null;

              ld[0].etype = LOCATE_TYPE_VARIABLE;
              ld[1].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY;
              ld[1].dmtach_blk = & ccs[0];
              ld[1].sig_setnums = sizeof (ccs)/ sizeof (ccs[0]);
              list_init (& __pat->block_list);

              while ( locate_searchp(essen, & ld[0], 2 , null) == 0) {
                struct uchar *u;
                uchar_init (& u);

                if (ld[1].sig_match == 0 ) {
                  uchar_assign (u, ld[0].acceptORacc, ld[0].ac_lens);
                  list_insert_tail (__pat->block_list, u);
                  continue  ;
                } else if (ld[1].sig_match == 1) {
                  uchar_assign (u, ld[0].acceptORacc, ld[0].ac_lens);
                  list_insert_tail (__pat->block_list, u);
                  if (locate_uselessdata (essen) == 0) {
                    __pat->amat = AMT_MATCH5;
                    return 0;
                  }
                  // only match , 
                  if (locate_match_sym_pure (essen, _GC(','), null, null) != 0) {
                    uchar_assign3 (err, _GR("analysis-error:[%d] !not find prefix|suffix set 's, "),  essen->line);
                    achieve LQ;
                  }
                  sig = -1;
                  break  ;
                } else {
                  uchar_uninit (& u);
                  uchar_assign3 (err, _GR("analysis-error:[%d] !serach errors in block_list "),  essen->line);
                  achieve LQ;
                }
              }
            }
            if (ktrue) {
                
              us *cm_ppfix[] = { _GR ("PREFIX_SET"), _GR ("SUFFIX_SET") };
              us *cm_pplink[] = { _GR ("?"), _GR (":") };

              ld[0].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7;
              ld[0].dmtach_blk = & cm_ppfix[0];
              ld[0].sig_setnums = sizeof (cm_ppfix)/ sizeof (cm_ppfix[0]);
              ld[1].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY;
              ld[1].dmtach_blk = & cm_pplink[0];
              ld[1].sig_setnums = sizeof (cm_pplink)/ sizeof (cm_pplink[0]);

              while ( locate_searchp(essen, & ld[0], 2 , null) == 0) {
                struct uchar *sp = null;
                struct uchar **c = null;
                struct __patternfx *q = null;
                uchar_init (& sp);

                if (ld[0].sig_match == 0) {
                  if (__pat->pref.template_ != null) {
                    uchar_assign3 (err, _GR("analysis-error:[%d] !PREFIX_SET repeat"),  essen->line);
                    achieve LQ2;
                  }
                  q = & __pat->pref;
                } else {
                  if (__pat->suff.template_ != null) {
                    uchar_assign3 (err, _GR("analysis-error:[%d] !SUFFIX_SET repeat"),  essen->line);
                LQ2: uchar_uninit (& sp);
                    achieve LQ;
                  }
                  q = & __pat->suff;
                }
                if (ld[1].sig_match == 0) {
                  q->complete = ktrue; 
                } else {
                  q->complete = kfalse; 
                }
                if ( expr_string (sp, & intdesc, essen, self, lysis, err) == MATCH_FIND) {
                  q->template_ = sp;
                  if (intdesc.inter_reason == INTERRUPT_byEOF) {
                    __pat->amat = sig? AMT_MATCH6 : AMT_MATCH4;
                    return 0;
                  }
                  continue;
                }
                uchar_uninit (& sp);
                uchar_assign3 (err, _GR("analysis-error:[%d] !expr_string chunk faile"),  essen->line);
                achieve LQ;
              }
            }
          }
          uchar_assign3 (err, _GR("analysis-error:[%d] !suffix block chunk faile"),  essen->line);
          achieve LQ;
        }
      }
    }
  }
  return 0;
}

// + | # | , | empty 
# define MPI_ADD 1
# define MPI_HSC 2
# define MPI_COMMA 4
# define MPI_NULL 8

// < 0 fail 
int match_private_interrupt (struct locate_essen_ *essen, /* inout*/ int *MPI_inpout) {
  int tss = *MPI_inpout;
  int tbb = 0;
  * MPI_inpout = 0;
  if (tss == 0) {
    // noset mask 
    return -2;
  }
  if (locate_uselessdata (essen) == 0) {
    tbb = MPI_NULL;
  } else if (locate_match_sym_pure (essen, _GC ('#'), null, null) == 0) {
    tbb = MPI_HSC;
  } else if (locate_match_sym_pure (essen, _GC (','), null, null) == 0) {
    tbb = MPI_COMMA; 
  } else if (locate_match_sym_pure (essen, _GC ('+'), null, null) == 0) {
    tbb = MPI_ADD; 
  }
  if (tss & tbb) {
    * MPI_inpout = tbb;
    return 0;
  } else {
    return -1;
  }
}

int __fag_get ( int *pout,
                      struct locate_essen_ *essen,
                 struct pass_node *self,
                 struct analysis *lysis,
                 struct uchar *err)
{ 
  struct locate_desc_ ld[2];

  ld[0].etype = LOCATE_TYPE_VARIABLE;
  * pout = 0;
  if ( (locate_uselessdata (essen) == 0)) {
    return 0;
  } else if ( (locate_searchp (essen, & ld[0], 1, null) == 0)
           && (locate_uselessdata (essen) == 0))  // XXX: order dep .
  {
    int s = 0;
    int id;
    for (id = 0; id != ld[0].ac_lens; id++) {
      us cc = ld[0].acceptORacc[id];
      switch (cc) { 
      case  _GC ('N'):
      case  _GC ('n'):
        if (s & AUXILIARY_GENERATE_FLAGS_N)
          achieve __err;
        else 
          s |= AUXILIARY_GENERATE_FLAGS_N;
        continue ;
      case  _GC ('A'):
      case  _GC ('a'):
        if (s & AUXILIARY_GENERATE_FLAGS_A)
          achieve __err;
        else 
          s |= AUXILIARY_GENERATE_FLAGS_A;
        continue ;
      case  _GC ('C'):
      case  _GC ('c'):
        if (s & AUXILIARY_GENERATE_FLAGS_C)
          achieve __err;
        else 
          s |= AUXILIARY_GENERATE_FLAGS_C;
        continue ;
      default: 
        return -1;
      }
    }
    * pout = s;
    return 0;
  }
  else 
  {
  __err : uchar_assign3 (err, _GR("analysis-error:[%d] !__fag_get fail"),  essen->line);
    return -1;
  }
}

int  vnset_init (struct vnset *vns, LIST_TYPE2_(spath) chain)  {

  vns->bc_count = 0;
  vns->bc_total = 0;
  vns->compelte = kfalse;

  /* collect/decimate subcount  */
  if    (    1   )       {
    int q;
    int b = chain->nums;
    struct list_v *p = chain->set;
    struct list_v *d;
   
    for  (q = 0; q != b; q++, p = d)  {
      int q2;
      int b2 = p->spath_o->xpath_dt->nums;
      struct list_v *p2 = p->spath_o->xpath_dt->set;
      struct list_v *d2;
      d  = p->level_n;

      for  (q2 = 0; q2 != b2; q2++, p2= d2)  {
        d2 = p2->level_n;
        if  (p2->xpath_o->kpath_dt->nums <= 0)  
          list_remove_ISQ_L (p->spath_o->xpath_dt, p2, xpath_uninit);
        else  vns->bc_total += p2->xpath_o->kpath_dt->nums;
      }
      if  (p->spath_o->xpath_dt->nums <= 0)   {
        list_remove_ISQ_L (chain, p, spath_uninit);
        continue  ;
      }
    }
    /* check spath nums**/
    if  (chain->nums > 0)    {
      vns->s_goal = chain->nums;
      vns->s_count = 0;
      vns->sgp = chain->set;
      vns->x_goal = vns->sgp->spath_o->xpath_dt->nums;
      vns->x_count = 0;
      vns->xgp = vns->sgp->spath_o->xpath_dt->set;
      vns->k_goal = vns->xgp->xpath_o->kpath_dt->nums;
      vns->k_count = 0;
      vns->kgp = vns->xgp->xpath_o->kpath_dt->set;
      return  0;
    }
    vns->compelte = ktrue;
    return  -1;
  }
}

void  vnset_push (  struct vnset *vns,
                   struct uchar *apc, 
                   struct uchar *bpath)
{
  int id;
  if  (vns->compelte != kfalse)  
    return ;

  /* get path */
  uchar_clearbuf (apc);
  path_inherit (& apc, bpath, vns->sgp->spath_o->path);
  slashtail_clear  (apc->str, apc->length, & id);
  uchar_setlens (apc, id);
  uchar_insert3 (apc, -1, _GR("\\"));
  /* get binname */
  uchar_insert3 (apc, -1, vns->kgp->uchar_o->str);
  /* get suffix*/ 
  if  (vns->xgp->xpath_o->suffix->length > 0)   {
    uchar_insert3 (apc, -1, _GR ("."));
    uchar_insert3 (apc, -1, vns->xgp->xpath_o->suffix->str);
  }
  /* switch list*/
  vns->bc_count++;
  if  (++ vns->k_count >= vns->k_goal)  {
    vns->k_count = 0;
    if ( ++ vns->x_count >= vns->x_goal)   { 
      if ( ++ vns->s_count >= vns->s_goal)   {
        vns->compelte = ktrue;
      }  else  {
        vns->sgp = vns->sgp->level_n;
        vns->x_goal = vns->sgp->spath_o->xpath_dt->nums;
        vns->xgp = vns->sgp->spath_o->xpath_dt->set;
        vns->kgp = vns->xgp->xpath_o->kpath_dt->set;
        vns->k_goal = vns->xgp->xpath_o->kpath_dt->nums;
        vns->x_count = 0;
      }
    }  else   {
      vns->xgp = vns->xgp->level_n;
      vns->kgp = vns->xgp->xpath_o->kpath_dt->set;
      vns->k_goal = vns->xgp->xpath_o->kpath_dt->nums;
    }
  }  else  {
      // vns->bc_count++;
      vns->kgp = vns->kgp->level_n;
  }
}


                   /* atdep */
void  atdep_init (struct atdep **at)  {
  struct atdep *p = umalloc (sizeof (struct atdep));
  uchar_init (& p->agpath);
  p->update = kfalse;
  * at = p;
}

void  atdep_uninit (struct atdep **at)  {
  if (at != null)  {
    struct atdep *a = *at;
    *at = null;
    if (a != null)  {
      uchar_uninit (& a->agpath);
      ufree (a);
    }
  }
}

struct  atdep *atdep_serach (LIST_TYPE2_(atdep) list, struct  uchar *agpath)  {

  int id;
  struct list_v *p;

  LIST_FOREACH (list, id, p)  {
    if (! uchar_cmp(p->atdep_o->agpath, agpath))
       return p->atdep_o;
    else  ;
  }
  return  null;
}



/* dep_cache*/
void dep_cache_init (struct dep_cache **depcc) {
  struct dep_cache *dc = umalloc (sizeof ( struct dep_cache));
  uchar_init (& dc->agpath);
  dc->timeStamp64 = 0;
  * depcc = dc;
}

void dep_cache_uninit (struct dep_cache **depcc) {
  if (depcc != null) {
    struct dep_cache *dc = * depcc;
    * depcc = null;
    if (dc != null) {
      uchar_uninit (& dc->agpath);
      ufree (dc);
    }
  }
}

struct  
dep_cache *dep_cache_serach (LIST_TYPE2_(dep_cache) list, struct  uchar *agpath)  {

  int id;
  struct list_v *p;

  LIST_FOREACH (list, id, p)  {
    if (! uchar_cmp(p->depc_o->agpath, agpath))
      return p->depc_o;
    else  ;
  }
  return  null;
}

void pattern_node_uninit (struct __pattern2_node **pn) {

  if (pn != null) {
    struct __pattern2_node *p_ = *pn;
    * pn = null;
    if (p_ != null) {
      uchar_uninit (& p_->expr_base);
      ufree (p_);
    }
  }
}

int idep_calc ( int line,
                kbool * update,
                struct uchar *agpath,
                struct pass_node *self,
                struct analysis *lysis,
                struct uchar *err )
{
  struct uchar *uch;
  struct atdep *atd;
  uint64_t mod64;
  /* for input file dependencies, 
  test analysis::outdep if there is an entry, 
  and if it does not, 
  then an error will be raised if the file is not found */
  if  ((atd = atdep_serach (lysis->outdep, agpath)) != null)   {
    if (atd->update != kfalse) 
      * update = ktrue;
    return 0;
  }
  if   (file_lastmod9 (agpath->str, & mod64) == 0)  { 
    struct dep_cache *depc;
    /* serach dpcdep chain */
    if ( (depc = dep_cache_serach (lysis->dpcdep, agpath)) != null)  {
      if (depc->timeStamp64 != mod64) 
        * update = ktrue;
      achieve QL ;
    }
    * update = ktrue;
QL: uchar_init ( & uch);
    uchar_assign0 (uch, agpath);
    list_insert_tail (lysis->collect, uch);
    return 0;
  }
  uchar_assign3(err, _GR("analysis-error[%d?%ls]:open file failed in idep_calc"), line, agpath->str);
  return -1;
}

void depcpp2_node_uninit (struct cppdep_node2 **node)  {
  if (node != null)  {
    struct cppdep_node2 *cn = *node;
    * node = null;
    if (cn != null)  {
      uchar_uninit (& cn->atgpath);
      ufree (cn);
    }
  }
}

int idepcpp_calc ( kbool *update,
                struct uchar *agpath,
                struct pass_node *self,
                struct analysis *lysis,
                struct uchar *err )
{
  struct list_ *cn = null;
  kbool e_int = kfalse;
  /*calc absolute path */
  depcpp_listcc (& cn, agpath, err, lysis->kcc_chain, & e_int);

  if  (e_int != kfalse)  {
    list_uninit_ISQ (& cn, depcpp2_node_uninit);
    return -1;
  }
  depcpp_list_cmpset (cn, lysis->dpcdep, lysis->collect, update);
  list_uninit_ISQ (& cn, depcpp2_node_uninit);
  return 0;
}


int odep_calc ( int line,
                struct uchar *outbase,
                struct shell_desc **sh_,
                struct __pattern2 *__pat,
                struct pass_node *self,
                struct analysis *lysis,
                struct uchar *err )
{
  if (__pat->input_ == kfalse && (__pat->pat_out == null)) 
    list_insert_tail (self->shell_chain, *sh_);
  else if (__pat->input_ != kfalse && (__pat->pat_out != null))  {
    /* check output, write into outdep  */
    int s;
    struct atdep *at;
    struct uchar *uu = null;
    atdep_init (& at);
    at->update = ktrue;
    s = path_inherit (& at->agpath, lysis->bpath, outbase);
    uchar_shrink (at->agpath, 1);
    assert (s == 0);
    if (__pat->update_ == kfalse)  {
      /* check out files  */
      uint64_t mod64;
      if (file_lastmod9 (at->agpath->str, & mod64) == 0)  {  
        struct dep_cache *dc;
        dc = dep_cache_serach (lysis->dpcdep, at->agpath);
        if (dc != null && (dc->timeStamp64 == mod64))  {
          at->update = kfalse;
        }
      }
    }
    /* cancel current shell request  */
    if (at->update == kfalse && (__pat->update_ == kfalse)) 
      shell_desc_uninit (sh_);
    else 
      at->update = ktrue;
    if (at->update != kfalse)
      list_insert_tail (self->shell_chain, *sh_);
    list_insert_tail (lysis->outdep, at);
    uchar_copy (& uu, at->agpath);
    list_insert_tail (lysis->collect, uu);
  }  else  {
    /* error, unaligned */
    uchar_assign3(err, _GR("analysis-error[%d]:input/output unmatch"), line);
    shell_desc_uninit (sh_);
    return -1;
  }
  *sh_ = null;
  return 0;
}

int pattern2_preprocessed ( struct __pattern2 *__pat,
                      struct locate_essen_ *essen,
                 struct pass_node *self,
                 struct analysis *lysis,
                 struct uchar *err )

{
  /* expr_pattern2 := cc_block # AUXILIARY_GENERATE_FLAGS 
   * cc_block := cc_block_base cc_block_base2
   * cc_block_base2 := null | cc_block_base2 + cc_block_base
   * cc_block_base := expr_string [not string var] | set_var | string_var 
   * set_var <- -IDEP | -IDEP_CPP | -VBATTER | -HBATTER | -ODEP
   * string_var <- -IDEP | -IDEP_CPP | -ODEP
   * set_var::-HBATTER <- prefix_set | suffix_set 
   *.*/

  struct __pattern2_node *p2n = null;
  struct __patternfx pref;
  struct __patternfx suff;
  struct __patternfx *spptr;
  struct stringv_desc sv;
  struct setv_desc cv;
  struct locate_desc_ ld[2];
  struct locate_desc_ ld2[2];
  struct interrupt_desc idc;
  int cq;
  int vhbatter = 0;

# define GC_IDEP 0
# define GC_IDEP_CPP 1
# define GC_ODEP 4
# define GC_DEPUNSET -1
# define GC_VBATTER 2
# define GC_HBATTER 3
# define GC_BATTER_UNSET -1
  us *cm_auxi[] = { _GR ("IDEP")
    , _GR ("IDEP_CPP")
    , _GR ("VBATTER")
    , _GR ("HBATTER")
    , _GR ("ODEP") };
  us *cm_ppfix[] = { _GR ("PREFIX_SET"), _GR ("SUFFIX_SET") };
  us *cm_pplink[] = { _GR ("?"), _GR (":") };
  us *cm_comq[] = { _GR (","), _GR ("#"), _GR ("+") };

  match_result m1;

  idc.terminal_symset = & cm_comq[0];
  idc.interrupt_bysymbols = ktrue;
  idc.sig_setnums = sizeof (cm_comq)/ sizeof (cm_comq[0]);
  list_init (& __pat->nd_list);
  
  ld[1].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7;
  ld[1].dmtach_blk = & cm_auxi[0];
  ld[1].sig_setnums = sizeof (cm_auxi) / sizeof (cm_auxi[0]);
  ld2[0].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7;
  ld2[0].dmtach_blk = & cm_ppfix[0];
  ld2[0].sig_setnums = sizeof (cm_ppfix)/ sizeof (cm_ppfix[0]);
  ld2[1].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY;
  ld2[1].dmtach_blk = & cm_pplink[0];
  ld2[1].sig_setnums = sizeof (cm_pplink)/ sizeof (cm_pplink[0]);
  LOCATE_SETACC_RELAXED_CONST (& ld[0], _GR ("-"));

  while (ktrue)  {
    p2n = umalloc (sizeof (struct __pattern2_node));
    p2n->expr_base = null;
    p2n->enum_dep = GC_DEPUNSET;
    p2n->set = null;
    p2n->enum_dep = GC_DEPUNSET; /* XXX: infact, not mask , is private enum define*/

    vhbatter = GC_BATTER_UNSET;
    list_insert_tail (__pat->nd_list, p2n);

    if ( (m1 = chunk_string (& p2n->expr_base, essen, self, lysis, err)) == MATCH_FIND) {
      achieve afdo;
    } else if (m1 == MATCH_SFALSE) {
        // try match set 
        if ((m1 = catch_set (& cv, essen, self, lysis, err)) == MATCH_FATAL_ERROR
          || (m1 == MATCH_SFALSE)) 
        {
          uchar_assign3 (err, _GR("analysis-error:[%d] ! some dirty things matchset in pattern2_preprocessed"),  essen->line);
          achieve cleanup;
        } 
        if (ktrue)
        {
          // set setvar 's mode 
          while (locate_searchp (essen, & ld[0], 2, null) == 0) {
            switch (ld[1].sig_match) {
            case  GC_IDEP:
            case  GC_IDEP_CPP:
            case  GC_ODEP: 
              if (p2n->enum_dep != GC_DEPUNSET) {
                uchar_assign3 (err, _GR("analysis-error:[%d] !DEP repeat"),  essen->line);
                achieve cleanup;
              } else if (__pat->pat_out != null && (ld[1].sig_match == 4)) {
                uchar_assign3 (err, _GR("analysis-error:[%d] !DEP-output repeat"),  essen->line);
                achieve cleanup;
              } else if ((ld[1].sig_match == 4)) { // ODEP
                __pat->pat_out = p2n;
              } else {
                __pat->input_ = ktrue;
              }
              p2n->enum_dep = ld[1].sig_match;
              continue  ;
            case  GC_VBATTER: // VBATTER
            case  GC_HBATTER: // HBATTER
              if (vhbatter != GC_BATTER_UNSET) {
                uchar_assign3 (err, _GR("analysis-error:[%d] !BATTER repeat"),  essen->line);
                achieve cleanup;
              }
              vhbatter = ld[1].sig_match;
              continue  ;
            }
          }
          if (vhbatter == GC_BATTER_UNSET) {
            // make one ...
            {
              struct vnset vs;
              uchar_init (& p2n->expr_base);
              vnset_init (& vs, cv.setvar->spath_chain);
              
              if (vs.compelte != kfalse)  {
                uchar_assign3 (err, _GR("analysis-error[%d]:nocontent in setvar"), essen->line);
                return -1;
              } 
              vnset_push (& vs, p2n->expr_base, lysis->bpath);
              if  (p2n->enum_dep != GC_DEPUNSET)   {   
                switch  (p2n->enum_dep)  {
                case  GC_IDEP:  // IDEP
                  if (idep_calc (essen->line, & __pat->update_, p2n->expr_base, self, lysis, err) != 0)
                    achieve cleanup;
                  break;
                case  GC_IDEP_CPP:  // IDEP_CPP
                  if (idepcpp_calc (& __pat->update_, p2n->expr_base, self, lysis, err) != 0)
                    achieve cleanup;
                case  GC_ODEP:  // ODEP, final calc, only attach now 
                  break;
                default:
                  assert (0);
                }
              }
              achieve afdo;
            }
          } else if (vhbatter == GC_VBATTER) {
             // vbatter ....
            struct uchar *uc = null;

            vnset_init (& p2n->vnset_, cv.setvar->spath_chain);
            if (p2n->vnset_.compelte != kfalse)  {
              uchar_assign3 (err, _GR("analysis-error[%d]:nocontent in setvar"), essen->line);
              achieve cleanup;
            } 
            if (__pat->vbat_n == -1
              || (__pat->vbat_n == p2n->vnset_.bc_total))
              __pat->vbat_n = p2n->vnset_.bc_total;
            else {
              uchar_assign3 (err, _GR("analysis-error[%d]:unmatch vnums"), essen->line);
              achieve cleanup;
            }
            p2n->set = cv.setvar;
            achieve afdo;
          } else if (vhbatter == GC_HBATTER) {

            kbool outps;
            struct uchar *ua;
            struct locate_desc_ QS[3];

            QS[1].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY7;
            QS[1].dmtach_blk = & cm_ppfix[0];
            QS[1].sig_setnums = sizeof (cm_ppfix)/ sizeof (cm_ppfix[0]);
            QS[2].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY;
            QS[2].dmtach_blk = & cm_pplink[0];
            QS[2].sig_setnums = sizeof (cm_pplink)/ sizeof (cm_pplink[0]);
            LOCATE_SETACC_RELAXED_CONST (& QS[0], _GR (","));

            pref.template_ = null;
            suff.template_ = null;

            // , PREFIX_SET ?|:  expr
            // , PREFIX_SET ?|:  expr
            uchar_init (& ua);
            outps = !! ( locate_searchp (essen, & QS[0], 3 , null) == 0);
            outps = ( (outps == ktrue) && (expr_string_trunc (ua, essen, self, lysis, err) == MATCH_FIND));
            if (outps != ktrue) {
              uchar_uninit (& ua);
              uchar_assign3 (err, _GR("analysis-error:[%d] !match fixset fail"),  essen->line);
              achieve cleanup;
            }
            if (QS[1].sig_match == 0)
              spptr = & pref;
            else 
              spptr = & suff;
            spptr->template_ = ua;
            spptr->complete = ( (QS[2].sig_match == 0) ? ktrue : kfalse);
            ua = null;
            uchar_init (& ua);
            outps = !! ( locate_searchp (essen, & QS[0], 3 , null) == 0);
            outps = ( (outps == ktrue) && ( (m1 = expr_string_trunc (ua, essen, self, lysis, err)) != MATCH_FATAL_ERROR));
            if ((m1 == MATCH_FATAL_ERROR)) {
              uchar_uninit (& ua);
              uchar_assign3 (err, _GR("analysis-error:[%d] !match fixset fail"),  essen->line);
              achieve cleanup;
            } else if (outps != kfalse) {
              if (QS[1].sig_match == 0)
                spptr = & pref;
              else 
                spptr = & suff;
              if (spptr->template_ != null) {
                uchar_assign3 (err, _GR("analysis-error:[%d] !match fixset repeat"),  essen->line);
                uchar_uninit (& ua);
cleanup_local:  uchar_uninit (& pref.template_);
                uchar_uninit (& suff.template_);
                achieve cleanup;
              }
              spptr->template_ = ua;
              spptr->complete = ( (QS[2].sig_match == 0) ? ktrue : kfalse);
            } else uchar_uninit (& ua);
            // make hout ...
            // return 0;
            {
              struct vnset vs;
              struct uchar *uc = null;

              vnset_init (& vs, cv.setvar->spath_chain);
              if (vs.compelte != kfalse)  {
cleanup_local2:  uchar_uninit (& uc);
                uchar_assign3 (err, _GR("analysis-error[%d]:nocontent in setvar"), essen->line);
                achieve cleanup_local;
              } 
              uchar_init (& p2n->expr_base);
              uchar_init (& uc);
              while  (vs.bc_count < vs.bc_total && (vs.compelte == kfalse))  {
                uchar_clearbuf (uc);
                vnset_push (& vs, uc, lysis->bpath);
                if (pref.template_ != null) {
                  if (__pat->vbat_n != 1 || pref.complete)
                    uchar_insert4 (p2n->expr_base, -1, pref.template_);
                  else ;
                }
                uchar_insert3 (p2n->expr_base, -1, uc->str);

                if  (p2n->enum_dep != GC_DEPUNSET)   {   
                  switch  (p2n->enum_dep)  {
                  case  GC_IDEP: // IDEP
                    if (idep_calc (essen->line, & __pat->update_, uc, self, lysis, err) != 0)
                      achieve cleanup_local2;
                    break;
                  case  GC_IDEP_CPP: // IDEP_CPP
                    if (idepcpp_calc (& __pat->update_, uc, self, lysis, err) != 0)
                      achieve cleanup_local2;
                    break;
                  case  GC_ODEP: // ODEP 
                  default:
                    assert (0);
                  }
                }
                if (suff.template_ != null) {
                  if (vs.bc_count != vs.bc_total || suff.complete) 
                    uchar_insert4 (p2n->expr_base, -1, suff.template_);
                  else ;
                }
              }
              uchar_uninit (& uc);
              uchar_uninit (& pref.template_);
              uchar_uninit (& suff.template_);
              achieve afdo;
            }
          }
        } 
    } else {
      // erros 
      achieve cleanup;
    }
afdo:
    cq = MPI_ADD | MPI_HSC | MPI_NULL;
    if ( match_private_interrupt (essen, & cq) == 0) {
      switch  (cq) {
      case  MPI_ADD: // + 
        continue  ;
      case  MPI_HSC: // # 
        if (__fag_get (& __pat->ag_flags, essen, self, lysis, err) != 0) {
          uchar_assign3 (err, _GR("analysis-error:[%d] !__fag_get faile"),  essen->line);
          achieve cleanup;
        }
      case  MPI_NULL: // EOF ..
        if ( (__pat->input_ == kfalse && (__pat->pat_out != null))
          || (__pat->input_ != kfalse && (__pat->pat_out == null)) ) {
          uchar_assign3 (err, _GR ("analysis-error:[%d] ! input/output unmatch"),  essen->line);
          achieve cleanup;
        }
        return 0;
      default  :
        uchar_assign3 (err, _GR("analysis-error:[%d] ! some dirty things"),  essen->line);
        achieve cleanup;
      }
    }
    // ----------------------
  }
  ;
cleanup:;
  list_uninit_ISQ (& __pat->nd_list, pattern_node_uninit);
  return -1;
}

int pattern2_calc ( struct __pattern2 *__pat,
                      struct locate_essen_ *essen,
                 struct pass_node *self,
                 struct analysis *lysis,
                 struct uchar *err )

{
   struct shell_desc *sh = null;
   struct list_v *p;
   struct uchar *cmdca = null;
   int id;
   int s = -1;

   if (__pat->vbat_n == -1)  {
     /* single op, no vertmapper 
      */
     sheel_desc_init (& sh);
     sh->flags = __pat->ag_flags;
     LIST_FOREACH (__pat->nd_list, id, p)  {
       struct  __pattern2_node *pn = p->obj;
       uchar_insert4 (sh->command, -1, pn->expr_base);
     }
     s = odep_calc  ( essen->line, 
          __pat->pat_out?__pat->pat_out->expr_base:null, 
          & sh, 
          __pat,
          self, lysis, err);
     assert (s == 0);
     list_uninit_ISQ (& __pat->nd_list, pattern_node_uninit);
     return  0;
   }  else  {
     /* exist vertmapper 
      */
      int id2;
      struct list_v *p2;
      uchar_init (& cmdca);
      for  (id = 0; id != __pat->vbat_n; id++)  {
        /*  get cache  base   */          
        struct  uchar *outb = null;
        kbool update_b = __pat->update_;
        uint64_t mod64= 0;
        sheel_desc_init (& sh);
        sh->flags = __pat->ag_flags;
          /* pass processed */
        LIST_FOREACH (__pat->nd_list, id2, p2)  {
          struct  __pattern2_node *pn = p2->obj;
          if (pn->set != null)   {
             vnset_push (& pn->vnset_, cmdca, lysis->bpath);
             uchar_insert4 (sh->command, -1, cmdca);
             switch  (pn->enum_dep)  {
             case  GC_IDEP: 
               if (idep_calc (essen->line, & __pat->update_, cmdca, self, lysis, err) != 0)
                 achieve cleanup;
               continue ;
             case  GC_IDEP_CPP: 
               if (idepcpp_calc (& __pat->update_, cmdca, self, lysis, err) != 0)
                 achieve cleanup;
               continue ;
             case  GC_ODEP:
               uchar_copy (& outb, cmdca);
               continue ;
             default:
               assert (0);
             }
          } else {
            uchar_insert4 (sh->command, -1, pn->expr_base);
          }  
        }
        odep_calc  ( essen->line, 
          outb ? outb : (__pat->pat_out?__pat->pat_out->expr_base:null), 
          & sh, 
          __pat,
          self, lysis, err);
        __pat->update_ = update_b;
        uchar_uninit (& outb);
      }
   }
   s = 0;
cleanup:
   uchar_uninit (& cmdca);
   shell_desc_uninit (& sh);
   list_uninit_ISQ (& __pat->nd_list, pattern_node_uninit);
   return s;
}

void pattern_init (struct __pattern *__pat) {
  __pat->expr_string = null;
  __pat->set = null;
  __pat->suffix = null;
  __pat->pref.complete = kfalse;
  __pat->pref.template_ = null;
  __pat->suff.complete = kfalse;
  __pat->suff.template_ = null;
  __pat->block_list = null;
}

void pattern2_init (struct __pattern2 *__pat) {
  __pat->ag_flags = 0;
  __pat->vbat_n = -1;
  __pat->pat_out = null;
  __pat->nd_list = null;
  __pat->update_ = kfalse;
  __pat->input_ = kfalse;
}

void pattern_uninit (struct __pattern *__pat) {
  uchar_uninit (& __pat->expr_string);
  uchar_uninit (& __pat->suffix);
  uchar_uninit (&  __pat->pref.template_);
  uchar_uninit (& __pat->suff.template_);
  list_uninit_ISQ (& __pat->block_list, uchar_uninit);
}

void pattern2_uninit (struct __pattern2 *__pat) {
  list_uninit_ISQ (& __pat->nd_list, pattern_node_uninit);
}