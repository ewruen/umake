# include "precond.h"
# include "locate.h"
# include "ustream.h"
# include "env.h"

/*  e.g. " #if !!!!44 && !3 || ((999||11) ^^ 0) || 19 && %EVAL && MAV" 
         " #elif !! 555 || %VS2010  ^^ MAV >= 5 " 
         " #else                              " 
         " #  define AC 5 
         " #       error " FATAL ERROR" ~ 'dsADSSDSADSA'       " 
         " #endif                             " 
   */

enum PRE_COND_ {
  PRE_COND_IF,  /* #if exp */
  PRE_COND_ELIF, /* #elif exp */
  PRE_COND_ELSE, /* #else */
  PRE_COND_ENDIF, /* #endif */
  PRE_COND_DEFINE, /* #define  e.g # define NUMBER | define NUMBER 6 */
  PRE_COND_ERROR, /* #error string and wrapline exit */
  PRE_COND_OUTPUT, /* #output string and wrapline */
  PRE_COND_NIL,
  PRE_COND_ABNORMAL
};

enum LG_SYM {
  LG_SYM_AND,
  LG_SYM_OR,
  LG_SYM_XOR,
  LG_SYM_NOFIND
};

struct pp_tree_node  {
   /*  handle bracket-related precedence   */
  int pt_left;
  int pt_right;
  struct pp_tree_node *parent;
  struct pp_tree_node *level_n;
  struct pp_tree_node *level_p;
  struct pp_tree_node *child; 
  int childs ;
};

struct pc_tree_node  {
  /*  handle cond-compile-related precedence   */
  int len;  /* buffer's size  */ 
  enum PRE_COND_ pc_type;
  int pt_left;
  int pt_right;
  struct pc_tree_node *parent;
  struct pc_tree_node *level_n;
  struct pc_tree_node *level_p;
  struct pc_tree_node *child; 
  int childs ;
  int line;
};


void pc_node_init (struct pc_tree_node **tn) {
  struct pc_tree_node *p = umalloc (sizeof (struct pc_tree_node));
  p->child = null;
  p->parent = null;
  p->level_n = p;
  p->level_p = p;
  p->len = 0;
  p->pc_type = PRE_COND_NIL;
  p->childs = 0;
  p->line = 0;
  * tn = p;
}

void pc_node_uninit (struct pc_tree_node **tn) {
  if (tn  != null)  {
    struct pc_tree_node *pn = * tn;
    * tn = null;
    if (pn != null)  {
      ufree (pn);
    }
  }
}

void pp_node_init (struct pp_tree_node **tn) {
  struct pp_tree_node *p = umalloc (sizeof (struct pp_tree_node));
  p->child = null;
  p->parent = null;
  p->level_n = p;
  p->level_p = p;
  p->pt_left = 0;
  p->pt_right = 0;
  p->childs = 0;
  * tn = p;
}

void pp_node_uninit (struct pp_tree_node **tn) {
  if (tn  != null)  {
      struct pp_tree_node *pn = * tn;
      * tn = null;
      if (pn != null)  {
        ufree (pn);
      }
  }
}

void precond_init (struct precond_ **prec) {
  if (prec != null) {
    struct precond_ *p = umalloc (sizeof (struct precond_));
    uchar_init (& p->name);
    p->type_numb = kfalse;
    p->numb = 0;
    * prec = p;
  }
}

void precond_uninit (struct precond_ **prec) {
  if (prec != null) {
    if (* prec != null) {
      struct precond_ *p = *prec;
      * prec= null;
      if (p->name != null) {
        uchar_uninit (& p->name);
      }
      ufree (p);
    }
  }
}

struct precond_  *precond_exist (LIST_TYPE2_(precond_) list, us *name, int cclen) {
  int id;
  struct list_v *pv;
  /* serach list */
  LIST_FOREACH (list, id, pv) {
    if (uchar_cmp3maxs (pv->prec_o->name, name, cclen) == 0)
      return pv->prec_o; 
    else ;
  }
  return null;
}


