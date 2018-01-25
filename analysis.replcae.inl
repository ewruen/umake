// alter chunk
// envs chunk do repleace 
// =========================================================================================
void 
alter_init (struct alter_ **p_ )  {
  struct alter_ *p = umalloc (sizeof(struct alter_));
  uchar_init (& p->name);
  uchar_init (& p->ext);
  * p_ = p;
}

void 
alter_uninit (struct alter_ **p_ )  {
  if (p_ != null)  {
    struct alter_ *p = * p_;
    * p_ = null;
    if (p != null)  {
      uchar_uninit (& p->ext);
      uchar_uninit (& p->name);
      ufree (p);
    }
  }
}

int 
alter_find (LIST_TYPE2_(alter_) list, us *name, int cnlen, struct uchar **alter_map)
{
  struct list_v *p;
  int id;

  LIST_FOREACH(list, id, p)  {
    if (uchar_cmp3(p->alt_o->name, name, cnlen) == 0)  {
      if (*alter_map == null) 
        uchar_init (alter_map);
      uchar_clearbuf(*alter_map);
      uchar_assign0(*alter_map, p->alt_o->ext);
      return 0;
    }
  }
  return -1;
}

int 
alter_find2 (LIST_TYPE2_(alter_) list, us *name, int cnlen)
{
  struct list_v *p;
  int id;

  LIST_FOREACH(list, id, p)  {
    if (uchar_cmp3(p->alt_o->name, name, cnlen) == 0)  {
      return 0;
    }
  }
  return -1;
}

int do_alter (  LIST_TYPE2_(alter_) alter_list, 
                LIST_TYPE2_ (userpath2) env2_list, 
                int line, 
                struct uchar *ana, 
                struct uchar *err )  {

  struct locate_desc_ ld[6];
  struct locate_essen_ es;
  struct locate_s_ ss;
  struct alter_ *ate = null;
  int fvail = 0;
  us buf[256];

  ld[1].etype = LOCATE_TYPE_VARIABLE;
  locate_attach (& es, ana, line);
  LOCATE_SETACC_RELAXED_CONST (& ld[0], _GR("$"));
  LOCATE_SETACC_RELAXED_CONST (& ld[2], _GR(":="));

  if (locate_clearrc (& es) == 0) 
    return 0;
  else  ;

  if (locate_searchp (& es, & ld[0], 3, & ss) == 0)  {
    struct uchar *emap = null;
    if (alter_find2 (alter_list, ld[1].acceptORacc, ld[1].ac_lens ) == 0)  {
      locate_setnullc (& ld[1]);
      uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !redef alter \r\n"), line, ld[1].acceptORacc);
      return -1;  
    }
    alter_init (& ate);
    uchar_assign(ate->name, ld[1].acceptORacc, ld[1].ac_lens);
    ld[0].etype = LOCATE_TYPE_STRING;
    ld[2].etype = LOCATE_TYPE_VARIABLE;
    ld[4].etype = LOCATE_TYPE_VARIABLE;
    LOCATE_SETACC_ACCURATE_RELAXED_CONST2 (& ld[3], _GR("$$"));
    LOCATE_SETACC_ACCURATE_RELAXED_CONST2 (& ld[5], _GR("+"));
    LOCATE_SETACC_ACCURATE_RELAXED_CONST2 (& ld[1], _GR("%%"));

    do 
    {
      if (locate_searchp (& es, & ld[0], 1, null) == 0)
        uchar_insert (ate->ext, -1, ld[0].acceptORacc, ld[0].ac_lens);
      else if (locate_searchp (& es, & ld[3], 2, null) == 0)  {
        if (alter_find (alter_list, ld[4].acceptORacc, ld[4].ac_lens, & emap ) != 0)  {
          locate_setnullc (& ld[4]);
          uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !undef alter \r\n"), line, ld[4].acceptORacc);
          achieve cleanup_l0;  
        }
        achieve L0;
      } else if (locate_searchp (& es, & ld[1], 2, null) == 0)  {
        if (env2_find (env2_list, ld[2].acceptORacc, ld[2].ac_lens, & emap ) != 0)  {
          locate_setnullc (& ld[2]);
          uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !undef env \r\n"), line, ld[2].acceptORacc);
          achieve cleanup_l0;  
        }
      L0: uchar_insert3 (ate->ext, -1, emap->str);
        uchar_uninit (& emap);
      } else {
        uchar_assign3 (err, _GR ("analysis-error:[%d] .alter expr errors \r\n"), line);
        achieve cleanup_l0;  
      }
      if (locate_uselessdata(&es) == 0)
        break ;
      else if (locate_searchp (& es, & ld[5], 1, null) != 0)  {
        achieve cleanup_l0;  
      }
    } while (ktrue);

    list_insert(alter_list, 0, ate);
    return 0;
  }
cleanup_l0:
  alter_uninit (& ate);
  return -1;
}

