/*
simple statement analysis,
this chunk's source code so bad ...
need to optimize a lot of places
*/

# include "analysis.h"
# include "locate.h"
# include "env.h"
# include "path.h"
# include "file.h"
# include "cppmmap.h"
# include "pattern.h"
# include "analysis.replcae.inl"
# include "analysis.chunk.inl"
# include "analysis.pass_base.inl"

// <shell_desc>==========================================================================================
void sheel_desc_init (struct shell_desc **sh_desc) {
  struct shell_desc *sh_ = umalloc (sizeof ( struct shell_desc));
  sh_->command = null;
  sh_->flags = 0;
  uchar_init (& sh_->command);
  * sh_desc = sh_;
}

void shell_desc_uninit (struct shell_desc **sh_desc) {
  if (sh_desc != null) {
    struct shell_desc *sh_ = * sh_desc;
    * sh_desc = null;
    if (sh_ != null) {
      uchar_uninit (& sh_->command);
      ufree (sh_);
    }
  }
}

void kcache_chain_uninit (struct kcache_chain **kcc_) {
  if (kcc_ != null) {
    struct kcache_chain *kcc = * kcc_;
    * kcc_ = null;
    if (kcc != null) {
      list_uninit_ISQ (& kcc->dt_chain, uchar_uninit);
      uchar_uninit (& kcc->attach);
      ufree (kcc);
    }
  }
}

int etreeroot_do (struct pass_node *pnrt, 
            struct analysis *lysis,
            struct list_ *chunk_chain, 
            struct uchar *ana, int line, struct uchar *err)
{
  /* left value rule: only the leaf node of the tree, 
     the right value must not appear in the current tree. (avoid the death of the ring)
     */
  struct locate_desc_ ld[6];
  struct locate_essen_ es;
  struct alter_ *ate = null;
  struct pass_node *pnt;
  int fvail = 0;

  ld[1].etype = LOCATE_TYPE_VARIABLE;
  locate_attach_weak (& es, ana, null, line);
  LOCATE_SETACC_RELAXED_CONST (& ld[0], _GR("."));
  LOCATE_SETACC_RELAXED_CONST (& ld[2], _GR(":="));
  LOCATE_SETACC_RELAXED_CONST (& ld[3], _GR("|"));

  if (locate_searchp(& es, & ld[0], 3, null) == 0)  {
    int n = wcslen (_GR ("etreeroot"));
    if (n == ld[1].ac_lens 
      && (wcsncmp (_GR ("etreeroot"), & ana->str[ld[1].ac_pos], n) == 0))  {
      pnt = pnrt; //FIXME:user's etreeroot
    } else  {
      kbool inter = kfalse;
      kbool isleaf = kfalse;
      isLeafnode (pnrt, & ana->str[ld[1].ac_pos], ld[1].ac_lens, & inter, & isleaf, & pnt);
      if (isleaf == kfalse)  {
        uchar_assign3 (err, _GR ("analysis-error:[%d] !deadring gener \r\n"), line);
        return -1;
      } 
    }
    
    /* serach rightval */
    do  {
      if (locate_searchp(& es, & ld[1], 1, null) == 0)  {
         /* check node **/
        kbool isexist = kfalse; 
        kbool inter = kfalse ;
        isExistnode (pnrt, & ana->str[ld[1].ac_pos], ld[1].ac_lens, & inter, & isexist);
        if (isexist != kfalse)  {
          ana->str[ld[1].ac_pos+ld[1].ac_lens] = 0;
          uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !rightvar's node exist \r\n"), line, & ana->str[ld[1].ac_pos]);
          return -1;
        } else  {
          /* serach file's chunk */
          struct list_v *p;
          int id;
          LIST_FOREACH (chunk_chain, id, p)  {
            if (p->ccp_o->ect_type == CHUNK_TYPE_PASS)  {
              if (uchar_cmp3maxs (p->ccp_o->passname, & ana->str[ld[1].ac_pos], ld[1].ac_lens) == 0)  {
                struct chunkccp *ccp = list_pop_L2 (chunk_chain, p);
                struct pass_node *pnd;
                pass_node_init (& pnd);  
                /* shard local varlink  */
                list_insert_tail (lysis->gsym, pnd->varChain);
                pnt->childs++;
                pnd->parent = pnt;
                pnd->sstart = ccp->sstart;
                pnd->sslen = ccp->len;
                uchar_assign0 (pnd->passname, ccp->passname);
                uchar_assign0 (pnd->varChain->passname, ccp->passname);
                chunkccp_uninit (& ccp);

                /* insert parent's tails*/
                if (pnt->child == null)  {
                  pnt->child = pnd; 
                } else  {
                  pnd->level_n = pnt->child;              
                  pnd->level_p = pnt->child->level_p;     
                  pnt->child->level_p->level_n = pnd;     
                  pnt->child->level_p = pnd;  
                }

                if (locate_uselessdata (& es) == 0) 
                  return 0;
                if (locate_searchp(& es, & ld[3], 1, null) != 0)  {
                  uchar_assign3 (err, _GR ("analysis-error:[%d] !no find | \r\n"), line);
                  return -1;
                }
                achieve out__;
              }
            }
          }
          ana->str[ld[1].ac_pos+ld[1].ac_lens] = 0;
          uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !no exist pass \r\n"), line, & ana->str[ld[1].ac_pos]);
          return -1;
out__:    continue  ;
        }
      } else {
        uchar_assign3 (err, _GR ("analysis-error:[%d] !serach passname failed \r\n"), line);
        return -1;
      }
    }  while (ktrue);
  }
  uchar_assign3 (err, _GR ("analysis-error:[%d] !etreeroot_do failed \r\n"), line);
  return -1;
}