int exprT (  LIST_TYPE2_(userpath2) up2_chain, 
             LIST_TYPE2_(precond_) prc_chain,                       
             struct locate_essen_ *essen, kbool *result, struct uchar *err)  {
  /* exp for #if/#elif 
     S -> negop S2 
     S2-> env | numbcc | define 
     env -> % env-varname 
     numbcc -> numb 
     define -> pdefine | ndefine 
     numb -> [0 1 2 3 4 5 6 7 8 9]
     negop -> ! negop | null 
     pdefine -> def-varname 
     ndefine -> pdefine | def-varname cmp_op numb 
     cmp_op -> == | > | >= | < | <=  */
  struct uchar *msp = null;
  int s, n;
  struct locate_desc_ env_[3];
  struct locate_desc_ numb_[2];
  struct locate_desc_ pdef_[2];
  struct locate_desc_ ndef_[4];

  us c;
//us *p;
  us *cmp_op[]  = { 
        _GR ("=="),  
        _GR (">="), 
         _GR (">"), 
        _GR ("<="),
        _GR ("<"),
  };

  kbool ccout;
  /* set env assert */
  LOCATE_SETACC_SYMBOLS_RELAXED_CONST2 (& env_[0], _GR ("!"));
  LOCATE_SETACC_ACCURATE_RELAXED_CONST2 (& env_[1], _GR ("%"));
  env_[2].etype = LOCATE_TYPE_VARIABLE;
  /* set number assert */
  LOCATE_SETACC_SYMBOLS_RELAXED_CONST2 (& numb_[0], _GR ("!"));
  numb_[1].etype = LOCATE_TYPE_NUMBER;  
  /* set pdefine assert */
  LOCATE_SETACC_SYMBOLS_RELAXED_CONST2 (& pdef_[0], _GR ("!"));
  pdef_[1].etype = LOCATE_TYPE_VARIABLE; 
  pdef_[1].ac_lens = 0;
  /* set ndefine assert */
  LOCATE_SETACC_SYMBOLS_RELAXED_CONST2 (& ndef_[0], _GR ("!"));
  ndef_[1].etype = LOCATE_TYPE_VARIABLE;  
  ndef_[2].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY;  
  ndef_[2].dmtach_blk = & cmp_op[0];
  ndef_[2].sig_setnums = sizeof (cmp_op)/ sizeof (cmp_op[0]);
  ndef_[3].etype = LOCATE_TYPE_NUMBER;  

  if ( locate_searchp 
         ( essen, 
         & env_[0], 
           sizeof (env_)/ sizeof (env_[0]), 
           null  ) == 0)
  {
    /* check exist */ 
    ccout = ! env2_exist
          ( up2_chain, env_[2].acceptORacc, env_[2].ac_lens);
    * result = ccout ^ (env_[0].sig_match & 1);
    return 0;
  }
  else if ( locate_searchp 
         ( essen, 
         & numb_[0], 
           sizeof (numb_)/ sizeof (numb_[0]), 
           null  ) == 0)
  {
    c = locate_setnullc (& numb_[1]);
    ccout = !! wcstol (numb_[1].acceptORacc, null, 10);
    locate_setnullq (& numb_[1], c);
    * result = ccout ^ (numb_[0].sig_match & 1);
    return 0;
  }
  else if ( locate_searchp 
         ( essen, 
         & ndef_[0], 
           sizeof (ndef_)/ sizeof (ndef_[0]), 
           null  ) == 0)
  {
    struct precond_ *prc_ = precond_exist 
        ( prc_chain, ndef_[1].acceptORacc, ndef_[1].ac_lens);;
    if (prc_ == null)
      s = 0;
    else if (prc_->type_numb == kfalse)
      s = 0;
    else 
      s = prc_->numb ;
    ccout = kfalse ;
    c = locate_setnullc (& ndef_[3]);
    n = wcstol (ndef_[3].acceptORacc, null, 10);
    locate_setnullq (& ndef_[3], c);

    // XXX: dep .x
    /*
            _GR ("=="),  
        _GR (">"), 
        _GR (">="), 
        _GR ("<"), 
        _GR ("<=")  */
    switch (ndef_[2].sig_match)  {
    case  0:  if  (s == n)  ccout = ktrue ; break ;
    case  1:  if  (s >= n)  ccout = ktrue ; break ;
    case  2:  if  (s > n)  ccout = ktrue ; break ;
    case  3:  if  (s <= n)  ccout = ktrue ; break ;
    case  4:  if  (s < n)  ccout = ktrue ; break ;
    default : assert (0);
    }
    * result = ccout ^ (ndef_[0].sig_match & 1);
    return 0;
  } 
  else if ( locate_searchp 
            ( essen, 
            & pdef_[0], 
              sizeof (pdef_)/ sizeof (pdef_[0]), 
              null  ) == 0)
  {
    ccout = !! precond_exist 
      ( prc_chain, pdef_[1].acceptORacc, pdef_[1].ac_lens);
    * result = ccout ^ (pdef_[0].sig_match & 1);
    return 0;
  }
  uchar_assign3 (err, _GR ("precond-error:[%d] !exprT error.\r\n"), essen->line);
  return  -1;
}

int exprS (struct locate_essen_ *essen, struct uchar **result, struct uchar *err)  {
  /* exp for #error/#output 
     S -> string c  
     c -> c ~ string | null  */
  struct locate_s_ ltes;
  struct locate_desc_ ld[2];
  struct uchar *msp = null;
  int sig = -1;
  us *fd = umalloc (essen->buf->length * sizeof (us));
  us *ot = essen->lu;
  essen->lu = fd;
  essen->upos = 0;
  locate_saveto (essen, & ltes);
  uchar_init (& msp);
  uchar_assign3 (err, _GR ("precond-error:[%d] !experS error.\r\n"), essen->line);
  LOCATE_SETACC_RELAXED_CONST (& ld[1], _GR ("+"));
  ld[0].etype = LOCATE_TYPE_STRING;

  while   ( ktrue )  {
    if ( locate_searchp (essen, & ld[0], 1, null) != 0)     
      break ;
    else if (  1 )
      uchar_insert (msp, -1, ld[0].acceptORacc, ld[0].ac_lens); 
    if ( (sig = locate_uselessdata(essen))== 0) 
      break  ;
    if ( (sig =locate_searchp (essen, & ld[1], 1, null)) != 0)
      break ;
    else   ;
  }

  if (sig != 0)  {
    uchar_uninit (& msp);
    locate_resumefrom (essen, & ltes);
  }
  else * result = msp ;

  ufree (essen->lu);
  essen->lu = ot;
  return sig;
}