// XXX:repeat 
int do_envs2 (  LIST_TYPE2_(alter_) alter_list, 
                LIST_TYPE2_ (userpath2) env2_list, 
                int line, 
                struct uchar *ana, 
                struct uchar *bpath, 
                struct uchar *err )  {

  struct locate_desc_ ld[6];
  struct locate_essen_ es;
  struct locate_s_ ss;
  struct alter_ *ate = null;
  struct uchar *mci = null;
  int fvail = 0;
  us buf[256];

  ld[1].etype = LOCATE_TYPE_VARIABLE;
  locate_attach (& es, ana, line);
  LOCATE_SETACC_RELAXED_CONST (& ld[0], _GR("%"));
  LOCATE_SETACC_RELAXED_CONST (& ld[2], _GR(":="));

  if (locate_clearrc (& es) == 0) 
    return 0;
  else  ;

  if (locate_searchp (& es, & ld[0], 3, & ss) == 0)  {
    struct uchar *emap = null;
    if (env2_exist_weak (env2_list, ld[1].acceptORacc, ld[1].ac_lens ) == 0)  {
      locate_setnullc (& ld[1]);
      uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !redef envir \r\n"), line, ld[1].acceptORacc);
      return -1;  
    }
    alter_init (& ate);
    uchar_assign(ate->name, ld[1].acceptORacc, ld[1].ac_lens);
    ld[0].etype = LOCATE_TYPE_STRING;
    ld[2].etype = LOCATE_TYPE_VARIABLE;
    ld[4].etype = LOCATE_TYPE_VARIABLE;
    LOCATE_SETACC_ACCURATE_RELAXED_CONST2 (& ld[3], _GR("$$"));
    LOCATE_SETACC_ACCURATE_RELAXED_CONST2 (& ld[5], _GR("+"));
    LOCATE_SETACC_ACCURATE_RELAXED_CONST2 (& ld[1], _GR("%%"));

    do 
    {
      if (locate_searchp (& es, & ld[0], 1, null) == 0)
        uchar_insert (ate->ext, -1, ld[0].acceptORacc, ld[0].ac_lens);
      else if (locate_searchp (& es, & ld[3], 2, null) == 0)  {
        if (alter_find (alter_list, ld[4].acceptORacc, ld[4].ac_lens, & emap ) != 0)  {
          locate_setnullc (& ld[4]);
          uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !undef alter \r\n"), line, ld[4].acceptORacc);
          achieve cleanup_l0;  
        }
        achieve L0;
      } else if (locate_searchp (& es, & ld[1], 2, null) == 0)  {
        if (env2_find (env2_list, ld[2].acceptORacc, ld[2].ac_lens, & emap ) != 0)  {
          locate_setnullc (& ld[2]);
          uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !undef env \r\n"), line, ld[2].acceptORacc);
          achieve cleanup_l0;  
        }
      L0: uchar_insert3 (ate->ext, -1, emap->str);
        uchar_uninit (& emap);
      } else {
        uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] .envir expr errors \r\n"), line);
        achieve cleanup_l0;  
      }
      if (locate_uselessdata(&es) == 0)
        break ;
      else if (locate_searchp (& es, & ld[5], 1, null) != 0)  {
        achieve cleanup_l0;  
      }
    } while (ktrue);

    mci = null;
    // calc inherit path 
    path_inherit (& mci, bpath, ate->ext);
    env2_insert (env2_list, ate->name->str, mci->str, ate->name->length, mci->length);
    alter_uninit (& ate);
    uchar_uninit (& mci);
    return 0;
  }
cleanup_l0:
  alter_uninit (& ate);
  return -1;
}

int do_env2_replace ( LIST_TYPE2_ (userpath2) env2_list, int line, struct uchar *ana, struct uchar *err)
{
  struct locate_desc_ ld[2];
  struct locate_essen_ es;
  struct locate_s_ ss;
  int fvail = 0;

  ld[1].etype = LOCATE_TYPE_VARIABLE;
  LOCATE_SETACC_SYMBOLS_TIGHTLY_CONST (& ld[0], _GR("%"), 0, 3);
  locate_attach_weak(& es, ana, null, -1);

  if (locate_clearrc (& es) == 0) 
    return 0;
  else  ;

  while (ktrue)  {
    if (locate_searchp2 (& es, & ld[0], 2, & ss, & fvail) == 0)  {
      struct uchar *emap = null;
      if (env2_find (env2_list, & es.buf->str[ld[1].ac_pos], ld[1].ac_lens, & emap ) != 0)  {
        es.buf->str[ld[1].ac_pos+ld[1].ac_lens] = 0;
        uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !undef env \r\n"), line, & es.buf->str[ld[1].ac_pos]);
        return -1;
      }
      if (ld[0].sig_match == 2)  {
        uchar_insert(emap, 0, _GR ("`"), wcslen (_GR ("`")));
        uchar_insert(emap, -1, _GR ("`"), wcslen (_GR ("`")));
      }
      uchar_replace7( es.buf, fvail, es.doing.apos - fvail, emap->str);
      uchar_uninit (& emap);

      es.doing.apos = fvail;
      es.doing.length = es.buf->length - fvail;
      continue  ;
    }
    if (locate_getcc(&es, null) != 0)  {
      return  0;
    }
  }
  return 0;
}