int do_etreeroot ( struct pass_node **pn,
                 struct analysis *lysis,
                 struct list_ *chunk_chain,
                 struct ustream *ust, 
                 struct uchar *err)  
{
  int id;
  struct chunkccp *ccp = list_pop2 (chunk_chain, 0);
  struct pass_node *nrt = null;
  assert (ccp->ect_type == CHUNK_TYPE_ETREEROOT);

  pass_node_init(& nrt);
  list_insert_tail (lysis->gsym, nrt->varChain);

  for (id = ccp->sstart - 1; ccp->len >= 0; id++, ccp->len--)  {
    struct raw_line *rl = ust->line_gp[id-1];
    if (rl->invalid != kfalse)
      continue ;
    if (etreeroot_do (nrt, lysis, chunk_chain, rl->content, id, err) != 0)  
      achieve cleanup;
    rl->invalid = ktrue;
  }
  * pn = nrt;
  chunkccp_uninit(& ccp);
  return 0;
cleanup:
  chunkccp_uninit(& ccp);
  pass_tree_uninit (nrt);
  return -1;
}

/* set var op - init */
void setVar_init2 (struct set_var **var, uconst us *name, int len) {
  struct set_var *sv = umalloc (sizeof(struct set_var));
  list_init (& sv->spath_chain);
  uchar_init (& sv->name);
  uchar_assign (sv->name, name, len);
  * var = sv;
}

/* set var op - uninit */
void setVar_uninit (struct set_var **var) {
  if (var != null) {
    struct set_var *va = *var;
    *var = null;
    if (va != null) {
      if (va->spath_chain != null)
        list_uninit_ISQ(& va->spath_chain, spath_uninit);
      if (va->name != null)
        uchar_uninit (& va->name);
      ufree (va);
    }
  }
}

/* string var op - init */
void stringVar_init2 (struct string_var **sv, uconst us *name, int len) {
  struct string_var *p = umalloc (sizeof(struct string_var));
  uchar_init (& p->mapper);
  uchar_init (& p->name);
  uchar_clearbuf(p->mapper);
  uchar_assign (p->name, name, len);
  assert (sv != null);
  * sv = p;
}

/* string var op - uninit */
void stringVar_uninit (struct string_var **sv) {
  if (sv != null) {
    struct string_var *p = *sv;
    *sv = null;
    if (p!= null) {
      uchar_uninit (& p->mapper);
      uchar_uninit (& p->name);
      ufree (p);
    }
  }
}

// <pass_varChain>==========================================================================================
void pass_varChain_init (struct pass_varChain **pv)  {
  struct pass_varChain *p = umalloc (sizeof (struct pass_varChain));
  uchar_init (& p->passname);
  list_init (& p->set_chain);
  list_init (& p->string_chain);
  * pv = p;
}

void pass_varChain_uninit (struct pass_varChain **pv)  {
  if (pv != null)  {
    struct pass_varChain *p = *pv;
    *pv = null;
    if (p != null)  {
      list_uninit_ISQ (& p->set_chain, setVar_uninit);
      list_uninit_ISQ (& p->string_chain, stringVar_uninit);
      uchar_uninit (& p->passname);
      ufree (p);
    }
  }
}
 
//<SERACH/FIND VAR>======================================================
struct pass_varChain *
  varChain_findg (LIST_TYPE2_(pass_varChain) gsym, uconst us *pass_name)
{
  int id;
  struct list_v *p;

  if (gsym == null)
    return null;
  if (gsym->nums <= 0)
    return null;

  LIST_FOREACH(gsym, id, p) {
    if (!uchar_cmp2 (p->pass_varChain_o->passname, pass_name)) 
      return p->pass_varChain_o;
    else ;
  }
  return null;
}

struct pass_varChain *
  varChain_findg2 (LIST_TYPE2_(pass_varChain) gsym, uconst us *pass_name, int len)
{
  int id;
  struct list_v *p;

  if (gsym == null)
    return null;
  if (gsym->nums <= 0)
    return null;

  LIST_FOREACH(gsym, id, p) {
    if (!uchar_cmp3maxs (p->pass_varChain_o->passname, pass_name, len)) 
      return p->pass_varChain_o;
    else ;
  }
  return null;
}

struct string_var *
  stringVar_find (struct pass_varChain *varChain, uconst us *var_name)
{
  int id;
  struct list_v *p;

  if (varChain == null)
    return null;
  if (varChain->string_chain->nums <= 0)
    return null;

  LIST_FOREACH(varChain->string_chain, id, p) {
    if (!uchar_cmp2(p->string_var_o->name, var_name))
      return p->string_var_o;
    else ;
  }
  return null;
}

struct string_var *
  stringVar_find2 (struct pass_varChain *varChain, us *var_name, int len)
{
  int id;
  struct list_v *p;

  if (varChain == null)
    return null;
  if (varChain->string_chain->nums <= 0)
    return null;

  LIST_FOREACH(varChain->string_chain, id, p) {
    if (!uchar_cmp3maxs(p->string_var_o->name, var_name, len))
      return p->string_var_o;
    else ;
  }
  return null;
}


struct set_var *
  setVar_find2 (struct pass_varChain *varChain, us *var_name, int len)
{
  int id;
  struct list_v *p;

  if (varChain == null)
    return null;
  if (varChain->set_chain->nums <= 0)
    return null;

  LIST_FOREACH(varChain->set_chain, id, p) {
    if (!uchar_cmp3maxs(p->set_var_o->name, var_name, len))
      return p->set_var_o;
    else ;
  }
  return null;
}