int exprQ (struct locate_essen_ *essen, LIST_TYPE2_(precond_) prc_chain, struct uchar *err)  {
  /* exp for #define
     S -> var number | var */
  struct locate_s_ ltes;
  struct locate_desc_ ld[2];
  struct uchar *msp = null;
  struct precond_ *cond = null;
  int sig = -1;
  ld[0].etype = LOCATE_TYPE_VARIABLE;
  ld[1].etype = LOCATE_TYPE_NUMBER;
  locate_saveto (essen, & ltes);
  uchar_assign3 (err, _GR ("precond-error:[%d] !experQ error.\r\n"), essen->line);

  if ( locate_searchp (essen, & ld[0], 2, null) == 0) {
    if (locate_uselessdata (essen) != 0)
      return -1;
    else if (precond_exist (prc_chain, ld[0].acceptORacc, ld[0].ac_lens) != null) {
      ld[0].acceptORacc[ld[0].ac_lens] = 0;
      uchar_assign3 (err, _GR ("precond-error:[%d?%ls] !redefine marco in #define block.\r\n"), essen->line, ld[0].acceptORacc);
      return -1;
    }
    precond_init (& cond);
    cond->type_numb = ktrue;
    ld[1].acceptORacc[ld[1].ac_lens] = 0;
    cond->numb = wcstol (ld[1].acceptORacc, null, 10);
    uchar_assign (cond->name, ld[0].acceptORacc, ld[0].ac_lens);
    list_insert_tail (prc_chain, cond);
    return 0;
  } else if ( locate_searchp (essen, & ld[0], 1, null) == 0) {
    if (locate_uselessdata (essen) != 0)
      return -1;
    else if (precond_exist (prc_chain, ld[0].acceptORacc, ld[0].ac_lens) != null) {
      ld[0].acceptORacc[ld[0].ac_lens] = 0;
      uchar_assign3 (err, _GR ("precond-error:[%d?%ls] !redefine marco in #define block.\r\n"), essen->line, ld[0].acceptORacc);
      return -1;
    }
    precond_init (& cond);
    cond->type_numb = kfalse;
    uchar_assign (cond->name, ld[0].acceptORacc, ld[0].ac_lens);
    list_insert_tail (prc_chain, cond);
    return 0;
  }
  return -1;
}
enum LG_SYM 
linkT (struct locate_essen_ *essen)  {

  struct locate_desc_ link_[1];

 // us c;
 // us *p;
  us *link_op[]  = { 
        _GR ("&&"),  
        _GR ("||"), 
        _GR ("^^")
  };

  link_[0].etype = LOCATE_TYPE_ACCURATE_BLOCK_TIGHTLY;  
  link_[0].dmtach_blk = & link_op[0];
  link_[0].sig_setnums = sizeof (link_op)/sizeof (link_op[0]);

  if ( locate_searchp 
         ( essen, 
         & link_[0], 
           sizeof (link_)/ sizeof (link_[0]), 
           null  ) == 0)
  {
    switch (link_[0].sig_match)  {
    case 0:  return LG_SYM_AND ;
    case 1:  return LG_SYM_OR ;
    case 2:  return LG_SYM_XOR ;
    default : assert (0);
    }
  }
  return LG_SYM_NOFIND;
}

kbool
linkRS (enum LG_SYM lgsym, kbool rs, kbool rs2)  {

  kbool tbo;
  switch (lgsym)  {
  case LG_SYM_AND:
    tbo = rs && rs2;  break ;
  case LG_SYM_OR:
    tbo = rs || rs2;  break ;
  case LG_SYM_XOR:
    tbo = (!!rs) ^ !!rs2;  break ;
  case LG_SYM_NOFIND:
    assert (0);
  }
  return tbo;
}

int exprLeaves (  LIST_TYPE2_(userpath2) up2_chain, 
             LIST_TYPE2_(precond_) prc_chain, 
             struct locate_essen_ *essen, 
                  kbool *result, 
                struct uchar *err  )  {
  /* exp for leaves node
     S -> exprT c  
     c -> c linkT exprT | null
     linkT -> && | ^^ | || 
     */
  struct locate_s_ ltes;
  enum LG_SYM lgsym = LG_SYM_NOFIND;
  
  kbool ccout ;
  kbool _init = kfalse ;
  kbool cctemp ;

  int sig = -1;
  locate_saveto (essen, & ltes);
  while   ( ktrue )  {
    
    if ( exprT (up2_chain, prc_chain, essen, & cctemp, err) != 0)     
      break ;
    if (_init == kfalse)  {
      ccout = cctemp;
    }
    if ( (sig = locate_uselessdata(essen))== 0) 
      break  ;
    if (_init == kfalse)  {
      _init = ktrue;
    }  else  {
      ccout = linkRS (lgsym, ccout, cctemp);
    }
    if ( (lgsym = linkT (essen)) == LG_SYM_NOFIND)  {
      sig = -1;
      uchar_assign3 (err, _GR ("precond-error:[%d] !linkT error ( .\r\n"), essen->line);
      break ;
    }
  }

  if (sig != 0)  {
    locate_resumefrom (essen, & ltes);
  }  else  {
    if (lgsym != LG_SYM_NOFIND)
      ccout = linkRS (lgsym, ccout, cctemp);
    * result = ccout;
  }
  return sig;
}


enum L_TG_TYPE {
  L_TG_TYPE_S1,
  L_TG_TYPE_S2,
  L_TG_TYPE_S3,
  L_TG_TYPE_FAILED
};