// XXX:repeat 
int do_alter_replace ( LIST_TYPE2_ (alter_) alter_list, int line, struct uchar *ana, struct uchar *err)
{
  struct locate_desc_ ld[2];
  struct locate_essen_ es;
  struct locate_s_ ss;
  int fvail = 0;

  ld[1].etype = LOCATE_TYPE_VARIABLE;
  LOCATE_SETACC_SYMBOLS_TIGHTLY_CONST (& ld[0], _GR("$"), 0, 3);
  locate_attach_weak(& es, ana, null, -1);

  if (locate_clearrc (& es) == 0) 
    return 0;
  else  ;

  while (ktrue)  {
    if (locate_searchp2 (& es, & ld[0], 2, & ss, & fvail) == 0)  {
      struct uchar *emap = null;
      if (alter_find (alter_list, & es.buf->str[ld[1].ac_pos], ld[1].ac_lens, & emap ) != 0)  {
        ld[1].acceptORacc[ld[1].ac_lens] = 0;
        uchar_assign3 (err, _GR ("analysis-error:[%d?%ls] !undef alter \r\n"), line, & es.buf[ld[1].ac_pos]);
        return -1;
      }
      if (ld[0].sig_match == 2)  {
        uchar_insert(emap, 0, _GR ("`"), wcslen (_GR ("`")));
        uchar_insert(emap, -1, _GR ("`"), wcslen (_GR ("`")));
      }
      uchar_replace7( es.buf, fvail, es.doing.apos - fvail, emap->str);
      uchar_uninit (& emap);

      es.doing.apos = fvail;
      es.doing.length = es.buf->length - fvail;
      continue  ;
    }
    if (locate_getcc(&es, null) != 0)  {
      /* burning out **/
      return  0;
    }
  }
  return 0;
}

int ae_replace ( struct list_ *chunk_chain, 
                 struct list_ *env2_chain,
                 struct ustream *ust, 
                 struct uchar *bpath,
                 struct uchar *err   )   {
  int id;
  int ig;
  int sig = -1;
  struct list_v *p;
  struct list_v *c;
  struct list_ *alter = null;
  
  ig = chunk_chain->nums;
  p = chunk_chain->set;

  list_init (& alter);
  for (id = 0; id != ig; p = c, id++)  {
    c = p->level_n;
    if (p->ccp_o->ect_type == CHUNK_TYPE_ALTER)  {
      int ij;
      for (ij = p->ccp_o->sstart; p->ccp_o->len > 0; ij++, p->ccp_o->len--)  {
        if (ust->line_gp[ij-1]->invalid != kfalse)
          continue ;
        if (do_alter (alter, env2_chain, ij, ust->line_gp[ij-1]->content, err) != 0)
          achieve cleanup;
        else { ust->line_gp[ij-1]->invalid = ktrue; }
      }
    } else if (p->ccp_o->ect_type == CHUNK_TYPE_ENVIR) {
      int ij;
      for (ij = p->ccp_o->sstart; p->ccp_o->len > 0; ij++, p->ccp_o->len--)  {
        if (ust->line_gp[ij-1]->invalid != kfalse)
          continue ;
        if (do_envs2 (alter, env2_chain, ij, ust->line_gp[ij-1]->content, bpath, err) != 0)
          achieve cleanup;
        else { ust->line_gp[ij-1]->invalid = ktrue; }
      }
    }
  }
  for (id = 0; id != ust->total; id++)  {
    if (ust->line_gp[id]->invalid != kfalse)
      continue ;
    if (do_env2_replace (env2_chain, id + 1, ust->line_gp[id]->content, err)!= 0)
      achieve cleanup;
    if (do_alter_replace (alter, id + 1, ust->line_gp[id]->content, err)!= 0)
      achieve cleanup;
  }
  sig  = 0;
cleanup:
  list_uninit_ISQ (& alter, alter_uninit );
  return sig;
}