struct set_var *
  setVar_find (struct pass_varChain *varChain, uconst us *var_name)
{
  int id;
  struct list_v *p;

  if (varChain == null)
    return null;
  if (varChain->set_chain->nums <= 0)
    return null;

  LIST_FOREACH(varChain->set_chain, id, p) {
    /* check/find pass name */
    if (!uchar_cmp2(p->set_var_o->name, var_name))
      return p->set_var_o;
    else ;
  }
  return null;
}

static 
struct string_var *
  stringVar_findg (struct list_ *gsym, uconst us *pass_name,
  uconst us *var_name)
{
  return stringVar_find 
    (  varChain_findg (gsym, pass_name), var_name);
}

static
struct set_var *
  setVar_findg (struct list_ *gsym, uconst us *pass_name,
  uconst us *var_name)
{
  return setVar_find 
    (  varChain_findg (gsym, pass_name), var_name);
}

int lvalue_get (struct leftVal_desc *leftv,
                     struct locate_essen_ *essen,
                struct pass_node *self,
                struct analysis *lysis,
                struct uchar *err)
{
  /* search keyword first */
  int id2;
  struct uchar *pas = self->passname;
  struct locate_desc_ ld[8];
  struct pass_varChain * g;
  LOCATE_SETACC_RELAXED_CONST (& ld[4], _GR ("."));
  LOCATE_SETACC_SYMBOLS_RELAXED_CONST2 (& ld[5], _GR  ("@"));
  ld[3].etype = LOCATE_TYPE_VARIABLE;
  ld[6].etype = LOCATE_TYPE_VARIABLE;
  g = self->varChain;

  if ( locate_searchp (essen, & ld[3], 1, null) == 0) {
    for (id2 = 0; id2 != sizeof(gs_Keyword)/ sizeof(gs_Keyword[0]); id2++) {
      if (wcslen (gs_Keyword[id2]) == ld[3].ac_lens 
        && ! wcsncmp (ld[3].acceptORacc, gs_Keyword[id2], ld[3].ac_lens))
        break;
      else ;
    }

    switch (id2) {
    case KEYWORD_OP_DEL:       // _GR ("del"), 
      leftv->uop_enum = UOP_KEYWORD_DEL;
      return 0;
    case KEYWORD_OP_DEL_SYNC:  // _GR ("del_sync"), 
      leftv->uop_enum = UOP_KEYWORD_DEL_SYNC;
      return 0;
    case KEYWORD_OP_ECHO:      // _GR ("echo"), 
      leftv->uop_enum = UOP_KEYWORD_ECHO;
      return 0;
    case KEYWORD_OP_ECHO_SYNC: // _GR ("echo_sync"),
      leftv->uop_enum = UOP_KEYWORD_ECHO_SYNC;
      return 0;
    case KEYWORD_OP_MKDIR:     // _GR ("mkdir"),
      leftv->uop_enum = UOP_KEYWORD_MAKEDIR;
      return 0;
    case KEYWORD_OP_SYS:       // _GR ("sys"),
      leftv->uop_enum = UOP_KEYWORD_SYS;
      return 0;
    case KEYWORD_OP_SYS_SYNC:  // _GR ("sys_sync")
      leftv->uop_enum = UOP_KEYWORD_SYS_SYNC;
      return 0;
    case KEYWORD_OP_STAT:
      leftv->uop_enum = UOP_KEYWORD_STAT;
      return 0;
    default:
      if (     1        ) {
      /* case:passname perfix 
         case:string without passname suffix */
        g = varChain_findg2 (    lysis->gsym,  
                                   ld[3].acceptORacc,
                              ld[3].ac_lens        );
        if (g != null) {
          /* perfix           */
          if (locate_searchp (essen, & ld[4], 3, null) == 0)
            if (ld[5].sig_match == 0) /* string*/
              achieve __strjmp;
            else if (ld[5].sig_match == 1) /* set*/
              achieve __setjmp;
            else ;
          else ;
          uchar_assign3(err, _GR("analysis-error[%d]:error behind passname perfix"), essen->line);
          return -1;
        } else  {
          struct string_var *sv;
          ld[6].ac_pos = ld[3].ac_pos;
          ld[6].ac_lens = ld[3].ac_lens;
          g = self->varChain;
__strjmp:       
          if ((sv = stringVar_find2 (g, ld[6].acceptORacc, ld[6].ac_lens)) == null) {
              /* left value not init -str var */
            stringVar_init2 (& sv, ld[6].acceptORacc, ld[6].ac_lens);
            list_insert_tail (g->string_chain, sv);
          }
          leftv->pvc = g;
          leftv->set_chain = g->set_chain;
          leftv->string_chain = g->string_chain;
          leftv->string = sv;

          /* string - get op command */
          for (id2 = 0; id2 != sizeof(gs_stringVar_Op)/ sizeof(gs_stringVar_Op[0]); id2++) {
            LOCATE_SETACC_TIGHTLY_CONST (& ld[0], gs_stringVar_Op[id2]);
            if (! locate_searchp (essen, & ld[0], 1, null)) {
              switch (id2) {
              case STRINGVAR_ASSGIN: //  _GR (":="),  
                leftv->uop_enum = UOP_STRING_ASSIGN;
                return 0;
              case STRINGVAR_APPEND: //  _GR ("+="),  
                leftv->uop_enum = UOP_STRING_APPEND;
                return 0;
              case STRINGVAR_PRECENT_ASSIGN_: // _GR ("%="),
                leftv->uop_enum = UOP_STRING_PERCENT_ASSIGN;
                return 0;
              case STRINGVAR_PRECENT_APPEND_: // _GR ("%+"),
                leftv->uop_enum = UOP_STRING_PERCENT_APPEND;
                return 0;
              default :
                assert (0);
              }
            }
          }
          /* #error:not find symbols */
   __cctac:
          uchar_assign3(err, _GR("analysis-error[%d]:leftVal mismatch or error op mode"), essen->line);
          return -1;
        } 
      }
    }
  } else {
    /* only @filevar */
    if (locate_searchp (essen, & ld[5], 2, null) != 0 || (ld[5].sig_match != 1)) {
      uchar_assign3(err, _GR("analysis-error[%d]:mismtach type or exp"), essen->line);
      return -1;
    } else {

        struct set_var *fv;
        g = self->varChain;
__setjmp:       
        if ((fv = setVar_find2 (g, ld[6].acceptORacc, ld[6].ac_lens)) == null) {
                    /* left value not init -set var */
          setVar_init2 (& fv, ld[6].acceptORacc, ld[6].ac_lens);
          list_insert_tail(g->set_chain, fv);
        }
        leftv->pvc = g;
        leftv->set_chain = g->set_chain;
        leftv->string_chain = g->string_chain;
        leftv->set = fv;

        /* file - get op command */
        for (id2 = 0; id2 != sizeof(gs_setVar_Op)/ sizeof(gs_setVar_Op[0]); id2++) {
          LOCATE_SETACC_TIGHTLY_CONST (& ld[0], gs_setVar_Op[id2]);
          if (! locate_searchp (essen, & ld[0], 1, null)) {
            switch (id2) {
            case SETVAR_AT: //  _GR ("@="),  
              leftv->uop_enum = UOP_SET_AT;
              return 0;
            case SETVAR_ASSIGN: //  _GR (":="),  
              leftv->uop_enum = UOP_SET_ASSIGN;
              return 0;
            case SETVAR_SUB: // _GR ("-="),
              leftv->uop_enum = UOP_SET_SUB;
              return 0;
            case SETVAR_APPEND: // _GR ("+="),
              leftv->uop_enum = UOP_SET_APPEND;
              return 0;
            default :
              assert (0);
            }
          }
        }
        achieve __cctac;
    }
  }
  return -1;
}