enum L_TG_TYPE 
exprL (  LIST_TYPE2_(userpath2) up2_chain, 
             LIST_TYPE2_(precond_) prc_chain, 
             struct locate_essen_ *essen, 
                  kbool *result, 
                  int *ngsym,
                  enum LG_SYM *lgsym,
                struct uchar *err  )  {
  /* merges - left 
     S -> exprLeaves linkT negop 
     S -> null 
     S -> negop 
     */
  struct locate_s_ ltes;
  struct locate_desc_ neg_[1];
  enum LG_SYM lgsym2 = LG_SYM_NOFIND;

  kbool ccout ;
  kbool _init = kfalse ;
  kbool cctemp ;
  int sig = -1;
  locate_saveto (essen, & ltes);
  LOCATE_SETACC_SYMBOLS_RELAXED_CONST3 (& neg_[0], _GR ("!"));

  if (locate_uselessdata(essen)== 0)   {
     /* empty ?*/
    return L_TG_TYPE_S2;
  }
  if (locate_searchp (essen, & neg_[0], 1, null) == 0)  {
    if (locate_uselessdata(essen)== 0) {
      * ngsym = neg_[0].sig_match;
      return L_TG_TYPE_S3;
    }
  }
  locate_resumefrom (essen, & ltes);

  while   ( ktrue )  {

    if ( exprT (up2_chain, prc_chain, essen, & cctemp, err) != 0)     
      break ;
    if (_init == kfalse)  {
      ccout = cctemp;
    }
    if ( (sig = locate_uselessdata(essen))== 0) 
      break  ;
    if (_init == kfalse)  {
      _init = ktrue;
    }  else  {
      ccout = linkRS (lgsym2, ccout, cctemp);
    }
    if ( (lgsym2 = linkT (essen)) == LG_SYM_NOFIND)  {
      sig = -1;
      uchar_assign3 (err, _GR ("precond-error:[%d] !linkT error .\r\n"), essen->line);
      break ;
    }  else  {
        /* serach !* or empty or next chunk**/
        struct locate_s_ ltes2;
        locate_saveto (essen, & ltes2);

      if (locate_uselessdata(essen) == 0)
        { neg_[0].sig_match = 0; achieve L0 ; }
      if (locate_searchp (essen, & neg_[0], 1, null) == 0)  {
        if (locate_uselessdata(essen)== 0) {
     L0 : * lgsym  = lgsym2;
          * ngsym = neg_[0].sig_match;
          * result = ccout;
          return L_TG_TYPE_S1;
        }
      } 
    }
  }
  locate_resumefrom (essen, & ltes);
  return L_TG_TYPE_FAILED;
}

enum M_TG_TYPE {
  M_TG_TYPE_S1,
  M_TG_TYPE_S2,
  M_TG_TYPE_FAILED
};

enum M_TG_TYPE 
exprM (  LIST_TYPE2_(userpath2) up2_chain, 
             LIST_TYPE2_(precond_) prc_chain, 
             struct locate_essen_ *essen, 
                  kbool *result, 
                  int *ngsym,
                  enum LG_SYM *lgsym,
                  enum LG_SYM *lgsym2,
                struct uchar *err  )  {
  /* merges - middle 
     S -> linkT exprLeaves linkT negop 
     S -> linkT negop
     */
  struct locate_s_ ltes;
  struct locate_desc_ neg_[1];
  enum LG_SYM lgsym7 = LG_SYM_NOFIND;

  locate_saveto (essen, & ltes);
  LOCATE_SETACC_SYMBOLS_RELAXED_CONST3 (& neg_[0], _GR ("!"));

  if (locate_uselessdata(essen)== 0)   {
     /* empty ?*/
    return M_TG_TYPE_FAILED;
  }
  /* match linkT */ 
  if ( (lgsym7 = linkT (essen)) == LG_SYM_NOFIND)  {
    uchar_assign3 (err, _GR ("precond-error:[%d] !exprM error .\r\n"), essen->line);
    return M_TG_TYPE_FAILED;
  }
  else   ;
  * lgsym = lgsym7;    

  switch (exprL (up2_chain, prc_chain, essen, result, ngsym, lgsym2, err))  {
  case L_TG_TYPE_S1:
    return M_TG_TYPE_S1;
  case L_TG_TYPE_S3:
  case L_TG_TYPE_S2: 
    return M_TG_TYPE_S2;
  case L_TG_TYPE_FAILED:
    return M_TG_TYPE_FAILED;
  default :assert (0);
  }
  return M_TG_TYPE_FAILED;
}

enum R_TG_TYPE {
  R_TG_TYPE_S1,
  R_TG_TYPE_S2,
  R_TG_TYPE_FAILED
};

enum R_TG_TYPE 
exprR (  LIST_TYPE2_(userpath2) up2_chain, 
             LIST_TYPE2_(precond_) prc_chain, 
             struct locate_essen_ *essen, 
                  kbool *result, 
                  enum LG_SYM *lgsym,
                struct uchar *err  )  {
  /* merges - right 
     S -> linkT exprLeaves
     S -> null 
     */
  struct locate_s_ ltes;
  struct locate_desc_ neg_[1];
  enum LG_SYM lgsym2 = LG_SYM_NOFIND;

  locate_saveto (essen, & ltes);
  LOCATE_SETACC_SYMBOLS_RELAXED_CONST3 (& neg_[0], _GR ("!"));

  if (locate_uselessdata(essen)== 0)   {
     /* empty ?*/
    return R_TG_TYPE_S2;
  }
  /* match linkT */ 
  if ( (lgsym2 = linkT (essen)) == LG_SYM_NOFIND)  {
    uchar_assign3 (err, _GR ("precond-error:[%d] !exprR error .\r\n"), essen->line);
    return R_TG_TYPE_FAILED;
  }
  * lgsym = lgsym2;
  return ( exprLeaves (up2_chain, prc_chain, essen, result, err)==0? R_TG_TYPE_S1: R_TG_TYPE_FAILED);
}

enum 
PRE_COND_ getpc (us *buf, int len)
{
  kbool _if     = (len == wcslen (_GR("if")) && !wcsncmp (buf, _GR ("if"), len));
  kbool _elif   = (len == wcslen (_GR("elif")) && !wcsncmp (buf, _GR ("elif"), len));
  kbool _else   = (len == wcslen (_GR("else")) && !wcsncmp (buf, _GR ("else"), len));
  kbool _endif  = (len == wcslen (_GR("endif")) && !wcsncmp (buf, _GR ("endif"), len));
  kbool _error  = (len == wcslen (_GR("error")) && !wcsncmp (buf, _GR ("error"), len));
  kbool _output = (len == wcslen (_GR("output")) && !wcsncmp (buf, _GR ("output"), len));
  kbool _define = (len == wcslen (_GR("define")) && !wcsncmp (buf, _GR ("define"), len));

  if (_if) return PRE_COND_IF;
  else if (_elif) return PRE_COND_ELIF;
  else if (_else) return PRE_COND_ELSE;
  else if (_endif) return PRE_COND_ENDIF;
  else if (_error) return PRE_COND_ERROR;
  else if (_output) return PRE_COND_OUTPUT;
  else if (_define) return PRE_COND_DEFINE;
  else  return PRE_COND_ABNORMAL;
}

void pp_tree_uninit0 (struct pp_tree_node *pp_)  {

  int id;
  int iq;
  struct pp_tree_node *pn = null;
  struct pp_tree_node *pg;

  if (pp_ == null)  {
    return  ;
  }

  pg = pp_->child;
  iq = pp_->childs;

  pp_node_uninit (& pp_);
  /* foreach child node */
  for (id = 0; id != iq; id++, pg = pn)  {
    pn = pg->level_n  ;
    pp_tree_uninit0 (pg);
  }
}

# define pp_tree_uninit(/*struct pp_tree_node **/ PpppP ) \
do { pp_tree_uninit0 (PpppP); PpppP = null; }while (0)

void pc_tree_uninit0 (struct pc_tree_node *pp_)  {

  int id;
  int iq;
  struct pc_tree_node *pn = null;
  struct pc_tree_node *pg;

  if (pp_ == null)  {
    return  ;
  }

  pg = pp_->child;
  iq = pp_->childs;

  pc_node_uninit (& pp_);
  /* foreach child node */
  for (id = 0; id != iq; id++, pg = pn)  {
    pn = pg->level_n  ;
    pc_tree_uninit0 (pg);
  }
}

# define pc_tree_uninit(/*struct pc_tree_node **/ PpppP ) \
  do { pc_tree_uninit0 (PpppP); PpppP = null; }while (0)


int pp_tree_build (struct pp_tree_node **pp_, struct locate_essen_ *essen, struct uchar *err)  {

  /* match L-R bracket, build gen tree */
  us cc;
  struct locate_s_ as;
  struct pp_tree_node *p9 = null, *p7 = null, *p8;
  struct pp_tree_node *pn = null;
  struct pp_tree_node *tiny_stack[512];
  int                  tiny_stackcount = 0;
  pp_node_init (& p7);
  locate_saveto (essen, & as);
  p9 = p8 = p7;
  p7->pt_left = essen->doing.apos;
  p7->pt_right = essen->doing.apos + essen->doing.length - 1;
  for (; locate_getcc(essen, & cc) == 0 ;)  {

     if  (cc == _GC ('('))  {
       /* push  */
       pp_node_init (& pn);
       locate_settoken2 (essen, -1, _GC (' '));
       pn->pt_left = locate_current (essen, -1);
       pn->parent = p7;
       p7->childs++; 
       if (p7->child == null)  {
         p7->child = pn;
       } else {
         pn->level_n = p7->child;
         pn->level_p = p7->child->parent;
         p7->child->level_p->level_n = pn;
         p7->child->level_p = pn;
       }    
       tiny_stack[tiny_stackcount++] = pn;
       p7 = pn;
     }
     else if (cc == _GC (')')) {
       /* pop */ 
       if (tiny_stackcount <= 0)  {
         /* miss left (  */
         uchar_assign3 (err, _GR ("precond-error:[%d] !extra (. \r\n"), essen->line);
         achieve cleanup ;
       }
       pn = tiny_stack[--tiny_stackcount] ;
       pn->pt_right = locate_current (essen, -1) ;
       p7 = pn->parent;
       locate_settoken2 (essen, -1, _GC (' '));
     }
  }

  if (tiny_stackcount != 0)  {
    uchar_assign3 (err, _GR ("precond-error:[%d] !L-R bracket mismatch\r\n"), essen->line);
cleanup:
    pp_tree_uninit (p8);
    return -1;
  }
  * pp_ = p8;
  return 0;
}

void calcnode_pp (  LIST_TYPE2_(userpath2) up2_chain, 
                LIST_TYPE2_(precond_) prc_chain, 
                 struct pp_tree_node *pp, 
                   struct locate_essen_ *ess,
                   kbool *error_,
                   kbool *btour,
                   struct uchar *err  )