void depcpp_list_cmpset (  LIST_TYPE2_(cppdep_node2) cppn, 
                            LIST_TYPE2_(dep_cache) depcc, 
                            LIST_TYPE2_(uchar) collect,  
                              kbool *contaminated)  
{
  int id;
  struct list_v *p;

  if (cppn == null) 
    return  ;
  else ;

  /* foreach cppdep_node2 node */
  LIST_FOREACH (cppn, id, p)  {
    int id2;
    struct list_v *p2;
    struct uchar *pq;
    struct cppdep_node2 *attach_temp = p->cpp2node_o;
    /* foreach dep_cache  */
    LIST_FOREACH (depcc, id2, p2)  {
      if (! uchar_cmp(p2->depc_o->agpath, attach_temp->atgpath))  {
        if (p2->depc_o->timeStamp64 != attach_temp->timeStamp64)
          *  contaminated = ktrue;   
        achieve __appenditem  ;
      }
    }
    *  contaminated = ktrue;
__appenditem:
    pq = null;
    uchar_copy (& pq, attach_temp->atgpath);
    list_insert_tail (collect, pq);
  }
  return  ;
}

void depcpp_listcc (    struct list_ **root, 
                        struct uchar *apfiles, 
                        struct uchar  *err,    
                        struct list_ *kcc_chain,
                        kbool *e_interrrupt  )
{
  int id ;
  struct list_v *p;
  struct list_ *s;
  struct list_ *st = null;
  struct kcache_chain *kc;
  struct uchar *bpath = null;
  struct cppdep_node2 *cnep = null;
  
  kbool isRoot = kfalse;
  kbool isInherit = kfalse;
  uint64_t rmod64 = 0; 

  if (*e_interrrupt)
    return  ;
  else if (*root != null)   {
    s  = *root;
    /* serach repeat include *.*/
    LIST_FOREACH (s, id, p) {
      if (!uchar_cmp (apfiles, p->cpp2node_o->atgpath))
        return ;
      else ;
    }
  } else {
     isRoot = ktrue;
     achieve Q2;
  }
  /* serach kcc_chain */
  LIST_FOREACH (kcc_chain, id, p) {
    if (!uchar_cmp (apfiles, p->kcc_o->attach)) {
      isInherit = ktrue;
      rmod64 = p->kcc_o->hdmod64;
      st = p->kcc_o->dt_chain;
      achieve Q3;
    }
  }
Q2:
  list_init (& st);
  if ( cppsrc_mapget_include ( st, apfiles, & rmod64 ) != 0)  {
    kprint2 (_GR ("cppdep-warning [%ls]: cppsrc_mapget_include failed"), apfiles->str);
    list_uninit_ISQ (& st, uchar_uninit);
    return ;
  }
Q3:
  /* collect header files node */
  cnep = umalloc (sizeof (struct cppdep_node2));
  cnep->timeStamp64 = rmod64;
  uchar_init (& bpath);
  uchar_init (& cnep->atgpath);
  slashclr_getpath (apfiles->str, apfiles->length, & id);
  assert (id >0 );
  uchar_assign (bpath, apfiles->str, id);
  uchar_assign0 (cnep->atgpath, apfiles);
  if (*root == null)
    list_init (root);
  else ;
  list_insert_tail (*root, cnep);
  if (isInherit != kfalse)
    achieve Q4;
  else ;

  /* get source files incldue */
  {
    /* append to ... */
    kc = umalloc (sizeof (struct kcache_chain));
    kc->hdmod64 = rmod64;
    kc->attach = null;
    uchar_copy ( & kc->attach, apfiles);
    kc->dt_chain = st;
    list_insert_tail (kcc_chain, kc);
 Q4:if  (st->nums > 0)  {
       /* poll child nums */
      struct list_v *p;
      struct uchar *ut = null;
      int id;
      LIST_FOREACH (st, id, p)  {
        int s;
        if ((s = path_inherit (& ut, bpath, p->uchar_o)) != 0)  {
          *e_interrrupt = ktrue;
          uchar_assign3 (err, _GR ("serach cpp dependency-error: path_inherit failed"), apfiles->str);
        } else { 
          uchar_shrink (ut, 1);
          depcpp_listcc (root, ut, err, kcc_chain, e_interrrupt);
        }
        if (*e_interrrupt) 
          break  ;
        else ;     
      }
      uchar_uninit (& ut);
    }
    uchar_uninit (& bpath);
  }
}

// some inline op 
// set := 
int set_assign_ (struct set_var *svar, struct uchar *suff, struct uchar *bpath, struct uchar *bbase) {

  int id;
  struct uchar *inherit = null;
  LIST_TYPE2_(uchar) pp= null;

  spath_chain_delete_spath_suff (svar->spath_chain, bbase, bpath, suff);

  // load from disk 
  // make path 
  path_inherit (& inherit, bpath, bbase);
  pp = set_serach (inherit->str, suff?suff->str:null, null);
  if (pp == null) {
    uchar_uninit (& inherit);
    return 0;
  } else {
    struct xpath *xp;
    struct spath *sp = null;
    xpath_init (& xp);
    uchar_assign0 (xp->suffix, suff);
    list_uninit_ISQ (& xp->kpath_dt, uchar_uninit);
    xp->kpath_dt = pp;
    sp = spath_chain_find_spath (svar->spath_chain, bbase, bpath);
    if (sp == null) {
      spath_init (& sp);
      uchar_assign0 (sp->path, inherit);
      list_insert_tail (svar->spath_chain, sp);
    }
    list_insert_tail (sp->xpath_dt, xp);
    uchar_uninit (& inherit);
    return 0;
  }
  return 0;
}

int set_assign2_ (struct set_var *svar, struct uchar *suff, struct uchar *bpath, struct uchar *bbase, struct list_ **pplist) {

  int id;
  struct uchar *inherit = null;
  LIST_TYPE2_(uchar) pp= null;

  spath_chain_delete_spath_suff (svar->spath_chain, bbase, bpath, suff);

  // load from disk 
  // make path 
  path_inherit (& inherit, bpath, bbase);
  pp = * pplist;
  * pplist = null;
  if (pp == null) {
    assert (0);
    uchar_uninit (& inherit);
    return 0;
  } else {
    struct xpath *xp;
    struct spath *sp = null;
    xpath_init (& xp);
    uchar_assign0 (xp->suffix, suff);
    list_uninit_ISQ (& xp->kpath_dt, uchar_uninit);
    xp->kpath_dt = pp;
    sp = spath_chain_find_spath (svar->spath_chain, bbase, bpath);
    if (sp == null) {
      spath_init (& sp);
      uchar_assign0 (sp->path, inherit);
      list_insert_tail (svar->spath_chain, sp);
    }
    list_insert_tail (sp->xpath_dt, xp);
    uchar_uninit (& inherit);
    return 0;
  }
  return 0;
}

int set_append_ (struct set_var *svar, struct uchar *suff, struct uchar *bpath, struct uchar *bbase) {

  int id;
  struct uchar *inherit = null;
  LIST_TYPE2_(uchar) pp= null;


  // load from disk 
  // make path 
  path_inherit (& inherit, bpath, bbase);
  pp = set_serach (inherit->str, suff?suff->str:null, null);
  if (pp == null) {
    uchar_uninit (& inherit);
    return 0;
  } else {
    struct xpath *xp;
    struct spath *sp;
    // check exist spath/xpath ?
    sp = spath_chain_find_spath (svar->spath_chain, bbase, bpath);
    if (sp == null) {
      spath_init (& sp);
      uchar_assign0 (sp->path, inherit);
      list_insert_tail (svar->spath_chain, sp);
    } 
    xp = spath_find_xpath (sp, suff);
    if (xp == null) {
      xpath_init (& xp);
      uchar_assign0 (xp->suffix, suff);
      list_insert_tail (sp->xpath_dt, sp);
    } 
    list_merge_dest (xp->kpath_dt, & pp);
    list_uninit_ISQ(& pp, uchar_uninit);
    uchar_uninit (& inherit);
    return 0;
  }
  return 0;
}

int set_append2_ (struct set_var *svar, struct uchar *suff, struct uchar *bpath, struct uchar *bbase, struct list_ **pplist) {

  int id;
  struct uchar *inherit = null;
  LIST_TYPE2_(uchar) pp= null;


  // load from disk 
  // make path 
  path_inherit (& inherit, bpath, bbase);
  pp = * pplist;
  * pplist = null;
  if (pp == null) {
    uchar_uninit (& inherit);
    return 0;
  } else {
    struct xpath *xp;
    struct spath *sp;
    // check exist spath/xpath ?
    sp = spath_chain_find_spath (svar->spath_chain, bbase, bpath);
    if (sp == null) {
      spath_init (& sp);
      uchar_assign0 (sp->path, inherit);
      list_insert_tail (svar->spath_chain, sp);
    } 
    xp = spath_find_xpath (sp, suff);
    if (xp == null) {
      xpath_init (& xp);
      uchar_assign0 (xp->suffix, suff);
      list_insert_tail (sp->xpath_dt, sp);
    } 
    list_merge_dest (xp->kpath_dt, & pp);
    list_uninit_ISQ(& pp, uchar_uninit);
    uchar_uninit (& inherit);
    return 0;
  }
  return 0;
}