# define CHK_PRRETURN_PRIVATE()   \
  if (* error_ == ktrue)      \
  return  
{
  int id;
  int pbl = 0, pbr = -1;
  int lg, lg2;
  int ngsym;
  struct pp_tree_node *pv;
  enum LG_SYM lgsym, lgsym2;
  kbool ccout, ccout2;
  
  if (  * error_ == ktrue
    ||  pp == null )
    return ;
  else if (pp->childs <= 0 || pp->child == null)   {
      /* leaves node  */
    locate_resumefrom2 (ess, pp->pt_left, pp->pt_right);
    if (exprLeaves(up2_chain, prc_chain, ess, btour, err) != 0)  {
      * error_ = ktrue;
    }
    return  ;
  }

  pv = pp->child;
  pbl = pp->pt_left;
  pbr = pp->pt_right; // - (  !(%MSVC_IDE && %WINDOWS_SDK) && 0  )-
  for (id = 0; id != pp->childs; pv =pv->level_n, pbl = lg2, id++)  {  
    CHK_PRRETURN_PRIVATE ();
    locate_resumefrom2 (ess, pv->pt_left, pv->pt_right);
    /* scan node */
    lg = pv->pt_left - pbl;
    lg2 = pv->pt_right + 1;
    if (lg <= 0)  {
       /* head, nodeal, ignore */
      if (pv == pp->child)  {
        calcnode_pp (up2_chain, prc_chain, pv, ess, error_, & ccout2, err);
        CHK_PRRETURN_PRIVATE ();
        ccout = ccout2;
        achieve L5 ;
      }
      /* otherwise, errors (no link symbol ) */
      uchar_assign3 (err, _GR ("precond-error:[%d] !no link symbol between parentheses .\r\n"), ess->line);
      achieve L8;
    }  
    /* reset ess */
    locate_resumefrom2 (ess, pbl, pv->pt_left - 1);
    /* is head? */
    if (pv == pp->child)  {
       /* do stuff exprL */
      kbool init_ = kfalse;
      switch (exprL (up2_chain, prc_chain, ess, & ccout, & ngsym, & lgsym, err ))  {
      /* merges - left 
         S -> exprLeaves linkT negop 
         S -> null 
         S -> negop 
         */
      case  L_TG_TYPE_FAILED:
         achieve L8;
      case  L_TG_TYPE_S1:
        init_ = ktrue;
        break  ;
      case  L_TG_TYPE_S2:
        lgsym = LG_SYM_NOFIND;
        ngsym = 0;
        break  ;
      case  L_TG_TYPE_S3:
        lgsym = LG_SYM_NOFIND;
        break  ;
      }
      calcnode_pp (up2_chain, prc_chain, pv, ess, error_, & ccout2, err);
      CHK_PRRETURN_PRIVATE ();
      if (init_ == kfalse) 
        ccout = !! ccout2  ^ (1 & ngsym);
      else 
        ccout = linkRS (lgsym, ccout, (1 & ngsym) ^ !! ccout2) ;
      achieve L5  ;
    } 

    if (pv != pp->child /* && pv != pp->child->level_p*/ )  {
      /* do stuff exprM */ 
      switch (exprM (  up2_chain, 
                prc_chain, ess, 
                  & ccout2, 
                & ngsym, 
                  & lgsym, 
                 & lgsym2, err )) {
    /* merges - middle 
       S -> linkT exprLeaves linkT negop 
       S -> linkT negop
       */
      case  M_TG_TYPE_S1:
        ccout = linkRS (lgsym, ccout, ccout2) ;
        lgsym = lgsym2;
        break  ;
      case  M_TG_TYPE_S2:
        break  ;
      case  M_TG_TYPE_FAILED:
        achieve L8;
      }
      calcnode_pp (up2_chain, prc_chain, pv, ess, error_, & ccout2, err);
      CHK_PRRETURN_PRIVATE ();
      ccout = linkRS (lgsym, ccout, ccout2) ;
      continue  ;
    }
L5: if (pv == pp->child->level_p)  {
      if (pv->pt_right == pp->pt_right)  {
        achieve L6;
      }
      locate_resumefrom2 (ess, pv->pt_right + 1, pp->pt_right);

      /* do stuff exprR */
      switch (exprR (  up2_chain, 
                prc_chain, ess, 
                  & ccout2,  
                  & lgsym, 
                err )) {
    /* merges - right 
       S -> linkT exprLeaves
       S -> null 
       */
      case  R_TG_TYPE_S1:
        ccout = linkRS (lgsym, ccout, ccout2) ;
      case  R_TG_TYPE_S2:
     L6: * btour = ccout;
        return  ;
      case  R_TG_TYPE_FAILED:
     L8: * error_ = ktrue;
        return ;
      }
    }
    CHK_PRRETURN_PRIVATE ();
# undef CHK_PRRETURN_PRIVATE
  }
  * btour = ccout;

}


void discard_pc (struct ustream *ust, int appos) {

  if (appos >= 0 && appos <= ust->total)
    ust->line_gp[appos-1]->invalid = ktrue;
}

void discard2_pc (struct ustream *ust, int left, int right) {
  for (; left <= right; left++)
    discard_pc (ust, left);

}

void discard3_pc (  struct ustream *ust, 
                     int dead_left, 
                     int dead_right,
                     int alive_left,
                     int alive_right  ) 
{
  discard2_pc (ust, dead_left, alive_left -1);
  discard2_pc (ust, alive_right + 1, dead_right);
}

void calcnode_pc (  LIST_TYPE2_(userpath2) up2_chain, 
                LIST_TYPE2_(precond_) prc_chain, 
                 struct pc_tree_node *pc, 
                   struct ustream *ust,
                   kbool *error_,
                   kbool *interrupt_,
                   struct uchar *err  )

# define CHK_PRRETURN_PRIVATE()   \
  if (* error_ == ktrue)      \
  return  