int set_at_ (struct set_var *svar, struct uchar *suff, struct uchar *bbase) {

  int id;
  struct list_v *p;

  struct xpath *xp;
  struct spath *sp;

  /* VVV->xxx */
  if (svar->spath_chain->nums <= 0)
    return 0;
  else ;

  spath_init (& sp);
  xpath_init (& xp);
  uchar_assign0 (sp->path, bbase);
  uchar_assign0 (xp->suffix, suff);
  list_insert_tail (sp->xpath_dt, xp);

  LIST_FOREACH (svar->spath_chain, id, p)  {
    int id2;
    struct list_v *p2; 
    LIST_FOREACH (p->spath_o->xpath_dt, id2, p2)  {
      int id3;
      int id3cc= p2->xpath_o->kpath_dt->nums;
      struct list_v *p3 = p2->xpath_o->kpath_dt->set;
      struct list_v *p3cc; 
      for (id3= 0; id3 != id3cc; id3++, p3 = p3cc)  {
        p3cc = p3->level_n;
        /* check filename redefine */
      Q3:do  
         {
           int ud;
           struct list_v *up;
           LIST_FOREACH (xp->kpath_dt, ud, up)  {
             if (! uchar_cmp(up->uchar_o, p3->uchar_o))  {
#     if defined (_WIN32)
               GUID gd;
               CoCreateGuid(& gd);
               uchar_insert7 (p3->uchar_o, -1, _GR ("_redefine_guid0x%08x"), gd.Data1);
               uchar_insert7 (p3->uchar_o, -1, _GR ("%08x"), * (int *)& gd.Data2); // XXX:type .
               uchar_insert7 (p3->uchar_o, -1, _GR ("%08x"), * (int *)& gd.Data4[0]); // XXX:type .
               uchar_insert7 (p3->uchar_o, -1, _GR ("%08x"), * (int *)& gd.Data4[4]); // XXX:type .
#     endif 
               achieve Q3 ;
             }
           }
           list_pop_L (p2->xpath_o->kpath_dt, p3);
           list_insert_tail_L (xp->kpath_dt, p3);
           break  ;

         }  while (ktrue);
      }
    }
  }
  list_removeALL_ISQ (svar->spath_chain, spath_uninit);
  list_insert_tail (svar->spath_chain, sp);
  return 0;
}

int action_get (struct leftVal_desc *leftv,
                     struct locate_essen_ *essen,
                struct pass_node *self,
                struct analysis *lysis,
                struct uchar *err)
{
  int id;
  struct list_v *p;
  struct list_ *pplist = null;

  struct uchar *pptemp = null;
  struct __pattern pat;
  struct shell_desc *sh_ = null;
  pattern_init (& pat);



  /*  
  // <string> -IDEP|-IDEP_CPP|-ODEP|NULL
  string := expr_string_path_auto, <>, [ASL, ERST, PQAS], PREFIX_SET:expr_string, SUFFIX_SET?expr_string
  string += expr_string_path_auto, <>, [ASL, ERST, PQAS], PREFIX_SET:expr_string, SUFFIX_SET?expr_string
  // <set> -IDEP|-IDEP_CPP|-ODEP|-VBATTER|-HBATTER 
  set @= expr_string_path_auto, <>         // spath_set->one spath, suffix_set->one set 
  set := expr_string_path_auto, <>
  set := expr_string_path_auto, <>, [ASL, ERST, PQAS]
  set += expr_string_path_auto, <>
  set += expr_string_path_auto, <>, [ASL, ERST, PQAS]
  set -= expr_string_path_auto, <>
  set -= expr_string_path_auto, <>, [ASL, ERST, PQAS]
  // <del_sync>  
  del_sync expr_string_path_auto, <> [ASL, PSQA] 
  del_sync expr_string_path_auto, <> 
  // <del> 
  del expr_string_path_auto, <> [ASL, PSQA] 
  del expr_string_path_auto, <> 
  // <echo_sync>
  echo_sync expr_string_path_auto, <> [ASL, PSQA] 
  echo_sync expr_string_path_auto, <> 
  // <echo>
  echo expr_string_path_auto, <> [ASL, PSQA] 
  echo expr_string_path_auto, <>

  
  
  





  // <stat> 
  stat [expr_string_path_auto | expr_string | string | set ] -IDEP |-ODEP | -IDEP_CPP PREFIX_SET:expr_string, SUFFIX_SET?expr_string $ ANC
  *.**/
  // expr := setvar 
  //        | expr_string 
  //        | expr_string, <null | .suffixname>
  //        | expr_string, <null | .suffixname>, prefix_set | suffix_set 
  //        | expr_string, <null | .suffixname>, [block set]
  //        | expr_string, <null | .suffixname>, [block set], prefix_set | suffix_set 

  if ( (leftv->uop_enum == UOP_KEYWORD_STAT))
  {
    struct __pattern2 pat2;
    pattern2_init (& pat2);
    if (pattern2_preprocessed (& pat2, essen, self, lysis, err) != 0) {
      pattern2_uninit (& pat2);
      return -1;
    }
    if (pattern2_calc (& pat2, essen, self, lysis, err) != 0) {
      pattern2_uninit (& pat2);
      return -1;
    }
    pattern2_uninit (& pat2);
    achieve final_;
  }
  else 
  {
    if (pattern_readline (& pat, 
      essen, 
      self, lysis, err) != 0)
      achieve final_;
  }

  // mode - 1 only match UOP_SET_ASSIGN
  if (pat.amat == AMT_MATCH 
    && (leftv->uop_enum == UOP_SET_ASSIGN))
  {
    list_removeALL_ISQ (leftv->set->spath_chain, spath_uninit);
    spath_chain_copy (leftv->set->spath_chain, pat.set->spath_chain);
    achieve final_;
  }
  // mode - 2 expr_string
  // sys|sys_sync|mkdir|echo|echo_sync
  // string[%=]|string[%+]|string[:=]|string[+=]
  // set[-=]
  if (pat.amat == AMT_MATCH2) {
    switch (leftv->uop_enum) {
    case  UOP_STRING_PERCENT_ASSIGN:
      path_inherit2 (lysis->bpath, pat.expr_string);
      uchar_shrink (pat.expr_string, 1);
    case  UOP_STRING_ASSIGN: // expr_string 
      uchar_assign0 (leftv->string->mapper, pat.expr_string);
      achieve final_;
    case  UOP_STRING_PERCENT_APPEND:
      path_inherit2 (lysis->bpath, pat.expr_string);
      uchar_shrink (pat.expr_string, 1);
    case  UOP_STRING_APPEND:
      uchar_insert4 (leftv->string->mapper, -1, pat.expr_string);
      achieve final_;
    case  UOP_KEYWORD_ECHO:
      wprintf (pat.expr_string->str);
      achieve final_;
    case  UOP_KEYWORD_SYS:
# if defined (_WIN32)
      _wsystem (pat.expr_string->str);
# endif 
      achieve final_;
    case  UOP_KEYWORD_SYS_SYNC:
      sheel_desc_init (& sh_);
      sh_->flags = AUXILIARY_GENERATE_FLAGS_P;
      uchar_assign0 (sh_->command, pat.expr_string);
      list_insert_tail (self->shell_chain, sh_);
      achieve final_;
    case  UOP_KEYWORD_ECHO_SYNC:
      sheel_desc_init (& sh_);
      sh_->flags = AUXILIARY_GENERATE_FLAGS_S;
      uchar_assign0 (sh_->command, pat.expr_string);
      list_insert_tail (self->shell_chain, sh_);
      achieve final_;
    case  UOP_SET_SUB:
      spath_chain_delete_spath (leftv->set->spath_chain, pat.expr_string, lysis->bpath);
      achieve final_;
    case  UOP_KEYWORD_MAKEDIR:
# if defined (_WIN32)
      path_inherit2 (lysis->bpath, pat.expr_string);
      uchar_insert3 (pat.expr_string, 0, _GR ("md "));
      _wsystem (pat.expr_string->str);
      achieve final_;
# endif 
    default:  assert (0);
    }
  }  else if (pat.amat == AMT_MATCH3) {
    switch (leftv->uop_enum) {
    case  UOP_SET_ASSIGN: // set := 
      set_assign_ (leftv->set, pat.suffix, lysis->bpath, pat.expr_string);
      achieve final_;
    case  UOP_SET_APPEND: // set +=
      set_append_ (leftv->set, pat.suffix, lysis->bpath, pat.expr_string);
      achieve final_;
    case  UOP_SET_AT: // set @= vector to one .
      set_at_ (leftv->set, pat.suffix, pat.expr_string);
      achieve final_;
    case  UOP_SET_SUB: // set -= [not from disk]
      spath_chain_delete_spath_suff (leftv->set->spath_chain, pat.suffix, lysis->bpath, pat.expr_string);
      achieve final_;
    case  UOP_KEYWORD_DEL: // del
      path_inherit2 (pat.expr_string, lysis->bpath);
      pplist = set_serach (pat.expr_string->str, pat.suffix?pat.suffix->str:null, null);
      LIST_FOREACH (pplist, id, p) {
        uchar_copy (& pptemp, pat.expr_string);
        uchar_assign0 (pptemp, p->uchar_o);
        uchar_insert4 (pptemp, -1, pat.suffix );
        file_del (pptemp->str);      
      }
      uchar_uninit (& pptemp);
      list_uninit_ISQ (& pplist, uchar_uninit);
      achieve final_;
    case  UOP_KEYWORD_DEL_SYNC: // del_sync 
      path_inherit2 (pat.expr_string, lysis->bpath);
      pplist = set_serach (pat.expr_string->str, pat.suffix?pat.suffix->str:null, null);
      LIST_FOREACH (pplist, id, p) {
        sheel_desc_init (& sh_);
        sh_->flags = AUXILIARY_GENERATE_FLAGS_K;
        uchar_copy (& sh_->command, pat.expr_string);
        uchar_assign0 (sh_->command, p->uchar_o);
        uchar_insert4 (sh_->command, -1, pat.suffix );
        list_insert_tail (self->shell_chain, sh_);
      }
      list_uninit_ISQ (& pplist, uchar_uninit);
      achieve final_;
    default:  assert (0);
    }
  } else if (pat.amat == AMT_MATCH5) {
    switch (leftv->uop_enum) {
    case  UOP_SET_ASSIGN: // set := 
      set_assign2_ (leftv->set, pat.suffix, lysis->bpath, pat.expr_string, & pat.block_list);
      achieve final_;
    case  UOP_SET_APPEND: // set +=
      set_append2_ (leftv->set, pat.suffix, lysis->bpath, pat.expr_string, & pat.block_list);
      achieve final_;
    case  UOP_SET_SUB: // set -= [not from disk]
      spath_chain_delete_spath_suff_block (leftv->set->spath_chain, pat.suffix, lysis->bpath, pat.expr_string, pat.block_list);
      achieve final_;
    case  UOP_KEYWORD_DEL: // del
      path_inherit2 (pat.expr_string, lysis->bpath);
      LIST_FOREACH (pat.block_list, id, p) {
        uchar_copy (& pptemp, pat.expr_string);
        uchar_assign0 (pptemp, p->uchar_o);
        uchar_insert4 (pptemp, -1, pat.suffix );
        file_del (pptemp->str);      
      }
      uchar_uninit (& pptemp);
      achieve final_;
    case  UOP_KEYWORD_DEL_SYNC: // del_sync 
      path_inherit2 (pat.expr_string, lysis->bpath);
      LIST_FOREACH (pat.block_list, id, p) {
        sheel_desc_init (& sh_);
        sh_->flags = AUXILIARY_GENERATE_FLAGS_K;
        uchar_copy (& sh_->command, pat.expr_string);
        uchar_assign0 (sh_->command, p->uchar_o);
        uchar_insert4 (sh_->command, -1, pat.suffix );
        list_insert_tail (self->shell_chain, sh_);
      }
      achieve final_;
    default:  assert (0);
    }
  }
  // mode - 3 expr_string
  // sys|sys_sync|mkdir|echo|echo_sync
  // string[%=]|string[%+]|string[:=]|string[+=]
  // set[-=]
  /*
          // <set> -IDEP|-IDEP_CPP|-ODEP|-VBATTER|-HBATTER 
          set @= expr_string_path_auto, <>         // spath_set->one spath, suffix_set->one set 
          set := expr_string_path_auto, <>
          set += expr_string_path_auto, <>
          set -= expr_string_path_auto, <>
          del_sync expr_string_path_auto, <> 
          del expr_string_path_auto, <> 



final_:;











  switch (leftv->uop_enum) {





  }*/









final_:
pattern_uninit (& pat);
return 0;
}