# define SET_PRRETURN_PRIVATE()   \
  do { * error_ = ktrue;      \
        return ;  } while (0)
{
  /* calc marco's output region, not node */
  int id;
  int s7mask=0;
  int s7pos;
  int s7pos2;
  int q3pos;
#  define S7MASK_LOCK 1
#  define S7MASK_BREAK 2
/*#  define S7MASK_LOCKPLUS 4*/
  /*struct pc_tree_node *p7  = null;*/
  struct pc_tree_node *pv;
  
  if (pc == null)  
    return  ;
  if (pc->child == null)
    return  ;
  if (* error_ == ktrue)
    return  ;

  id = 0;
  pv = pc->child;
  for (id = 0; id != pc->childs; pv = pv->level_n, id++)  {
    /*   case  type,  
         for the control flow, 
         extra output will only appear before-#if, and after #endif  */  
    struct  pp_tree_node *pp;
    struct  locate_essen_ essen;
    struct  uchar  *out;
    us buf[512];
    kbool result;

    if (  (s7mask & S7MASK_LOCK )  /* block it until # endif */
      && (pv->pc_type != PRE_COND_ENDIF)  ) 
      continue  ;
    switch   (pv->pc_type)   {
    case  PRE_COND_IF:
      q3pos = pv->line; /* mark < #if [#elif]* [#else] #endif > block start pos */
    case  PRE_COND_ELIF:
      /* build exprssion tree */
      locate_attach_weak (& essen, ust->line_gp[pv->line - 1]->content, null, pv->line);
      locate_resumefrom2 (& essen, pv->pt_left, pv->pt_right);
      if (pp_tree_build (& pp, & essen, err) != 0) 
        SET_PRRETURN_PRIVATE ();
      calcnode_pp (up2_chain, prc_chain, pp, & essen, error_, & result, err);
      pp_tree_uninit (pp);
      CHK_PRRETURN_PRIVATE ();
      if (result != kfalse)  {
    case  PRE_COND_ELSE:
         /* expr success, serach next vailed # endif block */
         s7mask |= S7MASK_LOCK;
         s7pos = pv->line;
         s7pos2 = pv->level_n->line;
         /* recur it */
         calcnode_pc (up2_chain, prc_chain, pv, ust, error_, null, err);
         CHK_PRRETURN_PRIVATE ();
      }
      continue  ;
    case  PRE_COND_ENDIF:
      if (! (s7mask & S7MASK_LOCK)) {
        /*chunk is discard */ 
        discard2_pc (ust, q3pos, pv->line );
      } else  {
        s7mask &= ~S7MASK_LOCK; 
        discard3_pc (ust, q3pos, pv->line, s7pos +1, s7pos2- 1);
      }
      continue ;
    case  PRE_COND_DEFINE:
      /* define */
      locate_attach(& essen, ust->line_gp[pv->line-1]->content, pv->line );
      locate_resumefrom2(& essen, pv->pt_left, pv->pt_right);
      if (exprQ (& essen, prc_chain, err) != 0)  {
        SET_PRRETURN_PRIVATE ();
      }
      discard_pc (ust, pv->line);
      continue  ;
    case  PRE_COND_ERROR:
    case  PRE_COND_OUTPUT:
      /* outinfos */
      locate_attach(& essen, ust->line_gp[pv->line-1]->content, pv->line );
      locate_resumefrom2(& essen, pv->pt_left, pv->pt_right);
      if (exprS (& essen, &out, err) != 0)  {
        SET_PRRETURN_PRIVATE ();
      }
      kprint ( _GR ("precond-%ls[%d]:%ls\r\n"), 
                  pv->pc_type != PRE_COND_OUTPUT 
                      ? _GR ("error") 
                      : _GR ("output"), 
                      pv->line,
                    out->str  );
      uchar_uninit (& out);
      if (pv->pc_type == PRE_COND_ERROR)
        SET_PRRETURN_PRIVATE ();
      discard_pc (ust, pv->line);
      continue  ;
    }
  }
}

#  if  0
  int  main  (void)   {

    struct uchar *err  ;
    struct uchar *ppp ;
    struct locate_essen_ essen;
    struct list_ *a, *b;
    struct pp_tree_node *ppc;
    struct uchar *ss;
    kbool scm, ip = kfalse;
    int sss;
    uchar_ginit ();
    uchar_init (& ppp);
    uchar_init (& ss);
    list_init (& a);
    list_init (& b);
    uchar_assign2 (ppp, _GR ("(((((  1) || 0))) || 1)"));
                  //          0123456789ABCD
    locate_attach_weak (& essen, ppp, null, 5);
    pp_tree_build (& ppc, & essen, ss);
   calcnode_pp (a, b, ppc, & essen,  & ip, & scm, ss);

   return ;



  }

#  endif 