void do_pass ( struct pass_node *pn, struct analysis *lysis, struct uchar *err, int * ret) {

  if (pn ==null)
    return ;
  else if ( 1)  {
    int id;
    struct pass_node *pp = pn->child;
    for (id = 0; id != pn->childs; pp = pp->level_n, id++) {
      if (*ret == -1)
        return ;
      else ;
      do_pass (pp, lysis, err, ret);
    }
    if (*ret == -1)
      return ;
    else ;
    if (pn->parent != null)  /* rott node nodo */
    {
      struct locate_essen_ ess;
      struct leftVal_desc la;
      int i;
      i = 5;
      for (id = pn->sstart; pn->sslen > 0; -- pn->sslen, id++)  {
        struct raw_line * rl = lysis->ust->line_gp[id-1];
        us xfb[1024];
        if (rl->invalid == kfalse)  {
          locate_attach (& ess, rl->content, id);
          if (lvalue_get (& la, & ess, pn, lysis, err) != 0) {
            * ret = -1;
            return ;
          }
          if (action_get (& la, & ess, pn, lysis, err) != 0) {
            * ret = -1;
            return ;
          }
        }
      }
    }
  }
}

void do_pass_shirk ( struct pass_node *pn) {

  if (pn ==null)
    return ;
  else if ( 1)  {
    int id;
    int iq = pn->childs;
    struct pass_node *pp = pn->child;
    struct pass_node *pcache;
    for (id = 0; id != iq; pp = pcache, id++)
      pcache = pp->level_n, do_pass_shirk (pp); /*recur it */
    if (pn->parent != null)  /* rott node nodo */
    {
      if (pn->shell_chain->nums <= 0)  {
          /* nodone , weak node, remove it*/
        struct pass_node *pg = pn->parent->child;
        if (pn->parent->childs == 1)  {  
          pn->parent->child = pn->child; /* merge child chain */
          pn->parent->childs = pn->childs;
        } else  {
          if (pg == pn) 
            pn->parent->child = pg->level_n; 
          pn->level_n->level_p = pn->level_p;
          pn->level_p->level_n = pn->level_n;
          pn->parent->childs --;
          /* link, marge child node chain */
          if (pn->childs > 0) {
            /* link child-block and parent's region out node */
            pn->child->level_p->level_n = pn->level_n;
            pn->child->level_p = pn->level_p;
            pn->level_p->level_n = pn->child;
            pn->level_n->level_p = pn->child->level_p;
            pn->parent->childs += pn->childs;
            if (pg == pn) 
              pn->parent->child = pn->child; 
          }
        }   
        pass_node_uninit (& pn);
      }
    }
  }
}


#  if 0


int main (void)
{
  struct cppdep_node *pn = null;
  struct uchar *g;
  struct uchar *p, *cc;

  kbool gc = kfalse;
  uchar_ginit ();
  uchar_init (&g);
  uchar_init (&p);
  uchar_init (&cc);
  uchar_assign2(g, _GR("E:\\umake\\"));
  uchar_assign2(p, _GR("E:\\umake\\analysis.c"));

  depcpp_treecc (& pn, null, g, p, cc, & gc);



  uchar_guninit ();
  recursion_include0 (pn);
}
#  endif 








#  if  0

void debug_output (struct ustream *s) {

  int id;
  for ( id =0; id != s->total; id++) {
    kprint (_GR("line:%d  "), id+1);
    //if (! s->line_gp[id]->invalid)
      kprint (s->line_gp[id]->content->str);
    //else kprint (_GR("invailed"));
    kprint (_GR("\n"));
  }

}
int  main  (void)   {

  struct uchar *err  ;
  struct uchar *ppp ;
  struct locate_essen_ essen;
  struct list_ *a, *b;
  struct list_ *ppl, cccc;
  struct pass_node *ppc;
  struct uchar *ss;
  struct ustream *iis;
  kbool scm, ip = kfalse;
  int sss;
  uchar_ginit ();
  uchar_init (& ppp);
  uchar_init (& ss);
  list_init (& a);
  list_init (& cccc);
  list_init (& b);
  open_usfile (L"D:\\sd.esc", & iis);
  serach_chunk(& ppl, iis, ppp);
  ae_replace (ppl, a, iis, ppp);
  do_etreeroot (& ppc, ppl, iis, ppp);



  debug_output (iis);








  list_uninit (& a);
  list_uninit (& b);
  uchar_uninit (& ppp);
  uchar_uninit (& ss);
  close_usfile (& iis);
  uchar_guninit ();
  leak_infos_get ();

  return ;



}


#endif 