/*  simple macro processing, same as clang */
  int precond_set (  LIST_TYPE2_(userpath2) up2_chain, 
                     LIST_TYPE2_(precond_) prc_chain, struct ustream *ust, struct uchar *err) {

  int id; 
  int sigr = -1;
  int curd = 0;
  us xfb[512];
  struct locate_essen_ es;
  struct locate_desc_ ld[2]; /*serach # +var [IF|ELIF|ELSE|ENDIF....]*/ 
  struct pc_tree_node *pn = null;
  struct pc_tree_node *p7 = null;
  struct pc_tree_node *p8 = null, *p9;
  union {
    struct pc_tree_node *pc[512];
    intptr_t ip[512];
  }  tp_stack;
  int                  tiny_stackcount = 0;
# define CBMASK_ELSE 1
# define CBMASK_IF 2
  /* weak type check  */
  kbool else_ = kfalse ;
  kbool if_ = kfalse;
  ld[1].etype = LOCATE_TYPE_VARIABLE;
  LOCATE_SETACC_RELAXED_CONST (& ld[0], _GR ("#"));
  
  /* init head*/
  pc_node_init (& p7);
  p7->pc_type = PRE_COND_NIL; 
  p9 = p8 = p7;
  for (id = 0; id != ust->total; id++)  {
    struct raw_line *aw = ust->line_gp[id];
    if (aw->invalid != kfalse) 
      continue  ; 
    /* serach condblock  */ 
    locate_attach (& es, aw->content, id + 1);
    if (locate_searchp (& es, & ld[0], 2, null) != 0) {
      continue ;
    }  
    {
      int kc = 0;
      enum PRE_COND_ pecond = getpc (& es.buf->str[ld[1].ac_pos], ld[1].ac_lens);

      if (pecond == PRE_COND_ABNORMAL) {
        /* cond abnormal quit **/
          es.buf->str[ld[1].ac_pos+ld[1].ac_lens] = 0;
          uchar_assign3 (err, _GR ("precond-error:[%d] !unknown precond-type:%ls.\r\n"), id + 1, & es.buf->str[ld[1].ac_pos]);
          achieve cleanup ;
      }
      kc = locate_clearrc (& es); /* remove space and comment */
      /*simple semantics check *.*/
      if (pecond == PRE_COND_ELIF || pecond == PRE_COND_ELSE) {
        /* check semantics, not allowed no more #else/#elif behind #else. */
        if (else_ != kfalse)  {
          uchar_assign3 (err, _GR ("precond-error:[%d] !not allowed no more #else and #elif behind #else.\r\n"), id + 1);
          achieve cleanup ;
        } else if (pecond == PRE_COND_ELSE) {
          else_ = ktrue;
        }
      }

      if (  pecond == PRE_COND_ELSE 
        ||     pecond == PRE_COND_ELIF
         || pecond == PRE_COND_ENDIF) {
        /* check semantics, #else/#elif/#endif is not allowed in blocks that do not begin with #if. */
        if (if_ != ktrue)  {
          uchar_assign3 (err, _GR ("precond-error:[%d] !#else/#elif/#endif is not allowed in blocks that do not begin with #if.\r\n"), id + 1);
          achieve cleanup ;
        } 
      }
        /* match condtype */ 
      switch (pecond)  {
      case PRE_COND_ELSE: /* check tail pure */
      case PRE_COND_ENDIF: /* check tail pure */
        if   (kc !=  0)  {
          /* not pure, there are impurities behind the #else| #endif */
          uchar_assign3 (err, _GR ("precond-error:[%d] !there are impurities behind the #else|#endif\r\n"), id + 1);
          achieve cleanup ; 
      case PRE_COND_IF: /* check tail exp */
      case PRE_COND_ELIF:/* check tail exp */
      case PRE_COND_OUTPUT:/* check tail string */
      case PRE_COND_ERROR:/* check tail string */
      case PRE_COND_DEFINE:/* check tail string */
          if (kc == 0)  {
            /* there is no expression behind the #define| #if| #elif|#output|#error */
            uchar_assign3 (err, _GR ("precond-error:[%d] !no expression behind the #define| #if| #elif|#output|#error\r\n"), id + 1);
            achieve cleanup ;
          }
        }
       
        /* stack emu recursion match condblock-if */ 
        pc_node_init (& pn);
        pn->line = id + 1;
        pn->len = es.doing.length;
        pn->pc_type = pecond;
        pn->pt_left = es.doing.apos;
        pn->pt_right = es.doing.apos + es.doing.length -1;

#   define INSERT_LIST_PRVATE(PPpp,CCcc)               \
        do  {                                         \
          if (PPpp->child == null)                    \
            PPpp->child = CCcc;                       \
          else  {                                     \
            /* insert tail  */                        \
            CCcc->level_n = PPpp->child;              \
            CCcc->level_p = PPpp->child->level_p;     \
            PPpp->child->level_p->level_n = CCcc;     \
            PPpp->child->level_p = CCcc;              \
          }                                           \
          PPpp->childs++;        /* setparent */      \
          CCcc->parent = PPpp;                        \
        } while (0)

        switch   (pn->pc_type)   {
        case  PRE_COND_IF:
          tp_stack.pc[tiny_stackcount++] = pn;
          tp_stack.ip[tiny_stackcount-1] |= else_ ? CBMASK_ELSE : 0;
          tp_stack.ip[tiny_stackcount-1] |= if_ ? CBMASK_IF : 0;
          INSERT_LIST_PRVATE (p8, pn);
          p7 = p8;
          p8 = pn;
          if_ = ktrue;
          else_ = kfalse;
          break  ;
        case  PRE_COND_ELIF:
        case  PRE_COND_ELSE:   
          INSERT_LIST_PRVATE (p7, pn);
          p8 = pn;
          break  ;
        case  PRE_COND_ENDIF:
          INSERT_LIST_PRVATE (p7, pn);
          p8 = (struct pc_tree_node *)
            (tp_stack.ip[--tiny_stackcount] & ~ ((intptr_t)(CBMASK_IF | CBMASK_ELSE)));
          if_ = (tp_stack.ip[tiny_stackcount] & CBMASK_IF) ? ktrue : kfalse;
          else_ = (tp_stack.ip[tiny_stackcount] & CBMASK_ELSE) ? ktrue : kfalse;
          p8 = p8->parent;
          p7 = p8->parent;      
          break  ;
        case  PRE_COND_ERROR:
        case  PRE_COND_OUTPUT:
        case  PRE_COND_DEFINE:
        default :
          INSERT_LIST_PRVATE (p8, pn);
          break  ;
        }
      }
    }
  }

  if (tiny_stackcount != 0)  {
    uchar_assign3 (err, _GR ("precond-error:[%d] ! #if/#endif mismatch\r\n"));
    achieve cleanup ;
  }

  /*  test whether the tree conforms to semantics */
  if (p9->childs <= 0)  {
    /* nodeal */ 
    pc_tree_uninit (p9);
    return  0;
  }  else  {
    kbool error_ = kfalse;
    calcnode_pc(up2_chain, prc_chain, p9, ust, & error_, null, err);
    pc_tree_uninit (p9);
    if (error_ != kfalse)     
      return -1;
    else  ;
    return 0;
  }
cleanup:
  pc_tree_uninit (p9);
  return -1;
}

#  if  0
int  main  (void)   {

  /* 
  sample

#if  1
             sads
             sd
   #if MSDAS
                 dsa
                 dsa
   #else 
   
                 dsa
   #endif 
   
   
   #if 0 || 5         
                 dsa
                 dsa
   #else 
   #output "HELLO"
                 dsa
   #endif 
   
      #output "HELLO2"
# else 

# endif 

   #output "HELLO3"
  */
  struct uchar *err  ;
  struct uchar *ppp ;
  struct locate_essen_ essen;
  struct list_ *a, *b;
  struct pp_tree_node *ppc;
  struct uchar *ss;
  struct ustream *iis;
  kbool scm, ip = kfalse;
  int sss;
  uchar_ginit ();
  uchar_init (& ppp);
  uchar_init (& ss);
  list_init (& a);
  list_init (& b);
  open_usfile (L"D:\\sd.esc", & iis);
  sss = precond_set (a, b, iis, ppp);\
  outinfos_usfile (iis);
  list_uninit (& a);
  list_uninit_ISQ (& b, precond_uninit);
  uchar_uninit (& ppp);
  uchar_uninit (& ss);
  close_usfile (& iis);
  uchar_guninit ();
  leak_infos_get ();

  return ;



}

#  endif 